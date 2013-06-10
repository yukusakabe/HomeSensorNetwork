//
//  libEEPROM.cpp
//  
//
//  Created by Yu Kusakabe on 2013/05/23.
//
//

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "Print.h"
#include "EEPROM.h"
#include "cardkeyEEPROM.h"


/* ------------------------
 * public
 * ------------------------ */

cardKeyEEPROM::cardKeyEEPROM()
{
    uint16_t i = 0;
    
    while (i < EEPROM_SIZE / 2) {
        rombuf[i] = readByte(i);
        i++;
    }
}

void cardKeyEEPROM::initcardKeyEEPROM()
{
    uint16_t i = 0;
    
    while (i < EEPROM_SIZE) {
        writeByte(i, 0x00);
        rombuf[i] = 0x00;
        i++;
    }
}

uint16_t cardKeyEEPROM::searchID(uint8_t *cid,
                                 uint8_t len)
{
    uint16_t i = 4;
    uint8_t buf[32] = {};
    
    while (i < (EEPROM_SIZE / BLOCK_SIZE / 2)) {
        readBlock(i, buf);
        
        if (memcmp(buf, cid, len) == 0) {
            return i;
        }
        
        i++;
    }
    
    return 0;
}

int cardKeyEEPROM::saveID(uint8_t *cid,
                          uint8_t len,
                          uint8_t *scode)
{
    uint16_t i;
    uint8_t buf[32] = {};
    
    i = searchID(cid, len);
    
    if (i != 0) {
        readBlock(i, buf);
        memcpy(buf, cid, len);
        memcpy(buf + 12, scode, 4);
        
        writeBlock(i, buf);
    } else {
        if (rombuf[0] >= EEPROM_SIZE / BLOCK_SIZE / 2 - 4) {
            return 1;
        } else {
            memcpy(buf, cid, len);
            memcpy(buf + 12, scode, 4);
            
            writeBlock(rombuf[0] + 4, buf);
            incNoCard();
        }
    }
    
    return 0;
}

int cardKeyEEPROM::loadID(uint8_t *cid,
                          uint8_t len,
                          uint8_t *scode)
{
    uint16_t i;
    uint8_t buf[32] = {};
    
    i = searchID(cid, len);
    
    if (i == 0) {
        return 1;
    } else {
        readBlock(i, buf);
        memcpy(scode, buf + 12, 4);
    }
    
    return 0;
}


/* ------------------------
 * private
 * ------------------------ */

uint8_t cardKeyEEPROM::readByte(uint16_t addr)
{
    return EEPROM.read(addr);
}

void cardKeyEEPROM::writeByte(uint16_t addr,
                              uint8_t data)
{
    EEPROM.write(addr, data);
}

void cardKeyEEPROM::readBlock(uint16_t block,
                              uint8_t *data)
{
    memcpy(data, rombuf + blockAddr(block), BLOCK_SIZE);
}

void cardKeyEEPROM::writeBlock(uint16_t block,
                               uint8_t *data)
{
    uint8_t i = 0;
    
    while (i < BLOCK_SIZE) {
        if (rombuf[i + blockAddr(block)] != data[i]) {
            writeByte(blockAddr(block) + i , data[i]);
        }
        i++;
    }
    
    memcpy(rombuf + blockAddr(block), data, BLOCK_SIZE);
}

void cardKeyEEPROM::incNoCard()
{
    writeByte(0, rombuf[0] + 1);
    rombuf[0]++;
}

uint16_t cardKeyEEPROM::blockAddr(uint16_t block)
{
    return block * EEPROM_SIZE / BLOCK_SIZE;
}