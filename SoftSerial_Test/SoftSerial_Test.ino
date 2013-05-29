#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <libRCS620S.h>
 
#define COMMAND_TIMEOUT 400
#define POLLING_INTERVAL 1000
#define LED_PIN 13

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
SoftwareSerial softSerial(2, 3);
RCS620S rcs620s(NULL);
 
void setup() { 
  int ret;
 
  pinMode(LED_PIN, OUTPUT);   // for Polling Status
  digitalWrite(LED_PIN, LOW); 
 
  Serial.begin(115200);      
  softSerial.begin(115200);   // for RC-S620/S
  lcd.begin(16, 2);           // for LCD
 
  // initialize RC-S620/S
  ret = rcs620s.initDevice();
  while (!ret) {}             // blocking
}
 
void loop() {
  int ret, i;
  uint8_t response[RCS620S_MAX_CARD_RESPONSE_LEN], responselen;
 
  // Polling
  digitalWrite(LED_PIN, HIGH);
  rcs620s.timeout = COMMAND_TIMEOUT;
  ret = rcs620s.pollingTypeA();
 
  lcd.clear();
  
  if(ret) {
    lcd.print("NFCID:");
    lcd.setCursor(0, 1);
    for(i = 0; i < rcs620s.nfcidlen; i++)
    {
      if(rcs620s.idm[i] / 0x10 == 0) lcd.print(0);
      lcd.print(rcs620s.nfcid[i], HEX);
    }
  } else {
    lcd.print("Polling...");
  }
  
  
  //ret = rcs620s.cardDataExchange((const uint8_t*)"\xa2\x06\x01\x02\x03\x04", 6, response, &responselen);
  /*ret = rcs620s.cardDataExchange((const uint8_t*)"\x30\x06", 2, response, &responselen);

  if(ret) {
    lcd.print("Response:");
    lcd.setCursor(0, 1);
    for(i = 0; i < 4; i++)
    {
      lcd.print(response[i], HEX);
    }
  } else {
    lcd.print("Miss...");
  }*/
 
  rcs620s.rfOff();
  digitalWrite(LED_PIN, LOW);
  delay(POLLING_INTERVAL);
}
