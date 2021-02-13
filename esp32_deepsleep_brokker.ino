#include <WiFi.h>
#include <PubSubClient.h>

#define uS_TO_S_FACTOR 3600000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  1
/* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;
 
const char* ssid = "AudiR8";
const char* password = "sam501550";
const char* mqttServer = "192.168.123.201";
const int mqttPort = 1883;
const char* mqttUser = "prateek";
const char* mqttPassword = "prateek";
const char* clientID = "ksiot";

String temp_str;
char temp[50];

WiFiClient espClient;
PubSubClient client(clientID,1883,espClient);
 
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}
 
void setup() {
 
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect(clientID, mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
      client.publish("test/dht","hello");
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.subscribe("test/dht");

  for(int i=0;i<10; i++)
  {
   client.loop();  //Ensure we've sent & received everything
   delay(100);
  }
  
  Serial.print("Going to Sleep...");
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();  
}
 
void loop() {
 
}
