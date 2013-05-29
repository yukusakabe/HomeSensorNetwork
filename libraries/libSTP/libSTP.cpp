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

STP::STP(SoftwareSerial *softSerial)
{
    this->serialPort = softSerial;
}

void STP::initSTP(uint8_t dipval)
{
    this->myaddr = dipval;
}

int STP::sendPacket(uint8_t sendaddr,
                    const uint8_t *data,
                    uint8_t len)
{
    uint8_t i;
    unsigned long t0;
    
    uint8_t buf[PACKET_MAX_LEN + 4];
    buf[0] = sendaddr;
    buf[1] = myaddr;
    buf[2] = sendaddr ^ myaddr;
    buf[3] = len;
    memcpy(buf + 4, data, len);
    
    
    t0 = micros();
    
    while (1) {
        if (checkTimeout(t0, SEND_TIMEOUT)) {
            return 1;
        }
        
        i = availableSerial();
        delayMicroseconds(CHAR_DELAY_TIME);
        if (availableSerial() > i) {
            delayMicroseconds(random(PACKET_DELAY_TIME, PACKET_DELAY_TIME + 1000));
        } else {
            break;
        }
    }
    
    writeSerial(buf, len + 4);
    
    return 0;
}

int STP::recvPacket(uint8_t *fromaddr,
                    uint8_t *data,
                    uint8_t *len)
{
    uint8_t i = 0;
    uint8_t header[4];
    unsigned long t0 = micros();
    
    if (availableSerial() == 0) {
        return 1;
    }
    
    while (i < 4) {
        if (checkTimeout(t0, RECV_TIMEOUT)) {
            flushSerial();
            return 1;
        }
        
        if (availableSerial() > 0) {
            header[i] = readSerial();
            i++;
        } else {
            delayMicroseconds(CHAR_DELAY_TIME);
        }
    }
    
    if (header[0] ^ header[1] != header[2]) {
        flushSerial();
        return 1;
    } else {
        i = 0;
        t0 = micros();
        
        while (i < header[3]) {
            if (checkTimeout(t0, RECV_TIMEOUT)) {
                flushSerial();
                return 2;
            }
            
            if (availableSerial() > 0) {
                data[i] = readSerial();
                i++;
            } else {
                delayMicroseconds(CHAR_DELAY_TIME);
            }
        }
    }
    fromaddr[0] = header[1];
    len[0] = header[3];
    flushSerial();
    
    return 0;
}

/* ------------------------
 * private
 * ------------------------ */

void STP::writeSerial(const uint8_t* data,
                            uint16_t len)
{
    if (serialPort) {
        serialPort->write(data, len);
    } else {
        Serial.write(data, len);
    }
}

int STP::readSerial()
{
    if (serialPort) {
        return serialPort->read();
    } else {
        return Serial.read();
    }
}

int STP::availableSerial()
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

int STP::checkTimeout(unsigned long t0, unsigned long timeout)
{
    unsigned long t = micros();
    
    if ((t - t0) >= timeout) {
        return 1;
    }
    
    return 0;
}