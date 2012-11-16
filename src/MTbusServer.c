#include "../inc/includes.h"

//#include "MTBusServer.h"
//#include "Terabus.h"
//#include "GpioDriver.h"
//#include "MTBusRoutingTable.h"


AssignHostPortAddr_t AssignHostPortAddr[MaxHostPorts];
DevId_t              MDevId[MaxDevices];
uint8_t              NetDevicesCnt=0;               


void TB_ReqConfirm(MTBusConfirm_t *TBusConf);


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

void MTBusPostTask(MTBusServer_t *Server){Server->MTBusPostTaskEn=1;}



void MTBusAddDevice(MTBusServer_t *Server,uint8_t DevAddr)
{
	uint8_t i;
	uint8_t n;
for(i=0;i!=MaxDevices;i++)
  {
  if(DevAddr==Server->NetDevices[i])
    return;
  }
Server->NetDevices[Server->NetDevCnt++]=DevAddr;

for(i=0;i!=MaxDevices;i++)
  {
  if(DevAddr == Server->MTBusModule.AddedDevices[i])//Åñëè ýòî óñòðîéñòâî óæå åñòü  
    break;
  if(0==Server->MTBusModule.AddedDevices[i])
    {
    Server->MTBusModule.AddedDevices[i] = DevAddr;  
    break;
    }
  }

MDevId[NetDevicesCnt].DevAddr  = DevAddr;
MDevId[NetDevicesCnt].HostAddr = Server;

for(n=0;n!=MaxHostPorts;n++)
  {
  if(AssignHostPortAddr[n].HostAddr==MDevId[NetDevicesCnt].HostAddr)  
    MDevId[NetDevicesCnt].Port  = AssignHostPortAddr[n].Port;
  }
NetDevicesCnt++;
}

void MTBusDelDevice(MTBusServer_t *Server,uint8_t DevAddr)
{
  uint8_t i;
  uint8_t j,n;
  uint16_t k;
for( i=0;i!=MaxDevices;i++)
  {
  if(Server->NetDevices[i]==DevAddr)
    {
    for(j=i;j!=MaxDevices;j++)
      {
      Server->NetDevices[j]=Server->NetDevices[j+1];
      }
    Server->NetDevices[MaxDevices-1]=0;
	//if (Server->NetDevCnt >= 0) // sky 02/04/12
	if (Server->NetDevCnt != 0)//bin 06/04/12
     Server->NetDevCnt--;

    if(0!=NetDevicesCnt)
      NetDevicesCnt--;
    break;  
    } 
  }

for( i=0;i!=MaxDevices;i++)//Óäàëÿåì èç MDevId
  {
  if(MDevId[i].DevAddr==DevAddr)
    {
    MDevId[i].DevAddr         = 0;  
    MDevId[i].Port            = HOST_PORT_UNKNOWN;
    MDevId[i].Online          = 0;
    MDevId[i].NotRespCnt      = 0;
    MDevId[i].HostAddr        = (void *)0;
    for( j=0;j!=sizeof(MDevId[i].DevName);j++)//Î÷èñòêà íàçâàíèÿ ïðèáîðà
       MDevId[i].DevName[j]=0;
    for(j=0;j!=sizeof(MDevId[i].ProgVer);j++)//Î÷èñòêà âåðñèè ïðîãðàììû ïðèáîðà
       MDevId[i].ProgVer[j]   = 0;
    MDevId[i].LastReceiveTime = 0;
    MDevId[i].OnlineTimeOut   = 0;
    break;
    }
  }  
  
  
for( k=0;k!=MRAS_COUNT;k++)//Óäàëÿåì èç MRAS
  {
  if(MRAS[k].DevAddr==DevAddr)
    {
    MRAS[k].DevAddr = 0;  
    MRAS[k].vIdx    = 0;
    MRAS[k].vType   = 0xFF; 
    }
  }


for( n=0;n!=MaxDevices;n++) //Óäàëÿåì èç ïåðå÷íÿ ðàçðåøåííûõ àäðåñîâ ïðèáîðîâ
  {
  if(DevAddr == Server->MTBusModule.AddedDevices[n])//Åñëè ýòî óñòðîéñòâî óæå åñòü  
    {
    Server->MTBusModule.AddedDevices[n] = 0x00;
    break;
    }
  }
}



void MTBusServerInit(MTBusServer_t *Server)
{
	uint8_t i;
Server->MTBusModule.ReqConf                   = TB_ReqConfirm;  
Server->MTBusModule.LastTBusReq.InitiatorAddr = (uint8_t *)Server;
Server->MTBusModule.FlowAnaliserTimeOut       = Server->FlowAnaliserTimeOut;
Server->MTBusModule.NoticationCallBack        = Server->NoticationCallBack;
Server->MTBusModule.ArchConfirmCallBack       = Server->ArchConfirmCallBack;


for( i=0;i!=MaxHostPorts;i++)
  {
  if(0==AssignHostPortAddr[i].Port)
    {
    AssignHostPortAddr[i].Port     =  (HostPort_t)(i+1);
    AssignHostPortAddr[i].HostAddr =  Server;
    break;
    }    
  }

MTBusInit(&Server->MTBusModule);
Server->State=MTBUS_MANAGER_INIT_STATE;
MTBusPostTask(Server);
}


void MTBusServerStartSearch(MTBusServer_t *Server)
{
	uint8_t i;
for(i=0;i!=MaxDevices;i++)
  Server->NetDevices[i] = 0x00;  

for( i=0;i!=MaxDevices;i++)
  {
  if(MDevId[i].HostAddr == Server)
    {
    MDevId[i].DevAddr    = 0x00;
    MDevId[i].Port       = HOST_PORT_UNKNOWN;
    MDevId[i].Online     = 0x00;
    MDevId[i].NotRespCnt = 0x00;
    MDevId[i].HostAddr   = (void *)0;
    MDevId[i].LastReceiveTime = 0x00;
    MDevId[i].OnlineTimeOut   = 0x00;
    if(0!=NetDevicesCnt)
      NetDevicesCnt--;
    }      
  }

Server->NetDevCnt     = 0x00;
Server->ScanDevIdx    = 0x00;
Server->SearchDevAddr = 0x00; //0x01;

Server->State=MTBUS_MANAGER_INIT_STATE;
MTBusPostTask(Server);  
}

void MTBusServerTimeOutCounter(MTBusServer_t *Server)
{
uint8_t i;
MTBusTimeOutCounter(&Server->MTBusModule);


//Êîä äëÿ êîíòðîëÿ äëèòåëüíîãî ïî âðåìåíè îòñóòñòâèÿ ïðèáîðà â ñåòè
if(Server->msec++>=1000) 
  {
  Server->msec=0;  
  for( i=0;i!=MaxDevices;i++)
    {
    if((MDevId[i].Online)&&(Server==MDevId[i].HostAddr))
      {
      MDevId[i].LastReceiveTime++;  
      if(MDevId[i].LastReceiveTime >= MDevId[i].OnlineTimeOut)
        {
        MDevId[i].LastReceiveTime = 0;//MDevId[i].OnlineTimeOut;
        MDevId[i].OnlineTimeOut = Server->MaxReceiveInterval;
        MDevId[i].Online = 0;
        }
      }
    }
  }
}

void MTBusServerDataReceiver(MTBusServer_t *Server,uint8_t byte)
{
MTBusDataReceiver(&Server->MTBusModule,byte);
}

void ChangeMRASValue(uint16_t MRASn,uint8_t *pData,uint8_t IdxMas,uint8_t vCnt)
{
uint8_t *pReceiver;  
uint8_t *pBuff;
uint16_t DataLen;
MTBusServer_t *Server=(void *)0;
uint8_t i;
uint8_t idx;
//Íà÷èíàåì ïîèñê îáüåêòà, â ïîä÷èíåíèè êîòîðîãî íàõîäèòñÿ óñòðîéñòâî, êîòîðîìó àäðåñîâàíû äàííûå
for( i=0;i!=MaxDevices;i++)
  {
  if(MDevId[i].DevAddr == MRAS[MRASn].DevAddr)  
    {
    if(MDevId[i].Online)//Åñëè óñòðîéñòâî â ñåòè
      Server = (MTBusServer_t *)MDevId[i].HostAddr;
    }
  }

if(((void *)0)!=Server)//Åñëè îáúåêò âåðõíåãî óðîâíÿ íàéäåí
  {
  if ( vCnt == 0xff)
    DataLen = MRAS[MRASn].vCnt*SizeOfType(MRAS[MRASn].vType);
  else
    DataLen = vCnt*SizeOfType(MRAS[MRASn].vType);

  if((uint32_t)(DataLen+Server->QueBuffIdx)>=(uint32_t)(sizeof(Server->QueBuff)-1))
    Server->QueBuffIdx=0;
  
  pReceiver=&Server->QueBuff[Server->QueBuffIdx];
  Server->WriteQueue[Server->QueLast].pData=pReceiver;
  pBuff=pData;
  
  for(idx=0;idx!=DataLen;idx++)
    {
    *pReceiver++=*pBuff++;
    Server->QueBuffIdx++;
    }
  //-----Çàïîëíåíèå áóôåðà äàííûõ-------
  if(Server->WriteBuffStorageEnable)
    {
    pBuff=pData;
    pReceiver = MRAS[MRASn].pData + //!
                 SizeOfType(MRAS[MRASn].vType)*IdxMas;
    for( idx=0;idx!=DataLen;idx++)
      {
      *pReceiver++=*pBuff++;  
      }
    }
  //------------------------------------
  Server->WriteQueue[Server->QueLast].MRasIdx = MRASn;
  Server->WriteQueue[Server->QueLast].IdxMas  = IdxMas;
  Server->WriteQueue[Server->QueLast].vCnt    = vCnt;
  
  Server->QueLast++;
  if(Server->QueLast>=MaxVarQueue)
    Server->QueLast=0;
  }
}


//Çàïðîñ îòëîæåííîé ïåðåäà÷è äàííûõ ñ ïîìîùüþ êîììàíäû _cmdDataTransfer
void TransferData(TRData_t *TRData)
{
uint8_t       *pReceiver;  
uint8_t       *pBuff;
MTBusServer_t *Server = (void *)0;
uint8_t i,idx;
//Ïîèñê îáúåêòà âåðõíåãî óðîâíÿ
for( i=0;i!=MaxDevices;i++)
  {
  if(MDevId[i].DevAddr==TRData->DevAddr)
    {
    if(MDevId[i].Online)
      Server = (MTBusServer_t *)MDevId[i].HostAddr; 
    } 
  }

if(((void *)0)!=Server)//Åñëè îáúåêò âåðõíåãî óðîâíÿ íàéäåí
  {
  if((uint32_t)(TRData->len+Server->TRQueBuffIdx)>=(uint32_t)(sizeof(Server->TRQueBuff)-1))
    Server->TRQueBuffIdx=0;
  
  pReceiver=&Server->TRQueBuff[Server->TRQueBuffIdx];
  Server->TRQueue[Server->TRQueLast].pData=pReceiver;
  pBuff=TRData->pData;
  
  for( idx=0;idx!=TRData->len;idx++)
    {
    *pReceiver++=*pBuff++;
    Server->TRQueBuffIdx++;
    }
  
  Server->TRQueue[Server->TRQueLast].len      = TRData->len;
  Server->TRQueue[Server->TRQueLast].DevAddr  = TRData->DevAddr;
  Server->TRQueue[Server->TRQueLast].Reserved = TRData->Reserved;
  Server->TRQueue[Server->TRQueLast].ProtType = TRData->ProtType;
  Server->TRQueue[Server->TRQueLast].State    = TRData->State;
  Server->TRQueue[Server->TRQueLast].FragCnt  = TRData->FragCnt;
  Server->TRQueue[Server->TRQueLast].FragIdx  = TRData->FragIdx;
  
  Server->TRQueLast++;
  if(Server->TRQueLast>=MaxTtansferQueue)
    Server->TRQueLast=0;
  }
}

void MTBusArchRequest(MTBusServer_t *Server,ArchReqDescriptor_t *ArchReqDescriptor)//Çàïðîñ àðõèâíîé êîììàíäû
{
Server->ArchReqDescriptor.ArchDevAddr 	 = ArchReqDescriptor->ArchDevAddr;
Server->ArchReqDescriptor.ArchSign 	 = ArchReqDescriptor->ArchSign;
Server->ArchReqDescriptor.ArchDay 	 = ArchReqDescriptor->ArchDay;
Server->ArchReqDescriptor.ArchHalfSec 	 = ArchReqDescriptor->ArchHalfSec;
Server->ArchReqDescriptor.ArchRecCount 	 = ArchReqDescriptor->ArchRecCount;
Server->ArchReqDescriptor.ArchCellNumber = ArchReqDescriptor->ArchCellNumber;
Server->ArchReqDescriptor.ArchCellCnt    = ArchReqDescriptor->ArchCellCnt;
Server->ArchReqDescriptor.ArchReqType 	 = ArchReqDescriptor->ArchReqType;
}


uint8_t StrCompare(uint8_t *str,uint8_t *stre,uint8_t len)
{
	uint8_t i;
for(i=0;i!=len;i++)
  {
  if(*str++!=*stre++)
    return 0;
  }
return 1;
}

//Ñåðâèñ ìàðøðóòèçàöèè
void RoutingRun(void)
{
uint8_t  *pSource;
uint8_t  *pDist;
uint16_t sMRASidx   = 0;//Èíäåêñ çàïèñè èñòî÷íèêà â MRAS 
uint16_t dMRASidx   = 0;//Èíäåêñ çàïèñè ïîëó÷àòåëÿ â MRAS
uint8_t  sFieldFind = 0;  
uint8_t  dFieldFind = 0;  
uint8_t  DataSize;  //Ðàçìåð äàííûõ â áàéòàõ êàêîé-ëèáî çàïèñè â MRAS
uint8_t i,d;
uint16_t cnt, inc;
for( i=0;i!=RoutingTableCnt;i++)
{
for(cnt=0;cnt!=MRAS_COUNT;cnt++)//Ïîèñê çàïèñè èñòî÷íèêà ïî ñåòåâîìó àäðåñó
  {
  if(MRAS[cnt].DevAddr!=0)//Åñëè çàïèñü íå ïóñòà
  if(MRAS[cnt].DevAddr==*RoutingTable[i].sDevAddr)
    if(MRAS[cnt].vIdx==RoutingTable[i].sVidx)
      {
      sMRASidx=cnt;  
      sFieldFind=1; 
      break;
      }
  sFieldFind=0;      
  }

for( cnt=0;cnt!=MRAS_COUNT;cnt++)//Ïîèñê çàïèñè ïîëó÷àòåëÿ ïî ñåòåâîìó àäðåñó
  {
  if(MRAS[cnt].DevAddr!=0)//Åñëè çàïèñü íå ïóñòà
  if(MRAS[cnt].DevAddr==*RoutingTable[i].dDevAddr)
    if(MRAS[cnt].vIdx==RoutingTable[i].dVidx)
      {
      dMRASidx=cnt;
      dFieldFind=1;
      break;
      }
  dFieldFind=0;      
  }


if((sFieldFind)&&(dFieldFind))//Ïîëÿ èñòî÷íèêà è ïîëó÷àòåëÿ íàéäåíû
  {
  if((MRAS[sMRASidx].vType==MRAS[dMRASidx].vType)&&//Ïðîâåðêà íà ñîâïàäåíèå òèïîâ èñòî÷íèêà è ïîëó÷àòåëÿ
  (MRAS[sMRASidx].vCnt==MRAS[dMRASidx].vCnt))
    {
    pSource=MRAS[sMRASidx].pData;
    pDist  =MRAS[dMRASidx].pData;   
    
    DataSize=MRAS[dMRASidx].vCnt*SizeOfType(MRAS[dMRASidx].vType);
    
    for( d=0;d!=DataSize;d++)
      {
      if(*pDist++!=*pSource++)
        {
        for( inc=0;inc!=sizeof(MDevId)/sizeof(DevId_t);inc++)
          {
          if(MDevId[inc].DevAddr==MRAS[dMRASidx].DevAddr)
            {
            ChangeMRASValue(dMRASidx,MRAS[sMRASidx].pData,0,0xFF);
            break;
            }
          }
        break;
        }
      }
    }
  else   //Òèïû èñòî÷íèêà è ïîëó÷àòåëÿ íå ñîâïàäàþò 
    {
	asm("nop");//
    }
  } 
sFieldFind=0;
dFieldFind=0;
}
}



void MTBusServerRun(MTBusServer_t *Server)
{
if(Server->MTBusPostTaskEn)
{
Server->MTBusPostTaskEn=0;  
  
switch(Server->State)
  {
  case MTBUS_MANAGER_INIT_STATE:

    if((0x00==Server->NetDevices[0])&&(Server->RequestEnable))
      Server->State=MTBUS_MANAGER_GET_ID_STATE;
    else
      Server->State=MTBUS_MANAGER_WRITE_VAR_STATE;
  MTBusPostTask(Server);
  break;  
    
    
  case MTBUS_MANAGER_GET_ID_STATE:
    if(++Server->SearchDevAddr==Server->MaxNetAddr)
      {
      //if(0x00==Server->NetDevices[0])  
      //  Server->State = MTBUS_MANAGER_INIT_STATE;
      //else
      Server->State=MTBUS_MANAGER_WRITE_VAR_STATE;  
      Server->SearchDevAddr=1;//0;
      }
    else
      Server->State=MTBUS_MANAGER_GET_ID_STATE;  
    Server->MTBusDescriptor.ServiceKey     = NONE_SERVICE_KEY;
    Server->MTBusDescriptor.InitiatorAddr  = (uint8_t *)Server;
    Server->MTBusDescriptor.DevAddr        = Server->SearchDevAddr;
    Server->MTBusDescriptor.Command        = _cmdId;
    Server->MTBusDescriptor.ResponseTime   = Server->MaxResponseTime;
    Server->MTBusDescriptor.SenderFunct    = Server->SenderFunct;
    MTBusReq(&Server->MTBusModule,&Server->MTBusDescriptor);  
  break;
  
  
  case MTBUS_MANAGER_ASYNCHR_GET_ID_STATE:
    Server->State=MTBUS_MANAGER_WRITE_VAR_STATE; 
    Server->MTBusDescriptor.ServiceKey     = NONE_SERVICE_KEY;
    Server->MTBusDescriptor.InitiatorAddr  = (uint8_t *)Server;
    Server->MTBusDescriptor.DevAddr        = Server->DeferredDevAddr;
    Server->MTBusDescriptor.Command        = _cmdId;
    Server->MTBusDescriptor.ResponseTime   = Server->MaxResponseTime;
    Server->MTBusDescriptor.SenderFunct    = Server->SenderFunct;
    MTBusReq(&Server->MTBusModule,&Server->MTBusDescriptor);  
  break;
  

  case MTBUS_MANAGER_WRITE_VAR_STATE:
  Server->State=MTBUS_MANAGER_TRANSFER_STATE;
  if((Server->QueFirst!=Server->QueLast)&&(0!=MRAS[Server->WriteQueue[Server->QueFirst].MRasIdx].DevAddr))//Åñëè åñòü â î÷åðåäè ïåðåìåííûå íà çàïèñü
    {                                                             //è ïîëå â êîòîðîå äåëàþò çàïèñ çàïîëíåíî
	if(Server->ServiceKeyEnable)
	  Server->MTBusDescriptor.ServiceKey   =  SET_ADDR_SERVICE_KEY;
	else
	  Server->MTBusDescriptor.ServiceKey   =  NONE_SERVICE_KEY;

    Server->MTBusDescriptor.InitiatorAddr  = (uint8_t *)Server;
    Server->MTBusDescriptor.DevAddr        = MRAS[Server->WriteQueue[Server->QueFirst].MRasIdx].DevAddr;
    Server->MTBusDescriptor.Command        = _cmdWriteRAM_EXT;
    Server->MTBusDescriptor.IdxRAS         = MRAS[Server->WriteQueue[Server->QueFirst].MRasIdx].vIdx;
    Server->MTBusDescriptor.IdxMAS         = Server->WriteQueue[Server->QueFirst].IdxMas;
    
   
    if(0xFF==Server->WriteQueue[Server->QueFirst].vCnt)
      Server->MTBusDescriptor.VarCnt       = MRAS[Server->WriteQueue[Server->QueFirst].MRasIdx].vCnt; 
    else
      Server->MTBusDescriptor.VarCnt       = Server->WriteQueue[Server->QueFirst].vCnt;
   
    Server->MTBusDescriptor.VarType        = (_VarType_t)MRAS[Server->WriteQueue[Server->QueFirst].MRasIdx].vType;
    Server->MTBusDescriptor.pData          = Server->WriteQueue[Server->QueFirst].pData;//MRAS[WriteQueue[QueFirst].MRasIdx].pData; 
    Server->MTBusDescriptor.ResponseTime   = Server->MaxResponseTime;
    Server->MTBusDescriptor.SenderFunct    = Server->SenderFunct;
   
    Server->QueFirst++;
    
    if(Server->QueFirst>=MaxVarQueue)
      Server->QueFirst=0;
  
    MTBusReq(&Server->MTBusModule,&Server->MTBusDescriptor);  
    }
  else 
    {
    MTBusPostTask(Server);
    } 
  break; 
  
  case MTBUS_MANAGER_TRANSFER_STATE:
  Server->State=MTBUS_MANAGER_GET_ARCH_STATE;
  if(Server->TRQueFirst!=Server->TRQueLast)//Åñëè åñòü â î÷åðåäè äàííûå äëÿ òðàíñïîðòèðîâêè
	{
	if(Server->ServiceKeyEnable)
      Server->MTBusDescriptor.ServiceKey   =  SET_ADDR_SERVICE_KEY;
    else  
      Server->MTBusDescriptor.ServiceKey   =  NONE_SERVICE_KEY;  
      
    Server->MTBusDescriptor.InitiatorAddr  = (uint8_t *)Server;  
    Server->MTBusDescriptor.DevAddr        = Server->TRQueue[Server->TRQueFirst].DevAddr;
    Server->MTBusDescriptor.Command        = _cmdDataTransfer;
    Server->MTBusDescriptor.ProtType       = Server->TRQueue[Server->TRQueFirst].ProtType;
    Server->MTBusDescriptor.State          = Server->TRQueue[Server->TRQueFirst].State;
    Server->MTBusDescriptor.FragCnt        = Server->TRQueue[Server->TRQueFirst].FragCnt;
    Server->MTBusDescriptor.FragIdx        = Server->TRQueue[Server->TRQueFirst].FragIdx;
    Server->MTBusDescriptor.Reserved       = Server->TRQueue[Server->TRQueFirst].Reserved;
    Server->MTBusDescriptor.DataCnt        = Server->TRQueue[Server->TRQueFirst].len;
    Server->MTBusDescriptor.pData          = Server->TRQueue[Server->TRQueFirst].pData;
    Server->MTBusDescriptor.ResponseTime   = Server->MaxResponseTime;
    Server->MTBusDescriptor.SenderFunct    = Server->SenderFunct;
  
    Server->TRQueFirst++;
    
    if(Server->TRQueFirst>=MaxTtansferQueue)
      Server->TRQueFirst=0;
  
    MTBusReq(&Server->MTBusModule,&Server->MTBusDescriptor);  
    }
  else
    {
	MTBusPostTask(Server);
    } 
  break;

  case MTBUS_MANAGER_GET_ARCH_STATE:
  Server->State=MTBUS_MANAGER_READ_VAR_STATE;
  if(ARCH_FREE_RUN==Server->ArchStatus)
   {
  if(ARCH_NO_REQ_TYPE!=Server->ArchReqDescriptor.ArchReqType)
    {
    Server->ArchStatus = ARCH_IN_PROCESS;
    if(Server->ServiceKeyEnable)
      Server->MTBusDescriptor.ServiceKey =  SET_ADDR_SERVICE_KEY;
    else
      Server->MTBusDescriptor.ServiceKey =  NONE_SERVICE_KEY;
  
    Server->MTBusDescriptor.InitiatorAddr  = (uint8_t *)Server;
    Server->MTBusDescriptor.DevAddr        = Server->ArchReqDescriptor.ArchDevAddr;
    Server->MTBusDescriptor.Command        = _cmdGetArch;
	Server->MTBusDescriptor.ArchCellNumber = Server->ArchReqDescriptor.ArchCellNumber;
    Server->MTBusDescriptor.ArchCellCnt    = Server->ArchReqDescriptor.ArchCellCnt;
    Server->MTBusDescriptor.ArchRecCount   = Server->ArchReqDescriptor.ArchRecCount;
    Server->MTBusDescriptor.ResponseTime   = Server->MaxResponseTime;
    Server->MTBusDescriptor.SenderFunct    = Server->SenderFunct;
  
	if(ARCH_GET_CELL_FORMAT_REQ_TYPE==Server->ArchReqDescriptor.ArchReqType)
	  Server->MTBusDescriptor.ArchSign = ARCH_CELL_FORMAT_SIGN;
	if(ARCH_GET_RECORD_REQ_TYPE==Server->ArchReqDescriptor.ArchReqType)
	  Server->MTBusDescriptor.ArchSign = ARCH_REC_SIGN;

  //	  {
  //	  Server->MTBusDescriptor.ArchDay        = 0xFFFF;
  //	  Server->MTBusDescriptor.ArchHalfSec    = 0xFFFFFFFF;
  //	  }
  //  {
	  Server->MTBusDescriptor.ArchDay        = Server->ArchReqDescriptor.ArchDay;
	  Server->MTBusDescriptor.ArchHalfSec    = Server->ArchReqDescriptor.ArchHalfSec;
  //  }
  
    MTBusReq(&Server->MTBusModule,&Server->MTBusDescriptor);
    Server->ArchReqDescriptor.ArchReqType = ARCH_NO_REQ_TYPE;
    }
  else
    MTBusPostTask(Server);
  }

  break;
  
  case MTBUS_MANAGER_READ_VAR_STATE:
    Server->State=MTBUS_MANAGER_WRITE_VAR_STATE;
    if(Server->RequestEnable)
    {
	Server->MTBusDescriptor.ServiceKey     =  NONE_SERVICE_KEY;
	Server->MTBusDescriptor.InitiatorAddr  = (uint8_t *)Server;
	Server->MTBusDescriptor.DevAddr        = Server->NetDevices[Server->ScanDevIdx];
	Server->MTBusDescriptor.Command        = _cmdReadRAM_EXT;
	Server->MTBusDescriptor.IdxRAS         = 0;
	Server->MTBusDescriptor.CntRAS         = 0xFF;
	Server->MTBusDescriptor.ResponseTime   = Server->MaxResponseTime;
	Server->MTBusDescriptor.SenderFunct    = Server->SenderFunct;
	MTBusReq(&Server->MTBusModule,&Server->MTBusDescriptor);
    }
  else
     MTBusPostTask(Server);
  break;

  case MTBUS_MANAGER_ASYNCHR_READ_VAR_LIST_STATE:
    Server->MTBusDescriptor.ServiceKey     =  NONE_SERVICE_KEY;
    Server->MTBusDescriptor.InitiatorAddr  = (uint8_t *)Server;
    Server->MTBusDescriptor.DevAddr        = Server->DeferredDevAddr;
    Server->MTBusDescriptor.Command        = _cmdGetVarList;
    Server->MTBusDescriptor.IdxRAS         = 0;
    Server->MTBusDescriptor.CntRAS         = 0xFF;
    Server->MTBusDescriptor.ResponseTime   = Server->MaxResponseTime;
    Server->MTBusDescriptor.SenderFunct    = Server->SenderFunct;
    MTBusReq(&Server->MTBusModule,&Server->MTBusDescriptor);
    
  break;  
  
  case MTBUS_MANAGER_READ_VAR_LIST_STATE:
    Server->MTBusDescriptor.ServiceKey     =  NONE_SERVICE_KEY;
    Server->MTBusDescriptor.InitiatorAddr  = (uint8_t *)Server;
    Server->MTBusDescriptor.DevAddr        = Server->NetDevices[Server->ScanDevIdx];
    Server->MTBusDescriptor.Command        = _cmdGetVarList;
    Server->MTBusDescriptor.IdxRAS         = 0;
    Server->MTBusDescriptor.CntRAS         = 0xFF;
    Server->MTBusDescriptor.ResponseTime   = Server->MaxResponseTime;
    Server->MTBusDescriptor.SenderFunct    = Server->SenderFunct;
    MTBusReq(&Server->MTBusModule,&Server->MTBusDescriptor);
    
  break;  
  
  default:break;
  }
//------------------
}
MTBusRun(&Server->MTBusModule); 

if (Server->RouteTableDataEn)   //! sky
 { RoutingRun();}
}

//uint32_t cnt__;

void TB_ReqConfirm(MTBusConfirm_t *TBusConf)
{
DTConf_t DTConf;//Äëÿ êîìàíäû cmdDataTransfer
uint8_t  NewDev;//Ïðèõçíàê, òîãî, ÷òî èíôîðìàöèè îá ýòîì óñòðîéñòâå åùå íåò
uint16_t DevIdx; //Èíäåêñ óñòðîéñòâà â òàáëèöå MDevId

MTBusServer_t *Server = (MTBusServer_t *)TBusConf->InitiatorAddr;
uint16_t i;
uint8_t d,n;
//ArchConfirm_t ArchConfirm;

//cnt__++;

switch(TBusConf->Command)
{
case _cmdId:
  if(MTB_SUCCESS_STATUS==TBusConf->Status)
    {
    NewDev = 1;  
    for( i=0;i!=sizeof(MDevId)/sizeof(DevId_t);i++)
      {
      if(TBusConf->DevAddr == MDevId[i].DevAddr)//Åñëè òàêîé àäðåñ óæå åñòü â MDevId
        {
          
        NewDev = 0;
        for( d=0;d!=20;d++)
          MDevId[i].DevName[d]=TBusConf->data[d];
        for( d=0;d!=10;d++)
          MDevId[i].ProgVer[d]=TBusConf->data[d+22];
        break;
        }
      }
    //Åñëè òàêîãî àäðåñà íåòó â MDevId
    if(NewDev)
      {
      //------------------------------
      Server->NetDevices[Server->NetDevCnt++]=TBusConf->DevAddr;
      
      MDevId[NetDevicesCnt].DevAddr=TBusConf->DevAddr;
      MDevId[NetDevicesCnt].HostAddr = Server;
      
      for( n=0;n!=MaxHostPorts;n++)
        {
        if(AssignHostPortAddr[n].HostAddr==MDevId[NetDevicesCnt].HostAddr)  
          MDevId[NetDevicesCnt].Port  = AssignHostPortAddr[n].Port;
        }
            
      
      for( d=0;d!=20;d++)
        MDevId[NetDevicesCnt].DevName[d]=TBusConf->data[d];
      for( d=0;d!=10;d++)
        MDevId[NetDevicesCnt].ProgVer[d]=TBusConf->data[d+22];
      
      NetDevicesCnt++;
      }
    //MTBusAddDevice((MTBusServer_t *)TBusConf->InitiatorAddr,TBusConf->DevAddr);
    }
  
    //Êîíòðîëü äëèòåëüíîãî îòñóòñòâèÿ ïðèáîðà â ñåòè
  DevIdx = 0xFFFF;
  
  for( i=0;i!=MaxDevices;i++)
    {
    if(MDevId[i].DevAddr == TBusConf->DevAddr)
      {
      DevIdx = i;
      break;
      }
    }
  
  if(0xFFFF!=DevIdx)
    {
    if(MTB_SUCCESS_STATUS==TBusConf->Status)
      {
        MDevId[DevIdx].Online = 4;//Ñòàòóñ ïîäãîòîâêè (Id îò ïðèáîðà ïðèøåë, íî äàííûå åùå îæèäàþòñÿ)    
         
        if(0 == MDevId[DevIdx].OnlineTimeOut)
          MDevId[DevIdx].OnlineTimeOut = Server->MaxReceiveInterval;
        else
         MDevId[DevIdx].OnlineTimeOut = Server->ReceiveIntervalMultFactor*MDevId[DevIdx].LastReceiveTime+MDevId[DevIdx].LastReceiveTime/2; 
       
        if (MDevId[DevIdx].OnlineTimeOut > Server->MaxReceiveInterval) 
         MDevId[DevIdx].OnlineTimeOut = Server->MaxReceiveInterval;
        
        if (MDevId[DevIdx].OnlineTimeOut < 1) 
         MDevId[DevIdx].OnlineTimeOut = Server->MaxReceiveInterval;
        
        MDevId[DevIdx].LastReceiveTime = 0;
        MDevId[DevIdx].NotRespCnt      = 0;
      }
    else
      {
      MDevId[DevIdx].NotRespCnt++;
      if(MDevId[DevIdx].NotRespCnt>Server->NotResponceCnt)
        {
        MDevId[DevIdx].Online       = 0;  
        MDevId[DevIdx].NotRespCnt   = 0;
        }
      }
    }

  
break;  

case _cmdReadRAM_EXT: 
if(NEW_VAR_DETECTED==TBusConf->Message)
  {
  TBusConf->Message=FREE;
  Server->State=MTBUS_MANAGER_READ_VAR_LIST_STATE;
  }
else
  {
  if(MTB_SUCCESS_STATUS==TBusConf->Status)
    {
    for( i=0;i!=sizeof(MDevId)/sizeof(DevId_t);i++)
      {
      if(MDevId[i].DevAddr == TBusConf->DevAddr) //Ïîèñê ... 
        {
        if(0==MDevId[i].DevName[0])//Åñëè ïîëå DevId ïóñòîå  
          {
          Server->State = MTBUS_MANAGER_ASYNCHR_GET_ID_STATE;
          Server->DeferredDevAddr = TBusConf->DevAddr;
          }
        }
      }
    if(Server->State != MTBUS_MANAGER_ASYNCHR_GET_ID_STATE)
      {
      //------------------------
      for( i=0;i!=MRAS_COUNT;i++)
        {
        if(TBusConf->DevAddr == MRAS[i].DevAddr)
          {
          if(StrCompare((uint8_t*)"New Variable",MRAS[i].vInfo,16))
            {
            Server->State = MTBUS_MANAGER_ASYNCHR_READ_VAR_LIST_STATE;  
            Server->DeferredDevAddr = TBusConf->DevAddr;
            break;
            }  
          }
        }
      }
    }
  
   
   if ( Server->ScanDevIdx < MaxDevices - 1) //Èíêðåìåíò çíà÷åíèÿ ïî àäðåñó â ScanDevIdx
     Server->ScanDevIdx++;
   
   if(0==Server->NetDevices[Server->ScanDevIdx])
        Server->ScanDevIdx=0;
  }

//Ïîäíèìàåì ñòàòóñ Online

DevIdx = 0xFFFF;

for( i=0;i!=MaxDevices;i++)
  {
  if(MDevId[i].DevAddr == TBusConf->DevAddr)
    {
    DevIdx = i;
    break;
    }
  }

if(0xFFFF!=DevIdx)
  {
  if(MDevId[DevIdx].Online!=0)
    MDevId[DevIdx].Online=1;
  }

break;

case _cmdWriteRAM_EXT: 

  
break;

case _cmdGetVarList:
Server->State=MTBUS_MANAGER_WRITE_VAR_STATE;
  
break;  


case _cmdDataTransfer:


DTConf.DevAddr    = TBusConf->DevAddr;
DTConf.Reserved   = TBusConf->Reserved;
DTConf.DTState    = (DTState_t)TBusConf->DTState;
DTConf.FragCnt    = TBusConf->FragCnt;
DTConf.FragIdx    = TBusConf->FragIdx;
DTConf.ProtType   = (ProtType_t)TBusConf->ProtType;
DTConf.pData      = TBusConf->data;
DTConf.DataCnt    = TBusConf->DataCnt;
  
TbusTrDataReceive(&DTConf);

break;


case _cmdGetArch:

//if(ARCH_CELL_FORMAT_SIGN==Server->MTBusModule.ArchConfirm.ArchSign)
//  Server->MTBusModule.ArchConfirm.ArchReqType = ARCH_GET_CELL_FORMAT_REQ_TYPE;
//else
//  Server->MTBusModule.ArchConfirm.ArchReqType = ARCH_GET_RECORD_REQ_TYPE;

if(ARCH_SUCCES_STATUS == Server->MTBusModule.ArchConfirm.ArchStatus)
  Server->MTBusModule.ArchConfirm.ArchStatus = ARCH_TERMINATOR_SUCCES_STATUS;

if(ARCH_CHS_ERROR_STATUS == Server->MTBusModule.ArchConfirm.ArchStatus)
  Server->MTBusModule.ArchConfirm.ArchStatus = ARCH_TERMINATOR_CHS_ERROR_STATUS;


Server->MTBusModule.ArchConfirmCallBack(&Server->MTBusModule.ArchConfirm);

Server->ArchStatus = ARCH_FREE_RUN;
break;

default: break;
}

  //Êîíòðîëü äëèòåëüíîãî îòñóòñòâèÿ ïðèáîðà â ñåòè
  DevIdx = 0xFFFF;
  
  for( i=0;i!=MaxDevices;i++)
    {
    if(MDevId[i].DevAddr == TBusConf->DevAddr)
      {
      DevIdx = i;
      break;
      }
    }
  
  if(0xFFFF!=DevIdx)
    {
    if(MTB_SUCCESS_STATUS==TBusConf->Status)
      {
      MDevId[DevIdx].Online          = 1;    
      MDevId[DevIdx].NotRespCnt      = 0;
      MDevId[DevIdx].LastReceiveTime = 0;
      }
    else
      {
      MDevId[DevIdx].NotRespCnt++;
      if(MDevId[DevIdx].NotRespCnt>Server->NotResponceCnt)
        {
        MDevId[DevIdx].Online       = 0;  
        MDevId[DevIdx].NotRespCnt   = 0;
        }
      }
    }
MTBusPostTask((MTBusServer_t *)TBusConf->InitiatorAddr);
}


