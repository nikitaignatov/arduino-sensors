#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHTesp.h"

const char* ssid =  "SSID";
const char* pw = "PASSWORD";

#define DHT 2
#define RELAY 16

unsigned long run_time = 10 * 60 * 1000;
unsigned long timer;
long force_run = 0 - run_time;

int humidity_threshold = 50; // percent
int hysteresis = 5; // percent humidity
int probe_interval = 3; // seconds

float humidity = 0;
float temperature = 0;

DHTesp dht;
ESP8266WebServer server (80);

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Wifi connecting");

  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pw);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".") ;
  }

  Serial.println ("connected.");
  Serial.print ("IP: ");

  Serial.println (WiFi.localIP());

  dht.setup(DHT, DHTesp::DHT11);

  server.on("/", handleRoot);
  server.on("/on", []() {
    force_run = millis();
    digitalWrite(RELAY, HIGH);
    redirect("/");
  });

  server.on("/off", []() {
    force_run = 0 - run_time;
    digitalWrite(RELAY, LOW);
    redirect("/");
  });

  server.begin();
}

void loop()
{
  changeState();
  server.handleClient();
}

void changeState()
{
  long m2 = millis();
  if (timer + probe_interval * 1000 > m2) {
    return;
  }

  timer = m2;
  humidity = dht.getHumidity();
  temperature = dht.getTemperature();

  bool turn_off = humidity < humidity_threshold;
  bool turn_on = humidity > humidity_threshold + hysteresis;

  Serial.print ("T: ");
  Serial.print (temperature);
  Serial.print (", H: ");
  Serial.println (humidity);
  
  if (force_run + run_time > m2) {
    digitalWrite(RELAY, HIGH);
  }
  else if (turn_off) {
    digitalWrite(RELAY, LOW);
  }
  else if (turn_on) {
    digitalWrite(RELAY, HIGH);
  }
}

void handleRoot() {
  String header = "";
  header += "<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">";
  header += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/4.1.3/css/bootstrap.css\" />";
  header += "</head><body><div class=\"container\">";
  String message = "";
  long m2 = millis();
  bool is_on = force_run + run_time > m2;
  if (is_on) {
    message = " FORCE RUN: " + String( (run_time - (m2 - force_run )) / 60 * 1000 ) + " minutes left";
  }
  server.send(200, "text/html", String(header) + "<h1>TEMP: " + String(temperature) + ", HUMIDITY: " + String(humidity) + message + "<hr /><a class=\"btn btn-light btn-lg\" href=\"/on\">ON</a> | <a class=\"btn btn-light btn-lg\" href=\"/off\">OFF</a> ");
}

void redirect(String location) {
  server.sendHeader("Location", location, true);
  server.send ( 302, "text/plain", "");
}
