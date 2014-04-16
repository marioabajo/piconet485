/*
  piconet485.h - Library for sending and receiving packets in a rs485 bus
  Created by Mario Abajo Duran, April 14, 2014.
  Released into the public domain.
*/
#ifndef piconet485_h
#define piconet485_h

#include <Arduino.h>
#include <SoftwareSerial.h>

#define msgStart 0xAA

/* Message structure
*  0xAA (src) (dst) (len) (data) (chk)
*  8bit  4bit  4bit  8bit  var    8bit
*  minimum message size = 4 bytes
*  maximum message size = 68 bytes (64 bytes payload)
*/

class piconet485
{
	public:
		char *msgDat;
		byte timeout;  // miliseconds
		boolean communicationLed;
		boolean ledPin;
		piconet485(byte id, byte rx, byte tx, byte mode, int baudrate);
		piconet485(byte id, byte rx, byte tx, byte mode);
		byte recv();
		byte send(byte dst, char *data, byte len);
	private:
		byte ID;
		byte msgSize;
		byte msgPos;
		byte msgSrc;
		byte msgDst;
		byte msgLen;
		byte msgChk;
		byte msgStatus;
		byte rsMode;	// the pin that change between receive and transmit
		SoftwareSerial rs485;
		void add_checksum(byte data);
};

#endif
