#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CLK_PIN   5
#define DATA_PIN  7
#define CS_PIN    6

#define PAUSE_TIME    500
#define SCROLL_SPEED  20

String message = "RELEASING, DO NOT DISTURB";

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void setup(void)
{
  Serial.begin(9600);
  P.begin();
  P.setIntensity(13);
}

void loop(void)
{
  while (Serial.available() > 0){
    message = Serial.readStringUntil ('\n');
  }
  if (P.displayAnimate())
  {
    char ar[]={};
    message.toCharArray(ar, message.length()+1 );
    P.displayScroll(ar, PA_RIGHT, PA_SCROLL_LEFT , SCROLL_SPEED);
  }
}
