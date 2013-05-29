//
//  libSTP.h
//  
//
//  Created by Yu Kusakabe on 2013/05/22.
//
//
#include <inttypes.h>

#ifndef ____libSTP__
#define ____libSTP__

#define BROADCAST_ADDR          0xFF    //Broadcast Address

#define PACKET_MAX_LEN          32      //Max 256
#define CHAR_DELAY_TIME         300     //us
#define PACKET_DELAY_TIME       10000   //us
#define SEND_TIMEOUT            10000   //us
#define RECV_TIMEOUT            12000   //us

class SoftwareSerial;
class STP
{
public:
    STP(SoftwareSerial *softSerial = NULL);
    
    void initSTP(uint8_t dipval);
    int sendPacket(uint8_t sendaddr,
                   const uint8_t *data,
                   uint8_t len);
    int recvPacket(uint8_t *fromaddr,
                   uint8_t *data,
                   uint8_t *len);

private:
    void writeSerial(const uint8_t *data,
                     uint16_t len);
    int readSerial();
    int availableSerial();
    void flushSerial();
    int checkTimeout(unsigned long t0,
                     unsigned long timeout);

public:
    uint8_t myaddr;
    
private:
    SoftwareSerial *serialPort;
};




#endif /* defined(____libSTP__) */
