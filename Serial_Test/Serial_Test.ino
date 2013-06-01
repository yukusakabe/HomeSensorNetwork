#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <TypeDefinition.h>
#include <libSTP.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
STP stp(9600, 0);

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  Serial.begin(9600);
}


void loop() {
  SBYT buf[32], test[32];
  SINT ret, i, len, addr;
  buf[0] = 0xF1;
  buf[1] = 0x82;
  
  test[0] = 0x02;
  test[1] = 0xF1;
  test[2] = 0x82;
  
  //stp.sendPacket(0x01, 0xFF, test, 3);
  
//  ret = stp.recvPacket(&addr, buf, &len);
  
  if (ret == 0) {
    lcd.clear();
    
    for (i = 0; i < 3; i++) {
      lcd.print(buf[i], HEX);
    }
  }
  
  


  delay(1000);
}
