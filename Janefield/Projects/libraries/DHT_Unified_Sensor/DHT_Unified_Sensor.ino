#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
// COMMENT OUT DEPENDING ON WHICH ONE....
// inside
#define DHTPIN 28

int rgb_pins[] = {A8, A9, A10};
#define Red A8   // d5...gp 14
#define Green A9 // D7.. gp 13
#define Blue A10 // .D8 .. gp 15
void setColor(long red, long green, long blue, int wai)
{
  holdup = ((wai * 1000) + millis());
  analogWrite(Red, red);
  analogWrite(Green, green);
  analogWrite(Blue, blue);
}
void setup()
{
  Serial.begin(115200);
  dht.begin();
  for (int i = 0; i < 3; i++)
  {
    pinMode(rgb_pins[i], OUTPUT);
    digitalWrite(rgb_pins[i], LOW);
  }
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
}
unsigned long mLastTime1;
void loop()
{
  //server.handleClient();
  //if (irrecv.decode(&results)) {
  //dump(&results);
  // irrecv.resume();

  if ((millis() - mLastTime1) >= 30000)
  {
    //   digitalWrite(ledPin, HIGH);
    mLastTime1 = millis();
    getTemperature();
  }
}
