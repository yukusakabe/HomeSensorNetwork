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

#define SBYT                    uint8_t
#define DBYT                    uint16_t
#define QBYT                    uint32_t
#define OBYT                    uint64_t

#define BROADCAST_ADDR          0xFF    //Broadcast Address
#define PACKET_MAX_LEN          32      //
#define CRC_CCITT16             0x1021  //CRC G(x)=X16+X12+X5+1=0x(1)1021
#define RESPNSE_TIMEOUT         500000

class SoftwareSerial;
class STP
{
public:
    STP(DBYT rate,
        DBYT addr);
    STP(SoftwareSerial *softSerial,
        DBYT rate,
        DBYT addr);
    SBYT sendPacket(SBYT sendaddr,
                    SBYT command,
                    SBYT *data,
                    SBYT len);
    SBYT recvPacket(SBYT *fromaddr,
                    SBYT *command,
                    SBYT *data,
                    SBYT *len);
    DBYT calcCRC16(SBYT *data,
                   DBYT length);
private:
    void writeSerial(SBYT *data,
                     DBYT len);
    SBYT readSerial();
    SBYT availableSerial();
    void flushSerial();
    QBYT usec();
    void delayu(QBYT time);
    SBYT checkTimeout(QBYT t0,
                      QBYT timeout);
    

public:
    DBYT bitrate;
    DBYT delaytime;
    QBYT timeout;
    SBYT myaddr;
    
private:
    SoftwareSerial *serialPort;
};




#endif /* defined(____libSTP__) */
