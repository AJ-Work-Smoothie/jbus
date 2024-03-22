#pragma once
#include <Arduino.h>

/*
 * VERSION: ART_SW_2.0
 * DATE: 2021-09-30

  Welcome to JBUS! Written by AJ Robinson starting 12/1/2023. 
  
  - Jbus can have up to 127 different addresses. 0 can never be used as an address.
  The JBUS packet structure is as follows: 

  Start Byte | R/W (MSB)  &  Address (LSB) | Message | Checksum | End Byte
  0xFF       |  0/1              0-127     | 0-255   | 0-255    | 0xFE

  The checksum calculates the address + the message, i.e checksum = Address ^ Message. It's also constrained to be less than 0xFE.

  - 0xFF & 0xFE are reserved for the start and end bytes.
  - JBUS using byte stuffing. The escape character is 0xFD. If the message contains 0xFD, 0xFE, or 0xFF, it is suffixed by 0xFD
  - The R/W bit is the most significant bit of the address. If it is 1, the slave is required to respond. If it is 0, the slave is not required to respond.
  - Poll returns with a pointer to the received messsage. *p is the address. We can check that address with the following:

  if ((*p & 0x7F) == slave_address)

  Next we can check to see if we need to resond:
  if ((*p >> 7) == 1) Serial.println("Required to respond!");
  Aternatively, we could OR the address with 0x80 to see if we are required to respond like so:
  if (*p == (slave_address | 0x80))

*/

#ifdef ARDUINO_AVR_MEGA2560
#define cereal (Serial3)
#define samd21Port1Begin(a)
#endif

#ifdef ARDUINO_SAMD_ZERO
#include <P1AM_Serial.h>
#define cereal (Port1)
#define samd21Port1Begin(a)  (PORT1_RS232_BEGIN(a))
#endif

#define STANDARD_MSG_SIZE 4

class jbus
{
  public:
    bool debugMode = false;
    bool debugChecksum = false;
    
    jbus(); // master config
    jbus(byte slaveAddress); // slave config
    void init(unsigned long buad);
    // Polls for a message of a specific length. Pass in the length of the message. Poll is NOT blocking
    byte* poll();
    byte* processMessage();
    // sends packet. If request is true, it will prefix the message with msgRFQ instead of standard msg
    void send(byte address, bool requestData, byte msgArr[]); // send packet
    byte calcChecksum(byte *buffPtr, int packetLen); // returns the checksum

  private:

    #define STARTBYTE       0xFF
    #define ENDBYTE         0xFE
    #define ESCAPEBYTE      0xFD
    #define MAXLEN          64
    #define WRAPPER_COUNT   4
    
    byte _slaveAddress = 0;
    int checksum = 0;
    byte badMsgByte = 0;
    byte *badMsgBytePtr;

};


/*
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
} */