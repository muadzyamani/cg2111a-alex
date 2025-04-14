#include <avr/io.h>
#include "Arduino.h"
#include <util/delay.h>

#define TRIG_DDR  DDRL
#define TRIG_PORT PORTL
#define TRIG_PIN  PL1     // D48 = ICP5 

#define ECHO_DDR  DDRL
#define ECHO_PIN  PL0     // D49 = ICP4

// Servo Pins (Timer5)
#define LEFT_SERVO_OCR  OCR5C   // PL5 (D44)
#define RIGHT_SERVO_OCR OCR5B   // PL4 (D45)
#define DISPENSER_SERVO_OCR OCR5A //PL3 (D46)

//right 800 - 2600
//left 2600 - 800
//90 - 0 degree 
#define MIN_PULSE 800
#define MAX_PULSE 2800 
#define PULSE_STEP 200

//90 - 45 degree
// #define MIN_PULSE 800
// #define MAX_PULSE 1700
// #define PULSE_STEP 200

//right 800 - 1700 
//left 2600 - 1700

//void setup() {
//    setUpServo();
//    setUpUltrasonic();
//  //     DDRL |= (1 << PL5) | (1 << PL4) | (1 << PL3); // Left Servo Pin 44, Right Servo Pin 45, Dispenser Servo Pin 46
//  
//  //     TCCR5A = (1 << COM5B1) | (1 << COM5C1) | (1 << COM5A1); // Enable OC5B (Right) and OC5C (Left), OC5A (Dispenser)
//  //     TCCR5B = (1 << WGM53) | (1 << WGM52) | (1 << CS51); // Fast PWM, TOP = ICR5, prescaler 8
//  //     TCCR5A |= (1 << WGM51); // Complete WGM mode 14
//  
//  //     ICR5 = 39999; // 20ms period for standard servo
//  
//  //     // Initial servo positions (open claw)
//  // //    LEFT_SERVO_OCR = MAX_PULSE;
//  // //    RIGHT_SERVO_OCR = MIN_PULSE;
//  
//  //     //close claw initially
//  //     LEFT_SERVO_OCR = MIN_PULSE;
//  //     RIGHT_SERVO_OCR = MAX_PULSE;
//  
//  
//  //     DISPENSER_SERVO_OCR = 600;
//      // // Setup TRIG pin (D48) as output
//      // TRIG_DDR |= (1 << TRIG_PIN);
//      // TRIG_PORT &= ~(1 << TRIG_PIN);
//  
//      // // Setup ECHO pin (D49 / ICP4) as input
//      // ECHO_DDR &= ~(1 << ECHO_PIN);
//  
//      // // Timer4 - Input Capture Mode
//      // TCCR4A = 0; // Normal mode
//      // TCCR4B = (1 << ICES4) | (1 << CS41); // Rising edge, prescaler = 8
//      // TIMSK4 = (1 << ICIE4); // Enable Input Capture interrupt
//      sei(); // Enable global interrupts
//  
//      Serial.begin(9600);
//  }

  void setUpServo(){
    DDRL |= (1 << PL5) | (1 << PL4) | (1 << PL3); // Left Servo Pin 44, Right Servo Pin 45, Dispenser Servo Pin 46

    TCCR5A = (1 << COM5B1) | (1 << COM5C1) | (1 << COM5A1); // Enable OC5B (Right) and OC5C (Left), OC5A (Dispenser)
    TCCR5B = (1 << WGM53) | (1 << WGM52) | (1 << CS51); // Fast PWM, TOP = ICR5, prescaler 8
    TCCR5A |= (1 << WGM51); // Complete WGM mode 14

    ICR5 = 39999; // 20ms period for standard servo
    // Initial servo positions (open claw)
    //    LEFT_SERVO_OCR = MAX_PULSE;
    //    RIGHT_SERVO_OCR = MIN_PULSE;

    //close claw initially
    LEFT_SERVO_OCR = MIN_PULSE;
    RIGHT_SERVO_OCR = MAX_PULSE;


    DISPENSER_SERVO_OCR = 2500;
}

void smoothMove(volatile uint16_t &ocr, uint16_t target, uint8_t step, uint16_t delayTime) {
  if (ocr < target) {
    for (uint16_t val = ocr; val <= target; val += step) {
      ocr = val;
      delay(delayTime);
    }
  } else {
    for (uint16_t val = ocr; val >= target; val -= step) {
      ocr = val;
      delay(delayTime);
      if (val < step) break; // avoid underflow
    }
  }
}

// 0 - 90 degree
void closeClaw(){
//    for (int pulse = MAX_PULSE; pulse >= MIN_PULSE; pulse -= PULSE_STEP) {
//        //left
//        LEFT_SERVO_OCR = pulse;
//        //right
//        RIGHT_SERVO_OCR = (MAX_PULSE - pulse) + MIN_PULSE;
//        delay(200);
//    }
    smoothMove(LEFT_SERVO_OCR, MIN_PULSE, 10, 5); // Mid
    smoothMove(RIGHT_SERVO_OCR, MAX_PULSE, 10, 5);

}

void openClaw(){
//    for (int pulse = MIN_PULSE; pulse <= MAX_PULSE; pulse += PULSE_STEP) {
//      //left
//      LEFT_SERVO_OCR = pulse;
//      //right
//      RIGHT_SERVO_OCR = (MAX_PULSE - pulse) + MIN_PULSE;
//      delay(200);
//    }
    LEFT_SERVO_OCR = MAX_PULSE;
    RIGHT_SERVO_OCR = MIN_PULSE;
}

void dispensing(){
//    for (int pulse = 600; pulse <= 2500; pulse += PULSE_STEP) {
//        DISPENSER_SERVO_OCR = pulse;
//        delay(300);
//    }
    DISPENSER_SERVO_OCR = 600;

}
  
  
void stop_dispensing(){
//    for (int pulse = 2500; pulse >= 600; pulse -= PULSE_STEP) {
//      DISPENSER_SERVO_OCR = pulse;
//      delay(300);
//  }
    DISPENSER_SERVO_OCR = 2500;

}
