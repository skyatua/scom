//Created by Ihor Bilorus
//---------------------------------------------------------------------------
//****************������ ��������� TeraBus(Master) ver 4.0 beta***************// 

#ifndef _MTBus_h
#define _MTBus_h

#include "MTBusConfiguration.h"

#include <stdint.h>

//Last modified 20.12.11 by Ihor Bilorus (added function _cmdDataTransfer)
//Last modified 12.01.12 by Ihor Bilorus (modified function _cmdDataTransfer)
//Last modified 16.01.12 by Ihor Bilorus (optimization algoritm by memory)
//Last modified 02.03.12 by Ihor Bilorus (changed "����� ����������" to "New Variable")
//Last modified by Ihor Bilorus at 29.03.12 - added Notification
//Last modified by Ihor Bilorus at 05.04.12 - added _cmdGetArch
//Last modified by Ihor Bilorus at 18.04.12 - added Variable Status


//***********************�������� �������������� ����������*******************//
typedef enum
{
_cmdReadRAM       = 0x02, //������ ���
_cmdWriteRAM      = 0x03, //������ � ���
_cmdReadEEPROM    = 0x12, //������ EEPROM
_cmdWriteEEPROM   = 0x13, //������ EEPROM
_cmdReadFlash     = 0x42, //������ Flash
_cmdWriteFlash    = 0x43, //������ �� Flash
_cmdId            = 0x20, //������������� �������
_cmdSetAddr       = 0x21, //��������� ������ �������
//_cmdArchParams    = 0x22, //������ ������������ ������
//_cmdArchData      = 0x23, //������ �������� �������
_cmdGetArch	  = 0x24, //����������� ������
_cmdGetVarList    = 0x25, //����������� ������ ���������� �������
_cmdReadRAM_EXT   = 0x26, //����������� ������ RAM
_cmdWriteRAM_EXT  = 0x27, //����������� ������ � ���
_cmdDataTransfer  = 0x35  //�������� ��������������� ������������ ������
}TbusCmd_t;
//****************************************************************************//

//************************���� ������ �������� ���������**********************//
typedef enum
{
_uint8_t_         = 0x00,
_int8_t_          = 0x01,
_uint16_t_        = 0x02,
_int16_t_         = 0x03,
_uint32_t_        = 0x04,
_int32_t_         = 0x05,
_float_t_         = 0x06,
_DateTime_t_      = 0x08,
_NoInit           = 0xFF
}_VarType_t;
//****************************************************************************//

//**********************�������� ������ �������� ���������********************//
typedef enum
{
_errNone          = 0x00,
_errCHS           = 0xF0,
_errUnknownCmd    = 0xF1,
//_errInvalidCount    = 0xF2,
//_errInvalidType     = 0xF3,
//_errInvalidDataLen  = 0xF4,
//_errInvalidData     = 0xF5,
//_errExecution       = 0xF6,
//_errFrame           = 0xF7,
//_errPending         = 0xF8
}TbusErr_t;
//****************************************************************************//


typedef enum
{
NONE_SERVICE_KEY      = 0x00000000,//0x0000000000000000L,
SET_ADDR_SERVICE_KEY  = 0x00000001//0x10570AC00E0F0010L
}ServiceKey_t;


//*******************������ ����������� ��������******************************//
typedef enum
{
MTB_FREE_RUN_STATUS,      //�������
MTB_SUCCESS_STATUS,       //�������� ���������� �������
MTB_BAD_STATUS,           //�������� ��������� ��������� (��-�� ������ ������ � �.�.)
MTB_PART_SUCCESS_STATUS,  //�������� �������� ��������� ������� (��������� ���������� �������� � ��������)
MTB_CHS_ERROR_STATUS,     //�������� ��������� � �������� ����������� �����
MTB_NO_RESPONSE_STATUS,   //�������� ���������, �� ������������ ���������� �� ��������
}MTBusStatus_t;     
//****************************************************************************//



typedef enum
{
VARIABLE_NOT_INIT_STATUS   = 0x00, //���������� �� ����������������
VARIABLE_PART_INIT_STATUS  = 0x01, //���������� �������� ����������������
VARIABLE_IS_INIT_STATUS    = 0x02, //���������� ����������������
}VariableStatus_t;


typedef enum
{
_SIGNATURE_ANALISE_STATE,      //��������� ������� ����������������� �����
_DEV_ADDR_RECEIVE_STATE,       //��������� ������� �������� ������ ����������
_OLD_DEV_ADDR_RECEIVE_STATE,   //��������� ������ ������� �������� ������ ���������� (��� �������� ����� ������)
_COMMAND_ANALISE_STATE,        //��������� ������� � ������ �������� ���������
_ID_RECEIVE_STATE,             //��������� ������ ID ����������
_IDX_RAS_RECEIVE_STATE,        //��������� ������ ����������� ������ ����������
_CNT_RAS_RECEIVE_STATE,        //��������� ������ ����������� ������������� ����������
_IDX_MAS_RECEIVE_STATE,        //��������� ������ ������� � ������
_VAR_ADDR_RECEIVE_STATE,       //��������� ������ ����������� ������ ���������� � ������ (��� ������ �������)
_VAR_CNT_RECEIVE_STATE,        //��������� ������ ����������� ���������� � ����������� �������
_VAR_TYPE_ANALISE_STATE,       //��������� ������ ���� ����������
_VAR_WRITE_EN_RECEIVE_STATE,   //��������� ������ ���������� �� ������
//ARCH_REC_NO_RECEIVE_STATE,
//ARCH_REC_CNT_RECEIVE_STATE,

//*************�����****************
_ARCH_SIGN_RECEIVE_STATE,
_ARCH_DAY_RECEIVE_STATE,
_ARCH_HALF_SEC_RECEIVE_STATE,
_ARCH_PERIOD_RECEIVE_STATE,
_ARCH_REC_SIZE_RECEIVE_STATE,
_ARCH_REC_COUNT_RECEIVE_STATE,
_ARCH_CELL_NUMBER_RECEIVE_STATE,
_ARCH_CELL_CNT_RECEIVE_STATE,
_ARCH_CELL_FORMAT_RECEIVE_STATE,
_ARCH_REC_RECEIVE_STATE,       //��������� ������ �������� �������
//*********************************

_CHS_ANALISE_STATE,            //��������� ������ � ��������� ����������� �����
_RESERVED_RECEIVE_STATE,       //��������� ������ ���������� ����� ��� �������� cmdDataTransfer
_STATE_RECEIVE_STATE,          //��������� ������ ��������� ����������� ������� cmdDataTransfer
_FRAG_CNT_IDX_RECEIVE_STATE,   //��������� ������ ����������� ���������� � ������ �������� ���������
_PROT_TYPE_RECEIVE_STATE,      //��������� ������ ���� ���������, ����������� �� ������ �������� cmdDataTransfer
_DATA_CNT_RECEIVE_STATE,       //��������� ������ ����������� ���� �������� cmdDataTransfer
_DATA_RECEIVE_STATE            //��������� ������ ��������� ������
}MParcerState_t;//��������� �������� ����������� �������� ������ ������



typedef enum 
{
NO_PROTOCOL_   = 0x00,
BOOT_PROTOCOL_ = 0x01
}MProtType_t;


typedef enum
{
_Request = 0x01,
_Answer  = 0x02,
_Next  	 = 0x03,
_Retry   = 0x04
}MDTState_t;   

typedef enum
{
FREE,
NEW_VAR_DETECTED,
}MTBusMessage_t;



typedef enum
{
NEW_DEVICE_FOUND_NTF_STATUS         = 0x01,
NEW_DEVICE_SUCCES_ADDED_NTF_STATUS  = 0x02,
}NoticationReqStatus_t;


typedef enum
{
NEW_DEVICE_ADDED_ENABLE             = 0x01,
NEW_DEVICE_ADDED_DISABLE            = 0x02,
}NoticationRespStatus_t;

typedef enum
{
ARCH_NO_REQ_TYPE		    = 0x00,
ARCH_GET_CELL_FORMAT_REQ_TYPE	    = 0x01,
ARCH_GET_RECORD_REQ_TYPE	    = 0x02,
}ArchReqType_t;

typedef enum
{
ARCH_SUCCES_STATUS	            = 0x00,  //�������� ������ �������� �������
ARCH_CHS_ERROR_STATUS		    = 0x01,  //�������� ������ �������� � ������� CRC
ARCH_NOT_RESP_STATUS  		    = 0x02,  //�������� ������ �� �������� ����� �������

ARCH_TERMINATOR_SUCCES_STATUS	    = 0x03,  //������� ������� ������� ���������� ������ ������� �������
ARCH_TERMINATOR_CHS_ERROR_STATUS    = 0x04   //������� ������� ���������� ������ ������� ������� � �������� CRC
}ArchOperationStatus_t;

typedef enum
{
ARCH_REC_SIGN 	 	  = 0x01, //������� ������ �������� �������
ARCH_CELL_FORMAT_SIGN     = 0x02, //������� ������ ������� �������� �����
ARCH_BORDER_SIGN	  = 0x04  //������� ������ �������� ����� ���������� �������������� �������
}ArchSign_t; //������� � ������� ������ ������


typedef struct
{
uint8_t           ArchDevAddr;
ArchReqType_t     ArchReqType;
uint16_t  	      ArchDay;
uint32_t  	      ArchHalfSec;
uint16_t 	      ArchPeriod;	   //������ �������������
uint16_t 	      ArchRecSize;	   //������ �������� ������
//uint32_t  	      ArchRecCount;
uint16_t  	      ArchCellNumber;
uint16_t  	      ArchCellCnt;
uint8_t 	      *pBuff;
ArchOperationStatus_t ArchStatus;
}ArchConfirm_t;


typedef struct
{
ArchReqType_t ArchReqType;
uint8_t   	  ArchDevAddr;
ArchSign_t	  ArchSign;	
uint16_t  	  ArchDay;
uint32_t  	  ArchHalfSec;
uint32_t  	  ArchRecCount;
uint16_t	  ArchPeriod;
uint16_t  	  ArchCellNumber;
uint16_t  	  ArchCellCnt;
}ArchReqDescriptor_t;



typedef struct
{
uint8_t               DevAddr;
NoticationReqStatus_t Status;
}NotificationReq_t;  
  
typedef struct
{
NoticationRespStatus_t Status;
}NotificationResp_t;


//**************������� ��� �������� ���������� � ���������� � ����***********//
typedef struct
{
uint8_t           DevAddr;   //����� �������
uint16_t          vIdx;      //���������� ����� ���������� � �������
uint8_t           vType;     //��� ����������
uint8_t           vCnt;      //����������� ���������� (1 - ���� ����, >1 ��� �������)
uint8_t           vWRE;      //���������� ������
uint8_t           vInfo[18]; //�������� ���������� 
uint8_t           *pData;     //��������� �� ������ ��� ������ ���������� 
VariableStatus_t  Status;
}MRAS_t;
//****************************************************************************//

typedef struct
{
uint8_t           *InitiatorAddr;//����� ����������, ���������� ����� (����� ������� �������� ������)   
uint8_t           DevAddr;
uint8_t           OldDevAddr;
TbusCmd_t         Command;
uint8_t           Reserved;    //����������������� ���� ��� �������� cmdDataTransfer
MDTState_t        DTState;     //��������� ���������� �������  cmdDataTransfer
uint8_t           FragCnt;     //����������� ����������
uint8_t           FragIdx;     //����� ���������
MProtType_t       ProtType;    //��� ���������, ����������� �� ������ �������� cmdDataTransfer
uint8_t           DataCnt;
uint32_t          VarAddr;
_VarType_t        VarType;
uint8_t           VarCnt;
uint8_t           VarWRE;
uint8_t           *data;

uint16_t          ArchDay; 	   //����, ������� � 1900 ����
uint32_t  	  ArchHalfSec;    //���������� ���������� � ������ �����

uint32_t 	  ArchRecCount;   //������������� ���������� �������� �������
uint16_t	  ArchCellNumber; //����� ������
uint16_t	  ArchCellCnt;    //���������� ������������� �����
uint16_t 	  ArchRecSize;    //������ �������� ������ � ��

ArchReqType_t     ArchReqType;
uint16_t	  ArchPeriod;

struct
  {
  uint32_t        BadVar;//������ ����������, ��� ����������� ������� ���� ��������
  MTBusStatus_t   VarErr;
  }Problem[10]; //�� �������� ���� ������

MTBusMessage_t    Message;
uint16_t     IdxRAS;
uint16_t     CntRAS;
uint16_t     IdxMAS;
uint16_t     ResponseTime;
MTBusStatus_t Status;
}MTBusConfirm_t;

typedef struct
{
ServiceKey_t     ServiceKey;   
uint8_t          *InitiatorAddr;//����� ����������, ���������� ����� (����� ������� �������� ������)  
uint8_t          DevAddr;
uint8_t          NewDevAddr;
TbusCmd_t        Command;
uint32_t         VarAddr;
uint8_t          VarCnt;
MDTState_t       State;
uint8_t 	 FragCnt;
uint8_t 	 FragIdx;
MProtType_t      ProtType;
uint8_t          Reserved;
uint8_t          DataCnt;

//******��������� ��� ������ ������******
ArchSign_t	 ArchSign;
uint16_t 	 ArchDay; 	  //����, ������� � 1900 ����
uint32_t 	 ArchHalfSec;     //���������� ���������� � ������ �����
uint32_t	 ArchRecCount;    //������������� ���������� �������� �������
uint16_t         ArchCellNumber;  //����� ������
uint16_t	 ArchCellCnt;     //���������� ������������� �����
//***************************************

_VarType_t       VarType;
uint16_t         IdxRAS;
uint16_t         IdxMAS;
uint16_t         CntRAS;
uint8_t          *pData;
uint16_t         ResponseTime;
void (*SenderFunct)(uint8_t *pData, uint16_t len);

}MTBusDescriptor_t;


typedef struct
{
uint8_t            Signature;    //���� ������������� 0�50 (��� �� ������������ ��� ������� ������������� �������)
uint8_t            DevAddr;      //����� ����������
uint8_t            OldDevAddr;   //������ ����� ����������
TbusCmd_t          Command;      //��������
uint8_t            Reserved;     //����������������� ���� ��� �������� cmdDataTransfer
MDTState_t         DTState;      //��������� ���������� �������  cmdDataTransfer
uint8_t            FragCnt;      //����������� ����������
uint8_t            FragIdx;      //����� ���������
MProtType_t        ProtType;     //��� ���������, ����������� �� ������ �������� cmdDataTransfer
uint8_t            DataCnt;
uint16_t           IdxRAS;       //���������� ����� ���������� � �������
uint16_t           IdxMAS;       //������ � �������
uint16_t           CntRAS;       //����������� ���������� ����������
uint32_t           VarAddr;      //���������� ����� ���������� � ������ (��� ������ �������)
uint8_t            VarCnt;       //����������� ���������� (���� ������ 1, �� ��� ������ �� VarCnt ���������)
_VarType_t         VarType;      //��� ����������
uint8_t            VarWRE;       //���������� �� ������
uint8_t            VarSize;
uint8_t            data[256];

ArchSign_t	   ArchSign;	   //������� � ������� ������ ������
uint16_t 	   ArchDay; 	   //����, ������� � 1900 ����
uint32_t  	   ArchHalfSec;    //���������� ���������� � ������ �����
uint32_t 	   ArchRecCount;   //������������� ���������� �������� �������
uint16_t	   ArchCellNumber; //����� ������
uint16_t	   ArchCellCnt;    //���������� ������������� �����
uint16_t 	   ArchRecSize;    //������ �������� ������ � ��
uint16_t 	   ArchPeriod;	   //������ �������������	
//uint32_t         ArchiveRecNo;
//uint16_t         ArchiveRecCnt;
uint8_t            DataLen;
uint16_t           chs;
TbusErr_t          error;
}MTree_t;


typedef struct
{
uint16_t        crc16;
uint8_t         inc;
uint8_t         *pReceiveCHS;
uint8_t         *pReceive;
MParcerState_t  State;
}FlowAnaliser_t;


typedef struct
{
MTree_t           Tree;  
uint16_t          FlowAnaliserTimeOut;//�����, �� ������� ������������ ���������� ������, ���� ������� �� �������� �� ����
uint16_t          FlowAnaliserTimeOutCounter;
uint16_t          RxFirst;
uint16_t          RxLast;  
uint8_t           BadVarIdx;
uint8_t           Problem;
uint8_t           Succes;
uint8_t           TxBuff[256];
uint8_t           RxBuff[512];
uint8_t           AddedDevices[MaxDevices];
uint8_t           DeviceEnable;//���������� o������� � ������� �����������
MTBusDescriptor_t LastTBusReq;
MTBusConfirm_t    TBusConfirm;
ArchConfirm_t     ArchConfirm;

void (*ArchConfirmCallBack)(ArchConfirm_t *ArchConfirm);//��������� �� ������������, ������� ���������� ����� ��������� ��������� ������
NotificationResp_t (*NoticationCallBack)(NotificationReq_t *Notification);
void (*ReqConf)(MTBusConfirm_t *TBusConf);
FlowAnaliser_t    FlowAnaliser;
volatile          uint16_t TimeOut_;
}MTBusModule_t;


extern MRAS_t  MRAS[];


extern void MTBusInit(MTBusModule_t *MTBusModule);//�������������
extern void MTBusDataReceiver(MTBusModule_t *MTBusModule,uint8_t byte);//���������� �� ������ �����
extern void MTBusFlowAnaliser(MTBusModule_t *MTBusModule,uint8_t byte);
extern void MTBusRun(MTBusModule_t *MTBusModule);
extern void MTBusReq(MTBusModule_t *MTBusModule,MTBusDescriptor_t *Descriptor);//������������� �������
extern void MTBusTimeOutCounter(MTBusModule_t *MTBusModule);
//---------------------------------------------------------------------------
#endif
