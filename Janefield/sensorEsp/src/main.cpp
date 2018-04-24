/*
 *      ESP SENSOR LIGHT
 *
*/

#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "RestClient.h"

#include <IRremoteESP8266.h>
//#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266SSDP.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <IFTTTMaker.h>

#define KEY "bxRjKjTWd3zf5CMs-T76Hg"
//  {"value1":"","value2":"","value3":""}
WiFiClientSecure iftttclient;
IFTTTMaker ifttt(KEY, iftttclient);

// COMMENT OUT DEPENDING ON WHICH ONE....
//////////////////////
//IPAddress ip(); ESP8266WebServer server(9188);  int Tfield = 1;   int Hfield = 2;const char* host = "nodeinside";
// Outside
IPAddress ip(10, 77, 0, 34);  int Tfield = 3;  int Hfield = 4; const char* HOST_NAME = "nodemotion";
IPAddress gateway(10, 77, 0, 100);
IPAddress sub(255,255,255, 192);

#define ledPin 0
#define pirPin 9               // 

bool motion = false;            //the time when the sensor outputs a low impulse
long unsigned int lowIn;               //the amount of milliseconds the sensor has to be low before we assume all motion has stopped
int pause = 5000;
bool lockLow = true;
bool takeLowTime;
int calibrationTime = 30;

WiFiClient wiFi;
int incomingByte = 0;
MDNSResponder mdns;
const char ssid[] = "Northern Frontier Intertubes";
const char pass[] = "num4jkha8nnk";

uint16_t RECV_PIN = 4;
//IRsend irsend(5);
uint16_t *code_array;
IRrecv irrecv(RECV_PIN);
decode_results results;  // Somewhere to store the results
irparams_t save;         // A place to copy the interrupt state while decoding.
bool waitin = false;
long waitTil = 0;
const char LIGHTS_ON[] = "{\"on\":true}";
const char LIGHTS_OFF[] = "{\"on\":false}";
const char *bridge_ip = "10.0.77.101"; // Hue Bridge IP
const int port = 80;
RestClient hue = RestClient(bridge_ip);

void dump(decode_results *results)
{
  // Dumps out the decode_results structure.
  // Call this after IRrecv::decode()
  uint16_t count = results->rawlen;
  if (results->decode_type == UNKNOWN)
  {
    Serial.print("Unknown encoding: ");
  }
  else if (results->decode_type == NEC)
  {
    Serial.print("Decoded NEC: ");
  }
  else if (results->decode_type == SONY)
  {
    Serial.print("Decoded SONY: ");
  }
  else if (results->decode_type == RC5)
  {
    Serial.print("Decoded RC5: ");
  }
  else if (results->decode_type == RC5X)
  {
    Serial.print("Decoded RC5X: ");
  }
  else if (results->decode_type == RC6)
  {
    Serial.print("Decoded RC6: ");
  }
  else if (results->decode_type == RCMM)
  {
    Serial.print("Decoded RCMM: ");
  }
  else if (results->decode_type == PANASONIC)
  {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->address, HEX);
    Serial.print(" Value: ");
  }
  else if (results->decode_type == LG)
  {
    Serial.print("Decoded LG: ");
  }
  else if (results->decode_type == JVC)
  {
    Serial.print("Decoded JVC: ");
  }
  else if (results->decode_type == AIWA_RC_T501)
  {
    Serial.print("Decoded AIWA RC T501: ");
  }
  else if (results->decode_type == WHYNTER)
  {
    Serial.print("Decoded Whynter: ");
  }
  else if (results->decode_type == NIKAI)
  {
    Serial.print("Decoded Nikai: ");
  }
  serialPrintUint64(results->value, 16);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): {");

  for (uint16_t i = 1; i < count; i++)
  {
    if (i % 100 == 0)
      yield(); // Preemptive yield every 100th entry to feed the WDT.
    if (i & 1)
    {
      Serial.print(results->rawbuf[i] * RAWTICK, DEC);
    }
    else
    {
      Serial.print(", ");
      Serial.print((uint32_t)results->rawbuf[i] * RAWTICK, DEC);
    }
  }
  Serial.println("};");
}


void hueturnOff1(){
  hue.put("/api/ttCUNfvp0nYFR7cjo7POWcygSVlWP06Us-Bu-L3S/lights/1/state/", LIGHTS_OFF);
}
void hueturnOn1(){
  hue.put("/api/ttCUNfvp0nYFR7cjo7POWcygSVlWP06Us-Bu-L3S/lights/1/state/", LIGHTS_ON);
}
void trigger(String event)
{
if (ifttt.triggerEvent(event, ssid, ip.toString()))
{
  Serial.println("Successfully sent");
}else
{
  Serial.println("Failed!");
}
}

long owtime =0;


void checkMotion()
{
    if (digitalRead(pirPin) == HIGH)
    {
        digitalWrite(ledPin, HIGH); //the led visualizes the sensors output pin state
        if (lockLow)
        {
            motion = true; //makes sure we wait for a transition to LOW before any further output is made:
            lockLow = false;
            waitin = false;
            hueturnOn1();
            Serial.println("---");
            Serial.print("motion detected at ");
            Serial.print(millis() / 1000);
            Serial.println(" sec");
            delay(50);
        }
        takeLowTime = true;
    }

    if (digitalRead(pirPin) == LOW)
    {
        digitalWrite(ledPin, LOW); //the led visualizes the sensors output pin state
        if (takeLowTime)
        {
            motion = false;
            lowIn = millis();    //save the time of the transition from high to LOW
            takeLowTime = false; //make sure this is only done at the start of a LOW phase
        }
        //if the sensor is low for more than the given pause,
        //we assume that no more motion is going to happen
        if (!lockLow && millis() - lowIn > pause)
        {
            //makes sure this block of code is only executed again after
            //a new motion sequence has been detected
            lockLow = true;
            Serial.print("motion ended at "); //output
            Serial.print((millis() - pause) / 1000);
            Serial.println(" sec");
            delay(50);
            waitin = true;
            waitTil = (millis() + 5000);
        }
    }
}

void setup(){
    motion = false;
    waitin = false;
    pinMode(pirPin, INPUT); // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
    pinMode(ledPin, OUTPUT);
    digitalWrite(pirPin, LOW);

    Serial.begin(115200);
    Serial.print("booting");
    delay(500);
    WiFi.mode(WIFI_STA);
    WiFi.config(ip, gateway, sub);
     WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);  Serial.print(".");
  }

  IPAddress myAddress = WiFi.localIP();
  Serial.println(myAddress);
  if (mdns.begin(HOST_NAME, myAddress))
  {
    Serial.println("MDNS responder started");
  }
  ArduinoOTA.setHostname("espMotion");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
    MDNS.addService("telnet", "tcp", 23);
  //irsend.begin();
  

    Serial.print("calibrating sensor ");
    for (int i = 0; i < calibrationTime; i++)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(500);
     
     
      ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
      });

      ArduinoOTA.begin();
    delay(500);
    irrecv.enableIRIn();
}

void loop() {
  ArduinoOTA.handle();
  owtime = millis();

if (waitTil<=owtime){
  waitin = false;
  hueturnOff1();
}

  if (irrecv.decode(&results)) {
   dump(&results);
   irrecv.resume();
  }
 // String msg = '';
  //if (client.available() > 0)
  //{
   // incomingByte = Serial.read();
   // msg += incomingByte;
   // if (incomingByte == '\n')
   // {
    //  Serial.println(msg);
   // }
   // client.flush();
    //}
     checkMotion();
    
}