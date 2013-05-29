#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <libRCS620S.h>
#include <libEEPROM.h>
 
#define COMMAND_TIMEOUT 400
#define POLLING_INTERVAL 1000
#define LED_PIN 13
#define SWITCH_PIN 6
#define SETUP_PIN 10
 
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
RCS620S rcs620s(NULL);
//cardKeyEEPROM cardkey;

/*
void cardSetup() {
  int ret, i;
  uint8_t response[RCS620S_MAX_CARD_RESPONSE_LEN], responselen;
  uint8_t scode[4], buf[6];
 
  lcd.clear();
  lcd.print("Card Setup...");
  
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
    
    lcd.setCursor(0, 0);
    lcd.print("Succeed");
    
    delay(5000);
  }
  
  rcs620s.rfOff();
}*/

void setup() { 
  int ret;
 
  pinMode(LED_PIN, OUTPUT);   // for Polling Status
  digitalWrite(LED_PIN, LOW); 
  
  pinMode(SWITCH_PIN, OUTPUT);
  digitalWrite(SWITCH_PIN, LOW); 
  
  pinMode(SETUP_PIN, INPUT_PULLUP);
 
  Serial.begin(115200);      // for RC-S620/S
  lcd.begin(16, 2);           // for LCD
 
  // initialize RC-S620/S
  ret = rcs620s.initDevice();
  while (!ret) {}             // blocking
}
 
void loop() {
  int ret, i;
  uint8_t response[RCS620S_MAX_CARD_RESPONSE_LEN], responselen;
  uint8_t scode[4], buf[6];
 
  // Polling
  digitalWrite(LED_PIN, HIGH);
  rcs620s.timeout = COMMAND_TIMEOUT;
  lcd.clear();
  lcd.print("Card Setup...");
  
  ret = rcs620s.pollingTypeA();
  
  
  if (ret) {
    /*
    buf[0] = 0xa2;
    buf[1] = 0x06;
    buf[2] = random(0, 255);
    buf[3] = random(0, 255);
    buf[4] = random(0, 255);
    buf[5] = random(0, 255);
    */
    
    rcs620s.cardDataExchange((const uint8_t*)"\x30\x06\xFF\x00\xFF\x00", 6, response, &responselen);
    
    //cardkey.saveID(rcs620s.nfcid, rcs620s.nfcidlen, buf + 2);
    
    lcd.setCursor(0, 0);
    lcd.print("Succeed");
    delay(5000);
  }
 
  rcs620s.rfOff();
  digitalWrite(LED_PIN, LOW);
  delay(1000);
}
