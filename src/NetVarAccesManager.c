#include "../inc/includes.h"

//#include "NetVarAccesManager.h"


typedef struct
{
uint8_t   DevAddr;
uint16_t  IdxNetVar;
uint8_t   HostInterfase;  
uint16_t  MRASIdx;
}SwitchVar_t;

SwitchVar_t SwitchVar;


void SwitchAcces(uint8_t DevAddr,uint16_t IdxNetVar,uint8_t HostInterface)
{
SwitchVar.DevAddr       = DevAddr;
SwitchVar.IdxNetVar     = IdxNetVar;
SwitchVar.HostInterfase = HostInterface;
}


VariableStatus_t GetVariableStatus(uint8_t DevAddr,uint16_t vIdx)
{
	uint16_t i;
VariableStatus_t Status = VARIABLE_NOT_INIT_STATUS;  
for(i=0;i!=MRAS_COUNT;i++)
  {
  if(DevAddr==MRAS[i].DevAddr)
    {
    if(vIdx==MRAS[i].vIdx)
      {
      Status = MRAS[i].Status;
      break;
      }
    }
  }
return Status;
}


_VarType_t GetVariableType(uint8_t DevAddr,uint16_t vIdx)//Ïîëó÷åíèå òèïà ïåðåìåííîé ñ óêàçàíèåì àäðåñà óñòðîéñòâà
{
	uint16_t i;
_VarType_t VarType = _uint8_t_;  
for(i=0;i!=MRAS_COUNT;i++)
  {
  if(DevAddr==MRAS[i].DevAddr)
    {
    if(vIdx==MRAS[i].vIdx)
      {
      VarType = (_VarType_t)MRAS[i].vType;
      break;
      }
    }
  }
return VarType;
}


_VarType_t GetVarType(void)
{
	uint16_t i;
_VarType_t VarType = _uint8_t_;  
if((SwitchVar.DevAddr==MRAS[SwitchVar.MRASIdx].DevAddr)&&(SwitchVar.IdxNetVar==MRAS[SwitchVar.MRASIdx].vIdx))
  {
  VarType = (_VarType_t)MRAS[SwitchVar.MRASIdx].vType;
  return VarType;
  }
for(i=0;i!=MRAS_COUNT;i++)
  {
  if((SwitchVar.DevAddr==MRAS[i].DevAddr)&&(SwitchVar.IdxNetVar==MRAS[i].vIdx))
    {
    SwitchVar.MRASIdx=i;  
    VarType = (_VarType_t)MRAS[SwitchVar.MRASIdx].vType;
    return VarType;
    }
  }
return VarType;
}

uint8_t GetAccessVarType(void)
{
	uint16_t i;
 uint8_t accesstype = 0;  
if((SwitchVar.DevAddr==MRAS[SwitchVar.MRASIdx].DevAddr)&&(SwitchVar.IdxNetVar==MRAS[SwitchVar.MRASIdx].vIdx))
  {
  accesstype = (_VarType_t)MRAS[SwitchVar.MRASIdx].vWRE;
  return accesstype;
  }
for(i=0;i!=MRAS_COUNT;i++)
  {
  if((SwitchVar.DevAddr==MRAS[i].DevAddr)&&(SwitchVar.IdxNetVar==MRAS[i].vIdx))
    {
    SwitchVar.MRASIdx=i;  
    accesstype = (_VarType_t)MRAS[SwitchVar.MRASIdx].vWRE;
    return accesstype;
    }
  }
return accesstype;
}


uint8_t * GetDataPointer(void)
{
	uint16_t i;
uint8_t *pData = (void*)0;  
if((SwitchVar.DevAddr==MRAS[SwitchVar.MRASIdx].DevAddr)&&(SwitchVar.IdxNetVar==MRAS[SwitchVar.MRASIdx].vIdx))
  {
  pData = MRAS[SwitchVar.MRASIdx].pData;
  return pData;
  }
for(i=0;i!=MRAS_COUNT;i++)
  {
  if((SwitchVar.DevAddr==MRAS[i].DevAddr)&&(SwitchVar.IdxNetVar==MRAS[i].vIdx))
    {
    SwitchVar.MRASIdx=i;  
    pData = MRAS[SwitchVar.MRASIdx].pData;
    return pData;
    }
  }
return pData;
}

uint8_t *  GetDataPtr(uint8_t DevAddr,uint8_t vIdx)//Âîçâðàùàåò óêàçàòåëü íà äàííûå ïåðåìåííîé vIdx ïðèáîðà DevAddr
{
	uint16_t i;
uint8_t *pData = (void*)0;  

for(i=0;i!=MRAS_COUNT;i++)
  {
  if((DevAddr==MRAS[i].DevAddr)&&(vIdx==MRAS[i].vIdx))
    {
    pData = MRAS[i].pData;
    break;
    }
  }
return pData;
}



uint8_t * GetTextInfo(void)
{
	uint16_t i;
uint8_t *pInfo = (void*)0;  
if((SwitchVar.DevAddr==MRAS[SwitchVar.MRASIdx].DevAddr)&&(SwitchVar.IdxNetVar==MRAS[SwitchVar.MRASIdx].vIdx))
  {
  pInfo = MRAS[SwitchVar.MRASIdx].vInfo;
  return pInfo;
  }
for(i=0;i!=MRAS_COUNT;i++)
  {
  if((SwitchVar.DevAddr==MRAS[i].DevAddr)&&(SwitchVar.IdxNetVar==MRAS[i].vIdx))
    {
    SwitchVar.MRASIdx=i;  
    pInfo = MRAS[SwitchVar.MRASIdx].vInfo;
    return pInfo;
    }
  }
return pInfo;
}

uint8_t  GetVarCnt(void)
{
	uint16_t i;
uint8_t vCnt = 0;  
if((SwitchVar.DevAddr==MRAS[SwitchVar.MRASIdx].DevAddr)&&(SwitchVar.IdxNetVar==MRAS[SwitchVar.MRASIdx].vIdx))
  {
  vCnt = MRAS[SwitchVar.MRASIdx].vCnt;
  return vCnt;
  }
for(i=0;i!=MRAS_COUNT;i++)
  {
  if((SwitchVar.DevAddr==MRAS[i].DevAddr)&&(SwitchVar.IdxNetVar==MRAS[i].vIdx))
    {
    SwitchVar.MRASIdx=i;  
    vCnt = MRAS[SwitchVar.MRASIdx].vCnt;
    return vCnt;
    }
  }
return vCnt;
}

uint8_t GetOnlineDevCnt(void)
{uint8_t i;
uint8_t OnlineCnt = 0;  
for(i=0;i!=MaxDevices;i++)
  {
  if((MDevId[i].Online)&&(0!=MDevId[i].DevAddr))
    OnlineCnt++;
  }
return OnlineCnt;
}


uint8_t GetDevOnlineStatus(uint8_t DevAddr)
{
  uint8_t i;	
 for(i=0;i!=MaxDevices;i++)
  {
  if (DevAddr != MDevId[i].DevAddr) continue;
   else
   {
    if (MDevId[i].Online) return 1;
    else
      return 0;
   }
  }
  return 0;
}

// 
uint8_t GetDevFirstEntryInNetwork(uint8_t DevAddr)
{
	uint8_t i;
 for(i=0; i < MaxDevices; i++)
  {
   if (DevAddr == MDevId[i].DevAddr) 
   {  return 1; }
  }
  return 0;
}


uint8_t *GetDevName(uint8_t DevAddr)
{
	uint8_t i;
uint8_t *pName = (void *)0;
for(i=0;i!=MaxDevices;i++)
  {
  if(DevAddr==MDevId[i].DevAddr)
    {
    pName = MDevId[i].DevName;
    return pName;
    }
  }
return pName;
}


uint8_t *GetProgVer(uint8_t DevAddr)
{uint8_t i;
uint8_t *pVer = (void *)0;
for( i=0;i!=MaxDevices;i++)
  {
  if(DevAddr==MDevId[i].DevAddr)
    {
    pVer = MDevId[i].ProgVer;
    return pVer;
    }
  }
return pVer;
}

HostPort_t GetHostPort(uint8_t DevAddr)
{uint8_t i;
HostPort_t HostPort = HOST_PORT_UNKNOWN;
for( i=0;i!=MaxDevices;i++)
  {
  if(DevAddr==MDevId[i].DevAddr)
    {
    HostPort = MDevId[i].Port;
    return HostPort;
    }
  }
return HostPort;
}

uint16_t GetVarAllCntOfDevice(uint8_t DevAddr)
{
	uint16_t i;
uint16_t VarCnt = 0;  
for( i=0;i!=MRAS_COUNT;i++)
  {
  if(DevAddr==MRAS[i].DevAddr)
    VarCnt++;
  }
return VarCnt;
}

uint8_t GetMaxDevAddr(HostPort_t Port)
{uint8_t n;
uint8_t MaxDevAddr = 0;
for( n=0;n!=MaxHostPorts;n++)
  {
  if(Port == AssignHostPortAddr[n].Port)
    {
    MaxDevAddr=AssignHostPortAddr[n].HostAddr->MaxNetAddr;
    break;
    }
  }
return MaxDevAddr;
}

uint8_t GetCurrSearchAddr(HostPort_t Port)
{uint8_t n;
uint8_t SeachDevAddr = 0;
for( n=0;n!=MaxHostPorts;n++)
  {
  if(Port == AssignHostPortAddr[n].Port)
    {
    SeachDevAddr=AssignHostPortAddr[n].HostAddr->SearchDevAddr;
    break;
    }
  }
return SeachDevAddr;  
}

uint8_t GetDevAddr(uint8_t idx)
{
 return MDevId[idx].DevAddr;  
}

uint8_t GetCurrReqAddr(HostPort_t Port)
{uint8_t n;
uint8_t ScanDevAddr = 0;
uint8_t ScanDevIdx = 0;
for( n=0;n!=MaxHostPorts;n++)
  {
  if(Port == AssignHostPortAddr[n].Port)
    {
    ScanDevIdx  = AssignHostPortAddr[n].HostAddr->ScanDevIdx; 
    ScanDevAddr = AssignHostPortAddr[n].HostAddr->NetDevices[ScanDevIdx];
    break;
    }
  }
return ScanDevAddr;  
}

void AddNetDevice(HostPort_t Port,uint8_t DevAddr)
{
	uint8_t n;
for(n=0;n!=MaxHostPorts;n++)
  {
  if(Port == AssignHostPortAddr[n].Port)
    {
    MTBusAddDevice(AssignHostPortAddr[n].HostAddr,DevAddr);
    break;
    }
  }
}

void WriteValueToVariable(uint8_t DevAddr,uint16_t vIdx,uint8_t *pData)
{
	uint16_t i;
for( i=0;i!=MRAS_COUNT;i++)
  {
  if((DevAddr==MRAS[i].DevAddr)&&(vIdx==MRAS[i].vIdx))
    {
    ChangeMRASValue(i,pData,0x00,0xff);
    break;
    }
  }
}

void StartNetSearch(HostPort_t Port)
{uint8_t n;
for( n=0;n!=MaxHostPorts;n++)
  {
  if(Port == AssignHostPortAddr[n].Port)
    {
    MTBusServerStartSearch(AssignHostPortAddr[n].HostAddr);
    break;
    }
  }
}

void SetNetSearchAddr(HostPort_t Port, uint8_t DevAddr)
{
 uint8_t n;
 for(n=0;n!=MaxHostPorts;n++)
  {
  if(Port == AssignHostPortAddr[n].Port)
    {
    AssignHostPortAddr[n].HostAddr->SearchDevAddr = DevAddr;
    break;
    }
  }
}

uint16_t GetWrQueueVarCnt( uint8_t DevAddr )
{
  uint16_t i;
  uint8_t n;	
  HostPort_t HostPort = HOST_PORT_UNKNOWN;
  uint16_t last = 0,
           first = 0;
  for(i=0;i != MaxDevices;i++)
  {
  if(DevAddr==MDevId[i].DevAddr)
    {
     HostPort = MDevId[i].Port;
    
     for( n=0;n!=MaxHostPorts;n++)
     {
      if(HostPort == AssignHostPortAddr[n].Port)
       {
         last = AssignHostPortAddr[n].HostAddr->QueLast;
         first = AssignHostPortAddr[n].HostAddr->QueFirst;
         if ( last > first)
           return ( last - first );
         else
           return ( first - last );
       }
      }
    }
  }
  return 0;
}

void DelNetDevice (HostPort_t Port,uint8_t DevAddr)
{	uint8_t n;
	
for( n=0;n!=MaxHostPorts;n++)
  {
  if(Port == AssignHostPortAddr[n].Port)
    {
    MTBusDelDevice(AssignHostPortAddr[n].HostAddr,DevAddr);
    break;
    }
  }
}

void WriteValueToMasVariable(uint8_t DevAddr,uint16_t vIdx,uint8_t IdxMas,uint8_t vCnt,uint8_t *pData)
{
uint16_t i;	
for(i=0;i!=MRAS_COUNT;i++)
  {
  if((DevAddr==MRAS[i].DevAddr)&&(vIdx==MRAS[i].vIdx))
    {
    ChangeMRASValue(i,pData,IdxMas,vCnt);
    break;
    }
  }
}


MTBusServer_t *GetHostServer(uint8_t DevAddr)
{ uint16_t i;
  uint8_t n;
MTBusServer_t *Server=(void *)0;

HostPort_t HostPort = HOST_PORT_UNKNOWN;

for( i=0;i != MaxDevices;i++)
  {
  if(DevAddr==MDevId[i].DevAddr)
    {
     HostPort = MDevId[i].Port;
    
     for( n=0;n!=MaxHostPorts;n++)
      {
      if(HostPort == AssignHostPortAddr[n].Port)
       {
       Server = AssignHostPortAddr[n].HostAddr;
       }
      }
    }
  }
return Server;  
}

static ArchReqDescriptor_t ArchReqDescriptor;

void RequestArchRecords(uint8_t DevAddr,uint16_t Day,uint32_t HalfSec,uint16_t RecCount)
{
MTBusServer_t *HostServer = GetHostServer(DevAddr);

ArchReqDescriptor.ArchReqType   = ARCH_GET_RECORD_REQ_TYPE;
ArchReqDescriptor.ArchDevAddr   = DevAddr;
ArchReqDescriptor.ArchDay       = Day;
ArchReqDescriptor.ArchHalfSec   = HalfSec;
ArchReqDescriptor.ArchRecCount  = RecCount;

if(((void *)0)!=HostServer)
  MTBusArchRequest(HostServer,&ArchReqDescriptor);
}


void RequestArchCellFormat(uint8_t DevAddr,uint16_t Day,uint32_t HalfSec,uint16_t CellNumber,uint16_t CellCnt)
{
MTBusServer_t *	 HostServer = GetHostServer(DevAddr);

ArchReqDescriptor.ArchReqType     = ARCH_GET_CELL_FORMAT_REQ_TYPE;
ArchReqDescriptor.ArchDevAddr     = DevAddr;
ArchReqDescriptor.ArchDay         = Day;
ArchReqDescriptor.ArchHalfSec     = HalfSec;
ArchReqDescriptor.ArchCellNumber  = CellNumber;
ArchReqDescriptor.ArchCellCnt     = CellCnt;

if(((void *)0)!=HostServer)
  MTBusArchRequest(HostServer,&ArchReqDescriptor);
}

uint8_t GetRoutingMode(uint8_t DevAddr)
{
 MTBusServer_t * HostServer = GetHostServer(DevAddr);
 if ( HostServer != (void*)0 )
   return HostServer->RouteTableDataEn;
 else
   return 0;
}

void SetRoutingMode(uint8_t DevAddr, uint8_t Mode)
{
 MTBusServer_t * HostServer = GetHostServer(DevAddr);
 if ( HostServer != (void*)0 )
    HostServer->RouteTableDataEn = Mode;
}

