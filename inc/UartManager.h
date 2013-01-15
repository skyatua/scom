#ifndef _UartManager_h
#define _UartManager_h

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
//#include "GpioDriver.h"

//UART1 ïðåäíàçíà÷åí äëÿ ïîäêëþ÷åíèÿ ê ÏÊ (slave)
//UART6 ïðåäíàçíà÷åí äëÿ ïîäêëþ÷åíèÿ ñåòè îïðàøèâàåìûõ ïðèáîðîâ (master)
//UART3 ïðåäíàçíà÷åí äëÿ ïîäêëþ÷åíèÿ êîîðäèíàòîðà



typedef enum
{
PORT_FREE,
PORT_BUSY
}PortStatus_t;//Ñòàòóñ UART - ñâîáîäåí/çàíÿò

extern PortStatus_t SlavePortSatus;//Ñòàòóñ UART - ñâîáîäåí/çàíÿò
extern uint8_t uart_speed_index;

extern void StartUart1Manager(void);
extern void StartUart3Manager(void);
extern void StartUart6Manager(void);
extern void StopUart6Manager(void);
extern void StopUart3Manager( void );

extern void SlaveSendPacket(uint8_t * p,uint16_t len);
extern void Uart3SendPacket(uint8_t * p,uint16_t len);
extern void Uart6SendPacket(uint8_t * p,uint16_t len);
extern uint8_t GetSlavePortStatus(void);
extern void SetSlavePortStatus(uint8_t Status);

#ifdef __cplusplus
}
#endif

#endif
