void twitchLoop() 
{

  // Try to connect to chat. If it loses connection try again
  if (!client.connected()) {
    Serial.println("Attempting to connect to " + ircChannel );
    // Attempt to connect
    // Second param is not needed by Twtich
    if (client.connect(Twitch_nick, "", Twitch_oauth_token)) 
      {
      client.sendRaw("JOIN " + ircChannel);
      Serial.println("Clock connected and ready to rock");
      sendTwitchMessage("Clock connected and ready to rock!");
      }
      else 
      {
        Serial.println("failed... try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    return;
  }
  client.loop();
}

void sendTwitchMessage(String message) {
  client.sendMessage(ircChannel, message);
}


void callback(IRCMessage ircMessage) 
{
  //Serial.println("In CallBack");
  String cType = "";
  
  if (ircMessage.command == "PRIVMSG" && ircMessage.text[0] != '\001') {
    //Serial.println("Passed private message.");
    
    //ircMessage.nick.toUpperCase();
    ircMessage.text.toUpperCase();
    Serial.println(ircMessage.text.indexOf("!CLOCK"));
    if (ircMessage.text.indexOf("!CLOCK") >= 0)
      {
      ircMessage.text.replace(" ", "");
      uint8_t commandPosition = ircMessage.text.indexOf("!CLOCK") + 6;
      uint8_t nextCommandPosition = ircMessage.text.length()-1;

      cType = ircMessage.text.substring(commandPosition, ircMessage.text.length());
      cType.toUpperCase();
      
      Serial.print("Command Position ");
      Serial.println(commandPosition);
      Serial.println(cType);
      if (cType == "TETRIS")
        displayMode = 4;
      if (cType == "ANALOG")
        displayMode = 2;
      if (cType == "PONG")
        displayMode = 3;
      ClockLastChange = millis();
      }
    if (ircMessage.text.indexOf("!MSG") >= 0)
      {
      uint8_t commandPosition = ircMessage.text.indexOf("!MSG") + 4;
      //uint8_t nextCommandPosition = ircMessage.text.length() - 1;

      cType = ircMessage.text.substring(commandPosition, ircMessage.text.length());
      


    
      String message("<" + ircMessage.nick + "> " + ircMessage.text);
      myMessages.push_back(message);
      stat_ScrollingText = 1;
      Serial.println(message);
      }

    // prints chat to serial
    
    return;
  }
}
