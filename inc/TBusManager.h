#ifndef _TBusManager_h
#define _TBusManager_h

#include "Terabus.h"

#define TeraDeviceName             "Регистр4kUSB"     //Имя прибора
#define TeraDeviceProgrammVersion  "v4.001.02"        //Версия программы прибора

extern uint8_t NetworkDeviceAddr;                 //Сетевой адрес прибора 

extern void TBusManagerInit(void);
extern void TBusManagerRun(void);
extern void TBusManagerTimeOutTimer(void);

extern TBusSlave_t TBusSlave;


#endif