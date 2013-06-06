//
//  libKeyManager.cpp
//  
//
//  Created by Yu Kusakabe on 2013/06/02.
//
//
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "HardwareSerial.h"
#include "SoftwareSerial.h"
#include "libKeyManager.h"

KeyManager::KeyManager(STP *stpa,
                       uint8_t key,
                       uint8_t lock,
                       uint8_t door)
{
    this->stpPort = stpa;
    this->keypin = key;
    this->lockpin = lock;
    this->doorpin = door;
    this->closeTime = 0;
    this->setTimeC = 0;
    this->setTimeI = 0;
}

void KeyManager::execComm()
{
    SBYT buf[28], buff[3];
    SBYT addr, len, ret;
    
    autoClose();
    
    ret = stpPort->recvPacket(&addr, buf, &len);
    
    if (ret == 0 && len >= 3 && buf[0] == 0xFA && buf[1] == 0xA1) {
        switch (buf[2]) {
            case 0x11:
                openKey(((DBYT)buf[3] << 4) + (DBYT)buf[4]);
                break;
                
            case 0x21:
                closeKey();
                break;
                
            case 0x31:
                buff[0] = 0xFA;
                buff[1] = 0xA2;
                buff[2] = readState();
                
                stpPort->sendPacket(addr, buff, 3);
                
                break;
        }
    }
}

SBYT KeyManager::readState()
{
    this->lockstate = digitalRead(lockpin);
    this->doorstate = digitalRead(doorpin);
    
    return (this->lockstate << 4) + this->doorstate;
}

void KeyManager::autoClose()
{
    readState();
    
    if (this->closeTime != 0) {
        if (checkTime(this->setTimeC, this->closeTime) && doorstate == LOW) {
            closeKey();
        }
    }
}

SBYT KeyManager::openKey(DBYT time)
{
    readState();
    
    if (lockstate == LOW) {
        if (checkTime(this->setTimeI, INTERVAL)) {
            digitalWrite(keypin, HIGH);
            delay(10);
            digitalWrite(keypin, LOW);
        
            this->closeTime = time * 1000;
            this->setTimeC = millis();
            this->setTimeI = millis();
        
            return 0;
        }
    }
    
    return 1;
}

SBYT KeyManager::closeKey()
{
    readState();
    
    if (lockstate == HIGH && doorstate == LOW) {
        if (checkTime(setTimeI, INTERVAL)) {
            digitalWrite(keypin, HIGH);
            delay(10);
            digitalWrite(keypin, LOW);
            
            this->closeTime = 0;
            this->setTimeI = millis();
            
            return 0;
        }
    }
    
    return 1;
}


SBYT KeyManager::checkTime(QBYT t0,
                           QBYT timeout)
{
    QBYT t = millis();
    
    if ((t - t0) >= timeout) {
        return 1;
    }
    
    return 0;
}
