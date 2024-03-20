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

  The checksum calculates everything up to it, excluding itself, i.e checksum = Start Byte ^ Address ^ Message

  
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
    
    jbus(); // master config
    jbus(byte serverAddress); // slave config
    void init(unsigned long buad);
    // Polls for a message of a specific length. Pass in the length of the message. Poll is NOT blocking
    byte* poll(int msgLen);
    bool verifyChecksum(byte *arrPtr, int arrLen); // returns true if the checksums match
    byte calcChecksum(byte *arrPtr, int arrLen); // returns true if the checksums match
    // sends packet. If request is true, it will prefix the message with msgRFQ instead of standard msg
    void send(byte address, bool requestData, byte msgArr[], int msgLen); // send packet
    //void send(byte msgArr[], int msgLen, byte customStartByte); // send packet
    void debugSet(bool mode);
    void reset();   

  private:
    
    #define STARTBYTE       0xFF
    #define ENDBYTE         0xFE
    #define MAXLEN          128
    #define WRAPPER_COUNT   4
    
    byte _slaveAddress = 0;
    int checksum = 0;
    byte badMsgByte = 0;
    byte *badMsgBytePtr;

    
    

};