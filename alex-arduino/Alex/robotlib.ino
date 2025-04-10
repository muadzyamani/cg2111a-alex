#include <AFMotor.h>

// Direction values

// Motor control
#define FRONT_LEFT  4  // M4 on the driver shield
#define FRONT_RIGHT 3 // M1 on the driver shield
#define BACK_LEFT   1   // M3 on the driver shield
#define BACK_RIGHT  2  // M2 on the driver shield

AF_DCMotor motorFL(FRONT_LEFT);
AF_DCMotor motorFR(FRONT_RIGHT);
AF_DCMotor motorBL(BACK_LEFT);
AF_DCMotor motorBR(BACK_RIGHT);

#define MOVING_SPEED 80
#define TURNING_SPEED 100

volatile int TURNING_TIME = 400;
volatile int MOVING_TIME = 400;

void setTurningTime(int turning_time) {
  TURNING_TIME = turning_time;
}

void setMovingTime(int moving_time) {
  MOVING_TIME = moving_time;
}

void move(float speed, int direction)
{
  int speed_scaled = (speed / 100.0) * 255;

  motorFL.setSpeed(speed_scaled);
  motorFR.setSpeed(speed_scaled);
  motorBL.setSpeed(speed_scaled);
  motorBR.setSpeed(speed_scaled);

  switch (direction)
  {
  case BACKWARD:
    motorFL.run(BACKWARD);
    motorFR.run(BACKWARD);
    motorBL.run(BACKWARD);
    motorBR.run(BACKWARD);
    break;
  case FORWARD:
    motorFL.run(FORWARD);
    motorFR.run(FORWARD);
    motorBL.run(FORWARD);
    motorBR.run(FORWARD);
    break;
  case LEFT:
    motorFL.run(BACKWARD);
    motorFR.run(FORWARD);
    motorBL.run(BACKWARD);
    motorBR.run(FORWARD);
    break;
  case RIGHT:
    motorFL.run(FORWARD);
    motorFR.run(BACKWARD);
    motorBL.run(FORWARD);
    motorBR.run(BACKWARD);
    break;
  case STOP:
  default:
    motorFL.run(STOP);
    motorFR.run(STOP);
    motorBL.run(STOP);
    motorBR.run(STOP); 
    break;
  }
}

void move_forward()
{
  int start_time = millis();
  int current_time = millis();

  while (current_time - start_time < MOVING_TIME) 
  {
    move(MOVING_SPEED, FORWARD);
    current_time = millis();
  }
  stop();
}

void move_backward()
{  
  int start_time = millis();
  int current_time = millis();
  while (current_time - start_time < MOVING_TIME) 
  {
    move(MOVING_SPEED, BACKWARD);
    current_time = millis();
  }
  stop();
}

void turn_left()
{  
  int start_time = millis();
  int current_time = millis();
  while (current_time - start_time < TURNING_TIME) 
  {
    move(TURNING_SPEED, LEFT);
    current_time = millis();
  }
  stop();
}

void turn_right()
{   
  int start_time = millis();
  int current_time = millis();
  while (current_time - start_time < TURNING_TIME) 
  {
    move(TURNING_SPEED, RIGHT);
    current_time = millis();
  }
  stop();
}

void stop()
{
  move(0, STOP);
}
