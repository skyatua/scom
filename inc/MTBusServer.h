//---------------------------------------------------------------------------
//Created by Ihor Bilorus at 
//Last modified by Ihor Bilorus at 27.01.12 added auto get Id mechanism
//Last modified by Ihor Bilorus at 02.03.12 added assign HostPort with HostAddr
//Last modified by Ihor Bilorus at 02.03.12 move RoutingTable array to RoutingTable.c file
//Last modified by Ihor Bilorus at 21.03.12 change Online status mechanism
//Last modified by Ihor Bilorus at 29.03.12 - added Notification
//Last modified by Ihor Bilorus at 29.03.12 - added WriteBuffStorageEnable
//Last modified by Ihor Bilorus at 29.03.12 - added write mas[idx]


#ifndef _MTBusServer_h
#define _MTBusServer_h

#include "MTBusConfiguration.h"
#include "MTBus.h"

typedef enum
{
HOST_PORT_UNKNOWN = 0x00,
HOST_PORT_1       = 0x01,
HOST_PORT_2       = 0x02,
HOST_PORT_3       = 0x03,
HOST_PORT_4       = 0x04,
HOST_PORT_5       = 0x05,
HOST_PORT_6       = 0x06,
HOST_PORT_7       = 0x07,
HOST_PORT_8       = 0x08
}HostPort_t; //������ ������, �� ������� ���� ����� (��������� ���������� � 1)


typedef struct
{
uint8_t 	DevAddr;
uint8_t 	*pData;
uint8_t 	len;
uint8_t 	FragCnt;
uint8_t 	FragIdx;
uint8_t 	Reserved;
MProtType_t     ProtType;
MDTState_t	State;
}TRData_t;

typedef enum
{
MTBUS_MANAGER_INIT_STATE,
MTBUS_MANAGER_FREE_RUN_STATE,
MTBUS_MANAGER_GET_ID_STATE,
MTBUS_MANAGER_ASYNCHR_GET_ID_STATE,
MTBUS_MANAGER_READ_VAR_STATE,
MTBUS_MANAGER_WRITE_VAR_STATE,
MTBUS_MANAGER_READ_VAR_LIST_STATE,
MTBUS_MANAGER_ASYNCHR_READ_VAR_LIST_STATE,
MTBUS_MANAGER_TRANSFER_STATE,
MTBUS_MANAGER_GET_ARCH_STATE
}MTBusManagerState_t;

typedef enum
{
ARCH_FREE_RUN	    = 0x00,
ARCH_IN_PROCESS     = 0x01
}ArchStatus_t;


typedef struct RoutTable
{
uint8_t * sDevAddr;//������� ����� �������-���������
uint16_t sVidx;   //����� ���������� �� ����� �������-���������
uint8_t * dDevAddr;//������� ����� �������-����������
uint16_t dVidx;   //����� ���������� �� ����� �������-����������
}RoutTable_t;


//*****����������, ����������� � ��������� ���������� ��������������� ������**//
typedef struct
{
uint8_t           DevAddr;  //����� ������� �������� ����� ������������ ������ �������� _cmdDataTransfer
uint8_t           *pData;   //��������� �� ������, ������� ����� ������������
uint8_t           len;      //����� ����� � ������
MProtType_t       ProtType;
uint8_t           Reserved;
MDTState_t        State;
uint8_t 	  FragCnt;
uint8_t           FragIdx;
}TtansferQueue_t;//��������� ��� �������� ���������� �� ���������� ������� _cmdDataTransfer

//************����������, ����������� � ��������� ���������� ������************//
typedef struct
{
uint16_t MRasIdx;
uint8_t  IdxMas;//������ �������� �������
uint8_t  vCnt;  //����������� ������������ ��������� ������� � IdxMas
uint8_t  *pData;
}WriteQueue_t; //��������� ��� �������� ���������� �� ���������� ������� ������





typedef struct
{
//��������� �������������
void (*SenderFunct)(uint8_t *pData, uint16_t len);//��������� �� ������������ �������� len ���� �� ����������
uint8_t   MaxNetAddr;//���������� ����� �������, ������� ����� ���� � ����
uint16_t  MaxResponseTime;
uint8_t   RequestEnable;       //��������� ��� ��������� ����� �������� (�������� ������ � �������� �������� ���������)
uint16_t  MaxReceiveInterval;        //������������ ���������� �������, �� ������� ������� ������ �������� ������ �� ������� 
uint16_t  ReceiveIntervalMultFactor; //�����, ������������ �� ������� ��� ������� �� ������ � ������� ������� Online ����� ��������� �������� ������
uint16_t  FlowAnaliserTimeOut; //�����, �� ��������� �������� ������������ ���������� �������� ������
uint16_t  NotResponceCnt;      //���������� �������� ��� ������, ���� �������� ���������� ��������� Offline  
uint8_t   ServiceKeyEnable;
uint8_t   WriteBuffStorageEnable;//���� 1, �� ����� ������ ��������� ������ ���������� ������ ������������ � � ����� �������� ������, ���� 0, �� ������ ����������, � � MRAS ����������� ���������� ����� ����������� ���������� 

void (*ArchConfirmCallBack)(ArchConfirm_t *ArchConfirm);//��������� �� ������������, ������� ���������� ����� ��������� ��������� ������
NotificationResp_t (*NoticationCallBack)(NotificationReq_t *Notification);
//------------------------------------------------------------------------------


//****************************************************************************//
WriteQueue_t        WriteQueue[MaxVarQueue];
uint8_t             QueBuff[MaxVarQueue*10];
uint8_t             QueBuffIdx;
uint8_t             QueFirst;
uint8_t             QueLast;
//****************************************************************************//

//****************************************************************************//
uint8_t             TRQueBuff[250*MaxTtansferQueue];//����� ��� �������� ������, ������������ ��������� _cmdDataTransfer
TtansferQueue_t     TRQueue[MaxTtansferQueue];
uint8_t             TRQueBuffIdx;                   //����� �������� �������� TRDataBuff
uint8_t             TRQueFirst;
uint8_t             TRQueLast;
//****************************************************************************//

MTBusDescriptor_t   MTBusDescriptor;
ArchReqDescriptor_t ArchReqDescriptor;
ArchStatus_t	    ArchStatus;

MTBusManagerState_t State;
uint8_t             DeferredDevAddr;//����� �������, ������� ����� �� ������� ����������� �������

uint8_t             MTBusPostTaskEn;
uint8_t             NetDevCnt;
uint8_t             ScanDevIdx;
uint8_t             SearchDevAddr;
uint8_t             NetDevices[MaxDevices];//����������, ������� ���� ������� ��� ������������, ��� ���������
        
uint16_t            msec;
uint16_t            RouteTableDataEn;      //! sky ���������� �������� ������ ������� �������������       

MTBusModule_t       MTBusModule;   //������ ������� ������ ��������� (���� MTerabus)
}MTBusServer_t;

typedef struct
{
HostPort_t    Port;      //����� �����
MTBusServer_t *HostAddr; //����� ������� � ������
}AssignHostPortAddr_t;//��������� ������������� ������ ������������ ������� �������� ������ � ������� ��� �����
//1-��� ���� ������������� ���� �������, ������� ������������������ ������, 2-� ����, ��� ������������������ ������ � �.�.


typedef struct _DevId_
{
uint8_t       DevAddr;           //����� �������
HostPort_t    Port;              //����� �����, � ���� �������� ��������� ������ � ������� DevAddr
uint8_t       Online;            //1 - ������ � ����, 0 - �� � ���� (�� ��������)
uint8_t       NotRespCnt;        //����������� �������� ��� ������ ��� ���������� � ������� DevAddr
MTBusServer_t *HostAddr;         //��������� �� ������, � ���������� �������� ��������� ������ � ������� DevAddr
uint8_t       DevName[20];       //��� �������
uint8_t       ProgVer[10];       //������ ��������� �������
uint16_t      LastReceiveTime;   //����� �� ���������� ����������� ���������
uint16_t      OnlineTimeOut;     //�����, �� ��������� �������� ���������� ������ �� OnLine  
}DevId_t;

extern DevId_t MDevId[MaxDevices];
extern AssignHostPortAddr_t AssignHostPortAddr[MaxHostPorts];

extern void MTBusServerInit(MTBusServer_t *Server);
extern void MTBusServerTimeOutCounter(MTBusServer_t *Server);
extern void MTBusServerDataReceiver(MTBusServer_t *Server,uint8_t byte);
extern void MTBusServerRun(MTBusServer_t *Server);

extern void MTBusAddDevice(MTBusServer_t *Server,uint8_t DevAddr);
extern void MTBusServerStartSearch(MTBusServer_t *Server);
extern void MTBusDelDevice(MTBusServer_t *Server,uint8_t DevAddr);

extern void MTBusArchRequest(MTBusServer_t *Server,ArchReqDescriptor_t *ArchReqDescriptor);//������ �������� ��������

extern void ChangeMRASValue(uint16_t MRASn,uint8_t *pData,uint8_t IdxMas,uint8_t vCnt);
extern void TransferData(TRData_t *TRData);

#endif
