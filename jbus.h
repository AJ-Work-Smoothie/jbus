#pragma once
#include <Arduino.h>

/*
 * VERSION: JBUS 2.0
 * DATE: 3/20/2024

  Welcome to JBUS! Written by AJ Robinson starting 12/1/2023.
  JBUS got revamped starting in 3/20/2024. The new version is called JBUS 2.0. 2.0 includes byte stuffing
  and address rejection. You can now use all bytes, 0x00 -> 0xFF without any issues.

  Get started with JBUS by creating a jbus object:
  jbus bus(). bus() is overloaded. If you are a master, do not pass any arguments. If you are a slave, pass
  in your address. By default, in slave configuration, poll() will reject any messages that are not addressed to the slave.
  If you wish to see all the messages in slave config, set it to true with obj.rejectWrongAddress = true. 
  
  * poll() actively gets messages. It's non-blocking if there are no messages in Serial.available(). If there are messages,
  it will process them first. To use poll(), create a pointer, and set it equal to poll(), i.e. byte *p = poll().
  poll() will return a null pointer if there are no new messages to deal with. You can check if you have a message with if(*p). 
  Poll returns with a pointer to the received messsage. *p is the address. We can check that address with 
  the following: if ((*p & 0x7F) == slave_address) 
  
  Next we can check to see if we need to resond: if (*p >> 7) Serial.println("Required to respond!");
  Aternatively, we could OR the address with 0x80 to see if we are required to respond like so: if (*p == (slave_address | 0x80))

  * send() simply requires an address, respond or not command, and a null terminated array to send. You must null terminate the array
  being passed into send(). send() determines the length of the array using terminating null pointer.
  
  - Jbus can have up to 127 different addresses. 0 can never be used as an address.
  The JBUS packet structure is as follows: 

  Start Byte |  Address (LSB) | Message | Checksum | End Byte
  0xFF       |      0-127     | 0-255   | 0-255    | 0xFE

  - send() requires a null terminated array.
  - 0xFF & 0xFE are used as the start and end bytes.
  - JBUS using byte stuffing. The escape character is 0xFD. If the message contains 0xFD, 0xFE, or 0xFF, it is suffixed by 0xFD
  - The checksum calculates the address + the message, i.e checksum = Address ^ Message.
  - cereal is used to represent a corresponding serial port. Please change below if need be. 

*/
#ifdef ARDUINO_AVR_MICRO
#define cereal (Serial1)
#endif

#ifdef ARDUINO_AVR_MEGA2560
#define cereal (Serial3)
//#define samd21Port1Begin(a)
#endif

#ifdef ARDUINO_SAMD_ZERO
#include <P1AM_Serial.h>
#define cereal (Port1)
#define samd21Port1Begin(a)  (PORT1_RS232_BEGIN(a))
#endif

class jbus
{
  public:
    bool debugRaw = false;
    bool debugRead = false;
    bool debugWrite = false;
    bool debugChecksum = false;
    
    jbus(); // master config
    jbus(byte slaveAddress); // slave config
      bool rejectWrongAddress = false;
    void init(unsigned long buad); // must call in setup.
    /**
     * @brief This function polls the bus for messages. If there are no messages, it returns a null pointer. Check
     * to see if there is a message with if(*p). If there is a message, *p is the address. You can check that address with
     * the following: if ((*p & 0x7F) == slave_address). Next we can check to see if we need to resond: if ((*p >> 7) == 1)
     * @return pointer to the received message.
    */
    byte* poll(); // returns a pointer to the message. 
    /**
     * @brief This function sends a message to the bus. If requestData is true, the slave is required to respond. 
     * If requestData is false, the slave is not required to respond. The message must be null terminated
     * @param address The address of the slave
     * @param requestData If true, the slave is required to respond
     * @param msgArr The message to send must be null terminated
    */
    // void send(byte address, byte msgArr[]); // this one is causing problems because I don't null terminate arrays
    void send(byte address, byte msgArr[], int arrLen);
    void clearBuffer();
    

  private:

    byte _calcChecksum(byte *buffPtr, int packetLen); // returns the checksum

    #define STARTBYTE       0xFF
    #define ENDBYTE         0xFE
    #define ESCAPEBYTE      0xFD
    #define MAXLEN          32
    #define WRAPPER_COUNT   4
    
    byte _slaveAddress = 0;
    byte badMsgByte = 0;
    byte *badMsgBytePtr;
};