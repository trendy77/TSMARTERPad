/* /*
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com

#include <IRremote.h>

int RECV_PIN = A14;

IRrecv irrecv(RECV_PIN);
IRsend irsend;
decode_results results;

void setup()
{

  Serial.begin(115200);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");
}

void sendIr()
{
  for (int i = 0; i < 3; i++)
  {
    irsend.sendSony(0xa90, 12);
    delay(40);
  }
  Serial.println("sent");
}

long ttime, last = 0;

void loop()
{
  ttime = millis();
  if (irrecv.decode(&results))
  {
    Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  }
  delay(100);

  if (ttime > (last + 2000))
  {
    last = millis();
    sendIr();
  }
}



*/
// Include a receiver either this or IRLibRecvPCI or IRLibRecvLoop
#include <IRLibRecv.h>

#include <IRLibDecodeBase.h> // First include the decode base
#include <IRLib_P01_NEC.h>   // Now include only the protocols you wish
#include <IRLib_P02_Sony.h>  // to actually use. The lowest numbered
#include <IRLib_P07_NECx.h>  // must be first but others can be any order.
#include <IRLib_P09_GICable.h>
#include <IRLib_P11_RCMM.h>
#include <IRLibCombo.h> // After all protocols, include this


void setup() {
  Serial.begin(9600);
  delay(2000); while (!Serial); //delay for Leonardo
  myReceiver.enableIRIn(); // Start the receiver
  Serial.println(F("Ready to receive IR signals"));
}
void loop() {
  //Continue looping until you get a complete signal received
  if (myReceiver.getResults()) {
    myDecoder.decode();           //Decode it
    myDecoder.dumpResults(true);  //Now print results. Use false for less detail
    myReceiver.enableIRIn();      //Restart receiver
  }
}
