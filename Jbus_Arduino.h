#pragma once
#include <Arduino.h>
#include <stdarg.h>
#include "Jbus_Arduino_config.h"

#define MAX_COMMANDS    7
#define MAX_NAME_LEN    10
#define MAX_CMD_LEN     32
#define MAX_ARR_SIZE    256
#define DB_RAW          1
#define DB_PACKET       2

struct ActionParameter {
  char action[MAX_CMD_LEN];
  char parameter[MAX_CMD_LEN];
};


class Jbus
{
  public:
    
    Jbus();
    void init(unsigned long baud = 115200); // must call in void setup, defaults to 115200
    void debugMode(int level); // 0 = none, 1 = raw messages, 2 = layers
    void setMyName(const char * myName);
    void rejectOtherSenders(const char * senderName); // use this if we want to reject other senders.
    
    /**
     * @brief poll() grabs messages from the serial buffer. Will shove them into an array of c-style strings.
     * @param msgs is a 7x32 2D char array, so 7 arrays of 32 chars each
     * @return the number of commands received (this count includes the sender name). Return 0 if no commands
     */
    int poll(char msgs[][MAX_CMD_LEN]);

    /**
     * @brief packages messages and shoots them off into the ethos!
     * @param first - This function allows you input a variable amount of arguments ( args < MAX_COMMANDS). Start 
     * with the sender name followed by the commands. **FINAL ARGUMENT MUST BE nullptr!!!!**
     */
    void send(const char* first, ...); // up to you what to send, so we'll keep it variable with (...)
    
    /**
     * @brief splits a *single* command string into an action and a parameter
     * @param command is single command string, like msgs[0] or "SPIN:10"
     * @param ap is an ActionParameter struct that is passed in by reference. ParseCMD will modify the two
     * strings within the struct to contatin an action and an parameter
     */
    void parseCommand(const char *command, ActionParameter &ap); // this function separates commands into actions and parameters
    
  private:
    int debug_ = 0;
    char myName_[MAX_NAME_LEN] = {0};      // we need to make sure we allocate space for this
    char senderName_[MAX_NAME_LEN] = {0};  // will store the name of the sender we only want to messages from
    const char JB_STARTBYTE_CHAR = '~';
    const char JB_SEPARATOR_CHAR = '|';
    const char JB_PARAMETER_IND = ':';
    const char JB_OPEN_CHAR = '{';
    const char JB_CLOSE_CHAR = '}';
    const char JB_ENDBYTE_CHAR = '\n';

    // capital JB indicates known positions, lowercase jb indicates calcuated positions
    // The first list are CONSTANTS in accordance with the JBUS protocol. 
    enum  { JB_STARTi, JB_LEN1, JB_LEN2, JB_SOM, JB_SENDER };
    // The rest depend on the length of the message, so use them + MSGLEN
    enum { jb_CLOSE, jb_CHECK1, jb_CHECK2, jb_ENDi };

    
};