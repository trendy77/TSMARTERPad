#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN            9

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>

#define DHTTYPE           DHT11     
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

/*
  Serial.begin(115200); 
  // Initialize device.
  dht.begin();
  Serial.println("DHTxx Unified Sensor Example");
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
  Serial.println("------------------------------------");
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

// COMMENT OUT DEPENDING ON WHICH ONE....
*/
// inside
// ESP8266WebServer server(9188);  IPAddress ip(192, 168, 78, 188);    int Tfield = 1;   int Hfield = 2;   int dhPin = 2;

// Outside
 int dhPin = 9;  IPAddress ip(10,0,77,32); ESP8266WebServer server(9189); int Tfield = 3;  int Hfield = 4;
//////////////////////

unsigned long myChannelNumber = 404585;
const char * myWriteAPIKey = "W124WS7UN76VCASZ";

//byte ledPin = 2;
// TEMPC AND HUMIDITY
int value = 0;
char temperatureString[6];
char humidString[6];
float temp, humid;

// IR
uint16_t RECV_PIN = 0;   // IR detector @ GPIO pin 5 (d1)
IRsend irsend(4);  // d6  ......  gpio 5=pin d1
IRrecv irrecv(RECV_PIN);
decode_results results;

//  comands
#define TV_OFF        0xa90
#define HiFi_OFF        0x540a
#define HiFi_TV        0x540a
#define HiFi_BT        0x540a
#define HiFi_Vup        0x240a
#define HiFi_Vdn        0x640a
#define L1_ON        0x640a
#define L1_OFF      0x640a
#define L2_ON        0x640a
#define L2_OFF        0x640a
// HUE LIGHTING
const char LIGHTS_ON[] = "{\"on\":true}";
const char LIGHTS_OFF[] = "{\"on\":false}";
const char *bridge_ip = "192.168.0.101"; // Hue Bridge IP
bool looping;

// Wi-Fi / IoT
const char ssid[] = "TPG 15EA";
const char password[] = "abcd1989";
const int port = 80;   // fix IP of this node
IPAddress gateway(192,168,78,78);     // WiFi router's IP
IPAddress subnet(255,255,255,0);
//  RestClient hue = RestClient(bridge_ip);
WiFiClient client;
unsigned long mLastTime1, mLastTime=0;


void doLoop(int on, int lightNo){
  if (on == 1){
  const char EFFECT_COLORLOOP[] = "{\"effect\":\"colorloop\"}";
    String cmd = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/";
    cmd += lightNo;   cmd += "/state/";
  //  hue.put(cmd, EFFECT_COLORLOOP);
    looping = true;
  }
  if (on == 0){
  const char NO_COLORLOOP[] = "{\"effect\":\"none\"}";
    String cmd = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/";
    cmd += lightNo;   cmd += "/state/";
    //hue.put(cmd, NO_COLORLOOP);
    looping = false;
  }
}

void getTemperature(){
   // Get temperature event and print its value.
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" *C");
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println("%");
  }
   ThingSpeak.setField(Tfield,event.temperature);ThingSpeak.setField(Hfield,event.relative_humidity);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  delay(500);
}

void sendIr(uint64_t cmd){
  irsend.begin();
  Serial.println("sending Sony");
  irsend.sendSony(cmd, 12, 2);
  delay(100);
}

void handleRoot() {
  String message = "<html><head><title>NodeIR TPad</title></head><body><h1>Temp Outside</h1>";
  message += "<h2>Temp =";
  message += temperatureString;
  message += "</h2>";
  message += "<h2>Humid =";
  message += humidString;
  message +=  "</h2>";
  message += "</h2><br><h2>LoungeCommands</h2><p><a href=\"tvoff\">Send TV On/Off</a></p><p><a href=\"volup\">Send VolUP</a></p><p><a href=\"voldn\">Send VolDN</a></p><p><a href=\"tvaux\">HiFi TV AUX</a></p><p><a href=\"btaux\">HiFi BT AUX</a></p></body></html>";
  server.send(200, "text/html", message);
}

void volup() {
  sendIr(HiFi_Vup);
}
void voldn() {
  sendIr(HiFi_Vdn);
}
void tvaux() {
  sendIr(HiFi_TV);
}
void tvoff(){
  sendIr(TV_OFF);
}
void btaux(){
  sendIr(HiFi_BT);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
  Serial.print(message);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
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
SoftwareSerial newSer(21,20);

void setup() {
  Serial.begin(115200);
  setup_wifi();
//  pinMode(ledPin, OUTPUT);
  ThingSpeak.begin(client);
  server.on("/", handleRoot);
  server.on("/volup", volup);
  server.on("/btaux", btaux);
  server.on("/tvaux", tvaux);
  server.on("/voldn", voldn);
  server.on("/tvoff", [](){
  sendIr(TV_OFF);
  server.send(200, "text/plain", "tv OFF");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  delay(100);
  irrecv.enableIRIn();  // Start the receiver
}

void loop() {
server.handleClient();
  if (irrecv.decode(&results)) {
    dump(&results);
    irrecv.resume();
    delay(100);
  }
  if ((millis() - mLastTime) >= 1000) {
    mLastTime = millis();
  //   digitalWrite(ledPin, LOW);
     // sendIr(HiFi_Vup);
  }
  if ((millis() - mLastTime1) >= 30000) {
  //   digitalWrite(ledPin, HIGH);
     mLastTime1 = millis();
     getTemperature();
    }
}
