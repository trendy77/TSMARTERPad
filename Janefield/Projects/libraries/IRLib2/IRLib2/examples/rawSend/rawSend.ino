/* rawSend.ino Example sketch for IRLib2
 *  Illustrates how to send a code Using raw timings which were captured
 *  from the "rawRecv.ino" sample sketch.  Load that sketch and
 *  capture the values. They will print in the serial monitor. Then you
 *  cut and paste that output into the appropriate section below.
 */
#include <IRLibSendBase.h>    //We need the base code
#include <IRLib_HashRaw.h>    //Only use raw sender
#include <IRLibRecvPCI.h>


IRsendRaw mySender;
IRrecvPCI myReceiver(2); //pin number for the receiver

void sendRawIr(uint8_t cmd){
  mySender.send(rawData, RAW_DATA_LEN, 36); //Pass the buffer,length, optionally frequency
  Serial.println(F("Sent signal."));
  delay(50);
  myReceiver.resume();

}

void setup() {
  Serial.begin(9600);
  Serial3.begin(115200);
  myReceiver.enableIRIn(); // Start the receiver
  Serial.println(F("Ready to receive IR signals"));

}


void loop() {

  if (myReceiver.getResults())
  {
    Serial.println(F("THE CODE FOR IR RECIEVED IS .... "));
    Serial.println(F("designated location in rawSend.ino"));
    Serial.print(F("\n#define RAW_DATA_LEN "));
    Serial.println(recvGlobal.recvLength, DEC);
    Serial.print(F("uint16_t rawData[RAW_DATA_LEN]={\n\t"));
    for (bufIndex_t i = 1; i < recvGlobal.recvLength; i++)
    {
      Serial.print(recvGlobal.recvBuffer[i], DEC);
      Serial.print(F(", "));
      if ((i % 8) == 0)
        Serial.print(F("\n\t"));
    }
    Serial.println(F("1000};")); //Add arbitrary trailing space
    myReceiver.enableIRIn();     //Restart receiver
  }
  
   if (Serial3.read() != -1)
  {
    String inputString = "";
      char inChar = (char)Serial.read();
    // add it to the inputString:
   inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n')
    {
      Serial.println(inputString);
  }
}
}
