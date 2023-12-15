#include "jbus.h"

#include <Adafruit_NeoPixel.h>
#define _neoPix     5
int pixelCount = 55;
int interval = 2;
Adafruit_NeoPixel pixels (pixelCount, _neoPix, NEO_GRB + NEO_KHZ800);

jbus bus;
byte *p;

// bool newData = false;
// byte buffer[128];
// int newDataLen = 0;
// bool pendingMsg = false;



void setup()
{
  pixels.begin();
  Serial.begin(115200);
  bus.init(115200);
  //Serial3.begin(115200);
  
}



void loop()
{

  p = bus.poll(4);
  
  for (int i = 0; i < 7; i++)
    {
      Serial.print(p[i]);
      Serial.print(" ");
    }
  
  // //bus.send();

  //rcv();
  //process();

  

  //pix();



}

/*
void rcv()
{
  static byte rawLen = 0;
  byte EOM = 0xFE;
  byte b; 

   while (Serial3.available() > 0 && !newData)
    {
      b = Serial3.read();
      if (b != EOM)
        {
          buffer[rawLen] = b;
          rawLen++;
          if (rawLen >= 128) rawLen = 127; // constraints
        }
      else 
        {
          buffer[rawLen] = b; // copy last byte into array
          newData = true; // now process data.
          newDataLen = rawLen;
          

          Serial.print("RW: ");
          Serial.print(rawLen);

          rawLen = 0;
        }
 
    }     

}

void process()
{
  if (newData)
    {
      Serial.print("  MSG: ");
      for(int i = 0; i < newDataLen; i++)
        {
          Serial.print(buffer[i]);
          Serial.print(" ");
        }
      Serial.println();
      newData = false;
      newDataLen = 0;
    }
  

}

while (pendingMsg)
    {
      //rcv();
      //process();
      if (Serial3.available() > 0)
        {
          byte b = Serial3.read();
          Serial.print(b);
          Serial.print(" ");

          if (b == 0xFE)
            {
              Serial.println();
              pendingMsg = false;
            }
        }
      
    }

*/

void pix()
{
  static unsigned long pMillis = 0;
  static bool startFlag = false;
  static int brightness = 0;
  static bool fadeIn = true;

  //pixels.clear();

 
  
  if (startFlag)
    {
      pMillis = millis();
      startFlag = false;
    }

  if (millis() - pMillis > interval)
    {
      if (brightness >= 255) fadeIn = false;
      if (brightness <= 0) fadeIn = true;

      for (int i = 0; i < pixelCount; i++) 
        {
          pixels.setPixelColor(i, pixels.Color(0, brightness, 0));
        }
      if (fadeIn) brightness++;
      else brightness--;
      pixels.show();
      pMillis = millis();
      startFlag = true;
    }
}