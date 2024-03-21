#include "jbus.h"

#define SLAVE_ADDY 10

jbus bus(SLAVE_ADDY);
byte *p;

// bool newData = false;
// byte buffer[128];
// int newDataLen = 0;
// bool pendingMsg = false;



void setup()
{
  Serial.begin(115200);
  bus.init(9600);

  bus.debugMode = false;
}


void loop()
{
  byte *p = bus.poll();

  if ((*p & 0x7F) == SLAVE_ADDY)
    {
      if ((*p >> 7) == 1) Serial.print("Required to respond!\t");
        
      while (*p != 0x00)
        {
          Serial.print(*p, HEX);
          Serial.print(" ");
          p++;
        }
      Serial.println();
    }

}