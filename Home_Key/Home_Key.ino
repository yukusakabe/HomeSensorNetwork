#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <libRCS620S.h>
#include <libEEPROM.h>
 
#define COMMAND_TIMEOUT 400
#define POLLING_INTERVAL 1000
#define LED_PIN 13
#define SWITCH_PIN 8
#define SETUP_PIN 9
#define DELETE_PIN 10
#define KEY_STATE_PIN 6

#define LED_CLOSE_PIN 2
#define LED_OPEN_PIN 3
#define OPEN_BUTTON_PIN 4
#define CLOSE_BUTTON_PIN 5

//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
RCS620S rcs620s(NULL);
cardKeyEEPROM cardkey;


void cardSetup() {
  int ret, i = 0;
  uint8_t response[RCS620S_MAX_CARD_RESPONSE_LEN], responselen;
  uint8_t scode[4], buf[6];
 
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
      
      rcs620s.cardDataExchange(buf, 6, response, &responselen);
      
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
  
  pinMode(SWITCH_PIN, OUTPUT);
  digitalWrite(SWITCH_PIN, LOW); 
  
  pinMode(SETUP_PIN, INPUT_PULLUP);
  pinMode(DELETE_PIN, INPUT_PULLUP);
  pinMode(KEY_STATE_PIN, INPUT_PULLUP);
  pinMode(LED_OPEN_PIN, OUTPUT);
  pinMode(LED_CLOSE_PIN, OUTPUT);
  pinMode(OPEN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(CLOSE_BUTTON_PIN, INPUT_PULLUP);
  
  digitalWrite(LED_OPEN_PIN, LOW); 
  digitalWrite(LED_CLOSE_PIN, LOW); 
 
  Serial.begin(115200);      // for RC-S620/S
//  lcd.begin(16, 2);           // for LCD
 
  // initialize RC-S620/S
  ret = rcs620s.initDevice();
  while (!ret) {}             // blocking
}
 
void loop() {
  int ret, i;
  uint8_t response[RCS620S_MAX_CARD_RESPONSE_LEN], responselen;
  uint8_t scode[4], buf[6];
  
  uint8_t s = 0;
 
  // Polling
  digitalWrite(LED_PIN, HIGH);
  rcs620s.timeout = COMMAND_TIMEOUT;
//  lcd.clear();
  
  if (digitalRead(SETUP_PIN) == LOW) {
    cardSetup();
//    lcd.clear();
  }
  
  if (digitalRead(DELETE_PIN) == LOW) {
//    lcd.print("Resetting...");
    cardkey.initcardKeyEEPROM();    
    delay(1000);
//    lcd.clear();
  }
  
  ret = rcs620s.pollingTypeA();
 
  if (ret) {
    rcs620s.cardDataExchange((const uint8_t*)"\x30\x06", 2, response, &responselen);
    
    ret = cardkey.loadID(rcs620s.nfcid, rcs620s.nfcidlen, scode);
      
    if (ret == 0) {
        if (memcmp(scode, response, 4) == 0) {
          buf[0] = 0xa2;
          buf[1] = 0x06;
          buf[2] = random(0, 255);
          buf[3] = random(0, 255);
          buf[4] = random(0, 255);
          buf[5] = random(0, 255);
          
          rcs620s.cardDataExchange(buf, 6, response, &responselen);
          
          cardkey.saveID(rcs620s.nfcid, rcs620s.nfcidlen, buf + 2);
          
          if (digitalRead(KEY_STATE_PIN) == LOW) {
            digitalWrite(LED_CLOSE_PIN, HIGH);
//            lcd.print("OPEN");
            
            while (s < 20) {
              if (digitalRead(OPEN_BUTTON_PIN) == LOW) {
                digitalWrite(SWITCH_PIN, HIGH);
                delay(1000);
                digitalWrite(SWITCH_PIN, LOW);
                delay(2000);
                
                s = 20;
              }
              delay(250);
              s++;
            }
            
            digitalWrite(LED_CLOSE_PIN, LOW);
          } else {
            digitalWrite(LED_OPEN_PIN, HIGH);
//            lcd.print("CLOSE");
            
            while (s < 20) {
              if (digitalRead(CLOSE_BUTTON_PIN) == LOW) {
                digitalWrite(SWITCH_PIN, HIGH);
                delay(1000);
                digitalWrite(SWITCH_PIN, LOW);
                delay(2000);
                
                s = 20;
              }
              
              delay(250);
              s++;
            }
            
            digitalWrite(LED_OPEN_PIN, LOW);
          }
          
          
          if (digitalRead(KEY_STATE_PIN) == LOW) {
            digitalWrite(LED_CLOSE_PIN, HIGH);
            delay(2000);
            digitalWrite(LED_CLOSE_PIN, LOW);
          } else {
            digitalWrite(LED_OPEN_PIN, HIGH);
            delay(2000);
            digitalWrite(LED_OPEN_PIN, LOW);
          }
        } else {
//          lcd.print("NG");
          delay(3000);
        }
    } else {
//      lcd.print("NG");
      delay(3000);
    }
  } else {
//    lcd.print("Polling...");
  }
 
  rcs620s.rfOff();
  digitalWrite(LED_PIN, LOW);
  delay(POLLING_INTERVAL);
}
