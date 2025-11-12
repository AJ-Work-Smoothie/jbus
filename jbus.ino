#include <Arduino.h>
#include "Jbus_3_0A.h"

Jbus_3_0A devName;
ActionParameter ap;
char commands[MAX_COMMANDS][MAX_CMD_LEN] = {0};       // 2D char array! 4 rows of 10 chars

void setup()
{
  Serial.begin(115200);
  devName.init(9600);

  devName.debugMode(DB_PACKET);
  devName.setMyName("ASA");

}

void loop()
{  
  devName.send("CHECK:12345678");
  // char static commands[MAX_COMMANDS][MAX_CMD_LEN] = {0};       // 2D char array! 4 rows of 10 chars 
  // int commandCount = devName.poll(commands);
  // for (int i = 0; i < commandCount; i++) // only prints when commandCount is > than 0
  //   {
  //     Serial.print("["); Serial.print(i); Serial.print("] = "); Serial.print(commands[i]); Serial.print("\t");
  //     if (i + 1 == commandCount) Serial.println();
  //   }
  
  // devName.send("Hipa", "World", "Beans", nullptr);
  
  delay(5000);

}