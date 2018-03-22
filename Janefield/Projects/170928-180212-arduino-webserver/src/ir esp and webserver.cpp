
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
WiFiServer server(80);
const char* ssid = "WiFi2";
const char* pass = "4328646518";

#define NEC 'N'
#define SONY 'S'
#define RC5 'r'
#define RC6 'R'

IRsend irsend(4);  // An IR LED is controlled by GPIO pin 4 (D2)

// Example of data captured by IRrecvDumpV2.ino
uint16_t rawData[67] = {9000, 4500, 650, 550, 650, 1650, 600, 550, 650, 550,
                        600, 1650, 650, 550, 600, 1650, 650, 1650, 650, 1650,
                        600, 550, 650, 1650, 650, 1650, 650, 550, 600, 1650,
                        650, 1650, 650, 550, 650, 550, 650, 1650, 650, 550,
                        650, 550, 650, 550, 600, 550, 650, 550, 650, 550,
                        650, 1650, 600, 550, 650, 1650, 650, 1650, 650, 1650,
                        650, 1650, 650, 1650, 650, 1650, 600};
unsigned long rctoggle = 0;

void process(String req) {
  char type = req[0];
  if (type != NEC && type != SONY && type != RC5 && type != RC6) {
    Serial.print("Unexpected type: ");
    Serial.println(type);
    return;
  }
  unsigned long code = 0;
  for (int i = 1; i < 8; i++) {
    char ch = req[i];
    if (ch >= '0' && ch <= '9') {
      code = code * 16 + ch - '0';
    }
    else if (ch >= 'a' && ch <= 'f') {
      code = code * 16 + ch - 'a' + 10;
    }
    else if (ch >= 'A' & ch <= 'F') {
      code = code * 16 + ch - 'A' + 10;
    }
    else {
      Serial.print("Unexpected hex char: ");
      Serial.println(ch);
      return;
    }
  }
  if (type == NEC) {
    irsend.sendNEC(code, 32);
  }
  else if (type == SONY) {
    // Send Sony code 3 times
    irsend.sendSony(code, 12, 2);
    delay(50);
  }
  else if (type == RC5) {
    // Filp the RC5 toggle bit
    code = code ^ (rctoggle << 11);
    rctoggle = 1 - rctoggle;
    irsend.sendRC5(code, 12);
  }
  else if (type == RC6) {
    // Flip the RC6 toggle bit
    code = code ^ (rctoggle << 16);
    rctoggle = 1 - rctoggle;
    irsend.sendRC6(code, 20);
  }
  Serial.print("Sent code: ");
  Serial.println(code, HEX);
}

void setup() {
  irsend.begin();
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  server.begin();
}

void loop() {

    Serial.println("new client");
    irsend.sendSony(0xa90, 12, 2);
  delay(2000);
}
