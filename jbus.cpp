#include "jbus.h"

jbus::jbus()
{
  badMsgBytePtr = &badMsgByte; // set badMsg to point to badmsg, a byte equaling 0;
}

jbus::jbus(byte slaveAddress)
{
  badMsgBytePtr = &badMsgByte; // set badMsg to point to badmsg, a byte equaling 0;
  _slaveAddress = slaveAddress;
}

void jbus::init(unsigned long buad)
{
  cereal.begin(buad);
  samd21Port1Begin(buad);
}

byte* jbus::poll()
{
  static byte rawBuffer[MAXLEN];
  static byte packet[MAXLEN];
  static int bufferIndex = 0;
  static int SOM = 0;

  if (cereal.available() > 0)
    {
      if (bufferIndex >= MAXLEN) bufferIndex = 0;
      rawBuffer[bufferIndex++] = cereal.read();
      // if we get a byte that is start byte, let's save that index
      if (rawBuffer[bufferIndex - 1] == STARTBYTE) SOM = bufferIndex - 1;
      if (rawBuffer[bufferIndex - 1] == ENDBYTE)
        {
          int packetLen = bufferIndex - SOM;
          // return if the message is too short
          if (packetLen <= WRAPPER_COUNT) return badMsgBytePtr;
          // copy the message from rawBuff into the packet array
          for (int i = SOM; i < SOM + packetLen; i++) packet[i - SOM] = rawBuffer[i];

          if (debugMode)
            {
              for (int i = 0; i < packetLen; i++) 
                {
                  Serial.print(packet[i], HEX); 
                  Serial.print(" ");
                }
              Serial.println();
            }

          int tempCheckSum = calcChecksum(packet, packetLen); // get the checksum
          // if the checksum fails
          if (!tempCheckSum == rawBuffer[packetLen - 2]) return badMsgBytePtr;
          // truncate the startbyte, checksum, and endbyte
          for (int i = 0; i < packetLen - 3; i++) 
              {
                packet[i] = packet[i + 1];
                if (i == packetLen - 4) packet[i + 1] = 0; // set the last byte to 0
              }      
          return packet;
        }    
    } // if cereal.available
  return badMsgBytePtr;
}

void jbus::send(byte address, bool requestData, byte msgArr[], int msgLen)
{
  // arr is the message we want to send. We create an array that is +4 bigger for our WRAPPER_COUNT
  int packetLen = msgLen + WRAPPER_COUNT;
  byte packet[packetLen];
  for (int i = 0; i < packetLen; i++) packet[i] = 0; // initialize all values to 0

  packet[0] = STARTBYTE; // start byte
  if (requestData) { packet[1] = (address |= 1<<7); } // if we are requesting data, set the MSB to 1,
  else { packet[1] = (address = address &= ~(1<<7)); } // if we are sending data, set the MSB to 0
  for (int i = 2; i < msgLen + 2; i++ ) 
    packet[i] = msgArr[i - 2]; // copies arr into outgoing packet
  packet[packetLen - 2] = calcChecksum(packet, packetLen);
  packet[packetLen - 1] = ENDBYTE; // remember, zero indexed
  cereal.write(packet, packetLen);

  if (debugMode)
    {
      Serial.print("MSG SENT: ");
      for (int i = 0; i < packetLen; i++)
        {
          Serial.print(packet[i], HEX);
          Serial.print(" ");
        }
      Serial.println();
    }
}

byte jbus::calcChecksum(byte *buffPtr, int packetLen)
{
  byte checksum = 0;
  // we are going to skip the first 0xFF, because it's always just that. 
  // The checksum is the Address ^(XOR) Message. 
  for (int i = 1; i < packetLen - 2; i++) { checksum ^= buffPtr[i]; }
  if (checksum >= 0xFE) checksum -= 2; // this prevents it from ever being anything from 0xFA to 0xFF
  if (debugMode) { Serial.print("Checksum Calc: "); Serial.print(checksum, HEX); Serial.println(); }
  return checksum;

  //Serial.print("Packet Length: "); Serial.println(packetLen);
  // Serial.print("Message Rxd is: ");
  // for (int i = 0; i < packetLen; i++)
  //   {
  //     Serial.print(buffPtr[i], HEX);
  //     Serial.print(" ");
  //   }
  // Serial.println();
}




  // Serial.print("RAW BUFFER: ");
  // for (int i = 0; i < MAXLEN; i++)
  //   {
  //     //Serial.print(i); Serial.print(":");
  //     Serial.print(rawBuffer[i], HEX);
  //     Serial.print(" ");
  //   }
  // Serial.println();