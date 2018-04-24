/**
 * NEOPIXEL KITT BETA - PRODUCTION
 * 
 * **/
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define PIN 4
#define NUMPIXELS 16
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int delayval = 500;
bool secondRun = false;
WiFiClient wiFi;
MDNSResponder mdns;
bool shownMenu = false;
int watsdoin = 1;
const char ssid[] = "Northern Frontier Interwebs";
const char pass[] = "num4jkha8nnk";
const char ssid3[] = "AlfaRomeoGT";
const char pass3[] = "turismoGT";
boolean firstRun = false;
int dot = 0;
int stdDelaySec = 0;
const char ssid2[] = "AndroidAP";
const char pass2[] = "tttttttt";
ESP8266WebServer server(80);
boolean off = false;

long theTime, lastTime2, lastTime1 = 0;

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>AlfaRomeoGT</title><script src=\"https://www.w3schools.com/lib/w3color.js\"></script><style src=\"https://trendypublishing.com/tsmartpad/css/style.css\"></style><script src=\"https://trendypublishing.com/tsmartpad/js/colour.js\"></script></head><div class=\"w3-col l4 m12\" style=\"text-align:center;\"><h1>AlfaRomeoGT Neopixel</h1><p>Uptime: ";
  html += String(millis());
 html += "</p><br><h4>KITT IS CURRENTLY ";
 if (off==1){
  html += "OFF";
 }else if (off==0){
  html += "ON";
 }
  html += "<br><p>StdDelay: ";
 html += stdDelaySec;
  html += " secs</p><br><table><tr><th>SPECIAL MODES</th><th>SOLID COLOURS</th><th>DELAY TIME</th><tr><td><a href=\"0\">OFF</a></td></tr><tr><td><a href=\"1\">1 STD MODE</a></td><td><a href=\"2\">2 - GREEN</a></td><td><a href=\"3\">3 - 10 sec delay</a></td></tr><tr><td><a href=\"4\">4 - COP MODE</a></td><td><a href=\"5\">5 - YELLOW</a></td><td><a href=\"6\">6 - 30 sec delay</a></td></tr><tr><td><a href=\"7\">7 - RAINBOW</a></td><td><a href=\"8\">8 - BLUE</a></td><td><a href=\"9\">9 - 60 sec delay</a></td></tr></table></body></html>";

   server.send(200, "text/html", html);
}

void handleRootCmd(int cmd) {
  String html = "<!DOCTYPE html><html><head><title>AlfaRomeoGT</title><script src=\"https://www.w3schools.com/lib/w3color.js\"></script><style src=\"https://trendypublishing.com/tsmartpad/css/style.css\"></style><script src=\"https://trendypublishing.com/tsmartpad/js/colour.js\"></script></head><div class=\"w3-col l4 m12\" style=\"text-align:center;\"><h1>AlfaRomeoGT Neopixel</h1><p>Uptime: ";
  html += String(millis());
 html += "<h4>COMMAND ACCEPTED...";
 html += cmd; html += "</p><br><h4>KITT IS CURRENTLY ";
 if (off){
  html += "OFF";
 }else {
  html += "ON";
 }
  html += "<br><p>StdDelay: ";
 html += stdDelaySec;
  html += " secs</p><br><table><tr><th>SPECIAL MODES</th><th>SOLID COLOURS</th><th>DELAY TIME</th><tr><td><a href=\"0\">OFF</a></td></tr><tr><td><a href=\"1\">1 STD MODE</a></td><td><a href=\"2\">2 - GREEN</a></td><td><a href=\"3\">3 - 10 sec delay</a></td></tr><tr><td><a href=\"4\">4 - COP MODE</a></td><td><a href=\"5\">5 - YELLOW</a></td><td><a href=\"6\">6 - 30 sec delay</a></td></tr><tr><td><a href=\"7\">7 - RAINBOW</a></td><td><a href=\"8\">8 - BLUE</a></td><td><a href=\"9\">9 - 60 sec delay</a></td></tr></table></body></html>";
  server.send(200, "text/html", html);
   delay(200);
   setLights(cmd);
 }

void handleNotFound(){
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
}

void knightRider(uint16_t cycles, uint16_t speed, uint8_t width, uint32_t color) {
  uint32_t old_val[NUMPIXELS]; // up to 256 lights!
  // Larson time baby!
  for(int i = 0; i < cycles; i++){
    for (int count = 1; count<NUMPIXELS; count++) {
      strip.setPixelColor(count, color);
      old_val[count] = color;
      for(int x = count; x>0; x--) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        strip.setPixelColor(x-1, old_val[x-1]); 
      }
      strip.show();
      delay(speed);
    }
    for (int count = NUMPIXELS-1; count>=0; count--) {
      strip.setPixelColor(count, color);
      old_val[count] = color;
      for(int x = count; x<=NUMPIXELS ;x++) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        strip.setPixelColor(x+1, old_val[x+1]);
      }
      strip.show();
      delay(speed);
    }
  }
  clearStrip();
}
void clearStrip() {
  for( int i = 0; i<NUMPIXELS; i++){
    strip.setPixelColor(i, 0x000000); strip.show();
  }
}
uint32_t dimColor(uint32_t color, uint8_t width) {
   return (((color&0xFF0000)/width)&0xFF0000) + (((color&0x00FF00)/width)&0x00FF00) + (((color&0x0000FF)/width)&0x0000FF);
}
uint32_t colorWheel(byte WheelPos) {
  byte state = WheelPos / 21;
  switch(state) {
    case 0: return strip.Color(255, 0, 255 - ((((WheelPos % 21) + 1) * 6) + 127)); break;
    case 1: return strip.Color(255, ((WheelPos % 21) + 1) * 6, 0); break;
    case 2: return strip.Color(255, (((WheelPos % 21) + 1) * 6) + 127, 0); break;
    case 3: return strip.Color(255 - (((WheelPos % 21) + 1) * 6), 255, 0); break;
    case 4: return strip.Color(255 - (((WheelPos % 21) + 1) * 6) + 127, 255, 0); break;
    case 5: return strip.Color(0, 255, ((WheelPos % 21) + 1) * 6); break;
    case 6: return strip.Color(0, 255, (((WheelPos % 21) + 1) * 6) + 127); break;
    case 7: return strip.Color(0, 255 - (((WheelPos % 21) + 1) * 6), 255); break;
    case 8: return strip.Color(0, 255 - ((((WheelPos % 21) + 1) * 6) + 127), 255); break;
    case 9: return strip.Color(((WheelPos % 21) + 1) * 6, 0, 255); break;
    case 10: return strip.Color((((WheelPos % 21) + 1) * 6) + 127, 0, 255); break;
    case 11: return strip.Color(255, 0, 255 - (((WheelPos % 21) + 1) * 6)); break;
    default: return strip.Color(0, 0, 0); break;
  }
}

void kitt(){
  knightRider(3, 12, 4, 0xFF1000); // Cycles, Speed, Width, RGB Color (original orange-red)
  knightRider(3, 17, 3, 0xFF00FF); // Cycles, Speed, Width, RGB Color (purple)
  knightRider(3, 22, 2, 0x0000FF); // Cycles, Speed, Width, RGB Color (blue)
  knightRider(3, 27, 5, 0xFF0000); // Cycles, Speed, Width, RGB Color (red)
  knightRider(3, 32, 6, 0x00FF00); // Cycles, Speed, Width, RGB Color (green)
  knightRider(3, 32, 7, 0xFFFF00); // Cycles, Speed, Width, RGB Color (yellow)
  knightRider(3, 32, 8, 0x00FFFF); // Cycles, Speed, Width, RGB Color (cyan)
  knightRider(3, 32, 2, 0xFFFFFF); // Cycles, Speed, Width, RGB Color (white)
  clearStrip();
  delay(2000);
}

void rainBow()
{
  for(byte j=0; j<252; j+=7) {
    knightRider(1, 16, 2, colorWheel(j)); // Cycles, Speed, Width, RGB Color
  }
  clearStrip();
}
 
void setLights(int cmd)
{
  watsdoin = cmd;
  switch (watsdoin)
    {
       // 1st column
    // MODES
  case '1':    
   //  STANDARD KITT
     knightRider(6, 26, 4, 0xFF1000); // (original orange-red)
     break;
     case '4':
     // cop mode
      for(int i=0;i<NUMPIXELS;i+2){
      strip.setPixelColor(i, strip.Color(170,0,0)); // RED
      strip.setPixelColor(i+1, strip.Color(0,0,170)); // BLUE EACH 2ND 
      strip.show(); // This sends the updated pixel color to the hardware.
      delay(delayval); // Delay for a period of time (in milliseconds).
    }
      for(int i=0;i<NUMPIXELS;i+2){
      strip.setPixelColor(i, strip.Color(0,0,170)); // RED
      strip.setPixelColor(i+1, strip.Color(170,0,0)); // BLUE EACH 2ND 
      strip.show(); // This sends the updated pixel color to the hardware.
      delay(delayval); // Delay for a period of time (in milliseconds).
      }
      break;
     case '7':        rainBow();         break;
     
    // 2ND COLUMN 
      case '2':   knightRider(4, 26, 3, 0x00FF00); break;     // green
      case '5': knightRider(4, 26, 3, 0xFFFF00); break;       // yellow
        case '8': knightRider(1, 36, 3, 0x0000FF);break;     // blue                
    // 3rd column
      // delay timing changes
     case '3': stdDelaySec = 10;   break;
      case '6': stdDelaySec = 30;   break;
       case '9': stdDelaySec = 60;   break;
    // ON / OFF
      case '0':   off = true; clearStrip();    break;
     }
}

 void setup(void){
    Serial.begin(115200);
   int n = WiFi.scanNetworks();
  Serial.println("scan done");
  
 for (int i = 0; i < n; ++i) {
  String tryWi = WiFi.SSID(i);
    if (tryWi == ssid){
      WiFi.begin(ssid, pass);
    } else if (tryWi == ssid2){
      WiFi.begin(ssid2, pass2);
    }
    else if (tryWi == ssid3){
      WiFi.begin(ssid3, pass3);
    }
   }  
Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
    ArduinoOTA.setHostname("AlfaGT");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
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
  server.on("/", handleRoot);
  server.on("/0", [](){
  handleRootCmd(0);
  });
  server.on("/1", [](){
  handleRootCmd(1);
  });
server.on("/2", [](){
  handleRootCmd(2);
  });
  server.on("/3", [](){
  handleRootCmd(3);
  });
  server.on("/5", [](){
  handleRootCmd(5);
  });
  server.on("/7", [](){
  handleRootCmd(7);
  });
  server.on("/4", [](){
  handleRootCmd(4);
  });  
  server.on("/6", [](){
  handleRootCmd(6);
  });
  server.on("/8", [](){
  handleRootCmd(8);
  });
  server.on("/9", [](){
  handleRootCmd(9);
  });
  server.onNotFound(handleNotFound);
  server.begin();
  delay(500);
stdDelaySec = 10;
  firstRun = true;
  watsdoin = 2; 
  strip.begin(); 
  strip.show();  
   Serial.println("ALFAPixels server started");
  delay(500);
  kitt();
 }
 

void loop(void){
  ArduinoOTA.handle();
  if (firstRun == 1){
  setLights(1);
 firstRun = false;
 secondRun = true;
 delay(2000);
}
if (secondRun == 1){
  setLights(2);
 secondRun = false;
 delay(2000);
}
 theTime = millis();
 server.handleClient(); 

  /** DEBUG.....
   * if (dot==100){
  Serial.println(".");
  dot=0;    
  }else{
  Serial.print(".");
  dot++;
  } 
   */
 if (theTime >= (lastTime1 + (stdDelaySec*1000))){
        knightRider(4, 36, 4, 0xFF1000); // (original orange-red)
        knightRider(4, 24, 3, 0xFF1000); // (original orange-red)
         lastTime1 = theTime;
        strip.clear();
        
   }
 }
