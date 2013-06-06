//
//  libSTP.h
//  
//
//  Created by Yu Kusakabe on 2013/05/22.
//
//

#ifndef ____libSTP__
#define ____libSTP__

#include <inttypes.h>
#include "SoftwareSerial.h"
#include "TypeDefinition.h"

#define BROADCAST_ADDR          0xFF    //Broadcast Address
#define PACKET_MAX_LEN          32      //
#define HEDDER_LEN              4       //
#define CRC_8_CCITT             0x8D    //CRC-8-CCITT

class STP
{
public:
    STP(DBYT rate,
        DBYT addr);
    STP(SoftwareSerial *softSerial,
        DBYT rate,
        DBYT addr);
    SINT sendPacket(SBYT sendaddr,
                    SBYT *data,
                    SBYT len);
    SINT recvPacket(SBYT *fromaddr,
                    SBYT *data,
                    SBYT *len);
    SBYT calcCRC8(SBYT *data,
                  DBYT length);
private:
    void writeSerial(SBYT *data,
                     DBYT len);
    SBYT readSerial();
    SBYT availableSerial();
    void flushSerial();
    QBYT usec();
    void delayu(QBYT time);
    SINT checkTimeout(QBYT t0,
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
