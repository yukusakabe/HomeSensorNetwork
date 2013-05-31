#include <SoftwareSerial.h>
#include <libSTP.h>

STP stp(9600, 0);

void setup() {                
  Serial.begin(9600);
}

void loop() {
  uint8_t buf[16];
  
  buf[0] = 0xAA;
  buf[1] = 0x55;
  buf[3] = 0x01;
  
  stp.sendPacket(0x01, (const uint8_t *)stp.calcCRC16(buf, 2), 2);


  delay(1000);
}
