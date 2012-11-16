#include "../inc/includes.h"

volatile uint32_t DTransferTime;

static uint8_t ProtocolVersion     = 0x04;//Âåðñèÿ ïðîòîêîëà
static DTConf_t DT_Confifm;

static uint8_t SizeOfType(uint8_t type)//Ïîäïðîãðàììà âû÷èñåíèÿ ðàçìåðà ïåðåìåííîé ïî åå òèïó
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


void TBusInit(TBusSlave_t *TBusModule)
{

}

void TBusDataReceiver(TBusSlave_t *TBusModule,uint8_t byte)//Ïðèåìíèê áàéò âõîäíîãî ïîòîêà äàííûõ
{
TBusModule->RxBuff[TBusModule->RxLast]=byte;
TBusModule->RxLast++;
if(TBusModule->RxLast>=(TBusModule->RxBuffSize-1))
  TBusModule->RxLast=0;
}


static ParcerState_t TbState=SIGNATURE_ANALISE_STATE;//Ñîñòîÿíè ïàðñåðà

//Àíàëèçàòîð âõîäíîãî ïîòîêà áàéò
void TBusFlowAnaliser(TBusSlave_t *TBusModule,uint8_t byte)
{
	uint8_t idx;
TBusModule->Time=0;

if(TbState!=CHS_ANALISE_STATE)
  CHS_16(&TBusModule->crc16,byte,calcCHS);

switch(TbState)
  {
  case SIGNATURE_ANALISE_STATE:
  if(byte==0x50)
    {
    TBusModule->Tree.IdxRAS=0x0000;
    TBusModule->Tree.IdxMAS=0x0000;
    TBusModule->Tree.CntRAS=0x0000;
    TBusModule->pReceiveCHS=(uint8_t *)&TBusModule->Tree.chs;
    TBusModule->pReceiveCHS++;
    TBusModule->Tree.error=errNone;
    TBusModule->Tree.DataLen=0;
    CHS_16(&TBusModule->crc16,0,resetCHS);
    TbState=DEV_ADDR_ANALISE_STATE;
    }
  break;

   case DEV_ADDR_ANALISE_STATE:
     TbState=SIGNATURE_ANALISE_STATE;
    
    if((TBusModule->NetworkDeviceAddr==byte)||(0xFF==byte))//Åñëè îáðàùàþòñÿ ê ìàñòåðó èëè êî âñåì
      {
      if(0xFF==byte)//Åñëè ñïðàøèâàþò âñåõ  
        TBusModule->Tree.DevAddr=TBusModule->NetworkDeviceAddr;//Âñòàâèòü ñâîé àäðåñ
      else
        TBusModule->Tree.DevAddr=byte;
      TbState=COMMAND_ANALISE_STATE;
      }
#ifdef MASTER_OPERATION_ENABLE    
    else
      {
      for(idx=0;idx!=MaxDevices;idx++)
        {
        if((byte==MDevId[idx].DevAddr)&&(MDevId[idx].Online))//Åñëè îáðàùåíèå ê óñòðîéñòâó â ñåòè ìàñòåðà
          {
          TBusModule->Tree.DevAddr=byte;
          TbState=COMMAND_ANALISE_STATE;
          break;
          }
        }
        }
#endif    
  break;


  case NEW_DEV_ADDR_RECEIVE_STATE:
  TBusModule->Tree.NewDevAddr=byte;
  TbState=CHS_ANALISE_STATE;
  TBusModule->inc=0;
  break;

  case COMMAND_ANALISE_STATE:
  TBusModule->Tree.Command=(TeraBusCmd_t )byte;//Çàïèñàòü ïðèíÿòóþ êîììàíäó â áóôôåðíîå äåðåâî ðàçáîðà

  switch(TBusModule->Tree.Command)
      {
      case cmdReadRAM:
      case cmdWriteRAM:
      case cmdReadFlash:
      case cmdWriteFlash:
      case cmdReadEEPROM:
      case cmdWriteEEPROM:

      TbState=VAR_ADDR_RECEIVE_STATE;
      TBusModule->pReceive=(uint8_t *)&TBusModule->Tree.VarAddr;
      TBusModule->inc=0;
      break;

      case cmdId:
      //case cmdArchParams:
      TbState=CHS_ANALISE_STATE;
      TBusModule->inc=0;
      break;

      case cmdSetAddr:
      TbState=NEW_DEV_ADDR_RECEIVE_STATE;
      break;

      case cmdGetArch:
      TbState = ARCH_SIGN_RECEIVE_STATE; 
      break;  
      
//      case cmdArchData:
//      TbState=ARCH_REC_NO_RECEIVE_STATE;
//      TBusModule->pReceive=(uint8_t *)&TBusModule->Tree.ArchiveRecNo;
//      TBusModule->inc=0;
//      break;

      case cmdGetVarList:
      case cmdReadRAM_EXT:
      case cmdWriteRAM_EXT:
      TbState=IDX_RAS_RECEIVE_STATE;
      TBusModule->pReceive=(uint8_t *)&TBusModule->Tree.IdxRAS;
      TBusModule->inc=0;
      break;

      case cmdDataTransfer:
      TbState=RESERVED_RECEIVE_STATE;
      break;  
      
      default:
      TbState=SIGNATURE_ANALISE_STATE;
      TBusModule->Tree.error=errUnknownCmd;
      TBusModule->Tree.Signature=0x50;
      break;
      };

  break;

  case IDX_RAS_RECEIVE_STATE:
    *TBusModule->pReceive++=byte;
     if(TBusModule->inc++>=1)
      {
      if(cmdWriteRAM_EXT==TBusModule->Tree.Command)
        {
        TbState=IDX_MAS_RECEIVE_STATE;
        TBusModule->pReceive=(uint8_t *)&TBusModule->Tree.IdxMAS;
        }
      else
        {
        TbState=CNT_RAS_RECEIVE_STATE;
        TBusModule->pReceive=(uint8_t *)&TBusModule->Tree.CntRAS;
        }
      TBusModule->inc=0;
      }
  break;

  case CNT_RAS_RECEIVE_STATE:
     *TBusModule->pReceive++=byte;
     if(TBusModule->inc++>=1)
      {
      TbState=CHS_ANALISE_STATE;
      TBusModule->inc=0;
      }
   //TBusModule->Tree.CntRAS=byte;
   //TbState=CHS_ANALISE_STATE;
   //inc=0;
  break;

  case IDX_MAS_RECEIVE_STATE:
   *TBusModule->pReceive++=byte;
     if(TBusModule->inc++>=1)
      {
      TbState=VAR_CNT_RECEIVE_STATE;
      TBusModule->inc=0;
      }
  break;


  case VAR_ADDR_RECEIVE_STATE:
  *TBusModule->pReceive++=byte;

  if(TBusModule->inc++>=3)
    {
    TbState=VAR_CNT_RECEIVE_STATE;
    }
  break;

  case VAR_CNT_RECEIVE_STATE:
  TBusModule->Tree.VarCnt=byte;
  TbState=VAR_TYPE_ANALISE_STATE;
  break;

  case VAR_TYPE_ANALISE_STATE:
  TBusModule->Tree.VarType=(VarType_t)byte;


  switch(TBusModule->Tree.Command)
    {
    case cmdReadRAM:
    case cmdReadEEPROM:
    case cmdReadFlash:
      TbState=CHS_ANALISE_STATE;
    break;

    case cmdWriteRAM:
    case cmdWriteRAM_EXT:
      TbState=DATA_RECEIVE_STATE;
    break;

    default: break;
    }
  TBusModule->inc=0;

  TBusModule->Tree.VarSize=SizeOfType(TBusModule->Tree.VarType);
  TBusModule->Tree.DataLen=TBusModule->Tree.VarCnt*TBusModule->Tree.VarSize;
  if(0==TBusModule->Tree.DataLen)
    {
    TbState=SIGNATURE_ANALISE_STATE;
    TBusModule->Tree.error=errInvalidType;
    TBusModule->Tree.Signature=0x50;
    }
  break;

  case DATA_RECEIVE_STATE:
  TBusModule->Tree.data[TBusModule->inc]=byte;
  if(TBusModule->inc++>=TBusModule->Tree.DataLen-1)
    {
    TBusModule->inc=0;
    TbState=CHS_ANALISE_STATE;
    }
  break;

  case ARCH_SIGN_RECEIVE_STATE:
    TBusModule->Tree.ArchSign = (ArchSignType_t)byte;
    TBusModule->pReceive=(uint8_t *)&TBusModule->Tree.ArchDay;
    TBusModule->inc=0;
    TbState = ARCH_DAY_RECEIVE_STATE;
  break;  
    
  
  case ARCH_DAY_RECEIVE_STATE:
  *TBusModule->pReceive++=byte;
   if(TBusModule->inc++>=1)
    {
    TbState=ARCH_HALF_SEC_RECEIVE_STATE;
    TBusModule->pReceive=(uint8_t *)&TBusModule->Tree.ArchHalfSec;    
    TBusModule->inc=0;
    }  
  break;  
  
  case ARCH_HALF_SEC_RECEIVE_STATE:
   *TBusModule->pReceive++=byte;
   if(TBusModule->inc++>=3)
    {
    if(ARCH_CELL_FORMAT_SIGN_==TBusModule->Tree.ArchSign)  
      {
      TbState=ARCH_CELL_NUMBER_RECEIVE_STATE;
      TBusModule->pReceive=(uint8_t *)&TBusModule->Tree.ArchCellNumber; 
      }     
    else
      {
      TbState=ARCH_REC_COUNT_RECEIVE_STATE;
      TBusModule->pReceive=(uint8_t *)&TBusModule->Tree.ArchRecCount; 
      }  
   
   
    TBusModule->inc=0;
    }
  break;  
  
  case ARCH_REC_COUNT_RECEIVE_STATE:
   *TBusModule->pReceive++=byte;
   if(TBusModule->inc++>=3)
    {
    TbState = CHS_ANALISE_STATE;  
    TBusModule->inc=0;
    } 
  break; 
  
  case ARCH_CELL_NUMBER_RECEIVE_STATE:
  *TBusModule->pReceive++=byte;
   if(TBusModule->inc++>=1)
    {
    TBusModule->pReceive=(uint8_t *)&TBusModule->Tree.ArchCellCnt;   
    TbState = ARCH_CELL_CNT_RECEIVE_STATE;  
    TBusModule->inc=0;
    } 
  break;  
  
  case ARCH_CELL_CNT_RECEIVE_STATE:
   *TBusModule->pReceive++=byte;
   if(TBusModule->inc++>=1)
    {
    TbState = CHS_ANALISE_STATE;  
    TBusModule->inc=0;
    } 
  break; 
    
  case RESERVED_RECEIVE_STATE:
    TBusModule->Tree.Reserved = byte;  
    TbState       = STATE_RECEIVE_STATE;
  break;  
  
  case STATE_RECEIVE_STATE:
    TBusModule->Tree.State  = (DTState_t)byte;  
    TbState     = FRAG_CNT_IDX_RECEIVE_STATE;  
  break;  
 
  case FRAG_CNT_IDX_RECEIVE_STATE:
    TBusModule->Tree.FragCnt  = byte>>4;
    TBusModule->Tree.FragIdx  = 0x0F&byte;
    TbState       = PROT_TYPE_RECEIVE_STATE;
  break;  
 
  
  
  case PROT_TYPE_RECEIVE_STATE:
    TBusModule->Tree.ProtType=(ProtType_t)byte;  
    TbState = DATA_CNT_RECEIVE_STATE;
  break;  
  
  case DATA_CNT_RECEIVE_STATE:
    TBusModule->Tree.DataLen=byte;
    TbState=DATA_RECEIVE_STATE;
    TBusModule->inc=0;    
  break;  
  
  case CHS_ANALISE_STATE:
  *TBusModule->pReceiveCHS--=byte;
  if(TBusModule->inc++>=1)
    {
    TbState=SIGNATURE_ANALISE_STATE;
    if(TBusModule->crc16==TBusModule->Tree.chs)
      {
      TBusModule->Tree.Signature=0x50;
      }
    else
      {
      TBusModule->Tree.error=errCHS;
      TBusModule->Tree.Signature=0x50;
      }
    }
  break;

  default:break;
  };
}


//void DTRxEvent(DTConf_t *Conf)//Äàííûå ïðèøëè ýòîìó óñòðîéñòâó
//{
// 
//asm("nop");  
//  
//Conf->Reserved=0xCA; 
//Conf->DTState=Answer;
//Conf->FragCnt =0x01;
//Conf->FragIdx =0x02;
//
//Conf->ProtType=BOOT_PROTOCOL;
//Conf->pData="abcdefgh";
//Conf->DataCnt=8;
//}


void TbusTrDataReceive(DTConf_t *DTConf)//Äàííûå ïðèøëè èç âåäîìîãî ïðèáîðà â ñåòè
{
DTransferTime = 0;  
DT_Confifm.DevAddr  = DTConf->DevAddr;
DT_Confifm.pData    = DTConf->pData;
DT_Confifm.DataCnt  = DTConf->DataCnt;
DT_Confifm.Reserved = DTConf->Reserved;
DT_Confifm.DTState  = DTConf->DTState;
DT_Confifm.FragCnt  = DTConf->FragCnt;
DT_Confifm.FragIdx  = DTConf->FragIdx;
DT_Confifm.ProtType = DTConf->ProtType;
DT_Confifm.State    = DT_SLAVE_RESPONSE_COMPLETED;
}

//Ñáîðùèê ïàêåòîâ äëÿ îòïðàâêè
void TBusRun(TBusSlave_t *TBusModule)
{
uint8_t       *pBuff;
uint8_t       *pSource;    
uint16_t      TxLen;      //Äëèíà ïåðåäàþùåéñÿ ïîñûëêè
uint16_t      CHS;        //Êîíòðîëüíàÿ ñóììà
uint8_t       dataIdx;
uint16_t      RasCnt = 0;
DTConf_t      DTConf;

uint16_t      i;
uint16_t      inc;
uint8_t       j;

#ifdef MASTER_OPERATION_ENABLE   
TRData_t      TRData;
#endif


while((TBusModule->RxFirst!=TBusModule->RxLast)&&(0==TBusModule->Tree.Signature))//Àíàëèç âõîäíîãî ïîòîêà áàéò
  {
  TBusFlowAnaliser(TBusModule,TBusModule->RxBuff[TBusModule->RxFirst++]);
  if(TBusModule->RxFirst>=(TBusModule->RxBuffSize-1))
    TBusModule->RxFirst=0;
  }



if(0x50==TBusModule->Tree.Signature)//Äåðåâî ðàçáîðà áûëî ñôîðìèðîâàíî ïîäïðîãðàììîé TBusFlowAnaliser
{
  if(errNone==TBusModule->Tree.error)//Åñëè çàïðîñ èäåíòèôèöèðîâàí áåç îøèáîê 
    {
    switch(TBusModule->Tree.Command)
    {
     case cmdId://Êîììàíäà èäåíòèôèêàöèè
    for( i=0;i!=TBusModule->TxBuffSize;i++)//Î÷èñòêà áóôåðà
      TBusModule->TxBuff[i]=0;
      
    TxLen=37; //Äëÿ ýòîé êîììàíäû äëèíà îòâåòà ôèêñèðîâàííàÿ
     pBuff=TBusModule->TxBuff;
    *pBuff++=TBusModule->Tree.Signature;
    *pBuff++=TBusModule->Tree.DevAddr;
    *pBuff++=TBusModule->Tree.Command;  
    if(TBusModule->NetworkDeviceAddr==TBusModule->Tree.DevAddr)//Åñëè ñïðàøèâàþò ìàñòåðà
      pSource=(uint8_t *)TBusModule->DevName;
#ifdef MASTER_OPERATION_ENABLE     
    else    //Åñëè ñïðàøèâàþò óñòðîéñòâî èç ñåòè ìàñòåðà
      {
      for(inc=0;inc!=MaxDevices;inc++)
        {
        if(TBusModule->Tree.DevAddr==MDevId[inc].DevAddr)
          {
          pSource=&MDevId[inc].DevName[0];
          break;
          }
        }    
      }
#endif
    
    TBusModule->Tree.Signature=0x00;//Ñáðîñ ïðèçíàêà çàêîí÷åííîãî ðàçáîðà
    for( i=0;i!=20;i++)
      *pBuff++=*pSource++;
     pBuff++;//Ðåçåðâíûé áàéò â íîâîé âåðñèè ïðîòîêîëà
    *pBuff++=ProtocolVersion;
   
    
    
    if(TBusModule->NetworkDeviceAddr==TBusModule->Tree.DevAddr)//Åñëè ñïðàøèâàþò ìàñòåðà
      pSource=(uint8_t *)TBusModule->ProgrammVersion;

#ifdef MASTER_OPERATION_ENABLE
    else    //Åñëè ñïðàøèâàþò óñòðîéñòâî èç ñåòè ìàñòåðà
      {
      for( inc=0;inc!=MaxDevices;inc++)
        {
        if(TBusModule->Tree.DevAddr==MDevId[inc].DevAddr)
          {
          pSource=&MDevId[inc].ProgVer[0];
          break;
          }
        }    
      }
#endif    
          
    for( i=0;i!=10;i++)
      *pBuff++=*pSource++;
    CHS=CALC_CHS_16(&TBusModule->TxBuff[1],TxLen-3);
    *pBuff++=(uint8_t)(CHS>>0x08);
    *pBuff++=(uint8_t)(CHS);
    TBusModule->SenderFunct(TBusModule->TxBuff,TxLen);//Ïîñëàòü â UART
    break;   
    
    case cmdSetAddr:
    TxLen=6; //Äëÿ ýòîé êîììàíäû äëèíà îòâåòà ôèêñèðîâàííàÿ
     pBuff=TBusModule->TxBuff;
    *pBuff++=TBusModule->Tree.Signature;
    *pBuff++=TBusModule->Tree.NewDevAddr;
    *pBuff++=TBusModule->Tree.Command;  
    *pBuff++=TBusModule->Tree.DevAddr;  
    TBusModule->Tree.Signature=0x00;
    CHS=CALC_CHS_16(&TBusModule->TxBuff[1],TxLen-3);
    *pBuff++=(uint8_t)(CHS>>0x08);
    *pBuff++=(uint8_t)(CHS);
    //Âñòàâèòü ïîäïðîãðàììó çàïèñè íîâîãî àäðåñà
    TBusModule->NetworkDeviceAddr=TBusModule->Tree.NewDevAddr;
    TBusModule->SenderFunct(TBusModule->TxBuff,TxLen);//Ïîñëàòü â UART
    break;  
    
    
#ifdef _connect_h    
    case cmdGetVarList: //Êîììàíäà âû÷èòûâàíèÿ êàðòû ïàìÿòè
    case cmdReadRAM_EXT://Êîììàíäà ðàñøèðåííîãî ÷òåíèÿ ÎÇÓ
   
    if(TBusModule->Tree.DevAddr==TBusModule->NetworkDeviceAddr)//Åñëè îáðàòèëèñü ê âåäóùåìó
      RasCnt=RAS_Count;

#ifdef MASTER_OPERATION_ENABLE
    else  
      {
      RasCnt=0;  
      for( inc=0;inc!=MRAS_COUNT;inc++)
        {
        if(TBusModule->Tree.DevAddr==MRAS[inc].DevAddr)
          RasCnt++;  
        }
      }
#endif
    
    if((0xFFFF==TBusModule->Tree.IdxRAS)&&(0xFFFF==TBusModule->Tree.CntRAS))
      {                   //Åñëè çàïðîñèëè êîëëè÷åñòâî ñòðîê ñòðóêòóðû RAS
      TxLen=9;
      for( i=0;i!=TBusModule->TxBuffSize;i++)//Î÷èñòêà áóôåðà
        TBusModule->TxBuff[i]=0;
       pBuff=TBusModule->TxBuff;
      *pBuff++=TBusModule->Tree.Signature;
      *pBuff++=TBusModule->Tree.DevAddr;
      *pBuff++=TBusModule->Tree.Command;    
      *pBuff++=(uint8_t)TBusModule->Tree.IdxRAS;
      *pBuff++=(uint8_t)(TBusModule->Tree.IdxRAS>>0x08);
      *pBuff++=(uint8_t)RasCnt;
      *pBuff++=(uint8_t)(RasCnt>>0x08);      
      CHS=CALC_CHS_16(&TBusModule->TxBuff[1],TxLen-3);
      *pBuff++=(uint8_t)(CHS>>0x08);
      *pBuff++=(uint8_t)(CHS);
      TBusModule->Tree.Signature=0x00;
      TBusModule->SenderFunct(TBusModule->TxBuff,TxLen);//Ïîñëàòü â UART    
      }
     else if((0!=TBusModule->Tree.CntRAS)&&
               (0==TBusModule->GetPortStatusFunct())) // 0!=TBusModule->Tree.CntRAS è îòïðàâùèê ñâîáîäåí
      {//Âû÷èòûâàíèå è ïåðåäà÷à ñòðóêòóðû RAS  
      
      for( i=0;i!=TBusModule->TxBuffSize;i++)//Î÷èñòêà áóôåðà
        TBusModule->TxBuff[i]=0;
   
       pBuff=TBusModule->TxBuff;
      *pBuff++=TBusModule->Tree.Signature;
      *pBuff++=TBusModule->Tree.DevAddr;
      *pBuff++=TBusModule->Tree.Command;       
       //UsartManagerSatus=UsartManagerBusy;
      if(RasCnt<=TBusModule->Tree.IdxRAS)
        {
        TxLen=9;  
        *pBuff++=0xFF;  
        *pBuff++=0xFF;
        *pBuff++=0xFF;
        *pBuff++=0xFF;
        TBusModule->Tree.Signature=0x00;
        }
      else
        {
        *pBuff++=(uint8_t)TBusModule->Tree.IdxRAS;
        *pBuff++=(uint8_t)(TBusModule->Tree.IdxRAS>>0x08);
        *pBuff++=0x01;
        *pBuff++=0x00;
        
        if(TBusModule->Tree.DevAddr==TBusModule->NetworkDeviceAddr)//Åñëè îáðàòèëèñü ê ìàñòåðó
          {
          *pBuff++=RAS[TBusModule->Tree.IdxRAS].vCount; 
          *pBuff++=RAS[TBusModule->Tree.IdxRAS].vType;  
          }
#ifdef MASTER_OPERATION_ENABLE
        else    //Åñëè îáðàòèëèñü ê óñòðîéñòâó â ñåòè ìàñòðà
          {
          for( inc=0;inc!=MRAS_COUNT;inc++)
            {
            if((TBusModule->Tree.DevAddr==MRAS[inc].DevAddr)&&(TBusModule->Tree.IdxRAS==MRAS[inc].vIdx))
              {
              TBusModule->MRasIdx=inc;
              *pBuff++=MRAS[TBusModule->MRasIdx].vCnt; 
              *pBuff++=MRAS[TBusModule->MRasIdx].vType;   
              break;
              }
            }
          }
#endif        
        if(cmdGetVarList==TBusModule->Tree.Command)
          {
          TxLen=30;
       
          if(TBusModule->Tree.DevAddr==TBusModule->NetworkDeviceAddr)//Åñëè îáðàòèëèñü ê ìàñòåðó
            {
            if(RAS[TBusModule->Tree.IdxRAS].func)
              *pBuff++=0x01;
            else
              *pBuff++=0x00;        
            pSource=(uint8_t *)RAS[TBusModule->Tree.IdxRAS].vInfo;
            }         
#ifdef MASTER_OPERATION_ENABLE
          else  //Åñëè îáðàòèëèñü ê óñòðîéñòâó â ñåòè ìàñòðà
            {
            if(MRAS[TBusModule->MRasIdx].vWRE)
              *pBuff++=0x01;
            else
              *pBuff++=0x00;        
            pSource=(uint8_t *)MRAS[TBusModule->MRasIdx].vInfo;
            }
#endif          
          for( i=0;i!=18;i++)
            *pBuff++=*pSource++;      
          }
        else if(cmdReadRAM_EXT==TBusModule->Tree.Command) 
          {
          if(TBusModule->Tree.DevAddr==TBusModule->NetworkDeviceAddr)//Åñëè îáðàòèëèñü ê âåäóùåìó
            {
            TBusModule->Tree.DataLen=RAS[(uint8_t)TBusModule->Tree.IdxRAS].vCount*SizeOfType(RAS[(uint8_t)TBusModule->Tree.IdxRAS].vType);
            TxLen=11+TBusModule->Tree.DataLen;
            pSource=(uint8_t *)((CPU_BUS_LEN *)RAS[(uint8_t)TBusModule->Tree.IdxRAS].Addr);
            }
#ifdef MASTER_OPERATION_ENABLE    
          else
            {
            TBusModule->Tree.DataLen=MRAS[TBusModule->MRasIdx].vCnt*SizeOfType(MRAS[TBusModule->MRasIdx].vType);
            TxLen=11+TBusModule->Tree.DataLen;
            pSource=MRAS[TBusModule->MRasIdx].pData;            
            }
#endif          
          for( i=0;i!=TBusModule->Tree.DataLen;i++)
            *pBuff++=*pSource++;  
          
          asm("nop");
          }
        }
      CHS=CALC_CHS_16(&TBusModule->TxBuff[1],TxLen-3);
      *pBuff++=(uint8_t)(CHS>>0x08);
      *pBuff++=(uint8_t)(CHS);
     
      TBusModule->Tree.CntRAS--;
      TBusModule->Tree.IdxRAS++;
              
      if(0==TBusModule->Tree.CntRAS)
        TBusModule->Tree.Signature=0x00;
      TBusModule->SenderFunct(TBusModule->TxBuff,TxLen);//Ïîñëàòü â UART  
       }
    break;
#endif //_connect_h   

    
    case cmdWriteRAM_EXT://Êîììàíäà ðàñøèðåííîé çàïèñè â RAM
    TxLen=11;//Äëèíà îòâåòà â áàéòàõ (äëÿ ýòîé êîììàíäû ôèêñèðîâàííàÿ)
    pBuff=TBusModule->TxBuff;
    *pBuff++=TBusModule->Tree.Signature;
    *pBuff++=TBusModule->Tree.DevAddr;
    *pBuff++=TBusModule->Tree.Command;    
    TBusModule->Tree.Signature=0x00;
  
    if(TBusModule->Tree.DevAddr==TBusModule->NetworkDeviceAddr)
      RasCnt=RAS_Count;
#ifdef MASTER_OPERATION_ENABLE
    else
      {
      RasCnt=0;  
      for( inc=0;inc!=MRAS_COUNT;inc++)
        {
        if(TBusModule->Tree.DevAddr==MRAS[inc].DevAddr)
          RasCnt++;  
        }
      }  
    for( inc=0;inc!=MRAS_COUNT;inc++)
      {
      if((TBusModule->Tree.DevAddr==MRAS[inc].DevAddr)&&(TBusModule->Tree.IdxRAS==MRAS[inc].vIdx))
        {
        TBusModule->MRasIdx=inc;
        break;
        }
      }
#endif    
    if(TBusModule->Tree.IdxRAS>=RasCnt)//Åñëè îáàòèëèñü ê íåñóùåñòâóþùåé çàïèñè
      {
      for( i=0x00;i!=0x06;i++)
        *pBuff++=0xFF;
      TxLen=0;  //Èñïîëüçîâàíèå TxLen=0, êàê ïðèçíàêà ïðîáëåìì
      }
    else
      {
      *pBuff++=(uint8_t)TBusModule->Tree.IdxRAS;
      *pBuff++=(uint8_t)(TBusModule->Tree.IdxRAS>>0x08);
      if(TBusModule->Tree.DevAddr==TBusModule->NetworkDeviceAddr)//Åñëè îáðàòèëèñü ê ìàñòåðó
        {
        if((TBusModule->Tree.IdxMAS>=RAS[TBusModule->Tree.IdxRAS].vCount))//Åñëè îáðàòèëèñü ê íåñóùåñòâóþùåìó èíäåêñó ìàññèâà
          {
          *pBuff++=0xFF;
          *pBuff++=0xFF;      
          TxLen=0;
          }
        else
          {
          *pBuff++=(uint8_t)TBusModule->Tree.IdxMAS;
          *pBuff++=(uint8_t)(TBusModule->Tree.IdxMAS>>0x08);
          }
        }
#ifdef MASTER_OPERATION_ENABLE
      else    //Åñëè îáðàòèëèñü ê óñòðîéñòâó â ñåòè âåäóùåãîâ
        {
        if((TBusModule->Tree.IdxMAS>=MRAS[TBusModule->MRasIdx].vCnt))//Åñëè îáðàòèëèñü ê íåñóùåñòâóþùåìó èíäåêñó ìàññèâà
          {
          *pBuff++=0xFF;
          *pBuff++=0xFF;      
          TxLen=0;
          }
        else
          {
          *pBuff++=(uint8_t)TBusModule->Tree.IdxMAS;
          *pBuff++=(uint8_t)(TBusModule->Tree.IdxMAS>>0x08);
          }
        }
#endif          
      if(TBusModule->Tree.DevAddr==TBusModule->NetworkDeviceAddr)//Åñëè îáðàòèëèñü ê ìàñòåðó
        {
        if((TBusModule->Tree.IdxMAS*TBusModule->Tree.VarCnt)>RAS[TBusModule->Tree.IdxRAS].vCount)//Åñëè âîçìîæåí âûõîä çà ïðåäåëû ìàññèâà â ïðîöåññå çàïèñè
          {
          TxLen=0;
          *pBuff++=0xFF;
          }
        else
          {
          *pBuff++=(uint8_t)TBusModule->Tree.VarCnt;
          }
        if(TBusModule->Tree.VarType!=RAS[TBusModule->Tree.IdxRAS].vType)//Åñëè òèï íå ñîâïàäàåò
          {
          *pBuff++=0xFF;
          TxLen=0;
          }    
        else
          {
          *pBuff++=(uint8_t)TBusModule->Tree.VarType;
          } 
        }
#ifdef MASTER_OPERATION_ENABLE
      else  //Åñëè îáðàòèëèñü ê óñòðîéñòâó â ñåòè âåäóùåãî
        {
        if((TBusModule->Tree.IdxMAS*TBusModule->Tree.VarCnt)>MRAS[TBusModule->MRasIdx].vCnt)//Åñëè âîçìîæåí âûõîä çà ïðåäåëû ìàññèâà â ïðîöåññå çàïèñè
          {
          TxLen=0;
          *pBuff++=0xFF;
          }
        else
          {
          *pBuff++=(uint8_t)TBusModule->Tree.VarCnt;
          }
        if(TBusModule->Tree.VarType!=MRAS[TBusModule->MRasIdx].vType)//Åñëè òèï íå ñîâïàäàåò
          {
          *pBuff++=0xFF;
          TxLen=0;
          }    
        else
          {
          *pBuff++=(uint8_t)TBusModule->Tree.VarType;
          } 
      
        }
#endif      
      }
    if(0==TxLen)//Åñëè áûëè ïðîáëåìû
      {
      TxLen=11;
      }
    else
      {//Åñëè ïðîáëåì íå áûëî
      
      //--------
      if(TBusModule->Tree.DevAddr==TBusModule->NetworkDeviceAddr)//Åñëè îáðàòèëèñü ê âåäóùåìó
        {  
        if(0!=RAS[TBusModule->Tree.IdxRAS].func)
          {  
          dataIdx=0;
          for( j=0;j!=TBusModule->Tree.VarCnt;j++)
            {
            (*RAS[TBusModule->Tree.IdxRAS].func)(&TBusModule->Tree.data[dataIdx],j+TBusModule->Tree.IdxMAS);
            dataIdx+=SizeOfType(TBusModule->Tree.VarType);
            }  
          }
        else
          {
          pSource=(uint8_t *)((CPU_BUS_LEN)RAS[TBusModule->Tree.IdxRAS].Addr);  
          pSource+=TBusModule->Tree.IdxMAS*SizeOfType(TBusModule->Tree.VarType);         
          for( i=0;i!=TBusModule->Tree.VarCnt*SizeOfType(TBusModule->Tree.VarType);i++)
            *pSource++=TBusModule->Tree.data[i];
          } 
        }
#ifdef MASTER_OPERATION_ENABLE  
      else //Åñëè îáðàòèëèñü ê óñòðîéñòâó â ñåòè âåäóùåãî
        {
          //----------------------------------------------------
        ChangeMRASValue(TBusModule->MRasIdx,&TBusModule->Tree.data[0],0,0xFF);
        }
#endif      
      }
      
   
    CHS=CALC_CHS_16(&TBusModule->TxBuff[1],TxLen-3);
    *pBuff++=(uint8_t)(CHS>>0x08);
    *pBuff++=(uint8_t)(CHS);
    
    TBusModule->SenderFunct(TBusModule->TxBuff,TxLen);    
    break;  
    
    
    case cmdReadRAM:
    case cmdReadEEPROM:
    case cmdReadFlash:   
    TxLen=11+TBusModule->Tree.DataLen;//Äëèíà îòâåòà â áàéòàõ
    pBuff=TBusModule->TxBuff;
    *pBuff++=TBusModule->Tree.Signature;
    *pBuff++=TBusModule->Tree.DevAddr;
    *pBuff++=TBusModule->Tree.Command;    
    TBusModule->Tree.Signature=0x00; 
    
    pSource=(uint8_t *)&TBusModule->Tree.VarAddr;
    
    for( inc=0;inc!=4;inc++)
      *pBuff++=*pSource++;
       
    *pBuff++=TBusModule->Tree.VarCnt;
    *pBuff++=TBusModule->Tree.VarType;
   
    if(cmdReadRAM==TBusModule->Tree.Command)
      {
        pSource=(uint8_t *)((CPU_BUS_LEN)TBusModule->Tree.VarAddr);//Âû÷èòûâàòü ïî àäðåñó

      for( i=0;i!=TBusModule->Tree.DataLen;i++)
        *pBuff++=*pSource++;
      }

#ifdef _eeprom_h
   else if(cmdReadEEPROM==TBusModule->Tree.Command)
      {
     //! e_Read(pBuff,TBusModule->Tree.dataLen,TBusModule->Tree.VarAddr);
     //! pBuff+=TBusModule->Tree.dataLen;
      }
#endif//_eeprom_h   

#ifdef _extflash_h  
   else if(cmdReadFlash==TBusModule->Tree.Command)
      {
     //! f_Read(pBuff,TBusModule->Tree.dataLen,TBusModule->Tree.VarAddr);
     //! pBuff+=TBusModule->Tree.dataLen;  
      }
#endif //_extflash_h    
    
          
    CHS=CALC_CHS_16(&TBusModule->TxBuff[1],TxLen-3);
    *pBuff++=(uint8_t)(CHS>>0x08);
    *pBuff++=(uint8_t)(CHS);
    TBusModule->SenderFunct(TBusModule->TxBuff,TxLen);
    break;  
 
    case cmdWriteRAM:
    case cmdWriteFlash:
    case cmdWriteEEPROM:  
    TxLen=11;//Äëèíà îòâåòà â áàéòàõ (äëÿ ýòîé êîììàíäû ôèêñèðîâàííàÿ)
    pBuff=TBusModule->TxBuff;
    *pBuff++=TBusModule->Tree.Signature;
    *pBuff++=TBusModule->Tree.DevAddr;
    *pBuff++=TBusModule->Tree.Command;    
    TBusModule->Tree.Signature=0x00;
    
    *pBuff++=(uint8_t)((TBusModule->Tree.VarAddr));
    *pBuff++=(uint8_t)((TBusModule->Tree.VarAddr)>>0x08);
    *pBuff++=(uint8_t)((TBusModule->Tree.VarAddr)>>0x10);
    *pBuff++=(uint8_t)((TBusModule->Tree.VarAddr)>>0x18);
   
    *pBuff++=TBusModule->Tree.VarCnt;
    *pBuff++=TBusModule->Tree.VarType;
    CHS=CALC_CHS_16(&TBusModule->TxBuff[1],TxLen-3);
    *pBuff++=(uint8_t)(CHS>>0x08);
    *pBuff++=(uint8_t)(CHS);
    
    for( i=0; i!=RasCnt; i++)
      {
      if((TBusModule->Tree.VarAddr >= (uint32_t)RAS[i].Addr)&&//Åñëè àäðåñ ëåæèò â äîïóñòèìûõ ïðåäåëàõ
         (TBusModule->Tree.VarAddr < ((uint32_t)RAS[i].Addr+(SizeOfType(RAS[i].vType))*RAS[i].vCount)))
        {//Òî i-íîìåð ñòðîêè â ñòðóêòóðå RAS
        if(0!=RAS[i].func)
          {  
          dataIdx=0;
          for( j=0;j!=TBusModule->Tree.VarCnt;j++)
            {
            uint8_t index=(TBusModule->Tree.VarAddr-(uint32_t)RAS[i].Addr)/TBusModule->Tree.VarSize;  
            (*RAS[i].func)(&TBusModule->Tree.data[dataIdx],index);
            dataIdx+=TBusModule->Tree.VarSize;
            }
          }
        else
        { 
         if(cmdWriteRAM==TBusModule->Tree.Command)
          {
          pBuff=(uint8_t *)((CPU_BUS_LEN)TBusModule->Tree.VarAddr);       
          pSource=&TBusModule->Tree.data[0];
          for( i=0;i!=TBusModule->Tree.DataLen;i++)//Çàïèñü ïðèíÿòûõ äàííûõ â ÎÇÓ
            *pBuff++=*pSource++;     
          }  
        #ifdef _eeprom_h
          else if(cmdWriteEEPROM==TBusModule->Tree.Command)//Çàïèñü ïðèíÿòûõ äàííûé â EEPROM
            {
          //!  e_Write(&TBusModule->Tree.data[0],TBusModule->Tree.dataLen,TBusModule->Tree.VarAddr);
            }
        #endif //_eeprom_h          
        #ifdef _extflash_h
          else if(cmdWriteFlash==TBusModule->Tree.Command)//Çàïèñü ïðèíÿòûõ äàííûé âî Flash
            {
           //! f_Write(&TBusModule->Tree.data[0],TBusModule->Tree.dataLen,TBusModule->Tree.VarAddr);
            }
        #endif //_extflash_h         
        }
        }
      }  
    TBusModule->SenderFunct(TBusModule->TxBuff,TxLen);  
    break;  

 //***********Àðõèâ******************//
    case cmdGetArch:
    if(0==TBusModule->GetPortStatusFunct())//Åñëè îòïðàâùèê ñâîáîäåí
      {
    if(ARCH_CELL_FORMAT_SIGN_==TBusModule->Tree.ArchSign)//Åñëè çàïðîñèëè ôîðìàò ÿ÷åéêè
      {
   
        //ArchConfirm.ArchCellCnt= 7;//sizeof(arch_struct);//sizeof(arch_points);///sizeof(ArchPoints_t);  
          
        TxLen=24;//Äëÿ ýòîãî îòâåòà äëèíà ôèêñèðîâàííàÿ
           
        pBuff=TBusModule->TxBuff;
        *pBuff++=TBusModule->Tree.Signature;
        *pBuff++=TBusModule->Tree.DevAddr;
        *pBuff++=TBusModule->Tree.Command;   
      
        *pBuff++=(uint8_t)TBusModule->Tree.ArchSign;
       
        TBusModule->ArchRequest.DevAddr        = TBusModule->Tree.DevAddr;
        TBusModule->ArchRequest.ArchDay        = TBusModule->Tree.ArchDay;
        TBusModule->ArchRequest.ArchHalfSec    = TBusModule->Tree.ArchHalfSec;
        TBusModule->ArchRequest.ArchReqType    = ARCH_GET_CELL_FORMAT_REQUEST_TYPE;
        TBusModule->ArchRequest.ArchCellNumber = TBusModule->Tree.ArchCellNumber;
          
        TBusModule->ArchConfirm = TBusModule->ArchCallBack(&TBusModule->ArchRequest);
        
        asm("nop");
        
        pSource = (uint8_t *)&TBusModule->ArchConfirm.ArchDay;
        *pBuff++=*pSource++;
        *pBuff++=*pSource++;
  
        pSource = (uint8_t *)&TBusModule->ArchConfirm.ArchHalfSec;
        *pBuff++=*pSource++;
        *pBuff++=*pSource++;
        *pBuff++=*pSource++;
        *pBuff++=*pSource++;
       
        
        if(TBusModule->Tree.ArchCellCnt)
        {
        if(TBusModule->Tree.ArchCellNumber>=TBusModule->ArchConfirm.ArchCellCnt)//Åñëè çàïðîñèëè ôîðìàò íåñóùåñòâóþùåé ÿ÷åêè
          {
          TxLen=16;  
          TBusModule->Tree.Signature=0;  
          *pBuff++=0xFF;
          *pBuff++=0xFF;
          
          pSource = (uint8_t *)&TBusModule->ArchConfirm.ArchCellCnt;
          *pBuff++=*pSource++;
          *pBuff++=*pSource++;
          }
        else
          {
          pSource = (uint8_t *)&TBusModule->Tree.ArchCellNumber;
          *pBuff++=*pSource++;
          *pBuff++=*pSource++;
          
          pSource = (uint8_t *)&TBusModule->ArchConfirm.ArchCellCnt;
          *pBuff++=*pSource++;
          *pBuff++=*pSource++;
                
          *pBuff++=TBusModule->ArchConfirm.ArchItem;
          *pBuff++=((TBusModule->ArchConfirm.ArchvType<<4)|TBusModule->ArchConfirm.ArchChannel);
          
          pSource = (uint8_t *)&TBusModule->ArchConfirm.ArchvIdx;
          *pBuff++=*pSource++;
          *pBuff++=*pSource++;
          
          *pBuff++=TBusModule->ArchConfirm.ArchIdxArray;
             
          *pBuff++=TBusModule->ArchConfirm.ArchPointCast;
        
          
          *pBuff++=0x00;
          *pBuff++=0x00;
        //   pSource = (uint8_t *)&TBusModule->ArchConfirm.ArchPeriod;
        
       //   *pBuff++=*pSource++;
       //   *pBuff++=*pSource++;
          }
        TBusModule->Tree.ArchCellCnt--;
        TBusModule->Tree.ArchCellNumber++;
            
       // if(0==TBusModule->Tree.ArchCellCnt)
       //   TBusModule->Tree.Signature=0;
        asm("nop");  
        }
      else
        {
        TxLen=16;  
        TBusModule->Tree.Signature=0;  
        *pBuff++=0xFF;
        *pBuff++=0xFF;
        *pBuff++=0xFF;
        *pBuff++=0xFF;
        
        //pSource = (uint8_t *)&ArchConfirm.ArchCellCnt;
        //*pBuff++=*pSource++;
        //*pBuff++=*pSource++;
        }
      //TBusModule->Tree.Signature=0;
      CHS=CALC_CHS_16(&TBusModule->TxBuff[1],TxLen-3);
      *pBuff++=(uint8_t)(CHS>>0x08);
      *pBuff++=(uint8_t)(CHS);
          
      TBusModule->SenderFunct(TBusModule->TxBuff,TxLen);
        
      asm("nop");  
      }
    else  //Åñëè çàïðîñèëè àðõèâíûå çàïèñè
      {
      if(0==TBusModule->GetPortStatusFunct())//Åñëè îòïðàâùèê ñâîáîäåí
        {
        //ArchCellCnt= 7;//sizeof(arch_struct);//sizeof(arch_points);///sizeof(ArchPoints_t);  
                
        pBuff=TBusModule->TxBuff;
        *pBuff++=TBusModule->Tree.Signature;
        *pBuff++=TBusModule->Tree.DevAddr;
        *pBuff++=TBusModule->Tree.Command;   
  
        
        TBusModule->ArchRequest.DevAddr        = TBusModule->Tree.DevAddr;
        TBusModule->ArchRequest.ArchDay        = TBusModule->Tree.ArchDay;
        TBusModule->ArchRequest.ArchHalfSec    = TBusModule->Tree.ArchHalfSec;
        TBusModule->ArchRequest.ArchReqType    = ARCH_GET_ARCH_PARAMS_REQUEST_TYPE;
        TBusModule->ArchRequest.ArchCellNumber = TBusModule->Tree.ArchCellNumber;
          
        TBusModule-> ArchConfirm = TBusModule->ArchCallBack(&TBusModule->ArchRequest);
        
        TBusModule->Tree.ArchDay     = TBusModule->ArchConfirm.ArchDay;
        TBusModule->Tree.ArchHalfSec = TBusModule->ArchConfirm.ArchHalfSec;
        
        *pBuff++=(uint8_t)TBusModule->Tree.ArchSign;
        pSource = (uint8_t *)&TBusModule->ArchConfirm.ArchDay;
        *pBuff++=*pSource++;
        *pBuff++=*pSource++;
  
        pSource = (uint8_t *)&TBusModule->ArchConfirm.ArchHalfSec;
        for( i=0;i!=sizeof(TBusModule->ArchConfirm.ArchHalfSec);i++)
          *pBuff++=*pSource++;
        
       
        
        //  pSource = (uint8_t *)&TBusModule->Tree.ArchRecCount;
      //  for(uint8_t i=0;i!=sizeof(TBusModule->Tree.ArchRecCount);i++)
      //    *pBuff++=*pSource++;
           
        if(TBusModule->Tree.ArchRecCount)
          {
          TBusModule->ArchRecCnt=(TBusModule->TxBuffSize-20)/TBusModule->ArchConfirm.ArchRecSize;//Ñêîëüêî çàïèñåé ïîìåñòèòñÿ â áóôåð
          
          if(TBusModule->ArchRecCnt>TBusModule->Tree.ArchRecCount)//Åñëè îñòàëîñü ìåíüøå çàïèñåé ÷åì ìîæåò ïîìåñòèòüñÿ â áóôåð
            TBusModule->ArchRecCnt=TBusModule->Tree.ArchRecCount;
        
          TxLen=TBusModule->ArchRecCnt*TBusModule->ArchConfirm.ArchRecSize+20;
          
          pSource = (uint8_t *)&TBusModule->ArchRecCnt;
          for( i=0;i!=4;i++)
            *pBuff++=*pSource++;
             
          pSource = (uint8_t *)&TBusModule->ArchConfirm.ArchPeriod;
          for( i=0;i!=2;i++)
            *pBuff++=*pSource++;
     
                       
          pSource = (uint8_t *)&TBusModule->ArchConfirm.ArchRecSize;
          for( i=0;i!=2;i++)
            *pBuff++=*pSource++;
          
        
       
        TBusModule->ArchRequest.DevAddr        = TBusModule->Tree.DevAddr;
        TBusModule->ArchRequest.ArchDay        = TBusModule->ArchConfirm.ArchDay;
        TBusModule->ArchRequest.ArchHalfSec    = TBusModule->ArchConfirm.ArchHalfSec;
        TBusModule->ArchRequest.ArchReqType    = ARCH_GET_REC_REQUEST_TYPE;
        TBusModule->ArchRequest.pData          = pBuff;
        TBusModule->ArchRequest.ArchRecCnt     = TBusModule->ArchRecCnt;
        
        TBusModule->ArchCallBack(&TBusModule->ArchRequest); //Ïåðåäàåì çàïðîñ íà âû÷èòûâàíèå ArchRecCnt àðõèâíûõ çàïèñåé
        
          
         //!!!!!!!!!!!!!!ArchRecRead(pBuff,ArchRecCnt,ArchConfirm.ArchDay,ArchConfirm.ArchHalfSec);
          
        ///////////????
        //Ïðîâåðèòü ïåðåõîä ÷åðåç íà÷àëî ñóòîê
        
        if((TBusModule->Tree.ArchHalfSec+TBusModule->ArchConfirm.ArchPeriod*TBusModule->ArchRecCnt)>=172800)
          {
          TBusModule->Tree.ArchDay++;
          TBusModule->Tree.ArchHalfSec = TBusModule->Tree.ArchHalfSec+TBusModule->ArchConfirm.ArchPeriod*TBusModule->ArchRecCnt - 172800;
          }
        else
          {
          TBusModule->Tree.ArchHalfSec+=TBusModule->ArchConfirm.ArchPeriod*TBusModule->ArchRecCnt;
          }
               
               
        TBusModule->Tree.ArchRecCount-=TBusModule->ArchRecCnt;
        }
      else
        {
        TBusModule->Tree.Signature=0x00;  
        TxLen=16;
        for( i=0;i!=4;i++)
          *pBuff++=0xFF;
        }
      
      pBuff=&TBusModule->TxBuff[TxLen-2];
     
      CHS=CALC_CHS_16(&TBusModule->TxBuff[1],TxLen-3);
      *pBuff++=(uint8_t)(CHS>>0x08);
      *pBuff++=(uint8_t)(CHS);
      
      TBusModule->SenderFunct(&TBusModule->TxBuff[0],TxLen);
      
      //if(0==TBusModule->Tree.ArchRecCount)
      //  TBusModule->Tree.Signature=0x00; 
          
        }
                             
      asm("nop");  
      }
    asm("nop"); 
    }
    break;  
      
    case cmdDataTransfer:
      if(TBusModule->Tree.DevAddr==TBusModule->NetworkDeviceAddr)//Åñëè îáðàòèëèñü ê âåäóùåìó
        {
        DTConf.Reserved = TBusModule->Tree.Reserved;
        DTConf.ProtType = TBusModule->Tree.ProtType;
        DTConf.pData    = &TBusModule->Tree.data[0];
        DTConf.DataCnt  = TBusModule->Tree.DataLen;     
        DTConf.DTState  = TBusModule->Tree.State;    
        //DTRxEvent(&DTConf);                 //Ïåðåäà÷à óïðàâëåíèÿ âíåøíåìó îáðàáîò÷èêó
        if(((void *)0)!=TBusModule->DTransferRxCallBack)
          TBusModule->DTransferRxCallBack(&DTConf);
        
        TxLen=10+DTConf.DataCnt;           //Äëèíà ïàêåòà äëÿ ïåðåäà÷è
        pBuff=TBusModule->TxBuff;      
        *pBuff++=TBusModule->Tree.Signature;
        TBusModule->Tree.Signature=0;
        *pBuff++=TBusModule->NetworkDeviceAddr;
        *pBuff++=TBusModule->Tree.Command;
        *pBuff++=DTConf.Reserved;
        *pBuff++=(uint8_t)DTConf.DTState;
        *pBuff++=(DTConf.FragCnt<<0x04)|(0x0F&DTConf.FragIdx); 
        *pBuff++=(uint8_t)DTConf.ProtType;
        *pBuff++=DTConf.DataCnt;   
        pSource=&DTConf.pData[0];
        for( i=0;i!=DTConf.DataCnt;i++)
          *pBuff++=*pSource++;  
        CHS=CALC_CHS_16(&TBusModule->TxBuff[1],TxLen-3);
        *pBuff++=(uint8_t)(CHS>>0x08);
        *pBuff++=(uint8_t)(CHS);
        asm("nop");
        TBusModule->SenderFunct(TBusModule->TxBuff,TxLen);
        }
#ifdef MASTER_OPERATION_ENABLE   
      else//åñëè îáðàòèëèñü ê âåäîìîììó óñòðîéñòâó â ñåòè âåäóùåãî
        {
        asm("nop");
        if(DT_FREE_RUN==DT_Confifm.State)
          {
          asm("nop");  
          TRData.DevAddr 	= TBusModule->Tree.DevAddr;
          TRData.pData   	= TBusModule->Tree.data;
          TRData.len     	= TBusModule->Tree.DataLen;
          TRData.FragCnt 	= TBusModule->Tree.FragCnt;
          TRData.FragIdx 	= TBusModule->Tree.FragIdx;
          TRData.Reserved       = TBusModule->Tree.Reserved;
          TRData.ProtType       = (MProtType_t)TBusModule->Tree.ProtType;
          TRData.State          = (MDTState_t)TBusModule->Tree.State;

          TransferData(&TRData);//Ïåðåäàòü äàííûå àäðåñóåìîìó âåäîìîìó    
      
          DT_Confifm.State=DT_WAIT_SLAVE_RESPONSE;//Ïåðåéòè â ñîñòîÿíèå îæèäàíèÿ îòâåòà îò âåäîìîãî
          }        
        if(DT_SLAVE_RESPONSE_COMPLETED==DT_Confifm.State)
          {
          DT_Confifm.State = DT_FREE_RUN;  
           
          TxLen=10+DT_Confifm.DataCnt;           //Äëèíà ïàêåòà äëÿ ïåðåäà÷è
          pBuff=TBusModule->TxBuff;      
          *pBuff++=TBusModule->Tree.Signature;
          TBusModule->Tree.Signature=0;
          *pBuff++=TBusModule->Tree.DevAddr;
          *pBuff++=TBusModule->Tree.Command;
          *pBuff++=DT_Confifm.Reserved;
          *pBuff++=(uint8_t)DT_Confifm.DTState;
          *pBuff++=(DT_Confifm.FragCnt<<0x04)|(0x0F&DT_Confifm.FragIdx); 
          *pBuff++=(uint8_t)DT_Confifm.ProtType;
          *pBuff++=DT_Confifm.DataCnt;   
          pSource=&DT_Confifm.pData[0];
          for( i=0;i!=DT_Confifm.DataCnt;i++)
            *pBuff++=*pSource++;  
          CHS=CALC_CHS_16(&TBusModule->TxBuff[1],TxLen-3);
          *pBuff++=(uint8_t)(CHS>>0x08);
          *pBuff++=(uint8_t)(CHS);
          asm("nop");
          TBusModule->SenderFunct(TBusModule->TxBuff,TxLen);
          }
        asm("nop");
        }
#endif     
    break;  
    
    
    default:break;  
    };
    }
  else if((errNone!=TBusModule->Tree.error)&&(TBusModule->Tree.Signature!=0x00))
    {
 
    TxLen=5;//Äëèíà îòâåòà â áàéòàõ (äëÿ ýòîé êîììàíäû ôèêñèðîâàííàÿ)
    TBusModule->TxBuff[0]=TBusModule->Tree.Signature;
    TBusModule->TxBuff[1]=TBusModule->Tree.DevAddr;
    TBusModule->TxBuff[2]=TBusModule->Tree.error;
    
    CHS=CALC_CHS_16(&TBusModule->TxBuff[1],TxLen-3);
    TBusModule->TxBuff[3]=(uint8_t)(CHS>>0x08);
    TBusModule->TxBuff[4]=(uint8_t)(CHS);
    TBusModule->Tree.Signature=0x00;
	float aa;
    for( aa=0;aa!=0xFFF;aa++)asm("nop");
    
    TBusModule->SenderFunct(TBusModule->TxBuff,TxLen);
   
    }
  }
}



void TBusTimeOutCounter(TBusSlave_t *TBusModule)
{
if(TbState != SIGNATURE_ANALISE_STATE)
  {
   if(TBusModule->Time++ > TBusTimeOut - 1)
     {
     TBusModule->Time=0;
     TbState=SIGNATURE_ANALISE_STATE;
    
     if (TBusModule->SetPortStatusFunct != (void*)0)
      TBusModule->SetPortStatusFunct(0); //sky!!
     
     //SlavePortSatus = PORT_FREE;
     }
  }

if(DT_FREE_RUN!=DT_Confifm.State)
  {
    if(DTransferTime++ > DTransferTimeOut - 1)
     {
     DTransferTime=0;
     DT_Confifm.State=DT_FREE_RUN;
     }
  }
}











