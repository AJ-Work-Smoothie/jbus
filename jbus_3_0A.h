#pragma once
#include <Arduino.h>
#include <stdarg.h>
#include "jbus_3_0A_config.h"

#define MAX_COMMANDS    4
#define MAX_CMD_LEN     10
#define MAX_ARR_SIZE    64


class jbus_3_0A
{
  public:
    bool debug = true;
    
    jbus_3_0A();
    void init(unsigned long baud = 115200); // must call in void setup, defaults to 115200
    void rejectOtherSenders(const char * senderName); // use this if we want to reject other senders.
    /**
     * @brief poll() grabs messages from the serial buffer. Will shove them into an array
     * of c-style strings.
     * @param msgs is a 32x32 2D char array, so 32 arrays of 32 chars
     * @return the number of commands received (this count includes the sender name)
     */
    int poll(char msgs[][MAX_CMD_LEN]);
    /**
     * @brief packages messages and shoots them off into the ethos!
     * @param first - This function allows you input a variable amount of arguments. Start with the sender name
     *                followed by the commands. **FINAL ARGUMENT MUST BE nullptr!!!!**
     */
    void send(const char* first, ...); // we aren't sure how many messages are coming in, so we'll keep it variable with . . .
    void clear(); // called when there is a checksum mistmatch

  private:
    // if senderName_ is NOT equal to nullPtr, then we must care about who we are getting messages from
    const char *senderName_ = nullptr; // will store the name of the sender we only want to messages from
    const char JB_STARTBYTE_CHAR = '~';
    const char JB_SEPARATOR_CHAR = '|';
    const char JB_OPEN_CHAR = '{';
    const char JB_CLOSE_CHAR = '}';
    const char JB_ENDBYTE_CHAR = '\n';

    //! jb OPEN IS NOT A KNOWN LOCATION
        // all of the jbust indexes
    // The first list are CONSTANTS in accordance with the JBUS protocol. 
    enum  { JB_STARTi, JB_LEN1, JB_LEN2, JB_SOM, JB_SENDER };
    // The rest depend on the length of the message, so use them + MSGLEN
    enum { jb_CLOSE, jb_CHECK1, jb_CHECK2, jb_ENDi };

    
};