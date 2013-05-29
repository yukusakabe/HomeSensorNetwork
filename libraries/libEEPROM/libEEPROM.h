//
//  libEEPROM.h
//  
//
//  Created by Yu Kusakabe on 2013/05/23.
//
//
#include <inttypes.h>

#ifndef ____libEEPROM__
#define ____libEEPROM__

#define EEPROM_SIZE 1024
#define BLOCK_SIZE  32

class cardKeyEEPROM
{
public:
    cardKeyEEPROM();
    
    void initcardKeyEEPROM();
    uint16_t searchID(uint8_t *cid,
                      uint8_t len);
    int saveID(uint8_t *cid,
               uint8_t len,
               uint8_t *scode);
    int loadID(uint8_t *cid,
               uint8_t len,
               uint8_t *scode);
    int deleteID(uint8_t *cid,
                 uint8_t len);

private:
    uint8_t readByte(uint16_t addr);
    void writeByte(uint16_t addr,
                   uint8_t data);
    void readBlock(uint16_t block,
                   uint8_t *data);
    void writeBlock(uint16_t blcok,
                    uint8_t *data);
    void incNoCard();
    uint16_t blockAddr(uint16_t block);
    
private:
    uint8_t rombuf[EEPROM_SIZE / 2];
    uint8_t processFlag;
};

#endif /* defined(____libEEPROM__) */
