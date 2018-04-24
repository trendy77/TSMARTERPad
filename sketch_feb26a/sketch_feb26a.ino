/**
 * Simple example to demo the El-Client UDP calls
 */

#include <ELClient.h>
#include <ELClientSocket.h>
#include <IRLibSendBase.h>    // First include the send base
#include "IRLibAll.h"
//Now include only the protocols you wish to actually use.
//The lowest numbered protocol should be first but remainder 
//can be any order.
#include <IRLib_P01_NEC.h>    
#include <IRLib_P02_Sony.h>   
#include <IRLibCombo.h>   

// IP address for this demo is a local IP.
// Replace it with the IP address where you have a UDP socket server running
char * const tcpServer PROGMEM = "10.0.77.110"; 
uint16_t const tcpPort PROGMEM = 7001;

long ttime,last =0;

//###########################################################
// For boards using the hardware serial port!
//###########################################################
// Initialize a connection to esp-link using the normal hardware serial port both for
// SLIP and for debug messages.
ELClient esp(&Serial3, &Serial3);

IRrecvPCI myReceiver(A14);        
IRsend mySender;      // THIS IS PIN 9 ON MEGA!
IRdecode myDecoder;   
ELClientSocket tcp(&esp);
int tcpConnNum;
// Timer value to send out data 
uint32_t wait;
// Time to wait between sending out data
uint32_t waitTime;
// Flag for wifi connection
boolean wifiConnected = false;

char* const errTxt[] PROGMEM = {"No error, everything OK.","Out of memory.","Unknown code.","Timeout.","Routing problem.","Operation in progress.",
         "Unknown code.","Total number exceeds the maximum limitation.","Connection aborted.","Connection reset.","Connection closed.",
          "Not connected.","Illegal argument.","Unknown code.","UDP send error.","Already connected."};
char * getErrTxt(int16_t commError) {
  commError = commError*-1;
  if (commError <= 15) {
    return (char *) pgm_read_word (&errTxt[commError]);
  } else {
    return (char *) pgm_read_word (&errTxt[2]); // Unknown code
  }
}

// Callback for TCP socket, called if data was sent or received Receives socket client number, can be reused for all initialized TCP socket connections
void tcpCb(uint8_t resp_type, uint8_t client_num, uint16_t len, char *data) {
  Serial.println("tcpCb connection #"+String(client_num));
  if (resp_type == USERCB_SENT) {
    Serial.println("\tSent " + String(len) + " bytes over client#" + String(client_num));
  } else if (resp_type == USERCB_RECV) {
    char recvData[len+1]; // Prepare buffer for the received data
    memcpy(recvData, data, len); // Copy received data into the buffer
    recvData[len] = '\0'; // Terminate the buffer with 0 for proper printout!
    Serial.println("\tReceived " + String(len) + " bytes over the client on connection #" + String(client_num));
    Serial.println("\tReceived: " + String(recvData));
  } else if (resp_type == USERCB_RECO) {
    if (len != -11) { // ignore "not connected" error, handled in USERCB_CONN
      Serial.print("\tConnection problem: ");
      Serial.println(getErrTxt(len));
    }
  } else if (resp_type == USERCB_CONN) {
    if (len == 0) {
      Serial.println("\tDisconnected");
    } else {
      Serial.println("\tConnected");
    }
  } else {
    Serial.println("Received invalid response type");
  }
}

// Callback made from esp-link to notify of wifi status changes
// Here we print something out and set a global flag
void wifiCb(void *response) {
  ELClientResponse *res = (ELClientResponse*)response;
  if (res->argc() == 1) {
    uint8_t status;
    res->popArg(&status, 1);
    if(status == STATION_GOT_IP) {
      Serial.println(F("WIFI CONNECTED"));
      wifiConnected = true;
    } else {
      Serial.print(F("WIFI NOT READY: "));
      Serial.println(status);
      wifiConnected = false;
    }
  }
}

void sentT(){
  // if we're connected send data over TCP socket
  if(wifiConnected) {
   if (tcpConnNum > 0 ){      // Send message to the previously set-up server #1
      Serial.print(F("Sending message to "));
      Serial.print(tcpServer);
      Serial.print(":");
      Serial.println(tcpPort);
      tcp.send("Message from Uno WiFi over TCP socket");
    }
  else {
   sendIr(); 
  }
  }
   else {
    // This is just for demo, you can as well just try to reconnect
    // and setup the connection to esp-link again 
    Serial.println(F("Lost WiFi connection, try to reboot"));
    delay(10000);
    asm volatile ("  jmp 0");
  }
}

void sendIr(){
     mySender.send(SONY,0x61a0f00f, 20);//Sony DVD power A8BCA, 20 bits
   
    Serial.println(F("Sent signal."));
   mySender.send(NEC,0x61a0f00f,0);//NEC TV power button=0x61a0f00f
  }

void setup() {
Serial.begin(115200);
  Serial.println(F("EL-Client starting!"));
Serial3.begin(115200);
    // Sync-up with esp-link, this is required at the start of any sketch and initializes the
  // callbacks to the wifi status change callback. The callback gets called with the initial
  // status right after Sync() below completes.
  esp.wifiCb.attach(wifiCb); // wifi status change callback, optional (delete if not desired)
  bool ok;
  do {
    ok = esp.Sync();      // sync up with esp-link, blocks for up to 2 seconds
    if (!ok) Serial.println(F("EL-Client sync failed!"));
  } while(!ok);
  Serial.println(F("EL-Client synced!"));
  // Get immediate wifi status info for demo purposes. This is not normally used because the
  // wifi status callback registered above gets called immediately.
  esp.GetWifiStatus();
  ELClientPacket *packet;
  if ((packet=esp.WaitReturn()) != NULL) {
    Serial.print(F("Wifi status: "));
    Serial.println(packet->value);
  }
  // Set up the TCP socket client for a connection to <tcpServer> on port <>, this doesn't connect to that server,
  // it just sets-up stuff on the esp-link side and waits until we send some data
  tcpConnNum = tcp.begin(tcpServer, tcpPort, SOCKET_TCP_CLIENT_LISTEN, tcpCb); // SOCKET_CLIENT ==> we expect a response
  if (tcpConnNum < 0) {
    Serial.println(F("TCP socket setup failed, try again in 10 seconds after reboot"));
    delay(10000);
    asm volatile ("  jmp 0");
  } else {
    Serial.println(String(tcpServer)+":"+String(tcpPort)+" is served over connection number # = "+String(tcpConnNum));
  }
  Serial.println(F("EL-TCP ready"));
  myReceiver.enableIRIn(); // Start the receiver
  Serial.println(F("Ready to receive IR signals"));
}

void loop() {
  esp.Process();       // process any callbacks coming from esp_link
  ttime = millis();
  //Continue looping until you get a complete signal received

  if (myReceiver.getResults()) {
    myDecoder.decode();           //Decode it
    myDecoder.dumpResults(true);  //Now print results. Use false for less detail
    myReceiver.enableIRIn();      //Restart receiver
  }
 
if (ttime >( last+2000)){
  last = ttime;
  sendIr();
} 
//if (ttime >( last+30000)){
  //last = ttime;
  //sentT();
//}
}

