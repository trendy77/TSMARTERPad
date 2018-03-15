#include <Arduino.h>

// 18 / 03 /18
// nodeMCU thingspeak hueControl.

// ota updating
// d2 / gpio 4 --- temp DHT11
// d1 / gp 5 --
// d6 / gp12  --
//d7
//d8
//d9
//d10
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <RestClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ThingSpeak.h>
#include <IRremoteESP8266.h>


/*

    int rgb_pins[] = {14, 13, 15};
    #define Red 14     // d5...gp 14
    #define Green 13    // D7.. gp 13
    #define Blue 15     // .D8 .. gp 15

*/                 ////////////////////////
#define RECV_PIN 0 /// d1 ...
#define SEND_PIN 0 /// d6?
//////////////////////////
#define LEDPIN 16 // Led in NodeMCU at pin GPIO16 (D0).
//#define DHTPIN 9
#define DHTPIN 4 // d2
//////////////////////
//const char ssid[] = "TPG 15EA";
//const char pass[] = "abcd1989";
/////////////////////
const char ssid[] = "Northern Frontier Interwebs";
const char pass[] = "num4jkha8nnk";

IPAddress gateway(10, 0, 77, 100);
IPAddress ip(10, 0, 77, 112);
IPAddress subnet(255, 255, 255, 192);

ESP8266WebServer server(80);
int Tfield = 3;
int Hfield = 4;
unsigned long myChannelNumber = 404585;
const char *myWriteAPIKey = "W124WS7UN76VCASZ";
int value = 0;
char temperatureString[6];
char humidString[6];
float temp, humid;
WiFiClient client;
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
const char *bridge_ip = "10.0.77.101";
RestClient hue = RestClient(bridge_ip);
const char *host = "nodeEsp112";
float maxT, minT, maxH, minH, prevT, prevH;
long t = 0;

#define BRIGHT 350  //max led intensity (1-500)
#define INHALE 1250 //Inhalation time in milliseconds.
#define PULSE INHALE * 1000 / BRIGHT
#define REST 1000 //Rest Between Inhalations.

IRrecv irrecv(RECV_PIN);
decode_results results;
IRsend irsend(SEND_PIN);
const char LIGHTS_ON[] = "{\"on\":true}";
const char LIGHTS_OFF[] = "{\"on\":false}";
bool looping;
unsigned long mLastTime1, mLastTime, holdup = 0;
bool held = false;

/*
void setColor(long red, long green, long blue, int wai) {
  held = true;
  holdup = ((wai * 1000) + millis());
  analogWrite(Red, red);
  analogWrite(Green, green);
  analogWrite(Blue, blue);
}
*/

void ledLoop()
{
    for (int i = 1; i < BRIGHT; i++)
    {
        digitalWrite(LEDPIN, LOW);         // turn the LEDPIN on.
        delayMicroseconds(i * 10);         // wait
        digitalWrite(LEDPIN, HIGH);        // turn the LEDPIN off.
        delayMicroseconds(PULSE - i * 10); // wait
        delay(0);                          //to prevent watchdog firing.
    }
    //ramp decreasing intensity, Exhalation (half time):
    for (int i = BRIGHT - 1; i > 0; i--)
    {
        digitalWrite(LEDPIN, LOW);         // turn the LEDPIN on.
        delayMicroseconds(i * 10);         // wait
        digitalWrite(LEDPIN, HIGH);        // turn the LEDPIN off.
        delayMicroseconds(PULSE - i * 10); // wait
        i--;
        delay(0); //to prevent watchdog firing.
    }
}

// web comms
void handleRoot()
{
    String message = "<html><head><title>NodeIR TPad</title></head><body><h1>Temp Outside</h1>";
    message += "<h2>Temp =";
    message += temperatureString;
    message += "</h2>";
    message += "<h2>Humid =";
    message += humidString;
    message += "</h2><br><h2>LoungeCommands</h2><p><a href=\"tvoff\">Send TV On/Off</a></p><p><a href=\"volup\">Send VolUP</a></p><p><a href=\"voldn\">Send VolDN</a></p><p><a href=\"tvaux\">HiFi TV AUX</a></p><p><a href=\"btaux\">HiFi BT AUX</a></p></body></html>";
    server.send(200, "text/html", message);
}
void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    server.send(404, "text/plain", message);
    Serial.print(message);
}

/*
void sendSonyIr(uint64_t cmd) {
  irsend.begin();
  Serial.println("sending Sony");
  irsend.sendSony(cmd, 12, 2);
  delay(200);
  irrecv.resume();
}

void dump(decode_results *results) {
  uint16_t count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  } else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  } else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  } else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  } else if (results->decode_type == RC5X) {
    Serial.print("Decoded RC5X: ");
  } else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  } else if (results->decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->address, HEX);
    Serial.print(" Value: ");
  } else if (results->decode_type == LG) {
    Serial.print("Decoded LG: ");
  } else if (results->decode_type == JVC) {
    Serial.print("Decoded JVC: ");
  } else if (results->decode_type == AIWA_RC_T501) {
    Serial.print("Decoded AIWA RC T501: ");
  } else if (results->decode_type == WHYNTER) {
    Serial.print("Decoded Whynter: ");
  }
  serialPrintUint64(results->value, 16);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");
  for (uint16_t i = 1; i < count; i++) {
    if (i % 100 == 0)
      yield();  // Preemptive yield every 100th entry to feed the WDT.
    if (i & 1) {
      //         Serial.print(results.rawbuf[i] * USECPERTICK, DEC);
    } else {
      Serial.write('-');
      //      Serial.print((uint32_t) results.rawbuf[i] * USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println();
}
**/

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

void getTemperature()
{
    // Get temperature event and print its value.
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
        Serial.println("Error reading temperature!");
    }
    else
    {
        temp = event.temperature;
        if (temp != prevT)
        {
            Serial.print("Temperature: ");
            Serial.print(event.temperature);
            Serial.println(" *C");
            if (temp > maxT)
            {
                maxT = temp;
            }
            else if (temp < minT)
            {
                minT = temp;
            }
            prevT = temp;
        }
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity))
    {
        Serial.println("Error reading humidity!");
    }
    else
    {
        humid = event.relative_humidity;
        if (humid != prevH)
        {
            Serial.print("Humidity: ");
            Serial.print(event.relative_humidity);
            Serial.println("%");
            if (humid > maxH)
            {
                maxH = humid;
                Serial.print("Humidity@newMax");
            }
            else if (humid < minH)
            {
                minH = humid;
                Serial.print("Humidity@newMin");
            }
            prevH = humid;
        }
    }
    ThingSpeak.setField(Tfield, temp);
    ThingSpeak.setField(Hfield, humid);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    delay(500);
}

void setup_wifi()
{
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup()
{
    Serial.begin(115200);
    pinMode(LEDPIN, OUTPUT);
    //for (int i=0; i<3; i++){
    //pinMode(rgb_pins[i], OUTPUT);
    // digitalWrite(rgb_pins[i],LOW);
    //}
    setup_wifi();
    ArduinoOTA.setHostname(host);

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
    ThingSpeak.begin(client);

    server.on("/", handleRoot); //server.on("/volup", volup); server.on("/btaux", btaux); server.on("/tvaux", tvaux); server.on("/voldn", voldn); server.on("/tvoff", [](){ sendIr(TV_OFF); server.send(200, "text/plain", "tv OFF"); });
    server.onNotFound(handleNotFound);

    ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade
        analogWrite(LEDPIN, 0);
    });

    ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
        for (int i = 0; i < 30; i++)
        {
            analogWrite(LEDPIN, (i * 100) % 1001);
            delay(50);
        }
    });

    ArduinoOTA.onError([](ota_error_t error) { ESP.restart(); });
    ArduinoOTA.begin();
    //irrecv.enableIRIn();
    Serial.println("setup complete.");
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
    delay(100);
}

void loop()
{
    ArduinoOTA.handle();
    unsigned long time = millis();
    //if (held == true) {
    // if (holdup > 0) {
    //holdup=holdup-time;
    //  } else {
    //setColor(0,0,1000,2);
    // }
    // }
    /*    if (irrecv.decode(&results))
    {
      dump(&results);
     irrecv.resume();
      }
*/
    server.handleClient();
    if ((millis() - mLastTime1) >= 30000)
    {
        mLastTime1 = millis();
        getTemperature();
    }

    delay(10);
}
