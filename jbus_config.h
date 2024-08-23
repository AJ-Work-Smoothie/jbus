#define JBUS_PORT   1
#define JBUS_PROTOCOL  485

#ifdef ARDUINO_AVR_MICRO
  #define SERIAL_PORT 1
  #define BAUD_RATE   9600
  
  #define cereal (Serial1)
  #if !defined (PORTNUM) || !defined (JBUS_PROTOCOL) || !defined (BAUDRATE)
    #error Please define PORTNUM, JBUS_PROTOCOL, and BAUDRATE
  #endif
#endif

#ifdef ARDUINO_AVR_MEGA2560
  #define JBUS_PORT  3
  #define BAUD_RATE 115200

  #define CEREAL_PORT_EXPAND(JBUS_PORT, BAUD_RATE) Serial##

  #define cereal (Serial3)
#endif

#ifdef ARDUINO_SAMD_ZERO
  #include <P1AM_Serial.h>


  #if (JBUS_PROTOCOL == 485)
    #include <ArduinoRS485.h>
  #endif
  // this first one is used to create the proper macro
  #define CREATE_PORT_EXPAND(number, type, baud) PORT##number##_RS##type##_BEGIN(baud)
  // but we need to expand JBUS_PROTOCOL & PORTNUM so they "convert" to their respective values. So we use the second macro
  #define CREATE_PORT(number, type, baud) CREATE_PORT_EXPAND(number, type, baud) // expands to something like PORT1_RS485_BEGIN(9600)
  
  #define CEREAL_PORT_EXPAND(number) Port##number // we need to create Port1,2 etc
  #define CEREAL_PORT(number) CEREAL_PORT_EXPAND(number)
  // this causes some problems. We can't use a use CEREAL_PORT(arugment) every time we want to do cereal.write
  // so let's make one more macro that's just cereal that a replacement for CEREAL_PORT(PORTNUM)
  #define cereal CEREAL_PORT(JBUS_PORT) // we need to us

#endif  