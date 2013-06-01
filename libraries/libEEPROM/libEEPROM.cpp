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
#include "libEEPROM.h"


/* ------------------------
 * public
 * ------------------------ */

libEEPROM::libEEPROM(DBYT size) {
    this->romsize = size;
}

void libEEPROM::readBlock(DBYT block,
                          SBYT *data)
{
    SBYT i = 0;
    
    while (i < BLOCK_SIZE) {
        data[i] = readByte(resBlockAddr(block) + i);
        i++;
    }
}

void libEEPROM::writeBlock(DBYT block,
                           SBYT *data)
{
    SBYT i = 0;
    
    while (i < BLOCK_SIZE) {
        if (readByte(resBlockAddr(block) + i) != data[i]) {
            writeByte(resBlockAddr(block) + i , data[i]);
        }
        i++;
    }
}

void libEEPROM::clearEEPROM()
{
    SBYT i = 0;
    
    while (i < this->romsize) {
        writeByte(i, 0x00);
        i++;
    }
}

void libEEPROM::clearBlock(DBYT block)
{
    SBYT i = 0;
    
    while (i < BLOCK_SIZE) {
        writeByte(resBlockAddr(block) + i, 0x00);
    }
}

/* ------------------------
 * private
 * ------------------------ */

SBYT libEEPROM::readByte(DBYT addr)
{
    return EEPROM.read(addr);
}

void libEEPROM::writeByte(DBYT addr,
                          SBYT data)
{
    EEPROM.write(addr, data);
}

DBYT libEEPROM::resBlockAddr(DBYT block)
{
    return block * BLOCK_SIZE;
}