
void handleServer()
{
  server.on("/", handleRoot);
  server.on("/myClock", handleChangeOfClock);
  server.on("/mySlots", handleSlots);
  server.on("/myMessage", handleMessage);
  server.begin();
}

void handleChangeOfClock()
{
  displayMode = server.arg("ClockMode").toInt();
  Serial.print("display Mode: ");
  Serial.println(displayMode);
  handleRoot();
}
void handleMessage()
{
  scrollMessage = server.arg("messageTXT");
  myMessages.push_back(scrollMessage);
  changeState(stat_NeedToChange);
  Serial.print("Sending Message: ");
  Serial.println(scrollMessage);
  handleRoot();
}

void handleSlots()
{
  oldDisplayMode = displayMode;
  displayMode = 3;
  Player = server.arg("userName");
  std::transform(Player.begin(), Player.end(), Player.begin(), ::toupper);
  PlayerPoints = server.arg("userPoints").toInt();
  PlayerBet = server.arg("userBet").toInt();
  Serial.print("display Mode: ");
  Serial.println(displayMode);
  Serial.print("Player: ");
  Serial.println(Player);
  Serial.print("Points: ");
  Serial.println(PlayerPoints);
  Serial.print("Bet: ");
  Serial.println(PlayerBet);
  handleRoot();
  
}

void handleRoot()
{
  server.send(200, "text/HTML", MAIN_page);
}