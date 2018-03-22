/*
C:\Users\trend\Documents\PlatformIO\Projects\180109-153921-megaatmega2560\src
elclient
*/
#include <ELClient.h>
#include <ELClientRest.h>
#include <ELClientSocket.h>

#define BUFLEN 266
#define BUFLENN 266

//###########################################################
// For boards using the hardware serial port!
//###########################################################
// Initialize a connection to esp-link using the normal hardware serial port both for
// SLIP and for debug messages.
ELClient esp(&Serial3, &Serial3);
// Initialize a REST client on the connection to esp-link
ELClientRest rest(&esp);

boolean wifiConnected = false;
long timer;

// Initialize a TCP socket client on the connection to esp-link
ELClientSocket tcp(&esp);
// Connection number for tcp
int tcpConnNum;

// Timer value to send out data
uint32_t wait;
// Time to wait between sending out data
uint32_t waitTime;
// IP address for this demo is a local IP.
// Replace it with the IP address where you have a TCP socket server running
char *const tcpServer PROGMEM = "10.0.77.120";
// Port for this demo is the port used by the TCP socket server.
// Replace it with the port that your TCP socket server is listening to
uint16_t const tcpPort PROGMEM = 7002;

char *const errTxt[] PROGMEM = {"No error, everything OK.", "Out of memory.", "Unknown code.", "Timeout.", "Routing problem.", "Operation in progress.",
                                "Unknown code.", "Total number exceeds the maximum limitation.", "Connection aborted.", "Connection reset.", "Connection closed.",
                                "Not connected.", "Illegal argument.", "Unknown code.", "UDP send error.", "Already connected."};
char *getErrTxt(int16_t commError)
{
    commError = commError * -1;
    if (commError <= 15)
    {
        return (char *)pgm_read_word(&errTxt[commError]);
    }
    else
    {
        return (char *)pgm_read_word(&errTxt[2]); // Unknown code
    }
}

// Callback made from esp-link to notify of wifi status changes   - Here we print something out and set a global flag
void wifiCb(void *response)
{
    ELClientResponse *res = (ELClientResponse *)response;
    if (res->argc() == 1)
    {
        uint8_t status;
        res->popArg(&status, 1);
        if (status == STATION_GOT_IP)
        {
            Serial.println("WIFI CONNECTED");
            wifiConnected = true;
        }
        else
        {
            Serial.print("WIFI NOT READY: ");
            Serial.println(status);
            wifiConnected = false;
        }
    }
}


// Callback for TCP socket, called if data was sent or received
// Receives socket client number, can be reused for all initialized TCP socket connections
void tcpCb(uint8_t resp_type, uint8_t client_num, uint16_t len, char *data)
{
    Serial.println("tcpCb connection #" + String(client_num));
    if (resp_type == USERCB_SENT)
    {
        Serial.println("\tSent " + String(len) + " bytes over client#" + String(client_num));
    }
    else if (resp_type == USERCB_RECV)
    {
        char recvData[len + 1];      // Prepare buffer for the received data
        memcpy(recvData, data, len); // Copy received data into the buffer
        recvData[len] = '\0';        // Terminate the buffer with 0 for proper printout!

        Serial.println("\tReceived " + String(len) + " bytes over the server on connection #" + String(client_num));
        Serial.println("\tReceived: " + String(recvData));
        // TODO add some functions to react to the received data
        char respData[len + 11]; // Prepare buffer for the response data
        char *respHdr = "Received: ";
        memcpy(respData, respHdr, 10);
        memcpy(&respData[10], recvData, len); // Copy received data into the buffer
        respData[len + 10] = '\0';
        Serial.println("\tSend response: " + String(respData));
        tcp.send(respData);
    }
    else if (resp_type == USERCB_RECO)
    {
        if (len != -11)
        { // ignore "not connected" error, handled in USERCB_CONN
            Serial.print("\tConnection problem: ");
            Serial.println(getErrTxt(len));
        }
    }
    else if (resp_type == USERCB_CONN)
    {
        if (len == 0)
        {
            Serial.println("\tDisconnected");
        }
        else
        {
            Serial.println("\tConnected");
        }
    }
    else
    {
        Serial.println("Received invalid response type");
    }
}

void printChar(char c)
{
    if (c < ' ' || c >= '~')
    {
        Serial.print("\\x");
        uint8_t c1 = (uint8_t)c >> 4;
        Serial.print((char)(c1 >= 10 ? 'A' + c1 - 10 : '0' + c1));
        uint8_t c2 = (uint8_t)c & 0xf;
        Serial.print((char)(c2 >= 10 ? 'A' + c2 - 10 : '0' + c2));
    }
    else
    {
        Serial.print(c);
    }
}
