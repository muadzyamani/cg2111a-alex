#include <stdarg.h>
#include <math.h>


// Colour sensor initialisation
byte s0 = (1 << 3), s1 = (1 << 4), s2 = (1 << 5), s3 = (1 << 6);
byte out = (1 << 0);
int flag = 1;
int redFreq, greenFreq, blueFreq;

struct colourPacket
{
  uint16_t red;
  uint16_t green;
  uint16_t blue;
} colourData;

void setupColourSensor() {
  PORTC |= s0; // set output frequency scaling to 100% (s0 and s1 to HIGH)
  PORTC &= ~s1;
  DDRC |= (s0 | s1 | s2 | s3); // Set s0,s1,s2,s3 to OUTPUT
  DDRD &= ~out; // Set to INPUT
}

void getColour() {
  PORTC &= ~(s2 | s3);
  redFreq = pulseIn(21, LOW);
  colourData.red = map(redFreq, 58, 115, 255, 0);
  // colourData.red = redFreq;

  _delay_ms(100);

  PORTC |= (s2 | s3);
  greenFreq = pulseIn(21, LOW);
  colourData.green = map(greenFreq, 19, 89, 255, 0);
  // colourData.green = greenFreq;
}

void sendColourData() {
  char redChar[10];
  itoa(colourData.red, redChar, 10);
  char greenChar[10];
  itoa(colourData.green, greenChar, 10);

  sendMessage("Red: ");
  sendMessage(redChar);
  sendNewline();
  sendMessage("Green: ");
  sendMessage(greenChar);
  sendNewline();
  sendMessage("Colour: ");
  if (colourData.green > 200) {
    sendMessage("Green");
  }

  else {
    sendMessage("Red");
  }

  sendNewline();
  sendNewline();
}

void sendColour() {
  setupColourSensor();
  getColour();
  sendColourData();
}
