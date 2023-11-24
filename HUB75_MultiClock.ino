#include <Adafruit_Protomatter.h>
#include <Fonts/FreeSansBold18pt7b.h> // Large friendly font
#include <Fonts/FreeSansBold9pt7b.h>  // Large friendly font
#include <Fonts/FreeSans9pt7b.h>      // Large friendly font
#include <TetrisMatrixDraw.h>
#include <ezTime.h>
#include <WiFi.h>
#include <vector>
#include <string>
#include "Secret.h"
#include "fontclock.h"
#include "Matrix_Config_GM.h"
#include "test8x8reduced.h"
#include "font5x5.h"

// OldSkoolCoder Additions
#include <WebServer.h>
#include "HTMLPage.h"
#include <IRCClient.h>


#define WIDTH 64
#define HEIGHT 64
// Set Timezone
#define MYTIMEZONE "Europe/London"


// chat bot settings
#define IRC_SERVER "irc.chat.twitch.tv"
#define IRC_PORT 6667

#define clear() fillScreen(myBLACK)
#define stat_NeedToChange 0
#define stat_MaxNumberOfStates 4
#define stat_ScrollingText 1

// create correct matrix.width() & matrix.height()
Adafruit_Protomatter matrix(
    WIDTH,                     // Width of panels
    4,                         // Bitdepth
    1, rgbPins,                // Number of matrix, RGB pins
    4, addrPins,               // No of address pins, address pins
    clockPin, latchPin, oePin, // clock, latch output enable pins
    true,                      // Dubble buffering
    //(HEIGHT==64? -2:1));       // number of panels high (- if alternate panel upside down (zig-zag scanning))
    -2);
Timezone myTZ;
TetrisMatrixDraw tetris(matrix);  // Main clock
TetrisMatrixDraw tetris2(matrix); // The "M" of AM/PM
TetrisMatrixDraw tetris3(matrix); // The "P" or "A" of AM/PM
TetrisMatrixDraw tetrisDate0(matrix);
TetrisMatrixDraw tetrisDate1(matrix);
TetrisMatrixDraw tetrisDate2(matrix);

// OSK added
WebServer server(80);
WiFiClient wiFiClient;
IRCClient client(IRC_SERVER, IRC_PORT, wiFiClient);
std::vector<String> myMessages;
IPAddress ip(192, 168, 1, 99); // 30 to 250
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
// Prototype functions
//void drawTXT(int16_t x, int16_t y, String text, int16_t size, uint16_t color);

// Init Local variables used in setup + Loop
String scrollMessage = "HELLO STREAMERS";
String HourString, MinuteString, SecondString, DayString, DateString, MonthString;
uint8_t SecondInt, MinuteInt, HourInt;
uint8_t displayMode = 0;
String ircChannel = "";
String Player = "";
String lastDisplayedTime = ""; // had to leave in here as tetrus needs to redraw full clock on first run
int oldDisplayMode;
uint8_t width = WIDTH;
uint8_t height = HEIGHT;
unsigned long ClockChangeTime = (1000*60)*5;
unsigned long ClockLastChange = ClockChangeTime;
//
// OldSkoolCoder Addition
uint32_t PlayerPoints = 0;
uint32_t PlayerBet = 0;
int8_t state = 0;
int8_t prevState = 0;
const char* HostName;
// Previous variables that have been changed or not used

// int Tz = 1; // Time Zone
// unsigned long LoopTime = millis();
// //unsigned long DisplayTime = (2 * 60) * 1000;
// unsigned long DisplayTime = 15000;

void setup()
{
    Serial.begin(115200);
    randomSeed(analogRead(A3));
    String Host = "PicoClock_" + String(width) + "x" + String(height);
    HostName = Host.c_str();
    // from OSK
    // myMessages.push_back(" ");
    myMessages.push_back("Say Hello Raspberry Pi PICO :)");
    // Initialize matrix...
    ProtomatterStatus status = matrix.begin();
    Serial.print("Protomatter begin() status: ");
    Serial.println((int)status);

    if (status != PROTOMATTER_OK)
    {
        for (;;)
            ; // halt of display error
    }
    matrix.setTextWrap(false); // Allow text to run off right edge
    matrix.setTextSize(0);
    matrix.setTextColor(myYELLOW);
    matrix.fillScreen(myBLACK);
    matrix.setCursor(0, 0);
    // Attempt to connect to Wifi network:
    matrix.println("Connecting ");
    matrix.println("To WiFi ");
    Serial.print("Connecting Wifi: ");
    Serial.println(ssid);
    matrix.show();
     
    // Set WiFi to station mode and disconnect from an AP if it was Previously connected
    WiFi.mode(WIFI_STA);
    //WiFi.config(ip);
    WiFi.setHostname(HostName);
    WiFi.begin(ssid, password);
    int i = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        drawTXT(i, 21, ".", 1, myYELLOW);
        matrix.show();
        delay(500);
        i++;
        if(i > 64)
            i = 0;
    }

    Serial.println("");
    Serial.println("WiFi\n connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    matrix.fillScreen(myBLACK);
    matrix.setCursor(0, 0);
<<<<<<< HEAD
    matrix.println("WiFi\n connected");
    String IPadd = "IP address :- ";
    IPadd += WiFi.localIP().toString().c_str();

    //myMessages.push_back(IPadd);
    //matrix.println(IPadd);
    //matrix.show();
=======
    matrix.println("WiFi connected");
    String IPadd = "IP address :- ";
    IPadd += WiFi.localIP().toString().c_str();

    myMessages.push_back(IPadd);
    matrix.println(IPadd);
    matrix.show();
>>>>>>> 517684a804d3ee06f83f58eb69e4cc2869ce1117
    delay(2000);
    // setup ezTime
    waitForSync();
    Serial.println();
    Serial.println("UTC:             " + UTC.dateTime());

    myTZ.setLocation(F(MYTIMEZONE));
    Serial.print(F("Time in your set timezone:         "));
    Serial.println(myTZ.dateTime());

    randomSeed(millis());
    displayMode = 0; // random(4);

    handleServer();

    ircChannel = "#" + Twitch_channel;
    client.setCallback(callback);
}

void readtime()
{
    HourString = myTZ.dateTime("H");
    MinuteString = myTZ.dateTime("i");
    SecondString = myTZ.dateTime(("s"));
    HourInt = HourString.toInt();
    MinuteInt = MinuteString.toInt();
    SecondInt = SecondString.toInt();
    DayString = myTZ.dateTime("l");
    DateString = myTZ.dateTime("j F");

    std::transform(DayString.begin(), DayString.end(), DayString.begin(), ::toupper);
    std::transform(DateString.begin(), DateString.end(), DateString.begin(), ::toupper);
}

void loop()
{
    byte i;

    if (displayMode == stat_NeedToChange)
    {
        if (!myMessages.empty())
        {
            getNextMessage();
            changeState(stat_ScrollingText);
            matrix.setFont(&FreeSansBold18pt7b); // Use nice bitmap font
        }
        else
        {
            int8_t newState = prevState + 1;

            if (newState > stat_MaxNumberOfStates)
            {
                newState = stat_ScrollingText + 1;
            }
            displayMode = newState;
            // state = stat_PongClock;
        }
    }
    switch (displayMode)
    {
    case stat_ScrollingText:

        performTextScrolling();
        break;
    case 2:

        AnalogClock(-15);

        break;
    case 3:
        PongClock();
        break;
    case 4:
        TetrisClock();
        break;
    case 5:
        int newPoints = SlotsLoop(Player, PlayerPoints, PlayerBet);
        // points[r] = newPoints;

        if (newPoints != 0)
        {
            sendTwitchMessage("Well Done..... " + Player + " Wins  " + newPoints);
            newPoints += PlayerBet;

            sendTwitchMessage("!addpoints " + Player + " " + newPoints);
        }
        else
        {
            sendTwitchMessage("Sorry..... " + Player + " Please Play again");
        }
        displayMode = oldDisplayMode;
        
        break;
    
    
    }
    server.handleClient();
    twitchLoop();
}
<<<<<<< HEAD
void changeClock(){
     if(millis() > ClockLastChange+ClockChangeTime){
          displayMode += 1;
          if (displayMode >4) displayMode =2;
          ClockLastChange = millis();
     }
=======
>>>>>>> 517684a804d3ee06f83f58eb69e4cc2869ce1117

}
void changeState(int8_t newState)
{
    prevState = displayMode;
    displayMode = newState;
}
