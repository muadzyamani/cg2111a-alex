#include <avr/io.h>
#include "Arduino.h"
#include <util/delay.h>

#define TRIG_DDR  DDRL
#define TRIG_PORT PORTL
#define TRIG_PIN  PL1     // D48 = ICP5 

#define ECHO_DDR  DDRL
#define ECHO_PIN  PL0     // D49 = ICP4

float duration, distance;

void setUpUltrasonic() {
    // Setup TRIG pin (D48) as output
    TRIG_DDR |= (1 << TRIG_PIN);
    TRIG_PORT &= ~(1 << TRIG_PIN);

    // Setup ECHO pin (D49 / ICP4) as input
    ECHO_DDR &= ~(1 << ECHO_PIN);
}

void getDistance() {
  // Trigger 10µs pulse on TRIG (D8)
  TRIG_PORT &= ~(1 << TRIG_PIN); // Ensures that the pin is initially LOW
  _delay_us(2);
  TRIG_PORT |= (1 << TRIG_PIN);
  _delay_us(10);
  TRIG_PORT &= ~(1 << TRIG_PIN);

  duration = pulseIn(49, HIGH); // Time taken for the pulse to bounce back and return to the sensor
  distance = (duration*.0343)/2;

  char distanceMsg[7];
  dtostrf(distance, 3, 2, distanceMsg);
  sendMessage("Distance: ");
  sendMessage(distanceMsg);
  sendNewline();
}
