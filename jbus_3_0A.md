# JBUS 3.0A!

**The A in 3.0A means that this is for Arduino!**

### So this isnn't confusing at all. I've never used the RS485 branch. I'm creating this new branch, which is based off the work I did on the XT7 rotary table. I re-wrote JBUS so that it can work with the RPI. This version will be a continuation of that. 

## What changed?
The original version of Jbus for the Arduino used to send over raw bytes, and it used byte stuffing to negotiate problems. The newer jbus is an ASCI-only based protocol. You send strings back and forth. Sending numbers is very easy. The entire packet structure is broken down later in this readme. 

  > TODO Add P1AM Support (whatever that means)

  > TODO MERGE RS485 Support Branch with this branch

  > ~~TODO Make a fucking readme~~

  > ~~TODO Merge all info from the .h into the readme~~


# Jbus 3.0 Packet Overview:

### | Start byte  | Message Len |   Name  | Payload | XOR Checksum  | End byte

- Jbus 3.0 is a ASCII based protocol. **All characters in sender names and messages must fall within printable ASCII range 0x20 (space) to 0x7A (z). The following characters are reserved for protocol use and must not appear in message content:** 
    - `{ (0x7B)`
    - `| (0x7C)` 
    - `} (0x7D)` 
    - `~ (0x7E)`

- The start char is `~`
- The sender includes a message length so receiver knows how many chars to expect
- Each sender shall have its own name. This can be used for device addressing. If you are the master device, instead of using your name while sending, you can put in the name of the slave device you want to converse with.
- Messages/Commands are strings. Each new command will be prefixed with the `|` character. Message structure is completely up to you. You can send words, characters, numbers, etc. Examples are:
    - `A`
    - `1234`
    - `CONF:RES 10M`
    - `CMD1|CMD2|CMD3`
- A checksum will appeneded to each message. This checksum is a simple XOR checksum
- All packets will be terminated with the `\n` newline character. 

### More details
- Between the start character `~` and the first `|` is the message length. The message length counts the amount of bytes from the first length (index 1) byte through the end of the payload marker `}`. The message len is represented by two ASCII hex characters.
- Immediately following the `|` is the name of the sender. The sender name is the substring between the `|` and the open `{` character.
- Payload appears between `{` and `}`. The `}` marks the end of the payload and the start of the checksum
- The checksum is a XOR of all bytes from the first length byte (index 1) through the `}` character. The checksum result is represented as two ASCII hex characters (e.g., 0A, 1B, FF) and immediately follows the `}` with no delimiter.
- Maximum packet size is 64 bytes
- No individual command should exceed 10 bytes. If you absolutely must send the worlds largest barcode, then split up the message into separate command
### Packet Structure
- [STARTi]  = `~`
- [LEN]    = 2-char hex indicating length from len to close
- [SOM]    = Start of message, marked `|`
- [SENDER] = variable string (no `{`, `|`, `}`, or `~`)
- [OPEN]   = `{`
- [MSG]    = command string with commands separated by '|'
- [CLOSE]  = `}`
- [CHECK]  = 2-digit hex XOR of (LEN thru `}`)
- [ENDi]    = `\n`

### Example Messages
- `~MSGLEN|SENDER{COMMANDS}CHECKSUM\n`
- `~19|ASA{MOVE:10|EAT:PIZZA}45\n`
- `~13|PILC{OPEN|CLOSE}50\n`

# Jbus_A: The Arduino version
Sady we no longer have access to cool things like strings and vectors, so we're gonna have to do some things the old school way.

## Limitations
Bye bye vectors, strings, and dynamic buffers. We now have to fix the length of certain things.
> Message strings are now limited to 4 commands x 10 (4 messages of 10 chars each).

## Getting Messages

Full code:
```cpp
#include <Arduino.h>
#include "jbus_3_0A.h"

jbus_3_0A devName;

void setup()
{
  devName.init(115200);
}

void loop()
{  
  char static commands[MAX_COMMANDS][MAX_CMD_LEN] = {0};       // 2D char array! 4 rows of 10 chars 
  int commandCount = devName.poll(commands);
  for (int i = 0; i < commandCount; i++) // only prints when commandCount is > than 0
    {
      Serial.print("["); Serial.print(i); Serial.print("] = "); Serial.print(commands[i]); Serial.print("\t");
      if (i + 1 == commandCount) Serial.println();
    }

}
```

1. Include the Jbus_A header
    - `#include "Jbus_3_0A.h"`
2. In `jbus_3_0A_config.h` are the serial port options. I currently have the correct options selected for the ATMega2560, Micro, Leonoardo, and maybe the P1AM
3. Create a Jbus object
    - `jbus_3_0A devName;`
4. Set the proper baud rate with `init()`. Must call in `void setup`
    - `devName.init(115200);`
5. Create 4 arrays that each contain an array of 10 chars. If you make them temp variables, they get reset each loop so you don't have to yourself.
    - `char commands[MAX_COMMANDS][MAX_CMD_LEN];`
6. Create an integer to hold the command count returned by poll()
    - `int commandCount = 0;` Make this local
6. Pass in your char array into poll. Poll will return the number of commands it recieved. 
    - `int commandCount = devName.poll(commands);`
7. If commandCount > 0, then we have messages to display!
    ```cpp
    for (int i = 0; i < commandCount; i++) // only prints when commandCount is > than 0
      {
        Serial.print("["); Serial.print(i); Serial.print("] = "); Serial.print(commands[i]); Serial.print("\t");
        if (i + 1 == commandCount) Serial.println();
      }
    ```

## Sending Messages
1. Follow steps 1 - 4 above
2. `devName.send("Hipa", "World", "Beans", "Bark", nullptr);`

> The null pointer is very important, do not forget to put it at the end 
> You are limited to 4 messages (including your name) and each much be shorter than 10 chars long

