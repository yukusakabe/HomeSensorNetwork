#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <TypeDefinition.h>
#include <libSTP.h>

#define FRONT_OPEN_BUTTON  3
#define FRONT_CLOSE_BUTTON 4
#define FRONT_OPEN_LED     5
#define FRONT_CLOSE_LED    6

#define BLINK_INTERVAL    500
#define BUTTON_INTERVAL   1000
#define BIT_RATE          19200
#define MY_ADDR           0x02

STP stp(BIT_RATE, MY_ADDR);

SBYT ledblink = 1, blinkflag;
SBYT chatto, chattc;
SBYT lockstate, doorstate;
QBYT btime, btime2, btime3;

void setup() { 
  wdt_enable(WDTO_1S);
  
  pinMode(2, OUTPUT);
  pinMode(FRONT_OPEN_LED, OUTPUT);
  pinMode(FRONT_CLOSE_LED, OUTPUT);
  pinMode(FRONT_OPEN_BUTTON, INPUT_PULLUP);
  pinMode(FRONT_CLOSE_BUTTON, INPUT_PULLUP);

  digitalWrite(2, LOW);
  btime = millis();
  btime2 = millis();
  btime3 = millis();
  
  Serial.begin(BIT_RATE);
}
 
void loop() {
  SBYT buf[28];
  SBYT addr, len, ret;
  
  if (millis() - btime3 >= 1000) {
    stp.sendPacket(0x01, (SBYT *)"\xFA\xA1\x31", 3);
    btime3 = millis();
  }
  ret = stp.recvPacket(&addr, buf, &len);
  
  if (ret == 0 && addr == 0x01 && len >= 3 && buf[0] == 0xFA && buf[1] == 0xA2) {
    switch (buf[2]) {
      case 0x00:
        digitalWrite(FRONT_CLOSE_LED, HIGH);
        digitalWrite(FRONT_OPEN_LED, LOW);
        blinkflag = 0;
        break;
      case 0x01:
        digitalWrite(FRONT_OPEN_LED, LOW);
        blinkflag = 1;
        break;
      case 0x10:
        digitalWrite(FRONT_CLOSE_LED, LOW);
        digitalWrite(FRONT_OPEN_LED, HIGH);
        blinkflag = 0;
        break;
      case 0x11:
        digitalWrite(FRONT_CLOSE_LED, LOW);
        blinkflag = 2;
        break;
    }
  }
  
  if ((millis() - btime2) >= BUTTON_INTERVAL) {
    if (digitalRead(FRONT_OPEN_BUTTON) == LOW) {
      delay(10);
      if (digitalRead(FRONT_OPEN_BUTTON) == LOW) {
        stp.sendPacket(0x01, (SBYT *)"\xFA\xA1\x11\x00\x3C", 5);
        btime2 = millis();
      }
    }
    
    if (digitalRead(FRONT_CLOSE_BUTTON) == LOW) {
      delay(10);
      if (digitalRead(FRONT_CLOSE_BUTTON) == LOW) {
        stp.sendPacket(0x01, (SBYT *)"\xFA\xA1\x21", 3);
        btime2 = millis();
      }
    }
  }

  
  if ((millis() - btime) >= BLINK_INTERVAL) {
    switch (blinkflag) {
      case 1:
        digitalWrite(FRONT_CLOSE_LED, ledblink);
        break;
        
      case 2:
        digitalWrite(FRONT_OPEN_LED, ledblink);
        break;
    }
    
    ledblink = !ledblink;
    btime = millis();
  }
  
  wdt_reset();
  delay(50);
}
