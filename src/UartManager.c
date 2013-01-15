#include "../inc/includes.h"

//#include "UartManager.h"
//#include "UartDriver.h"
//#include "MTbus.h"
//#include "MTBusManager.h"
//#include "MTBusManager.h"
//#include "TBusManager.h"




//assign_pin(DE1,B,2);
//assign_pin(DE6,C,9);

PortStatus_t SlavePortSatus; //Ñòàòóñ ïîðòà âåäîìîãî (ñâîáîäåí/çàíÿò)


static UartDescriptor_t uart3Descriptor;
static UartDescriptor_t uart1Descriptor;
static UartDescriptor_t uart6Descriptor;

static uint8_t RxBuff1[512]; //Âíóòðåííèé áóôåð Uart 1
static uint8_t RxBuff3[2048]; //Âíóòðåííèé áóôåð Uart 3
static uint8_t RxBuff6[512]; //Âíóòðåííèé áóôåð Uart 6



static void UART1_RxEvent(uint8_t *pData, uint8_t len);
static void UART1_TxConfirm(void);

static void UART3_RxEvent(uint8_t *pData, uint8_t len);
static void UART3_TxConfirm(void);


static void UART6_RxEvent(uint8_t *pData, uint8_t len);
static void UART6_TxConfirm(void);

uint8_t uart_speed_index = 0x00; // 9600bps defaults


void StartUart3Manager(void)
{
#ifdef __STM32__
 uart3Descriptor.chanell              = USART3;
#endif
#ifdef __LINX__
 uart3Descriptor.chanell              = 0x03;
#endif
#ifdef __WINX__
 uart3Descriptor.chanell              = 0x03;
#endif

 uart3Descriptor.baudrate             = USART_BAUDRATE_38400;
 uart3Descriptor.dataLength           = UART_DATA8;
 uart3Descriptor.stopbits             = UART_STOPBIT_1;
 uart3Descriptor.parity               = UART_PARITY_NONE;
 uart3Descriptor.flowControl          = UART_FLOW_CONTROL_NONE;
 uart3Descriptor.HardwareAccelerator  =  DISABLE_DMA_RX_CHANELL |
                                          DISABLE_DMA_TX_CHANELL;
 uart3Descriptor.rxBuff               = RxBuff3;
 uart3Descriptor.rxBuffSize           = sizeof(RxBuff3);
 uart3Descriptor.rxCallback           = UART3_RxEvent;
 uart3Descriptor.txCallback           = UART3_TxConfirm;
 OpenUart(&uart3Descriptor);
}

void StartUart1Manager(void)
{
#ifdef __STM32__
 uart1Descriptor.chanell              = USART1;
#endif
#ifdef __LINX__
 uart1Descriptor.chanell              = 0x06;
#endif
#ifdef __WINX__
 uart1Descriptor.chanell              = 0x06;
#endif
 /*uart1Descriptor.baudrate             = (uart_speed_index == 0)?USART_BAUDRATE_9600:
                                          (uart_speed_index == 1)?USART_BAUDRATE_14400:
                                           (uart_speed_index == 2)?USART_BAUDRATE_19200:
                                            (uart_speed_index == 3)?USART_BAUDRATE_38400:
                                             (uart_speed_index == 4)?USART_BAUDRATE_56000:
                                              (uart_speed_index == 5)?USART_BAUDRATE_57600:
                                               (uart_speed_index == 6)?USART_BAUDRATE_115200:
                                                (uart_speed_index == 7)?USART_BAUDRATE_128000:
                                                               USART_BAUDRATE_9600;
	*/
 uart1Descriptor.baudrate             = USART_BAUDRATE_9600;

 uart1Descriptor.dataLength           = UART_DATA8;
 uart1Descriptor.stopbits             = UART_STOPBIT_1;
 uart1Descriptor.parity               = UART_PARITY_NONE;
 uart1Descriptor.flowControl          = UART_FLOW_CONTROL_NONE;
 uart1Descriptor.HardwareAccelerator  = DISABLE_DMA_RX_CHANELL |
                                         DISABLE_DMA_TX_CHANELL;

 uart1Descriptor.rxBuff               = RxBuff1;
 uart1Descriptor.rxBuffSize           = sizeof(RxBuff1);
 uart1Descriptor.rxCallback           = UART1_RxEvent;
 uart1Descriptor.txCallback           = UART1_TxConfirm;

#ifdef __STM32__
 //DE1_make_out_pp();
 //DE1_clr();
 GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_WriteBit(GPIOB,GPIO_Pin_2,Bit_RESET);
#endif

 OpenUart(&uart1Descriptor);
}


void StartUart6Manager(void)
{
#ifdef __STM32__
 uart6Descriptor.chanell              = USART6;
#endif
#ifdef __LINX__
 uart6Descriptor.chanell              = 0x06;
#endif
#ifdef __WINX__
 uart6Descriptor.chanell              = 0x06;
#endif

 uart6Descriptor.baudrate             = USART_BAUDRATE_9600;
 uart6Descriptor.dataLength           = UART_DATA8;
 uart6Descriptor.stopbits             = UART_STOPBIT_1;
 uart6Descriptor.parity               = UART_PARITY_NONE;
 uart6Descriptor.flowControl          = UART_FLOW_CONTROL_NONE;
 uart6Descriptor.HardwareAccelerator  = DISABLE_DMA_RX_CHANELL | DISABLE_DMA_TX_CHANELL;

 uart6Descriptor.rxBuff               = RxBuff6;
 uart6Descriptor.rxBuffSize           = sizeof(RxBuff6);
 uart6Descriptor.rxCallback           = UART6_RxEvent;
 uart6Descriptor.txCallback           = UART6_TxConfirm;

#ifdef __STM32__
 //DE6_make_out_pp();
// DE6_clr();

 RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;

   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_RESET);
#endif

 OpenUart(&uart6Descriptor);
}




static void UART3_RxEvent(uint8_t *pData, uint8_t len)
{

#ifdef _DEBUG_UART_
	 printf("\n<- ");
	  uint8_t aa = 0;
	  for (aa = 0; aa < len; aa++)
        {
			if ((aa%16) == 0) printf("\n");
			printf("%02X ", ((uint8_t*)pData)[aa] );
         }
#endif

//for(uint8_t i=0;i!=len;i++)
//  Rx[ii++]=*pData++;
//CDReceive(*pData++);
MTBusManagerDataReceiver(&ServerB,pData,len);
//MTBusManagerDataReceiver(&CoordSniffer,pData,len);
// asm("nop");

//  TBusDataReceiver(*pData++);
}

static void UART3_TxConfirm(void)
{
asm("nop");
// Rx_SW;
 SlavePortSatus = PORT_FREE;
}


static void UART6_RxEvent(uint8_t *pData, uint8_t len)
{
#ifdef _DEBUG_UART_
	 printf("\n<- ");
	  uint8_t aa = 0;
	  for (aa = 0; aa < len; aa++)
        {
			if ((aa%16) == 0) printf("\n");
			printf("%02X ", ((uint8_t*)pData)[aa] );
         }
#endif

MTBusManagerDataReceiver(&ServerA,pData,len);
//for(uint8_t i=0;i!=len;i++)
//MTBusServerDataReceiver(&ServerA,*pData++);
//MTBusDataReceiver(&MTBusModule,*pData++);  !!!!!!!!!!!!!!
asm("nop");
}


static void UART6_TxConfirm(void)
{
#ifdef __STM32__
//DE6_clr();
   GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_RESET);
#endif

asm("nop");
}


void Uart6SendPacket(uint8_t * p,uint16_t len)
{
asm("nop");
#ifdef __STM32__
//DE6_set();
 GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_SET);
#endif

WriteUart(&uart6Descriptor,p,len);
}


void Uart3SendPacket(uint8_t * p,uint16_t len)
{

 WriteUart(&uart3Descriptor,p,len);
}


void SlaveSendPacket(uint8_t * p,uint16_t len)
{
//DE1_set();
#ifdef __STM32__
  GPIO_WriteBit(GPIOB,GPIO_Pin_2,Bit_SET);
#endif

SlavePortSatus = PORT_BUSY;
WriteUart(&uart1Descriptor,p,len);
}



static void UART1_RxEvent(uint8_t *pData, uint8_t len)
{ uint8_t i;
for(i=0;i!=len;i++)
  TBusDataReceiver(&TBusSlave,*pData++);

asm("nop");
}


static void UART1_TxConfirm(void)
{
//DE1_clr();
#ifdef __STM32__
  GPIO_WriteBit(GPIOB,GPIO_Pin_2,Bit_RESET);
#endif

SlavePortSatus = PORT_FREE;
}

uint8_t GetSlavePortStatus(void)
{
uint8_t Status;
if(PORT_FREE==SlavePortSatus)
  Status = 0;
else
  Status = 1;
return Status;
}

void SetSlavePortStatus(uint8_t Status)
{
if(0==Status)
  SlavePortSatus = PORT_FREE;
else
  SlavePortSatus = PORT_BUSY;
}

void StopUart6Manager()
{
#ifdef __WINX__
 	CloseUart(&uart6Descriptor);
#endif
#ifdef __LINX__
 	CloseUart(&uart6Descriptor);
#endif

}

void StopUart3Manager()
{
#ifdef __WINX__
 	CloseUart(&uart3Descriptor);
#endif
#ifdef __LINX__
 	CloseUart(&uart3Descriptor);
#endif
}


