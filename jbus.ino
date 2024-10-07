#include <P1AM.h>
#include "jbus.h"
// master sketch
jbus bus;
byte arr[5] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
void setup()
{
  Serial.begin(115200);
  bus.init(115200);
  //bus.debugWrite = true;
  
  Serial.println(); Serial.println("Ready to go!"); Serial.println();

  //while (!Serial);
}


void loop()
{
  //bus.send(11, arr, sizeof(arr));

  byte *p = bus.poll();
  if (*p)
    {
      while (*p)
        {
          Serial.print(*p++, HEX);
          Serial.print(" ");
        }
      Serial.println();
    }
}