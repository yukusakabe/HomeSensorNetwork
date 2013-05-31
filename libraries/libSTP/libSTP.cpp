//
//  libSTP.cpp
//  
//
//  Created by Yu Kusakabe on 2013/05/22.
//
//

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "Print.h"
#include "HardwareSerial.h"
#include "SoftwareSerial.h"

#include "libSTP.h"

/* ------------------------
 * public
 * ------------------------ */

STP::STP(DBYT rate,
         DBYT addr)
{
    this->serialPort = NULL;
    this->bitrate = rate;
    this->delaytime = (1000000 / (this->bitrate / 8)) * 1.2;
    this->timeout = this->delaytime * (PACKET_MAX_LEN + 8) * 3;
}

STP::STP(SoftwareSerial *softSerial,
         DBYT rate,
         DBYT addr)
{
    this->serialPort = softSerial;
    this->bitrate = rate;
    this->delaytime = (1000000 / (this->bitrate / 8)) * 1.2;
    this->timeout = this->delaytime * (PACKET_MAX_LEN + 8) * 3;
}

SBYT STP::sendPacket(SBYT sendaddr,
                     SBYT command,
                     SBYT *data,
                     SBYT len)
{
    SBYT i;
    DBYT crc;
    QBYT t0;
    
    SBYT buf[PACKET_MAX_LEN + 8];
    buf[0] = sendaddr;
    buf[1] = myaddr;
    buf[2] = command;
    crc = calcCRC16(buf, 3);
    memcpy(buf + 3, &crc, 2);
    buf[5] = len;
    memcpy(buf + 6, data, len);
    crc = calcCRC16(buf + 5, len + 1);
    memcpy(buf + 6 + len, &crc, 2);
    
    t0 = usec();
    
    while (1) {
        if (checkTimeout(t0, this->timeout)) {
            return 1;
        }
        
        i = availableSerial();
        delayu(delaytime);
        if (availableSerial() > i) {
            delayu(random(this->delaytime * 8, this->delaytime * 16));
        } else {
            break;
        }
    }
    
    writeSerial(buf, len + 8);
    
    return 0;
}

SBYT STP::recvPacket(SBYT *fromaddr,
                     SBYT *command,
                     SBYT *data,
                     SBYT *len)
{
    SBYT i = 0;
    DBYT crc;
    SBYT buf[PACKET_MAX_LEN + 8];
    QBYT t0;
    
    if (availableSerial() == 0) {
        return 1;
    }
    
    t0 = usec();
    while (i <= 5) {
        if (checkTimeout(t0, this->delaytime * 8)) {
            flushSerial();
            return 1;
        }
        
        if (availableSerial() > 0) {
            buf[i] = readSerial();
            i++;
        } else {
            delayu(this->delaytime);
        }
    }
    
    crc = calcCRC16(buf, 3);
    if (memcmp(buf + 3, &crc, 2) != 0) {
        flushSerial();
        return 1;
    }
    
    t0 = usec();
    while (i < 6 + buf[5] + 2) {
        if (checkTimeout(t0, this->timeout)) {
            flushSerial();
            return 2;
        }

        if (availableSerial() > 0) {
            buf[i] = readSerial();
            i++;
        } else {
            delayu(this->delaytime);
        }
    }
    
    crc = calcCRC16(buf + 5, buf[5] + 1);
    if (memcmp(buf + 6 + buf[5], &crc, 2) != 0) {
        flushSerial();
        return 2;
    }
    
    if (buf[0] != myaddr && buf[0] != BROADCAST_ADDR) {
        return 1;
    }
    
    *fromaddr = buf[1];
    *command = buf[2];
    *len = buf[5];
    memcpy(data, buf + 6, buf[5]);
    
    return 0;
}

DBYT STP::calcCRC16(SBYT *data,
                    DBYT len)
{
    SBYT i, j, buf;
    DBYT crc = 0xFFFF;
    
    for (i = 0; i < len; i++) {
        crc ^= (DBYT)(data[i] << 8);
        for (j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc <<= 1;
                crc ^= CRC_CCITT16;
            } else {
                crc <<= 1;
            }
        }
    }
    
    return crc;
}

/* ------------------------
 * private
 * ------------------------ */

void STP::writeSerial(SBYT* data,
                      DBYT len)
{
    if (serialPort) {
        serialPort->write(data, len);
    } else {
        Serial.write(data, len);
    }
}

SBYT STP::readSerial()
{
    if (serialPort) {
        return serialPort->read();
    } else {
        return Serial.read();
    }
}

SBYT STP::availableSerial()
{
    if (serialPort) {
        return serialPort->available();
    } else {
        return Serial.available();
    }
}

void STP::flushSerial()
{
	if (serialPort) {
		serialPort->flush();
	} else {
		Serial.flush();
	}
}

QBYT STP::usec() {
    return micros();
}

void STP::delayu(QBYT time) {
    delayMicroseconds(time);
}

SBYT STP::checkTimeout(QBYT t0,
                       QBYT timeout)
{
    QBYT t = usec();
    
    if ((t - t0) >= timeout) {
        return 1;
    }
    
    return 0;
}