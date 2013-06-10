#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <TypeDefinition.h>
#include <libRCS620S.h>
#include <cardkeyEEPROM.h>
#include <libSTP.h>
 
#define COMMAND_TIMEOUT 400
#define POLLING_INTERVAL 1000
#define LED_PIN 13
#define SETUP_PIN 11
#define DELETE_PIN 12

#define RED_PIN  6
#define GRN_PIN  7
#define SWT_PIN  8

RCS620S rcs620s(NULL);
cardKeyEEPROM cardkey;
SoftwareSerial SoftSerial(2, 3);
STP stp(&SoftSerial, 19200, 3);


void cardSetup() {
  int ret, i = 0;
  SBYT response[RCS620S_MAX_CARD_RESPONSE_LEN], responselen;
  SBYT scode[4], buf[6];
 
//  lcd.clear();
//  lcd.print("Card Setup...");
  
  while (i < 10) {
  
    ret = rcs620s.pollingTypeA();
  
    if (ret) {
      buf[0] = 0xa2;
      buf[1] = 0x06;
      buf[2] = random(0, 255);
      buf[3] = random(0, 255);
      buf[4] = random(0, 255);
      buf[5] = random(0, 255);
      
      rcs620s.cardDataExchange(buf, 6, response);
      
      cardkey.saveID(rcs620s.nfcid, rcs620s.nfcidlen, buf + 2);
      
//      lcd.setCursor(0, 0);
//      lcd.clear();
//      lcd.print("Succeed");
      
      delay(2000);
      i = 10;
    }
    rcs620s.rfOff();
    i++;
    delay(1000);
  }
}

void setup() { 
  int ret;
 
  pinMode(LED_PIN, OUTPUT);   // for Polling Status
  digitalWrite(LED_PIN, LOW); 
  
  pinMode(SETUP_PIN, INPUT_PULLUP);
  pinMode(DELETE_PIN, INPUT_PULLUP);
  //pinMode(4, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  pinMode(SWT_PIN, INPUT_PULLUP);
  
  //digitalWrite(4, LOW);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GRN_PIN, LOW);
 
  Serial.begin(115200);      // for RC-S620/S
  SoftSerial.begin(19200);
  SoftSerial.print("TESTTEST");
 
  // initialize RC-S620/S
  ret = rcs620s.initDevice();
  while (!ret) {}             // blocking
}
 
void loop() {
  int ret, i;
  SBYT response[RCS620S_MAX_CARD_RESPONSE_LEN], responselen;
  SBYT scode[4], buf[6];
  SBYT buff[28], addr, len;
  SBYT s = 0;
 
  // Polling
  digitalWrite(LED_PIN, HIGH);
  rcs620s.timeout = COMMAND_TIMEOUT;
  
  if (digitalRead(SETUP_PIN) == LOW) {
    cardSetup();
  }
  
  if (digitalRead(DELETE_PIN) == LOW) {
    cardkey.initcardKeyEEPROM();    
    delay(1000);
  }
  
  ret = rcs620s.pollingTypeA();
  

  /*if (SoftSerial.available()) {
      SoftSerial.write(stp.recvPacket(&addr, buf, &len));
  }*/
 
  if (ret) {
    rcs620s.cardDataExchange((const SBYT*)"\x30\x06", 2, response);
    
    ret = cardkey.loadID(rcs620s.nfcid, rcs620s.nfcidlen, scode);
      
    if (ret == 0) {
        if (memcmp(scode, response, 4) == 0) {
          buf[0] = 0xa2;
          buf[1] = 0x06;
          buf[2] = random(0, 255);
          buf[3] = random(0, 255);
          buf[4] = random(0, 255);
          buf[5] = random(0, 255);
          
          rcs620s.cardDataExchange(buf, 6, response);
          
          cardkey.saveID(rcs620s.nfcid, rcs620s.nfcidlen, buf + 2);
          
          stp.sendPacket(0x01, (SBYT *)"\xFA\xA1\x31", 3);
          stp.flushSerial();
          
          i = 1000;
          while (i >= 0) {
            ret = stp.recvPacket(&addr, buf, &len);
           
            if (ret == 0) {
              stp.sendPacket(0x01, (SBYT *)"\xFA\xA1\x31", 3);
              break;
            }
            i--;
            delay(10);
          }
          
          if (ret == 0) {
            switch (buf[2]) {
              case 0x00:
                digitalWrite(GRN_PIN, HIGH);
                i = 100;
                while (i >= 0) {
                  if (digitalRead(SWT_PIN) == LOW) {
                    stp.sendPacket(0x01, (SBYT *)"\xFA\xA1\x11", 3);
                    break;
                  } 
                  delay(50);
                  i--;
                }
                digitalWrite(GRN_PIN, LOW);
                
                break;
              
              case 0x10:
                digitalWrite(RED_PIN, HIGH);
                stp.sendPacket(0x01, (SBYT *)"\xFA\xA1\x21", 3);
                delay(3000);
                digitalWrite(RED_PIN, LOW);
                break;
            }
          } 
        }
    }
  }
 
  rcs620s.rfOff();
  digitalWrite(LED_PIN, LOW);
  delay(POLLING_INTERVAL);
}
