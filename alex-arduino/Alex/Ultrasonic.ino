#include <avr/io.h>
#include "Arduino.h"
#include <util/delay.h>

#define TRIG_DDR  DDRL
#define TRIG_PORT PORTL
#define TRIG_PIN  PL1     // D48 = ICP5 

#define ECHO_DDR  DDRL
#define ECHO_PIN  PL0     // D49 = ICP4

volatile uint16_t startTime = 0;
volatile uint16_t endTime = 0;
volatile bool risingEdge = true;
volatile bool distanceReady = false;

void setUpUltrasonic(){
    sei();
    // Setup TRIG pin (D48) as output
    TRIG_DDR |= (1 << TRIG_PIN);
    TRIG_PORT &= ~(1 << TRIG_PIN);

    // Setup ECHO pin (D49 / ICP4) as input
    ECHO_DDR &= ~(1 << ECHO_PIN);

    // Timer4 - Input Capture Mode
    // TCCR4A = 0; // Normal mode
    // TCCR4B = (1 << ICES4) | (1 << CS41); // Rising edge, prescaler = 8
    // TIMSK4 = (1 << ICIE4); // Enable Input Capture interrupt
}

void getDistance() {
  // Reset state
  distanceReady = false;
  risingEdge = true;
  // TCCR4B |= (1 << ICES4); // Set to detect rising edge

  // Trigger 10µs pulse on TRIG (D8)
  TRIG_PORT &= ~(1 << TRIG_PIN);
  _delay_us(2);
  TRIG_PORT |= (1 << TRIG_PIN);
  _delay_us(10);
  TRIG_PORT &= ~(1 << TRIG_PIN);

  // Wait for echo to be processed in ISR
  while (!distanceReady);

  uint16_t pulseTicks = endTime - startTime;
  float distance_cm = pulseTicks * 0.008575; // 0.5µs tick → cm
  TIMSK4 = 0;

  char distanceMsg[7];
  dtostrf(distance_cm, 3, 2, distanceMsg);
  sendMessage("Distance: ");
  sendMessage(distanceMsg);
  sendNewline();
}

// ISR for Timer4 Input Capture
// ISR(TIMER4_CAPT_vect) {
//   if (risingEdge) {
//     startTime = ICR4;
//     TCCR4B &= ~(1 << ICES4); // Switch to falling edge
//     risingEdge = false;
//   } else {
//     endTime = ICR4;
//     distanceReady = true;
//     TCCR4B |= (1 << ICES4); // Switch back to rising edge
//     risingEdge = true;
//   }
// }
