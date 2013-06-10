//
//  libKeyManager.h
//  
//
//  Created by Yu Kusakabe on 2013/06/02.
//
//

#ifndef ____libKeyManager__
#define ____libKeyManager__

#define INTERVAL    2000

#include <inttypes.h>
#include "TypeDefinition.h"
#include "libSTP.h"

class KeyManager
{
public:
    KeyManager(STP *stpa,
               SBYT led,
               SBYT lock,
               SBYT door);
    
    
    void execComm();
    
private:
    SBYT readState();
    void autoClose();
    SBYT openKey(DBYT time);
    SBYT closeKey();
    SBYT checkTime(QBYT t0,
                   QBYT timeout);
    
private:
    STP *stpPort;
    SBYT keypin;
    SBYT lockpin;
    SBYT doorpin;
    
    SBYT lockstate;
    SBYT doorstate;
    QBYT closeTime;
    QBYT setTimeC;
    QBYT setTimeI;

};


#endif /* defined(____libKeyManager__) */
