#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHTesp.h>

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
  connectToWiFi();
  dht.setup(DHT, DHTesp::DHT11);
  startServer();
}

void loop()
{
  changeState();
  server.handleClient();
}

void connectToWiFi() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Wifi connecting");

  pinMode(RELAY, OUTPUT);
  pinMode(DHT, INPUT);
  digitalWrite(RELAY, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pw);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.print(" Connected with IP: ");
  Serial.println (WiFi.localIP());
}

void startServer() {
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

void changeState()
{
  long current_timer = millis();
  if (timer + probe_interval * 1000 > current_timer) {
    return;
  }

  timer = current_timer;
  humidity = dht.getHumidity();
  temperature = dht.getTemperature();

  bool turn_off = humidity < humidity_threshold;
  bool turn_on = humidity > humidity_threshold + hysteresis;

  if (force_run + run_time > current_timer) {
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
  long current_timer = millis();
  bool is_on = force_run + run_time > current_timer;
  if (is_on) {
    message = " FORCE RUN: " + String( (run_time - (current_timer - force_run )) / 60 * 1000 ) + " minutes left";
  }
  message =
    String(header)
    + "<h1>TEMP: "
    + String(temperature)
    + ", HUMIDITY: "
    + String(humidity)
    + message
    + "<hr /><a class=\"btn btn-light btn-lg\" href=\"/on\">ON</a> | <a class=\"btn btn-light btn-lg\" href=\"/off\">OFF</a> ";
  server.send(200, "text/html", );
}

void redirect(String location) {
  server.sendHeader("Location", location, true);
  server.send ( 302, "text/plain", "");
}
