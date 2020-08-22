/*
 * WebSocketServer.ino
 *
 *  Created on: 22.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;

WebSocketsServer webSocket = WebSocketsServer(81);


#define ML_F D1
#define ML_B D2
#define MR_B D6
#define MR_F D7

bool last_state = false; 

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u][BOT-1] Disconnected!\n", num);
      break;
      
    case WStype_CONNECTED:{
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u][BOT-1] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      // send message to client
			webSocket.sendTXT(num, "Connected");
      break;
    }
      
    case WStype_TEXT:{
      Serial.printf("[%u][BOT-1] got Text: %s\n", num, payload);
      String data = String((char*) payload);     // Explicit Char pointer conversion try (char)
      // send message to client
      webSocket.sendTXT(num, "Recieved: %s", data);
      update_motor(data);
      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
    }
  }
}

void setup() {
  // Serial.begin(921600);
  Serial.begin(115200);
  pinMode(D1, OUTPUT);
  pinMode(ML_F, OUTPUT);
  pinMode(ML_B, OUTPUT);
  pinMode(MR_B, OUTPUT);
  pinMode(MR_F, OUTPUT);

  //Serial.setDebugOutput(true);
  //Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for(uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  
  //WiFiMulti.addAP("SSID", "passpasspass");
  WiFi.begin("SSID", "passpasspass");
  while(WiFiMulti.run() != WL_CONNECTED) {
    Serial.println('.');
    delay(100);
  }
  
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  digitalWrite(D1, HIGH);
  delay(1000);
  digitalWrite(D1, LOW);
  digitalWrite(ML_F, LOW);
  digitalWrite(ML_B, LOW);
  digitalWrite(MR_B, LOW);
  digitalWrite(MR_F, LOW);
}

void update_motor(String text){
    if (text == "1-ON") {
      digitalWrite(D1, HIGH);
    }
    else if (text == "1-OFF") {
      digitalWrite(D1, LOW);
    }
}  

void loop() {
  webSocket.loop();
}
