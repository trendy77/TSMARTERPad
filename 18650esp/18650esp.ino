/*
18650 NODEMCU
OLED SCREEN 
T
 */

    //// choose
#define N18650
//////    OLED SCRREN
/////     IR SENDER
    /// or
//#define E81111
//////    TEMP
/////////

#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <RestClient.h>
#include <WiFiClientSecure.h>
#include <DNSServer.h>
#include <ThingSpeak.h>
#include <PubSubClient.h>
#include <IFTTTMaker.h>
#include "JsonStreamingParser.h"
#include "JsonListener.h"
#include "ExampleParser.h"

WiFiClientSecure iftttclient;
#define KEY "bxRjKjTWd3zf5CMs-T76Hg"
#define EVENT1 "temp"
const char LIGHT_ON[] = "{\"on\":\"true\"}";
const char LIGHT_OFF[] = "{\"on\":\"false\"}";

/*void setLight(int hue, int lux, int bri, int lightNo)
{
  const char start[] = "{\"hue\":\"" + hue + "\"," + "\"bri\":\"" + bri + "\"," + "\"lux\":\"" + lux + "\"}";
  hue.put(start);
}
*/
//  {"temp":tempC,"humid":humidQ,"lux":""}
IFTTTMaker ifttt(KEY, iftttclient);
unsigned long tTime=0;
long lastUpdate, mLastTime1 = 0;
//uint16_t RECV_PIN = 3;  // d9
int delayMS = 500;
JsonStreamingParser parser;
ExampleListener listener;
const char *bridge_ip = "10.0.77.101";
RestClient hue = RestClient(bridge_ip);
IPAddress subnet(255, 255, 255, 192);
ESP8266WebServer server(80);
IPAddress gateway(10, 0, 77, 100);
WiFiClient client;
const char ssid[] = "Northern Frontier Interwebs";
const char pass[] = "num4jkha8nnk";
unsigned long myChannelNumber = 404585;
const char *myWriteAPIKey = "W124WS7UN76VCASZ";
MDNSResponder mdns;

int Tfield = 3;
int Hfield = 4;
float tempC, humidQ, temp, humid, maxT, minT, maxH, minH;
long resetTime = (12 * 60 * 1000);

#ifdef E81111
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define HOST_NAME "nodeEsp"
IPAddress ip(10, 0, 77, 112);
#define DHTPIN 3 //d9?    // d4
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);
#define LED 16      // Led in NodeMCU at pin GPIO16 (D0).
#define BRIGHT 350  //max led intensity (1-500)
#define INHALE 1250 //Inhalation time in milliseconds.
#define PULSE INHALE * 1000 / BRIGHT
#define REST 1000 //Rest Between Inhalations.
#endif

char tempString[6];
char humidString[6];

#ifdef N18650
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "OLEDDisplay.h"
#include "OLEDDisplayUi.h"
#include "Wire.h"
#include "WeatherStationFonts.h"
#include "WeatherStationImages.h"
#include "SSD1306Brzo.h"
#define HOST_NAME "node1860"
IPAddress ip(10, 0, 77, 111);
IRsend irsend(3); // d9
#define TV_OFF 0xa90
#define HiFi_OFF 0x540a
#define HiFi_TV 0x540a
#define HiFi_BT 0x540a
#define HiFi_Vup 0x240a
#define HiFi_Vdn 0x640a
#define L1_ON 0xA010C  // RED
#define L1_OFF 0x6010C // GREEN
#define L2_ON 0xE010C  // YELLOW
#define L2_OFF 0x2010C // BLUE
#define candleON 0x1FE48B7
#define candleOFF 0x1FE58A7
#define candle4H 0x1FE807F
#define candle8H 0x1F2A0F6B
#define candleMODE 0x1FE7887
#define candleMULTI 0x1FEC03F
#define candleB1 0x1FE609F
#define candleB2 0x1FE906F
#define candleB3 0x1FEF807
#define candleB4 0x1FE708F
#define candleG1 0x1FEA05F
#define candleG2 0x1FE10EF
#define candleG3 0x1FED827
#define candleG4 0x1FEB04F
#define candleR1 0x1FE20DF
#define candleR2 0x1FEE01F
#define candleR3 0x1FE50AF
#define candleR4 0x1FE30CF
#endif
/*
void doLoop(int on, int lightNo)
{
  if (on == 1)
  {
    const char EFFECT_COLORLOOP[] = "{\"effect\":\"colorloop\"}";
    String cmd = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/";
    cmd += lightNo;
    cmd += "/state/";
    hue.put(cmd, EFFECT_COLORLOOP);
  }
  if (on == 0)
  {
    const char NO_COLORLOOP[] = "{\"effect\":\"none\"}";
    String cmd = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/";
    cmd += lightNo;
    cmd += "/state/";
    hue.put(cmd, NO_COLORLOOP);
  }
}
void hue_turnOff(int lightNo)
{
  String cmd = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/";
  cmd += lightNo;
  cmd += "/state/";
  int buf = cmd.length();
  char buffer[buf];
  cmd.toCharArray(buffer, buf);
  hue.put(buffer, LIGHT_ON);
}

void trigger(String event)
{
  if (ifttt.triggerEvent(event, ssid, ip.toString()))
  {
    Serial.println("Successfully sent");
  }
  else
  {
    Serial.println("Failed!");
  }
}
*/
#ifdef N18650

void sendNECIr(uint8_t cmd)
{
  irsend.begin();
  Serial.println("sending " + cmd);
  irsend.sendNEC(cmd, 32);
  delay(100);
}

void sendSonyIr(uint64_t cmd)
{
  irsend.begin();
  Serial.println("sending Sony");
  irsend.sendSony(cmd, 12, 2);
  delay(100);
}

const int I2C_DISPLAY_ADDRESS = 0x3c;
const int SDA_PIN = 4; //D2
const int SDC_PIN = 5; //D1
SSD1306Brzo display(0x3c, SDC_PIN, SDA_PIN);
OLEDDisplayUi ui(&display);

void drawProgress(OLEDDisplay *display, int percentage, String label);
void drawOtaProgress(unsigned int, unsigned int);
void updateData(OLEDDisplay *display);
void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void drawCurrentWeather(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void drawSendIr(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void drawThingspeak(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void drawForecastDetails(OLEDDisplay *display, int x, int y, int dayIndex);
void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState *state);
void setreadyForIFTTTUpdate();
int8_t getWifiQuality();
int numberOfFrames = 5;
OverlayCallback overlays[] = {drawHeaderOverlay};
int numberOfOverlays = 1;

void drawProgress(OLEDDisplay *display, int percentage, String label)
{
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(64, 10, label);
  display->drawProgressBar(2, 28, 124, 10, percentage);
  display->display();
}

void drawOtaProgress(unsigned int progress, unsigned int total)
{
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 10, "OTA Update");
  display.drawProgressBar(2, 28, 124, 10, progress / (total / 100));
  display.display();
}

void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  String date = "date";
  int textWidth = display->getStringWidth(date);
  display->drawString(64 + x, 5 + y, date);
  display->setFont(ArialMT_Plain_24);
  long time = millis();
  textWidth = display->getStringWidth(time);
  display->drawString(64 + x, 15 + y, time);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
}

void updateData(OLEDDisplay *display)
{
  drawProgress(display, 10, "Updating time...");
  drawProgress(display, 30, "Updating temp...");
  drawProgress(display, 50, "Updating humidity...");
  drawProgress(display, 80, "Updating thingspeak...");
  lastUpdate = millis();
  drawProgress(display, 100, "Done...");
  delay(1000);
}

void drawThingspeak(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(64 + x, 0 + y, "Current Temp/Humidity");
  display->setFont(ArialMT_Plain_16);
  display->drawString(64 + x, 10 + y, tempString);
  display->drawString(64 + x, 30 + y, humidString);
}
void drawMil(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(64 + x, 0 + y, "RUNNING FOR");
  display->setFont(ArialMT_Plain_16);
  String mil = millis();
  display->drawString(64 + x, 10 + y, mil);

  long secs = String(mil / 1000);
  long mins = (secs / 60);
  long hrs = (mins / 60);
  String tohe = String(hrs)+":" + String(mins) + "." + String(secs));
  display->drawString(64 + x, 30 + y, hrs);
}
int8_t getWifiQuality()
{
  int32_t dbm = WiFi.RSSI();
  if (dbm <= -100)
  {
    return 0;
  }
  else if (dbm >= -50)
  {
    return 100;
  }
  else
  {
    return 2 * (dbm + 100);
  }
}

FrameCallback frames[] = {drawThingspeak, drawMil};

void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState *state)
{
  display->setColor(WHITE);
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0, 54, String(state->currentFrame + 1) + "/" + String(numberOfFrames));
  long time = millis();
  String tttt = String(time);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(38, 54, tttt);
  int8_t quality = getWifiQuality();
  for (int8_t i = 0; i < 4; i++)
  {
    for (int8_t j = 0; j < 2 * (i + 1); j++)
    {
      if (quality > i * 25 || j == 0)
      {
        display->setPixel(120 + 2 * i, 63 - j);
      }
    }
  }
}

#endif

/*
uint16_t *code_array;
IRrecv irrecv(RECV_PIN);
decode_results results;  // Somewhere to store the results
//int candleArray[]={candleON,candleOFF,candle4H,candle8H,candleMODE,candleMULTI,candleB1,candleB2,candleB3,candleB4,candleG1,candleG2,candleG3,candleG4,candleG3,candleR1,candleR2,candleR3,candleR4};
//int candleNames[]={'candleON','candleOFF','candle4H','candle8H','candleMODE','candleMULTI','candleB1','candleB2','candleB3','candleB4','candleG1','candleG2','candleG3','candleG4','candleG3','candleR1','candleR2','candleR3','candleR4'};

*/

void getTemperature()
{
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println("Error reading temperature!");
  }
  else
  {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    tempC = event.temperature;
    Serial.println(" *C");
  }
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))
  {
    Serial.println("Error reading humidity!");
  }
  else
  {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    humidQ = event.relative_humidity;
    Serial.println("%");
  }
  dtostrf(tempC, 2, 2, tempString);
  dtostrf(humidQ, 2, 2, humidString);
  
  
  ThingSpeak.setField(Tfield, tempC);
  ThingSpeak.setField(Hfield, humidQ);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

`#ifdef ESPMEGA
  tempC = ThingSpeak.readFloatField(myChannelNumber, 3);
  humidQ = ThingSpeak.readFloatField(myChannelNumber, 4);
  float lux = ThingSpeak.readFloatField(myChannelNumber, 5);
#endif
}



void setup()
{

#ifdef N18650
  display.init();
  display.clear();
  display.display();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setContrast(255);
  drawProgress(display, 20, "starting...");
  ui.setTargetFPS(30);
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);
  ui.setIndicatorPosition(BOTTOM);
  ui.setFrameAnimation(SLIDE_LEFT);
  ui.setFrames(frames, numberOfFrames);
  ui.setOverlays(overlays, numberOfOverlays);
  ui.init();
#endif  
  
  
  Serial.begin(115200);
  WiFi.config(ip, gateway, subnet);
  String hostname(HOST_NAME);
  WiFi.hostname(hostname);
  
  #ifdef N18650
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  drawProgress(display, 20, "STARTING WIFI...");
  display.display();
  #endif
      
  WiFi.begin(ssid, pass);
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    #ifdef N18650
    display.drawXbm(60, 30, 8, 8, counter % 3 == 1 ? activeSymbol : inactiveSymbol);
    display.drawXbm(74, 30, 8, 8, counter % 3 == 2 ? activeSymbol : inactiveSymbol);
    display.display();
    #endif
    counter++;
  }

  #ifdef N18650
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  drawProgress(display, 40, "STARTING TEMP...");
  display.display();
  #endif
  
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print("Sensor:       ");
  Serial.println(sensor.name);
  Serial.print("Driver Ver:   ");
  Serial.println(sensor.version);
  Serial.print("Unique ID:    ");
  Serial.println(sensor.sensor_id);
  Serial.print("Max Value:    ");
  Serial.print(sensor.max_value);
  Serial.println(" *C");
  Serial.print("Min Value:    ");
  Serial.print(sensor.min_value);
  Serial.println(" *C");
  Serial.print("Resolution:   ");
  Serial.print(sensor.resolution);
  Serial.println(" *C");
  Serial.println("------------------------------------");
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print("Sensor:       ");
  Serial.println(sensor.name);
  Serial.print("Driver Ver:   ");
  Serial.println(sensor.version);
  Serial.print("Unique ID:    ");
  Serial.println(sensor.sensor_id);
  Serial.print("Max Value:    ");
  Serial.print(sensor.max_value);
  Serial.println("%");
  Serial.print("Min Value:    ");
  Serial.print(sensor.min_value);
  Serial.println("%");
  Serial.print("Resolution:   ");
  Serial.print(sensor.resolution);
  Serial.println("%");
  Serial.println("------------------------------------");
  delayMS = sensor.min_delay / 1000;
#ifdef N18650
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    drawProgress(display, 60, "SETTING UP OTA...");
    display.display();
#endif
  Serial.println("Hostname: " + hostname);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
    Serial.println("Start updating " + type);

#ifdef N18650
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 - 10, "OTA Update");
    display.display();
#endif
  });
  ArduinoOTA.onEnd([]() {

#ifdef N18650
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, "Restart");
    display.display();
#endif

  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT
  ArduinoOTA.onProgress(drawOtaProgress);
  updateData(&display);_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });

  ArduinoOTA.onProgress(drawOtaProgress);
  updateData(&display);
  
  ArduinoOTA.begin();

#ifdef N18650
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    drawProgress(display, 80, "OTA Hostname: " + hostname);
    display.display();
#endif
  Serial.println("Ready");
  Serial.println(WiFi.localIP());
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    drawProgress(display, 100, "READY.");
    display.display();
    delay(2000);
#endif

}

void loop()
{

#ifdef N18650
  updateData(&display);
  int remainingTimeBudget = ui.update();
  if (remainingTimeBudget > 0)
  {
    ArduinoOTA.handle();
  }
#else
  ArduinoOTA.handle();
#endif
  if (millis() >= (tTime + 30000))
  {
    getTemperature();
    tTime = millis();
  }
}
