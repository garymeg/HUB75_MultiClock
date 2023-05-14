#include <Adafruit_Protomatter.h>
#include <TetrisMatrixDraw.h>
#include <ezTime.h>
#include <WiFi.h>
#include "Secret.h"
#include "fontclock.h"
#include "Matrix_Config_GM.h"
#include "test8x8reduced.h"
#include "font5x5.h"

// OldSkoolCoder Additions
#include <WebServer.h>
#include "HTMLPage.h"
#include <IRCClient.h>

#define MYTIMEZONE "Europe/London"
// Definitions for buttons
#define Bet_up 18
#define Bet_down 19
#define Play_button 20
// Definitions for coin acceptor
#define COINPIN 21

// Buzzer
#define speakerOut 22
// #define show() matrix.show()
#define WIDTH 64
#define HEIGHT 64
Adafruit_Protomatter matrix(
    64,                                 // Width of panels
    4,                                  // Bitdepth
    1, rgbPins,                         // , RGB pins
    4, addrPins,                        // No of address pins, address pins
    clockPin, latchPin, oePin,          // clock, latch output enable pins
    true,                               // Dubble buffering
    -2);                                // number of panels high (- if alternate panel upside down (zig-zag scanning))
Timezone myTZ;
#define clear() fillScreen(myBLACK)
TetrisMatrixDraw tetris(matrix);  // Main clock
TetrisMatrixDraw tetris2(matrix); // The "M" of AM/PM
TetrisMatrixDraw tetris3(matrix); // The "P" or "A" of AM/PM
TetrisMatrixDraw tetrisDate0(matrix);
TetrisMatrixDraw tetrisDate1(matrix);
TetrisMatrixDraw tetrisDate2(matrix);
int points[]{1250, 9000, 4530, 3440,5780};
String user[]{"OLDSKOOLCODER", "GARYMEG", "_SP175", "_DOCSTER", "STACBATS"};

String HourString, MinuteString, SecondString, DayString, DateString, MonthString;
uint8_t SecondInt, MinuteInt, HourInt;
uint8_t NewRTCm = 60;
int Tz = 1; // Time Zone

// OldSkoolCoder Addition
WebServer server(80);
String Player = "";
int PlayerPoints = 0;
uint16_t PlayerBet = 0;

#define IRC_SERVER   "irc.chat.twitch.tv"
#define IRC_PORT     6667
WiFiClient wiFiClient;
IRCClient client(IRC_SERVER, IRC_PORT, wiFiClient);
String ircChannel = "";

unsigned long lastTime = millis();
unsigned long LoopTime = millis();
int displayMode = 0;
//unsigned long DisplayTime = (2 * 60) * 1000;
unsigned long DisplayTime = 15000;

String lastDisplayedTime = ""; // had to leave in here as tetrus needs to redraw full clock on first run


void setup()
{
    Serial.begin(115200);
    randomSeed(analogRead(A3));
    // Attempt to connect to Wifi network:
    Serial.print("Connecting Wifi: ");
    Serial.println(ssid);

    // Set WiFi to station mode and disconnect from an AP if it was Previously
    // connected
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

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
    matrix.setCursor(0, 0);
    matrix.fillScreen(myBLACK);
    waitForSync();
    Serial.println();
    Serial.println("UTC:             " + UTC.dateTime());

    myTZ.setLocation(F(MYTIMEZONE));
    Serial.print(F("Time in your set timezone:         "));
    Serial.println(myTZ.dateTime());

    matrix.fillScreen(myBLACK);
    randomSeed(millis());
    displayMode = 0; //random(4);

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
    // if (LoopTime + DisplayTime <=millis())
    // {
    //     displayMode += 1;
    //     lastDisplayedTime = " ";
    //     if (displayMode > 3)
    //         displayMode = 0;
    //     LoopTime = millis();
    // }
    // lock display for testing
    //displayMode = 3;
    
    switch(displayMode){
    
    case 0:
        AnalogClock();
        break;
    case 1:
        PongClock();
        break;
    case 2:
        TetrisClock();
        break;
    case 3:
        // Tempory variables to use in game
        // int r = random(5);
        // String Player = user[r];
        // int Point = points[r];
        // uint16_t Bet = random(100)*10;

        int newPoints = SlotsLoop(Player, PlayerPoints, PlayerBet);
        //points[r] = newPoints;

        if (newPoints != 0)
        {
          // Generic BOT Command.
          sendTwitchMessage("!addpoints " + Player + " " + newPoints);

          // OSK Bot Command.
          //sendTwitchMessage("!addosk " + Player + " " + newPoints);
        }
        displayMode = 0;
        break;
    }

    server.handleClient();

    twitchLoop();
}