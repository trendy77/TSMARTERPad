
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <IRremoteESP8266.h>
#include "RestClient.h"
#include <ArduinoJson.h>
WiFiClient client;
RestClient client1 = RestClient("192.168.0.101");
int pirState = LOW;
#define HOST_NAME "tnode188"        // Host mDNS
char bridge_ip[] = "192.168.0.101";  // IP address of the HUE bridge
const char *pathGet = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/5";
const char *pathPut = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/5/state";
String response;
unsigned long timeq, lastMove=0;
    int bufferMove=0;
    bool motion=false;
const char ssid[] = "WiFi2";
const char pass[] = "4328646518";
const int port = 80;
#define pirPin 14//14 or d6 is 12
int pirValue;     // Place to store read PIR Value

char json[] ="{\"state\":{\"on\":true,\"bri\":150,\"hue\":33333,\"sat\":240,\"effect\":\"none\",\"xy\":[0.111,0.07],\"ct\":500,\"alert\":\"select\",\"colormode\":\"xy\",\"reachable\":true},\"swupdate\":{\"state\":\"noupdates\",\"lastinstall\":null},\"type\":\"Extended color light\",\"name\":\"Lounge Outwards\",\"modelid\":\"LCT001\",\"manufacturername\":\"Philips\",\"uniqueid\":\"00:17:88:01:00:f8:d4:a5-0b\",\"swversion\":\"5.23.1.13452\"}";
StaticJsonBuffer<300> jsonBuffer;  // Root of the object tree.
JsonObject& root = jsonBuffer.parseObject(json);
  const char* state = root["state"];
  bool on = root["on"];
 int bri = root["bri"];
 int hue = root["hue"];
 int sat = root["sat"];
 double x = root["xy"][0];
 double y = root["xy"][1];
 int ct = root["ct"];

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

//Setup
void setup() {
  Serial.begin(115200);
  Serial.println("connect to WiFi network");
  setup_wifi();
    pinMode(pirPin, INPUT);
  pirValue = LOW;
  Serial.println("Setup!");
}

void loop() {
timeq = millis();
  int inputState = digitalRead(pirPin);
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
  JsonObject& root = jsonBuffer.parseObject(json);
  const char* state = root["state"];
  bool on = root["state"][0];
 int bri = root["state"][1];
 int hue = root["hue"][2];
 int sat = root["sat"];
 double x = root["xy"][0];
 double y = root["xy"][1];
 int ct = root["ct"];
String response = "";
client1.get(pathGet, &response);

Serial.println("state = " + on);
Serial.println("bri = " + bri);
//   sendRequest('5',response,'true');
delay(2000);
}

void sendRequest(int light, String scmd, String value) {
    // make a String for the HTTP request path:
  String request = "";
  request += "/lights/";
  request += light;
  request += "/state/";
  String contentType = "application/text";
    // make a string for the JSON command:
  String hueCmd = "{\"" + scmd;
  hueCmd += "\":\"";
  hueCmd += value;
  hueCmd += "\"}";
  int length = hueCmd.length();
  char msgBuffer[length];
      // Convert data string to character buffer
  hueCmd.toCharArray(msgBuffer,length+1);
    // see what you assembled to send:
  Serial.print("PUT request to server: ");
  Serial.println(request);
client1.put(pathPut,msgBuffer,&response);
  //int statusCode = httpClient.responseStatusCode();
    Serial.print(response);
}

void sendCheck(int light) {
  Serial.print("GET light state");
   Serial.println(pathGet);
   String response = "";
      // make the GET request to the hub:
 client1.get(pathGet, &response);
    Serial.print(response);

  }

