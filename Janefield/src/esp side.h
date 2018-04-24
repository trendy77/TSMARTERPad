/*
wemos MEGA ESP
ESP SKETCH
 3.1 RUN
*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


  // Wi-Fi / IoT
  const char ssid[] = "Northern Frontier Intertubes";
  const char pass[] = "num4jkha8nnk";
  const int port = 80;                // fix IP of this node
  IPAddress gateway(10, 0, 77, 100);  // WiFi router's IP
  IPAddress subnet(255, 255, 255, 0); //IPAddress ip(192, 68,78,188);

  
  void setup_wifi()
  {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, pass);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      Serial.println("Connection Failed! Rebooting...");
      delay(5000);
      ESP.restart();
    }
  }
