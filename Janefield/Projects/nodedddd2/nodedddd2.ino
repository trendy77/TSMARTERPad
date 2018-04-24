#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//#include <RestClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ThingSpeak.h>

const char ssid[] = "TPG 15EA";
const char pass[] = "abcd1989";
const int port = 1234;                // fix IP of this node

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete


void setup() {   

   Serial.begin(9600);
    delay(500); // Wait a bit for the serial connection to be established.
   Serial.print("Connecting to ");
    Serial.println(ssid);
  //  WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, pass);
    ;
  Serial.println("Booting");
  
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
    ArduinoOTA.setHostname("mgaaesp");
    //ArduinoOTA.setPort(8189);
   
   ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
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
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
    ArduinoOTA.handle();

int dataState = wifi.isNewDataComing(WIFI_SERVER);
  if(dataState != WIFI_NEW_NONE) {
    if(dataState == WIFI_NEW_CONNECTED) {
	  // Connected with TCP Client Side
      Serial.println("Status : Connected");
    } else if(dataState == WIFI_NEW_DISCONNECTED) {
	  // Disconnected from TCP Client Side
      Serial.println("Status : Disconnected");
    } else if(dataState == WIFI_NEW_MESSAGE) {
	  // Got a message from TCP Client Side
      Serial.println("ID : " + String(wifi.getId()));
      Serial.println("Message : " + wifi.getMessage());
      wifi.closeTCPConnection(0);
    } else if(dataState == WIFI_NEW_SEND_OK) {
	  // Message transfer has successful
      Serial.println("SENT!!!!");
    }
  }
}


/*

   while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
 }
    if (stringComplete) {
    Serial.println(inputString);
    inputString = "";
    stringComplete = false;
  }
  } 

 */
