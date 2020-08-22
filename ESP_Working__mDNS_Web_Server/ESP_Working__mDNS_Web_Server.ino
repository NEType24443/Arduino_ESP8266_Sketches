/*
  ESP8266 mDNS responder sample

  This is an example of an HTTP server that is accessible
  via http://esp8266.local URL thanks to mDNS responder.

  Instructions:
  - Update WiFi SSID and password as necessary.
  - Flash the sketch to the ESP8266 board
  - Install host software:
    - For Linux, install Avahi (http://avahi.org/).
    - For Windows, install Bonjour (http://www.apple.com/support/bonjour/).
    - For Mac OSX and iOS support is built in through Bonjour already.
  - Point your browser to http://esp8266.local, you should see a response.

*/


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>

#ifndef STASSID
#define STASSID "D-Link"
#define STAPSK  "tp181100"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);

void setup(void) {
  Serial.begin(115200);

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin("esp8266")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  // Start TCP (HTTP) server
  server.begin();
  Serial.println("TCP server started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
}

void loop(void) {

  MDNS.update();

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println("");
  //Serial.println("New client"); <--ORIGINALLY HERE-----\
                                                      // |
  // Wait for data from client to become available       |
  while (client.connected() && !client.available()) { // |
    delay(1);                                         // |
  }                                                   // |
                                                      // |
  Serial.println("New client");//THIS WAS --->--->--->---/
  
  // Read the first line of HTTP request
  String req = client.readStringUntil('\r');

  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
    Serial.print("Invalid request: ");
    Serial.println(req);
    return;
  }
  req = req.substring(addr_start + 1, addr_end);
  Serial.print("Request: ");
  Serial.println(req);
  client.flush();

  String s;
  if (req == "/home") 
  {
    IPAddress ip = WiFi.localIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    s = "HTTP/1.1 200 OK\r\n  Content-Type: text/html\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n<body>\r\n<h1> Welcome to Neil's ESP8266 </h1>\r\n";
    s += "<h2> Links to subpages </h2>\r\n<p> 1) <a href='http://esp8266.local/sites'>My Sites</a></p>\r\n";
    s += "<p> 2) <a href='http://esp8266.local/games'>Games List</a></p>\r\n\r\n\r\n\r\n<p>The local IP address of ESP is ";
    s += ipStr;
    s += "</p>\r\n</body>\r\n</html>\r\n\r\n";
    Serial.println("OK");
  } 
  else
  {  
    if(req == "/sites")
    {
      s = "HTTP/1.1 200 OK\r\n  Content-Type: text/html\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n<body>\r\n<h1> Websites I go to </h1>\r\n";
      s += "<h2> Google </h2>\r\n<p> 1) <a href='https://www.google.com/'>Google</a> </p>\r\n<h2> Youtube </h2>\r\n";
      s += "<p> 1) <a href='https://www.youtube.com/'>Home</a> </p>\r\n";
      s += "<p> 2) <a href='https://www.youtube.com/feed/subscriptions'>Subscriptions</a> </p>";
      s += "\r\n\r\n\r\n\r\n\r\n <a href= 'http://esp8266.local/home'>Home</a></body>\r\n</html>\r\n";
      
      Serial.println("OK");
    }
    else
    {
      if(req == "/games")
      {
        s = "HTTP/1.1 200 OK\r\n  Content-Type: text/html\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n<body>\r\n<h1> My Games List </h1>\r\n";
        s += "<h2> PC Games </h2>\r\n<p> 1) Watch Dogs 1 & 2 <br> 2) Alien Isolation <br> 3) Minecraft Java <br> 4) Counter-Strike Global ";
        s += "Offensive <br> 5) The Evil Within 2 <br> 6) Subnautica <br> 7) Just Cause 3 <br> 8) Need For Speed Most Wanted <br> ";
        s += "9) Need For Speed Undercover <br> 10) Need For Speed Carbon <br> 11) Super Hot </p>\r\n<h2> Mobile Games </h2>\r\n";
        s += "<p> 1) Player Unknown's Battle Grounds <br> 2) Motor Hero! <br> 3) Shadow of Death <br> 4) Little Alchemy 2 <br>";
        s += " 5) Alto's Odyssey <br> 6) hocus. <br> 7) Sky Force Reloaded <br> 8) Minecraft Bedrock Edition <br> 9) Rop </p>";
        s += "\r\n\r\n\r\n\r\n\r\n <a href= 'http://esp8266.local/home'>Home</a></body>\r\n</html>\r\n";
        Serial.println("OK");
      }
      else
      {
        s = "HTTP/1.1 404 Not Found \r\n\r\n";
        Serial.println("ERROR 404");
      }
    }
  }
  client.print(s);  //Sending the data to client

  Serial.println("Done with client");
}
