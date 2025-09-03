#include "jbus_3_0A.h"

jbus_3_0A::jbus_3_0A()
{
  
}
void jbus_3_0A::init(unsigned long baud)
{
  cereal.begin(baud);
}

int jbus_3_0A::poll(char msgs[][MAX_CMD_LEN])
{
  /**
   * We initialize buffIndex to 1. Why? Instead of using a flag to figure out if we've gotten a new start byte, we use SOM.
   * If SOM is != 0, that means we got a start message byte. We are reservering zero so that when SOM = 0, we know that 
   * we haven't gotten a start flag. We also have to start buffIndex at 1 so that we don't put our start byte in index 0, making 
   * SOM 0, meaning we'll miss that start byte. We must reset buffIndex to 1 instead of 0.
  */
  static char buffIndex = 1;
  static char rawBuffer[MAX_ARR_SIZE];
  static char packet[MAX_ARR_SIZE];

  while (cereal.available() > 0)
    {
      if (buffIndex >= 255) buffIndex = 1;
      rawBuffer[buffIndex] = cereal.read();
      //if (debug) Serial.print(rawBuffer[buffIndex]);  // print the byte that was just read
      if (rawBuffer[buffIndex] == '\n') break; // if we found an EOF, we should have a parsable message
      buffIndex++;
    }

  int SOM = 0, EOM = 0, packetLen = 0;
  // gets the start of the message
  for (int i = 0; i < buffIndex - 1; i++) if (rawBuffer[i] == JB_STARTBYTE_CHAR) SOM = i; // start from the beginning and find the SOM
  for (int i = SOM; i < MAX_ARR_SIZE; i++) if (rawBuffer[i] == JB_ENDBYTE_CHAR) EOM = i; // start from SOM and look for the EOM
  if (SOM == 0 || EOM == 0) return false; // if we didnt' find either a SOM or a EOM, return and let's look again
  //if (debug) { Serial.print("SOM: "); Serial.print(SOM); Serial.print(" EOM: "); Serial.println(EOM); }

  for (int i = 0; i < MAX_ARR_SIZE; i++) packet[i] = 0; // reset packet
  packetLen = EOM - SOM + 1; // easy stuff let's determine the length  
  for (int i = 0; i < packetLen; i++) packet[i] = rawBuffer[SOM + i]; // copy the message from rawBuff into the packet
  SOM = EOM = 0; buffIndex = 1; // reset variables before we forget to
  for (int i = 0; i < MAX_ARR_SIZE; i++) rawBuffer[i] = 0; // clear our buffer

  // Time to process our packet! The first thing is we need to figure out what the length is
  char lenHexChar[3]; // char array to store our two hex length chars 
  lenHexChar[0] = packet[JB_LEN1]; lenHexChar[1] = packet[JB_LEN2]; lenHexChar[2] = '\0';
  int msgLen = strtol(lenHexChar, nullptr, 16); // convert chars to their actual hex value
  //if (debug) Serial.print("Msg Len: "); Serial.print(msgLen); Serial.println();
  //if (debug) Serial.print("Char @ "); Serial.println((char)packet[msgLen]);
  if (packet[msgLen] != JB_CLOSE_CHAR) 
    { 
      Serial.println("Message Length is too wrong, too long, or can't find the }");
      return false;
    }
  // Time to check the checksum in the message vs our own calculations
  uint8_t calcChecksum = 0, receivedChecksum = 0;
  for (size_t i = JB_LEN1; i <= msgLen; i++) calcChecksum ^= packet[i]; // calculate our own checksum
  char checksumChar[3];
  checksumChar[0] = packet[msgLen + jb_CHECK1]; checksumChar[1] = packet[msgLen + jb_CHECK2]; checksumChar[3] = '\0'; // make sure to null-terminate your strings!
  receivedChecksum = strtol(checksumChar, nullptr, 16); // getting the checksum from the message
  // if (debug) 
  //   { 
  //     Serial.print("Calc-ed checksum: "); Serial.print(calcChecksum, HEX); 
  //     Serial.print("\tReceived Checksum: "); Serial.println(receivedChecksum, HEX);
  //   }
  if (calcChecksum != receivedChecksum)
    {
      Serial.println("There was a checksum mismatch :(");
      for (int i = 0; i < MAX_ARR_SIZE; i++) packet[i] = 0; // reset packet
      return false; // everything will reset itself
    }
  /**
   * It's passed all our checks (all 2 of them lol). Time to parse out the message
   * First thing is we need to find the sender name (or slave Address). That name is going to be between the 
   * first | and {.
   */
  
  // find the length of the sender name
  int nameLen = 0;
  for (int i = JB_SOM + 1; packet[i] != JB_OPEN_CHAR; i++) nameLen++;
  strncpy(msgs[0], &packet[JB_SOM + 1], nameLen);
  msgs[0][nameLen] = '\0'; // VERY important null term the string  

  /**
   * Here is a test message, the message index is listed on top.
   * 0 	1	  2	  3	  4	  5	  6	  7	  8	  9	  10	11	12	13	14	15	16 	17	18	19	20	21	22	23	24	25	26	27 28
   * ~ 	1 	9 	| 	H 	i 	p 	a 	{ 	W 	o 	r 	l 	d 	| 	B 	e 	a 	n 	s 	| 	B 	a 	r 	k 	} 	6 	1  \n
   * start = JB_SOM(i3) + 1 + nameLen(5) (puts us at JB_OPEN(i8) { ) + 1 to get start of message index (i9)
   * increment commandLen until we find a | or the }. commandLen now equals 5, which is just the length of the message NOT including the delimiter
   * Save the string from start + length (i9 - i13). Now start = start + commandlen + 1 which puts us at i15, ready for the next message. 
   * When we check if i < msgLen, we must include msgLen (i <= msgLen). We need the final } character to be included in our search so we can verify
   * that it closes off our packet.
   * CommandCount starts at 1 because we've already dumped in the sender name
   */

  int start = JB_SOM + 1 + nameLen + 1;  
  int commandLen = 0, commandCount = 1; // commandCount = 1 because we already got the sender name in to msgs[0]
  for (int i = start; i <= msgLen; i++)
    {
      if (packet[i] == JB_SEPARATOR_CHAR || packet[i] == JB_CLOSE_CHAR)
        {
          //Serial.print("M: "); Serial.write(&packet[start], commandLen); Serial.println(); 
          strncpy(msgs[commandCount], &packet[start], commandLen);
          msgs[commandCount][commandLen] = '\0'; // we always have to null-terminate our strings!!!
          //Serial.print("Message["); Serial.print(commandCount); Serial.print("] = "); Serial.print(msgs[commandCount]); Serial.println();
          start += commandLen + 1; // +1 to get it past the delimiter
          commandCount++;
          commandLen = 0;
        }
      else commandLen++; // if we didn't find | or }, then the current char is part of the len of the message
    }
  return commandCount;
}

void jbus_3_0A::send(const char* first, ...)
{
  static char callerStrings[MAX_ARR_SIZE] = {0}; // first start by filling with all 0s
  memset(callerStrings, 0, sizeof(callerStrings)); // since cs is static, we need to reset it everytime.
  strcat(callerStrings, "|");

  va_list args; // create an iterator called args that will be our list index
  va_start(args, first); // we want args to start with our first char

  int count = 0;
  const char *charListPtr = first; // asign a pointer to the first argument
  while (charListPtr != nullptr)
    {
      strcat(callerStrings, charListPtr); // add argument char to callerStrings
      if (charListPtr == first) strcat(callerStrings, "{"); // if it's the first time, put a { after the sender name. 
      else strcat(callerStrings, "|"); // after that, these are commands that need a |
      charListPtr = va_arg(args, const char*); // points charListPtr to the next char arg in the list
    }
  
  va_end(args); // very important to call this!! Ask Miranda if you don't know why
 
  int msgLen = strlen(callerStrings);
  callerStrings[msgLen - 1] = JB_CLOSE_CHAR;
  msgLen += 2; // gotta add the two len bytes

  // find len, do checksum, append, append \n, send!
  char msgLenChars[3];
  sprintf(msgLenChars, "%02X", msgLen);
  msgLenChars[2] = '\0'; // always nullterminate strings!

  char finalPacket[MAX_ARR_SIZE] = {0};
  strcat(finalPacket, "~");
  strcat(finalPacket, msgLenChars); // final packet now has ~len
  strcat(finalPacket, callerStrings); // ~len|name{Messages}

  uint8_t checksum = 0;
  for (int i = 1; i < strlen(finalPacket); i++) checksum ^= finalPacket[i]; // i = 1 to skip the first ~
  char checksumChars[3];
  sprintf(checksumChars, "%02X", checksum);
  checksumChars[2] = '\0';
  //Serial.print("Checksum: "); Serial.print(checksum); Serial.print(" & the chars: "); Serial.print(checksumChars);

  strcat(finalPacket, checksumChars);
  strcat(finalPacket, "\n");

  if (strlen(finalPacket) > MAX_ARR_SIZE)
    {
      Serial.println("Yo, you're trying to send too much data. Rethink your message");
      return;
    }

  // let's actually send the darn packet!
  if (debug) { Serial.print("Final Packet: "); Serial.write(finalPacket, strlen(finalPacket));}
  cereal.write(finalPacket);
  
}

//^ ~25|Alpha{Bravo|Charlie|Delta|Foxtrot}51