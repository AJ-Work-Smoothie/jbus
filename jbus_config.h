#ifdef ARDUINO_AVR_MICRO
  #define SERIAL_PORT 1
  #define BAUD_RATE   9600
  
  #define cereal (Serial1)
  #if !defined (PORTNUM) || !defined (PROTOCOL) || !defined (BAUDRATE)
    #error Please define PORTNUM, PROTOCOL, and BAUDRATE
  #endif
#endif

#ifdef ARDUINO_AVR_MEGA2560
  #define PORT_NUM  3
  #define BAUD_RATE 115200

  #define CEREAL_PORT_EXPAND(PORT_NUM, BAUD_RATE) Serial##

  #define cereal (Serial3)
#endif

#ifdef ARDUINO_SAMD_ZERO
  #include <P1AM_Serial.h>
  
  #define PORT_NUM   1
  #define PROTOCOL  485
  #define BAUD_RATE  250000

  #if !defined (PORT_NUM) || !defined (PROTOCOL) || !defined (BAUD_RATE)
    #error Please define PORT_NUM, PROTOCOL, and BAUDRATE
  #endif

  #if (PROTOCOL == 485)
    #include <ArduinoRS485.h>
  #endif
  // this first one is used to create the proper macro
  #define CREATE_PORT_EXPAND(number, type, baud) PORT##number##_RS##type##_BEGIN(baud)
  // but we need to expand PROTOCOL & PORTNUM so they "convert" to their respective values. So we use the second macro
  #define CREATE_PORT(number, type, baud) CREATE_PORT_EXPAND(number, type, baud) // expands to something like PORT1_RS485_BEGIN(9600)
  
  #define CEREAL_PORT_EXPAND(number) Port##number // we need to create Port1,2 etc
  #define CEREAL_PORT(number) CEREAL_PORT_EXPAND(number)
  // this causes some problems. We can't use a use CEREAL_PORT(arugment) every time we want to do cereal.write
  // so let's make one more macro that's just cereal that a replacement for CEREAL_PORT(PORTNUM)
  #define cereal CEREAL_PORT(PORT_NUM) // we need to us

#endif  