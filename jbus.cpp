#include "jbus.h"

jbus::jbus()
{
  

}

void jbus::init(unsigned long buad)
{
  cereal.begin(buad);
  samd21Port1Begin(buad);
}

byte* jbus::poll(int msgLen)
{
  // we pass in the msgLen, so the array len is msgLen+3 (MSGstart, checksum, MSGend)
  static int arrLen = msgLen + 3; // arrlen needs to stay for when Serial is NOT available
  static byte packet[MAXLEN]; // if not declared static, the function return does not work. 
  
  for (int i = 0; i < arrLen; i++) packet[i] = 0; // erase all contects of packet

  if (cereal.available() >= arrLen) 
    { 
      //for (int i = 0; i < arrLen; i++) packet[i] = 0; // erase all contects of packet
      // rcv message   
      for (int i = 0; i < arrLen; i++)
        {
          packet[i] = cereal.read();
          //Serial.print(packet[i]);
          //Serial.print(" ");
        }
      //Serial.println();

      // calculate checksum
      checksum = 0; // reset before each calc
      for (int i = 0; i < arrLen - 2; i++)
        {
          checksum ^= packet[i];
        }
      //Serial.print("   Checksum Calc: "); Serial.print(checksum);
      //Serial.println(); 

      // if checksum good
      if (checksum == packet[arrLen - 2])
        {
          // Serial.print("MSG GOOD! ");
          // for (int i = 0; i < arrLen; i++)
          //   {
          //     Serial.print(packet[i]);
          //     Serial.print(" ");
          //   }
          // Serial.println();

          return packet;
        }
      
  // if checksum is bad
      else 
        {
          Serial.println("CHECKSUM IS BAD");
          for (int i = 0; i < arrLen; i++) packet[i] = 0; // erase all contects of packet
          
          return 0; // FAILED
        }
    } // if cereal.avilable

  else return 0; 

}

bool jbus::send(byte arr[], int msgLen)
{
  // arr is the message we want to send. We create an array that is +3 bigger for 
  // MSGstart, checksum, and MSGend
  int packetLen = msgLen + 3;
  byte packet[packetLen];

  for (int i = 0; i < packetLen; i++)
    packet[i] = 0; // initialize all values to 0

  packet[0] = MSGSTART;
  for (int i = 1; i < msgLen + 1; i++ ) 
    packet[i] = arr[i - 1]; // copies arr into outgoing packet
  for (int i = 0; i < packetLen - 2; i++) 
    packet[packetLen - 2] ^= packet[i]; // simple XOR checksum  
  packet[packetLen - 1] = MSGEND; // remember, zero indexed
  cereal.write(packet, packetLen);

  for (int i = 0; i < packetLen; i++)
    {
      Serial.print(packet[i]);
      Serial.print(" ");
    }
  Serial.println();
}

/* void jbus::testWrite()
{
  for (int i = 0; i < arrLen; i++) packet[i] = 0; // clear out packet

  packet[0] = MSGSTART;
  packet[1] = 1;
  packet[2] = 2;
  packet[3] = 3;
  packet[4] = 4; 
  for (int i = 0; i < arrLen - 2; i++) packet[arrLen - 2] ^= packet[i]; // simple XOR checksum
  packet[6] = MSGEND;
  cereal.write(packet, arrLen);
}

void jbus::testRead()
{
  if (cereal.available())
    {
      byte b = cereal.read();
      Serial.print(b);
      if (b == 0xFE)  Serial.println();
    }
} */

/* This code is perfect! Just didn't need it
byte* jbus::pollCont()
{
  if (cereal.available() >= BUFFLEN) 
    { 
      clearPacket(); // erase all contects of packet
      // rcv message   
      for (int i = 0; i < BUFFLEN; i++)
        {
          buffer[i] = cereal.read();
        }
      
      int pos = 0;
      for ( ; buffer[pos] != MSGSTART; pos++) // increment through buffer until we've found MSGSTART
        ;
      for (int i = pos; i < (pos + arrLen); i++)
        {
          packet[i - pos] = buffer[i]; // copy buffer into packet, zero indexed
          // Serial.print(packet[i - pos]);
          // Serial.print(" ");
        }
      // Serial.println();

      // calculate checksum
      checksum = 0; // reset before each calc
      for (int i = 0; i < arrLen - 2; i++)
        {
          checksum ^= packet[i];
        }
      //Serial.print("   Checksum Calc: "); Serial.print(checksum);
      //Serial.println(); 

      // if checksum good
      if (checksum == packet[arrLen - 2])
        {
          // Serial.print("MSG GOOD! ");
          // for (int i = 0; i < arrLen; i++)
          //   {
          //     Serial.print(packet[i]);
          //     Serial.print(" ");
          //   }
          // Serial.println();

          clearBuffer();
          return packet;
        }
      
  // if checksum is bad
      else 
        {
          Serial.println("CHECKSUM IS BAD");
          clearBuffer();
          
          return 0; // FAILED
        }
    } // if cereal.avilable

  else return 0; 

} */