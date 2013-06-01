/*
 * RC-S620/S sample library for Arduino
 *
 * Copyright 2010 Sony Corporation
 */

#include <inttypes.h>

#ifndef libRCS620S_H_
#define libRCS620S_H_

/* --------------------------------
 * Constant
 * -------------------------------- */

#define RCS620S_MAX_CARD_RESPONSE_LEN    254
#define RCS620S_MAX_RW_RESPONSE_LEN      265

/* --------------------------------
 * Class Declaration
 * -------------------------------- */

class SoftwareSerial;
class RCS620S
{
public:
    //RCS620S();
    RCS620S(SoftwareSerial *softSerial = NULL);	

    int initDevice(void);
    int pollingFeliCa(uint16_t systemCode = 0xffff);
    int pollingTypeA();
    int cardCommand(const uint8_t* command,
                    uint8_t commandLen,
                    uint8_t response[RCS620S_MAX_CARD_RESPONSE_LEN],
                    uint8_t* responseLen);
    int cardDataExchange(const uint8_t* command,
                         uint8_t commandLen,
                         uint8_t response[RCS620S_MAX_CARD_RESPONSE_LEN]);
    int rfOff(void);
    int push(const uint8_t* data,
             uint8_t dataLen);

private:
    int rwCommand(const uint8_t* command,
                  uint16_t commandLen,
                  uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN],
                  uint16_t* responseLen);
    void cancel(void);
    uint8_t calcDCS(const uint8_t* data,
                    uint16_t len);
    void writeSerial(const uint8_t* data,
                     uint16_t len);
    int readSerial(uint8_t* data,
                   uint16_t len);
    void flushSerial(void);
    int checkTimeout(unsigned long t0);

public:
    unsigned long timeout;
    uint8_t idm[8];
    uint8_t pmm[8];
    uint8_t nfcid[10];
    uint8_t nfcidlen;
    
private:
    SoftwareSerial *serialPort;
};

#endif /* !RCS620S_H_ */
