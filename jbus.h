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
    byte* poll(int msgLen);  // looking for a single msg
    bool send(byte arr[], int msgSize); // send packet
    byte* transceive(byte arr[], int arrLen); // sends msgs, returns response
  

/*
  Packet to ASA
  | MSG START | VoltA | VoltB | VoltC |  VoltD |  Checksum  | MSG END | 
  |   0xFF    | 0-22  | 0-22  | 0-22  |  0-22  |    0xXX    |   0xFE  |



  Packet to Table
  | MSG START | TripA | TripB | TripC |  TripD |  Checksum  | MSG END | 
  |   0xFF    |  Y/N  |  Y/N  |  Y/N  |   Y/N  |    0xXX    |   0xFE  |
*/

  private:
    #define MSGSTART      0xFF
    #define MSGEND        0xFE
    #define MAXLEN        20
    int checksum = 0;

};