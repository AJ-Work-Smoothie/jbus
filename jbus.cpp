#include "jbus.h"

jbus::jbus()
{
  badMsgBytePtr = &badMsgByte; // set badMsg to point to badmsg, a byte equaling 0;
}

jbus::jbus(byte serverAddress)
{
  badMsgBytePtr = &badMsgByte; // set badMsg to point to badmsg, a byte equaling 0;
  _slaveAddress = serverAddress;
}

void jbus::init(unsigned long buad)
{
  cereal.begin(buad);
  samd21Port1Begin(buad);
}

byte* jbus::poll(int msgLen)
{
  static byte packet[MAXLEN]; // holds the entire raw packet
  static int packetLen = msgLen + WRAPPER_COUNT; // arrlen needs to stay for when Serial is NOT available
  
  for (int i = 0; i < MAXLEN; i++) packet[i] = 0; // erase all contects of packet

  if (cereal.available() >= packetLen) 
    { 
      for (int i = 0; i < packetLen; i++) 
        { 
          packet[i] = cereal.read(); // get packs from the serial port!
          //Serial.print(packet[i], HEX); Serial.print(" ");
        }
      //Serial.println();

      if (verifyChecksum(packet, packetLen)) 
        { 
          static byte message[MAXLEN];
          for (int i = 0; i < MAXLEN; i++) message[i] = 0; // erase the packet
          // we are starting at + 1 because the first byte is the start byte. MSG len does not account for the address, so let's add that
          for (int i = 1; i < 1 + (msgLen + 1); i++) { message[i - 1] = packet[i]; } // copy the message into the message array - wrappers (keeping address)
          // If the message isn't for us, let's ignore it
          if ((packet[1] & 0x7F) != _slaveAddress) return badMsgBytePtr; // if the address is not for us, return a bad message (0x00
          return message;
        }
      else 
        {
          Serial.println("CHECKSUM IS BAD");
          for (int i = 0; i < MAXLEN; i++) packet[i] = 0; // erase all contects of packet
          while (cereal.read() != ENDBYTE) { ; } // clear out serial buffer until we find an end byte, so the next one we read is the start byte
          return badMsgBytePtr;
        }
    } // if cereal.avilable

  else return badMsgBytePtr; 
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
/*
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
*/
bool jbus::verifyChecksum(byte *arrPtr, int arrLen)
{
  byte checksum = 0;
  // this gets everything from the beginning of the array up to the checksum - 1
  for (int i = 0; i < arrLen - 2; i++) { checksum ^= arrPtr[i]; }
  checksum = checksum - 2; // this prevents it from ever being FF or FE
  //Serial.print("   Checksum Calc: "); Serial.print(checksum); Serial.println();
  if (checksum == arrPtr[arrLen - 2]) { return true; }
  return false;
}

byte jbus::calcChecksum(byte *arrPtr, int arrLen)
{
  byte checksum = 0;
  // this gets everything from the beginning of the array up to the checksum - 1
  for (int i = 0; i < arrLen - 2; i++) { checksum ^= arrPtr[i]; }
  checksum = checksum - 2; // this prevents it from ever being FF or FE
  //Serial.print("   Checksum Calc: "); Serial.print(checksum); Serial.println();
  return checksum;
}

void jbus::reset()
{
  for (int i = 0; i < 64; i++) { cereal.read(); } // clear out serial buffer
}
