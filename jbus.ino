#include "jbus.h"

#define SERVER_ADDY 10

jbus bus(SERVER_ADDY);
byte *p;

// bool newData = false;
// byte buffer[128];
// int newDataLen = 0;
// bool pendingMsg = false;



void setup()
{
  Serial.begin(115200);
  bus.init(9600);

  bus.debugMode = true;
}


void loop()
{
  byte *p = bus.poll(2);
  // this only grabs the lower 7 bits of the address
  if (*p != 0x00)
    {
      if ((*p >> 7) == 1) Serial.println("Required to respond!");
        
      while (*p != 0x00)
        {
          Serial.print(*p, HEX);
          Serial.print(" ");
          p++;
        }
      Serial.println();
    }
}