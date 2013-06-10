/*
 * RC-S620/S sample library for Arduino
 *
 * Copyright 2010 Sony Corporation
 */

#ifndef ____libRCS620S__
#define ____libRCS620S__

#include <inttypes.h>
#include <SoftwareSerial.h>
#include "TypeDefinition.h"

/* --------------------------------
 * Constant
 * -------------------------------- */

#define RCS620S_MAX_CARD_RESPONSE_LEN    254
#define RCS620S_MAX_RW_RESPONSE_LEN      265

/* --------------------------------
 * Class Declaration
 * -------------------------------- */

class RCS620S
{
public:
    //RCS620S();
    RCS620S(SoftwareSerial *softSerial = NULL);	

    SBYT initDevice(void);
    SBYT pollingFeliCa(DBYT systemCode = 0xffff);
    SBYT pollingTypeA();
    SBYT cardCommand(const SBYT *command,
                     SBYT commandLen,
                     SBYT response[RCS620S_MAX_CARD_RESPONSE_LEN],
                     SBYT *responseLen);
    SBYT cardDataExchange(const SBYT *command,
                          SBYT commandLen,
                          SBYT response[RCS620S_MAX_CARD_RESPONSE_LEN]);
    SBYT rfOff(void);
    SBYT push(const SBYT *data,
              SBYT dataLen);

private:
    SBYT rwCommand(const SBYT *command,
                   DBYT commandLen,
                   SBYT response[RCS620S_MAX_RW_RESPONSE_LEN],
                   DBYT *responseLen);
    void cancel(void);
    SBYT calcDCS(const SBYT *data,
                 DBYT len);
    void writeSerial(const SBYT *data,
                     DBYT len);
    SBYT readSerial(SBYT *data,
                    DBYT len);
    void flushSerial(void);
    QBYT msec();
    QBYT usec();
    void delaym(QBYT time);
    void delayu(QBYT time);
    SBYT checkTimeout(QBYT t0);

public:
    QBYT timeout;
    SBYT idm[8];
    SBYT pmm[8];
    SBYT nfcid[10];
    SBYT nfcidlen;
    
private:
    SoftwareSerial *serialPort;
};

#endif /* !RCS620S_H_ */
