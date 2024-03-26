#include "jbus.h"

#define SLAVE_ADDY 11

jbus bus(SLAVE_ADDY);
byte *p;


void setup()
{
  Serial.begin(115200);
  bus.init(9600);
  bus.debugMode = false;
  bus.debugChecksum = false;
}


void loop()
{
  byte *p = bus.poll();

  if (*p)
    {
      while (*p)
        {
          Serial.print(*p, HEX);
          Serial.print(" ");
          p++;
        }
      Serial.println();
    }

    // if (*p >> 7)
    //   { 
    //     byte msg[] = { 'B', 'O', 'O', 'B', 'S', '!', 0 };
    //     bus.send(SLAVE_ADDY, false, msg);

    //     while (*p)
    //       {
    //         Serial.print(*p, HEX);
    //         Serial.print(" ");
    //         p++;
    //       }
    //     Serial.println();
    // }

}