extern "C" {
#include "user_interface.h"
}

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>

#define DHT 2
#define RTC_MEM_START 64
#define RTC_MEM_LEN 128

//const char* ssid =  "SSID";
//const char* pw = "PASSWORD";
const char* url = "http://192.168.0.13:1880/api/sensor";
const char* mqttServer = "192.168.0.13";
const int mqttPort = 1883;

typedef struct {
  int temp;
  int humidity;
} rtcStore;

rtcStore rtcMem;

int humidity_threshold = 65; // percent
int hysteresis = 5; // percent humidity
int probe_interval = 60; // seconds

int humidity = 0;
float temperature = 0;
char msg[50];

DHTesp dht;
WiFiClient espClient;
PubSubClient client(espClient);
bool above_threshold = false;

void setup()
{
  system_rtc_mem_read(RTC_MEM_START, &above_threshold, 4);

  pinMode(DHT, INPUT);
  
  client.setServer(mqttServer, mqttPort);
  connectToWiFi();
  connectToMqtt();
  dht.setup(DHT, DHTesp::DHT11);
 // system_rtc_mem_write(RTC_MEM_START, &above_threshold, 4);
 // delay(1);
  
  //ESP.deepSleep(5000000, WAKE_RFCAL);
}

void loop() { 
  if(WiFi.status()== WL_CONNECTED){
    connectToMqtt();
    changeState();
  }else{
    connectToWiFi();
  }
  delay(30 * 1000);
}

void connectToWiFi() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Wifi connecting");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pw);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.print("Connected with IP: ");
  Serial.println (WiFi.localIP());
}

void connectToMqtt(){
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void changeState()
{ 
  humidity = dht.getHumidity();
  temperature = dht.getTemperature();

  bool change_to_false = above_threshold && humidity < humidity_threshold;
  bool change_to_true = !above_threshold && humidity > humidity_threshold + hysteresis;

  dtostrf(temperature , 2, 2, msg);
  client.publish("sensor/temperature/1", msg);
  dtostrf(humidity , 2, 2, msg);
  client.publish("sensor/humidity/1", msg);
  
  if (change_to_false || change_to_true) {
    // send wifi
  }
  else {
    
  }
}
