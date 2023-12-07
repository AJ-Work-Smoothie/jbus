#include "jbus.h"

jbus bus;

void setup()
{
  Serial.begin(115200);
  bus.init(115200);

}

void loop()
{

  //bus.poll();
  //bus.send();

}