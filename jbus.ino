#include "jbus.h"

jbus asa;

byte *tableCommand;

byte arr[] = { 1, 2, 3 };

void setup()
{
  Serial.begin(115200);
  asa.init(115200);
}

void loop()
{
  //asa.testWrite();
  //delay(10);

  asa.send(arr, sizeof(arr));
  delay(20);

  // tableCommand = asa.poll();
  // if (*tableCommand == 0xFF) // if buffer[i] = 0xFF, we must have gotten a new msg!
  //   {
  //     for (int i = 0; i < JBUS_PACK_LEN; i++)
  //       {
  //         Serial.print(tableCommand[i]);
  //         Serial.print(" ");
  //       }
  //     Serial.println();

  //  }
}