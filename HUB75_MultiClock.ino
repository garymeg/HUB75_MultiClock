#include <Adafruit_Protomatter.h>
#include <TetrisMatrixDraw.h>
#include <ezTime.h>
#include <WiFi.h>
#include "Secret.h"
#include "fontclock.h"
#include "Matrix_Config_GM.h"
#include "test8x8reduced.h"
#include "font5x5.h"

#define MYTIMEZONE "Europe/London"
#define clear() matrix.fillScreen(myBLACK)

//#define show() matrix.show()
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
TetrisMatrixDraw tetris(matrix);  // Main clock
TetrisMatrixDraw tetris2(matrix); // The "M" of AM/PM
TetrisMatrixDraw tetris3(matrix); // The "P" or "A" of AM/PM
TetrisMatrixDraw tetrisDate0(matrix);
TetrisMatrixDraw tetrisDate1(matrix);
TetrisMatrixDraw tetrisDate2(matrix);

String HourString, MinuteString, SecondString, DayString, DateString, MonthString;
uint8_t SecondInt, MinuteInt, HourInt;
uint8_t NewRTCm = 60;
int Tz = 1; // Time Zone


unsigned long lastTime = millis();
unsigned long LoopTime = millis();
int displayMode = 0;
unsigned long DisplayTime = (5 * 60) * 1000;

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
    displayMode = random(4);
    
    // Setup for Slots

    pinMode(Bet_up, INPUT_PULLUP);
    pinMode(Bet_down, INPUT_PULLUP);
    pinMode(Play_button, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(COINPIN), coinInterrupt, RISING);
    randomSeed(millis());
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
    if (LoopTime + DisplayTime <=millis())
    {
        displayMode += 1;
        lastDisplayedTime = " ";
        if (displayMode > 2)
            displayMode = 0;
        LoopTime = millis();
    }
    //displayMode = 2;
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
    }
}