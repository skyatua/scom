#include "../inc/includes.h"

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
Free,
Busy
}Status_t;

typedef struct
{
uint8_t   InitComplete; //Ïðèçíàê òîãî, ÷òî äàííûé UART ïðîèíèöèàëèçèðîâàí
uint8_t   *pTxBuff;
uint8_t   *pRxBuff;
uint16_t  TxLen;
uint16_t  RxLen;
uint16_t  RxFirst;
uint16_t  RxLast;
Status_t  status;
uint8_t   HardwareAccelerator;
uint16_t  DmaCnt;
int32_t   hSerial;                  // handle for serial port
void(*RxCallback)(uint8_t *,uint8_t);
void(*TxCallback)(void);
}UART_t;

UART_t uart3;
UART_t uart1;
UART_t uart6;
UART_t uart2;

//------------------------------Èíèöèàëèçàöèÿ UART-----------------------------//
int8_t OpenUart(UartDescriptor_t *descriptor)
{
/*
#ifdef __STM32__
NVIC_InitTypeDef    NVIC_InitStructure;
USART_InitTypeDef   USART_InitStructure;
DMA_InitTypeDef     DMA_InitStructure;
GPIO_InitTypeDef    GPIO;


USART_DeInit(descriptor->chanell);

if(USART3 == descriptor->chanell) //Åñëè èíèöèàëèçèðóåì USART3
  {
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE );

  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10, GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource11, GPIO_AF_USART3);

  // Configure USART Tx as alternate function push-pull
  GPIO.GPIO_Mode = GPIO_Mode_AF;
  GPIO.GPIO_Pin = GPIO_Pin_10;
  GPIO.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO);

  // Configure USART Rx as input floating
  GPIO.GPIO_Mode = GPIO_Mode_AF;
  GPIO.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOC, &GPIO);

  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;

  uart3.pRxBuff=(uint8_t *)descriptor->rxBuff;
  uart3.RxLen=descriptor->rxBuffSize;
  uart3.TxCallback=descriptor->txCallback;
  uart3.RxCallback=descriptor->rxCallback;
  uart3.status=Free;
  uart3.RxFirst=0;
  uart3.RxLast=0;
  uart3.HardwareAccelerator=descriptor->HardwareAccelerator;

  if(ENABLE_DMA_RX_CHANELL&descriptor->HardwareAccelerator)
    {
    //Äëÿ îïðåäåëåíèÿ êàíàëîâ è ïîòîêîâ DMA ñì. RM0033 ñòð. 172
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Stream1);
    while (DMA_GetCmdStatus(DMA1_Stream1) != DISABLE){};

    DMA_InitStructure.DMA_Channel            = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t )descriptor->rxBuff;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize         = descriptor->rxBuffSize;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;//DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;

    DMA_Init(DMA1_Stream1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Stream1, ENABLE);
    USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
    }
  else
    {
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    }

  if(ENABLE_DMA_TX_CHANELL&descriptor->HardwareAccelerator)
    {
    NVIC_InitStructure.NVIC_IRQChannel                    = DMA1_Stream3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    }
  else
    {
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    }
  uart3.InitComplete=1;
  }

if(USART1 == descriptor->chanell)
  {
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE );

  GPIO_PinAFConfig(GPIOB,GPIO_PinSource6, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource7, GPIO_AF_USART1);

  // Configure USART Tx as alternate function push-pull
  GPIO.GPIO_Mode  = GPIO_Mode_AF;
  GPIO.GPIO_Pin   = GPIO_Pin_6;
  GPIO.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO);

  // Configure USART Rx as input floating
  GPIO.GPIO_Mode = GPIO_Mode_AF;
  GPIO.GPIO_Pin  = GPIO_Pin_7;
  GPIO.GPIO_Speed = GPIO_Speed_50MHz;
 // GPIO.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO);

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;

  uart1.pRxBuff=(uint8_t *)descriptor->rxBuff;
  uart1.RxLen=descriptor->rxBuffSize;
  uart1.TxCallback=descriptor->txCallback;
  uart1.RxCallback=descriptor->rxCallback;
  uart1.status=Free;
  uart1.RxFirst=0;
  uart1.RxLast=0;
  uart1.HardwareAccelerator=descriptor->HardwareAccelerator;

  if(ENABLE_DMA_RX_CHANELL&descriptor->HardwareAccelerator)
    {
    //Äëÿ îïðåäåëåíèÿ êàíàëîâ è ïîòîêîâ DMA ñì. RM0033 ñòð. 172
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  //!!!!!!!!
    DMA_DeInit(DMA2_Stream2);
    while (DMA_GetCmdStatus(DMA2_Stream2) != DISABLE){};

    DMA_InitStructure.DMA_Channel            = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t )descriptor->rxBuff;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize         = descriptor->rxBuffSize;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;//DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;

    DMA_Init(DMA2_Stream2, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream2, ENABLE);
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
    }
  else
    {
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    }

  if(ENABLE_DMA_TX_CHANELL&descriptor->HardwareAccelerator)
    {
    NVIC_InitStructure.NVIC_IRQChannel                    = DMA2_Stream7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    }
  else
    {
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    }
  uart1.InitComplete=1;
  }


if(USART2 == descriptor->chanell)
  {
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

  GPIO_PinAFConfig(GPIOA,GPIO_PinSource2, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource3, GPIO_AF_USART2);

  // Configure USART Tx as alternate function push-pull
  GPIO.GPIO_Mode  = GPIO_Mode_AF;
  GPIO.GPIO_Pin   = GPIO_Pin_2;
  GPIO.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO);

  // Configure USART Rx as input floating
  GPIO.GPIO_Mode = GPIO_Mode_AF;
  GPIO.GPIO_Pin  = GPIO_Pin_3;
  GPIO.GPIO_Speed = GPIO_Speed_50MHz;
 // GPIO.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO);

  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;

  uart2.pRxBuff=(uint8_t *)descriptor->rxBuff;
  uart2.RxLen=descriptor->rxBuffSize;
  uart2.TxCallback=descriptor->txCallback;
  uart2.RxCallback=descriptor->rxCallback;
  uart2.status=Free;
  uart2.RxFirst=0;
  uart2.RxLast=0;
  uart2.HardwareAccelerator=descriptor->HardwareAccelerator;

  if(ENABLE_DMA_RX_CHANELL&descriptor->HardwareAccelerator)
    {
    //Äëÿ îïðåäåëåíèÿ êàíàëîâ è ïîòîêîâ DMA ñì. RM0033 ñòð. 172
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  //!!!!!!!!
    DMA_DeInit(DMA1_Stream5);
    while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE){};

    DMA_InitStructure.DMA_Channel            = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t )descriptor->rxBuff;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize         = descriptor->rxBuffSize;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;//DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;

    DMA_Init(DMA1_Stream5, &DMA_InitStructure);
    DMA_Cmd(DMA1_Stream5, ENABLE);
    USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
    }
  else
    {
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    }

  if(ENABLE_DMA_TX_CHANELL&descriptor->HardwareAccelerator)
    {
    NVIC_InitStructure.NVIC_IRQChannel                    = DMA1_Stream6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    }
  else
    {
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    }
  uart2.InitComplete=1;
  }

if(USART6 == descriptor->chanell)
  {
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE );

  GPIO_PinAFConfig(GPIOC,GPIO_PinSource6, GPIO_AF_USART6);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource7, GPIO_AF_USART6);

  // Configure USART Tx as alternate function push-pull
  GPIO.GPIO_Mode  = GPIO_Mode_AF;
  GPIO.GPIO_Pin   = GPIO_Pin_6;
  GPIO.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO);

  // Configure USART Rx as input floating
  GPIO.GPIO_Mode = GPIO_Mode_AF;
  GPIO.GPIO_Pin  = GPIO_Pin_7;
  GPIO.GPIO_Speed = GPIO_Speed_50MHz;
 // GPIO.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO);

  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;

  uart6.pRxBuff=(uint8_t *)descriptor->rxBuff;
  uart6.RxLen=descriptor->rxBuffSize;
  uart6.TxCallback=descriptor->txCallback;
  uart6.RxCallback=descriptor->rxCallback;
  uart6.status=Free;
  uart6.RxFirst=0;
  uart6.RxLast=0;
  uart6.HardwareAccelerator=descriptor->HardwareAccelerator;

  if(ENABLE_DMA_RX_CHANELL&descriptor->HardwareAccelerator)
    {
    //Äëÿ îïðåäåëåíèÿ êàíàëîâ è ïîòîêîâ DMA ñì. RM0033 ñòð. 172
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  //!!!!!!!!
    DMA_DeInit(DMA2_Stream1);
    while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){};

    DMA_InitStructure.DMA_Channel            = DMA_Channel_5;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART6->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t )descriptor->rxBuff;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize         = descriptor->rxBuffSize;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;//DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;

    DMA_Init(DMA2_Stream1, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream1, ENABLE);
    USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);
    }
  else
    {
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    }

  if(ENABLE_DMA_TX_CHANELL&descriptor->HardwareAccelerator)
    {
    NVIC_InitStructure.NVIC_IRQChannel                    = DMA2_Stream6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    }
  else
    {
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    }
  uart6.InitComplete=1;
  }


USART_InitStructure.USART_BaudRate    = (uint32_t)descriptor->baudrate;
USART_InitStructure.USART_WordLength  = descriptor->dataLength;
USART_InitStructure.USART_StopBits    = descriptor->stopbits;
USART_InitStructure.USART_Parity      = descriptor->parity;
USART_InitStructure.USART_HardwareFlowControl = descriptor->flowControl;
USART_InitStructure.USART_Mode        = USART_Mode_Rx|USART_Mode_Tx;//descriptor->mode;

// USART configuration
USART_Init(descriptor->chanell, &USART_InitStructure);
USART_ClearFlag(descriptor->chanell,USART_FLAG_CTS|USART_FLAG_LBD|USART_FLAG_TC|USART_FLAG_RXNE);


if(ENABLE_DMA_RX_CHANELL&descriptor->HardwareAccelerator)
  USART_ITConfig(descriptor->chanell, USART_IT_RXNE,DISABLE);//Åñëè ðàçðåøåí DMA, òî çàïðåùàåì ïðåðûâàíèÿ ïî ïðèåìó
else
  USART_ITConfig(descriptor->chanell, USART_IT_RXNE,ENABLE);

USART_ITConfig(descriptor->chanell, USART_IT_TXE, DISABLE);

USART_Cmd(descriptor->chanell, ENABLE);// Enable USART

#endif
*/

#ifdef __WINX__

// for ZigBit
if(0x03 == descriptor->chanell)
  {
  uart3.pRxBuff=(uint8_t *)descriptor->rxBuff;
  uart3.RxLen=descriptor->rxBuffSize;
  uart3.TxCallback=descriptor->txCallback;
  uart3.RxCallback=descriptor->rxCallback;
  uart3.status=Free;
  uart3.RxFirst=0;
  uart3.RxLast=0;
  uart3.hSerial = -1;

  int hSerial = (int) CreateFile
	(	"COM8",//"COM4",
	    GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		0,
		0);
	if (hSerial == -1)
	{  // can't open port
		return -1;
		}

	DCB dcbSP = {0};
	dcbSP.DCBlength = sizeof(dcbSP);
	if (!GetCommState((HANDLE)hSerial,&dcbSP))
	{  // could not get the state of the port
		return -1;
		}

	dcbSP.BaudRate = (uint32_t)descriptor->baudrate;
	dcbSP.ByteSize = descriptor->dataLength;
	dcbSP.StopBits = descriptor->stopbits;
	dcbSP.Parity   = descriptor->parity;

	if (!SetCommState((HANDLE)hSerial,&dcbSP))
	{  // analyse error
		return -1;
		}

	COMMTIMEOUTS timeouts={0};
	timeouts.ReadIntervalTimeout=50;
	timeouts.ReadTotalTimeoutConstant=50;
	timeouts.ReadTotalTimeoutMultiplier=10;
	timeouts.WriteTotalTimeoutConstant=50;
	timeouts.WriteTotalTimeoutMultiplier=10;

	if(!SetCommTimeouts(hSerial, &timeouts))
	{
       return -1;
     }

    uart3.hSerial = hSerial;
    uart3.InitComplete=1;
  }

//
if(0x06 == descriptor->chanell)
  {
  uart6.pRxBuff=(uint8_t *)descriptor->rxBuff;
  uart6.RxLen=descriptor->rxBuffSize;
  uart6.TxCallback=descriptor->txCallback;
  uart6.RxCallback=descriptor->rxCallback;
  uart6.status=Free;
  uart6.RxFirst=0;
  uart6.RxLast=0;
  uart6.hSerial = -1;
  uart6.InitComplete = 0;

  int hSerial = (int) CreateFile
	(	"COM8",//"COM4",
	    GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		0,
		0);
	if (hSerial == -1)
	{  // can't open port
		return -1;
		}

	DCB dcbSP = {0};
	dcbSP.DCBlength = sizeof(dcbSP);
	if (!GetCommState((HANDLE)hSerial,&dcbSP))
	{  // could not get the state of the port
		return -1;
		}

	dcbSP.BaudRate = (uint32_t)descriptor->baudrate;
	dcbSP.ByteSize = descriptor->dataLength;
	dcbSP.StopBits = descriptor->stopbits;
	dcbSP.Parity   = descriptor->parity;

	if (!SetCommState((HANDLE)hSerial,&dcbSP))
	{  // analyse error
		return -1;
		}

	COMMTIMEOUTS timeouts={0};
	timeouts.ReadIntervalTimeout=50;
	timeouts.ReadTotalTimeoutConstant=50;
	timeouts.ReadTotalTimeoutMultiplier=10;
	timeouts.WriteTotalTimeoutConstant=50;
	timeouts.WriteTotalTimeoutMultiplier=10;

	if(!SetCommTimeouts(hSerial, &timeouts))
	{
       return -1;
     }

    uart6.hSerial = hSerial;
    uart6.InitComplete=1;
  }

#endif

#ifdef __LINX__

if(0x03 == descriptor->chanell)
{
  uart3.pRxBuff=(uint8_t *)descriptor->rxBuff;
  uart3.RxLen=descriptor->rxBuffSize;
  uart3.TxCallback=descriptor->txCallback;
  uart3.RxCallback=descriptor->rxCallback;
  uart3.status=Free;
  uart3.RxFirst=0;
  uart3.RxLast=0;
  uart3.hSerial = -1;

  int hSerial = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
  if (hSerial == -1)
  {
	  return -1;
  	  }
	//CONFIGURE THE UART
	//The flags (defined in termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)

  struct termios options;
  tcgetattr(hSerial, &options);
  cfsetispeed(&options, (uint32_t)descriptor->baudrate );
  cfsetospeed(&options, (uint32_t)descriptor->baudrate );
  options.c_cflag = (uint32_t)descriptor->baudrate |
                     descriptor->dataLength |
		     CLOCAL |
		     CREAD;
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_iflag =  IGNPAR |
                   ICRNL;
  //options.c_iflag |= (INPCK | ISTRIP );
  //options.c_iflag &= ~(IXON | IXOFF | IXANY);

  options.c_oflag &= ~( OPOST );
  tcflush(hSerial, TCIFLUSH);
  tcsetattr(hSerial, TCSAFLUSH, &options);

  uart3.hSerial = hSerial;
  uart3.InitComplete=1;
}

if(0x06 == descriptor->chanell)
{
  uart6.pRxBuff=(uint8_t *)descriptor->rxBuff;
  uart6.RxLen=descriptor->rxBuffSize;
  uart6.TxCallback=descriptor->txCallback;
  uart6.RxCallback=descriptor->rxCallback;
  uart6.status=Free;
  uart6.RxFirst=0;
  uart6.RxLast=0;
  uart6.hSerial = -1;

  int hSerial = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
  if (hSerial == -1)
  {
	  return -1;
  	  }
	//CONFIGURE THE UART
	//The flags (defined in termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)

  struct termios options;
  tcgetattr(hSerial, &options);
  cfsetispeed(&options, (uint32_t)descriptor->baudrate );
  cfsetospeed(&options, (uint32_t)descriptor->baudrate );
  options.c_cflag = (uint32_t)descriptor->baudrate |
                    descriptor->dataLength |
		    CLOCAL |
		    CREAD;

  options.c_lflag &= ~(ICANON |
                       ECHO |
		       ECHOE |
		       ISIG );

  options.c_iflag = IGNPAR |
                     ICRNL;

  options.c_oflag = 0;
  tcflush(hSerial, TCIFLUSH);
  tcsetattr(hSerial, TCSANOW, //TCSAFLUSH,//TCSANOW,
                     &options);

  uart6.hSerial = hSerial;
  uart6.InitComplete=1;
}
#endif

return 0;
}

//***Ïðåðûâàíèå DMA1_Stream3 (ïî çàâåðøåíèþ ïåðåäà÷è)
void DMA1_Stream3_IRQHandler(void)
{
#ifdef __STM32__
DMA_ClearITPendingBit(DMA1_Stream3,DMA_IT_TC);
DMA_ITConfig(DMA1_Stream3,DMA_IT_TC,DISABLE);
while(RESET==USART_GetFlagStatus(USART3,USART_FLAG_TC)){};
#endif

(*uart3.TxCallback)();
uart3.status=Free;
}


void DMA1_Stream6_IRQHandler(void)
{
#ifdef __STM32__
DMA_ClearITPendingBit(DMA1_Stream6,DMA_IT_TC);
DMA_ITConfig(DMA1_Stream6,DMA_IT_TC,DISABLE);
while(RESET==USART_GetFlagStatus(USART2,USART_FLAG_TC)){};
#endif
(*uart2.TxCallback)();
uart2.status=Free;
}


void DMA2_Stream7_IRQHandler(void)
{
#ifdef __STM32__
DMA_ClearITPendingBit(DMA2_Stream7,DMA_IT_TC);
DMA_ITConfig(DMA2_Stream7,DMA_IT_TC,DISABLE);
while(RESET==USART_GetFlagStatus(USART1,USART_FLAG_TC)){};
#endif

(*uart1.TxCallback)();
uart1.status=Free;
}


void DMA2_Stream6_IRQHandler(void)
{
asm("nop");
#ifdef __STM32__
DMA_ClearITPendingBit(DMA2_Stream6,DMA_IT_TC);
DMA_ITConfig(DMA2_Stream6,DMA_IT_TC,DISABLE);
while(RESET==USART_GetFlagStatus(USART6,USART_FLAG_TC)){};
#endif

(*uart6.TxCallback)();
uart6.status=Free;
}


void USART3_IRQHandler(void)
{
#ifdef __STM32__
static uint8_t *p;
uint8_t buff;
uint8_t ExpectedInt = 0;//Ïðèçíàê òîãî, ÷òî ïðåðûâàíèå áûëî îáðàáîòàíî

//Ïðåðûâàíèå áûëî âûçâàíî ïðèåìîì
if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
  USART_ClearITPendingBit(USART3, USART_IT_RXNE);
  buff=USART_ReceiveData(USART3);

  if(!uart3.RxLast++)
    p=uart3.pRxBuff;
  *p++=buff;

  if(uart3.RxLast==uart3.RxLen)
    uart3.RxLast=0;
  ExpectedInt=1;
  }

//Ïðåðûâàíèå áûëî âûçâàíî çàâåðøåíèåì çàãðóçêè áàéòà â ïåðåäàþùèé áóôåð
if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
  {
  USART_ClearITPendingBit(USART3, USART_IT_TXE);
  USART_SendData(USART3,*uart3.pTxBuff++);
  if(uart3.TxLen)
    {
    uart3.TxLen--;
    }
  else
    {
    USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART3, USART_IT_TC, ENABLE);
    uart3.status=Free;
    }
  ExpectedInt=1;
  }
//Ïðåðûâàíèå áûëî âûçâàíî çàâåðøåíèåì îòïðàâêè áàéòà
if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)
  {
  USART_ClearITPendingBit(USART3, USART_IT_TC);
  USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
  (*uart3.TxCallback)(); //Âûçîâ ïîäïðîãðàììû, óêàçûâàþùåé íà çàâåðøåííîñòü ïåðåäà÷è
  ExpectedInt=1;
  }

if (0==ExpectedInt)//Åñëè äëÿ ïðåðûâàíèÿ íå îïèñàíî äåéñòâèé (íå Tx, íå Rx, íå Tc)
  USART_ReceiveData(USART3);
#endif

#ifdef __WINX__

/*
static uint8_t *p;
uint8_t buff;
uint8_t ExpectedInt = 0;

if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
  USART_ClearITPendingBit(USART3, USART_IT_RXNE);
  buff=USART_ReceiveData(USART3);

  if(!uart3.RxLast++)
    p=uart3.pRxBuff;
  *p++=buff;

  if(uart3.RxLast==uart3.RxLen)
    uart3.RxLast=0;
  ExpectedInt=1;
  }

//Ïðåðûâàíèå áûëî âûçâàíî çàâåðøåíèåì çàãðóçêè áàéòà â ïåðåäàþùèé áóôåð
if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
  {
  USART_ClearITPendingBit(USART3, USART_IT_TXE);
  USART_SendData(USART3,*uart3.pTxBuff++);
  if(uart3.TxLen)
    {
    uart3.TxLen--;
    }
  else
    {
    USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART3, USART_IT_TC, ENABLE);
    uart3.status=Free;
    }
  ExpectedInt=1;
  }
//Ïðåðûâàíèå áûëî âûçâàíî çàâåðøåíèåì îòïðàâêè áàéòà
if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)
  {
  USART_ClearITPendingBit(USART3, USART_IT_TC);
  USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
  (*uart3.TxCallback)(); //Âûçîâ ïîäïðîãðàììû, óêàçûâàþùåé íà çàâåðøåííîñòü ïåðåäà÷è
  ExpectedInt=1;
  }

if (0==ExpectedInt)//Åñëè äëÿ ïðåðûâàíèÿ íå îïèñàíî äåéñòâèé (íå Tx, íå Rx, íå Tc)
  USART_ReceiveData(USART3);
*/

  static uint8_t *p;
  uint8_t buff;
  int32_t cnt;

  if ( uart3.hSerial != -1 )
  {
	// write data
	if (uart3.TxLen)
	 {
      //////
#ifdef _DEBUG_UART_
      printf("\n-> ");
	  uint8_t aa = 0;
	  for (aa = 0; aa < uart3.TxLen; aa++)
        { printf("%02X ", ((uint8_t*)uart3.pTxBuff)[aa] ); }
#endif
	  ///////
	  if( WriteFile((HANDLE)uart3.hSerial, uart3.pTxBuff, uart3.TxLen, &cnt, 0))
	  {
	   uart3.TxLen -= cnt;
	   if (!uart3.TxLen)
	   {
	     uart3.status=Free;
		 (*uart3.TxCallback)();
	    }
	   }
      }

    // read data
	do{
		ReadFile((HANDLE)uart3.hSerial, &buff, 1, (PDWORD)&cnt, NULL );

		if ( cnt > 0 )
		{
			if(!uart3.RxLast++)
				p=uart3.pRxBuff;
			*p++=buff;

			if(uart3.RxLast >= uart3.RxLen)
				uart3.RxLast=0;
		  }
		}while( cnt > 0 );
  }
#endif

#ifdef __LINX__
static uint8_t *p;
  uint8_t buff;
  int32_t cnt;

  if ( uart3.hSerial != -1 )
  {
	// write data
	if (uart3.TxLen)
	 {

#ifdef _DEBUG_UART_
      //////
      printf("\n-> ");
	  uint8_t aa = 0;
	  for (aa = 0; aa < uart3.TxLen; aa++)
        { printf("%02X ", ((uint8_t*)uart3.pTxBuff)[aa] ); }
	  ///////
#endif

	  if((cnt = write( uart3.hSerial, uart3.pTxBuff, uart3.TxLen)) > 0 )
	  {
	   uart3.TxLen -= cnt;
	   if (!uart3.TxLen)
	   {
	     uart3.status=Free;
		 (*uart3.TxCallback)();
	    }
	   }
      }

    // read data
	do{
		cnt = read( uart3.hSerial, &buff, 1 );

		if ( cnt > 0 )
		{
			if(!uart3.RxLast++)
				p=uart3.pRxBuff;
			*p++=buff;

			if(uart3.RxLast >= uart3.RxLen)
				uart3.RxLast=0;
		  }
		}while( cnt > 0);
  }
#endif
}

void USART1_IRQHandler(void)
{
#ifdef __STM32__
static uint8_t *p;
uint8_t buff;
uint8_t ExpectedInt = 0;//Ïðèçíàê òîãî, ÷òî ïðåðûâàíèå áûëî îáðàáîòàíî

//Ïðåðûâàíèå áûëî âûçâàíî ïðèåìîì
if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
  USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  buff=USART_ReceiveData(USART1);

  if(!uart1.RxLast++)
    p=uart1.pRxBuff;
  *p++=buff;

  if(uart1.RxLast==uart1.RxLen)
    uart1.RxLast=0;
  ExpectedInt=1;
  }

//Ïðåðûâàíèå áûëî âûçâàíî çàâåðøåíèåì çàãðóçêè áàéòà â ïåðåäàþùèé áóôåð
if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {
  USART_ClearITPendingBit(USART1, USART_IT_TXE);
  USART_SendData(USART1,*uart1.pTxBuff++);
  if(uart1.TxLen)
    {
    uart1.TxLen--;
    }
  else
    {
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    uart1.status=Free;
    }
  ExpectedInt=1;
  }
//Ïðåðûâàíèå áûëî âûçâàíî çàâåðøåíèåì îòïðàâêè áàéòà
if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)
  {
  USART_ClearITPendingBit(USART1, USART_IT_TC);
  USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
  (*uart1.TxCallback)(); //Âûçîâ ïîäïðîãðàììû, óêàçûâàþùåé íà çàâåðøåííîñòü ïåðåäà÷è
  ExpectedInt=1;
  }

if (0==ExpectedInt)//Åñëè äëÿ ïðåðûâàíèÿ íå îïèñàíî äåéñòâèé (íå Tx, íå Rx, íå Tc)
  USART_ReceiveData(USART1);
#endif
}



void USART6_IRQHandler(void)
{
#ifdef __STM32__
static uint8_t *p;
uint8_t buff;
uint8_t ExpectedInt = 0;//Ïðèçíàê òîãî, ÷òî ïðåðûâàíèå áûëî îáðàáîòàíî

//Ïðåðûâàíèå áûëî âûçâàíî ïðèåìîì
if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
  {
  USART_ClearITPendingBit(USART6, USART_IT_RXNE);
  buff=USART_ReceiveData(USART6);

  if(!uart6.RxLast++)
    p=uart6.pRxBuff;
  *p++=buff;

  if(uart6.RxLast==uart6.RxLen)
    uart6.RxLast=0;
  ExpectedInt=1;
  }

//Ïðåðûâàíèå áûëî âûçâàíî çàâåðøåíèåì çàãðóçêè áàéòà â ïåðåäàþùèé áóôåð
if(USART_GetITStatus(USART6, USART_IT_TXE) != RESET)
  {
  USART_ClearITPendingBit(USART6, USART_IT_TXE);
  USART_SendData(USART6,*uart6.pTxBuff++);
  if(uart6.TxLen)
    {
    uart6.TxLen--;
    }
  else
    {
    USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART6, USART_IT_TC, ENABLE);
    uart6.status=Free;
    }
  ExpectedInt=1;
  }
//Ïðåðûâàíèå áûëî âûçâàíî çàâåðøåíèåì îòïðàâêè áàéòà
if(USART_GetITStatus(USART6, USART_IT_TC) != RESET)
  {
  USART_ClearITPendingBit(USART6, USART_IT_TC);
  USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
  (*uart6.TxCallback)(); //Âûçîâ ïîäïðîãðàììû, óêàçûâàþùåé íà çàâåðøåííîñòü ïåðåäà÷è
  ExpectedInt=1;
  }

if (0==ExpectedInt)//Åñëè äëÿ ïðåðûâàíèÿ íå îïèñàíî äåéñòâèé (íå Tx, íå Rx, íå Tc)
  USART_ReceiveData(USART6);
#endif

#ifdef __WINX__
  static uint8_t *p;
  uint8_t buff;
  uint32_t cnt;

  if ( uart6.hSerial != -1 )
  {
	// write data
	if (uart6.TxLen)
	 {
      //////
#ifdef _DEBUG_UART_
      printf("\n-> ");
	  uint8_t aa = 0;
	  for (aa = 0; aa < uart6.TxLen; aa++)
        { printf("%02X ", ((uint8_t*)uart6.pTxBuff)[aa] ); }
#endif
	  ///////
	  if( WriteFile((HANDLE)uart6.hSerial, uart6.pTxBuff, uart6.TxLen, &cnt, 0))
	  {
	   uart6.TxLen -= cnt;
	   if (!uart6.TxLen)
	   {
	     uart6.status=Free;
		 (*uart6.TxCallback)();
	    }
	   }
      }

    // read data
	do{
		ReadFile((HANDLE)uart6.hSerial, &buff, 1, (PDWORD)&cnt, NULL );

		if ( cnt )
		{
			if(!uart6.RxLast++)
				p=uart6.pRxBuff;
			*p++=buff;

			if(uart6.RxLast >= uart6.RxLen)
				uart6.RxLast=0;
		  }
		}while(cnt);
  }
#endif

#ifdef __LINX__
 static uint8_t *p;
  uint8_t buff;
  int32_t cnt;

  if ( uart6.hSerial != -1 )
  {
	// write data
	if (uart6.TxLen)
	 {

#ifdef _DEBUG_UART_
      //////
      printf("\n-> ");
	  uint8_t aa = 0;
	  for (aa = 0; aa < uart6.TxLen; aa++)
        { printf("%02X ", ((uint8_t*)uart6.pTxBuff)[aa] ); }
	  ///////
#endif

	  if((cnt = write( uart6.hSerial, uart6.pTxBuff, uart6.TxLen)) > 0 )
	  {
	   uart6.TxLen -= cnt;
	   if (!uart6.TxLen)
	   {
	     uart6.status=Free;
		 (*uart6.TxCallback)();
	    }
	   }
      }

    // read data
	do{
		cnt = read( uart6.hSerial, &buff, 1 );

		if ( cnt > 0 )
		{
			if(!uart6.RxLast++)
				p=uart6.pRxBuff;
			*p++=buff;

			if(uart6.RxLast >= uart6.RxLen)
				uart6.RxLast=0;
		  }
		}while( cnt > 0);
  }
#endif
}

void USART2_IRQHandler(void)
{
#ifdef __STM32__
static uint8_t *p;
uint8_t buff;
uint8_t ExpectedInt = 0;//Ïðèçíàê òîãî, ÷òî ïðåðûâàíèå áûëî îáðàáîòàíî

//Ïðåðûâàíèå áûëî âûçâàíî ïðèåìîì
if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
  USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  buff=USART_ReceiveData(USART2);

  if(!uart2.RxLast++)
    p=uart2.pRxBuff;
  *p++=buff;

  if(uart2.RxLast==uart2.RxLen)
    uart2.RxLast=0;
  ExpectedInt=1;
  }

//Ïðåðûâàíèå áûëî âûçâàíî çàâåðøåíèåì çàãðóçêè áàéòà â ïåðåäàþùèé áóôåð
if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
  {
  USART_ClearITPendingBit(USART2, USART_IT_TXE);
  USART_SendData(USART2,*uart2.pTxBuff++);
  if(uart2.TxLen)
    {
    uart2.TxLen--;
    }
  else
    {
    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART2, USART_IT_TC, ENABLE);
    uart2.status=Free;
    }
  ExpectedInt=1;
  }
//Ïðåðûâàíèå áûëî âûçâàíî çàâåðøåíèåì îòïðàâêè áàéòà
if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)
  {
  USART_ClearITPendingBit(USART2, USART_IT_TC);
  USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
  (*uart2.TxCallback)(); //Âûçîâ ïîäïðîãðàììû, óêàçûâàþùåé íà çàâåðøåííîñòü ïåðåäà÷è
  ExpectedInt=1;
  }

if (0==ExpectedInt)//Åñëè äëÿ ïðåðûâàíèÿ íå îïèñàíî äåéñòâèé (íå Tx, íå Rx, íå Tc)
  USART_ReceiveData(USART2);
#endif

}



void UartDriverRun(void)
{
#ifdef __STM32__
uint16_t RxLen_=0;
uint16_t RxFirst_=0;
uint16_t RxLast_=0;


if(uart3.InitComplete)//Åñëè 3 UART ïðîèíèöèàëèçèðîâàí
  {
  if(ENABLE_DMA_RX_CHANELL&uart3.HardwareAccelerator)
    {
    uart3.DmaCnt = DMA_GetCurrDataCounter(DMA1_Stream1);
    RxLast_ = uart3.RxLen - uart3.DmaCnt;//Ïðè ïåðâîì çàïóñêå âîçâðàùàåò ðàçìåð áóôåðà
    if(RxLast_==uart3.RxLen)
       RxLast_=0;
    }
  else
    RxLast_  = uart3.RxLast;

  RxFirst_ = uart3.RxFirst;

  if(RxFirst_ != RxLast_)//Àíàëèç âõîäíîãî ïîòîêà áàéò
    {
    if(RxLast_>RxFirst_)//Íîðìàëüíûé ðåæèì
      {
      uart3.RxFirst = RxLast_;
      RxLen_=RxLast_ - RxFirst_;
      (*uart3.RxCallback)(uart3.pRxBuff+RxFirst_,RxLen_);
      }
    else
      {
      uart3.RxFirst = 0;
      RxLen_=uart3.RxLen - RxFirst_;
      (*uart3.RxCallback)(uart3.pRxBuff+RxFirst_,RxLen_);
      }
    }
  }



if(uart1.InitComplete)//Åñëè 1 UART ïðîèíèöèàëèçèðîâàí
  {
  if(ENABLE_DMA_RX_CHANELL&uart1.HardwareAccelerator)
    {
    uart1.DmaCnt = DMA_GetCurrDataCounter(DMA2_Stream2);
    RxLast_ = uart1.RxLen - uart1.DmaCnt;//Ïðè ïåðâîì çàïóñêå âîçâðàùàåò ðàçìåð áóôåðà
    if(RxLast_==uart1.RxLen)
       RxLast_=0;
    }
  else
    RxLast_  = uart1.RxLast;

  RxFirst_ = uart1.RxFirst;

  if(RxFirst_ != RxLast_)//Àíàëèç âõîäíîãî ïîòîêà áàéò
    {
    if(RxLast_>RxFirst_)//Íîðìàëüíûé ðåæèì
      {
      uart1.RxFirst = RxLast_;
      RxLen_=RxLast_ - RxFirst_;
      (*uart1.RxCallback)(uart1.pRxBuff+RxFirst_,RxLen_);
      }
    else
      {
      uart1.RxFirst = 0;
      RxLen_=uart1.RxLen - RxFirst_;
      (*uart1.RxCallback)(uart1.pRxBuff+RxFirst_,RxLen_);
      }
    }
  }




if(uart2.InitComplete)//Åñëè 1 UART ïðîèíèöèàëèçèðîâàí
  {
  if(ENABLE_DMA_RX_CHANELL&uart2.HardwareAccelerator)
    {
    uart2.DmaCnt = DMA_GetCurrDataCounter(DMA1_Stream5);
    RxLast_ = uart2.RxLen - uart2.DmaCnt;//Ïðè ïåðâîì çàïóñêå âîçâðàùàåò ðàçìåð áóôåðà
    if(RxLast_==uart2.RxLen)
       RxLast_=0;
    }
  else
    RxLast_  = uart2.RxLast;

  RxFirst_ = uart2.RxFirst;

  if(RxFirst_ != RxLast_)//Àíàëèç âõîäíîãî ïîòîêà áàéò
    {
    if(RxLast_>RxFirst_)//Íîðìàëüíûé ðåæèì
      {
      uart2.RxFirst = RxLast_;
      RxLen_=RxLast_ - RxFirst_;
      (*uart2.RxCallback)(uart2.pRxBuff+RxFirst_,RxLen_);
      }
    else
      {
      uart2.RxFirst = 0;
      RxLen_=uart2.RxLen - RxFirst_;
      (*uart2.RxCallback)(uart2.pRxBuff+RxFirst_,RxLen_);
      }
    }
  }


if(uart6.InitComplete)//Åñëè 6 UART ïðîèíèöèàëèçèðîâàí
  {
  if(ENABLE_DMA_RX_CHANELL&uart6.HardwareAccelerator)
    {
    uart6.DmaCnt = DMA_GetCurrDataCounter(DMA2_Stream1);
    RxLast_ = uart6.RxLen - uart6.DmaCnt;//Ïðè ïåðâîì çàïóñêå âîçâðàùàåò ðàçìåð áóôåðà
    if(RxLast_==uart6.RxLen)
       RxLast_=0;
    }
  else
    RxLast_  = uart6.RxLast;

  RxFirst_ = uart6.RxFirst;

  if(RxFirst_ != RxLast_)//Àíàëèç âõîäíîãî ïîòîêà áàéò
    {
    if(RxLast_>RxFirst_)//Íîðìàëüíûé ðåæèì
      {
      uart6.RxFirst = RxLast_;
      RxLen_=RxLast_ - RxFirst_;
      (*uart6.RxCallback)(uart6.pRxBuff+RxFirst_,RxLen_);
      }
    else
      {
      uart6.RxFirst = 0;
      RxLen_=uart6.RxLen - RxFirst_;
      (*uart6.RxCallback)(uart6.pRxBuff+RxFirst_,RxLen_);
      }
    }
  }

#endif

#ifdef __WINX__
uint16_t RxLen_=0;
uint16_t RxFirst_=0;
uint16_t RxLast_=0;

if( uart3.InitComplete )
  {
  RxLast_  = uart3.RxLast;
  RxFirst_ = uart3.RxFirst;

  if(RxFirst_ != RxLast_)
    {
    if(RxLast_>RxFirst_)//Íîðìàëüíûé ðåæèì
      {
      uart3.RxFirst = RxLast_;
      RxLen_=RxLast_ - RxFirst_;
      (*uart3.RxCallback)(uart3.pRxBuff+RxFirst_,RxLen_);
      }
    else
      {
      uart3.RxFirst = 0;
      RxLen_=uart3.RxLen - RxFirst_;
      (*uart3.RxCallback)(uart3.pRxBuff+RxFirst_,RxLen_);
      }
    }
  }

if(uart6.InitComplete)//Åñëè 6 UART ïðîèíèöèàëèçèðîâàí
  {
   RxLast_  = uart6.RxLast;
   RxFirst_ = uart6.RxFirst;

  if(RxFirst_ != RxLast_)//Àíàëèç âõîäíîãî ïîòîêà áàéò
    {
    if(RxLast_>RxFirst_)//Íîðìàëüíûé ðåæèì
      {
      uart6.RxFirst = RxLast_;
      RxLen_=RxLast_ - RxFirst_;
      (*uart6.RxCallback)(uart6.pRxBuff+RxFirst_,RxLen_);
      }
    else
      {
      uart6.RxFirst = 0;
      RxLen_=uart6.RxLen - RxFirst_;
      (*uart6.RxCallback)(uart6.pRxBuff+RxFirst_,RxLen_);
      }
    }
  }
#endif

#ifdef __LINX__
uint16_t RxLen_=0;
uint16_t RxFirst_=0;
uint16_t RxLast_=0;

if( uart3.InitComplete )
  {
  RxLast_  = uart3.RxLast;
  RxFirst_ = uart3.RxFirst;

  if(RxFirst_ != RxLast_)
    {
    if(RxLast_>RxFirst_)
      {
      uart3.RxFirst = RxLast_;
      RxLen_=RxLast_ - RxFirst_;
      (*uart3.RxCallback)(uart3.pRxBuff+RxFirst_,RxLen_);
      }
    else
      {
      uart3.RxFirst = 0;
      RxLen_=uart3.RxLen - RxFirst_;
      (*uart3.RxCallback)(uart3.pRxBuff+RxFirst_,RxLen_);
      }
    }
  }

if(uart6.InitComplete)//Åñëè 6 UART ïðîèíèöèàëèçèðîâàí
  {
   RxLast_  = uart6.RxLast;
   RxFirst_ = uart6.RxFirst;

  if(RxFirst_ != RxLast_)//Àíàëèç âõîäíîãî ïîòîêà áàéò
    {
    if(RxLast_>RxFirst_)//Íîðìàëüíûé ðåæèì
      {
      uart6.RxFirst = RxLast_;
      RxLen_=RxLast_ - RxFirst_;
      (*uart6.RxCallback)(uart6.pRxBuff+RxFirst_,RxLen_);
      }
    else
      {
      uart6.RxFirst = 0;
      RxLen_=uart6.RxLen - RxFirst_;
      (*uart6.RxCallback)(uart6.pRxBuff+RxFirst_,RxLen_);
      }
    }
  }
#endif
}


void WriteUart(UartDescriptor_t *uartDescriptor,uint8_t *p,uint16_t len)
{

#ifdef __STM32__
DMA_InitTypeDef  DMA_InitStructure;

if(USART3==uartDescriptor->chanell)
  {
  if(Free==uart3.status)
      {
      uart3.pTxBuff=p;
      uart3.TxLen=len-1;
      uart3.status=Busy;
      }

  if(ENABLE_DMA_TX_CHANELL&uart3.HardwareAccelerator)
    {
   //Äëÿ îïðåäåëåíèÿ êàíàëîâ è ïîòîêîâ DMA ñì. RM0033 ñòð. 172
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Stream3);

    DMA_InitStructure.DMA_Channel            = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t )p;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize         = len;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;//DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE){};
    DMA_Init(DMA1_Stream3, &DMA_InitStructure);
    DMA_Cmd(DMA1_Stream3, ENABLE);
    USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);
    DMA_ITConfig(DMA1_Stream3,DMA_IT_TC,ENABLE);
    }
  else
    {
    USART_ITConfig(uartDescriptor->chanell,USART_IT_TXE,ENABLE);
    }
  }


if(USART2==uartDescriptor->chanell)
  {
  if(Free==uart2.status)
      {
      uart2.pTxBuff=p;
      uart2.TxLen=len-1;
      uart2.status=Busy;
      }

  if(ENABLE_DMA_TX_CHANELL&uart2.HardwareAccelerator)
    {
   //Äëÿ îïðåäåëåíèÿ êàíàëîâ è ïîòîêîâ DMA ñì. RM0033 ñòð. 172
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Stream6);

    DMA_InitStructure.DMA_Channel            = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t )p;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize         = len;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;//DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    while (DMA_GetCmdStatus(DMA1_Stream6) != DISABLE){};
    DMA_Init(DMA1_Stream6, &DMA_InitStructure);
    DMA_Cmd(DMA1_Stream6, ENABLE);
    USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
    DMA_ITConfig(DMA1_Stream6,DMA_IT_TC,ENABLE);
    }
  else
    {
    USART_ITConfig(uartDescriptor->chanell,USART_IT_TXE,ENABLE);
    }
  }


if(USART1==uartDescriptor->chanell)
  {
  if(Free==uart1.status)
      {
      uart1.pTxBuff=p;
      uart1.TxLen=len-1;
      uart1.status=Busy;
      }

  if(ENABLE_DMA_TX_CHANELL&uart1.HardwareAccelerator)
    {
   //Äëÿ îïðåäåëåíèÿ êàíàëîâ è ïîòîêîâ DMA ñì. RM0033 ñòð. 172
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    DMA_DeInit(DMA2_Stream7);

    DMA_InitStructure.DMA_Channel            = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t )p;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize         = len;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;//DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE){};
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream7, ENABLE);
    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
    DMA_ITConfig(DMA2_Stream7,DMA_IT_TC,ENABLE);
    }
  else
    {
    USART_ITConfig(uartDescriptor->chanell,USART_IT_TXE,ENABLE);
    }
  }


if(USART6==uartDescriptor->chanell)
  {
  if(Free==uart6.status)
      {
      uart6.pTxBuff=p;
      uart6.TxLen=len-1;
      uart6.status=Busy;
      }

  if(ENABLE_DMA_TX_CHANELL&uart6.HardwareAccelerator)
    {
   //Äëÿ îïðåäåëåíèÿ êàíàëîâ è ïîòîêîâ DMA ñì. RM0033 ñòð. 172
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    DMA_DeInit(DMA2_Stream6);

    DMA_InitStructure.DMA_Channel            = DMA_Channel_5;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART6->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t )p;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize         = len;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;//DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE){};
    DMA_Init(DMA2_Stream6, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream6, ENABLE);
    USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);
    DMA_ITConfig(DMA2_Stream6,DMA_IT_TC,ENABLE);
    }
  else
    {
    USART_ITConfig(uartDescriptor->chanell,USART_IT_TXE,ENABLE);
    }
  }
#endif

#ifdef __WINX__

if(0x03==uartDescriptor->chanell)
  {
  if(Free==uart3.status)
      {
      uart3.pTxBuff=p;
      uart3.TxLen=len; // -1
      uart3.status=Busy;
      }
  }

if(0x06==uartDescriptor->chanell)
  {
  if(Free==uart6.status)
      {
      uart6.pTxBuff=p;
      uart6.TxLen=len; // -1
      uart6.status=Busy;
      }
  }
#endif

#ifdef __LINX__

if(0x03==uartDescriptor->chanell)
  {
  if(Free==uart3.status)
      {
      uart3.pTxBuff=p;
      uart3.TxLen=len; // -1
      uart3.status=Busy;
      }
  }

if(0x06==uartDescriptor->chanell)
  {
  if(Free==uart6.status)
      {
      uart6.pTxBuff=p;
      uart6.TxLen=len; // -1
      uart6.status=Busy;
      }
  }
#endif
}

void CloseUart(UartDescriptor_t * uartDescriptor)
{
#ifdef __WINX__
 if (0x03 == uartDescriptor->chanell)
  {
	  if (uart3.hSerial != -1)
	  {
		  CloseHandle((HANDLE)uart3.hSerial);
	  }
  }

  if (0x06 == uartDescriptor->chanell)
  {
	  if (uart6.hSerial != -1)
	  {
		  CloseHandle((HANDLE)uart6.hSerial);
	  }
  }
#endif

#ifdef __LINX__
 if (0x03 == uartDescriptor->chanell)
  {
	  if (uart3.hSerial != -1)
	  {
		  close(uart3.hSerial);
	  }
  }

  if (0x06 == uartDescriptor->chanell)
  {
	  if (uart6.hSerial != -1)
	  {
		  close(uart6.hSerial);
	  }
  }
#endif
}

