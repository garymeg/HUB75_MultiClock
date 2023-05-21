static String timeStr = "00";
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
int HourOld, MinuteOld, secondOld, DateInt;
unsigned long lastTime = millis();
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
                                // rightPlayerTargetY -= 1 + random(3);
                            }
                            else
                            {
                                // leftPlayerTargetY -= 1 + random(3);
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
