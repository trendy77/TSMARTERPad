/**
 * Simple example to demo the El-Client REST calls
 */

#include <ELClient.h>
#include <ELClientRest.h>

#include <IRremote.h>
int RECV_PIN = A14;
IRrecv irrecv(RECV_PIN);
decode_results results;
IRsend irsend;    // pin 3 or 9?

// Initialize a connection to esp-link using the normal hardware serial port both for
// SLIP and for debug messages.
ELClient esp(&Serial3, &Serial3);

// Initialize a REST client on the connection to esp-link
ELClientRest rest(&esp);

boolean wifiConnected = false;

// Callback made from esp-link to notify of wifi status changes
// Here we print something out and set a global flag
void wifiCb(void *response) {
  ELClientResponse *res = (ELClientResponse*)response;
  if (res->argc() == 1) {
    uint8_t status;
    res->popArg(&status, 1);

    if(status == STATION_GOT_IP) {
      Serial.println("WIFI CONNECTED");
      wifiConnected = true;
    } else {
      Serial.print("WIFI NOT READY: ");
      Serial.println(status);
      wifiConnected = false;
    }
  }
}



#define BUFLEN 266
void liux(){ 
  // if we're connected make an HTTP request
  if(wifiConnected) {
    // Request /utc/now from the previously set-up server
    rest.put("", "");

    char response[BUFLEN];
    memset(response, 0, BUFLEN);
    uint16_t code = rest.waitResponse(response, BUFLEN);
    if(code == HTTP_STATUS_OK){
      Serial.println("ARDUINO: GET successful:");
      Serial.println(response);
    } else {
      Serial.print("ARDUINO: GET failed: ");
      Serial.println(code);
    }
    delay(1000);
  }
}

C:\Users\trend\Downloads\My Project-a0ff5810734a

void setup() {
  Serial.begin(115200);   // the baud rate here needs to match the esp-link config
  Serial.println("EL-Client starting!");
Serial3.begin(115200); 
  esp.wifiCb.attach(wifiCb); // wifi status change callback, optional (delete if not desired)
  bool ok;
  do {
    ok = esp.Sync();      // sync up with esp-link, blocks for up to 2 seconds
    if (!ok) Serial.println("EL-Client sync failed!");
  } while(!ok);
  Serial.println("EL-Client synced!");

  // Get immediate wifi status info for demo purposes. This is not normally used because the
  // wifi status callback registered above gets called immediately. 
  esp.GetWifiStatus();
  ELClientPacket *packet;
  if ((packet=esp.WaitReturn()) != NULL) {
    Serial.print("Wifi status: ");
    Serial.println(packet->value);

  }
  int err = rest.begin("10.0.77.101");
  if (err != 0) {
    Serial.print("REST begin failed: ");
    Serial.println(err);
    while(1) ;
  }
  Serial.println("EL-REST ready");
  
irrecv.enableIRIn();       
  
}

void loop() {
  // process any callbacks coming from esp_link
  esp.Process();


 if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  }
}
