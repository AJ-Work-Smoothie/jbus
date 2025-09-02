// Make sure you define which port you are using for jbus. Their options are located under their specific board type.

// If you don't define the JBUS_PORT, then this block of code will sort that out for you.
// ASA's default port is 3, so it does that. 
#ifndef JBUS_PORT
  #if defined(ARDUINO_AVR_MEGA2560)
    #error PLEASE DEFINE PORT 2(RS485) or PORT3 (RS232) for ASA BOARD
  #elif defined(ARDUINO_AVR_MICRO) || defined(ARDUINO_SAMD_ZERO) || defined (ARDUINO_SAMD_ZERO)
    #define JBUS_PORT 1
  #else
    #define JBUS_PORT 3   // generic fallback
  #endif
#endif

#if defined(ARDUINO_AVR_MICRO) || defined(ARDUINO_AVR_MEGA2560)
  #define CEREAL_PORT_EXPAND(jbus_port) Serial##jbus_port
  #define CEREAL_PORT_WRAP(jbus_port) CEREAL_PORT_EXPAND(jbus_port)
  #define cereal CEREAL_PORT_WRAP(JBUS_PORT)
  #undef JBUS_PROTOCOL // we're not using 232 or 485, so undefine this so that it doesn't
  // try to call Serial1.beginTransmission within jbus.cpp, which is an RS485 thing
#elif defined (ARDUINO_SAMD_ZERO)
  #define JBUS_PROTOCOL  232
  #include <P1AM_Serial.h>
  #if !defined(JBUS_PROTOCOL)
    #error JBUS_PROTOCOL not defined. Please set to Serial, 232, or 485
  #endif
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
// Flag if you didn't define any board type
#else
  #error NO BOARD WAS DEFINED
#endif