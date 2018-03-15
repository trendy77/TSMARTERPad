#include <Arduino.h>
/*
18650 NODEMCU
OLED SCREEN 
T
 */

#ifndef UNIT_TEST
#include <Arduino.h>
#endif

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "OLEDDisplay.h"
#include "OLEDDisplayUi.h"

int delayMS = 500;
JsonStreamingParser parser;
ExampleListener listener;
#define HOST_NAME "node18650"
IPAddress ip(10, 0, 77, 111);
IPAddress subnet(255, 255, 255, 192);
ESP8266WebServer server(80);
IPAddress gateway(10, 0, 77, 100);
WiFiClient client;
const char ssid[] = "Northern Frontier Interwebs";
const char pass[] = "num4jkha8nnk";
int Tfield = 3;
int Hfield = 4;

#define LUXPIN A3      //2    // d4
#define DHTPIN 3       //2    // d4
uint16_t RECV_PIN = 3; // d9
IRsend irsend(1);      // d10
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);
char tempString[6];
char humidString[6];
float tempC, humidQ;
long tTime, mLastTime1 = 0;

/*
uint16_t *code_array;
IRrecv irrecv(RECV_PIN);
decode_results results;  // Somewhere to store the results
    #define TV_OFF        0xa90
    #define HiFi_OFF        0x540a
    #define HiFi_TV        0x540a
    #define HiFi_BT        0x540a
    #define HiFi_Vup        0x240a
    #define HiFi_Vdn        0x640a
    #define L1_ON        0xA010C  // RED
    #define L1_OFF       0x6010C  // GREEN
    #define L2_ON        0xE010C  // YELLOW
    #define L2_OFF       0x2010C  // BLUE
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
//int candleArray[]={candleON,candleOFF,candle4H,candle8H,candleMODE,candleMULTI,candleB1,candleB2,candleB3,candleB4,candleG1,candleG2,candleG3,candleG4,candleG3,candleR1,candleR2,candleR3,candleR4};
//int candleNames[]={'candleON','candleOFF','candle4H','candle8H','candleMODE','candleMULTI','candleB1','candleB2','candleB3','candleB4','candleG1','candleG2','candleG3','candleG4','candleG3','candleR1','candleR2','candleR3','candleR4'};

            void sendNECIr(uint8_t cmd){
                irsend.begin();
                Serial.println("sending "+ cmd);
                irsend.sendNEC(cmd, 32);
                delay(100);
              }

        void sendSonyIr(uint64_t cmd){
        irsend.begin();
        Serial.println("sending Sony");
        irsend.sendSony(cmd, 12, 2);
        delay(100);
        }

        void dump(decode_results *results)
        {
        uint16_t count = results->rawlen;
        if (results->decode_type == UNKNOWN)
        {
            Serial.print("Unknown encoding: ");
        }
        else if (results->decode_type == NEC)
        {
            Serial.print("Decoded NEC: ");
        }
        else if (results->decode_type == SONY)
        {
            Serial.print("Decoded SONY: ");
        }
        else if (results->decode_type == RC5)
        {
            Serial.print("Decoded RC5: ");
        }
        else if (results->decode_type == RC5X)
        {
            Serial.print("Decoded RC5X: ");
        }
        else if (results->decode_type == RC6)
        {
            Serial.print("Decoded RC6: ");
        }
        else if (results->decode_type == PANASONIC)
        {
            Serial.print("Decoded PANASONIC - Address: ");
            Serial.print(results->address, HEX);
            Serial.print(" Value: ");
        }
        else if (results->decode_type == LG)
        {
            Serial.print("Decoded LG: ");
        }
        else if (results->decode_type == JVC)
        {
            Serial.print("Decoded JVC: ");
        }
        else if (results->decode_type == AIWA_RC_T501)
        {
            Serial.print("Decoded AIWA RC T501: ");
        }
        else if (results->decode_type == WHYNTER)
        {
            Serial.print("Decoded Whynter: ");
        }
        serialPrintUint64(results->value, 16);
        Serial.print(" (");
        Serial.print(results->bits, DEC);
        Serial.println(" bits)");
        Serial.print("Raw (");
        Serial.print(count, DEC);
        Serial.print("): ");
        for (uint16_t i = 1; i < count; i++)
        {
            if (i % 100 == 0)
            yield(); // Preemptive yield every 100th entry to feed the WDT.
            if (i & 1)
            {
            //         Serial.print(results.rawbuf[i] * USECPERTICK, DEC);
            }
            else
            {
            Serial.write('-');
            //      Serial.print((uint32_t) results.rawbuf[i] * USECPERTICK, DEC);
            }
            Serial.print(" ");
        }
        Serial.println();
        }


        void doLoop(int on, int lightNo)
        {
        if (on == 1)
        {
            const char EFFECT_COLORLOOP[] = "{\"effect\":\"colorloop\"}";
            String cmd = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/";
            cmd += lightNo;
            cmd += "/state/";
            //  hue.put(cmd, EFFECT_COLORLOOP);
            looping = true;
        }
        if (on == 0)
        {
            const char NO_COLORLOOP[] = "{\"effect\":\"none\"}";
            String cmd = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/";
            cmd += lightNo;
            cmd += "/state/";
            //hue.put(cmd, NO_COLORLOOP);
            looping = false;
        }
        }

*/

const int I2C_DISPLAY_ADDRESS = 0x3c;
const int SDA_PIN = 21; //D2
const int SDC_PIN = 20; //D1
SSD1306 display(0x3c, SDC_PIN, SDA_PIN);
WiFiClient wifiClient;

const int UPDATE_INTERVAL_SECS = 10 * 60; // Update every 10 minutes
const float UTC_OFFSET = 2;

OLEDDisplayUi ui(&display);
TimeClient timeClient(UTC_OFFSET);
bool readyForWeatherUpdate = false;
String lastUpdate = "--";
Ticker ticker;
void drawProgress(OLEDDisplay *display, int percentage, String label);
void drawLux();
void updateData(OLEDDisplay *display);
void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void drawCurrentWeather(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void drawForecast(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void drawThingspeak(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void drawForecastDetails(OLEDDisplay *display, int x, int y, int dayIndex);
void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState *state);
void setReadyForWeatherUpdate();
int8_t getWifiQuality();
int numberOfFrames = 2;
OverlayCallback overlays[] = {drawHeaderOverlay};
int numberOfOverlays = 1;

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
}

void drawProgress(OLEDDisplay *display, int percentage, String label)
{
    display->clear();
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_10);
    display->drawString(64, 10, label);
    display->drawProgressBar(2, 28, 124, 10, percentage);
    display->display();
}
int lux = 0;

void drawLux()
{
    int luxRead = analogRead(LUXPIN);
    lux = (luxRead / 1023);
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 10, "lux= ");
    display.drawProgressBar(2, 28, 124, 10, lux * 100));
    display.display();
}

void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_10);
    String date = "millis";
    int textWidth = display->getStringWidth(date);
    display->drawString(64 + x, 5 + y, date);
    display->setFont(ArialMT_Plain_24);
    String time = millis();
    textWidth = display->getStringWidth(time);
    display->drawString(64 + x, 15 + y, time);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
}

void drawCurrentWeather(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_LEFT);

    display->setFont(ArialMT_Plain_24);

    display->drawString(60 + x, 15 + y, lux);
    int luxWidth = display->getStringWidth(lux);
    display->setFont(Meteocons_Plain_42);

    int weatherIconWidth = display->getStringWidth(luxString);
    display->drawString(32 + x - weatherIconWidth / 2, 05 + y, luxString);
}
FrameCallback frames[] = {drawLux, drawDateTime}; //drawDateTime, drawCurrentWeather, drawForecast, drawThingspeak, drawPast24};

void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState *state)
{
    display->setColor(WHITE);
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawString(0, 54, String(state->currentFrame + 1) + "/" + String(numberOfFrames));
    String time = timeClient.getFormattedTime().substring(0, 5);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(38, 54, time);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(90, 54, tempString);
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

void setup()
{
    Serial.begin(9600);
    Serial3.begin(115200);

    display.init();
    display.clear();
    display.display();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setContrast(255);

    /*    dht.begin();
 //   sensor_t sensor;
 //  dht.temperature().getSensor(&sensor);
  //  Serial.println("------------------------------------");
   // Serial.println("Temperature");
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
    dht.begin();
*/
    Serial.println("Ready");

    ui.setTargetFPS(30);
    ui.setActiveSymbol(activeSymbol);
    ui.setInactiveSymbol(inactiveSymbol);
    ui.setIndicatorPosition(BOTTOM);
    ui.setFrameAnimation(SLIDE_LEFT);
    ui.setFrames(frames, numberOfFrames);
    ui.setOverlays(overlays, numberOfOverlays);
    ui.init();
}

void loop()
{
    if (readyForWeatherUpdate && ui.getUiState()->frameState == FIXED)
    {
        updateData(&display);
    }
    int remainingTimeBudget = ui.update();
    if (remainingTimeBudget > 0)
    {

        delay(remainingTimeBudget);
    }
}