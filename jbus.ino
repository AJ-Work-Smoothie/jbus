#include "jbus.h"

jbus asa;

byte *tableCommand;
byte *rcvMsg;

byte arr[] = { 2, 2, 0, 6};

void setup()
{
  while (!Serial){}
  Serial.begin(115200);
  asa.init(115200);
  while(!asa.signOnASA()) { ; }
}

void loop()
{

}