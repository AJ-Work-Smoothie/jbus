#pragma once
#include <Arduino.h>

#ifdef ARDUINO_AVR_MEGA2560
#define cereal (Serial1)
#define samd21Port1Begin(a)
#endif

#ifdef ARDUINO_SAMD_ZERO
#include <P1AM_Serial.h>
#define cereal (Port1)
#define samd21Port1Begin(a)  (PORT1_RS232_BEGIN(a))
#endif

class jbus
{
  public:
    jbus(); 
    void init(unsigned long buad);
    // Polls for a message of a specific length. Pass in the length of the message
    byte* poll(int msgLen); 
    // sends packet. If request is true, it will prefix the message with 0xFD
    bool send(byte msgArr[], int msgLen, bool requestData); // send packet
    // Sends a message and waits for a response.
    //byte* transceive(byte msgArr[], int msgLen, int pollTime); 

    byte signOnCMD[4] = {0xFC, 0xFC, 0xFB, 0xFB};
    // ROTARY TABLE TO ASA
    bool signOnASA();
    bool asa_connected = false;
    void runASAV2();
    

    // ASA TO ROTARY TABLE
    bool signOnToTable();
    

/*
  Packet to ASA
  | MSG START | VoltA | VoltB | VoltC |  VoltD |  Checksum  | MSG END | 
  |   0xFF    | 0-22  | 0-22  | 0-22  |  0-22  |    0xXX    |   0xFE  |



  Packet to Table
  | MSG START | TripA | TripB | TripC |  TripD |  Checksum  | MSG END | 
  |   0xFF    |  Y/N  |  Y/N  |  Y/N  |   Y/N  |    0xXX    |   0xFE  |

*/

  private:
    #define STANDARD_MSG_SIZE 4
    #define MSGSTART      0xFF
    #define MSGEND        0xFE
    #define REQUEST       0xFD
    #define MAXLEN        20
    int checksum = 0;
    byte badMsg = 0;
    byte *badMsgPtr;
    byte signOnCMDlen;    //defined in default constructor  

    // for ASA sign on
    bool flag = true;
    unsigned long pMillis = 0;
    int pollTime = 500;

};