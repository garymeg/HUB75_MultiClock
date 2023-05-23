TetrisMatrixDraw tetris(matrix);  // Main clock
TetrisMatrixDraw tetris2(matrix); // The "M" of AM/PM
TetrisMatrixDraw tetris3(matrix); // The "P" or "A" of AM/PM
TetrisMatrixDraw tetrisDate0(matrix);
TetrisMatrixDraw tetrisDate1(matrix);
TetrisMatrixDraw tetrisDate2(matrix);
bool twelveHourFormat = false;
bool forceRefresh = false;
bool showColon = true;
volatile bool finishedAnimating = false;
String lastDisplayedAmPm = "";

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
    String AmPmString = "";
    ;
    int daylength;
    if (twelveHourFormat)
    {
        // Get the time in format "1:15" or 11:15 (12 hour, no leading 0)
        // Check the EZTime Github page for info on
        // time formatting
        timeString = HourString + ":" + MinuteString;

        // tetrisDate0.setText(DateString, forceRefresh);
        // tetrisDate1.setText(DateString, forceRefresh);
        //  If the length is only 4, pad it with
        //   a space at the beginning
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
