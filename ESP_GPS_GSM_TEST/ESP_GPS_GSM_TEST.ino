#include <SoftwareSerial.h> //Software Serial header to communicate with GSM module 
#include <Streaming.h>

SoftwareSerial SIM800 (D6, D5); // RX, TX
SoftwareSerial GPS    (D7, A0); // RX, TX

const unsigned char UBX_HEADER[] = { 0xB5, 0x62 };

const String PHONENUM = "09448517225";
const String BaseLink = "The current Location is https://www.google.com/maps/place/";
String incoming = "";

struct NAV_POSLLH {
  unsigned char cls;
  unsigned char id;
  unsigned short len;
  unsigned long iTOW;
  long lon;
  long lat;
  long height;
  long hMSL;
  unsigned long hAcc;
  unsigned long vAcc;
};

NAV_POSLLH posllh;


String SIM800_send(String incoming) //Function to communicate with SIM800 module
{
  SIM800<<incoming<<endl; //Print what is being sent to GSM module
  String result = "";
  while(!SIM800.available());
  while (SIM800.available()) //Wait for result
  {
    char letter = SIM800.read();
    result += String(letter); //combine char to string to get result
  }
  return result; //return the result
}


void calcChecksum(unsigned char* CK) {
  memset(CK, 0, 2);
  for (int i = 0; i < (int)sizeof(NAV_POSLLH); i++) {
    CK[0] += ((unsigned char*)(&posllh))[i];
    CK[1] += CK[0];
  }
}

bool processGPS() {
  static int fpos = 0;
  static unsigned char checksum[2];
  const int payloadSize = sizeof(NAV_POSLLH);

  while ( GPS.available() ) {
    byte c = GPS.read();
    if ( fpos < 2 ) {
      if ( c == UBX_HEADER[fpos] )
        fpos++;
      else
        fpos = 0;
    }
    else {
      if ( (fpos-2) < payloadSize )
        ((unsigned char*)(&posllh))[fpos-2] = c;

      fpos++;

      if ( fpos == (payloadSize+2) ) {
        calcChecksum(checksum);
      }
      else if ( fpos == (payloadSize+3) ) {
        if ( c != checksum[0] )
          fpos = 0;
      }
      else if ( fpos == (payloadSize+4) ) {
        fpos = 0;
        if ( c == checksum[1] ) {
          return true;
        }
      }
      else if ( fpos > (payloadSize+4) ) {
        fpos = 0;
      }
    }
  }
  return false;
}


void setup() {
  //PWRKY pin of GSM module has to be pulled low for 1sec to enable the module
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.begin(9600); //Serial COM for debugging
  SIM800.begin(9600); //Software serial called SIM800 to speak with SIM800 Module
  GPS.   begin(9600);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000); //wait for serial COM to get ready
}

void loop() {
  digitalWrite(LED_BUILTIN, millis()%2000<1900);
  if (SIM800.available()) { //Check if the SIM800 Module is telling anything
    char a = SIM800.read();
    Serial.write(a); //print what the module tells on serial monitor
    incoming += String(a);
    if (a == 13) //check for new line
      incoming = ""; //clear the string if new line is detected
    incoming.trim(); //Remove /n or /r from the incoming data

    if (incoming == "RING") //If an incoming call is detected the SIM800 module will say "RING" check for it
    {
      Serial<<(SIM800_send("ATH" )); //Hand up the incoming call using ATH
      delay (1000);
      Serial<<(SIM800_send("ATE0")); //Disable Echo
      delay (1000);

      while(!processGPS());

      Serial<<"Sending sms"<<endl;
      SIM800<<"AT+CMGF=1"<<endl; //Set the module in SMS mode
      delay(1000);

      SIM800<<"AT+CMGS="<<"\""<<PHONENUM<<"\""<<endl; //Send SMS to this number
      delay(1000);
      String Lat = String(posllh.lat/10000000) + "." + String(posllh.lat%10000000);
      String Lon = String(posllh.lon/10000000) + "." + String(posllh.lon%10000000);
      Serial<< BaseLink << Lat <<","<< Lon << endl;
      SIM800<< BaseLink << Lat <<","<< Lon << endl<< (char)26 <<endl;
      delay(1000);
    }
  }

  else{
    GPS.flush();
  }

  if (Serial.available()) { //For debugging
    SIM800.write(Serial.read());
  }
}
