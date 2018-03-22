#include "JsonStreamingParser.h"
#include "JsonListener.h"
#include "ExampleParser.h"

JsonStreamingParser parser;
ExampleListener listener;

void setup() {
  Serial.begin(115200);
  Serial.println(String(ESP.getFreeHeap()));
  parser.setListener(&listener);
  // put your setup code here, to run once:
 
  
  char json[] = "{\"a\":3, \"b\":{\"c\":\"d\"}}";





GET /api//groups/0 HTTP/1.1
Host: 10.0.77.101
Accept-Encoding: gzip, deflate
Accept: */*
Connection: keep-alive
{
  "action":
  {
    "on":false,
    "bri":254,
    "hue":14922,
    "sat":144,
    "xy":[0.4595,0.4105],
    "ct":369,
    "effect":"none",
    "colormode":"ct"
  },
  "lights": ["1","2","3"],
  "name": "Lightset 0"
}
  for (int i = 0; i < sizeof(json); i++) {
    parser.parse(json[i]); 
  }
  Serial.println(String(ESP.getFreeHeap()));
}

void loop() {
  // put your main code here, to run repeatedly:

}
