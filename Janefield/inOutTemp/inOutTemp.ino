// 18 / 01 /18
// nodeMCU thingspeak client with IR send/receive support and webserver for Temp /Humidity checking live on the fly. from anywhere.
// digital 2 / gpio 4 --- temp DHT11
// dig 1 / gp 5 -- ir rec @ (d1)---gpio 5 ?
// d6 / gp12  -- ir sender

#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <RestClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <WiFiClient.h>

// COMMENT OUT DEPENDING ON WHICH ONE....
//////////////////////
// inside
//IPAddress ip(192, 168, 78, 188); ESP8266WebServer server(9188);  int Tfield = 1;   int Hfield = 2;const char* host = "nodeinside";
// Outside
IPAddress ip(192, 168, 78, 189);
ESP8266WebServer server(9189);
int Tfield = 3;
int Hfield = 4;
const char *host = "nodeoutside";
float prevTemp;
long t = 0;
int rgb_pins[] = {14, 13, 15};
#define Red 14   // d5...gp 14
#define Green 13 // D7.. gp 13
#define Blue 15  // .D8 .. gp 15

// Wi-Fi / IoT
const char ssid[] = "TPG 15EA";
const char password[] = "abcd1989";
const int port = 80;                 // fix IP of this node
IPAddress gateway(192, 168, 78, 78); // WiFi router's IP
IPAddress subnet(255, 255, 255, 0);
WiFiClient client;
unsigned long mLastTime1, mLastTime, holdup = 0;
bool held = false;

void setColor(long red, long green, long blue, int wai)
{
  held = true;
  holdup = ((wai * 1000) + millis());
  analogWrite(Red, red);
  analogWrite(Green, green);
  analogWrite(Blue, blue);
}
void lightLoop()
{
  for (int i = 1; i < BRIGHT; i++)
  {
    digitalWrite(LED, LOW);            // turn the LED on.
    delayMicroseconds(i * 10);         // wait
    digitalWrite(LED, HIGH);           // turn the LED off.
    delayMicroseconds(PULSE - i * 10); // wait
    delay(0);                          //to prevent watchdog firing.
  }
  //ramp decreasing intensity, Exhalation (half time):
  for (int i = BRIGHT - 1; i > 0; i--)
  {
    digitalWrite(LED, LOW);            // turn the LED on.
    delayMicroseconds(i * 10);         // wait
    digitalWrite(LED, HIGH);           // turn the LED off.
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

void setup_wifi()
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.config(ip, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(500);
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  for (int i = 0; i < 3; i++)
  {
    pinMode(rgb_pins[i], OUTPUT);
    digitalWrite(rgb_pins[i], LOW);
  }
  setup_wifi();
  ArduinoOTA.setHostname(host);
  // turn of leds
  digitalWrite(Red, HIGH);
  digitalWrite(Green, HIGH);
  digitalWrite(Blue, HIGH);
  /* configure dimmers, and OTA server events */
  analogWriteRange(1000);
  analogWrite(Blue, 990);

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

  //server.on("/", handleRoot); //server.on("/volup", volup); server.on("/btaux", btaux); server.on("/tvaux", tvaux); server.on("/voldn", voldn); server.on("/tvoff", [](){ sendIr(TV_OFF); server.send(200, "text/plain", "tv OFF"); });
  //server.onNotFound(handleNotFound); server.begin();
  delay(100);
  irrecv.enableIRIn();
  Serial.println("setup complete.");
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
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
  if (irrecv.decode(&results))
  {
    dump(&results);
    irrecv.resume();
  }
  //server.handleClient();
  if ((time - mLastTime1) >= 30000)
  {
    mLastTime1 = millis();
    getTemperature();
    if (temp != prevTemp)
    {
      if (temp > prevTemp)
      {
        analogWrite(Red, 1000);
      }
      else if (temp < prevTemp)
      {
        analogWrite(Green, 1000);
      }
      prevTemp = temp;
    }
  }
  delay(10);
}
