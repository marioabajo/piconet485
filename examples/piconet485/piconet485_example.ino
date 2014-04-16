#include <piconet485.h>
#include <SoftwareSerial.h>

#define MODULE_ID 0

piconet485 net(MODULE_ID, 10, 11, 12, 19200);  // ID, RX, TX, , BaudRate

void setup()  
{
  Serial.begin(115200);
}

void loop()
{
  char data[64];
  static byte i=1;
  
  if (MODULE_ID==0) // Master
  {
    Serial.print("Sending: ");
    sprintf(data,"Hola modulo %d, desde modulo 0",i);
    Serial.println(data);
    net.send(i,data,strlen(data));
    i++;
    if (i>2)
      i=1;
  }
  
  switch (net.recv())
  {
    case 1:
      Serial.print("Message received: ");
      Serial.println(net.msgDat);
      break;
    case 2:
      Serial.print("Incorrect message received: ");
      Serial.println(net.msgDat);
      break;   
  }
}
