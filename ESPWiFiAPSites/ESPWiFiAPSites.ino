#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "ESP8266"
#define APPSK  "nope"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot();

void handleMygames();

void handleMysites();

void handleNotFound();

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.print("Configuring access point...\n");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid,password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/games", handleMygames);
  server.on("/sites", handleMysites);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
} 
void handleRoot() {
  IPAddress ip = WiFi.softAPIP();
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  String s = "<body>\r\n<h1> Welcome to Neil's ESP8266 </h1>\r\n<h2> Links to subpages </h2>\r\n<p> 1) <a href='http://192.168.4.1/sites'>My Sites</a></p>\r\n<p> 2) <a href='http://192.168.4.1/games'>Games List</a></p>\r\n\r\r\r<p>The IP address of server is ";
  s += ipStr;
  s += "</p>\r\n</body>\r\n\r\n"; 
  server.send(200, "text/html", s );
}
void handleMygames() {
  server.send(200, "text/html", "<body>\r\n<h1> My Games List </h1>\r\n<h2> PC Games </h2>\r\n<p> 1) Watch Dogs 1 & 2 <br> 2) Alien Isolation <br> 3) Minecraft Java <br> 4) Counter-Strike Global Offensive <br> 5) The Evil Within 2 <br> 6) Subnautica <br> 7) Just Cause 3 <br> 8) Need For Speed Most Wanted <br> 9) Need For Speed Undercover <br> 10) Need For Speed Carbon <br> 11) Super Hot </p>\r\n<h2> Mobile Games </h2>\r\n<p> 1) Player Unknown's Battle Grounds <br> 2) Motor Hero! <br> 3) Shadow of Death <br> 4) Little Alchemy 2 <br> 5) Alto's Odyssey <br> 6) hocus. <br> 7) Sky Force Reloaded <br> 8) Minecraft Bedrock Edition <br> 9) Rop </p>\r\n\n\n\n\n <a href= 'http://192.168.4.1/'>Home</a></body>\r\n");
}
void handleMysites() {
  server.send(200, "text/html", "<body>\r\n<h1> Websites I go to </h1>\r\n<h2> Google </h2>\r\n<p> 1) <a href='https://www.google.com/'>Google</a> </p>\r\n<h2> Youtube </h2>\r\n<p> 1) <a href='https://www.youtube.com/'>Home</a> </p>\r\n<p> 2) <a href='https://www.youtube.com/feed/subscriptions'>Subscriptions</a> </p>\r\n\n\n\r <a href= 'http://192.168.4.1/'>Home</a></body>\r\n");
}
void handleNotFound() {
  server.send(404, "text/plain", "Error 404 Not Found"); 
}
