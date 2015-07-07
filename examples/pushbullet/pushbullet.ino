/**
 * \file
 *       ESP8266 Pushbullet example
 * \author
 *       Tuan PM <tuanpm@live.com>
 */

#include <espduino.h>
#include <rest.h>
#include <dht.h>

#define PIN_ENABLE_ESP 13
#define SSID  "YOUR-SSID"
#define PASS  "YOUR-WIFI-PASS"

dht DHT;

ESP esp(&Serial1, &Serial, 6);

REST rest(&esp);

boolean wifiConnected = false;
boolean readyToSend = false;
int pirState = 0;
const char pirStatus[][4] = {"OFF", "ON"};

void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if(res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if(status == STATION_GOT_IP) {
      Serial.println("WIFI CONNECTED");
     
      wifiConnected = true;
    } else {
      wifiConnected = false;
    }
    
  }
}
void pirChange()
{
  pirState = digitalRead(2);
  readyToSend = true;
}

void setup() {
  Serial1.begin(19200);
  Serial.begin(19200);
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while(!esp.ready());

  Serial.println("ARDUINO: setup rest client");
  if(!rest.begin("api.pushbullet.com", 443, true)) {
    Serial.println("ARDUINO: failed to setup rest client");
    while(1);
  }
  rest.setContentType("application/json");
  rest.setHeader("Authorization: Bearer <your_api_key>\r\n");

  /*setup wifi*/
  Serial.println("ARDUINO: setup wifi");
  esp.wifiCb.attach(&wifiCb);

  esp.wifiConnect(SSID, PASS);
  Serial.println("ARDUINO: system started");

  attachInterrupt(0, pirChange, CHANGE);
}

void loop() {
  char data_buf[256];
  esp.process();
  if(wifiConnected && readyToSend) {
      sprintf(data_buf, "{\"type\": \"note\", \"title\": \"PIR Motion\", \"body\": \"Status = [%s]\"}", pirStatus[pirState]);
      Serial.println(data_buf);
      rest.post("/v2/pushes", (const char*)data_buf);
      Serial.println("ARDUINO: send post");

      if(rest.getResponse(data_buf, 256) == HTTP_STATUS_OK){
        Serial.println("ARDUINO: POST successful");

    
      } else {
        Serial.println("ARDUINO: POST error");
      }
      delay(2000);
      readyToSend = false;
      
    } 
}
