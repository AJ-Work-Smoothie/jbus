#include <Arduino.h>
#include "Jbus_3_0A.h"

Jbus_3_0A devName;

void setup()
{
  devName.init(115200);
}

void loop()
{  
  char commands[MAX_COMMANDS][MAX_CMD_LEN] = {0};       // 2D char array! 4 rows of 10 chars 
  int commandCount = devName.poll(commands);
  for (int i = 0; i < commandCount; i++) // only prints when commandCount is > than 0
    {
      Serial.print("["); Serial.print(i); Serial.print("] = "); Serial.print(commands[i]); Serial.print("\t");
      if (i + 1 == commandCount) 
        {
          Serial.println();
          devName.send("ASA", "CMD1", "CMD2", nullptr);
        }
    }

  Serial.println("GOOOOOOOOOD!");

}




  // if (Serial.available())
  //   {
  //     char c = Serial.read();
  //     switch(c)
  //       {
  //         case 'a': devName.send("Hipa", "World", "Beans", "Bark", nullptr); break;
  //       }
  //   }
