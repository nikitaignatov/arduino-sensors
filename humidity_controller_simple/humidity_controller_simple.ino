#include "DHTesp.h"

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

#define RELAY 16
#define DHT 16

int humidity_threshold = 50;
int sleep_seconds = 30;
int hysteresis = 5;

DHTesp dht;

void setup()
{
  dht.setup(DHT, DHTesp::DHT11);
  pinMode(RELAY, OUTPUT);
}

void loop()
{
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  bool turn_off = humidity < humidity_threshold;
  bool turn_on = humidity > humidity_threshold + hysteresis;

  if (turn_off) {
    digitalWrite(RELAY, LOW);
  }
  else if (turn_on) {
    digitalWrite(RELAY, HIGH);
  }
  delay(sleep_seconds * 1000);
}
