
#include "../inc/includes.h"
//#include "MTBus.h"
//#include "UartManager.h"
//#include "CHS.h"

typedef enum
{
UNKNOWN_VAR,
KNOWN_VAR,
READY_VAR, //Ðàáîòà íàä ïðèíÿòîé ïåðåìåííîé èëè ìàññèâîì çàêîí÷åíà
ARCH_PACKET_RECEIVED,
}VarStatus_t;

//VarStatus_t VarStatus;

MRAS_t         MRAS[MRAS_COUNT];
static uint8_t DataBuffer[BuffSizeToVar*MRAS_COUNT];


static void TbusConfirm(MTBusModule_t *MTBusModule);

static uint8_t SizeOfType(uint8_t type)
{
switch(type)
  {
  case 0:
  case 1:
    return 1;
  case 2:
  case 3:
    return 2;
  case 4:
  case 5:
  case 6:
    return 4;
  case 8:
    return 6;
  default:
    return 0;
  }
}

static void CopyStr(uint8_t *pReceiver,uint8_t *pSource)
{
while(*pSource)
  {
  *pReceiver++=*pSource++;
  }
}


void MTBusInit(MTBusModule_t *MTBusModule)
{
 uint16_t i = 0;
for(i=0;i!=MRAS_COUNT;i++)
  {
  MRAS[i].Status = VARIABLE_NOT_INIT_STATUS;
  MRAS[i].vType  = 0xFF;
  }
MTBusModule->LastTBusReq.ResponseTime    = 0xFFFF;
MTBusModule->TBusConfirm.Status          = MTB_FREE_RUN_STATUS;
}

void MTBusDataReceiver(MTBusModule_t *MTBusModule,uint8_t byte)//Ïåðåäàâàòü ïî îäíîìó áàéòó
{

MTBusModule->RxBuff[MTBusModule->RxLast]  = byte;
MTBusModule->RxLast++;
if(MTBusModule->RxLast>=(sizeof(MTBusModule->RxBuff)-1))
  MTBusModule->RxLast=0;
}



void MTBusFlowAnaliser(MTBusModule_t *MTBusModule,uint8_t byte)
{
if(MTBusModule->FlowAnaliser.State!=_CHS_ANALISE_STATE)
  CHS_16(&MTBusModule->FlowAnaliser.crc16,byte,calcCHS);


if(_SIGNATURE_ANALISE_STATE!=MTBusModule->FlowAnaliser.State)
  MTBusModule->FlowAnaliserTimeOutCounter=0;

switch(MTBusModule->FlowAnaliser.State)
  {
  case _SIGNATURE_ANALISE_STATE:

  if(byte==0x50)
    {
    MTBusModule->FlowAnaliser.pReceiveCHS =(uint8_t *)&MTBusModule->Tree.chs;
    MTBusModule->FlowAnaliser.pReceiveCHS++;
    MTBusModule->Tree.CntRAS  = 0x00;
    MTBusModule->Tree.IdxRAS  = 0x00;
    MTBusModule->Tree.error   = _errNone;
    MTBusModule->Tree.DataLen = 0;
    CHS_16(&MTBusModule->FlowAnaliser.crc16,0,resetCHS);
    MTBusModule->FlowAnaliser.State=_DEV_ADDR_RECEIVE_STATE;
    }
  break;  
  
  case _DEV_ADDR_RECEIVE_STATE:
    {
    MTBusModule->Tree.DevAddr=byte;
    MTBusModule->FlowAnaliser.State=_COMMAND_ANALISE_STATE;
    }
  break;

  case _COMMAND_ANALISE_STATE:
  MTBusModule->Tree.Command=(TbusCmd_t)byte;
    switch(MTBusModule->Tree.Command)
      {
      case _cmdReadRAM:
      case _cmdReadFlash:
      case _cmdReadEEPROM:

      case _cmdWriteRAM:
      case _cmdWriteFlash:
      case _cmdWriteEEPROM:

      MTBusModule->FlowAnaliser.State=_VAR_ADDR_RECEIVE_STATE;
      MTBusModule->FlowAnaliser.pReceive=(uint8_t *)&MTBusModule->Tree.VarAddr;
      MTBusModule->FlowAnaliser.inc=0;
      break;

      case _cmdId:
      MTBusModule->FlowAnaliser.State=_ID_RECEIVE_STATE;
      MTBusModule->FlowAnaliser.inc=0;
      break;

      case _cmdSetAddr:
      MTBusModule->FlowAnaliser.State=_OLD_DEV_ADDR_RECEIVE_STATE;
      MTBusModule->FlowAnaliser.inc=0;
      break;

      case _cmdGetVarList:
      case _cmdReadRAM_EXT:
      case _cmdWriteRAM_EXT:
      MTBusModule->FlowAnaliser.State=_IDX_RAS_RECEIVE_STATE;
      MTBusModule->FlowAnaliser.pReceive=(uint8_t *)&MTBusModule->Tree.IdxRAS;
      MTBusModule->FlowAnaliser.inc=0;
      break;

      case _cmdDataTransfer:
      MTBusModule->FlowAnaliser.State=_RESERVED_RECEIVE_STATE;
      break;  

	  case _cmdGetArch:
	  MTBusModule->FlowAnaliser.State=_ARCH_SIGN_RECEIVE_STATE;
	  break;

      default:
      MTBusModule->FlowAnaliser.State=_SIGNATURE_ANALISE_STATE;
      MTBusModule->Tree.Signature=0x50;
      MTBusModule->Tree.error=_errUnknownCmd;
      break;
      };
  break;  

  case _OLD_DEV_ADDR_RECEIVE_STATE:
  MTBusModule->Tree.OldDevAddr=byte;
  MTBusModule->FlowAnaliser.State=_CHS_ANALISE_STATE;
  break;

  case _VAR_ADDR_RECEIVE_STATE:
  *MTBusModule->FlowAnaliser.pReceive++=byte;

  if(MTBusModule->FlowAnaliser.inc++>=3)
    {
    MTBusModule->FlowAnaliser.State=_VAR_CNT_RECEIVE_STATE;
    }
  break;

  case _VAR_CNT_RECEIVE_STATE:
  MTBusModule->Tree.VarCnt=byte;
  MTBusModule->FlowAnaliser.State=_VAR_TYPE_ANALISE_STATE;
  break;

  case _VAR_TYPE_ANALISE_STATE:

  MTBusModule->Tree.VarType=(_VarType_t)byte;
  MTBusModule->Tree.VarSize=SizeOfType(MTBusModule->Tree.VarType);
  MTBusModule->Tree.DataLen=MTBusModule->Tree.VarCnt*MTBusModule->Tree.VarSize;

    switch(MTBusModule->Tree.Command)
    {
    case _cmdGetVarList:
      MTBusModule->FlowAnaliser.State=_VAR_WRITE_EN_RECEIVE_STATE;
      MTBusModule->FlowAnaliser.inc=0;
    break;

    case _cmdReadRAM_EXT:
      //MTBusModule->Tree.dataLen=1+MTBusModule->Tree.VarCnt*SizeOfType(MTBusModule->Tree.VarType);
      MTBusModule->FlowAnaliser.State=_DATA_RECEIVE_STATE;
      MTBusModule->FlowAnaliser.inc=0;

    break;

    case _cmdReadRAM:
    case _cmdReadFlash:
    case _cmdReadEEPROM:
    MTBusModule->FlowAnaliser.State=_DATA_RECEIVE_STATE;
    MTBusModule->FlowAnaliser.inc=0;
    break;

    case _cmdWriteRAM:
    case _cmdWriteFlash:
    case _cmdWriteEEPROM:
    case _cmdWriteRAM_EXT: 
      MTBusModule->FlowAnaliser.State=_CHS_ANALISE_STATE;
      MTBusModule->FlowAnaliser.inc=0;
    break;


    default:
    MTBusModule->FlowAnaliser.State=_SIGNATURE_ANALISE_STATE;
    break;//MTBusModule->Tree.Command
    }

  break;

  case _ID_RECEIVE_STATE:
  MTBusModule->Tree.data[MTBusModule->FlowAnaliser.inc]=byte;

  if(MTBusModule->FlowAnaliser.inc++>=31)
    {
    MTBusModule->FlowAnaliser.inc=0;
    MTBusModule->Tree.DataLen=32;
    MTBusModule->FlowAnaliser.State=_CHS_ANALISE_STATE;
    }
  break;

  case _IDX_RAS_RECEIVE_STATE:
    *MTBusModule->FlowAnaliser.pReceive++=byte;
     if(MTBusModule->FlowAnaliser.inc++>=1)
      {
      if(_cmdWriteRAM_EXT==MTBusModule->Tree.Command)
        {
        MTBusModule->FlowAnaliser.State=_IDX_MAS_RECEIVE_STATE;
        MTBusModule->FlowAnaliser.pReceive=(uint8_t *)&MTBusModule->Tree.IdxMAS;
        }
      else
        {
        MTBusModule->FlowAnaliser.State=_CNT_RAS_RECEIVE_STATE;  
        MTBusModule->FlowAnaliser.pReceive=(uint8_t *)&MTBusModule->Tree.CntRAS;
        }
      MTBusModule->FlowAnaliser.inc=0;
      }
  break;


  case _CNT_RAS_RECEIVE_STATE:
  //for(uint8_t i=0;i!=32;i++)
  // MTBusModule->Tree.data[i]=0;

    *MTBusModule->FlowAnaliser.pReceive++=byte;
     if(MTBusModule->FlowAnaliser.inc++>=1)
      {
      switch(MTBusModule->Tree.Command)
        {
        case _cmdGetVarList:
        case _cmdReadRAM_EXT:
        if((0xFFFF!=MTBusModule->Tree.CntRAS)&&(0xFFFF!=MTBusModule->Tree.IdxRAS))
          {
          MTBusModule->FlowAnaliser.State=_VAR_CNT_RECEIVE_STATE;
          MTBusModule->FlowAnaliser.inc=0;
          }
        else
          MTBusModule->FlowAnaliser.State=_CHS_ANALISE_STATE;
        MTBusModule->FlowAnaliser.inc=0;
        break;
        default:MTBusModule->FlowAnaliser.State=_SIGNATURE_ANALISE_STATE;break;
          }
        }
  break;

  
  case _VAR_WRITE_EN_RECEIVE_STATE:
  MTBusModule->Tree.VarWRE = byte;  
  MTBusModule->Tree.DataLen=18;   
  MTBusModule->FlowAnaliser.State=_DATA_RECEIVE_STATE;
  MTBusModule->FlowAnaliser.inc=0;  
  break;  

  case _IDX_MAS_RECEIVE_STATE:
     *MTBusModule->FlowAnaliser.pReceive++=byte;
     if(MTBusModule->FlowAnaliser.inc++>=1)
      {  
      MTBusModule->FlowAnaliser.State=_VAR_CNT_RECEIVE_STATE;  
      MTBusModule->FlowAnaliser.inc=0;
      }
  break;

  case _DATA_RECEIVE_STATE:
  //if(inc==0)
    MTBusModule->Tree.data[MTBusModule->FlowAnaliser.inc]=byte;

  if(MTBusModule->FlowAnaliser.inc++>=MTBusModule->Tree.DataLen-1)
    {
    MTBusModule->FlowAnaliser.inc=0;
    MTBusModule->FlowAnaliser.State=_CHS_ANALISE_STATE;
    }
  break;

  case _ARCH_SIGN_RECEIVE_STATE:
  MTBusModule->Tree.ArchSign = (ArchSign_t) byte;
  MTBusModule->FlowAnaliser.pReceive=(uint8_t *)&MTBusModule->Tree.ArchDay;
  MTBusModule->FlowAnaliser.inc=0;
  MTBusModule->FlowAnaliser.State=_ARCH_DAY_RECEIVE_STATE;
  break;

  case _ARCH_DAY_RECEIVE_STATE:
  *MTBusModule->FlowAnaliser.pReceive++=byte;
  if(MTBusModule->FlowAnaliser.inc++>=1)
    {
    MTBusModule->FlowAnaliser.pReceive=(uint8_t *)&MTBusModule->Tree.ArchHalfSec;
    MTBusModule->FlowAnaliser.inc=0;
    MTBusModule->FlowAnaliser.State=_ARCH_HALF_SEC_RECEIVE_STATE;
    }
  break;

  case _ARCH_HALF_SEC_RECEIVE_STATE:
  *MTBusModule->FlowAnaliser.pReceive++=byte;
  if(MTBusModule->FlowAnaliser.inc++>=3)
	{
	if(ARCH_CELL_FORMAT_SIGN==MTBusModule->Tree.ArchSign)
      {
      MTBusModule->FlowAnaliser.pReceive=(uint8_t *)&MTBusModule->Tree.ArchCellNumber;
      MTBusModule->FlowAnaliser.inc=0;
      MTBusModule->FlowAnaliser.State=_ARCH_CELL_NUMBER_RECEIVE_STATE;
      }
    else
      {
	  MTBusModule->FlowAnaliser.pReceive=(uint8_t *)&MTBusModule->Tree.ArchRecCount;
      MTBusModule->FlowAnaliser.inc=0;
	  MTBusModule->FlowAnaliser.State=_ARCH_REC_COUNT_RECEIVE_STATE;
      }
    }
  break;

  case _ARCH_REC_COUNT_RECEIVE_STATE:
  *MTBusModule->FlowAnaliser.pReceive++=byte;
  if(MTBusModule->FlowAnaliser.inc++>=3)
	{
	if(0xFFFFFFFF==MTBusModule->Tree.ArchRecCount)//Îáíàðóæåí òåðìèíàòîð
	  {
	  MTBusModule->FlowAnaliser.inc=0;
	  MTBusModule->FlowAnaliser.State=_CHS_ANALISE_STATE;
	  }
	else
		{
		MTBusModule->FlowAnaliser.pReceive=(uint8_t *)&MTBusModule->Tree.ArchPeriod;
		MTBusModule->FlowAnaliser.inc=0;
		MTBusModule->FlowAnaliser.State=_ARCH_PERIOD_RECEIVE_STATE;
		}
	}
  break;

  case _ARCH_PERIOD_RECEIVE_STATE:
   *MTBusModule->FlowAnaliser.pReceive++=byte;
  if(MTBusModule->FlowAnaliser.inc++>=1)
    {
    MTBusModule->FlowAnaliser.pReceive=(uint8_t *)&MTBusModule->Tree.ArchRecSize;
    MTBusModule->FlowAnaliser.inc=0;
	MTBusModule->FlowAnaliser.State=_ARCH_REC_SIZE_RECEIVE_STATE;
    }
  break;


  case _ARCH_REC_SIZE_RECEIVE_STATE:
  *MTBusModule->FlowAnaliser.pReceive++=byte;
  if(MTBusModule->FlowAnaliser.inc++>=1)
	{
	MTBusModule->FlowAnaliser.pReceive=(uint8_t *)&MTBusModule->Tree.data[0];
    MTBusModule->FlowAnaliser.inc=0;
	MTBusModule->FlowAnaliser.State=_ARCH_REC_RECEIVE_STATE;
    }
  break;




  case _ARCH_REC_RECEIVE_STATE:
  *MTBusModule->FlowAnaliser.pReceive++=byte;
  if(MTBusModule->FlowAnaliser.inc++>=(uint8_t)((MTBusModule->Tree.ArchRecSize*MTBusModule->Tree.ArchRecCount)-1))
    {
    MTBusModule->FlowAnaliser.inc=0;
    MTBusModule->FlowAnaliser.State=_CHS_ANALISE_STATE;
    }

  break;

  case _ARCH_CELL_NUMBER_RECEIVE_STATE: //Ïðèåì íîìåðà àðõèâíîé ÿ÷åéêè
  *MTBusModule->FlowAnaliser.pReceive++=byte;

  if(MTBusModule->FlowAnaliser.inc++>=1)
    {
    MTBusModule->FlowAnaliser.pReceive=(uint8_t *)&MTBusModule->Tree.ArchCellCnt;
    MTBusModule->FlowAnaliser.inc=0;
    MTBusModule->FlowAnaliser.State=_ARCH_CELL_CNT_RECEIVE_STATE;
    }
  break;


  case _ARCH_CELL_CNT_RECEIVE_STATE: //Ïðèåì êîëè÷åñòâà àðõèâíûõ ÿ÷ååê
  *MTBusModule->FlowAnaliser.pReceive++=byte;
  if(MTBusModule->FlowAnaliser.inc++>=1)
    {
    MTBusModule->FlowAnaliser.pReceive=(uint8_t *)&MTBusModule->Tree.data[0];
    MTBusModule->FlowAnaliser.inc=0;
    if(0xFFFF==MTBusModule->Tree.ArchCellNumber)
      MTBusModule->FlowAnaliser.State=_CHS_ANALISE_STATE;
    else
      MTBusModule->FlowAnaliser.State=_ARCH_CELL_FORMAT_RECEIVE_STATE;
    }

  break;

  case _ARCH_CELL_FORMAT_RECEIVE_STATE:
  *MTBusModule->FlowAnaliser.pReceive++=byte;
  if(MTBusModule->FlowAnaliser.inc++>=7)
    {
    MTBusModule->FlowAnaliser.inc=0;
    MTBusModule->FlowAnaliser.State=_CHS_ANALISE_STATE;
    }
  break;

  case _RESERVED_RECEIVE_STATE:
  MTBusModule->Tree.Reserved      = byte;  
  MTBusModule->FlowAnaliser.State = _STATE_RECEIVE_STATE;
  break;  
  
  case _STATE_RECEIVE_STATE:
  MTBusModule->Tree.DTState = (MDTState_t)byte;  
  MTBusModule->FlowAnaliser.State = _FRAG_CNT_IDX_RECEIVE_STATE;  
  break;  
 
  case _FRAG_CNT_IDX_RECEIVE_STATE:
  MTBusModule->Tree.FragCnt  = byte>>4;
  MTBusModule->Tree.FragIdx  = 0x0F&byte;
  MTBusModule->FlowAnaliser.State =  _PROT_TYPE_RECEIVE_STATE;
  break;  
  
  case _PROT_TYPE_RECEIVE_STATE:
  MTBusModule->Tree.ProtType=(MProtType_t)byte;  
  MTBusModule->FlowAnaliser.State = _DATA_CNT_RECEIVE_STATE;
  break;  
  
  case _DATA_CNT_RECEIVE_STATE:
  MTBusModule->Tree.DataCnt       = byte;
  MTBusModule->Tree.DataLen       = MTBusModule->Tree.DataCnt;
  MTBusModule->FlowAnaliser.State = _DATA_RECEIVE_STATE;
  MTBusModule->FlowAnaliser.inc   = 0;    
  break;  
  
    
  case _CHS_ANALISE_STATE:
  *MTBusModule->FlowAnaliser.pReceiveCHS--=byte;
  if(MTBusModule->FlowAnaliser.inc++>=1)
    {
    MTBusModule->FlowAnaliser.State=_SIGNATURE_ANALISE_STATE;
    if(MTBusModule->FlowAnaliser.crc16==MTBusModule->Tree.chs)
      {
      MTBusModule->Tree.Signature=0x50;
      }
    else
      {
      MTBusModule->Tree.error     = _errCHS;
      MTBusModule->Tree.Signature = 0x50;
      }
    }
  break;  
  
  default:
  MTBusModule->FlowAnaliser.State=_SIGNATURE_ANALISE_STATE;
  break;
  };
}

//--------Ïîäïðîãðàììà ôîðìèðîâàíèÿ çàïðîñà-------------
void MTBusReq(MTBusModule_t *MTBusModule,MTBusDescriptor_t *Descriptor)
{
uint8_t *pBuff;
uint8_t *pSource;
uint16_t CHS;
uint8_t TxLen;

uint8_t i;
 
MTBusModule->LastTBusReq=*Descriptor;

MTBusModule->TimeOut_=0;

pBuff=&MTBusModule->TxBuff[0];

pSource = (uint8_t *)&Descriptor->ServiceKey;

if(Descriptor->ServiceKey!=NONE_SERVICE_KEY)
  {
	  uint8_t inc;
  for(inc=0;inc!=sizeof(ServiceKey_t);inc++)
    *pBuff++ = *pSource++;
  }

*pBuff++=0x50;

switch(Descriptor->Command)
{
case _cmdReadRAM:
case _cmdReadEEPROM:
case _cmdReadFlash:
  
  TxLen=11;
  
  *pBuff++=Descriptor->DevAddr;
  *pBuff++=Descriptor->Command;

  *pBuff++=(uint8_t)(((uint32_t )Descriptor->VarAddr));
  *pBuff++=(uint8_t)(((uint32_t )Descriptor->VarAddr)>>0x08);
  *pBuff++=(uint8_t)(((uint32_t )Descriptor->VarAddr)>>0x10);
  *pBuff++=(uint8_t)(((uint32_t )Descriptor->VarAddr)>>0x18);

  *pBuff++=Descriptor->VarCnt;
  *pBuff++=Descriptor->VarType;
break;

case _cmdId:
  TxLen=5;
 
  *pBuff++=Descriptor->DevAddr;
  *pBuff++=Descriptor->Command;
break;

case _cmdSetAddr:
  TxLen=6;
  *pBuff++=Descriptor->DevAddr;
  *pBuff++=Descriptor->Command;
  *pBuff++=Descriptor->NewDevAddr;
break;

case _cmdReadRAM_EXT:
case _cmdGetVarList:
  TxLen=9;

  *pBuff++=Descriptor->DevAddr;
  *pBuff++=Descriptor->Command;
  *pBuff++=(uint8_t)Descriptor->IdxRAS;
  *pBuff++=(uint8_t)(Descriptor->IdxRAS>>0x08);
  *pBuff++=(uint8_t)Descriptor->CntRAS;
  *pBuff++=(uint8_t)(Descriptor->CntRAS>>0x08);
break;

case _cmdWriteRAM_EXT:
  TxLen=11+Descriptor->VarCnt*SizeOfType(Descriptor->VarType);

  *pBuff++=Descriptor->DevAddr;
  *pBuff++=Descriptor->Command;
  *pBuff++=(uint8_t)Descriptor->IdxRAS;
  *pBuff++=(uint8_t)(Descriptor->IdxRAS>>0x08);
  *pBuff++=(uint8_t)Descriptor->IdxMAS;
  *pBuff++=(uint8_t)(Descriptor->IdxMAS>>0x08);
  *pBuff++=Descriptor->VarCnt;
  *pBuff++=Descriptor->VarType;
 
 
  for(i=0;i!=TxLen-11;i++)
    *pBuff++=*(Descriptor->pData++);

break;


case _cmdWriteRAM:
case _cmdWriteEEPROM:
case _cmdWriteFlash:

  TxLen=9+SizeOfType(Descriptor->VarType)*Descriptor->VarCnt+2;
   
  *pBuff++=Descriptor->DevAddr;
  *pBuff++=Descriptor->Command;
  *pBuff++=(uint8_t)(((uint32_t )Descriptor->VarAddr));
  *pBuff++=(uint8_t)(((uint32_t )Descriptor->VarAddr)>>0x08);
  *pBuff++=(uint8_t)(((uint32_t )Descriptor->VarAddr)>>0x10);
  *pBuff++=(uint8_t)(((uint32_t )Descriptor->VarAddr)>>0x18);
  *pBuff++=Descriptor->VarCnt;
  *pBuff++=Descriptor->VarType;
  for(i=0;i!=TxLen-11;i++)
    *pBuff++=*(Descriptor->pData++);
break;

case _cmdDataTransfer:
  TxLen=10+Descriptor->DataCnt;

  *pBuff++=Descriptor->DevAddr;
  *pBuff++=Descriptor->Command;
  *pBuff++=Descriptor->Reserved;
  *pBuff++=(uint8_t)Descriptor->State;
  *pBuff++=((Descriptor->FragCnt<<4)|Descriptor->FragIdx);
  *pBuff++=(uint8_t)Descriptor->ProtType;
  *pBuff++=Descriptor->DataCnt;
  for(i=0;i!=Descriptor->DataCnt;i++)
	*pBuff++=*(Descriptor->pData++);
break;

case _cmdGetArch:
  TxLen=16;//+Descriptor->DataCnt;

  *pBuff++=Descriptor->DevAddr;
  *pBuff++=Descriptor->Command;

  *pBuff++=(uint8_t)Descriptor->ArchSign;
  
  pSource = (uint8_t *)&Descriptor->ArchDay;
  *pBuff++=*pSource++;
  *pBuff++=*pSource++;

  pSource = (uint8_t *)&Descriptor->ArchHalfSec;
  *pBuff++=*pSource++;
  *pBuff++=*pSource++;
  *pBuff++=*pSource++;
  *pBuff++=*pSource++;

  if(ARCH_CELL_FORMAT_SIGN==Descriptor->ArchSign)//Çàïðîñ ôîðìàòà àðõèâíûõ ÿ÷ååê
    {
    pSource = (uint8_t *)&Descriptor->ArchCellNumber;
    *pBuff++=*pSource++;
    *pBuff++=*pSource++;
  
    pSource = (uint8_t *)&Descriptor->ArchCellCnt;
    *pBuff++=*pSource++;
    *pBuff++=*pSource++;
    }
  else
    {
    pSource = (uint8_t *)&Descriptor->ArchRecCount;
    *pBuff++=*pSource++;
    *pBuff++=*pSource++;
    *pBuff++=*pSource++;
    *pBuff++=*pSource++;
     }

  //*pBuff++=Descriptor->Reserved;
  //*pBuff++=(uint8_t)Descriptor->State;
  //*pBuff++=((Descriptor->FragCnt<<4)|Descriptor->FragIdx);
  //*pBuff++=(uint8_t)Descriptor->ProtType;
  //*pBuff++=Descriptor->DataCnt;
break;


default:break;
}
if(Descriptor->ServiceKey!=NONE_SERVICE_KEY)
  {
  if(Descriptor->ServiceKey!=NONE_SERVICE_KEY)
    TxLen+=sizeof(ServiceKey_t);  
  CHS=CALC_CHS_16(&MTBusModule->TxBuff[1+sizeof(ServiceKey_t)],TxLen-3-sizeof(ServiceKey_t));
  }
else
  {
  CHS=CALC_CHS_16(&MTBusModule->TxBuff[1],TxLen-3);
  }
*pBuff++=(uint8_t)(CHS>>0x08);
*pBuff++=(uint8_t)(CHS);

Descriptor->SenderFunct(MTBusModule->TxBuff,TxLen);
}
//------------------------------------------------------




//------------------------------------------------------
void MTBusRun(MTBusModule_t *MTBusModule)
{
	uint8_t i;
NotificationReq_t NotificationReq; 
NotificationResp_t NotificationResp;  
//NotificationResp_t (*NoticationCallBack)(NotificationReq_t *Notification);  
  
while((MTBusModule->RxFirst!=MTBusModule->RxLast)&&(0==MTBusModule->Tree.Signature))//Åñëè åñòü ïðèíÿòûå äàííûå
  {
  MTBusFlowAnaliser(MTBusModule,MTBusModule->RxBuff[MTBusModule->RxFirst++]);     //Àíàëèç âõîäíîãî ïîòîêà áàéò
  if(MTBusModule->RxFirst>=(sizeof(MTBusModule->RxBuff)-1))
    MTBusModule->RxFirst=0;
  }


if(0x50==MTBusModule->Tree.Signature)//Âõîäÿùèé ïàêåò áûë îáðàáîòàí
  {
  MTBusModule->Tree.Signature = 0;  
  for(i=0;i!=MaxDevices;i++)
    { 
    if(MTBusModule->AddedDevices[i]==MTBusModule->Tree.DevAddr)
      {
      MTBusModule->Tree.Signature = 0x50;  
      break;
      }
    }
  if(0==MTBusModule->Tree.Signature)//Åñëè ýòî óñòðîéñòâî íå íàéäåíî
    {
    NotificationReq.DevAddr = MTBusModule->Tree.DevAddr;
    NotificationReq.Status  = NEW_DEVICE_FOUND_NTF_STATUS;
    NotificationResp        = MTBusModule->NoticationCallBack(&NotificationReq); 
    if(NEW_DEVICE_ADDED_ENABLE == NotificationResp.Status)
      {
      MTBusModule->Tree.Signature = 0x50;
      for(i=0;i!=MaxDevices;i++)
        {
        if(MTBusModule->Tree.DevAddr == MTBusModule->AddedDevices[i])//Åñëè ýòî óñòðîéñòâî óæå åñòü  
          break;
        if(0==MTBusModule->AddedDevices[i])
          {
          MTBusModule->AddedDevices[i] = MTBusModule->Tree.DevAddr;  
          break;
          }
        }
      }
    }
  }

if(0x50==MTBusModule->Tree.Signature)//Âõîäÿùèé ïàêåò áûë îáðàáîòàí
{
MTBusModule->Tree.Signature         = 0x00;

MTBusModule->TBusConfirm.ResponseTime  = MTBusModule->TimeOut_;
MTBusModule->TimeOut_=0;

switch(MTBusModule->Tree.Command)
  {

  case _cmdId:
	if(_errNone==MTBusModule->Tree.error)
      MTBusModule->TBusConfirm.Status=MTB_SUCCESS_STATUS;
    else if(_errCHS==MTBusModule->Tree.error)
      MTBusModule->TBusConfirm.Status=MTB_CHS_ERROR_STATUS;
  break;

  
  case _cmdReadRAM:
  case _cmdReadEEPROM:
  case _cmdReadFlash:
  case _cmdReadRAM_EXT:
  case _cmdWriteRAM_EXT:
  case _cmdWriteRAM:
  case _cmdWriteFlash:
  case _cmdWriteEEPROM:
  case _cmdGetArch:
	if(_errNone==MTBusModule->Tree.error)
	  MTBusModule->TBusConfirm.Status=MTB_SUCCESS_STATUS;
    else if(_errCHS==MTBusModule->Tree.error)
      MTBusModule->TBusConfirm.Status=MTB_CHS_ERROR_STATUS;
  break;

  case _cmdSetAddr:
    MTBusModule->TBusConfirm.OldDevAddr=MTBusModule->Tree.OldDevAddr;
    if(_errNone==MTBusModule->Tree.error)
      MTBusModule->TBusConfirm.Status=MTB_SUCCESS_STATUS;
    else if(_errCHS==MTBusModule->Tree.error)
      MTBusModule->TBusConfirm.Status=MTB_CHS_ERROR_STATUS;
  break;


  case _cmdGetVarList:
  if(_errNone==MTBusModule->Tree.error)
    MTBusModule->TBusConfirm.Status=MTB_SUCCESS_STATUS;
  else if(_errCHS==MTBusModule->Tree.error)
    MTBusModule->TBusConfirm.Status=MTB_CHS_ERROR_STATUS;
  break;

  case _cmdDataTransfer:
  if(_errNone==MTBusModule->Tree.error)
    MTBusModule->TBusConfirm.Status=MTB_SUCCESS_STATUS;
  else if(_errCHS==MTBusModule->Tree.error)
    MTBusModule->TBusConfirm.Status=MTB_CHS_ERROR_STATUS;   
  break;  
  
  default: break;
  }
}


//  if(_cmdId == MTBusModule->TBusConfirm.Command)
//    {  
//    if(MTB_SUCCESS_STATUS==MTBusModule->TBusConfirm.Status)
//      {
//        asm("nop");
//      NewDev = 1;  
//      for(uint16_t i=0;i!=sizeof(MDevId)/sizeof(DevId_t);i++)
//        {
//        if(TBusConf->DevAddr == MDevId[i].DevAddr)//Åñëè òàêîé àäðåñ óæå åñòü â MDevId
//          {
//            
//          NewDev = 0;
//          for(uint8_t d=0;d!=20;d++)
//            MDevId[i].DevName[d]=TBusConf->data[d];
//          for(uint8_t d=0;d!=10;d++)
//            MDevId[i].ProgVer[d]=TBusConf->data[d+22];
//          break;
//          }
//        }
//      //Åñëè òàêîãî àäðåñà íåòó â MDevId
//      if(NewDev)
//        {
//        //------------------------------
//        Server->NetDevices[Server->NetDevCnt++]=TBusConf->DevAddr;
//        
//        MDevId[NetDevicesCnt].DevAddr=TBusConf->DevAddr;
//        MDevId[NetDevicesCnt].HostAddr = (uint8_t *)Server;
//        
//        for(uint8_t d=0;d!=20;d++)
//          MDevId[NetDevicesCnt].DevName[d]=TBusConf->data[d];
//        for(uint8_t d=0;d!=10;d++)
//          MDevId[NetDevicesCnt].ProgVer[d]=TBusConf->data[d+22];
//        
//        NetDevicesCnt++;
//        }
      ////MTBusAddDevice((MTBusServer_t *)TBusConf->InitiatorAddr,TBusConf->DevAddr);
//      }
//    }
//    



if(MTB_FREE_RUN_STATUS!=MTBusModule->TBusConfirm.Status)
  {
  TbusConfirm(MTBusModule);
  MTBusModule->TBusConfirm.Status=MTB_FREE_RUN_STATUS;
  }
}

//Ïîäïðîãðàììà ñîðòèðîâêè ïîëó÷åííûõ äàííûõ (ñêëàäûâàåò èõ â ñòðóêòóðó MRAS)
MTBusMessage_t SortingData(MTree_t *Tree)
{
uint8_t        *pData;  
uint8_t        *pBuff;
VarStatus_t    VarStatus    = UNKNOWN_VAR;
MTBusMessage_t MTBusMessage = FREE;
uint16_t i;
uint16_t MRASIdx;
uint8_t inc;
uint8_t j;

//---Ïîèñê ïåðåìåííîé â òàáëèöå, è çàïîëíèå ñòðîêè òàáëèöè, åñëè îíà åñòü äëÿ ýòîé ïåðåìåííîé
for(i=0;i!=MRAS_COUNT;i++)
  {
  if((MRAS[i].DevAddr==Tree->DevAddr)&&(MRAS[i].vIdx==Tree->IdxRAS))
    {
    switch(Tree->Command)
      {
      case _cmdReadRAM:
      case _cmdReadRAM_EXT:

        MRAS[i].DevAddr   = Tree->DevAddr;
        MRAS[i].vIdx      = Tree->IdxRAS;
        MRAS[i].vType     = Tree->VarType;
        MRAS[i].vCnt      = Tree->VarCnt;
        pData             = &Tree->data[0];
        //-------Âû÷èñëåíèå èíäåêñà ñâîáîäíîãî ìåñòà â áóôåðå äàííûõ---------
        MRAS[i].pData     = &DataBuffer[0];
        
		for(MRASIdx=0;MRASIdx!=i;MRASIdx++)
          MRAS[i].pData+=MRAS[MRASIdx].vCnt*SizeOfType(MRAS[MRASIdx].vType);
        //-------------------------------------------------------------------
        pBuff=MRAS[i].pData;
		for(inc=0;inc!=MRAS[i].vCnt*SizeOfType(MRAS[i].vType);inc++)
          *pBuff++=*pData++;  
          
        if(VARIABLE_NOT_INIT_STATUS == MRAS[i].Status)
          MRAS[i].Status = VARIABLE_PART_INIT_STATUS;
        else if(VARIABLE_PART_INIT_STATUS==MRAS[i].Status)
          MRAS[i].Status = VARIABLE_IS_INIT_STATUS;

      break;

      case _cmdGetVarList:
        MRAS[i].DevAddr = Tree->DevAddr;
        MRAS[i].vIdx    = Tree->IdxRAS;
        MRAS[i].vType   = Tree->VarType;
        MRAS[i].vCnt    = Tree->VarCnt;
        MRAS[i].vWRE    = Tree->VarWRE;
        pData           = &Tree->data[0];
        //-------Âû÷èñëåíèå èíäåêñà ñâîáîäíîãî ìåñòà â áóôåðå äàííûõ---------
        MRAS[i].pData   = &DataBuffer[0];
        for(MRASIdx=0;MRASIdx!=i;MRASIdx++)
          MRAS[i].pData+=MRAS[MRASIdx].vCnt*SizeOfType(MRAS[MRASIdx].vType);
        //-------------------------------------------------------------------
        //MTBusModule->TBusConf.data+=1;
        for(j=0;j!=18;j++)
          MRAS[i].vInfo[j]=*pData++;
                  
        if(VARIABLE_NOT_INIT_STATUS == MRAS[i].Status)
          MRAS[i].Status = VARIABLE_PART_INIT_STATUS;
        else if(VARIABLE_PART_INIT_STATUS==MRAS[i].Status)
          MRAS[i].Status = VARIABLE_IS_INIT_STATUS;

      break;
      default:break;
      }
    VarStatus=KNOWN_VAR;
    }
  }
//-------------------------------------------------------------------------

//------------- Ïðèåì äàííûõ è äðóãîé èíôû ðàíåå íåèçâåñòíîé ïåðåìåííîé----
if(UNKNOWN_VAR==VarStatus)
  {
 
  for(i=0;i!=MRAS_COUNT;i++)
    {
    if(0x00==MRAS[i].DevAddr)//Íàéäåíà ñâîáîäíàÿ ñòðîêà â òàáëèöå
      {
      switch(Tree->Command)
        {
        case _cmdReadRAM:
        case _cmdReadRAM_EXT:
          MRAS[i].DevAddr = Tree->DevAddr;
          MRAS[i].vIdx    = Tree->IdxRAS;
          MRAS[i].vType   = Tree->VarType;
          MRAS[i].vCnt    = Tree->VarCnt;
          pData           = &Tree->data[0];

          //-------Âû÷èñëåíèå èíäåêñà ñâîáîäíîãî ìåñòà â áóôåðå äàííûõ---------
          MRAS[i].pData=&DataBuffer[0];
          for( MRASIdx=0;MRASIdx!=i;MRASIdx++)
                MRAS[i].pData+=MRAS[MRASIdx].vCnt*SizeOfType(MRAS[MRASIdx].vType);
          //-------------------------------------------------------------------
          pBuff=MRAS[i].pData;
          for( inc=0;inc!=MRAS[i].vCnt*SizeOfType(MRAS[i].vType);inc++)
                *pBuff++=*pData;
          CopyStr(&MRAS[i].vInfo[0],(uint8_t*)"New Variable");
          MTBusMessage=NEW_VAR_DETECTED;     
            
          if(VARIABLE_NOT_INIT_STATUS == MRAS[i].Status)
            MRAS[i].Status = VARIABLE_PART_INIT_STATUS;
          else if(VARIABLE_PART_INIT_STATUS==MRAS[i].Status)
            MRAS[i].Status = VARIABLE_IS_INIT_STATUS; 
        break;

        case _cmdGetVarList:
          MRAS[i].DevAddr = Tree->DevAddr;
          MRAS[i].vIdx    = Tree->IdxRAS;
          MRAS[i].vType   = Tree->VarType;
          MRAS[i].vCnt    = Tree->VarCnt;
          MRAS[i].vWRE    = Tree->VarWRE;

          pData = &Tree->data[0]; 
		  //-------Âû÷èñëåíèå èíäåêñà ñâîáîäíîãî ìåñòà â áóôåðå äàííûõ---------
          MRAS[i].pData=&DataBuffer[0];
            for( MRASIdx=0;MRASIdx!=i;MRASIdx++)
          MRAS[i].pData+=MRAS[MRASIdx].vCnt*SizeOfType(MRAS[MRASIdx].vType);
          //-------------------------------------------------------------------
          //MTBusModule->TBusConf.data+=1;
          for( j=0;j!=18;j++)
            MRAS[i].vInfo[j]=*pData++;
                    
          if(VARIABLE_NOT_INIT_STATUS == MRAS[i].Status)
            MRAS[i].Status = VARIABLE_PART_INIT_STATUS;
          else if(VARIABLE_PART_INIT_STATUS==MRAS[i].Status)
            MRAS[i].Status = VARIABLE_IS_INIT_STATUS;

        break;

        default:break;
        }
        break;
      }
    }
  }
return MTBusMessage;
}

void TbusConfirm(MTBusModule_t *MTBusModule)
{
	uint16_t mrcnt;
VarStatus_t VarStatus = UNKNOWN_VAR;

if(MTB_SUCCESS_STATUS==MTBusModule->TBusConfirm.Status)
  MTBusModule->Succes=1;
else
  MTBusModule->Problem=1;

MTBusModule->TimeOut_=0;

switch(MTBusModule->TBusConfirm.Status)
{
case MTB_SUCCESS_STATUS://Îòâåò îò ïðèáîðà áåç îøèáîê


VarStatus=UNKNOWN_VAR;

if(0!=MTBusModule->LastTBusReq.CntRAS)
	MTBusModule->LastTBusReq.CntRAS--;
//if(0!=MTBusModule->LastTBusReq.ArchRecCount)
//	MTBusModule->LastTBusReq.ArchRecCount--;
if(0!=MTBusModule->LastTBusReq.ArchCellCnt)
	MTBusModule->LastTBusReq.ArchCellCnt--;

MTBusModule->TBusConfirm.ResponseTime+=MTBusModule->TBusConfirm.ResponseTime;

switch(MTBusModule->Tree.Command)
  {
  case _cmdId:
    VarStatus=READY_VAR;
    MTBusModule->LastTBusReq.ResponseTime=0xFFFF;
  break;

  case _cmdSetAddr:
    for(mrcnt=0;mrcnt!=MRAS_COUNT;mrcnt++)//Âíåñòè èçìåíåíèå àäðåñà â òàáëèöó
      {
      if(MTBusModule->TBusConfirm.OldDevAddr==MRAS[mrcnt].DevAddr)
        MRAS[mrcnt].DevAddr=MTBusModule->Tree.DevAddr;
      }

    VarStatus=READY_VAR;
    MTBusModule->LastTBusReq.ResponseTime=0xFFFF;
  break;

  case _cmdReadRAM_EXT:
    if((0xFFFF==MTBusModule->Tree.CntRAS)&&(0xFFFF==MTBusModule->Tree.IdxRAS))
      {
      VarStatus=READY_VAR;
      MTBusModule->LastTBusReq.ResponseTime=0xFFFF;
      }
  break;

  case _cmdGetVarList:
    if((0xFFFF==MTBusModule->Tree.CntRAS)&&(0xFFFF==MTBusModule->Tree.IdxRAS))
      {
      MTBusModule->LastTBusReq.ResponseTime=0xFFFF;
      VarStatus=READY_VAR;
      }
  break;

  case _cmdDataTransfer:
  VarStatus=READY_VAR;
  MTBusModule->LastTBusReq.ResponseTime=0xFFFF;
  break;
  

  case _cmdGetArch:
  VarStatus = ARCH_PACKET_RECEIVED;
  MTBusModule->ArchConfirm.ArchStatus = ARCH_SUCCES_STATUS;
  if(ARCH_CELL_FORMAT_SIGN==MTBusModule->Tree.ArchSign)
    {
    if(0xFFFF==MTBusModule->Tree.ArchCellNumber)//Îáíàðóæåí òåðìèíàòîð
      {
      VarStatus=READY_VAR;
      MTBusModule->LastTBusReq.ResponseTime=0xFFFF;
      }
    }
  else
    {
	if(0xFFFFFFFF==MTBusModule->Tree.ArchRecCount)//Îáíàðóæåí òåðìèíàòîð
      {
      VarStatus=READY_VAR;
      MTBusModule->LastTBusReq.ResponseTime=0xFFFF;
      }
    }


  break;

default: break;
}

if(VarStatus!=READY_VAR)//Îïåðàöèè íàä îáðàáîòêîé âõîäíûõ äàííûõ íå çàâåðøåíû
  {
  MTBusModule->TBusConfirm.Message = SortingData(&MTBusModule->Tree);
  }


switch(MTBusModule->Tree.Command)
  {
  case _cmdGetVarList:
  if(0==MTBusModule->LastTBusReq.CntRAS)//Ïîñëåäíèé ïàêåò áîëüøîé ïîñûëêè
    {
    MTBusModule->LastTBusReq.ResponseTime=0xFFFF;
    VarStatus=READY_VAR;
    }
  break;
  case _cmdReadRAM_EXT:
  if(0==MTBusModule->LastTBusReq.CntRAS)//Ïîñëåäíèé ïàêåò áîëüøîé ïîñûëêè
    {
    VarStatus=READY_VAR;
    MTBusModule->LastTBusReq.ResponseTime=0xFFFF;
    }
  break;

  case _cmdReadRAM:
  case _cmdReadFlash:
  case _cmdReadEEPROM:

  case _cmdWriteRAM:
  case _cmdWriteFlash:
  case _cmdWriteEEPROM:
  case _cmdWriteRAM_EXT:
  VarStatus=READY_VAR;
  MTBusModule->LastTBusReq.ResponseTime=0xFFFF;
  break;

  default:break;//MTBusModule->Tree.Command
  }

break;


case MTB_CHS_ERROR_STATUS: //Áûëà îáíàðóæåíà îøèáêà ïàêåòà
MTBusModule->LastTBusReq.CntRAS--;
MTBusModule->TBusConfirm.ResponseTime+=MTBusModule->TBusConfirm.ResponseTime;

switch(MTBusModule->Tree.Command)
  {
  case _cmdGetVarList:
  case _cmdReadRAM_EXT:
   MTBusModule->TBusConfirm.Problem[MTBusModule->BadVarIdx].BadVar=MTBusModule->Tree.IdxRAS;
   MTBusModule->TBusConfirm.Problem[MTBusModule->BadVarIdx].VarErr=MTBusModule->TBusConfirm.Status;
   MTBusModule->BadVarIdx++;
   if(MTBusModule->BadVarIdx>=10)
    MTBusModule->BadVarIdx=0;  
   if(0==MTBusModule->LastTBusReq.CntRAS)
    {
    MTBusModule->LastTBusReq.ResponseTime=0xFFFF;
    VarStatus=READY_VAR;
    }
  break;

  case _cmdId:
  case _cmdSetAddr:
  case _cmdReadRAM:
  case _cmdReadEEPROM:
  case _cmdReadFlash:
  
  case _cmdWriteRAM:
  case _cmdWriteEEPROM:
  case _cmdWriteFlash:
  case _cmdWriteRAM_EXT:
  case _cmdDataTransfer:
  case _cmdGetArch:
    if ( MTBusModule->Tree.Command == _cmdGetArch)  //! sky 13.07.12
    {
     VarStatus = ARCH_PACKET_RECEIVED;
     MTBusModule->ArchConfirm.ArchStatus = ARCH_CHS_ERROR_STATUS;
     if(0==MTBusModule->LastTBusReq.ArchCellCnt)
      {
       VarStatus=READY_VAR;
       MTBusModule->LastTBusReq.ResponseTime=0xFFFF;
       }
     }
  break;
default:break;
}
break;

case MTB_NO_RESPONSE_STATUS://Âðåìÿ îæèäàíèÿ îòâåòà îò ïðèáîðà èñòåêëî
MTBusModule->LastTBusReq.CntRAS--;
MTBusModule->TBusConfirm.ResponseTime+=MTBusModule->TBusConfirm.ResponseTime;

switch(MTBusModule->Tree.Command)
  {
  case _cmdGetVarList:
  case _cmdReadRAM_EXT:
   //MTBusModule->TBusConfirm.Problem[MTBusModule->BadVarIdx].BadVar=MTBusModule->Tree.IdxRAS;
   //MTBusModule->TBusConfirm.Problem[MTBusModule->BadVarIdx].VarErr=MTBusModule->TBusConfirm.Status;
   //MTBusModule->BadVarIdx++;
   //if(MTBusModule->BadVarIdx>=10)
   //	  MTBusModule->BadVarIdx=0;
 //   if(0==LastTBusReq.VarListCnt)
   //	  {
	  MTBusModule->LastTBusReq.ResponseTime=0xFFFF;
	  VarStatus=READY_VAR;
   //   }
  break;

  case _cmdId:
  case _cmdSetAddr:
  case _cmdReadRAM:
  case _cmdReadEEPROM:
  case _cmdReadFlash:
  
  case _cmdWriteRAM:
  case _cmdWriteFlash:
  case _cmdWriteEEPROM:
  case _cmdWriteRAM_EXT:
  case _cmdDataTransfer:
  case _cmdGetArch:       
    MTBusModule->ArchConfirm.ArchStatus = ARCH_NOT_RESP_STATUS;
    MTBusModule->LastTBusReq.ResponseTime=0xFFFF;
    VarStatus=READY_VAR;
  break;

  default:break;
  }
break;//MTB_NO_RESPONSE_STATUS

default:break;//MTBusModule->TBusConf.Status

}

if(VarStatus == ARCH_PACKET_RECEIVED)
  {
  MTBusModule->ArchConfirm.ArchDevAddr = MTBusModule->Tree.DevAddr;
  
  if(ARCH_CELL_FORMAT_SIGN==MTBusModule->Tree.ArchSign)
    MTBusModule->ArchConfirm.ArchReqType = ARCH_GET_CELL_FORMAT_REQ_TYPE;
  else
    MTBusModule->ArchConfirm.ArchReqType = ARCH_GET_RECORD_REQ_TYPE;
    
  if(ARCH_GET_RECORD_REQ_TYPE == MTBusModule->ArchConfirm.ArchReqType)
    {
    MTBusModule->ArchConfirm.ArchPeriod   = MTBusModule->Tree.ArchPeriod;
    MTBusModule->ArchConfirm.ArchRecSize  = MTBusModule->Tree.ArchRecSize;
    MTBusModule->ArchConfirm.pBuff        = &MTBusModule->Tree.data[0];
    
	uint32_t n;
    for( n=0;n!=MTBusModule->Tree.ArchRecCount;n++)
      {
      MTBusModule->ArchConfirm.ArchDay      = MTBusModule->Tree.ArchDay;
      MTBusModule->ArchConfirm.ArchHalfSec  = MTBusModule->Tree.ArchHalfSec;
      //MTBusModule->ArchConfirm.ArchRecCount   = MTBusModule->Tree.ArchRecCount;
      MTBusModule->ArchConfirmCallBack(&MTBusModule->ArchConfirm);
       MTBusModule->ArchConfirm.pBuff+=MTBusModule->ArchConfirm.ArchRecSize;
  
      if((MTBusModule->Tree.ArchHalfSec+MTBusModule->Tree.ArchPeriod)>=172800)
        {
        MTBusModule->Tree.ArchDay++;
        MTBusModule->Tree.ArchHalfSec = MTBusModule->Tree.ArchHalfSec+MTBusModule->Tree.ArchPeriod - 172800;
        }
      else
        {
        MTBusModule->Tree.ArchHalfSec+=MTBusModule->Tree.ArchPeriod;
        }
      }
    }
   else if(ARCH_GET_CELL_FORMAT_REQ_TYPE == MTBusModule->ArchConfirm.ArchReqType)
    {
    MTBusModule->ArchConfirm.ArchDay	    = MTBusModule->Tree.ArchDay;
    MTBusModule->ArchConfirm.ArchHalfSec    = MTBusModule->Tree.ArchHalfSec;
    
    MTBusModule->ArchConfirm.ArchCellNumber = MTBusModule->Tree.ArchCellNumber;
    MTBusModule->ArchConfirm.ArchCellCnt    = MTBusModule->Tree.ArchCellCnt;
    MTBusModule->ArchConfirm.pBuff	    = &MTBusModule->Tree.data[0];
    
    MTBusModule->ArchConfirmCallBack(&MTBusModule->ArchConfirm);
    }
  
  }

else if(VarStatus==READY_VAR)//Îáðàáîò÷èê ãîòîâ ïåðåäàòü îò÷åò âåðõíåìó óðîâíþ
  {
  MTBusModule->TBusConfirm.InitiatorAddr  = MTBusModule->LastTBusReq.InitiatorAddr;
  MTBusModule->TBusConfirm.DevAddr        = MTBusModule->Tree.DevAddr;
  MTBusModule->TBusConfirm.Command        = MTBusModule->Tree.Command;
  MTBusModule->TBusConfirm.OldDevAddr     = MTBusModule->Tree.OldDevAddr;
  MTBusModule->TBusConfirm.VarAddr        = MTBusModule->Tree.VarAddr;
  MTBusModule->TBusConfirm.VarType        = MTBusModule->Tree.VarType;
  MTBusModule->TBusConfirm.VarCnt         = MTBusModule->Tree.VarCnt;
  MTBusModule->TBusConfirm.data           = MTBusModule->Tree.data;
  MTBusModule->TBusConfirm.IdxRAS         = MTBusModule->Tree.IdxRAS;
  MTBusModule->TBusConfirm.CntRAS         = MTBusModule->Tree.CntRAS;
  MTBusModule->TBusConfirm.IdxMAS         = MTBusModule->Tree.CntRAS;
  MTBusModule->TBusConfirm.DataCnt        = MTBusModule->Tree.DataCnt;
  MTBusModule->TBusConfirm.Reserved       = MTBusModule->Tree.Reserved;
  MTBusModule->TBusConfirm.DTState        = MTBusModule->Tree.DTState;
  MTBusModule->TBusConfirm.FragCnt        = MTBusModule->Tree.FragCnt;
  MTBusModule->TBusConfirm.FragIdx        = MTBusModule->Tree.FragIdx;
  MTBusModule->TBusConfirm.ProtType       = MTBusModule->Tree.ProtType;
  MTBusModule->TBusConfirm.ArchDay	  = MTBusModule->Tree.ArchDay;
  MTBusModule->TBusConfirm.ArchHalfSec	  = MTBusModule->Tree.ArchHalfSec;
  MTBusModule->TBusConfirm.ArchRecCount	  = MTBusModule->Tree.ArchRecCount;
  MTBusModule->TBusConfirm.ArchCellNumber = MTBusModule->Tree.ArchCellNumber;
  MTBusModule->TBusConfirm.ArchCellCnt	  = MTBusModule->Tree.ArchCellCnt;
  MTBusModule->TBusConfirm.ArchRecSize	  = MTBusModule->Tree.ArchRecSize;
  MTBusModule->TBusConfirm.ArchReqType	  = MTBusModule->ArchConfirm.ArchReqType;
  MTBusModule->TBusConfirm.ArchPeriod	  = MTBusModule->Tree.ArchPeriod;


  //!!!!!!!!!!!!!!!!!!!!!!!Äîáàâèòü/èñïðàâèòü óïðàâëåíèå ñòàòóñàìè

  switch(MTBusModule->TBusConfirm.Command)
  {
  case  _cmdDataTransfer:
  if(MTBusModule->Succes)
	MTBusModule->TBusConfirm.Status = MTB_SUCCESS_STATUS;
  else    
	MTBusModule->TBusConfirm.Status = MTB_NO_RESPONSE_STATUS;
  break;

 // case  _cmdGetArch:
  //MTBusModule->TBusConfirm.Status = MTB_SUCCESS_STATUS;

//  if(MTBusModule->Tree.error ==_errCHS)
//	MTBusModule->TBusConfirm.Status = MTB_CHS_ERROR_STATUS;
//  else if(MTBusModule->Succes)
//	MTBusModule->TBusConfirm.Status = MTB_SUCCESS_STATUS;
//  else
//	MTBusModule->TBusConfirm.Status = MTB_NO_RESPONSE_STATUS;
//  break;


  default:
	if((MTBusModule->Problem==0)&&(MTBusModule->Succes==1))        //Åñëè íå áûëî ïðîáëåì è ïîëó÷åííûå îòâåòû óäà÷íûå
      MTBusModule->TBusConfirm.Status=MTB_SUCCESS_STATUS;
    else if((MTBusModule->Problem==1)&&(MTBusModule->Succes==0))   //Åñëè áûëè ïðîáëåìû, è íå áûëî óäà÷íûõ îòâåòîâ
      MTBusModule->TBusConfirm.Status=MTB_BAD_STATUS;
    else if((MTBusModule->Problem==1)&&(MTBusModule->Succes==1))   //Åñëè áûëè ïðîáëåìû, íî åñòü è óäà÷íûå îòâåòû 
      MTBusModule->TBusConfirm.Status=MTB_PART_SUCCESS_STATUS;
    else
      {
      MTBusModule->TBusConfirm.Status=MTB_PART_SUCCESS_STATUS;
      }    
    break;
  }
  
  MTBusModule->Problem=0;
  MTBusModule->Succes=0;
  if(MTBusModule->ReqConf!=(void *)0)
    {
    MTBusModule->ReqConf(&MTBusModule->TBusConfirm);
    }
  
  uint8_t bv;
  for(bv=0;bv!=10;bv++)
    {
    MTBusModule->TBusConfirm.Problem[bv].BadVar=0;
    MTBusModule->TBusConfirm.Problem[bv].VarErr=MTB_FREE_RUN_STATUS;
    }
  MTBusModule->BadVarIdx=0;
  MTBusModule->TBusConfirm.ResponseTime=0;
  }
}


void MTBusTimeOutCounter(MTBusModule_t *MTBusModule)
{
MTBusModule->FlowAnaliserTimeOutCounter++;
if(MTBusModule->FlowAnaliserTimeOutCounter>=MTBusModule->FlowAnaliserTimeOut)
  {
  MTBusModule->FlowAnaliserTimeOutCounter=0;
  MTBusModule->FlowAnaliser.State = _SIGNATURE_ANALISE_STATE;
  }     
  
if(MTBusModule->LastTBusReq.ResponseTime!=0xFFFF)
  {
   if(MTBusModule->TimeOut_++>MTBusModule->LastTBusReq.ResponseTime)
    {
    MTBusModule->Tree.DevAddr               = MTBusModule->LastTBusReq.DevAddr;
    MTBusModule->Tree.Command               = MTBusModule->LastTBusReq.Command;
    MTBusModule->Tree.VarAddr               = MTBusModule->LastTBusReq.VarAddr;
    MTBusModule->Tree.VarCnt                = MTBusModule->LastTBusReq.VarCnt;
    MTBusModule->Tree.IdxRAS                = MTBusModule->LastTBusReq.IdxRAS;
    MTBusModule->Tree.CntRAS                = MTBusModule->LastTBusReq.CntRAS;

    MTBusModule->ArchConfirm.ArchDay        = MTBusModule->LastTBusReq.ArchDay;
    MTBusModule->ArchConfirm.ArchHalfSec    = MTBusModule->LastTBusReq.ArchHalfSec;
    MTBusModule->ArchConfirm.ArchCellNumber = MTBusModule->LastTBusReq.ArchCellNumber;
    MTBusModule->ArchConfirm.ArchCellCnt    = MTBusModule->LastTBusReq.ArchCellCnt;

    MTBusModule->TBusConfirm.ResponseTime   = MTBusModule->TimeOut_;
    MTBusModule->TBusConfirm.Status         = MTB_NO_RESPONSE_STATUS;
    MTBusModule->TimeOut_=0;
    }
  }
}


