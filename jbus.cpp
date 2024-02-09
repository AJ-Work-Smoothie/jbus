#include "jbus.h"

jbus::jbus()
{
  badMsgBytePtr = &badMsgByte; // set badMsg to point to badmsg, a byte equaling 0;
}

void jbus::init(unsigned long buad)
{
  cereal.begin(buad);
  samd21Port1Begin(buad);
}

byte* jbus::poll(int msgLen)
{
  //while(Serial.available()) { ; } // wait for messages to arrive
  // we pass in the msgLen, so the array len is msgLen+3 (PACKETSTART, checksum, PACKETEND)
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
          if (debugMode)
           {
              Serial.print("MSG RECEIVED GOOD! ");
              for (int i = 0; i < arrLen; i++)
                {
                  Serial.print(packet[i]);
                  Serial.print(" ");
                }
              Serial.println();
           }

          return packet;
        }
      
  // if checksum is bad
      else 
        {
          Serial.println("CHECKSUM IS BAD");
          for (int i = 0; i < arrLen; i++) packet[i] = 0; // erase all contects of packet
          reset(); // completely clears out the serial buffer
          return badMsgBytePtr; // FAILED
        }
    } // if cereal.avilable

  else return badMsgBytePtr; 

}

void jbus::debugSet(bool mode)
{
  debugMode = mode;
}

void jbus::send(byte msgArr[], int msgLen, bool requestData)
{
  // arr is the message we want to send. We create an array that is +3 bigger for 
  // PACKETSTART, checksum, and PACKETEND
  int packetLen = msgLen + 3;
  byte packet[packetLen];

  for (int i = 0; i < packetLen; i++)
    packet[i] = 0; // initialize all values to 0

  if (requestData) packet[0] = REQUEST;
  else packet[0] = PACKETSTART;

  for (int i = 1; i < msgLen + 1; i++ ) 
    packet[i] = msgArr[i - 1]; // copies arr into outgoing packet
  for (int i = 0; i < packetLen - 2; i++) 
    packet[packetLen - 2] ^= packet[i]; // simple XOR checksum  
  packet[packetLen - 1] = PACKETEND; // remember, zero indexed
  cereal.write(packet, packetLen);

  if (debugMode)
    {
      Serial.print("MSG SENT: ");
      for (int i = 0; i < packetLen; i++)
        {
          Serial.print(packet[i]);
          Serial.print(" ");
        }
      Serial.println();
    }
}

void jbus::send(byte msgArr[], int msgLen, byte customStartByte)
{
  // arr is the message we want to send. We create an array that is +3 bigger for 
  // PACKETSTART, checksum, and PACKETEND
  int packetLen = msgLen + 3;
  byte packet[packetLen];

  for (int i = 0; i < packetLen; i++)
    packet[i] = 0; // initialize all values to 0

  packet[0] = customStartByte; // our custom start bite!!

  for (int i = 1; i < msgLen + 1; i++ ) 
    packet[i] = msgArr[i - 1]; // copies arr into outgoing packet
  for (int i = 0; i < packetLen - 2; i++) 
    packet[packetLen - 2] ^= packet[i]; // simple XOR checksum  
  packet[packetLen - 1] = PACKETEND; // remember, zero indexed
  cereal.write(packet, packetLen);

  if (debugMode)
    {
      Serial.print("MSG SENT: ");
      for (int i = 0; i < packetLen; i++)
        {
          Serial.print(packet[i]);
          Serial.print(" ");
        }
      Serial.println();
    }
}

void jbus::reset()
{
  for(int i = 0; i < 128; i++)
    {
      // clear out serial buffer
      cereal.read();
    }
}

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
      for ( ; buffer[pos] != PACKETSTART; pos++) // increment through buffer until we've found PACKETSTART
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