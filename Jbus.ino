#include <Arduino.h>
#include "Jbus_Arduino.h"

Jbus devName;
ActionParameter ap;
char commands[MAX_COMMANDS][MAX_CMD_LEN] = {0};       // 2D char array! 4 rows of 10 chars

void setup()
{
  Serial.begin(115200);
  devName.init(115200);

  devName.debugMode(DEBUG_RAW);
  devName.setMyName("Grippa");

}

//f|Grippa{OPEN}3

void loop()
{  
  //devName.send("CHECK:12345678", nullptr);
  char static commands[MAX_COMMANDS][MAX_CMD_LEN] = {0};       // 2D char array! 4 rows of 10 chars 
  int commandCount = devName.poll(commands);
  for (int i = 0; i < commandCount; i++) // only prints when commandCount is > than 0
    {
      Serial.print("["); Serial.print(i); Serial.print("] = "); Serial.print(commands[i]); Serial.print("\t");
      if (i + 1 == commandCount) Serial.println();
    }

}