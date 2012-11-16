#ifndef _TBusManager_h
#define _TBusManager_h

#include "Terabus.h"

#define TeraDeviceName             "�������4kUSB"     //��� �������
#define TeraDeviceProgrammVersion  "v4.001.02"        //������ ��������� �������

extern uint8_t NetworkDeviceAddr;                 //������� ����� ������� 

extern void TBusManagerInit(void);
extern void TBusManagerRun(void);
extern void TBusManagerTimeOutTimer(void);

extern TBusSlave_t TBusSlave;


#endif