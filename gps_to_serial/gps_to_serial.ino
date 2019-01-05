#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 12, TXPin = 13;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);
}

void loop()
{
  Serial.println("----------------------");
  Serial.print("Latitude  : ");
  Serial.println(gps.location.lat(), 5);
  Serial.print("Longitude : ");
  Serial.println(gps.location.lng(), 4);
  Serial.print("Satellites: ");
  Serial.println(gps.satellites.value());
  Serial.print("Elevation : ");
  Serial.print(gps.altitude.meters());
  Serial.println("m");
  Serial.print("Time UTC  : ");
  Serial.print(gps.time.hour());
  Serial.print(":");
  Serial.print(gps.time.minute());
  Serial.print(":");
  Serial.println(gps.time.second());
  Serial.print("Heading   : ");
  Serial.println(gps.course.deg());
  Serial.print("Speed     : ");
  Serial.print(gps.speed.kmph());
  Serial.println("km/h");

  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < 500);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received."));
}
