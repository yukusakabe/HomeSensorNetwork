/*
 * RC-S620/S sample library for Arduino
 *
 * Copyright 2010 Sony Corporation
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "HardwareSerial.h"
#include "SoftwareSerial.h"
#include "libRCS620S.h"

/* --------------------------------
 * Constant
 * -------------------------------- */

#define RCS620S_DEFAULT_TIMEOUT  1000

/* --------------------------------
 * Variable
 * -------------------------------- */

/* --------------------------------
 * Prototype Declaration
 * -------------------------------- */

/* --------------------------------
 * Macro
 * -------------------------------- */

/* --------------------------------
 * Function
 * -------------------------------- */

/* ------------------------
 * public
 * ------------------------ */

//RCS620S::RCS620S()
RCS620S::RCS620S(SoftwareSerial *softSerial)
{
    this->serialPort = softSerial;
    this->timeout = RCS620S_DEFAULT_TIMEOUT;
}

SINT RCS620S::initDevice(void)
{
    SINT ret;
    SBYT response[RCS620S_MAX_RW_RESPONSE_LEN];
    DBYT responseLen;

    /* RFConfiguration (various timings) */
    ret = rwCommand((const SBYT *)"\xd4\x32\x02\x00\x00\x00", 6, response, &responseLen);
    if (!ret || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
        return 0;
    }

    /* RFConfiguration (max retries) */
    ret = rwCommand((const SBYT *)"\xd4\x32\x05\x00\x00\x00", 6, response, &responseLen);
    if (!ret || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
        return 0;
    }

    /* RFConfiguration (additional wait time = 24ms) */
    ret = rwCommand((const SBYT *)"\xd4\x32\x81\xff", 4, response, &responseLen);
    if (!ret || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
        return 0;
    }

    return 1;
}

SINT RCS620S::pollingFeliCa(DBYT systemCode)
{
    SINT ret;
    SBYT buf[9];
    SBYT response[RCS620S_MAX_RW_RESPONSE_LEN];
    DBYT responseLen;
    
    /* InListPassiveTarget */
    memcpy(buf, "\xd4\x4a\x01\x01\x00\xff\xff\x00\x00", 9);
    buf[5] = (uint8_t)((systemCode >> 8) & 0xff);
    buf[6] = (uint8_t)((systemCode >> 0) & 0xff);
    
    ret = rwCommand(buf, 9, response, &responseLen);
    if (!ret || (responseLen != 22) ||
        (memcmp(response, "\xd5\x4b\x01\x01\x12\x01", 6) != 0)) {
        return 0;
    }
    
    memcpy(this->idm, response + 6, 8);
    memcpy(this->pmm, response + 14, 8);
    
    return 1;
}

SINT RCS620S::pollingTypeA()
{
    SINT ret;
    SBYT buf[4];
    SBYT response[RCS620S_MAX_RW_RESPONSE_LEN];
    DBYT responseLen;

    /* InListPassiveTarget */
    memcpy(buf, "\xd4\x4a\x01\x00", 4);

    ret = rwCommand(buf, 4, response, &responseLen);
    if (!ret || (memcmp(response, "\xd5\x4b\x01\x01", 4) != 0)) {
        return 0;
    }
    nfcidlen = response[7];
    memcpy(this->nfcid, response + 8, this->nfcidlen);

    return 1;
}

SINT RCS620S::cardCommand(const SBYT *command,
                          SBYT commandLen,
                          SBYT response[RCS620S_MAX_CARD_RESPONSE_LEN],
                          SBYT *responseLen)
{
    SINT ret;
    DBYT commandTimeout;
    SBYT buf[RCS620S_MAX_RW_RESPONSE_LEN];
    DBYT len;
    
    if (this->timeout >= (0x10000 / 2)) {
        commandTimeout = 0xffff;
    } else {
        commandTimeout = (DBYT)(this->timeout * 2);
    }
    
    /* CommunicateThruEX */
    buf[0] = 0xd4;
    buf[1] = 0xa0;
    buf[2] = (SBYT)((commandTimeout >> 0) & 0xff);
    buf[3] = (SBYT)((commandTimeout >> 8) & 0xff);
    buf[4] = (SBYT)(commandLen + 1);
    memcpy(buf + 5, command, commandLen);
    
    ret = rwCommand(buf, 5 + commandLen, buf, &len);
    if (!ret || (len < 4) || (memcmp(buf, "\xd5\xa1\x00", 3) != 0) || (len != (3 + buf[3]))) {
        return 0;
    }
    
    *responseLen = (SBYT)(buf[3] - 1);
    memcpy(response, buf + 4, *responseLen);
    
    return 1;
}

SINT RCS620S::cardDataExchange(const SBYT *command,
                               SBYT commandLen,
                               SBYT response[RCS620S_MAX_CARD_RESPONSE_LEN])
{
    SINT ret;
    DBYT commandTimeout;
    SBYT buf[RCS620S_MAX_RW_RESPONSE_LEN];
    DBYT len;
    
    /* cardDataExchange */
    buf[0] = 0xd4;
    buf[1] = 0x40;
    buf[2] = 0x01;
    memcpy(buf + 3, command, commandLen);
    
    ret = rwCommand(buf, 3 + commandLen, buf, &len);
    if (!ret || (memcmp(buf, "\xd5\x41\x00", 3) != 0)) {
        return 0;
    }

    memcpy(response, buf + 3, 4);
    
    return 1;
}

SINT RCS620S::rfOff(void)
{
    SINT ret;
    SBYT response[RCS620S_MAX_RW_RESPONSE_LEN];
    DBYT responseLen;

    /* RFConfiguration (RF field) */
    ret = rwCommand((const SBYT *)"\xd4\x32\x01\x00", 4,
                    response, &responseLen);
    if (!ret || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
        return 0;
    }

    return 1;
}

SINT RCS620S::push(const SBYT *data,
                   SBYT dataLen)
{
    SINT ret;
    SBYT buf[RCS620S_MAX_CARD_RESPONSE_LEN];
    SBYT responseLen;

    if (dataLen > 224) {
        return 0;
    }

    /* Push */
    buf[0] = 0xb0;
    memcpy(buf + 1, this->idm, 8);
    buf[9] = dataLen;
    memcpy(buf + 10, data, dataLen);

    ret = cardCommand(buf, 10 + dataLen, buf, &responseLen);
    if (!ret || (responseLen != 10) || (buf[0] != 0xb1) || (memcmp(buf + 1, this->idm, 8) != 0) || (buf[9] != dataLen)) {
        return 0;
    }

    buf[0] = 0xa4;
    memcpy(buf + 1, this->idm, 8);
    buf[9] = 0x00;

    ret = cardCommand(buf, 10, buf, &responseLen);
    if (!ret || (responseLen != 10) || (buf[0] != 0xa5) || (memcmp(buf + 1, this->idm, 8) != 0) || (buf[9] != 0x00)) {
        return 0;
    }

    delaym(1000);

    return 1;
}

/* ------------------------
 * private
 * ------------------------ */

SINT RCS620S::rwCommand(const SBYT *command,
                        DBYT commandLen,
                        SBYT response[RCS620S_MAX_RW_RESPONSE_LEN],
                        DBYT *responseLen)
{
    SINT ret;
    SBYT buf[9];

    flushSerial();

    SBYT dcs = calcDCS(command, commandLen);

    /* transmit the command */
    buf[0] = 0x00;
    buf[1] = 0x00;
    buf[2] = 0xff;
    if (commandLen <= 255) {
        /* normal frame */
        buf[3] = commandLen;
        buf[4] = (SBYT)-buf[3];
        writeSerial(buf, 5);
    } else {
        /* extended frame */
        buf[3] = 0xff;
        buf[4] = 0xff;
        buf[5] = (SBYT)((commandLen >> 8) & 0xff);
        buf[6] = (SBYT)((commandLen >> 0) & 0xff);
        buf[7] = (SBYT)-(buf[5] + buf[6]);
        writeSerial(buf, 8);
    }
    writeSerial(command, commandLen);
    buf[0] = dcs;
    buf[1] = 0x00;
    writeSerial(buf, 2);

    /* receive an ACK */
    ret = readSerial(buf, 6);
    if (!ret || (memcmp(buf, "\x00\x00\xff\x00\xff\x00", 6) != 0)) {
        cancel();
        return 0;
    }

    /* receive a response */
    ret = readSerial(buf, 5);
    if (!ret) {
        cancel();
        return 0;
    } else if  (memcmp(buf, "\x00\x00\xff", 3) != 0) {
        return 0;
    }
    if ((buf[3] == 0xff) && (buf[4] == 0xff)) {
        ret = readSerial(buf + 5, 3);
        if (!ret || (((buf[5] + buf[6] + buf[7]) & 0xff) != 0)) {
            return 0;
        }
        *responseLen = (((DBYT)buf[5] << 8) | ((DBYT)buf[6] << 0));
    } else {
        if (((buf[3] + buf[4]) & 0xff) != 0) {
            return 0;
        }
        *responseLen = buf[3];
    }
    if (*responseLen > RCS620S_MAX_RW_RESPONSE_LEN) {
        return 0;
    }

    ret = readSerial(response, *responseLen);
    if (!ret) {
        cancel();
        return 0;
    }

    dcs = calcDCS(response, *responseLen);

    ret = readSerial(buf, 2);
    if (!ret || (buf[0] != dcs) || (buf[1] != 0x00)) {
        cancel();
        return 0;
    }

    return 1;
}

void RCS620S::cancel(void)
{
    /* transmit an ACK */
    writeSerial((const SBYT *)"\x00\x00\xff\x00\xff\x00", 6);
    delaym(1);
    flushSerial();
}

SBYT RCS620S::calcDCS(const SBYT *data,
                      DBYT len)
{
    SBYT sum = 0;

    for (DBYT i = 0; i < len; i++) {
        sum += data[i];
    }

    return (SBYT)-(sum & 0xff);
}

void RCS620S::writeSerial(const SBYT *data,
                          DBYT len)
{
    if (serialPort) {
        serialPort->write(data, len);
    } else {
        Serial.write(data, len);
    }
}

SINT RCS620S::readSerial(SBYT *data,
                         DBYT len)
{
    DBYT nread = 0;
    QBYT t0 = msec();

    while (nread < len) {
        if (checkTimeout(t0)) {
            return 0;
        }

        if (serialPort)	{
			if (serialPort->available() > 0) {
				data[nread] = serialPort->read();
				nread++;
			}
		} else {
			if (Serial.available() > 0) {
				data[nread] = Serial.read();
				nread++;
			}
		}
    }

    return 1;
}

void RCS620S::flushSerial(void)
{
	if (serialPort) {
		serialPort->flush();
	} else {
		Serial.flush();
	}
}

QBYT RCS620S::msec() {
    return millis();
}

QBYT RCS620S::usec() {
    return micros();
}

void RCS620S::delaym(QBYT time) {
    delay(time);
}

void RCS620S::delayu(QBYT time) {
    delayMicroseconds(time);
}

SINT RCS620S::checkTimeout(QBYT t0)
{
    QBYT t = msec();

    if ((t - t0) >= this->timeout) {
        return 1;
    }

    return 0;
}