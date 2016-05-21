#include <Ethernet.h>
#include <SPI.h>
#include <EEPROM.h>
#include "DDNS.h"


/************ ETHERNET STUFF ************/
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 
  192, 168, 3, 222 };
byte gateway[] = { 
  192, 168, 1, 10 };   //your router's IP address
byte subnet[] = { 
  255, 255, 255, 0 };    //subnet mask of the network 
EthernetServer server(80);
EthernetClient client;

DDNS_c DDNS;

IPAddress ddnsIP(204,16,170,42); // http://nic.ChangeIP.com/nic/update
const char hostname[ ] PROGMEM  = "username";
const char userpwdb64[ ] PROGMEM = "x";  //http://www.functions-online.com/base64_encode.html    use->   username:password


void setup() {

  /************* web server and sd conf **************/

  pinMode(10, OUTPUT);                       // set the SS pin as an output (necessary!)
  digitalWrite(10, HIGH);                    // but turn off the W5100 chip!


  //root.ls(LS_DATE | LS_SIZE);
  Serial.println();

  // Debugging complete, we start the server!
  Ethernet.begin(mac, ip); 
  server.begin();
  //Serial.println("end");

  DDNS.init(ddnsIP, hostname,userpwdb64);

}


/*
  These are the main functions for the Dynamic DNS client for arduino/smarthome
 
 */


void loop() 
{
  DDNS.update();
  
}

