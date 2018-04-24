/*
C:\Users\trend\Documents\PlatformIO\Projects\180109-153921-megaatmega2560\src

ALFA TURISMO GT MACH II
KNIGHT RIDER GRILLE

*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN            A15
#define NUMPIXELS      8
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int delayval = 500;
long lastTime1,lastTime2,time=0;

/**  TEMPC AND HUMIDITY
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
float prevTemp;
long t = 0;
uint32_t delayMS;
#define DHTPIN     22   // d2
#define DHTTYPE           DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);
unsigned long myChannelNumber = 404585;
const char * myWriteAPIKey = "W124WS7UN76VCASZ";
int value = 0;
char temperatureString[6]; char humidString[6]; float temp, humid;

  void getTemperature() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    temp = event.temperature;
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
    humid = event.relative_humidity;
    Serial.println("%");
  }
  dtostrf(temp, 2, 2 , temperatureString);
  dtostrf(humid, 2, 2 , humidString);
 //ThingSpeak.setField(Tfield, temp);
 //ThingSpeak.setField(Hfield, humid);
 //ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}
  dht.begin();
  sensor_t sensor; dht.temperature().getSensor(&sensor); Serial.println("------------------------------------"); Serial.println("Temperature");  Serial.print  ("Sensor:       "); Serial.println(sensor.name);  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version); Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C"); Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C"); Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C"); Serial.println("------------------------------------");
  dht.humidity().getSensor(&sensor); Serial.println("------------------------------------");  Serial.println("Humidity");  Serial.print  ("Sensor:       "); Serial.println(sensor.name);  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id); Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  Serial.println("------------------------------------");
  delayMS = sensor.min_delay / 1000;
  myReceiver.enableIRIn(); // Start the receiver
*/



  void clearStrip() {
    for( int i = 0; i<NUMPIXELS; i++){
      strip.setPixelColor(i, 0x000000); strip.show();
    }
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

  uint32_t dimColor(uint32_t color, uint8_t width) {
       return (((color&0xFF0000)/width)&0xFF0000) + (((color&0x00FF00)/width)&0x00FF00) + (((color&0x0000FF)/width)&0x0000FF);
    }

  void knightRider(uint16_t cycles, uint16_t speed, uint8_t width, uint32_t color) {
      uint32_t old_val[8]; // up to 256 lights!
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
}

    // Using a counter and for() loop, input a value 0 to 251 to get a color value.
    // The colors transition like: red - org - ylw - grn - cyn - blue - vio - mag - back to red.
    // Entering 255 will give you white, if you need it.

  void goSolid(int colour){
  switch (colour){
  case '1':
  for(int i=0;i<NUMPIXELS;i++){
  // strip.Color takes RGB values, from 0,0,0 up to 255,255,255
    strip.setPixelColor(i, strip.Color(200,0,0)); // Moderately bright green color.
    strip.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).
  }
  break;
  case '2':
  for(int i=0;i<NUMPIXELS;i++){
  // strip.Color takes RGB values, from 0,0,0 up to 255,255,255
    strip.setPixelColor(i, strip.Color(0,200,0)); // Moderately bright green color.
    strip.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).
  }
  break;
  case '3':
  for(int i=0;i<NUMPIXELS;i++){
  // strip.Color takes RGB values, from 0,0,0 up to 255,255,255
    strip.setPixelColor(i, strip.Color(0,0,200)); // Moderately bright green color.
    strip.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).
  }
  break;
  }
}

void kitt(){
    knightRider(1, 46, 4, 0xFF1000); // Cycles, Speed, Width, RGB Color (original orange-red)
    knightRider(1, 46, 3, 0xFF00FF); // Cycles, Speed, Width, RGB Color (purple)
      knightRider(1, 46, 4, 0xFF1000); // Cycles, Speed, Width, RGB Color (original orange-red)
    knightRider(1, 46, 2, 0x0000FF); // Cycles, Speed, Width, RGB Color (blue)
      knightRider(1, 46, 4, 0xFF1000); // Cycles, Speed, Width, RGB Color (original orange-red)
    knightRider(1, 46, 5, 0xFF0000); // Cycles, Speed, Width, RGB Color (red)     knightRider(1, 46, 6, 0x00FF00); // Cycles, Speed, Width, RGB Color (green)
    knightRider(1, 46, 7, 0xFFFF00); // Cycles, Speed, Width, RGB Color (yellow)
      knightRider(1, 46, 4, 0xFF1000); // Cycles, Speed, Width, RGB Color (original orange-red)
      knightRider(1, 46, 8, 0x00FFFF); // Cycles, Speed, Width, RGB Color (cyan)
    knightRider(1, 46, 4, 0xFF1000); // Cycles, Speed, Width, RGB Color (original orange-red)
    knightRider(1, 46, 2, 0xFFFFFF); // Cycles, Speed, Width, RGB Color (white)
    clearStrip();
    delay(1000);
}

void copMode()
{
  byte width = 2;
  for (byte jj = 0; jj < 10; jj++){
    for (byte j = 0; j < 150; j += 7)
    {
      knightRider(1, j, width, 0xFF0000); // Cycles, Speed, Width, RGB Color red
        }
    for (byte j = 0; j < 150; j += 7)
    {
      knightRider(1, j, width, 0x0000FF); // Cycles, Speed, Width, RGB Color blue
    }
  }
  delay(delayval);
  for (int i = 0; i < NUMPIXELS; i + 2)
  {
    // strip.Color takes RGB values, from 0,0,0 up to 255,255,255
    strip.setPixelColor(i, strip.Color(200, 0, 0)); // Moderately bright green color.
  }
    for (int i = 1; i < NUMPIXELS; i + 2)
    {
      // strip.Color takes RGB values, from 0,0,0 up to 255,255,255
      strip.setPixelColor(i, strip.Color(0, 0, 200)); // Moderately bright green color.
    }
      strip.show();    // This sends the updated pixel color to the hardware.

      delay(delayval); // Delay for a period of time (in milliseconds).
      for (byte j = 0; j < 150; j += 7)
      {
        knightRider(2, j,width, ); // Cycles, Speed, Width, RGB Color
        width++;
      }
      for (int i = 0; i < NUMPIXELS; i + 2)
      {
        // strip.Color takes RGB values, from 0,0,0 up to 255,255,255
        strip.setPixelColor(i, strip.Color(200, 0, 0)); // Moderately bright green color.
      }
      for (int i = 1; i < NUMPIXELS; i + 2)
      {
        // strip.Color takes RGB values, from 0,0,0 up to 255,255,255
        strip.setPixelColor(i, strip.Color(0, 0, 200)); // Moderately bright green color.
      }
      strip.show(); // This sends the updated pixel color to the hardware.
      delay(delayval);

      clearStrip();
}
// Cycles - one


void rainBow()
{
  byte width = 2;
  for (byte j = 0; j < 150; j += 7)
  {                                          // Iterate through a whole rainbow of colors
    knightRider(1, j, width, colorWheel(j)); // Cycles, Speed, Width, RGB Color
    width++;
    }
    clearStrip();
  }
  // Cycles - one cycle is scanning through all strip left then right (or right then left)
  // Speed - how fast one cycle is (46 with 16 strip is default KnightRider speed)
  // Width - how wide the trail effect is on the fading out LEDs.  The original display used   light bulbs, so they have a persistance when turning off.  This creates a trail. Effective range is 2 - 8, 4 is default for 16 strip.  Play with this.
  // Color - 46-bit packed RGB color value.  All strip will be this color.
  // knightRider(cycles, speed, width, color);

void setup() {
 // pinMode(A0, OUTPUT);      // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  Serial.begin(115200);
  Serial.print("booting");
  Serial3.begin(9600);
  Serial3.println("1");
  strip.begin(); // This initializes the NeoPixel library.
}

void loop() {
  time = millis();
   if (Serial.available())
    {
      char dato = Serial.read();
    switch (dato){
      case '1':  
      goSolid(1); brea1k;
      case '2':
        goSolid(2);
        break;
      case '3':
        goSolid(3);
        break;
      case '4':
        knightRider(4, 46, 3, 0x0000FF); // Cycles, Speed, Width, RGB Color (blue)
        break;
      case '5':
        knightRider(4, 46, 5, 0xFF0000);  // red
        break;
      case '6':
        knightRider(4, 46, 3, 0xFFFF00); // Cycles, Speed, Width, RGB Color (yellow)
        break;
      case '7':
        knightRider(4, 46, 4, 0xFF1000); // Cycles, Speed, Width, RGB Color (original orange-red) 
         break;
      case '8':
        knightRider(4, 46, 2, 0xFFFFFF); // Cycles, Speed, Width, RGB Color (white)
         break;
      case '9':
        clearStrip();
        break;
      case 'c':
        copMode();
        break;
      }  
    } else if (Serial3.available())
      {
      char dato2 = Serial3.read();
      String lah = ""; 
      lah += dato2;
      if (dato2 == '/n'){
      Serial.write(lah);
      }
     } 
      if (time >= (lastTime2 + 300000))
      { // every 60 secs
        rainBow();
      }
      if (time >= (lastTime1 + 600000))
      { // every 100 secs
        kitt();
        lastTime1 = time;
   }

}
