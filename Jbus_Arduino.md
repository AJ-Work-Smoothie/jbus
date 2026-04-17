# JBUS 3.0 Ardiuno Version

> TODO: In `Jbus_config.h` there is code to enable the RS485 on the P1AM. This has not been implemented. It's not that much work, but needs to be done and tested. ¯\_(ツ)_/¯ 

> Latestes updates:
We deleted the branch JBUS 3.0A. We changed all the files names to Jbus_Arduino. No longer any version names in the files. 

### What changed from Jbus 2 to Jbus 3?
The original version of Jbus for the Arduino used to send over raw bytes, and it used byte stuffing. The newer jbus is an ASCII-only based protocol. You send strings back and forth. This was done because it was much easier to communicate with C++ using strings than raw bytes. It makes our lives a little bit harder on the Arduino, but not too bad at all!


## Limitations
For the Arduino version of this library, we have to say bye bye vectors, strings, and dynamic buffers. We now have to manually use c-style strings.
> Message strings are now limited to 3 payload commands x 32 chars each, with a 128-byte maximum packet size.

## Getting Messages

Full code:
```cpp
#include <Arduino.h>
#include "Jbus_Arduino.h"

Jbus devName;

void setup()
{
  devName.init(115200);
}

void loop()
{  
  char static commands[MAX_COMMANDS][MAX_CMD_LEN] = {0};       // 2D char array! 4 rows of 10 chars. NO reason to be static
  int commandCount = devName.poll(commands);
  for (int i = 0; i < commandCount; i++) // only prints when commandCount is > than 0
    {
      Serial.print("["); Serial.print(i); Serial.print("] = "); Serial.print(commands[i]); Serial.print("\t");
      if (i + 1 == commandCount) Serial.println();
    }

}
```

1. Include the Jbus_A header
    - `#include "Jbus_Arduino.h"`
2. In `Jbus_Arduino_config.h` are the serial port options. I currently have the correct options selected for the ATMega2560, Micro, Leonoardo, and maybe the P1AM
3. Create a Jbus object
    - `Jbus devName;`
4. Set the proper baud rate with `init()`. Must call in `void setup`
    - `devName.init(115200);`
5. Create 4 arrays that each contain an array of 10 chars. If you make them temp variables, they get reset each loop so you don't have to yourself, so feel free to make them static.
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
- The first argument will be the sender/slave name. If you used `setMyName`, then the library will automatically send your name as the first argument, and you can put in just the commands. 

> The null pointer is very important, do not forget to put it at the end. If you entire board crashes, it's most likely because you did not put a nullptr at the end of a send!
> You are limited to 3 payload messages per packet, and each must be shorter than 32 chars long.

# Jbus 3.0 Packet Overview:
### | Start byte  | Message Len |   Name  | Payload | XOR Checksum  | End byte

- Jbus 3.0 is a ASCII based protocol. **All characters in sender names and messages must fall within printable ASCII range 0x20 (space) to 0x7A (z). The following characters are reserved for protocol use and must not appear in message content:** 
    - `{ (0x7B)`
    - `| (0x7C)` 
    - `} (0x7D)` 
    - `~ (0x7E)`

- The start char is `~`
- The sender includes a message length so receiver knows how many chars to expect
- Every message is sent with a name in the name field. This can be configured to be your master's name, or you can put in a slave's name if you are speaking to multiple slaves. I left it flexible to suit different needs. 
- You can reject messages from unkown names. If the name field in an incoming messages doesn't match a specified name, it will return nothing and you won't receive the message. If it does match, it will remove the name and return commands only. You can enable name rejection with `rejectOtherNames("Safe Name Here");`. This way if you have a master device, the master device can send messages with different slave names for different slaves. If the sent slave name doesn't match a particular slave's name, that particular slave can ignore the message. 
- A checksum will appeneded to each message. This checksum is a simple XOR checksum
- All packets will be terminated with the `\n` newline character. 
- Messages/Commands are strings. Each new command will be prefixed with the `|` character. Message structure is completely up to you. You can send words, characters, numbers, etc. Examples are:
    - `A`
    - `1234`
    - `MOVE:120`
    - `CMD1|CMD2|CMD3`
- As stated above, you have the freedom to choose however your string format is, however I have chosen a default method of sending commands. You will find a function that will help you deal with them. For example take `MOVE:10`. 
    - `MOVE:10` in its entirety is called a *command*
    - `MOVE` is the *action*
    - `:` is the *action/parameter separator*
    - `10` is the *parameter*

  Accompanying this format is a function called `parseCommand`. Pass in an ActionParameter struct, and `parseCommand` will split a singluar command into an action and a parameter and write those to the two strings inside of the struct. Parse command always erases the ActionParameter struct each call.

### More details
- Make sure to check the different buffer sizes. Names can be no longer than 10 chars.
- Between the start character `~` and the first `|` is the message length. The message length counts the amount of bytes from the first length (index 1) byte through the end of the payload marker `}`. The message len is represented by two ASCII hex characters.
- Immediately following the `|` is the name of the sender. The sender name is the substring between the `|` and the open `{` character.
- Payload appears between `{` and `}`. The `}` marks the end of the payload and the start of the checksum
- The checksum is a XOR of all bytes from the first length byte (index 1) through the `}` character. The checksum result is represented as two ASCII hex characters (e.g., 0A, 1B, FF) and immediately follows the `}` with no delimiter.
- Maximum packet size is 128 bytes
- No individual command should exceed 32 chars. If you absolutely must send the worlds largest barcode, then split up the message into separate commands
- Maximum number of strings returned by `poll()` is four: sender name plus up to three payload commands
- Maximum number of payload commands accepted by `send()` is three
- When calling `send(. . .)` you must *ALWAYS* put `nullptr` as your last argument. If you don't, the program will crash!

### Packet Structure
- [STARTi] = `~`
- [LEN]    = 2-char hex indicating length from len to close
- [SOM]    = Start of message, marked `|`
- [SENDER] = variable string (no `{`, `|`, `}`, or `~`)
- [OPEN]   = `{`
- [MSG]    = command string with commands separated by '|'
- [CLOSE]  = `}`
- [CHECK]  = 2-digit hex XOR of (LEN thru `}`)
- [ENDi]   = `\n`

### Example Messages
- FORMAT: `~MSGLEN|SENDER{COMMANDS}CHECKSUM\n`
- `~19|ASA{MOVE:10|EAT:PIZZA}45\n`
- `~13|PILC{OPEN|CLOSE}50\n`
- `~25|Alpha{Bravo|Charlie|Delta:11|Foxtrot}51\n`
