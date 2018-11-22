#include <UTFT.h>

extern uint8_t SmallFont[];
UTFT myGLCD(CTE40, 38, 39, 40, 41); // ILI9486

#define CIRCLE_SIZE 26

String circleData[CIRCLE_SIZE] = {
  "open cmd",
  "type the following command \"mode\" ",
  "find the port this device is connected to",
  "type",
  "echo MESSAGE > PORTNAME",
  "For example:",
  "echo Hello World > COM3"
};

uint8_t current = 0;

void setup()
{
  myGLCD.InitLCD();
  myGLCD.clrScr();

  myGLCD.setColor(0, 255, 0);
  myGLCD.setBackColor(0, 0, 0);

  myGLCD.setFont(SmallFont);
  Serial.begin(9600);
  printMessages();
  reset();
}

void loop()
{
  while (Serial.available() > 0) {
    current = current + 1 < CIRCLE_SIZE ? current + 1 : current;
    if (current + 1 == CIRCLE_SIZE)moveup();

    circleData[current] = Serial.readStringUntil ('\n');
    myGLCD.clrScr();
  }
  printMessages();
}

void printMessages() {
  for (int i = 0; i < CIRCLE_SIZE; i++)
  {
    myGLCD.print(String(i) + ". " + circleData[i], LEFT, i * 12);
  }
}

void reset() {
  for (int i = 0; i < CIRCLE_SIZE; i++)
  {
    circleData[i] = "";
  }
}

void moveup() {
  for (int i = 1; i < CIRCLE_SIZE; i++)
  {
    circleData[i - 1] = circleData[i];
  }
}
