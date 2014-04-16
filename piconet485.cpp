#include <Arduino.h>
#include "piconet485.h"

piconet485::piconet485(byte id, byte rx, byte tx, byte mode) : rs485(SoftwareSerial(rx, tx))
{
	piconet485(id, rx, tx, mode, 19200);
}

piconet485::piconet485(byte id, byte rx, byte tx, byte mode, int baudrate) : rs485(SoftwareSerial(rx, tx))
{
	pinMode(mode,OUTPUT);
	digitalWrite(mode,LOW);
	rs485.begin(baudrate);
	rsMode=mode;
	msgSize=64;
	timeout=100;
	msgDat= (char *) malloc(msgSize+1);
	msgPos=0;
	msgSrc=0;
	msgDst=0;
	msgLen=0;
	msgChk=0;
	msgStatus=0;
	communicationLed=true;
	ledPin=13;
	ID=id;
}

void piconet485::add_checksum(byte data)
{
	if (msgChk > 127)
		msgChk = ((msgChk << 1) + data) + 1;
	else
		msgChk = ((msgChk << 1) + data);
}

byte piconet485::recv()
/*
 Return values:
    0 - No message received (or not completed)
    1 - Message received
    2 - Invallid checksum (message invalid)
 TODO:
   Use timeout
*/
{
	byte retValue=0;
	byte data;

	while (rs485.available())
	{
		if (communicationLed)
			digitalWrite(ledPin,HIGH);
		data=rs485.read();
    
		switch (msgStatus)
		{
			case 0: // Waiting message start code
				if (data == 0xAA)
				{
					msgStatus++;
					msgChk=0;
				}
				break;
			case 1: // Get Source and destination address
				msgSrc = data >> 4;
				msgDst = data & 0x0F;
				add_checksum(data);
				if (msgDst != ID)
					msgStatus = 0;
				else
					msgStatus++;
				break;
			case 2: // Get data length
				if (data < msgSize)
					msgLen = data;
				else
					msgLen = msgSize;
				msgPos=0;
				add_checksum(data);
				msgStatus++;
				break;
			case 3: // Getting payload data
				if (msgPos<msgLen)
				{
					msgDat[msgPos]=data;
					add_checksum(data);
					msgPos++;
				}
				else  // Get checksum and analyze message
				{
					msgDat[msgPos]='\0'; // For security. Not to be calculated in checksum
					msgStatus=0;
					if (data == msgChk)
						retValue=1;
					else
						retValue=4;
				}
				break;
		}
		if (communicationLed)
			digitalWrite(ledPin,LOW);
	}

	return retValue;
}

byte piconet485::send(byte dst, char *data, byte len)
{
	if (!rs485.available())
	{
		if (communicationLed)
			digitalWrite(ledPin,HIGH);

		// Start transmitting
		digitalWrite(rsMode,HIGH);

		// Start message
		rs485.write(msgStart);
		// Source and destination address
		msgDst=(ID<<4) + (dst & 0xF);
		rs485.write(msgDst);
		msgChk = 0;
		add_checksum(msgDst);
		if (len>msgSize)
			len=msgSize;
		// Payload
		rs485.write(len);
		add_checksum(len);
		for (msgPos=0;msgPos<len;msgPos++)
		{
			rs485.write(data[msgPos]);
			add_checksum(data[msgPos]);
		}
		// Checksum
		rs485.write(msgChk);

		// Stop transmitting
		digitalWrite(rsMode,LOW);

		if (communicationLed)
			digitalWrite(ledPin,LOW);

		return true;
	}

	return false;
}

