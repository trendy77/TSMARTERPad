#include <dht.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRtimer.h>
#include <IRutils.h>

// TIME, INTERNAL VARS
long lastChk, lastPrint, timeGone = 0;
char msg[50];
// Temperature AND HUMIDITY VARS
int value, temp, humid = 0;
dht DHT;
#define DHT11_PIN 14 //(D5)
char temperatureString[6];
char humidString[6];
// CONNECTIVITY
const char* ssid = "WiFi2";
const char* pass = "4328646518";
const char* mqtt_server = "iot.eclipse.org";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
//char msg[50];
// INFRARED RECIEVER
uint16_t RECV_PIN = 4;
IRrecv irrecv(RECV_PIN);
decode_results results;

void reconnect() {
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "node189";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str()))
    {
    Serial.println("connected");
    client.subscribe("irComms");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  //server_start(0);
  irrecv.enableIRIn();  // Start the receiver
   delay(2000);
}

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

void getTemperature() {
  int chk = DHT.read11(DHT11_PIN);
  temp = DHT.temperature;
  humid = DHT.humidity;
Serial.print("int Temperature = ");
  Serial.println(temp);
  Serial.print("int Humidity = ");
  Serial.println(humid);
  Serial.println(" ");
  dtostrf(temp,2,2 , temperatureString);
  dtostrf(humid,2,2 , humidString);
   client.publish("temp", temperatureString);
   client.publish("humid",humidString);
    }

  void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
  }

void loop() {
   if (!client.connected()) {
     reconnect();
   }
   client.loop();
   long now = millis();
     if (irrecv.decode(&results))
     {
       String msg="Received value: ";
       uint64_t i = results.value;
      char buffer [sizeof(uint64_t)*8+1];
      ulltoa(i,buffer,HEX);
        Serial.println(thi);
        char message[58];
    //    msg1.toCharArray(message,58);
        Serial.println(message);
       //client.publish("IRRec", message);
  Serial.println("");
    irrecv.resume();
    // Receive the next value
  delay(100);
  }
  if (millis() - now > 30000) {
    getTemperature();
    now = millis();
  }
}
