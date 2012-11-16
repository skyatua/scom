#ifndef _MTBusManager_h_
#define _MTBusManager_h_

#include "MTBusServer.h"



extern MTBusServer_t ServerA;
extern MTBusServer_t ServerB;

 
extern void MTBusManagerStart(void);
extern void MTBusManagerRun(void);
extern void MTBusManagerTimeOutCounter(void);
extern void MTBusManagerDataReceiver(MTBusServer_t *Server,uint8_t *pData,uint16_t len);

#endif