
//#include <IRremoteESP8266.h>
#include <Arduino.h>
//#include <IRsend.h>
//#include <gpio.h>
//IRsend mySender(A0);
#define bedPin 25
#define bathPin 27
unsigned long stars,down,stars2=0;

#define bedDigiPin 28
#define bathDigiPin 29
int bedState = 0;
int bathState = 0;
int bathVal = LOW;
int bedVal = LOW;
	int bathLight = 9;
	int bedLight = 3;

		#define L1_ON        0xA010C	// RED
		#define L1_OFF    	 0x6010C	// GREEN
		#define L2_ON        0xE010C	// YELLOW
		#define L2_OFF       0x2010C 	// BLUE

				void sendIr(uint64_t cmd){
            Serial3.println("sending Sony");
          }
/**			         mySender.send(SONY,0xa8bca, 20);
                    delay(100);
          }
**/
void setup()
{
  //Serial3.begin(115200);
  //Serial3.println("*");
  Serial3.begin(115200);
  Serial3.println("*");
  delay(10);
	pinMode(bedPin, INPUT);
	pinMode(bathPin, INPUT);
  delay(500);
//pinMode(SEND_PIN, OUTPUT);
}

void bedCheck(){
	bedVal = digitalRead(bedPin); // read input value
  if (bedVal == HIGH) {
    if (bedState == LOW) {         // we have just turned on
	sendIr(L1_ON); Serial3.println("1"); stars=0; stars++;down=0; bedState = HIGH;
       } else if (bedState == HIGH){
        if (stars>=100){
  //        Serial3.println("*"); stars=0;
        }
//      Serial3.print("*"); stars++;
       }
  } else if (bedVal == LOW){
    if (bedState == HIGH) {
      Serial3.println("_END - TURNING OFF LIGHT in 5");
        sendIr(L1_OFF); Serial3.println("2");
      bedState = LOW; stars = 0; down =0; down++;
  } else if (bedState == LOW){
    if (down>=100){
  //        Serial3.println("*"); down=0;
        }
        down++;
  //  Serial3.print("_");
    }
    }
  delay(10);
  }

void bathCheck(){
	bathVal = digitalRead(bathPin); // read input value
  if (bathVal == HIGH) {
    if (bathState == LOW) {         // we have just turned on
	sendIr(L2_ON);Serial3.println("3");
//  Serial3.println("*NEW MOVE"); stars=0; stars++;down=0;
       bathState = HIGH;
       } else if (bathState == HIGH){
        if (stars>=100){
  //        Serial3.println("*"); stars=0;
        }
  //    Serial3.print("*"); stars++;
       }
  } else if (bathVal == LOW){
    if (bathState == HIGH) {
    //  Serial3.println("_END - TURNING OFF LIGHT in 5");
          bathState = LOW; stars = 0; down =0; down++;
   sendIr(L2_OFF);Serial3.println("4");
    } else if (bathState == LOW){
    if (down>=100){
    //      Serial3.println("*"); down=0;
        }
        down++;
//    Serial3.print("_");
    }
    }
  delay(10);
  }

void loop() {
	bathCheck();
	bedCheck();
	delay(1000);
 }
