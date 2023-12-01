#pragma once
#include <Arduino.h>

/*
  Welcome to JBUS! Written by AJ Robinson 12/1/2023

  JBUS works off of two main commands, poll() & send()

  Poll looks for a new message and returns a pointer. If a new message hasn't been recieved,
  the returned pointer will = 0. If a new message has been received, the pointer will equal
  the first value of the new received message. You can then access that message by using 
  pointer[i]. A quick shortcut to access pointer[0] is to just use *pointer.

  You must tell poll() what size message (NOT ENTIRE ARRAY LENGTH) it's looking for. If the 
  message received is a different size then what you are requesting, then checksum will fail.

  Send takes a byte array as an argument. Since arrays are passed as pointers, you also need to 
  pass the array size. send() will wrap your data into a nice little packet. The first byte of
  the packet will be 0xFF (if you set requestData argument to false) or it will equal 0xFD if
  set the requestData argument to true. After this start byte, your passed byte array will be
  appended. Next, a checksum is calculated and appended, and lastly, a stop byte is appended. The
  message structure looks like this:

  |   Start Byte  |   Passed Array  |   Checksum  |   Stop Byte |

  SignOnASA() sends a special sign on message - signOnCMD - with the start byte as request. It
  will repeat this every 200ms. In the meantime, it is using poll() to look for a message. If
  it gets a message with the start byte set to true, it checks the checksum. If the checksum
  matches the checksum for a sign on message, it assumes the correct message was sent and it
  returns true with a proper sign on.

  respondToTable() is what ASAV2 should always be running. It simply polls for messages, and 
  it if gets a RFQ message, it will handle it accordingly. Otherwise, it just does what the 
  incoming messages as for it to do. 





*/

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
    void send(byte msgArr[], int msgLen, bool requestData); // send packet

    bool signOnASA();
      bool asa_connected = false;
    
    void respondToTable();
    
    

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
    #define SIGNONRFQ     0xFC
    #define SIGNONRFQCHECKSUM 0xFD
    #define MAXLEN        20
    int checksum = 0;
    byte badMsg = 0;
    byte *badMsgPtr;
    byte signOnCMDlen;    //defined in default constructor
    byte signOnCMD[4] = {0xFC, 0xFC, 0xFC, 0xFC};  

    // for ASA sign on
    bool flag = true;
    unsigned long pMillis = 0;
    int pollTime = 500;

};