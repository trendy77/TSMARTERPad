#include <Arduino.h>
#line 1 "c:\\Users\\trend\\Documents\\GitHub\\TSMARTPad\\Janefield\\src\\main.cpp"
#line 1 "c:\\Users\\trend\\Documents\\GitHub\\TSMARTPad\\Janefield\\src\\main.cpp"
/*
wemos MEGA ESP
FIRST RUN
*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
// #include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <TimeLib.h>

// Include the correct display library   // For a connection via I2C using Wire include
#include <Wire.h>    // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

#include "OLEDDisplayUi.h"
#include "images.h"

SSD1306 display(0x3c, D3, D5);

  // Wi-Fi / IoT
  const char ssid[] = "Northern Frontier Intertubes";
  const char pass[] = "num4jkha8nnk";
  const int port = 80;                // fix IP of this node
  IPAddress gateway(10, 0, 77, 100);  // WiFi router's IP
  IPAddress subnet(255, 255, 255, 0); //IPAddress ip(192, 68,78,188);

  OLEDDisplayUi ui(&display);

  int screenW = 128;
  int screenH = 64;
  int clockCenterX = screenW / 2;
  int clockCenterY = ((screenH - 16) / 2) + 16; // top yellow part is 16 px height
  int clockRadius = 23;

  // utility function for digital clock display: prints leading 0
  String twoDigits(int digits)
  {
    if (digits < 10)
    {
      String i = '0' + String(digits);
      return i;
    }
    else
    {
      return String(digits);
    }
  }

  void clockOverlay(OLEDDisplay *display, OLEDDisplayUiState *state)
  {
  }

  void analogClockFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
  {
    //  ui.disableIndicator();

    // Draw the clock face
    //  display->drawCircle(clockCenterX + x, clockCenterY + y, clockRadius);
    display->drawCircle(clockCenterX + x, clockCenterY + y, 2);
    //
    //hour ticks
    for (int z = 0; z < 360; z = z + 30)
    {
      //Begin at 0° and stop at 360°
      float angle = z;
      angle = (angle / 57.29577951); //Convert degrees to radians
      int x2 = (clockCenterX + (sin(angle) * clockRadius));
      int y2 = (clockCenterY - (cos(angle) * clockRadius));
      int x3 = (clockCenterX + (sin(angle) * (clockRadius - (clockRadius / 8))));
      int y3 = (clockCenterY - (cos(angle) * (clockRadius - (clockRadius / 8))));
      display->drawLine(x2 + x, y2 + y, x3 + x, y3 + y);
    }

    // display second hand
    float angle = second() * 6;
    angle = (angle / 57.29577951); //Convert degrees to radians
    int x3 = (clockCenterX + (sin(angle) * (clockRadius - (clockRadius / 5))));
    int y3 = (clockCenterY - (cos(angle) * (clockRadius - (clockRadius / 5))));
    display->drawLine(clockCenterX + x, clockCenterY + y, x3 + x, y3 + y);
    //
    // display minute hand
    angle = minute() * 6;
    angle = (angle / 57.29577951); //Convert degrees to radians
    x3 = (clockCenterX + (sin(angle) * (clockRadius - (clockRadius / 4))));
    y3 = (clockCenterY - (cos(angle) * (clockRadius - (clockRadius / 4))));
    display->drawLine(clockCenterX + x, clockCenterY + y, x3 + x, y3 + y);
    //
    // display hour hand
    angle = hour() * 30 + int((minute() / 12) * 6);
    angle = (angle / 57.29577951); //Convert degrees to radians
    x3 = (clockCenterX + (sin(angle) * (clockRadius - (clockRadius / 2))));
    y3 = (clockCenterY - (cos(angle) * (clockRadius - (clockRadius / 2))));
    display->drawLine(clockCenterX + x, clockCenterY + y, x3 + x, y3 + y);
  }

  void digitalClockFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
  {
    String timenow = String(hour()) + ":" + twoDigits(minute()) + ":" + twoDigits(second());
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_24);
    display->drawString(clockCenterX + x, clockCenterY + y, timenow);
  }

  void setup_wifi()
  {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, pass);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      Serial.println("Connection Failed! Rebooting...");
      delay(5000);
      ESP.restart();
    }
  }

  void setup()
  {
    Serial.begin(115200);
    delay(500); // Wait a bit for the serial connection to be established.

    display.init();
    display.flipScreenVertically();
    display.setContrast(255);

    setup_wifi();

    // Port defaults to 8266
    ArduinoOTA.setHostname("mgaaesp");
    //ArduinoOTA.setPort(8189);

    /*
  #if DECODE_HASH
  // Ignore messages with less than minimum on or off pulses.
  irrecv.setUnknownThreshold(MIN_UNKNOWN_SIZE);

  #endif  // DECODE_HASH
  irrecv.enableIRIn();  // Start the receiver
   /* switch on led */
    * /
        Serial.println("Booting");

    ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade
      display.clear();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      display.drawString(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 - 10, "OTA Update");
      display.display();
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      display.drawProgressBar(4, 32, 120, 8, progress / (total / 100));
      display.display();
    });

    ArduinoOTA.onEnd([]() {
      display.clear();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      display.drawString(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, "Restart");
      display.display();
    });

    // Align text vertical/horizontal center
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.setFont(ArialMT_Plain_10);
    display.drawString(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, "Ready for OTA:\n" + WiFi.localIP().toString());
    display.display();

    ArduinoOTA.onError([](ota_error_t error) { ESP.restart(); });

    /* setup the OTA server */
    ArduinoOTA.begin();
    Serial.println("Ready");
  }

  void loop()
  {
    ArduinoOTA.handle();

    /*
  if (irrecv.decode(&results)) {
        // Display a crude timestamp.
    uint32_t now = millis();
    Serial.printf("Timestamp : %06u.%03u\n", now / 1000, now % 1000);
    if (results.overflow)
      Serial.printf("WARNING: IR code is too big for buffer (>= %d). "
                    "This result shouldn't be trusted until this is resolved. "
                    "Edit & increase CAPTURE_BUFFER_SIZE.\n",
                    CAPTURE_BUFFER_SIZE);
    // Display the basic output of what we found.
    Serial.print(resultToHumanReadableBasic(&results));
    dumpACInfo(&results);  // Display any extra A/C info if we have it.
    yield();  // Feed the WDT as the text output can take a while to print.
  // Display the library version the message was captured with.
    Serial.print("Library   : v");
    Serial.println(_IRREMOTEESP8266_VERSION_);
    Serial.println();
    // Output RAW timing info of the result.
    Serial.println(resultToTimingInfo(&results));
    yield();  // Feed the WDT (again)
    // Output the results as source code
    Serial.println(resultToSourceCode(&results));
    Serial.println("");  // Blank line between entries
    yield();  // Feed the WDT (again)
  }
*/
    Serial.println("."); // Blank line between entries
  }

