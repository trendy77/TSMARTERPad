#include <Arduino.h>
#include <Time.h>       
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#define I2C_ADDRESS 0x3C
#define RST_PIN -1

SSD1306AsciiAvrI2c oled;

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete


/* record.ino Example sketch for IRLib2
 *  Illustrate how to record a signal and then play it back.
 */
#include <IRLib2.h>
#include <DecodeBase.h>  //We need both the coding and
#include <IRLibSendBase.h>    // sending base classes
#include <IRLib_P01_NEC.h>    //Lowest numbered protocol 1st
#include <IRLib_P02_Sony.h>   // Include only protocols you want
#include <IRLib_P03_RC5.h>
#include <IRLib_P04_RC6.h>
#include <IRLib_P05_Panasonic_Old.h>
#include <IRLib_P07_NECx.h>
#include <IRLib_HashRaw.h>    //We need this for IRsendRaw
#include <IRLibCombo.h>       // After all protocols, include this
// All of the above automatically creates a universal decoder
// class called "IRdecode" and a universal sender class "IRsend"
// containing only the protocols you want.
// Now declare instances of the decoder and the sender.
IRdecode myDecoder;
IRsend mySender;
#include <IRLibRecv.h>
IRrecv myReceiver(2); 

// Storage for the recorded code
uint8_t codeProtocol;  // The type of code
uint32_t codeValue;    // The data bits if type is not raw
uint8_t codeBits;      // The length of the code in bits

//These flags keep track of whether we received the first code 
//and if we have have received a new different code from a previous one.
bool gotOne, gotNew; 

void setup() {
  gotOne=false; gotNew=false;
  codeProtocol=UNKNOWN; 
  codeValue=0; 
  Serial.begin(9600);
  delay(2000);while(!Serial);//delay for Leonardo
  Serial.println(F("Send a code from your remote and we will record it."));
  Serial.println(F("Type any character and press enter. We will send the recorded code."));
  Serial.println(F("Type 'r' special repeat sequence."));
  myReceiver.enableIRIn(); // Start the receiver
}

// Stores the code for later playback
void storeCode(void) {
  gotNew=true;    gotOne=true;
  codeProtocol = myDecoder.protocolNum;
  Serial.print(F("Received "));
  Serial.print(Pnames(codeProtocol));
  if (codeProtocol==UNKNOWN) {
    Serial.println(F(" saving raw data."));
    myDecoder.dumpResults();
    codeValue = myDecoder.value;
  }
  else {
    if (myDecoder.value == REPEAT_CODE) {
      // Don't record a NEC repeat value as that's useless.
      Serial.println(F("repeat; ignoring."));
    } else {
      codeValue = myDecoder.value;
      codeBits = myDecoder.bits;
    }
    Serial.print(F(" Value:0x"));
    Serial.println(codeValue, HEX);
  }
}
void sendCode(void) {
  if( !gotNew ) {//We've already sent this so handle toggle bits
    if (codeProtocol == RC5) {
      codeValue ^= 0x0800;
    }
    else if (codeProtocol == RC6) {
      switch(codeBits) {
        case 20: codeValue ^= 0x10000; break;
        case 24: codeValue ^= 0x100000; break;
        case 28: codeValue ^= 0x1000000; break;
        case 32: codeValue ^= 0x8000; break;
      }      
    }
  }
  gotNew=false;
  if(codeProtocol== UNKNOWN) {
    //The raw time values start in decodeBuffer[1] because
    //the [0] entry is the gap between frames. The address
    //is passed to the raw send routine.
    codeValue=(uint32_t)&(recvGlobal.decodeBuffer[1]);
    //This isn't really number of bits. It's the number of entries
    //in the buffer.
    codeBits=recvGlobal.decodeLength-1;
    Serial.println(F("Sent raw"));
  }
  mySender.send(codeProtocol,codeValue,codeBits);
  if(codeProtocol==UNKNOWN) return;
  Serial.print(F("Sent "));
  Serial.print(Pnames(codeProtocol));
  Serial.print(F(" Value:0x"));
  Serial.println(codeValue, HEX);
}



 void setSSD(){
 // first row
  oled.println("TSMARTPad MCU");
  // second row
  oled.set2X();
  oled.println("Setting Up...");
  // third row
  oled.set1X();
  oled.print("time: ");
  oled.print(millis());
  }
  

void serialEvent() {
  while (Serial3.available()){ 
    char inChar = (char)Serial3.read();
     inputString += inChar;
     if (inChar == '\n') {
      stringComplete = true;
    }
  }
}












void setup()
{

 pinMode(22, OUTPUT);
 digitalWrite(22, LOW);
  oled.begin(&Adafruit128x32, I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  oled.println("SETTING UP");
  oled.clear();
  
    Serial.begin(115200);
    Serial.print("start");
    Serial3.begin(9600);
    delay(500); // Wait a bit for the serial connection to be established.
   inputString.reserve(200);   
  oled.setFont(System5x7);
  oled.setScroll(true);
    oled.print("Done");

   Serial.println("Ready");
  String inpot = "";
  while (Serial3.available()){ 
    char inChar = (char)Serial3.read();
     inpot += inChar;
     if (inChar == '\n') {
      oled.println(inpot);
    }
  }
}

void loop()
{
  long time = millis();

 if (myReceiver.getResults()) {
    myDecoder.decode();
    storeCode();
    myReceiver.enableIRIn(); 
 }
      if (stringComplete) {
            Serial.println(inputString);
            inputString = "";
            stringComplete = false;
        }
        setSSD();

   
}


