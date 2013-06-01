#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <TypeDefinition.h>
#include <libEEPROM.h>
 
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
libEEPROM eeprom(2048);


void setup() { 
  
 
  lcd.begin(16, 2);
  lcd.clear();
}
 
void loop() {
  SBYT buf[32], i;
  
  eeprom.readBlock(0, buf);
  
  for(i = 0; i < 12; i++){
    lcd.print(buf[i], HEX);
  }

  delay(1000);
}
