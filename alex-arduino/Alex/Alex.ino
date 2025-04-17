#include <serialize.h>
#include <stdarg.h>
#include <math.h>
#include "packet.h"
#include "constants.h"

#define DEFAULT_MOTOR_SPEED 80

/*
 * 
 * Alex Communication Routines.
 * 
 */
 
TResult readPacket(TPacket *packet)
{
    // Reads in data from the serial port and
    // deserializes it.Returns deserialized
    // data in "packet".
    
    char buffer[PACKET_SIZE];
    int len;

    len = readSerial(buffer);

    if(len == 0)
      return PACKET_INCOMPLETE;
    else
      return deserialize(buffer, len, packet);
}

void sendMessage(const char *message)
{
  // Sends text messages back to the Pi. Useful
  // for debugging.
  
  TPacket messagePacket;
  messagePacket.packetType = PACKET_TYPE_MESSAGE;
  strncpy(messagePacket.data, message, MAX_STR_LEN);
  sendResponse(&messagePacket);
}

void sendNewline()
{
  const char *empty = "";
  TPacket messagePacket;
  messagePacket.packetType = PACKET_TYPE_NEWLINE;
  strncpy(messagePacket.data, empty, MAX_STR_LEN);


  sendResponse(&messagePacket);
}

// use as printf statement for debugging
// EG: dbprintf("PI is %3.2f \n", PI);
void dbprintf(char *format, ...) {
  va_list args;
  char buffer[128];

  va_start(args, format);
  vsprintf(buffer, format, args);
  sendMessage(buffer);
}

void sendBadPacket()
{
  // Tell the Pi that it sent us a packet with a bad
  // magic number.
  
  TPacket badPacket;
  badPacket.packetType = PACKET_TYPE_ERROR;
  badPacket.command = RESP_BAD_PACKET;
  sendResponse(&badPacket);
  
}

void sendBadChecksum()
{
  // Tell the Pi that it sent us a packet with a bad
  // checksum.
  
  TPacket badChecksum;
  badChecksum.packetType = PACKET_TYPE_ERROR;
  badChecksum.command = RESP_BAD_CHECKSUM;
  sendResponse(&badChecksum);  
}

void sendBadCommand()
{
  // Tell the Pi that we don't understand its
  // command sent to us.
  
  TPacket badCommand;
  badCommand.packetType=PACKET_TYPE_ERROR;
  badCommand.command=RESP_BAD_COMMAND;
  sendResponse(&badCommand);
}

void sendBadResponse()
{
  TPacket badResponse;
  badResponse.packetType = PACKET_TYPE_ERROR;
  badResponse.command = RESP_BAD_RESPONSE;
  sendResponse(&badResponse);
}

void sendOK()
{
  TPacket okPacket;
  okPacket.packetType = PACKET_TYPE_RESPONSE;
  okPacket.command = RESP_OK;
  sendResponse(&okPacket);  
}

void sendResponse(TPacket *packet)
{
  // Takes a packet, serializes it then sends it out
  // over the serial port.
  char buffer[PACKET_SIZE];
  int len;

  len = serialize(buffer, packet, sizeof(TPacket));
  writeSerial(buffer, len);
}


/*
 * Setup and start codes for external interrupts and 
 * pullup resistors.
 * 
 */
// Enable pull up resistors on pins 18 and 19
// void enablePullups()
// {
//   // Use bare-metal to enable the pull-up resistors on pins
//   // 19 and 18. These are pins PD2 and PD3 respectively.
//   // We set bits 2 and 3 in DDRD to 0 to make them inputs. 
//   DDRD &= 0b11110011;
//   PORTD |= 0b00001100;
// }

/*
 * Setup and start codes for serial communications
 * 
 */
 
// Set up the serial connection. For now we are using 
// Arduino Wiring, you will replace this later
// with bare-metal code.
void setupSerial()
{
  // To replace later with bare-metal.
  Serial.begin(9600);
  // Change Serial to Serial2/Serial3/Serial4 in later labs when using the other UARTs
}

// Start the serial connection. For now we are using
// Arduino wiring and this function is empty. We will
// replace this later with bare-metal code.

void startSerial()
{
  // Empty for now. To be replaced with bare-metal code
  // later on.
  
}

// Read the serial port. Returns the read character in
// ch if available. Also returns TRUE if ch is valid. 
// This will be replaced later with bare-metal code.

int readSerial(char *buffer)
{

  int count=0;

  // Change Serial to Serial2/Serial3/Serial4 in later labs when using other UARTs

  while(Serial.available())
    buffer[count++] = Serial.read();

  return count;
}

// Write to the serial port. Replaced later with
// bare-metal code

void writeSerial(const char *buffer, int len)
{
  Serial.write(buffer, len);
  // Serial.flush();

  // Change Serial to Serial2/Serial3/Serial4 in later labs when using other UARTs
}

/*
 * Alex's setup and run codes
 * 
 */

void handleCommand(TPacket *command)
{
  switch(command->command)
  {
    case COMMAND_FORWARD:
        sendOK();
        move_forward();
      break;
    case COMMAND_REVERSE:
        sendOK();
        move_backward();
      break;
    case COMMAND_TURN_LEFT:
        sendOK();
        turn_left();
      break;
    case COMMAND_TURN_RIGHT:
        sendOK();
        turn_right();
      break;
    case COMMAND_STOP: // Assumes 'p' maps to this
        sendOK();
        stop(); // This should call move(0, STOP)
      break;
    case COMMAND_OPEN_CLAW:
        sendOK();
        openClaw();
      break;
    case COMMAND_CLOSE_CLAW:
        sendOK();
        closeClaw();
      break;
    case COMMAND_COLOUR_SENSOR:
        sendOK();
        sendColour();
      break;
    case COMMAND_OPEN_DISPENSER:
        sendOK();
        dispensing();
      break;
    case COMMAND_CLOSE_DISPENSER:
        sendOK();
        stop_dispensing();
      break;
    case COMMAND_SET_TURNINGTIME:
        sendOK();
        setTurningTime(command->params[0]);
      break;
    case COMMAND_SET_MOVINGTIME:
        sendOK();
        setMovingTime(command->params[0]);
      break;
    case COMMAND_ULTRASONIC_SENSOR:
        sendOK();
        setUpUltrasonic();
        getDistance();
      break;
    default:
      sendBadCommand();
  }
}

void waitForHello()
{
  int exit=0;

  while(!exit)
  {
    TPacket hello;
    TResult result;
    
    do
    {
      result = readPacket(&hello);
    } while (result == PACKET_INCOMPLETE);

    if(result == PACKET_OK)
    {
      if(hello.packetType == PACKET_TYPE_HELLO)
      {
     

        sendOK();
        exit=1;
      }
      else
        sendBadResponse();
    }
    else
      if(result == PACKET_BAD)
      {
        sendBadPacket();
      }
      else
        if(result == PACKET_CHECKSUM_BAD)
          sendBadChecksum();
  } // !exit
}

void setup() {
  // put your setup code here, to run once:
  cli();
  setupSerial();
  startSerial();
  //setupColourSensor();
  setUpServo();
  sei();

  waitForHello();
}

void handlePacket(TPacket *packet)
{
  switch(packet->packetType)
  {
    case PACKET_TYPE_COMMAND:
      handleCommand(packet);
      break;

    case PACKET_TYPE_RESPONSE:
      break;

    case PACKET_TYPE_ERROR:
      break;

    case PACKET_TYPE_MESSAGE:
      break;

    case PACKET_TYPE_HELLO:
      break;
  }
}
void resetPacket(TPacket *packet) {
  memset(packet, 0, sizeof(TPacket));
}

void loop() {
// Uncomment the code below for Step 2 of Activity 3 in Week 8 Studio 2

//  forward(0,50);
//  Serial.print("0");

// Uncomment the code below for Week 9 Studio 2


// put your main code here, to run repeatedly:
TPacket recvPacket; // This holds commands from the Pi

TResult result = readPacket(&recvPacket);

if(result == PACKET_OK)
  handlePacket(&recvPacket);
else if(result == PACKET_BAD)
  {
    sendBadPacket();
    resetPacket(&recvPacket);
  }
else if(result == PACKET_CHECKSUM_BAD)
  {
    sendBadChecksum();
    resetPacket(&recvPacket);
  }  
}
