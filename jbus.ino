#include "jbus.h"

jbus bus;

void setup()
{
  Serial.begin(115200);
  bus.init(115200);

}

void loop()
{

  bus.poll(4);
  //bus.send();

  for(int i = 0; i < 1000; i++)
    {
      Serial.println(i);
    }

}