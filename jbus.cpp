#include "jbus.h"

jbus::jbus()
{
  badMsgBytePtr = &badMsgByte; // set badMsg to point to badmsg, a byte equaling 0;
  rejectWrongAddress = false;
}

jbus::jbus(byte slaveAddress)
{
  badMsgBytePtr = &badMsgByte; // set badMsg to point to badmsg, a byte equaling 0;
  _slaveAddress = slaveAddress;
  rejectWrongAddress = true;
}

void jbus::init(unsigned long buad)
{
  #ifdef ARDUINO_SAMD_ZERO
     samd21Port1Begin(buad); 
  #else 
    cereal.begin(buad);
  #endif  
  
}

byte* jbus::poll()
{
  /**
   * We initialize buffIndex to 1. Why? Instead of using a flag to figure out if we've gotten a new start byte, we use SOM.
   * If SOM is != 0, that means we got a start message byte. We are reservering zero so that when SOM = 0, we know that 
   * we haven't gotten a start flag. We also have to start buffIndex at 1 so that we don't put our start byte in index 0, making 
   * SOM 0, meaning we'll miss that start byte. We must reset buffIndex to 1 instead of 0.
  */
  static byte buffIndex = 1;
  static byte rawBuffer[MAXLEN];
  static byte packet[MAXLEN];

  while (cereal.available() > 0)
    {
      if (buffIndex >= MAXLEN - 1) buffIndex = 1;
      rawBuffer[buffIndex++] = cereal.read();
      // debugging
      if (debugRaw) 
        { 
          Serial.print(rawBuffer[buffIndex - 1]); Serial.print(" ");  // print the byte that was just read
          if (rawBuffer[buffIndex - 1] == ENDBYTE) Serial.println(); // if we hit the end byte, print a newline
        }
    }


  // find the start and end of the message
  int SOM = 0, EOM = 0, packetLen = 0;
  // gets the start of the message
  for (int i = 0; i < buffIndex - 1; i++)
    {
      // only count the start if the byte before it *wasn't* an escape byte
      if (rawBuffer[i] == STARTBYTE && rawBuffer[i - 1] != ESCAPEBYTE) SOM = i;
    }
  // gets the end of the message
  for (int i = SOM; i < MAXLEN; i++)
    {
      // only set EOM if the when we found an ENDBYTE it wasn't after an escape byte
      if (rawBuffer[i] == ENDBYTE && rawBuffer[i - 1] != ESCAPEBYTE) EOM = i;
    }

  //Serial.print("SOM: "); Serial.print(SOM); Serial.print(" EOM: "); Serial.println(EOM);

  //if we have a start and end of message, we can push it into packet.
  if (SOM != 0 && EOM != 0) 
    {
      for (int i = 0; i < MAXLEN; i++) packet[i] = 0;
      packetLen = EOM - SOM + 1;
      // copy the message from rawBuff into the packet
      if (debugRead) Serial.print("MSG Read: "); 
      for (int i = 0; i < packetLen; i++) 
        {
          packet[i] = rawBuffer[SOM + i];
          if(debugRead) { Serial.print(packet[i]); Serial.print(" "); }
        }
      if(debugRead) Serial.println();
      // reset variables
      SOM = EOM = 0; buffIndex = 1;
      for (int i = 0; i < MAXLEN; i++) rawBuffer[i] = 0; // clear our buffer

      byte tempCheckSum = _calcChecksum(packet, packetLen); // get the checksum
      // if the checksum fails
      if (tempCheckSum != packet[packetLen - 2]) return badMsgBytePtr;
      // if the address is incorrect and we are in reject mode, return
      if (rejectWrongAddress && (packet[1] & 0x7F) != _slaveAddress) return badMsgBytePtr;

      // byte unstuffing!
      int escapeChars = 0; // how many escape chars we have
      for (int i = 0; i < packetLen; i++)
        {
          if (packet[i] == ESCAPEBYTE)
            {
              escapeChars++;              
              for (int j = i; j < packetLen; j++)
                {
                  packet[j] = packet[j + 1];
                  if (j == packetLen - 1) packet[j] = 0;
                }
              packet[i] = packet[i] ^ 0x20; // XOR the post escape byte with 0x20 to get it back to what it was
            }
    
        }
      packetLen -= escapeChars; // we need to subtract the number of escape chars from the packet length

      // truncate the startbyte, checksum, and endbyte
      for (int i = 0; i < packetLen - 3; i++) 
        {
          packet[i] = packet[i + 1];
          if (i == packetLen - 4) packet[i + 1] = 0; // set the last byte to 0
        } 

      // for (int i = 0; i < packetLen; i++) 
      //   {
      //     Serial.print(packet[i], HEX); 
      //     Serial.print(" ");
      //   }
      // Serial.println("\tUnstuffed Packet: ");

      return packet;
    }
  
  return badMsgBytePtr;

}

void jbus::send(byte address, bool requestData, byte msgArr[], int arrLen)
{
  int packetLen = 0;
  int msgLen = arrLen;

  // if (debugMode) 
  //   {
  //     Serial.print("Packet Received: ");
  //     for (int i = 0; i < msgLen; i++) 
  //       {
  //         Serial.print(msgArr[i], HEX);
  //         Serial.print(" ");
  //       }
  //       Serial.println();
  //   }
  
   
  packetLen = msgLen + WRAPPER_COUNT;
  byte packet[packetLen];
  for (int i = 0; i < packetLen; i++) packet[i] = 0; // initialize all values to 0

  packet[0] = STARTBYTE; // start byte
  if (requestData) { packet[1] = (address |= 1<<7); } // if we are requesting data, set the MSB to 1,
  else { packet[1] = (address = address &= ~(1<<7)); } // if we are sending data, set the MSB to 0


    // instead of using msglen, we're using packetlen, because packen len increases as we add escape bytes
    int packetIndex = 2;     // we're starting on index two of packet len, start byte and address are already set
    for (int i = 2; i < msgLen + 2; i++ )
      {
        if (msgArr[i - 2] == 0xFD || msgArr[i - 2] == 0xFE || msgArr[i - 2] == 0xFF)
          {
            packet[packetIndex] = ESCAPEBYTE;
            packet[packetIndex + 1] = msgArr[i - 2] ^ 0x20;
            packetIndex += 2;
            packetLen++; // we added in an addtional byte, so we need to increment the packet length
          }
        else 
          {
            packet[packetIndex] = msgArr[i - 2];
            packetIndex++;
          }
      }
    
  packet[packetLen - 2] = _calcChecksum(packet, packetLen);
  packet[packetLen - 1] = ENDBYTE; // remember, zero indexed
  cereal.write(packet, packetLen);

  if (debugWrite)
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



/* void jbus::send(byte address, bool requestData, byte msgArr[])
{
  int msgLen = 0, packetLen = 0;
  while (msgArr[msgLen] != 0) { msgLen++; } // determine the length of the message.

  // if (debugMode) 
  //   {
  //     Serial.print("Packet Received: ");
  //     for (int i = 0; i < msgLen; i++) 
  //       {
  //         Serial.print(msgArr[i], HEX);
  //         Serial.print(" ");
  //       }
  //       Serial.println();
  //   }
  
   
  packetLen = msgLen + WRAPPER_COUNT;
  byte packet[packetLen];
  for (int i = 0; i < packetLen; i++) packet[i] = 0; // initialize all values to 0

  packet[0] = STARTBYTE; // start byte
  if (requestData) { packet[1] = (address |= 1<<7); } // if we are requesting data, set the MSB to 1,
  else { packet[1] = (address = address &= ~(1<<7)); } // if we are sending data, set the MSB to 0


    // instead of using msglen, we're using packetlen, because packen len increases as we add escape bytes
    int packetIndex = 2;     // we're starting on index two of packet len, start byte and address are already set
    for (int i = 2; i < msgLen + 2; i++ )
      {
        if (msgArr[i - 2] == 0xFD || msgArr[i - 2] == 0xFE || msgArr[i - 2] == 0xFF)
          {
            packet[packetIndex] = ESCAPEBYTE;
            packet[packetIndex + 1] = msgArr[i - 2] ^ 0x20;
            packetIndex += 2;
            packetLen++; // we added in an addtional byte, so we need to increment the packet length
          }
        else 
          {
            packet[packetIndex] = msgArr[i - 2];
            packetIndex++;
          }
      }
    
  packet[packetLen - 2] = _calcChecksum(packet, packetLen);
  packet[packetLen - 1] = ENDBYTE; // remember, zero indexed
  cereal.write(packet, packetLen);

  if (debugWrite)
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

byte jbus::_calcChecksum(byte *buffPtr, int packetLen)
{
  byte checksum = 0;
  // we are going to skip the first 0xFF, because it's always just that. 
  // The checksum is the Address ^(XOR) Message. 
  for (int i = 1; i < packetLen - 2; i++) { checksum ^= buffPtr[i]; }
  // I didnt add byte stuffing on the checksum. Therefore, if it's any of our stuffing bytes, we need to change it
  if (checksum == 0xFD || checksum == 0xFE || checksum == 0xFF) checksum ^= 0x20;
  if (debugChecksum)
    {
      Serial.print("Checksum: "); Serial.print(checksum);
      Serial.print("  PacketLen: "); Serial.print(packetLen);
      Serial.print("  Packet: ");
      for (int i = 0; i < packetLen; i++)
        {
          Serial.print(buffPtr[i]);
          Serial.print(" ");
        }
      Serial.print("  Checksum Status: "); Serial.println(checksum == buffPtr[packetLen - 2] ? "GOOD" : "BAD");
    }

  return checksum;
}