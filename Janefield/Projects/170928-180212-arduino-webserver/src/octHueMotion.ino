// LOUNGE TEMPc & HUMIDITY SENSOR +
//  UPDATES TO MQTT / THINGSPEAK CHANNEL
// https://thingspeak.com/channels/339378
// SEE THE WEBPAGE @
// https://thingspeak.com/apps/matlab_visualizations/36336
// https://thingspeak.com/apps/matlab_visualizations/177957
//      thingSpeakArea(temperatureData, humidityData,'xlabel', 'Temperature', 'ylabel', 'Humidity','LineStyle','-.','LineWidth',2.5)
// IR RECEIVER TO LOG INFRARED SIGNALES INTERCEPTED...
//TNODE - IR REC
//      - IR WEB SONY SEND + CANDLE NEC COMMS
//          NEC

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
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <ESP8266WiFi.h>
#include <RestClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <dht.h>
#include <PubSubClient.h>
#include <ThingSpeak.h>

// PINS
  uint16_t RECV_PIN = 5;
  IRsend irsend(14);  // gpio14 = D5   //  gpio12 = D6 //// gpio14... is d5?  //  gpio5 = D1    //    gpio4 = D2  //
  #define DHT11_PIN 4
  // #define pirPin 0
  //byte ledPin = 2;

// VARS
        // IR
      IRrecv irrecv(RECV_PIN); decode_results results;
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

      // HUE LIGHTING
      const char LIGHTS_ON[] = "{\"on\":true}";
      const char LIGHTS_OFF[] = "{\"on\":false}";
      const char *bridge_ip = "192.168.0.101"; // Hue Bridge IP
      long looped;  unsigned long mLastTime=0;  bool looping = false;
//  const int port = 80;   const char *ccchar = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/1/state/";

    // Wi-Fi / IoT
    const char ssid[] = "WiFi2";
    const char password[] = "4328646518";
    IPAddress ip(192, 168, 0, 188);      // fix IP of this node
    IPAddress gateway(192,168,0,1);     // WiFi router's IP
    IPAddress subnet(255,255,255,0);
    ESP8266WebServer server(9001);
    RestClient hue = RestClient(bridge_ip);
    int status = WL_IDLE_STATUS;    WiFiClient client;

    // Motion PIR sensor
    //unsigned long stars,down=0;    boolean motion = false;    int pirVal = LOW;

    // TEMPC AND HUMIDITY VARS
    int value, temp, humid = 0;    dht DHT;    char temperatureString[6];    char humidString[6];    unsigned long DHTtimer = 0;

    // THINGSPEAK INFO
    unsigned long myChannelNumber = 339378;
    const char * myWriteAPIKey = "GFL52HKR8RQ44R08";
    char buffer[100];

        void sendIr(uint64_t cmd){
          irsend.begin();
          Serial.println("sending Sony");
          irsend.sendSony(cmd, 12, 2);
          delay(100);
          irrecv.resume();
        }

        void hue_turnOff(int lightNo){
          String cmd = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/";
    			cmd += lightNo;
    			cmd += "/state/";  int buf = cmd.length();
          cmd.toCharArray(buffer, buf);
    			hue.put(buffer, LIGHTS_OFF);
        }

        void hue_turnOn(int lightNo){
          String cmd = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/";
  				cmd += lightNo;
  				cmd += "/state/";
          int buf = cmd.length();
          cmd.toCharArray(buffer, buf);
  				hue.put(buffer, LIGHTS_ON);
        }

			  void doLoop(int on, int lightNo){
  			  if (on == 1){
  				      const char EFFECT_COLORLOOP[] = "{\"effect\":\"colorloop\"}";
  					String cmd = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/";
  					cmd += lightNo;
  					cmd += "/state/";  int buf = cmd.length();
                cmd.toCharArray(buffer, buf);
  					hue.put(buffer, EFFECT_COLORLOOP);
  					looping = true;
  				}
  				if (on == 0){
  				      const char NO_COLORLOOP[] = "{\"effect\":\"none\"}";
  					String cmd = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/";
  					cmd += lightNo;
  					cmd += "/state/";  int buf = cmd.length();
            cmd.toCharArray(buffer, buf);
  					hue.put(buffer, NO_COLORLOOP);
  					looping = false;
  				}
			  }

int times=0;

void handleRootC(String cmdT) {
 doLoop(1,1);
 String message = "<!DOCTYPE html><html><head><title>NodeIR TPad</title></head><body><h1>NodeIRHue Command</h1><h2>";
     message += cmdT;
     message += " COMMAND SENT</h2><h3>CurrentConditions</h3><h4>Temp =";
     message += temperatureString;
     message += "</h4><h4>Humid =";
     message += humidString;
     message += "</h4><table><tr><th>TV/HiFi</th><th>Hue</th><th>Candle</th></tr><tr><td><a href=\"tvoff\">TV On/Off</a></td><td><a href=\"hue1\">Hue On/Off</a></td><td><a href=\"candle0\">CandleOn</a></td></tr><tr><td><a href=\"volup\">Send VolUP</a></td><td><a href=\"hue2\">HueBlue</a></td><td><a href=\"candle1\">CandleOff</a></td></tr><tr><td><a href=\"voldn\">Send VolDN</a></td><td><a href=\"hue2\">HueRelax</a></td><td><a href=\"candle2\">candleMULTI</a></td></tr><tr><td><a href=\"tvaux\">HiFi TVAUX</a></td><td><a href=\"hue3\">Hue3</a></td><td><a href=\"candle3\">CandleMode</a></td></tr><tr><td><a href=\"btaux\">HiFi BTAUX</a></td><td><a href=\"hue4\">Hue4</a></td><td><a href=\"candle4\">Candle4hrs</a></td></tr></table><iframe src=\"http://bit.ly/2AjM3oj\"></iframe><iframe src=\"http://bit.ly/2AoDXgS\"></iframe><iframe src=\"http://bit.ly/2zAjMZI\" width=\"420\" height=\"240\"></iframe></body></html>";
             server.send(200, "text/html", message);
 }

    	     void handleRoot() {
            doLoop(1,1);
          String message = "<!DOCTYPE html><html><head><style>table{font-family: arial, sans-serif;border-collapse: collapse;width: 100%;}td,th{border:1pxsolid #dddddd;text-align: left;padding: 8px;}tr:nth-child(even){background-color: #dddddd;}</style><title>NodeIR TPad</title></head><body><h1>NodeIRHue Command</h1><h2>CurrentConditions</h2><h4>Temp:";
    				message += temperatureString;
            message += "</h4><h4>Humidity:";
            message += humidString;
            message += "</h4><table><tr><th>TV/HiFi</th><th>Hue</th><th>Candle</th></tr><tr><td><a href=\"tvoff\">TV On/Off</a></td><td><a href=\"hue1\">Hue On/Off</a></td><td><a href=\"candle0\">CandleOn</a></td></tr><tr><td><a href=\"volup\">Send VolUP</a></td><td><a href=\"hue2\">HueBlue</a></td><td><a href=\"candle1\">CandleOff</a></td></tr><tr><td><a href=\"voldn\">Send VolDN</a></td><td><a href=\"hue2\">HueRelax</a></td><td><a href=\"candlemu\">candleMULTI</a></td></tr><tr><td><a href=\"tvaux\">HiFi TVAUX</a></td><td><a href=\"hue3\">Hue3</a></td><td><a href=\"candlemo\">CandleMode</a></td></tr><tr><td><a href=\"btaux\">HiFi BTAUX</a></td><td><a href=\"hue4\">Hue4</a></td><td><a href=\"candle4\">Candle4hrs</a></td></tr></table><iframe src=\"http://bit.ly/2AjM3oj\"></iframe><iframe src=\"http://bit.ly/2AoDXgS\"></iframe><iframe src=\"http://bit.ly/2zAjMZI\" width=\"420\" height=\"240\"></iframe></body></html>";
                    server.send(200, "text/html", message);
            }

            void sendNECIr(uint8_t cmd){
                irsend.begin();
                Serial.println("sending "+ cmd);
                irsend.sendNEC(cmd, 32);
                delay(100);
                  irrecv.resume();
                  handleRootC("sent "+ cmd);
              }
              void candlemo() {
              sendNECIr(CandleMode);
              handleRootC("Mode-Candle SENT");
              }
              void candlemu() {
              sendNECIr(candleMULTI);
              handleRootC("MultiColor SENT");
              }
              void candle1() {
              sendNECIr(CandleOff);
              handleRootC("candle off SENT");
              }
              void candle4(){
                sendNECIr(candle4H);
              handleRootC("candle on 4hrs SENT");
              }

            void volup() {
            sendIr(HiFi_Vup);
            handleRootC("vol up SENT");
            }
            void voldn() {
            sendIr(HiFi_Vdn);
            handleRootC("vol down SENT");
            }
            void tvaux() {
            sendIr(HiFi_TV);
            handleRootC("tvaux SENT");
            }
            void tvoff(){
              sendIr(TV_OFF);
            handleRootC("tv off SENT");
          }
          void btaux(){
              sendIr(HiFi_BT);
            handleRootC("bt SENT");
          }
          void hue2(){
            hue.put("/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/1/state/", LIGHTS_OFF);
            handleRootC("hue2 SENT");
          }
          void hue1(){
            hue.put("/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/1/state/", LIGHTS_ON);
            handleRoot();
          }
          void hue3(){
            hue.put("/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/9/state/", LIGHTS_ON);
            handleRoot();
          }
          void hue4(){
            hue.put("/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/9/state/", LIGHTS_OFF);
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

    //     void motionCheck(){
            //  pirVal = digitalRead(pirPin); // read input value
      //         if (pirVal == HIGH) {
    //             if (motion == LOW) {         // we have just turned on
      //             hue_turnOn(1);
    //             Serial.println("*NEW MOVE"); stars=0; stars++;down=0;
      //              motion = HIGH;
    //                } else if (motion == HIGH){
      //             Serial.print("*"); stars++;
    //                }
    //           } else if (pirVal == LOW){
    //             if (motion == HIGH){
    //             Serial.println("_END - TURNING OFF LIGHT in 5");
          //         motion = LOW; stars = 0; down =0; down++;
            //       hue_turnOff(1);Serial.println("_");
    //             } else if (motion == LOW){
      //         down++;
      //           Serial.print("_");
      //           }
      //           }
      //         delay(10);
      //        }

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
                 String hostNameWifi = "loungeNode";
                 hostNameWifi.concat(".local");
                 WiFi.hostname(hostNameWifi);
                }

            void getTemperature() {
              int chk = DHT.read11(DHT11_PIN);
                temp = DHT.temperature;
                humid = DHT.humidity;
                dtostrf(temp,2,2 , temperatureString);
                dtostrf(humid,2,2 , humidString);
                ThingSpeak.setField(1,temp);
                ThingSpeak.setField(2,humid);
                ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
            }

                  void encoding(decode_results *results) {
                    switch (results->decode_type) {
                      default:
                      case UNKNOWN:      Serial.print("UNKNOWN");       break;
                      case NEC:          Serial.print("NEC");           break;
                      case NEC_LIKE:     Serial.print("NEC (non-strict)");  break;
                      case SONY:         Serial.print("SONY");          break;
                      case RC5:          Serial.print("RC5");           break;
                      case RC6:          Serial.print("RC6");           break;
                      case DISH:         Serial.print("DISH");          break;
                      case SHARP:        Serial.print("SHARP");         break;
                      case JVC:          Serial.print("JVC");           break;
                      case SANYO:        Serial.print("SANYO");         break;
                      case SANYO_LC7461: Serial.print("SANYO_LC7461");  break;
                      case MITSUBISHI:   Serial.print("MITSUBISHI");    break;
                      case SAMSUNG:      Serial.print("SAMSUNG");       break;
                      case LG:           Serial.print("LG");            break;
                      case WHYNTER:      Serial.print("WHYNTER");       break;
                      case AIWA_RC_T501: Serial.print("AIWA_RC_T501");  break;
                      case PANASONIC:    Serial.print("PANASONIC");     break;
                      case DENON:        Serial.print("DENON");         break;
                      case COOLIX:       Serial.print("COOLIX");        break;
                    }
                    if (results->repeat) Serial.print(" (Repeat)");
                  }
                  // Dump out the decode_results structure.
                  void dumpInfo(decode_results *results) {
                    if (results->overflow)
                      Serial.println("WARNING: IR code too long."
                                     "Edit IRrecv.h and increase RAWBUF");
                    Serial.print("Encoding  : ");
                    encoding(results);
                    Serial.println("");
                    Serial.print("Code      : ");
                    serialPrintUint64(results->value, 16);
                    Serial.print(" ");
                    Serial.print(results->bits, DEC);
                    Serial.println(" bits");
                  }
                  void dumpRaw(decode_results *results) {
                     Serial.print("Timing[");  Serial.print(results->rawlen - 1, DEC); Serial.println("]: ");
                    for (uint16_t i = 1;  i < results->rawlen;  i++) {
                      if (i % 100 == 0)
                        yield();  // Preemptive yield every 100th entry to feed the WDT.
                        //    uint32_t x = results->rawbuf[i] * USECPERTICK;
                      if (!(i & 1)) {  // even
                        Serial.print("-");
                    //    if (x < 1000) Serial.print(" ");
                      //  if (x < 100) Serial.print(" ");
                        //Serial.print(x, DEC);
                      } else {  // odd
                        Serial.print("     ");
                        Serial.print("+");
                    //    if (x < 1000) Serial.print(" ");
                    //    if (x < 100) Serial.print(" ");
                    //    Serial.print(x, DEC);
                        if (i < results->rawlen - 1)
                          Serial.print(", ");  // ',' not needed for last one
                      }
                      if (!(i % 8)) Serial.println("");
                    }
                    Serial.println("");  // Newline
                  }
                  // Dump out the decode_results structure.
                  void dumpCode(decode_results *results) {
                    Serial.print("uint16_t  "); Serial.print("rawData["); Serial.print(results->rawlen - 1, DEC); Serial.print("] = {");
                    for (uint16_t i = 1; i < results->rawlen; i++) {
                    //  Serial.print(results->rawbuf[i] * USECPERTICK, DEC);
                      if (i < results->rawlen - 1)
                        Serial.print(",");  // ',' not needed on last one
                      if (!(i & 1)) Serial.print(" ");
                    }
                    Serial.print("};");
                    Serial.print("  // ");  encoding(results);  Serial.print(" ");
                    serialPrintUint64(results->value, 16);  Serial.println("");
                    if (results->decode_type != UNKNOWN) {
                      if (results->address > 0 || results->command > 0) {
                        Serial.print("uint32_t  address = 0x");
                        Serial.print(results->address, HEX);
                        Serial.println(";");
                        Serial.print("uint32_t  command = 0x");
                        Serial.print(results->command, HEX);
                        Serial.println(";");
                      }
                      Serial.print("uint64_t  data = 0x");
                      serialPrintUint64(results->value, 16);
                      Serial.println(";");
                    }
                  }
            void dump(decode_results *results) {
                       // Dumps out the decode_results structure.
                       // Call this after IRrecv::decode()
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
                     Serial.print("*");
                   }
                   Serial.println();
                 }

            void serialTemp() {
             Serial.print("Temperature = ");
             Serial.println(DHT.temperature);
             Serial.print("Humidity = ");
             Serial.println(DHT.humidity);
             Serial.println(" ");
             Serial.print("int Temperature = ");
             Serial.println(temp);
             Serial.print("int Humidity = ");
             Serial.println(humid);
             Serial.println(" ");
           }

           void brightenHue(int lightNo,int briTo, int waiting, boolean loop){
             String com = "/api/fRgcNsvxh3ytQKVUZlCso0KbAn7zOlMhtkVmwzQG/lights/";
             com += lightNo;
             com += "/state/";
              for (int i = 1; i<briTo;i+5){

            //     const char BRI[] = "{\"bri\":\"i}";
              //    hue.put(com, BRI);
                delay(waiting);
              }
              if (loop){
              doLoop(1,lightNo);
            }
          }

          void darkenHue(int lightNo,int waiting, boolean loop){
            for (int i = 255; i>0;i-5){
               delay(waiting);
            }
            if (loop){
            doLoop(1,lightNo);
            }
          }










void setup() {
     Serial.begin(115200);
     setup_wifi();

       //	pinMode(pirPin, INPUT);
       	delay(500);
        ThingSpeak.begin(client);
       server.on("/", handleRoot);
       server.on("/volup", volup);
       server.on("/btaux", btaux);
	      server.on("/tvaux", tvaux);
       server.on("/voldn", voldn);
       server.on("/candle4", [](){
         sendNECIr(candle4H);handleRootC("candleOn 4hrs");
       });
       server.on("/candle1", [](){
       sendNECIr(candleOFF);handleRootC("candle Off");
        });
       server.on("/candlemu", [](){
       sendNECIr(candleMULTI);handleRootC("candleMulti");
        });
       server.on("/candlemo", [](){
       sendNECIr(candleMODE);handleRootC("candleMode");
        });
       server.on("/tvoff", [](){
         sendIr(TV_OFF);
        handleRootC("tvOff");
       });
       server.onNotFound(handleNotFound);
       server.begin();
         delay(100);
       irrecv.enableIRIn();  // Start the receiver
      }


void loop() {
  server.handleClient();
     if (irrecv.decode(&results)) {
		   if (results.value == L1_ON){
			hue_turnOn(1);
		}
		  if (results.value == L1_OFF){
        hue_turnOff(1);
			}
		    dumpInfo(&results);           // Output the results
    dumpRaw(&results);            // Output the results in RAW format
    dumpCode(&results);           // Output the results as source code
    Serial.println("Rec");           // Blank line between entries
    irrecv.resume();
       delay(100);
     }

    if(looping){
    looped++;
      if (looped >=5000) {
      doLoop(0,1);
      looped=0;
      }
    }


   if ((millis() - mLastTime) >= 30000) {
     mLastTime = millis();
   	getTemperature();
        times++;
        if (times == 2) {
          times = 0;
        serialTemp();
      	}
   	//  motionCheck();
    }
  }
