
/*
THIS FILE IS

C:\Users\trend\Documents\PlatformIO\Projects\171111-175952-nodemcuv2\src

 */

#ifndef UNIT_TEST
#include <Arduino.h>
#endif
//
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ThingSpeak.h>
// web n wifi
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <RestClient.h>
// infrared
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>

// COMMENT OUT DEPENDING ON WHICH ONE....
IPAddress ip(10, 77, 0, 34);
ESP8266WebServer server(3434);
int Tfield = 3;
int Hfield = 4;
const char *host = "nodeoutside";
IPAddress gateway(10, 77, 0, 100);
uint16_t RECV_PIN = 4;
Rsend irsend(5);

byte ledPin = 0;
const char ssid[] = "Northern Frontier Intertubes";
const char pass[] = "num4jkha8nnk";

// TEMPC AND HUMIDITY
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);
unsigned long myChannelNumber = 404585;
const char *myWriteAPIKey = "W124WS7UN76VCASZ";
int value = 0;
char temperatureString[6];
char humidString[6];
float temp, humid;
float prevTemp;
long t = 0;
WiFiClient wiFi;
int incomingByte = 0;
MDNSResponder mdns;
uint16_t *code_array;
IRrecv irrecv(RECV_PIN);
decode_results results; // Somewhere to store the results
irparams_t save;        // A place to copy the interrupt state while decoding.

/*
IR STUFF....
#define TV_OFF        0xa90
#define HiFi_OFF        0x540a
#define HiFi_TV        0x540a
#define HiFi_BT        0x540a
#define HiFi_Vup        0x240a
#define HiFi_Vdn        0x640a
#define L1_ON        0xA010C	// RED
#define L1_OFF    	 0x6010C	// GREEN
#define L2_ON        0xE010C	// YELLOW
#define L2_OFF       0x2010C 	// BLUE
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
  IRsend irsend(4);

            void sendNECIr(uint8_t cmd){
                irsend.begin();
                Serial.println("sending "+ cmd);
                irsend.sendNEC(cmd, 32);
                delay(100);
              }

void sendIr(uint64_t cmd){
  irsend.begin();
  Serial.println("sending Sony");
  irsend.sendSony(cmd, 12, 2);
  delay(100);
}

RestClient hue = RestClient(bridge_ip);
const char LIGHTS_ON[] = "{\"on\":true}";
const char LIGHTS_OFF[] = "{\"on\":false}";
const char *bridge_ip = "192.168.0.101"; // Hue Bridge IP
const int port = 80;

void hueturnOff1(){
  hue.put("/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/1/state/", LIGHTS_OFF);
}
void hueturnOn1(){
  hue.put("/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/1/state/", LIGHTS_ON);
}
  void hue_turnOff(int lightNo){
    String cmd = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/";
    cmd += lightNo;
    cmd += "/state/";  int buf = cmd.length();
    char buffer[buf];
    cmd.toCharArray(buffer, buf);
    hue.put(buffer, LIGHTS_OFF);
}
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
        temp = event.temperature;
        Serial.println(" *C");
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity))
    {
        Serial.println("Error reading humidity!");
    }
    else
    {
        Serial.print("Humidity: ");
        Serial.print(event.relative_humidity);
        humid = event.relative_humidity;
        Serial.println("%");
    }
    dtostrf(temp, 2, 2, temperatureString);
    dtostrf(humid, 2, 2, humidString);
    ThingSpeak.setField(Tfield, temp);
    ThingSpeak.setField(Hfield, humid);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("node begin");
    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(900);
        Serial.print(".");
    }
    server.begin();
    IPAddress myAddress = WiFi.localIP();
    Serial.println(myAddress);
    if (mdns.begin(HOST_NAME, WiFi.localIP()))
    {
        Serial.println("MDNS responder started");
    }
    MDNS.addService("telnet", "tcp", 23);
    //irsend.begin();
    //pinMode(pirPin, INPUT);

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
   
    ThingSpeak.begin(clientTH);

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);

    ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade
        for (int i = 0; i < 3; i++)
            analogWrite(rgb_pins[i], 0);
    });
    ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
        for (int i = 0; i < 30; i++)
        {
            analogWrite(Blue, (i * 100) % 1001);
            delay(50);
            analogWrite(Green, (1 - (i * 100) % 1001));
        }
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
            Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
            Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
            Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
            Serial.println("End Failed");
    });

    ArduinoOTA.begin();

    irrecv.enableIRIn();
    delay(500);
}

void loop()
{
    unsigned long time = millis();
    ArduinoOTA.handle();

    WiFiClient client = server.available();

    if (irrecv.decode(&results))
    {
        dump(&results);
        irrecv.resume();
    }

    if (Serial.available() > 0)
    {
        incomingByte = Serial.read();
        msg += incomingByte;
        if (incomingByte == '*')
        {
        }

        /*  switch(incomingByte){
      case '1':
      hueturnOn1();
      break;
      case '2':
      hueturnOff1();
      break;
      case '3':
      hueturnOn1();
      break;
      case '4':
      hueturnOff1();
      break;
    }
  }
    */
        if (client)
        {
            if (client.connected())
            {
                digitalWrite(ledPin, LOW);              // to show the communication only (inverted logic)
                request = client.readStringUntil('\r'); // receives the message from the client
                Serial.println(request);
                client.flush();
                client.println(msg);
                digitalWrite(ledPin, HIGH);
            }
            client.stop();
        }

        if ((time - mLastTime1) >= 30000)
        {
            mLastTime1 = millis();
            getTemperature();
        }
    }
