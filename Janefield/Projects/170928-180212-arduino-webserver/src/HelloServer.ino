#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

const char* ssid = "WiFi2";
const char* pass = "4328646518";
MDNSResponder mdns;
ESP8266WebServer server(80);
IRsend irsend(4);  // An IR LED is controlled by GPIO pin 4 (D2)
const int led = 13;


void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from ir SENDER!");
  digitalWrite(led, 0);
}
void handleIr(code,proto) {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from ir SENDER!");
  digitalWrite(led, 0);
    for (uint8_t i = 0; i < server.args(); i++) {
      if (server.argName(i) == "proto") {
        uint32_t code = strtoul(server.arg(i).c_str(), NULL, 10);
        //switch which protocol...
          switch(proto){
              case 'NEC':
              // send nec
              if (server.argName(i) == "code") {
                uint32_t code = strtoul(server.arg(i).c_str(), NULL, 10);
                irsend.sendNEC(code, 32);
              }
              break;
              case 'SONY':
              // send SONY
              break;
          }
      }
  }
}
void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(led, OUTPUT);
  irsend.begin();
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  Serial.println("");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);  Serial.print(".");
    }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
    if (mdns.begin("nodeIRsendr", WiFi.localIP())) {
      Serial.println("MDNS responder started");
    }
  server.on("/", handleRoot);
  server.on("/", handleIr);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
