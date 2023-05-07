#include <Adafruit_Protomatter.h>
#include <TetrisMatrixDraw.h>
#include <ezTime.h>
#include <WiFi.h>
#include "Secret.h"
#include "fontclock.h"
#include "Matrix_Config_GM.h" 

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


bool twelveHourFormat = false;
bool forceRefresh = false;
bool showColon = true;
const byte CENTRE_X = 32;
const byte CENTRE_Y = 32;
int Tz = 1; // Time Zone
uint8_t SecondInt;
uint8_t MinuteInt;
uint8_t HourInt;
uint8_t NewRTCm = 60;
int co0, co1;
uint16_t MyColor[92] = {0xF800, 0xF880, 0xF900, 0xF980, 0xFA20, 0xFAA0, 0xFB20, 0xFBA0, 0xFC40, 0xFCC0, 0xFD40, 0xFDC0,
                        0xFDC0, 0xFE60, 0xFEE0, 0xFF60, 0xFFE0, 0xEFE0, 0xDFE0, 0xCFE0, 0xBFE0, 0xAFE0, 0x9FE0, 0x8FE0,
                        0x77E0, 0x67E0, 0x57E0, 0x47E0, 0x37E0, 0x27E0, 0x17E0, 0x07E0, 0x07E2, 0x07E4, 0x07E6, 0x07E8,
                        0x07EA, 0x07EC, 0x07EE, 0x07F1, 0x07F3, 0x07F5, 0x07F7, 0x07F9, 0x07FB, 0x07FD, 0x07FF, 0x077F,
                        0x06FF, 0x067F, 0x05DF, 0x055F, 0x04DF, 0x045F, 0x03BF, 0x033F, 0x02BF, 0x023F, 0x019F, 0x011F,
                        0x009F, 0x001F, 0x101F, 0x201F, 0x301F, 0x401F, 0x501F, 0x601F, 0x701F, 0x881F, 0x981F, 0xA81F,
                        0xB81F, 0xC81F, 0xD81F, 0xE81F, 0xF81F, 0xF81D, 0xF81B, 0xF819, 0xF817, 0xF815, 0xF813, 0xF811,
                        0xF80E, 0xF80C, 0xF80A, 0xF808, 0xF806, 0xF804, 0xF802, 0xF800};

unsigned long lastTime = millis();
unsigned long LoopTime = millis();
int displayMode = 0;
unsigned long DisplayTime = (5 * 60) * 1000;
unsigned long oneSecondLoopDue = 0;
volatile bool finishedAnimating = false;
bool displayIntro = true;

String lastDisplayedTime = "";
String lastDisplayedAmPm = "";

float ballX = 31;
float ballY = (random(16)) + 8;
float leftPlayerTargetY = ballY;
float rightPlayerTargetY = ballY;
float leftPlayerY = 8;
float rightPlayerY = 18;
float ballVX = 1;
float ballVY = 0.5;
int playerLoss = 0;
int gameStopped = 0;
String HourString, MinuteString, SecondString, DayString, DateString, MonthString;
int HourOld, MinuteOld, secondOld, DateInt;
String timeStr = "00";

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

    co0 = random(0, 91);
    if (NewRTCm != MinuteInt)
    {
        co1 = random(0, 91);
        NewRTCm = MinuteInt;
    }
}

void AnalogClock()
{
    clear();
    readtime();
    drawclockBDot();
    drawclockDot();
    drawclockNB();
    drawtime();
    matrix.show();
    delay(1000);
}

void drawclockNB()
{
    float radians, angle;
    int j = 0;
    matrix.setFont(&fontclock);
    for (int i = 0; i < 60; i += 5)
    {
        matrix.setTextColor(MyColor[(co1 + i * 5) % 92]);
        angle = 180 - i * 6;
        radians = radians(angle);
        int x0 = CENTRE_X + 26 * sin(radians);
        int y0 = CENTRE_Y + 26 * cos(radians);
        matrix.setCursor(x0, y0);
        matrix.print(char(j));
        j++;
        if (j == 10)
        {
            j = 11;
        }
    }
    matrix.setFont();
}
void drawclockDot()
{
    float radians, angle;
    for (int i = 0; i < 60; i += 5)
    {
        uint16_t color = MyColor[(co0 + i * 5) % 92];
        angle = 360 - i * 6;
        radians = radians(angle);
        int x0 = CENTRE_X + 30 * sin(radians);
        int y0 = CENTRE_Y + 30 * cos(radians);
        matrix.fillCircle(x0, y0, 1, color);
    }
}
void drawclockBDot()
{
    float radians, angle;
    matrix.drawCircle(CENTRE_X, CENTRE_Y, 30, MyColor[co1]);
    for (int i = 0; i < 60; i += 15)
    {
        uint16_t color = MyColor[(co1 + i * 5) % 92];
        angle = 360 - i * 6;
        radians = radians(angle);
        int x0 = CENTRE_X + 30 * sin(radians);
        int y0 = CENTRE_Y + 30 * cos(radians);
        matrix.fillCircle(x0, y0, 2, color);
    }
}
void drawHMS(float angle, int x0, int y0, int w, int c0, int c1, int z1)
{
    int x[3];
    int y[3];
    float radians = radians(angle);
    x[0] = x0 + w * sin(radians);
    y[0] = y0 + w * cos(radians);
    radians = radians(angle + z1);
    x[1] = x0 + 6 * sin(radians);
    y[1] = y0 + 6 * cos(radians);
    radians = radians(angle - z1);
    x[2] = x0 + 6 * sin(radians);
    y[2] = y0 + 6 * cos(radians);
    matrix.fillTriangle(x0, y0, x[0], y[0], x[1], y[1], MyColor[c0]);
    matrix.fillTriangle(x0, y0, x[0], y[0], x[2], y[2], MyColor[c0]);
    matrix.drawLine(x[0], y[0], x[1], y[1], MyColor[c1]);
    matrix.drawLine(x[0], y[0], x[2], y[2], MyColor[c1]);
}
void drawtime()
{
    if (HourInt > 12)
        HourInt -= 12;
    float angle = 180 - (30 * HourInt) - (MinuteInt / 2);
    drawHMS(angle, CENTRE_X, CENTRE_Y, 17, 0, 0, 30);
    angle = 180 - (6 * MinuteInt);
    drawHMS(angle, CENTRE_X, CENTRE_Y, 25, 32, 34, 25);
    angle = 180 - (SecondInt * 6);
    drawHMS(angle, CENTRE_X, CENTRE_Y, 29, co0, co1, 20);
    matrix.fillCircle(CENTRE_X, CENTRE_Y, 2, MyColor[co1]);
    matrix.fillCircle(CENTRE_X, CENTRE_Y, 1, MyColor[co0]);
}
void updateTime()
{
    // rtc.DSgetdatime();
    // configTime(Tz * 3600, 0, "", "");
}
void writeTime()
{
    // configTime(0, 0, "", "");
    // rtc.DSsetdatime();
    // configTime(Tz * 3600, 0, "", "");
}

void PongClock()
{
    {
        if (millis() > lastTime + 20)
        {
            lastTime = millis();
            matrix.fillScreen(myBLACK);

            if (gameStopped < 20)
            {
                gameStopped++;
            }
            else
            {

                ballX += ballVX; // set ball next X position
                ballY += ballVY; // set ball next Y position

                // is ball in play at bat and time dosen't need changing
                if ((ballX >= 60 && playerLoss != 1) || // is X position > 60 and Player Not loose  or
                    (ballX <= 2 && playerLoss != -1))   // is X position < 2 and Player Not Loose
                {
                    ballVX = -ballVX;
                    int tmp = random(4); // perform a random(0-3), last second flick to inflict effect on the ball

                    if (tmp > 0)
                    {
                        tmp = random(2);
                        if (tmp == 0)
                        {
                            if (ballVY > 0 && ballVY < 2.5) // is ball moving down and speed < 2.5
                            {
                                ballVY += 0.2; // Increase ball speed
                            }
                            else if (ballVY < 0 && ballVY > -2.5) // is ball moving up and speed > -2.5
                            {
                                ballVY -= 0.2; // Increase ball speed
                            }
                            if (ballX >= 60) // Is ball at Max Right side of screen
                            {
                                rightPlayerTargetY += 1 + random(3);
                            }
                            else
                            {
                                leftPlayerTargetY += 1 + random(3);
                            }
                        }
                        else
                        {
                            if (ballVY > 0.5)
                            {
                                ballVY -= 0.2;
                            }
                            else if (ballVY < -0.5)
                            {
                                ballVY += 0.2;
                            }
                            if (ballX >= 60)
                            {
                                //rightPlayerTargetY -= 1 + random(3);
                            }
                            else
                            {
                                //leftPlayerTargetY -= 1 + random(3);
                            }
                        }
                        // Check Bat Boundrys (Top & Bottom)
                        if (leftPlayerTargetY < 0)
                        {
                            leftPlayerTargetY = 0;
                        }
                        if (leftPlayerTargetY > 24)
                        {
                            leftPlayerTargetY = 24;
                        }
                        if (rightPlayerTargetY < 0)
                        {
                            rightPlayerTargetY = 0;
                        }
                        if (rightPlayerTargetY > 24)
                        {
                            rightPlayerTargetY = 24;
                        }
                    }
                }
                // ball out of play reset game
                else if ((ballX > 62 && playerLoss == 1) ||
                         (ballX < 0 && playerLoss == -1))
                {
                    // RESET GAME
                    ballX = 31;
                    ballY = random(16) + 8;
                    ballVX = 1;
                    ballVY = 0.5;
                    if (random(2) == 0)
                    {
                        ballVY = -0.5;
                    }
                    readtime();
                    drawTime(HourInt, MinuteInt);
                    playerLoss = 0;
                    gameStopped = 0;
                }
                // Reverse ball when it hits top or bottom of screen
                if (ballY >= 30 || ballY <= 0)
                {
                    ballVY = -ballVY;
                }

                // when the ball is on the other side of the court, move the player "randomly" to simulate an AI
                if ((int)ballX <= 8 + random(13))
                {
                    leftPlayerTargetY = (int)ballY - 3;
                    if (leftPlayerTargetY < 0)
                    {
                        leftPlayerTargetY = 0;
                    }
                    if (leftPlayerTargetY > 24)
                    {
                        leftPlayerTargetY = 24;
                    }
                }
                if ((int)ballX >= 40 + random(13))
                {
                    rightPlayerTargetY = (int)ballY - 3;
                    if (rightPlayerTargetY < 0)
                    {
                        rightPlayerTargetY = 0;
                    }
                    if (rightPlayerTargetY > 24)
                    {
                        rightPlayerTargetY = 24;
                    }
                }

                if (leftPlayerTargetY > leftPlayerY)
                {
                    leftPlayerY++;
                }
                else if (leftPlayerTargetY < leftPlayerY)
                {
                    leftPlayerY--;
                }

                if (rightPlayerTargetY > rightPlayerY)
                {
                    rightPlayerY++;
                }
                else if (rightPlayerTargetY < rightPlayerY)
                {
                    rightPlayerY--;
                }

                // If the ball is in the middle, check if we need to lose and calculate the endpoint to avoid/hit the ball
                if (ballX == 32)
                {
                    readtime();
                    if ((MinuteOld != MinuteInt) && (playerLoss == 0))
                    { // needs to change one or the other

                        MinuteOld = MinuteInt;
                        if (MinuteInt == 0)
                        { // need to change hour
                            playerLoss = 1;
                        }
                        else
                        { // need to change the minute
                            playerLoss = -1;
                        }
                    }

                    if (ballVX < 0)
                    {
                        // moving to the

                        leftPlayerTargetY = calculateEndPoint(ballX, ballY, ballVX, ballVY, playerLoss != -1) - 3;
                        // Serial.printf("%f\t%f\t%f\t%f\t%f\t%d\n", leftPlayerTargetY, ballX, ballY, ballVX, ballVY, playerLoss != -1);
                        if (playerLoss == -1)
                        { // we need to lose
                            if (leftPlayerTargetY < 16)
                            {
                                leftPlayerTargetY = 19 + random(5);
                            }
                            else
                            {
                                leftPlayerTargetY = 5 * random(5);
                            }
                            // Serial.printf("Left Target %f\n", leftPlayerTargetY);
                        }
                        if (leftPlayerTargetY < 0)
                        {
                            leftPlayerTargetY = 0;
                        }
                        if (leftPlayerTargetY > 24)
                        {
                            leftPlayerTargetY = 24;
                        }
                    }
                    if (ballVX > 0)
                    { // moving to the right
                        rightPlayerTargetY = calculateEndPoint(ballX, ballY, ballVX, ballVY, playerLoss != 1) - 3;
                        if (playerLoss == 1)
                        { // we need to lose
                            if (rightPlayerTargetY < 16)
                            {
                                rightPlayerTargetY = 19 + random(5);
                            }
                            else
                            {
                                rightPlayerTargetY = random(5);
                            }
                        }
                        if (rightPlayerTargetY < 0)
                        {
                            rightPlayerTargetY = 0;
                        }
                        if (rightPlayerTargetY > 24)
                        {
                            rightPlayerTargetY = 24;
                        }
                    }
                }

                if (ballY < 0)
                {
                    ballY = 0;
                }
                if (ballY > 30)
                {
                    ballY = 30;
                }
            }
            // show stuff on the display
            drawNet();
            drawPlayer(0, (int)leftPlayerY);
            drawPlayer(62, (int)rightPlayerY);
            drawBall(ballX, ballY);
            drawTime(HourInt, MinuteInt);
        }
        matrix.show();
    }
}
void drawNet()
{
    for (int i = 1; i < 32; i += 2)
    {
        matrix.drawPixel(31, i, myYELLOW);
    }
}
void drawPlayer(int x, int y)
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            matrix.drawPixel(x + i, y + j, myYELLOW);
        }
    }
}
void drawBall(int x, int y)
{
    matrix.drawPixel(x, y, myYELLOW);
    matrix.drawPixel(x + 1, y, myYELLOW);
    matrix.drawPixel(x, y + 1, myYELLOW);
    matrix.drawPixel(x + 1, y + 1, myYELLOW);
}
float calculateEndPoint(float x, float y, float vx, float vy, int hit) //(float ty)
{
    while (true)
    {
        x += vx;
        y += vy;
        if (hit)
        {
            if (x >= 60 || x <= 2)
            {
                return y;
            }
        }
        else
        {
            if (x >= 62 || x <= 0)
            {
                return y;
            }
        }
        if (y = 30 || y <= 0)
        {
            vy = -vy;
        }
        // return y;
    }
}
void drawTime(int Hour, int Minute)
{
    timeStr[1] = 48 + (HourInt % 10);
    if (HourInt > 9)
    {
        timeStr[0] = 48 + (HourInt / 10);
    }
    else
    {
        timeStr[0] = 32;
    }
    matrix.setTextSize(0);
    matrix.setCursor(19, 2);
    matrix.setTextColor(myYELLOW);
    matrix.print(timeStr);

    timeStr[1] = 48 + (MinuteInt % 10);
    if (MinuteInt > 9)
    {
        timeStr[0] = 48 + (MinuteInt / 10);
    }
    else
    {
        timeStr[0] = 48;
    }
    matrix.setTextSize(0);
    matrix.setCursor(33, 2);
    matrix.setTextColor(myYELLOW);
    matrix.print(timeStr);
}

void TetrisClock()
{
    static bool displaydate = 0;

        tetris.scale = 1; // must be called before setText, setTime or setNumbers
        setMatrixDate();

        tetris.scale = 2; // must be called before setText, setTime or setNumbers
        setMatrixTime();
        
        while (!finishedAnimating)
        {
        matrix.fillScreen(tetris.tetrisBLACK);

        tetrisDate0.drawText(0, 48);
        tetrisDate1.drawText(0, 63);
        // tetrisDate1.drawText(15, 55);
        finishedAnimating = tetris.drawNumbers(2, 26, true);
        matrix.show();
        delay(100);
        }

}


void setMatrixTime()
{
    String timeString = "";
    String AmPmString = "";;
    int daylength;
    if (twelveHourFormat)
    {
        // Get the time in format "1:15" or 11:15 (12 hour, no leading 0)
        // Check the EZTime Github page for info on
        // time formatting
        timeString = HourString + ":" + MinuteString;

        //tetrisDate0.setText(DateString, forceRefresh);
        //tetrisDate1.setText(DateString, forceRefresh);
        // If the length is only 4, pad it with
        //  a space at the beginning
        if (timeString.length() == 4)
        {
            timeString = " " + timeString;
        }

        // Get if its "AM" or "PM"
        AmPmString = myTZ.dateTime("A");
        if (lastDisplayedAmPm != AmPmString)
        {
            Serial.println(AmPmString);
            lastDisplayedAmPm = AmPmString;
            // Second character is always "M"
            // so need to parse it out
            tetris2.setText("M", forceRefresh);

            // Parse out first letter of String
            tetris3.setText(AmPmString.substring(0, 1), forceRefresh);
        }
    }
    else
    {
        // Get time in format "01:15" or "22:15"(24 hour with leading 0)
        timeString = myTZ.dateTime("H:i");
    }
    // Only update Time if its different
    if (lastDisplayedTime != timeString)
    {
        Serial.println(timeString);
        lastDisplayedTime = timeString;
        tetris.setTime(timeString, forceRefresh);

        // Must set this to false so animation knows
        // to start again
        finishedAnimating = false;
    }
}
void setMatrixDate()
{
    readtime();
    int daylength;
    Serial.print("TODAY IS ");
    Serial.println(DayString);
    Serial.print("TODAYS DATE IS ");
    Serial.println(DateString);

    tetrisDate0.setText(DayString, forceRefresh);
    tetrisDate1.setText(DateString, forceRefresh);

    finishedAnimating = false;
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