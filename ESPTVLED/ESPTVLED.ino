#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include<FS.h>

#ifndef STASSID
#define STASSID "Neil"
#define STAPSK  "NeIlNoR18"
#endif

#define PIN    D1  // Digital IO pin connected to the NeoPixels.

#define PIXELS 20  // Number of NeoPixels

const char* ssid = STASSID;
const char* password = STAPSK;
bool notConnected = false;

ESP8266WebServer server(80);

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(PIXELS, PIN, NEO_GRB + NEO_KHZ800);

void handleRoot();

void handleNotFound();

void siteStop();

void siteColorWipe();

void siteTheatreChase();

void siteRainbow();

void siteRainbowTheatre();

void siteLooper();

void siteReflector();

void colorWipe();

void theaterChase();

void rainbow();

void theaterChaseRainbow();

void looper();

void reflector();

void pixelmov();

int hextocolor();

int hextodec();

void setup() {
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  strip.show();  // Initialize all pixels to 'off'

  SPIFFS.begin();
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  int i = 0;
  int j = 15;
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if(i>5) i = 0;
    if(j<10) j = 15;
    strip.setPixelColor(i, strip.Color( 0, 255, 0));
    strip.setPixelColor(j, strip.Color( 0, 255, 0));
    strip.show();
    delay(150);
    strip.setPixelColor(i, strip.Color( 0, 0, 0));
    strip.setPixelColor(j, strip.Color( 0, 0, 0));
    strip.show(); 
    delay(350);
    i++;
    j--;
    count++;
    if(count>30){
      Serial.print("No connection available starting in offline mode...");
      WiFi.mode(WIFI_OFF);
      break;
    }
  }
  i=0;
  j=10;
  int del = 160;
  while(i<=9 && j<=19){
    strip.setPixelColor(i, strip.Color( 0, 255, 0));
    strip.setPixelColor(j, strip.Color( 0, 255, 0));
    strip.show();
    delay(del);
    del-=10;
    i++;
    j++;
  }
  while(i>=0 && j>=10){
    strip.setPixelColor(i, strip.Color( 0, 0, 0));
    strip.setPixelColor(j, strip.Color( 0, 0, 0));
    strip.show();
    delay(del);
    del+=10;
    i--;
    j--;
  }
  if(count>30){
    notConnected = true;
    loop();
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.on("/colourWipe", siteColorWipe);

  server.on("/theatreChase", siteTheatreChase);
  
  server.on("/rainbow", siteRainbow);

  server.on("/rainbowTheatre", siteRainbowTheatre);

  server.on("/looper", siteLooper);

  server.on("/reflector", siteReflector);

  server.on("/stop", siteStop);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  if(notConnected==true) noConnection();
  else server.handleClient();    
}

void noConnection(void){
  rainbow(random(4,50),random(1,5),random(16,256));
} 

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  unsigned long begining;
  for(int i=0; i<strip.numPixels(); i++) {   
      strip.setPixelColor(i, color);    //  Set pixel's color (in RAM)
      strip.show();                 
      begining = millis();
      while(begining+wait>=millis()){
        server.handleClient();
      }
  }
  
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait , uint8_t cycles) {
  unsigned long begining;
  while(cycles>0){  
    for(int a=0; a<10; a++) {  // Repeat 10 times...
      for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
        strip.clear();         //   Set all pixels in RAM to 0 (off)
        // 'c' counts up from 'b' to end of strip in steps of 3...
        for(int c=b; c<strip.numPixels(); c += 3) {
          strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
        }
        strip.show(); // Update strip with new contents
        begining = millis();
        while(begining+wait>=millis()){
          server.handleClient();
        }
      }
    }
    cycles--;
  } 
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait , int cycles, int rate) {
  // Hue of first pixel runs 3 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  unsigned long begining;
  while(cycles>0){
    for(long firstPixelHue = 0; firstPixelHue < 65536; firstPixelHue += rate*4) {
      for(int i=0; i<strip.numPixels(); i++) { 
        // For each pixel in strip...
        // Offset pixel hue by an amount to make one full revolution of the
        // color wheel (range of 65536) along the length of the strip
        // (strip.numPixels() steps):
        int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
        // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
        // optionally add saturation and value (brightness) (each 0 to 255).
        // Here we're using just the single-argument hue variant. The result
        // is passed through strip.gamma32() to provide 'truer' colors
        // before assigning to each pixel:
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
      }
      strip.show(); // Update strip with new contents
      begining = millis();
      while(begining+wait>=millis()){
        server.handleClient();
      }  // Pause for a moment
    }
    cycles--;
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait,int cycles) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  unsigned long begining;
  while(cycles>0){
    for(int a=0; a<30; a++) {  // Repeat 30 times...
      for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
        strip.clear();         //   Set all pixels in RAM to 0 (off)
        // 'c' counts up from 'b' to end of strip in increments of 3...
        for(int c=b; c<strip.numPixels(); c += 3) {
          // hue of pixel 'c' is offset by an amount to make one full
          // revolution of the color wheel (range 65536) along the length
          // of the strip (strip.numPixels() steps):
          int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
          uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
          strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
        }
        strip.show();                // Update strip with new contents
        begining = millis();
        while(begining+wait>=millis()){
          server.handleClient();
        }    // Pause for a moment
        firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
      }
    }
    cycles--;
  }
}

void looper(uint32_t color, uint16_t wait, uint8_t cycles, uint8_t num){
  uint8_t  pixels = strip.numPixels();
  unsigned long begining ;
  strip.clear();
  while(cycles>0){
    for(uint8_t i=0; i<pixels; i++){
      for(int n=num; n>0; n--){
        strip.setPixelColor(i+(n-1)*pixels/num, color);
      }
      strip.show();
      begining = millis();
      while(begining+wait>=millis()){
        server.handleClient();
      }
      for(int n=num; n>0; n--){
        strip.setPixelColor(i+(n-1)*pixels/num, strip.Color( 0, 0, 0));
      }
      strip.show();       
    }
    cycles--;
  }
}

void reflector(uint32_t color, uint32_t cycdur, uint8_t cycles, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4){
  strip.clear();
  if(cycdur<10){
    cycdur=10;  //just in case 
  }
  int pac1[]={0,1}, pac2[]={0,1}, pac3[]={0,1}, pac4[]={0,1};      //packets e.g: pac1[0] represents pixel number, 
                                                             //pac1[1] represents direction of packet 1 is forward 0 is backward  
  unsigned long ini=millis(), ini1=ini, ini2=ini, ini3=ini, ini4=ini, tim;    //initial start time declaration
  //all if() conditions in loop are to check if the duration has passed for each pixel movement
  if((d1>0)||(d2>0)||(d3>0)||(d4>0)){
    while(cycles>0){
      server.handleClient();
      tim = millis();
      if((ini1+d1<=tim) && (d1>0)){
        pixelmov(pac1, color);
        //Serial.print("Called 1");
        ini1=millis();
      }
      if((ini2+d2<=tim) && (d2>0)){
        pixelmov(pac2, color);  
        //Serial.print("Called 2");
        ini2=millis();
      }
      if((ini3+d3<=tim) && (d3>0)){
        pixelmov(pac3, color);
        //Serial.print("Called 3");
        ini3=millis();
      }
      if((ini4+d4<=tim) && (d4>0)){
        pixelmov(pac4, color);
        //Serial.print("Called 4");
        ini4=millis();
      }
      if(ini+cycdur<=tim){
        cycles--;
        //Serial.print("Cycle reduced by 1");
        ini=millis();
      }
      yield();
    }
  }
}

void pixelmov( int* pixel, uint32_t color){
  if(*(pixel+1)==1){                                        //direction check.
    strip.setPixelColor(*pixel-1, strip.Color( 0, 0, 0));   //turn off previous pixel.
    strip.setPixelColor(*pixel, color);                     //turn on pixel.
    strip.show();
    if(*pixel==19){             //to reverse the direction of the pixel movement. 
      *(pixel+1)=0;             //direction reversed.
      *pixel=*pixel-1;                 //pixel 18 will be the next since the direction is reversed 
    }
    else{
      *pixel=*pixel+1;                 //keeps incrementing till it reaches 19 since it is the last pixel.
    }
    //Serial.println(*pixel);
  }
  if(*(pixel+1)==0){                                        //direction check.
    strip.setPixelColor(*pixel+1, strip.Color( 0, 0, 0));   //turn off previous pixel.
                                                            //When first direction change occurs it turn off pixel 19.
    strip.setPixelColor(*pixel, color);                     //turn on pixel.
    strip.show();
    if(*pixel==0) {             //to reverse the direction of the pixel movement.
      *(pixel+1)=1;            //direction reversed.
      *pixel=*pixel+1;                //pixel 1 will be the next pixel since direction is reversed. 
    }
    else{
      *pixel=*pixel-1;                //keeps decrementing till it reaches 0 since it is the first pixel 
    }
    //Serial.println(*pixel);
  }
  yield();
}

int hextocolor(String hexstr)
{
  //Serial.println(hexstr);
  int strlength = hexstr.length();
  for(int i=0; i<strlength; i++){
    hexstr[i]=hexstr[i+1];
  }
  char buff[8], hexr[2], hexg[2], hexb[2];
  hexstr.toCharArray(buff,strlength-3);
  for(int i=0; i<=1; i++){
    hexr[i]=hexstr[i];
    hexg[i]=hexstr[i+2];
    hexb[i]=hexstr[i+4];
  }
  uint8_t r=0, g=0 ,b=0;
  for(int i=0,j=10; i<=1 ;i++){
    if(i)j=1; 
    r = r + (hextodec(hexr,i)*j);
    //Serial.println(r);
    g = g + (hextodec(hexg,i)*j);
    //Serial.println(g);
    b = b + (hextodec(hexb,i)*j);
    //Serial.println(b);
  }
  /*Serial.print("R:");
  Serial.println(r);
  Serial.print("G:");
  Serial.println(g);
  Serial.print("B:");
  Serial.println(b);*/
  uint32_t color = ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
  return(color);
}

int hextodec(char* digit,int i){
  uint8_t decimal=0;
  //Serial.print ("\nDigit: ");
  //Serial.println(*(digit+i));
    if ((*(digit+i)>='0') && (*(digit+i)<='9')){
      decimal=*(digit+i)-48;
    }
    else if ((*(digit+i)=='A')||(*(digit+i)=='a')){
      decimal=10;
      //Serial.print("\na trig");
    }
    else if ((*(digit+i)=='B')||(*(digit+i)=='b')){
      decimal=11;
      //Serial.print("\nb trig");
    }
    else if ((*(digit+i)=='C')||(*(digit+i)=='c')){
      decimal=12;
      //Serial.print("\nc trig");
    }
    else if ((*(digit+i)=='D')||(*(digit+i)=='d')){
      decimal=13;
      //Serial.print("\nd trig");
    }
    else if ((*(digit+i)=='E')||(*(digit+i)=='e')){
      decimal=14;
      //Serial.print("\ne trig");
    }
    else if ((*(digit+i)=='F')||(*(digit+i)=='f')){
      decimal=15;
      //Serial.print("\nf trig");
    }
    //Serial.print("\ndecimal:");
    //Serial.println(decimal);
  return(decimal);
}

void handleRoot(){
  File file = SPIFFS.open("/root.html","r");
  server.streamFile(file, "text/html");
  file.close();
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void siteStop(){  
  strip.clear();
  strip.show();
  handleRoot();
}

void siteColorWipe(){
  File file = SPIFFS.open("/colorWipe.html","r");
  server.streamFile(file, "text/html");
  file.close();
  if(server.hasArg("delay")){
    String hex = server.arg("color"); 
    uint16_t del = server.arg("delay").toInt();
    uint32_t color = hextocolor(hex);
    colorWipe(color, del);
  }
}

void siteTheatreChase(){
  File file = SPIFFS.open("/theaterChase.html","r");
  server.streamFile(file, "text/html");
  file.close();
  if(server.hasArg("delay")){
    String hex = server.arg("color");  
    uint16_t del = server.arg("delay").toInt();
    uint8_t cycle = server.arg("cycle").toInt();
    uint32_t color = hextocolor(hex);
    theaterChase(color, del, cycle);
  }
}

void siteRainbow(){
  File file = SPIFFS.open("/rainbow.html","r");
  server.streamFile(file, "text/html");
  file.close();
  if(server.hasArg("delay") && server.hasArg("cycle") && server.hasArg("rate")){
    uint16_t del = server.arg("delay").toInt();
    uint8_t cycle = server.arg("cycle").toInt();
     uint8_t rate = server.arg("rate").toInt();
    rainbow(del, cycle, rate);
  }
}

void siteRainbowTheatre(){
  File file = SPIFFS.open("/rainbowTheater.html","r");
  server.streamFile(file, "text/html");
  file.close();
  if(server.hasArg("delay") && server.hasArg("cycle")){
    uint16_t del = server.arg("delay").toInt();
    uint8_t cycle = server.arg("cycle").toInt();
    theaterChaseRainbow(del,cycle);
  }
}

void siteLooper(){
  File file = SPIFFS.open("/looper.html","r");
  server.streamFile(file, "text/html");
  file.close();
  if(server.hasArg("delay")){
    String hex = server.arg("color"); 
    uint16_t del = server.arg("delay").toInt();
    uint8_t cycle = server.arg("cycle").toInt();
    uint8_t num = server.arg("num").toInt();
    uint32_t color = hextocolor(hex);
    looper(color, del, cycle, num);
  }
}
void siteReflector(){
  File file = SPIFFS.open("/reflector.html","r");
  server.streamFile(file, "text/html");
  file.close();
  if(server.hasArg("cycleDuration")){
    String hex = server.arg("color"); 
    uint16_t d1 = server.arg("d1").toInt();
    uint16_t d2 = server.arg("d2").toInt();
    uint16_t d3 = server.arg("d3").toInt();
    uint16_t d4 = server.arg("d4").toInt();
    uint32_t cycdur = server.arg("cycleDuration").toInt();
    uint8_t cycle = server.arg("cycle").toInt();
    uint32_t color = hextocolor(hex);
  /*Serial.println(color);
    Serial.println(d1);
    Serial.println(d2);
    Serial.println(d3);
    Serial.println(cycdur);
    Serial.println(cycle);*/
    reflector(color, cycdur, cycle, d1, d2, d3, d4);
  }
}
