#include <SoftwareSerial.h>
#include <MsTimer2.h>
#include <TypeDefinition.h>
#include <libSTP.h>

#define LED_PIN   13
#define DOOR_PIN  2
#define LOCK_PIN  3
#define KEY_PIN   4

#define BIT_RATE  19200
#define MY_ADDR   0x01

STP stp(BIT_RATE, MY_ADDR);

SBYT state;

void closedoor() {
  if (state == 0x01) {
    
  } 
}

void opendoor(QBYT time) {
  if (state == 0x00) {
    
  }
}

void mainLoop() {
  SBYT buf[28];//, buff[28];
  SBYT addr, len, ret;
  QBYT t;
  static boolean output = HIGH;
  
  state = 0;
  state += digitalRead(DOOR_PIN) << 4;
  state += digitalRead(LOCK_PIN);
  
  ret = stp.recvPacket(&addr, buf, &len);
  //stp.sendPacket(0x01, (SBYT *)"ABCDEFGHIJKLMNOPGRSTUVWXYZ", 26);
  
  if (!ret) {
    if (buf[0] == 0xC0) {
      switch (buf[1]) {
        case 0xA0:
          closedoor();
          break;
          
        case 0xAF:
          opendoor(buf[2]);
          break;
          
        case 0xB0:
        /*
          buff[0] = 0xC0;
          buff[1] = 0xBF;
          buff[2] = state;
          stp.sendPacket(addr, buff, 3);
          */
          break;
      }
    }
  }
  
  
  digitalWrite(LED_PIN, output);
  output = !output;
}

void setup() { 
  pinMode(LED_PIN, OUTPUT);
  pinMode(DOOR_PIN, INPUT_PULLUP);
  pinMode(LOCK_PIN, INPUT_PULLUP);
  pinMode(KEY_PIN, OUTPUT);
  
  Serial.begin(BIT_RATE);
  
  MsTimer2::set(50, mainLoop);
  MsTimer2::start();
}
 
void loop() {
}
