#include <P1AM.h>
#include "jbus.h"
// master sketch
jbus bus;
byte arr[5] = {1, 2, 3, 4, 5};
void setup()
{
  Serial.begin(115200);
  bus.init(250000);
  
  bus.debugWrite = true;
  
  Serial.println(); Serial.println("Ready to go!"); Serial.println();

  //while (!Serial);
}


void loop()
{
  bus.send(0x11, arr, sizeof(arr));
  delay(1000);
  // byte *p = bus.poll();
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