//
//  libEEPROM.h
//  
//
//  Created by Yu Kusakabe on 2013/05/23.
//
//
#include <inttypes.h>
#include "TypeDefinition.h"

#ifndef ____libEEPROM__
#define ____libEEPROM__

#define BLOCK_SIZE  32

class libEEPROM
{
public:
    libEEPROM(DBYT size);
    
    void readBlock(DBYT block,
                   SBYT *data);
    void writeBlock(DBYT blcok,
                    SBYT *data);
    void clearEEPROM();
    void clearBlock(DBYT block);

private:
    SBYT readByte(DBYT addr);
    void writeByte(DBYT addr,
                   SBYT data);
    DBYT resBlockAddr(DBYT block);

private:
    DBYT romsize;
};

#endif /* defined(____libEEPROM__) */
