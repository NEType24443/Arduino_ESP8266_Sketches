#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <WiFiClient.h>

const char *ssidAP = "NTP_ESP8266_AP"; // The name of the Wi-Fi network that will be created
const char *passwordAP = "bestpasswordever";   // The password required to connect to it, leave blank for an open network

const char* ssid = "D-Link";
const char* password = "tp181100";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');
  WiFi.mode(WIFI_STA);

  WiFi.softAP(ssidAP, passwordAP);             // Start the access point
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started");

  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());         // Send the IP address of the ESP8266 to the computer
}

void loop() { }
