#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
const char ssid[] = "WiFi2";
const char pass[] = "4328646518";

#define pirbath 4
#define pirbed 14
int pirbathIs,pirbedIs,pirbathWas = 0;
long time =0;
long delayLength = 10000;
bool withinBedDelay,withinBathDelay = false;
long delayBathStart, delayBedStart=0;

void readBath(){ // bath is gpio 4
   if ((time - delayBathStart) >= delayLength){
      withinBathDelay= false;
   }
   int pir1pin = digitalRead(pirbath);
          if (pir1pin == HIGH){
             // BATH MOTION
             Serial.print("BATHROOM MOTION...");
             //turn on light...!
            withinBathDelay=true; delayBathStart=millis();
            pirbathIs=1;
            } else {   // difference to prev, being no movement...
            if (withinBathDelay){
               return;
            } else {    // no movement, not within delay...
               Serial.print("turn light off here...");
               }
   }
pirbathWas = pirbathIs;
pirbathIs=pir1pin;
}

void readBed(){  // bed is gpio 14
   int pir2pin = digitalRead(pirbed);
   if ((time - delayBedStart) >= delayLength){
      withinBedDelay= false;
   }
       if (pir2pin == HIGH){
             // BED MOTION
             Serial.print("BEDROOM MOTION...");
             //turn on light...!
            withinBedDelay=true; delayBedStart=millis();
            pirbedIs=1;
            } else {   // difference to prev, being no movement...
            if (withinBedDelay){
               return;
            } else {    // no movement, not within delay...
               Serial.print("turn light off here...");
               }
         }
   pirbedIs=pir2pin;
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void setup() {
 withinBathDelay=false;
 withinBedDelay=false;
 pinMode(pirbed, INPUT_PULLUP);
 pinMode(pirbath, INPUT_PULLUP);
 delay(50);
   Serial.begin(115200);
   Serial.println("COMMENCING TNODE LIGHTSENSOR");
 delay(50);
 setup_wifi();
}

void loop(){
 time = millis();
 int inputState = digitalRead(pirbath);
 if (inputState==HIGH){        // something's moving!
   if (motion==false){         // we didn't think anything was moving!!
     Serial.println("motion detected!");
     client1.put(pathPut,"{\"on\":true}", &response);        /// TURN OFF LIGHT!
   motion =true;
     lastMove=millis();
     bufferMove=1;
   } else {
     bufferMove++;             // we know you're moving --> you're still moving!
   }
 } else if (inputState==LOW){  // nothing moving....
     if (motion==true){        // there used to be movement!?
       motion = false;
       Serial.println("motion NOT detected!");
       Serial.println("buffer is " + bufferMove);
       bufferMove=bufferMove+5;
       if (bufferMove <= 0 ){
         bufferMove--;
         delay(1000);
   client1.put(pathPut,"{\"on\":false}", &response);        /// TURN OFF LIGHT!
       }
     } else if ( motion ==false){
       bufferMove--;
     }
   }
 }
