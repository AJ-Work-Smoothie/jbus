#include "jbus.h"

#define SLAVE_ADDY 11

jbus bus(SLAVE_ADDY);
byte *p;

byte signOn[] = {0x10, 'A', 'S', 'S', '!' };


void setup()
{
  Serial.begin(115200);
  bus.init(9600);
  bus.debugRaw = true;
}


void loop()
{
  byte *p = bus.poll();
  // if (*p)
  //   {
  //     while (*p)
  //       {
  //         Serial.print(*p, HEX);
  //         Serial.print(" ");
  //         p++;
  //       }
  //     Serial.println();
  //   }


  // while (!done)
  //   {
  //     byte *p = bus.poll();
  //     Serial.println(*p);
  //     if (*p)
  //       {
  //         // for (int i = 0; i < 5; i++)
  //         //   {
  //         //     Serial.print(p[i]);
  //         //     Serial.print(" ");
  //         //   }
  //         done = true;
  //         //Serial.println();
  //       }
  //   }

  // if (Serial.available() > 0 )
  //   {
  //     char c = Serial.read();
  //     switch(c)
  //       {
  //         case 'a':
  //             bus.send(SLAVE_ADDY, false, signOn, sizeof(signOn));
  //         break;
  //         case 'b':
  //           for (int i = 0; i < 20; i++)
  //             bus.send(SLAVE_ADDY, false, signOn, sizeof(signOn));
  //         break;
  //         case 'c': done = false; break;
  //         case 'd': done = true; break;
  //       }
  //   }


/*     if (*p >> 7)
      { 
        byte msg[] = { 'B', 'O', 'O', 'B', 'S', '!', 0 };
        bus.send(SLAVE_ADDY, false, msg);

        while (*p)
          {
            Serial.print(*p, HEX);
            Serial.print(" ");
            p++;
          }
        Serial.println();
    }
 */
}