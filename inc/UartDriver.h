//UartDriver for STM32F2xx
//Created by Ihor Bilorus at 30.12.2011
//Last Modified by Ihor Bilorus at 20.04.2012 (added UART2)


//UART2 íå ïðîòåñòèðîâàí

#ifndef _UartDriver_h
#define _UartDriver_h

#ifdef __cplusplus
 extern "C" {
#endif

#ifdef __STM32__
#include "stm32f2xx_usart.h"   
#endif

#ifdef __WINX__
#include <windows.h>
#endif

#ifdef __LINX__
#include <unistd.h>			
#include <fcntl.h>			
#include <termios.h>		
#endif
   
typedef enum
{
#ifdef __STM32__	
USART_BAUDRATE_9600   = 9600,  
USART_BAUDRATE_14400  = 14400,
USART_BAUDRATE_19200  = 19200,
USART_BAUDRATE_38400  = 38400,
USART_BAUDRATE_56000  = 56000,
USART_BAUDRATE_57600  = 57600,
USART_BAUDRATE_115200 = 115200,
USART_BAUDRATE_128000 = 128000,
#endif
#ifdef __WINX__
USART_BAUDRATE_110    = CBR_110,
USART_BAUDRATE_300    = CBR_300,
USART_BAUDRATE_600    = CBR_600,
USART_BAUDRATE_1200   = CBR_1200,
USART_BAUDRATE_2400   = CBR_2400,
USART_BAUDRATE_4800   = CBR_4800,
USART_BAUDRATE_9600   = CBR_9600,
USART_BAUDRATE_14400  = CBR_14400,
USART_BAUDRATE_19200  = CBR_19200,
USART_BAUDRATE_38400  = CBR_38400,
USART_BAUDRATE_56000  = CBR_56000,
USART_BAUDRATE_57600  = CBR_57600,
USART_BAUDRATE_115200 = CBR_115200,
USART_BAUDRATE_128000 = CBR_128000,
USART_BAUDRATE_256000 = CBR_256000,
#endif
#ifdef __LINX__
USART_BAUDRATE_9600   = B9600,  
USART_BAUDRATE_19200  = B19200,
USART_BAUDRATE_38400  = B38400,
USART_BAUDRATE_57600  = B57600,
USART_BAUDRATE_115200 = B115200,
#endif
}baudrate_t;   
   
typedef enum
{
#ifdef __STM32__	
UART_DATA8       = USART_WordLength_8b,
UART_DATA9       = USART_WordLength_9b
#endif
#ifdef __LINX__  
UART_DATA5       = CS5,
UART_DATA6       = CS6, 
UART_DATA7       = CS7,
UART_DATA8       = CS8 
#endif
#ifdef __WINX__
UART_DATA5       = DATABITS_5,
UART_DATA6       = DATABITS_6, 
UART_DATA7       = DATABITS_7,
UART_DATA8       = DATABITS_8
#endif
}UartDataLen_t;

typedef enum
{
#ifdef 	__STM32__
UART_STOPBIT_1   = USART_StopBits_1,
UART_STOPBIT_0_5 = USART_StopBits_0_5,
UART_STOPBIT_2   = USART_StopBits_2,
UART_STOPBIT_1_5 = USART_StopBits_1_5
#endif
#ifdef __LINX__
UART_STOPBIT_1   = 0x01,
UART_STOPBIT_0_5 = 0x02,
UART_STOPBIT_2   = 0x03,
UART_STOPBIT_1_5 = 0x04
#endif
#ifdef __WINX__
UART_STOPBIT_1   = ONESTOPBIT,
UART_STOPBIT_1_5 = ONE5STOPBITS,
UART_STOPBIT_2   = TWOSTOPBITS,
#endif
}UartStopBits_t;

typedef enum
{
#ifdef __STM32__	
UART_FLOW_CONTROL_NONE    = USART_HardwareFlowControl_None,
UART_FLOW_CONTROL_RTS     = USART_HardwareFlowControl_RTS,
UART_FLOW_CONTROL_CTS     = USART_HardwareFlowControl_CTS,
UART_FLOW_CONTROL_RTS_CTS = USART_HardwareFlowControl_RTS_CTS,
#endif
#ifdef __LINX__
UART_FLOW_CONTROL_NONE    = 0x00,
UART_FLOW_CONTROL_RTS     = 0x01,
UART_FLOW_CONTROL_CTS     = 0x02,
UART_FLOW_CONTROL_RTS_CTS = 0x03,
#endif
#ifdef __WINX__
UART_FLOW_CONTROL_NONE    = 0x00,
#endif
}UartFlowControl_t;

typedef enum
{
#ifdef __STM32__	
UART_PARITY_NONE = USART_Parity_No,
UART_PARITY_EVEN = USART_Parity_Even,
UART_PARITY_ODD  = USART_Parity_Odd,
#endif
#ifdef __LINX__
UART_PARITY_NONE = 0x00,
UART_PARITY_EVEN = 0x01,
UART_PARITY_ODD  = 0x02,
#endif
#ifdef __WINX__
UART_PARITY_NONE   = PARITY_NONE,
UART_PARITY_ODD    = PARITY_ODD,
UART_PARITY_EVEN   = PARITY_EVEN,
UART_PARITY_MARK   = PARITY_MARK,
UART_PARITY_SPACE  = PARITY_SPACE,
#endif
}UartParity_t;

typedef enum
{
ENABLE_DMA_RX_CHANELL  = 0x01,
DISABLE_DMA_RX_CHANELL = 0x02,
ENABLE_DMA_TX_CHANELL  = 0x04,
DISABLE_DMA_TX_CHANELL = 0x08,
}HardAcceleratorCmd_t;

typedef struct
{
#ifdef __STM32__	
USART_TypeDef       *chanell;
#endif
#ifdef __LINX__
uint8_t             chanell;
#endif
#ifdef __WINX__
uint8_t             chanell;
#endif
baudrate_t          baudrate;
UartDataLen_t       dataLength;
UartStopBits_t      stopbits;
UartParity_t        parity;
uint8_t             HardwareAccelerator;
UartFlowControl_t   flowControl;
uint8_t             *rxBuff;
uint16_t             rxBuffSize;
void(*rxCallback)(uint8_t *,uint8_t);
void(*txCallback)(void);
}UartDescriptor_t;

extern int8_t OpenUart(UartDescriptor_t *descriptor);
extern void CloseUart(UartDescriptor_t * descriptor);
extern void WriteUart(UartDescriptor_t *uartDescriptor,uint8_t *p,uint16_t len);
extern void UartDriverRun(void);

extern void test_wr(void );
#ifdef __cplusplus
}
#endif   


#endif