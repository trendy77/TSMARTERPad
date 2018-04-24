/*
 *      ESP SENSOR LIGHT
 *
*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266SSDNS.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <RestClient.h>
// infrared
#include <IRremoteESP8266.h>
//#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>


// COMMENT OUT DEPENDING ON WHICH ONE....
//////////////////////
//IPAddress ip(); ESP8266WebServer server(9188);  int Tfield = 1;   int Hfield = 2;const char* host = "nodeinside";
// Outside
IPAddress ip(10, 77, 0, 34); ESP8266WebServer server(3434); int Tfield = 3;  int Hfield = 4; const char* HOST_NAME = "nodemotion"; IPAddress gateway(10,77,0,100);


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

const char LIGHTS_ON[] = "{\"on\":true}";
const char LIGHTS_OFF[] = "{\"on\":false}";
const char *bridge_ip = "10.0.77.101"; // Hue Bridge IP
const int port = 80;
RestClient hue = RestClient(bridge_ip);

bool looping;
#define pirPin D1   //d1
int pirState = 0;
int pirVal = LOW;
bool watch = false;

void hueturnOff1(){
  hue.put("/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/1/state/", LIGHTS_OFF);
}
void hueturnOn1(){
  hue.put("/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/1/state/", LIGHTS_ON);
}
  void hue_turn(int no,const char cmdw){
    String cmd = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/";
    cmd += no;
    cmd += "/state/";  int buf = cmd.length();
    char buffer[buf];
    cmd.toCharArray(buffer, buf);
    hue.put(buffer, cmdw);
}

long time =0;


void checkMotion()
{
    if (digitalRead(pirPin) == HIGH)
    {
        digitalWrite(ledPin, HIGH); //the led visualizes the sensors output pin state
        if (lockLow)
        {
            motion = true; //makes sure we wait for a transition to LOW before any further output is made:
            lockLow = false;
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
        }
    }
}

void setup(){
    motion = false;
    pinMode(pirPin, INPUT); // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
    pinMode(ledPin, OUTPUT);
    digitalWrite(pirPin, LOW);

    Serial.begin(115200);
    Serial.print("booting");
    delay(500);
   
    WiFi.config(ip, gateway);
     WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);  Serial.print(".");
  }
    server.begin();
    IPAddress myAddress = WiFi.localIP(); 
    Serial.println(myAddress);
   if (mdns.begin(HOST_NAME, WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
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
      ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      });
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
  time = millis();


  WiFiClient client = server.available();

  if (irrecv.decode(&results)) {
   dump(&results);
   irrecv.resume();
  }

  	if (client.available() > 0) {
  		incomingByte = Serial.read();
  		msg += incomingByte;
      if (incomingByte == '*'){

      }

    time = millis();

    checkMotion();

    if (motion)
    {
        Serial.print("!");
    }

  
}