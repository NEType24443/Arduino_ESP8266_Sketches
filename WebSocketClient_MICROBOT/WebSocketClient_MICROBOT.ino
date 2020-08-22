/*
 * WebSocketClient.ino
 *
 *  Created on: 24.05.2015
 *
 */
//BOT-1

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>

#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define ML_F D1
#define ML_B D2
#define MR_B D6
#define MR_F D7

String text =  "";
bool last_state = false; 

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("[BOT-1] Disconnected!\n");
			break;
      
		case WStype_CONNECTED:
			Serial.printf("[BOT-1] Connected to url: %s\n", payload);
			// send message to server when Connected
			//webSocket.sendTXT("Connected BOT-1");
			break;
      
		case WStype_TEXT:
			Serial.printf("[BOT-1] get text: %s\n", payload);
      text = String((char*) payload);     // 3 MOTHER FUCKING DAYS FOR THIS ONE LINE... ONE FUCKING LINE!!!!! // Explicit Char pointer conversion try (char)
			//webSocket.sendTXT("Acknowledged");
			break;
		case WStype_BIN:
			break;
    case WStype_PING:
      // pong will be send automatically
      Serial.printf("[BOT-1] get ping\n");
      break;
    case WStype_PONG:
      // answer to a ping we send
      Serial.printf("[BOT-1] get pong\n");
      break;
  }
}

void setup() {
	// Serial.begin(921600);
  pinMode(D1, OUTPUT);
  pinMode(ML_F, OUTPUT);
  pinMode(ML_B, OUTPUT);
  pinMode(MR_B, OUTPUT);
  pinMode(MR_F, OUTPUT);
	Serial.begin(115200);

	//Serial.setDebugOutput(true);
	//Serial.setDebugOutput(true);

	Serial.println();
	Serial.println();
	Serial.println();

	for(uint8_t t = 4; t > 0; t--) {
		Serial.printf("[BOT-1] BOOT WAIT %d...\n", t);
		Serial.flush();
		delay(1000);
	}

	WiFiMulti.addAP("Neil", "NeIlNoR18");       // CHANGE THIS

	//WiFi.disconnect();
	while(WiFiMulti.run() != WL_CONNECTED) {
    Serial.print('.');
    digitalWrite(LED_BUILTIN, HIGH);
		delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
	}

	// server address, port and URL
	webSocket.begin("192.168.43.9", 81, "/");       // CHANGE SERVER IP

	// event handler
	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	//webSocket.setAuthorization("user", "Password");

	// try every 5000ms again if connection has failed
	webSocket.setReconnectInterval(5000);
 
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  //webSocket.enableHeartbeat(50, 40, 1);
  digitalWrite(D1, HIGH);
  delay(1000);
  digitalWrite(D1, LOW);
  digitalWrite(ML_F, LOW);
  digitalWrite(ML_B, LOW);
  digitalWrite(MR_B, LOW);
  digitalWrite(MR_F, LOW);
}

void loop() {
	webSocket.loop();
  //Serial.println(data); DEATH INCARNATE DO NOT UNCOMMENT
  if (text == "1:ON" && last_state == false) {
    Serial.println("LED__ON");
    digitalWrite(D1, HIGH);
    last_state = true;
  }
  else if (text == "1:OFF" && last_state == true) {
    Serial.println("LED__OFF");
    digitalWrite(D1, LOW);
    last_state = false;
  }
//  else {
//    webSocket.sendTXT("Illegal Input");
//  }
}
