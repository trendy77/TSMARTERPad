#include <Arduino.h>
#include <Wire.h>
#include <IRLib2.h>
#include <IRLibRecv.h>
#include <IRLibSendBase.h> // First include the send base
//#include <DecodeBase.h>

//Now include only the protocols you wish to actually use.//The lowest numbered protocol should be first but remainder
//can be any order.
#include <IRLib_P02_Sony.h>
#include <IRLib_P01_NEC.h>
#include <IRLibCombo.h> // After all protocols, include this
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

String inputString = "";        // a String to hold incoming data
boolean stringComplete = false; // whether the string is complete

uint8_t codeProtocol; // The type of code
uint32_t codeValue;   // The data bits if type is not raw
uint8_t codeBits;     // The length of the code in bits

//These flags keep track of whether we received the first code
//and if we have have received a new different code from a previous one.
bool gotOne, gotNew;

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1
IRrecv myReceiver(2);

SSD1306AsciiWire oled;
IRsend mySender;
IRdecode myDecoder;



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

void sendIr(){
//send a code every time a character is received from the
// serial port. You could modify this sketch to send when you
// push a button connected to an digital input pin.
//Substitute values and protocols in the following statement
// for device you have available.
mySender.send(SONY, 0xa8bca, 20); //Sony DVD power A8BCA, 20 bits
//mySender.send(NEC,0x61a0f00f,0);//NEC TV power button=0x61a0f00f
Serial.println(F("Sent signal."));
}

void setup()
{
     gotOne = false;
    gotNew = false;
    codeProtocol = UNKNOWN;
    codeValue = 0;
    Serial.begin(115200);
    Serial3.begin(9600);
    delay(2000);
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
 
    myReceiver.enableIRIn(); // Start the receiver
}

void loop()
{
    long time = millis();

    if (myReceiver.getResults())
    {
        myDecoder.decode();
        storeCode();
        myReceiver.enableIRIn();
    }
    if (stringComplete)
    {
        Serial.println(inputString);
        inputString = "";
        stringComplete = false;
    }
    setSSD();

    
}
