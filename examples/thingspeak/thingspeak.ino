/**
 * \file
 *       ESP8266 RESTful Bridge example
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

ESP esp(&Serial1, &Serial, PIN_ENABLE_ESP);

REST rest(&esp);

boolean wifiConnected = false;

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

void setup() {
  Serial1.begin(19200);
  Serial.begin(19200);
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while(!esp.ready());

  Serial.println("ARDUINO: setup rest client");
  if(!rest.begin("api.thingspeak.com")) {
    Serial.println("ARDUINO: failed to setup rest client");
    while(1);
  }

  /*setup wifi*/
  Serial.println("ARDUINO: setup wifi");
  esp.wifiCb.attach(&wifiCb);

  esp.wifiConnect(SSID, PASS);
  Serial.println("ARDUINO: system started");
}

void loop() {
  char response[266];
  esp.process();
  if(wifiConnected) {
    int chk = DHT.read11(7);
    if(chk == DHTLIB_OK){
      char buff[64];
      char str_hum[6], str_temp[6];
      dtostrf(DHT.humidity, 4, 2, str_hum);
      dtostrf(DHT.temperature, 4, 2, str_temp);
      sprintf(buff, "/update?api_key=MAY03AKJDMPP4Y4I&field1=%s&field2=%s", str_hum, str_temp);
      Serial.println(buff);
      rest.get((const char*)buff);
      Serial.println("ARDUINO: send get");

      if(rest.getResponse(response, 266) == HTTP_STATUS_OK){
        Serial.println("ARDUINO: GET successful");
        Serial.println(response);
      }
      delay(30000);
      
    } else {
      Serial.print("error,\r\n"); 
    }
    
    
  }
  
  
}
