/*
        Script for DDNS for Xboard
        Todo:
                turn it into a library
                clean up and comment
        
        Licence CC = CC BY-SA
        Tested on Arduino Xboard, written by Jose
*/


#include "DDNS.h"




DDNS_c::DDNS_c( )
{
	ipadsl = INADDR_NONE;

	ipcheck = DDNS_CHECK_IP;
    checkPort = DDNS_CHECK_PORT;

}

void DDNS_c::init( IPAddress ddnsPara, const char *pUser, const char *pPwd )
{
	ddns = ddnsPara;
	phostname = pUser;
	puserpwdb64 = pPwd;
}



void DDNS_c::update( )
{
  int timeout=0;
  int counter = 0;
  int ipi = 0;
  int codetemp=0;

  IPAddress ipadslold;

  Serial.print(F("getting current IP \n"));
  Serial.print(F("connecting to "));
  Serial.print(ipcheck);

  if (client.connect(ipcheck, 8245)) 
  {
    Serial.println(F("\n       client connected "));
    client.println(F("GET / HTTP/1.0"));
    client.println();
  } 
  else {
    Serial.println(F("\nconnection failed"));
  }

  while (!client.available() && timeout<50)
  {
    timeout++;
    Serial.print(F("Time out "));
    Serial.println(timeout);
    delay(100);
  }

  while (client.available())
  {
    if (client.available())
    { 
      char c = client.read();
      if (c == '\n')
      {
        counter++;
      } 
      if (counter == 9 && c != '\n')
      {
        if(c == '.' )
        {
          ipadsl[ipi] = codetemp;
          codetemp = 0;
          ipi++;
        } 
        else {
          codetemp = codetemp *10 + (c-0x30);
        }
        ipadsl[3] = codetemp;
      }
    }
  }
  client.flush();

  if (!client.connected())
  {
    Serial.println(F("disconnecting. \n"));
    client.stop();
    delay (1000);
    if ( ipadsl == INADDR_NONE )
	{
      //nothing
	}
	else
    {
      Serial.print(F("New IP: >"));
      Serial.print(ipadsl);
      Serial.println(F("<"));             
      for (int i=0; i<4; i++) {    //save in memory
        ipadslold[i] = EEPROM.read(i);
      }
      Serial.print(F("Old IP: >"));
      Serial.print(ipadslold);
      Serial.println(F("<"));
      if (ipadsl != ipadslold)
      {
        Serial.println(F("IP different from PUBLIC IP"));
        ddns_changer();
      }
      else
      {
        Serial.println(F("same IP"));
      }

      ipadslold = ipadsl;
      for (int i=0; i<4; i++)
      {
        EEPROM.write(i, ipadsl[i]);                // http://www.arduino.cc/en/Reference/EEPROMWrite
      }
      Serial.println(F("IP saved !"));
    }
	
  }
}



void DDNS_c::ddns_changer()
{
  int timeout=0;
  Serial.print(F("connecting to"));
  Serial.println(ddns);

  Serial.print(F("Public IP"));
  Serial.println(ipadsl);

  if (client.connect(ddns, 80)) {
    Serial.println(F("connected"));
    client.print(F("GET /nic/update?hostname="));
    client.print(phostname);
    client.print(F("&myip="));
    client.print(ipadsl);
    client.println(F(" HTTP/1.1 "));
    client.print(F("Host: "));
    client.println(ddns);

    client.print(F("Authorization: Basic "));
    client.println(puserpwdb64);

    client.println(F("User-Agent: DFRobot - jose at dfrobot.com"));
    client.println();
  } 
  else {
    Serial.println(F("connection failed"));
  }

  while (!client.available() && timeout<50)
  {
    timeout++;
    Serial.print(F("Time out"));
    Serial.println(timeout);
    delay(100);
  }

  while (client.available())
  {
    if (client.available())
    {
      char c = client.read();
      Serial.print(c);
    }
  }

  if (!client.connected())
  {
    Serial.println();
    Serial.println(F("disconnecting."));
    client.stop();
  }
}

