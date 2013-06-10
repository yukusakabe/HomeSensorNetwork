#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <TypeDefinition.h>
#include <libSTP.h>

void setup() {
  Serial.begin(19200);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
}


void loop() {
  Serial.print("ABC");
  delay(1000);
}
