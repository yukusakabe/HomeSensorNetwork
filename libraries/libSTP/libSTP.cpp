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
    this->timeout = this->delaytime * (PACKET_MAX_LEN) * 2;
}

STP::STP(SoftwareSerial *softSerial,
         DBYT rate,
         DBYT addr)
{
    this->serialPort = softSerial;
    this->bitrate = rate;
    this->delaytime = (1000000 / (this->bitrate / 8)) * 1.2;
    this->timeout = this->delaytime * (PACKET_MAX_LEN) * 2;
}

SBYT STP::sendPacket(SBYT sendaddr,
                     SBYT *data,
                     SBYT len)
{
    SBYT buf[PACKET_MAX_LEN];
    SBYT i;
    QBYT t0;

    buf[0] = sendaddr;
    buf[1] = myaddr;
    buf[2] = len + 4;
    buf[3] = calcCRC8(buf, 3);
    
    if (len != 0) {
        memcpy(buf + 4, data, len);
    }
    
    t0 = usec();
    while (1) {
        if (checkTimeout(t0, this->timeout)) {
            return 1;
        }
        
        i = availableSerial();
        delayu(delaytime);
        if (availableSerial() > i) {
            delayu(random(this->delaytime * 4, this->delaytime * 16));
        } else {
            break;
        }
    }
    
    writeSerial(buf, len + 4);
    
    return 0;
}

SBYT STP::recvPacket(SBYT *fromaddr,
                     SBYT *data,
                     SBYT *len)
{
    SBYT buf[PACKET_MAX_LEN];
    SBYT i = 0;
    QBYT t0;
    
    if (availableSerial() == 0) {
        return 1;
    }
    
    t0 = usec();
    while (i < 4) {
        if (checkTimeout(t0, this->delaytime * HEDDER_LEN * 2)) {
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
    
    if (calcCRC8(buf, 3) != buf[3]) {
        flushSerial();
        return 1;
    }

    t0 = usec();
    while (i < buf[2]) {
        if (checkTimeout(t0, this->delaytime * (buf[2] - 4) * 2)) {
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
    
    if (buf[0] == myaddr || buf[0] == BROADCAST_ADDR) {
        *fromaddr = buf[1];
        *len = buf[2];
        memcpy(data, buf + 4, buf[2] - 4);
        return 0;
    }
    
    return 1;
}

SBYT STP::calcCRC8(SBYT *data,
                   DBYT len)
{
    SBYT i, j;
    SBYT crc = 0xFF;
    
    for (i = 0; i < len; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc <<= 1;
                crc ^= CRC_8_CCITT;
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