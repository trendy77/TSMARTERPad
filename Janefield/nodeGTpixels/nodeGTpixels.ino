/*
neopixel node
*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 10
#define NUMPIXELS 16
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int delayval = 500;
long lastTime1, lastTime2, time = 0;

uint32_t dimColor(uint32_t color, uint8_t width)
{
    return (((color & 0xFF0000) / width) & 0xFF0000) + (((color & 0x00FF00) / width) & 0x00FF00) + (((color & 0x0000FF) / width) & 0x0000FF);
}
void clearStrip()
{
    for (int i = 0; i < NUMPIXELS; i++)
    {
        strip.setPixelColor(i, 0x000000);
        strip.show();
    }
}
uint32_t colorWheel(byte WheelPos)
{
    byte state = WheelPos / 21;
    switch (state)
    {
    case 0:
        return strip.Color(255, 0, 255 - ((((WheelPos % 21) + 1) * 6) + 127));
        break;
    case 1:
        return strip.Color(255, ((WheelPos % 21) + 1) * 6, 0);
        break;
    case 2:
        return strip.Color(255, (((WheelPos % 21) + 1) * 6) + 127, 0);
        break;
    case 3:
        return strip.Color(255 - (((WheelPos % 21) + 1) * 6), 255, 0);
        break;
    case 4:
        return strip.Color(255 - (((WheelPos % 21) + 1) * 6) + 127, 255, 0);
        break;
    case 5:
        return strip.Color(0, 255, ((WheelPos % 21) + 1) * 6);
        break;
    case 6:
        return strip.Color(0, 255, (((WheelPos % 21) + 1) * 6) + 127);
        break;
    case 7:
        return strip.Color(0, 255 - (((WheelPos % 21) + 1) * 6), 255);
        break;
    case 8:
        return strip.Color(0, 255 - ((((WheelPos % 21) + 1) * 6) + 127), 255);
        break;
    case 9:
        return strip.Color(((WheelPos % 21) + 1) * 6, 0, 255);
        break;
    case 10:
        return strip.Color((((WheelPos % 21) + 1) * 6) + 127, 0, 255);
        break;
    case 11:
        return strip.Color(255, 0, 255 - (((WheelPos % 21) + 1) * 6));
        break;
    default:
        return strip.Color(0, 0, 0);
        break;
    }
}

void knightRider(uint16_t cycles, uint16_t speed, uint8_t width, uint32_t color)
{
    uint32_t old_val[8];
    for (uint16_t i = 0; i < cycles; i++)
    {
        for (uint16_t count = 1; count < NUMPIXELS; count++)
        {
            strip.setPixelColor(count, color);
            old_val[count] = color;
            for (uint16_t x = count; x > 0; x--)
            {
                old_val[x - 1] = dimColor(old_val[x - 1], width);
                strip.setPixelColor(x - 1, old_val[x - 1]);
            }
            strip.show();
            delay(speed);
        }
        for (int count = NUMPIXELS - 1; count >= 0; count--)
        {
            strip.setPixelColor(count, color);
            old_val[count] = color;
            for (int x = count; x <= NUMPIXELS; x++)
            {
                old_val[x - 1] = dimColor(old_val[x - 1], width);
                strip.setPixelColor(x + 1, old_val[x + 1]);
            }
            strip.show();
            delay(speed);
        }
    }
}
void goSolid(int colour)
{
    switch (colour)
    {
    case '1':
        for (int i = 0; i < NUMPIXELS; i++)
        {
            // strip.Color takes RGB values, from 0,0,0 up to 255,255,255
            strip.setPixelColor(i, strip.Color(200, 0, 0)); // Moderately bright green color.
            strip.show();                                   // This sends the updated pixel color to the hardware.
            delay(delayval);                                // Delay for a period of time (in milliseconds).
        }
        break;
    case '2':
        for (int i = 0; i < NUMPIXELS; i++)
        {
            // strip.Color takes RGB values, from 0,0,0 up to 255,255,255
            strip.setPixelColor(i, strip.Color(0, 200, 0)); // Moderately bright green color.
            strip.show();                                   // This sends the updated pixel color to the hardware.
            delay(delayval);                                // Delay for a period of time (in milliseconds).
        }
        break;
    case '3':
        for (int i = 0; i < NUMPIXELS; i++)
        {
            // strip.Color takes RGB values, from 0,0,0 up to 255,255,255
            strip.setPixelColor(i, strip.Color(0, 0, 200)); // Moderately bright green color.
            strip.show();                                   // This sends the updated pixel color to the hardware.
            delay(delayval);                                // Delay for a period of time (in milliseconds).
        }
        break;
    }
}
void kitt()
{
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
    for (byte jj = 0; jj < 10; jj++)
    {
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

    for (int i = 0; i < NUMPIXELS; i++)
    {
        // strip.Color takes RGB values, from 0,0,0 up to 255,255,255
        strip.setPixelColor(i, strip.Color(200, 0, 0)); // Moderately bright green color.
    }
    for (int i = 1; i < NUMPIXELS; i++)
    {
        // strip.Color takes RGB values, from 0,0,0 up to 255,255,255
        strip.setPixelColor(i, strip.Color(0, 0, 200)); // Moderately bright green color.
    }
    strip.show(); // This sends the updated pixel color to the hardware.

    delay(delayval); // Delay for a period of time (in milliseconds).
    for (byte j = 0; j < 150; j += 7)
    {
        knightRider(2, j, width, 0x0000FF); // Cycles, Speed, Width, RGB Color
        width++;
    }
    for (int i = 0; i < NUMPIXELS; i++)
    {
        // strip.Color takes RGB values, from 0,0,0 up to 255,255,255
        strip.setPixelColor(i, strip.Color(200, 0, 0)); // Moderately bright green color.
    }
    for (int i = 1; i < NUMPIXELS; i++)
    {
        // strip.Color takes RGB values, from 0,0,0 up to 255,255,255
        strip.setPixelColor(i, strip.Color(0, 0, 200)); // Moderately bright green color.
    }
    strip.show(); // This sends the updated pixel color to the hardware.
    delay(delayval);
    clearStrip();
}
void serialSet()
{
    if (shownMenu == false)
    {
        Serial.println("by all means, please choose a lighting scheme for the AlfaGT....");
        Serial.println("1- 2- 3- 4- 5- 6- 7- 8- 9- 10- ");
        shownMenu = true;
    }
    if (Serial.available())
    {
        char dato = Serial.read();
        switch (dato)
        {
        case '1':
            goSolid(1);
            break;
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
            knightRider(4, 46, 5, 0xFF0000); // red
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
    }
}
void rainBow()
{
    byte width = 2;
    for (byte j = 0; j < 150; j += 7)
    {                                            // Iterate through a whole rainbow of colors
        knightRider(1, j, width, colorWheel(j)); // Cycles, Speed, Width, RGB Color
        width++;
    }
    clearStrip();
}

void setLights()
{
    if (watsdoin == 11)
    {
        rainBow();
    }
    else if (watsdoin == 12)
    {
        kitt();
    }
    else if (watsdoin == 13)
    {
        for (int i = 0; i < NUMPIXELS; i++)
        {
            strip.setPixelColor(i, strip.Color(100, 100, 100)); // Moderately bright white??
            strip.show();
        }
    }
    watsdoin++;
}

void sentGet(String cmd, String data)
{
    char __endp[(sizeof(cmd) + 1)];
    cmd.toCharArray(__endp, sizeof(__endp));
    char __e1ndp[(sizeof(data) + 1)];
    data.toCharArray(__e1ndp, sizeof(__e1ndp));

    if (wifiConnected)
    {
        rest.get(__endp, __e1ndp);
        char response[BUFLEN];
        memset(response, 0, BUFLEN);
        uint16_t code = rest.waitResponse(response, BUFLEN);
        if (code == HTTP_STATUS_OK)
        {
            Serial.println("ARDUINO: GET successful:");
            Serial.println(response);
        }
        else
        {
            Serial.print("ARDUINO: GET failed: ");
            Serial.println(code);
        }
        delay(500);
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
void setLights()
{
    if (watsdoin == 11)
    {
        rainBow();
    }
    if (watsdoin == 12)
    {
        kitt();
    }
    else if (watsdoin == 13)
    {
        for (int i = 0; i < NUMPIXELS; i++)
        {
            strip.setPixelColor(i, strip.Color(100, 100, 100)); // Moderately bright white??
            strip.show();
        }
    }
    watsdoin++;
}
bool msgToSend = false;
String damsgToSend = "";

void setup()
{
    pinMode(ledPin, OUTPUT);
    digitalWrite(pirPin, LOW);
    Serial.begin(115200);
    Serial.print("booting");
    delay(500);
    strip.begin(); // This initializes the NeoPixel library.
    watsdoin = 11;
    tcpConnNum = tcp.begin(tcpServer, tcpPort, SOCKET_TCP_SERVER, tcpCb); // SOCKET_SERVER ==> accept connections
    if (tcpConnNum < 0)
    {
        Serial.println(F("TCP socket setup failed, try again in 10 seconds after reboot"));
        delay(10000);
        asm volatile("  jmp 0");
    }
    else
    {
        Serial.println(String(tcpServer) + ":" + String(tcpPort) + " is served over connection number # = " + String(tcpConnNum));
    }
    Serial.println(F("EL-TCP ready"));
}

void loop()
{
    char response[266];
    timer = millis();

    if (motionActive)
    {

        checkMotion();
    }
    if (motion)
    {
        Serial.print("motion");
    }
    //setLights();
    //serialSet();

    // if (time >= (lastTime2 + 300000))
    // { // every 60 secs
    //     rainBow();
    // }
    //   if (time >= (lastTime1 + 600000))
    // { // every 100 secs
    //   kitt();
    //  lastTime1 = time;
    //}
}