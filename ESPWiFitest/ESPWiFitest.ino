#include <ESP8266WiFi.h>
void setup() {
  const char* ssid = "D-Link";
  const char* password = "tp181100";
  
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');

  WiFi.begin(ssid, password);   //Connect to WiFi
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");
  int i = 0;
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.print("Connected to:\t"); Serial.print(ssid);
  Serial.println('\n');
  Serial.print("IP address:\t");
  Serial.print(WiFi.localIP());  // Send the IP address of the ESP8266 to the computer
}
void loop(){}
