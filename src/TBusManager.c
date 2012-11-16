#include "../inc/includes.h"

//#include "TBusManager.h"
//#include "UartManager.h"
//#include "Archive.h"
//#include "ArchReader.h"


//__no_init uint8_t mem[32] @ 0x20000000;


TBusSlave_t TBusSlave;


uint8_t TxBuff[250];
uint8_t RxBuff[250];
uint8_t NetworkDeviceAddr;

static void DTransferRxEvent(DTConf_t *Conf);
static ArchConf_t ArchCallBack(ArchReq_t * Request);

void TBusManagerInit(void)
{
TBusSlave.NetworkDeviceAddr   = NetworkDeviceAddr;
TBusSlave.DevName             = (uint8_t*)TeraDeviceName;
TBusSlave.ProgrammVersion     = (uint8_t*)TeraDeviceProgrammVersion;
TBusSlave.TxBuff              = &TxBuff[0];
TBusSlave.TxBuffSize          = sizeof(TxBuff);
TBusSlave.RxBuff              = &RxBuff[0];
TBusSlave.RxBuffSize          = sizeof(RxBuff);
TBusSlave.SenderFunct         = SlaveSendPacket;   //Óêàçàòåëü íà ïîäïðîãðàììó îòïðàâêè len áàéò ïî èíòåðôåéñó
TBusSlave.GetPortStatusFunct  = GetSlavePortStatus;//Óêàçàòåëü íà ïîäïðîãðàììó, êîòîðàÿ ïîêàçûâàåò, ñâîáîäåí ëè îòïðàâùèê èëè íåò
TBusSlave.SetPortStatusFunct  = SetSlavePortStatus;//Óêàçàòåëü íà ïîäïðîãðàììó, êîòîðàÿ óñòàíàâëèâàåò ñòàòóñ îòïðàâùèêà
TBusSlave.DTransferRxCallBack = DTransferRxEvent;
TBusSlave.ArchCallBack        = ArchCallBack;


TBusInit(&TBusSlave);
}


static void DTransferRxEvent(DTConf_t *Conf)
{
asm("nop");  
  
//***Âñòàâëåíî äëÿ ïðèìåðà
Conf->Reserved=0xCA; 
Conf->DTState=Answer;
Conf->FragCnt =0x01;
Conf->FragIdx =0x02;

Conf->ProtType=BOOT_PROTOCOL;
Conf->pData= (uint8_t*)"abcdefgh";
Conf->DataCnt=8;
//***********************
 ////////////////
 /*
 __disable_interrupt();
 
  GrContextForegroundSet(&DisplayContext, 0xffff);
  DisplayContext.sClipRegion = (tRectangle){0,-1,479,318};
  GrRectFill(&DisplayContext, &DisplayContext.sClipRegion);
  ShowWaitMessage(&DisplayContext,"\rÂíèìàíèå!\rÏðèáîð íàõî-\räèòñÿ â ðåæèìå\rîáíîâëåíèÿ\rïðîøèâêè.", 1, 0, (void*)0);  
 
 *(uint32_t*)(&mem[0]) = 0x01020304;
 *(uint32_t*)(&mem[4]) = 0x01020304;
 *(uint32_t*)(&mem[8]) = 0x01020304;
 *(uint32_t*)(&mem[12]) = 0x01020304;
 *(uint32_t*)(&mem[16]) = 0x01020304;
 *(uint32_t*)(&mem[20]) = 0x01020304;
 *(uint32_t*)(&mem[24]) = 0x01020304;
 *(uint16_t*)(&mem[28]) = 0x0102;
 
 *(uint16_t*)(&mem[30]) = CALC_CHS_16(&mem[0], 30);
 
 // ðåêîíôèãóðèðóåì ïîðò 
 // íàïðÿâëåíèÿ ïåðåäà÷è
 GPIO_DeInit(GPIOB); 
 
 int * addr = (int*)0x08000004;
 // Initialize user application's Stack Pointer
 __set_MSP(*(__IO uint32_t*)0x08000000);
 // pointer recast as function pointer and the dereferenced/called
 ((void (*)())(*addr))();
   */
  
/*  
  WWDG_DeInit();  
  // Enable WWDG clock 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

  // WWDG clock counter = (PCLK1 (30MHz)/4096)/8 = 915 Hz (~1092 us) 
  WWDG_SetPrescaler(WWDG_Prescaler_8);

  // Set Window value to 80; WWDG counter should be refreshed only when the counter
  // is below 80 (and greater than 64) otherwise a reset will be generated 
  WWDG_SetWindowValue(80);

  // Enable WWDG and set counter value to 127, WWDG timeout = ~1092 us * 64 = 69.9 ms 
  // In this case the refresh window is: ~1092 * (127-80) = 51.3 ms < refresh window < ~1092 * 64 = 69.9ms
  WWDG_Enable(127);
 */
 while(1);
 ////////////////
 
}

void DateTimeConvPeriod(uint16_t *Day,uint32_t *HalfSec,uint16_t ArchPeriod)//Ïðèâåäåíèå ïðîèçâîëüíîé äàòû è âðåìåíè ê êðàòíîé ïåðèîäó àðõèâèðîâàíèÿ
{
uint16_t delta;
uint32_t ArchHalfSec;
delta = (*HalfSec)%ArchPeriod;
//if(delta>(ArchPeriod/2))//Åñëè îêðóãëåíè íóæíî ïðîèçâîäèòüâ áîëüøóþ ñòîðîíó
//  {
  
//  }
//else
//  {
  ArchHalfSec = *HalfSec-delta;
//  }

*HalfSec = ArchHalfSec;
asm("nop");
}

static ArchConf_t ArchCallBack(ArchReq_t * Request)
{

ArchConf_t ArchConf;
/*
ArchConf.DevAddr       = Request->DevAddr;

DateTimeConvPeriod(&Request->ArchDay,
                  &Request->ArchHalfSec,ArchPeriod);//Ïðèâåäåíèå äàòû è âðåìåíè ê êðàòíîé ïåðèîäó

ArchConf.ArchDay     = Request->ArchDay;
ArchConf.ArchHalfSec = Request->ArchHalfSec;

switch(Request->ArchReqType)//Ñìîòðèì, ÷òî çàïðàøèâàþò
{
case ARCH_GET_CELL_FORMAT_REQUEST_TYPE:
ArchConf.ArchCellCnt   = ArchCellCount; 
ArchConf.ArchItem      = (uint8_t)arch_struct[Request->ArchCellNumber].item;
ArchConf.ArchvType     = (uint8_t)arch_struct[Request->ArchCellNumber].type;
ArchConf.ArchChannel   = (uint8_t)arch_struct[Request->ArchCellNumber].channel;
ArchConf.ArchvIdx      = (uint8_t)arch_struct[Request->ArchCellNumber].IdxRAS;
ArchConf.ArchIdxArray  = (uint8_t)arch_struct[Request->ArchCellNumber].IdxArray;    //Èíäåêñ ìàññèâà 
ArchConf.ArchPointCast = *(uint8_t *)&arch_struct[Request->ArchCellNumber].PointCast;
break;  

case ARCH_GET_ARCH_PARAMS_REQUEST_TYPE:
ArchConf.ArchPeriod    = ArchPeriod; 
ArchConf.ArchRecSize   = ArchRecSize;
break;  

case ARCH_GET_REC_REQUEST_TYPE:
ArchRecRead(Request->pData,Request->ArchRecCnt,ArchConf.ArchDay,ArchConf.ArchHalfSec);  
break;

default:break;

}


asm("nop");
 * */
return ArchConf;
}

void TBusManagerRun(void)
{
TBusRun(&TBusSlave);
}

void TBusManagerTimeOutTimer(void)
{
TBusTimeOutCounter(&TBusSlave);
}