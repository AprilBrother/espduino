espduino
========
This is MQTT client for arduino connect to broker via ESP8266 AT command, port from [MQTT client library for Contiki](https://github.com/esar/contiki-mqtt)

You can find the Native MQTT client library for ESP8266 work well here: https://github.com/tuanpmt/esp_mqtt


Features
========


- Support subscribing, publishing, authentication, will messages, keep alive pings and all 3 QoS levels (it should be a fully functional client).
- Easy to setup and use


Status
========
**NOT Working**


Usage
=======

```c
#include "esp8266.h"
#include <SoftwareSerial.h>

SoftwareSerial debugPort(2, 3); // RX, TX

ESP esp(&Serial, &debugPort, 13);


void wifiCb(uint8_t status)
{
  debugPort.println("WIFI: Connected");
  
  esp.mqttConnect("mqtt.domain.io", 1880);
}

void mqttConnected(uint32_t* args)
{
  
  
  debugPort.println("MQTT:Connected");
  esp.subscribe("/topic");
}

void mqttDisconnected(uint32_t* args)
{
  debugPort.println("MQTT:Disconnected");
}

void mqttPublished(uint32_t* args)
{
  debugPort.println("MQTT:Published");
}

void mqttData(uint32_t* args)
{
  
  debugPort.println("MQTT:data");
  
}

const char clientId[] = "clientId";
const char user[] = "user";
const char pass[] = "pass";


void setup() {
  delay(2000);
  Serial.begin(115200);
  debugPort.begin(9600);
  
  /* setup event */
  esp.wifiCb.attach(&wifiCb);
  esp.mqttConnected.attach(&mqttConnected);
  esp.mqttDisconnected.attach(&mqttDisconnected);
  esp.mqttPublished.attach(&mqttPublished);
  esp.mqttData.attach(&mqttData);
  
  /* Init data */
  esp.initMqttClient(clientId, user, pass, 30);
  
  /* wifi connect */
  esp.wifiConnect("DVES_HOME", "yourpassword");
  
}

void loop() {
  esp.process();
}

```
