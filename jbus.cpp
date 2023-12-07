#include "jbus.h"

jbus::jbus()
{
  badMsgBytePtr = &badMsgByte; // set badMsg to point to badmsg, a byte equaling 0;
  badMsgFloatPtr = &badMsgFloat;
  signOnCMDlen = sizeof(signOnCMD);
}

void jbus::init(unsigned long buad)
{
  cereal.begin(buad);
  samd21Port1Begin(buad);
}

bool jbus::signOnASA()
{
  if (flag)
    {
      Serial.println("Attempting to connect to ASA V2");
      send(signOnCMD, signOnCMDlen, true);
      pMillis = millis();
      flag = false;
    }
  if (millis() - pMillis > msgSendInterval)
    {
      flag = true;
    }

  byte *p;   // if returning, needs to be static
  p = poll(signOnCMDlen);
  // the if the checksum didn't add up, then the sign on msg wasn't sent or it was erroneous. 
  // if *p == our beginning of mesasage, then the msg was processed and the checksum passesd,
  // therfore we got a good response back. No need to check the message byte by byte.
  if (*p == REQUEST) 
    {
      Serial.println("Connection to ASA V2 Established");
      asa_connected = true; // save state to variable for updating the HMI
      return true; // we have a sign on!
    }
  else 
    {
      asa_connected = false;
      return false;
    }
}

void jbus::setVoltage(float va, float vb, float vc, float vd)
{
  byte daca = (va * 10);
  byte dacb = (vb * 10);
  byte dacc = (vc * 10);
  byte dacd = (vd * 10);

  byte arr[] = { daca, dacb, dacc, dacd };
  send(arr, sizeof(arr), false);
}


byte* jbus::getDutStatus()
{
  byte *p = poll(STANDARD_MSG_SIZE);  
  if (*p == PACKETSTART)
    {
      newResponse = true;
      for (int i = 0; i < STANDARD_MSG_SIZE; i++)
        dutStatus[i] = (p[MSGSTART + i] - 250); // converting from our PASS/FAIL message to boolean logic

      return dutStatus;

    }    
  newResponse = false;
  return badMsgBytePtr;

}

byte* jbus::poll(int msgLen)
{
  // we pass in the msgLen, so the array len is msgLen+3 (PACKETSTART, checksum, PACKETEND)
  static int arrLen = msgLen + 3; // arrlen needs to stay for when Serial is NOT available
  static byte packet[MAXLEN]; // if not declared static, the function return does not work. 
  
  for (int i = 0; i < arrLen; i++) packet[i] = 0; // erase all contects of packet

  if (cereal.available() >= arrLen) 
    { 
      //for (int i = 0; i < arrLen; i++) packet[i] = 0; // erase all contects of packet
      // rcv message   
      for (int i = 0; i < arrLen; i++)
        {
          packet[i] = cereal.read();
          //Serial.print(packet[i]);
          //Serial.print(" ");
        }
      //Serial.println();

      // calculate checksum
      checksum = 0; // reset before each calc
      for (int i = 0; i < arrLen - 2; i++)
        {
          checksum ^= packet[i];
        }
      //Serial.print("   Checksum Calc: "); Serial.print(checksum);
      //Serial.println(); 

      // if checksum good
      if (checksum == packet[arrLen - 2])
        {
          // Serial.print("MSG GOOD! ");
          // for (int i = 0; i < arrLen; i++)
          //   {
          //     Serial.print(packet[i]);
          //     Serial.print(" ");
          //   }
          // Serial.println();

          return packet;
        }
      
  // if checksum is bad
      else 
        {
          Serial.println("CHECKSUM IS BAD");
          for (int i = 0; i < arrLen; i++) packet[i] = 0; // erase all contects of packet
          
          return badMsgBytePtr; // FAILED
        }
    } // if cereal.avilable

  else return badMsgBytePtr; 

}

void jbus::send(byte msgArr[], int msgLen, bool requestData)
{
  // arr is the message we want to send. We create an array that is +3 bigger for 
  // PACKETSTART, checksum, and PACKETEND
  int packetLen = msgLen + 3;
  byte packet[packetLen];

  for (int i = 0; i < packetLen; i++)
    packet[i] = 0; // initialize all values to 0

  if (requestData) packet[0] = REQUEST;
  else packet[0] = PACKETSTART;

  for (int i = 1; i < msgLen + 1; i++ ) 
    packet[i] = msgArr[i - 1]; // copies arr into outgoing packet
  for (int i = 0; i < packetLen - 2; i++) 
    packet[packetLen - 2] ^= packet[i]; // simple XOR checksum  
  packet[packetLen - 1] = PACKETEND; // remember, zero indexed
  cereal.write(packet, packetLen);

  // for (int i = 0; i < packetLen; i++)
  //   {
  //     Serial.print(packet[i]);
  //     Serial.print(" ");
  //   }
  // Serial.println();
}

void jbus::reset()
{
  for(int i = 0; i < 128; i++)
    {
      // clear out serial buffer
      cereal.read();
    }
}
/* This code is perfect! Just didn't need it
byte* jbus::pollCont()
{
  if (cereal.available() >= BUFFLEN) 
    { 
      clearPacket(); // erase all contects of packet
      // rcv message   
      for (int i = 0; i < BUFFLEN; i++)
        {
          buffer[i] = cereal.read();
        }
      
      int pos = 0;
      for ( ; buffer[pos] != PACKETSTART; pos++) // increment through buffer until we've found PACKETSTART
        ;
      for (int i = pos; i < (pos + arrLen); i++)
        {
          packet[i - pos] = buffer[i]; // copy buffer into packet, zero indexed
          // Serial.print(packet[i - pos]);
          // Serial.print(" ");
        }
      // Serial.println();

      // calculate checksum
      checksum = 0; // reset before each calc
      for (int i = 0; i < arrLen - 2; i++)
        {
          checksum ^= packet[i];
        }
      //Serial.print("   Checksum Calc: "); Serial.print(checksum);
      //Serial.println(); 

      // if checksum good
      if (checksum == packet[arrLen - 2])
        {
          // Serial.print("MSG GOOD! ");
          // for (int i = 0; i < arrLen; i++)
          //   {
          //     Serial.print(packet[i]);
          //     Serial.print(" ");
          //   }
          // Serial.println();

          clearBuffer();
          return packet;
        }
      
  // if checksum is bad
      else 
        {
          Serial.println("CHECKSUM IS BAD");
          clearBuffer();
          
          return 0; // FAILED
        }
    } // if cereal.avilable

  else return 0; 

} */