#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <TypeDefinition.h>
#include <libSTP.h>
#include <libKeyManager.h>

#define LED_PIN   13
#define DOOR_PIN  3
#define LOCK_PIN  4
#define KEY_PIN   5

#define BIT_RATE  19200
#define MY_ADDR   0x01

STP stp(BIT_RATE, MY_ADDR);
KeyManager km(&stp, KEY_PIN, LOCK_PIN, DOOR_PIN);

boolean i = false;

void setup() { 
  wdt_enable(WDTO_1S);
  
  pinMode(2, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(DOOR_PIN, INPUT_PULLUP);
  pinMode(LOCK_PIN, INPUT_PULLUP);
  pinMode(KEY_PIN, OUTPUT);
  
  digitalWrite(2, LOW);
  Serial.begin(BIT_RATE);
}
 
void loop() {
  km.execComm();
  digitalWrite(LED_PIN, i);
  i = !i;
  wdt_reset();
  delay(50);
}
