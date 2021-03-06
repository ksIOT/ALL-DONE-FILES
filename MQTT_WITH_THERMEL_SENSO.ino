

/******************************************************************************

Example Code for controlling a light ON/OFF and getting the data from the temperature sensor using MQTT
by: deepak ,prateek

  Settings: 
  Board -> ESP32 DEv Module 
  Baudrate -> 115200
 
  MQTT Broker --> Mosquitto MQTT on Raspberry Pi 4 
  
//The code works on MQTT Protocol to send/receive data across the cloud.
 
  
//Edit ONLY the "Configuration" part of the code to make any changes to WiFi or MQTT settings.
 
 
 This sketch connects the ESP32  to a MQTT broker and subcribes to the topic
test/dht. 
******************************************************************************/

#include "PubSubClient.h" // Connect and publish to the MQTT broker

// Code for the ESP32
#include "WiFi.h"
#include <Wire.h>
//#include <Adafruit_BME280.h>// NEW EDIT ON THE 12.02.2021

#include <Adafruit_Sensor.h>
*********************************** Configuration *************************************************
//wifi code
const char* ssid = "KS SMART";
const char* password = "godspeed123";
const char* mqtt_port = 1883

//code for mqtt 

const char* mqtt_server = "192.168.50.25";// add the server's IP address
const char* mqtt_username = "prateek"; // MQTT username
const char* mqtt_password = "prateek"; //MQTT password
const char* clientID = "ksiot";// Client ID


//*********************************************************************************************

WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient);// BRIDGE FOR CONNECTING THE  SERVER TO THE ESP32
long lastMsg = 0;
char msg[50];
int value = 0;

//uncomment the following lines if you're using SPI
/*#include <SPI.h>
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/      

//Adafruit_BME280 bme; // I2C// once connected with the sensor(12.02.2021)
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI
float temperature = 200;
float humidity = 100;

// LED Pin 
const int ledPin = 4;
//*********************************************************************************************

void setup() {
  Serial.begin(115200);
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  //status = bme.begin();  
//  if (!bme.begin(0x76)) {
//    Serial.println("Could not find a valid BME280 sensor, check wiring!");
//    while (1);
//  }
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
// MESSAGE CODE ON THE SERVER 
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "test/dht") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}
// AUTO RECONNECT CODE ONCE THE CONNECTION GETS RIPED OFF
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // THE CLIENT .CONNECT BRIDGE WITH THE  CLIENT ID AND THE USER NAME AND THE PASSWORD
    if (client.connect(clientID, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("test/dht");
    } else {
      Serial.print("failed, rc=");// RETURN CODE CONDITION OCCURS WHEN  THECONNECTION IS NOT DONE
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//when the system gets the error it gets reeconnected
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
    // Temperature in Celsius
  //temperature = bme.readTemperature();   //NEW EDIT ON THBE
    // Uncomment the next line to set temperature in Fahrenheit 
    // (and comment the previous temperature line)
    //temperature = 1.8 * bme.readTemperature() + 32; // Temperature in Fahrenheit
    
    // Convert the value to a char array
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    client.publish("test/dht", tempString);
    //humidity = bme.readHumidity();
    
    // Convert the value to a char array
    char humString[8];
    dtostrf(humidity, 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);
    client.publish("test/dht", humString);
  }
}
