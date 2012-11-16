//Created by Ihor Bilorus
//---------------------------------------------------------------------------
//****************Модуль протокола TeraBus(Master) ver 4.0 beta***************// 

#ifndef _MTBus_h
#define _MTBus_h

#include "MTBusConfiguration.h"

#include <stdint.h>

//Last modified 20.12.11 by Ihor Bilorus (added function _cmdDataTransfer)
//Last modified 12.01.12 by Ihor Bilorus (modified function _cmdDataTransfer)
//Last modified 16.01.12 by Ihor Bilorus (optimization algoritm by memory)
//Last modified 02.03.12 by Ihor Bilorus (changed "новая переменная" to "New Variable")
//Last modified by Ihor Bilorus at 29.03.12 - added Notification
//Last modified by Ihor Bilorus at 05.04.12 - added _cmdGetArch
//Last modified by Ihor Bilorus at 18.04.12 - added Variable Status


//***********************Комманды поддерживаемые протоколом*******************//
typedef enum
{
_cmdReadRAM       = 0x02, //Чтение ОЗУ
_cmdWriteRAM      = 0x03, //Запись в ОЗУ
_cmdReadEEPROM    = 0x12, //Чтение EEPROM
_cmdWriteEEPROM   = 0x13, //Запись EEPROM
_cmdReadFlash     = 0x42, //Чтение Flash
_cmdWriteFlash    = 0x43, //Запись во Flash
_cmdId            = 0x20, //Идентификация прибора
_cmdSetAddr       = 0x21, //Установка адреса прибора
//_cmdArchParams    = 0x22, //Чтение конфигурации архива
//_cmdArchData      = 0x23, //Чтение архивных записей
_cmdGetArch	  = 0x24, //Вычитывание архива
_cmdGetVarList    = 0x25, //Вычитывание списка переменных прибора
_cmdReadRAM_EXT   = 0x26, //Расширенное чтение RAM
_cmdWriteRAM_EXT  = 0x27, //Расширенная запись в ОЗУ
_cmdDataTransfer  = 0x35  //Комманда транспортировки произвольных данных
}TbusCmd_t;
//****************************************************************************//

//************************Типы данных согласно протоколу**********************//
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

//**********************Перечень ошибок согласно протоколу********************//
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


//*******************Статус выполненной операции******************************//
typedef enum
{
MTB_FREE_RUN_STATUS,      //Простой
MTB_SUCCESS_STATUS,       //Операция выполненна успешно
MTB_BAD_STATUS,           //Операция выполнена неуспешно (из-за потреи данных и т.д.)
MTB_PART_SUCCESS_STATUS,  //Операция частично выполнена успешно (некоторые переменные вычитаны с ошибками)
MTB_CHS_ERROR_STATUS,     //Операция выполнена с ошибками контрольной суммы
MTB_NO_RESPONSE_STATUS,   //Операция выполнена, но опрашиваемое устройство не ответило
}MTBusStatus_t;     
//****************************************************************************//



typedef enum
{
VARIABLE_NOT_INIT_STATUS   = 0x00, //Переменная не инициализирована
VARIABLE_PART_INIT_STATUS  = 0x01, //Переменная частично инициализирована
VARIABLE_IS_INIT_STATUS    = 0x02, //Переменная инициализирована
}VariableStatus_t;


typedef enum
{
_SIGNATURE_ANALISE_STATE,      //Состояние анализа синхронизирующего байта
_DEV_ADDR_RECEIVE_STATE,       //Состояние анализа сетевого адреса устройства
_OLD_DEV_ADDR_RECEIVE_STATE,   //Состояние приема старого сетевого адреса устройства (для комманды смены адреса)
_COMMAND_ANALISE_STATE,        //Состояние анализа и приема комманды протокола
_ID_RECEIVE_STATE,             //Состояние приема ID устройства
_IDX_RAS_RECEIVE_STATE,        //Состояние приема порядкового номера переменной
_CNT_RAS_RECEIVE_STATE,        //Состояние приема колличества запрашиваемых переменных
_IDX_MAS_RECEIVE_STATE,        //Состояние приема индекса в масиве
_VAR_ADDR_RECEIVE_STATE,       //Состояние приема физического адреса переменной в памяти (для старых комманд)
_VAR_CNT_RECEIVE_STATE,        //Состояние приема колличества переменных в принимаемом массиве
_VAR_TYPE_ANALISE_STATE,       //Состояние приема типа переменной
_VAR_WRITE_EN_RECEIVE_STATE,   //Состояние приема разрешения на запись
//ARCH_REC_NO_RECEIVE_STATE,
//ARCH_REC_CNT_RECEIVE_STATE,

//*************Архив****************
_ARCH_SIGN_RECEIVE_STATE,
_ARCH_DAY_RECEIVE_STATE,
_ARCH_HALF_SEC_RECEIVE_STATE,
_ARCH_PERIOD_RECEIVE_STATE,
_ARCH_REC_SIZE_RECEIVE_STATE,
_ARCH_REC_COUNT_RECEIVE_STATE,
_ARCH_CELL_NUMBER_RECEIVE_STATE,
_ARCH_CELL_CNT_RECEIVE_STATE,
_ARCH_CELL_FORMAT_RECEIVE_STATE,
_ARCH_REC_RECEIVE_STATE,       //Состояние приема архивных записей
//*********************************

_CHS_ANALISE_STATE,            //Состояние приема и сравнения контрольной суммы
_RESERVED_RECEIVE_STATE,       //Состояние приема резервного байта для комманды cmdDataTransfer
_STATE_RECEIVE_STATE,          //Состояние приема состояния выполенения команды cmdDataTransfer
_FRAG_CNT_IDX_RECEIVE_STATE,   //Состояние приема колличества фрагментов и номера текущего фрагмента
_PROT_TYPE_RECEIVE_STATE,      //Состояние приема типа протокола, наложенного на данные комманды cmdDataTransfer
_DATA_CNT_RECEIVE_STATE,       //Состояние приема колличества байт комманды cmdDataTransfer
_DATA_RECEIVE_STATE            //Состояние приема фрагмента данных
}MParcerState_t;//Состояния автомата анализатора входного потока данных



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
ARCH_SUCCES_STATUS	            = 0x00,  //Архивная запись получена успешно
ARCH_CHS_ERROR_STATUS		    = 0x01,  //Архивная запись получена с ошиками CRC
ARCH_NOT_RESP_STATUS  		    = 0x02,  //Архивная запись не получена после запроса

ARCH_TERMINATOR_SUCCES_STATUS	    = 0x03,  //Успешно получен признак последнего пакета большой посылки
ARCH_TERMINATOR_CHS_ERROR_STATUS    = 0x04   //Получен признак последнего пакета большой посылки с ошибками CRC
}ArchOperationStatus_t;

typedef enum
{
ARCH_REC_SIGN 	 	  = 0x01, //Признак чтения архивных записей
ARCH_CELL_FORMAT_SIGN     = 0x02, //Признак чтения формата архивных ячеек
ARCH_BORDER_SIGN	  = 0x04  //Признак гранци перехода между различными конфигурациями архивов
}ArchSign_t; //Признак в команде чтения архива


typedef struct
{
uint8_t           ArchDevAddr;
ArchReqType_t     ArchReqType;
uint16_t  	      ArchDay;
uint32_t  	      ArchHalfSec;
uint16_t 	      ArchPeriod;	   //Период архивирования
uint16_t 	      ArchRecSize;	   //Размер архивной записи
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


//**************Таблица для хранения информации о переменных в сети***********//
typedef struct
{
uint8_t           DevAddr;   //Адрес прибора
uint16_t          vIdx;      //Порядковый номер переменной в приборе
uint8_t           vType;     //Тип переменной
uint8_t           vCnt;      //Колличество переменных (1 - если одна, >1 для массива)
uint8_t           vWRE;      //Разрешение записи
uint8_t           vInfo[18]; //Описание переменной 
uint8_t           *pData;     //Указатель на данные для данной переменной 
VariableStatus_t  Status;
}MRAS_t;
//****************************************************************************//

typedef struct
{
uint8_t           *InitiatorAddr;//Адрес инициатора, вызвавшего обмен (адрес обьекта верхнего уровня)   
uint8_t           DevAddr;
uint8_t           OldDevAddr;
TbusCmd_t         Command;
uint8_t           Reserved;    //Зарезервированный байт для комманды cmdDataTransfer
MDTState_t        DTState;     //Состояние выполнения команды  cmdDataTransfer
uint8_t           FragCnt;     //Колличество фрагментов
uint8_t           FragIdx;     //Номер фрагмента
MProtType_t       ProtType;    //Тип протокола, наложенного на данные комманды cmdDataTransfer
uint8_t           DataCnt;
uint32_t          VarAddr;
_VarType_t        VarType;
uint8_t           VarCnt;
uint8_t           VarWRE;
uint8_t           *data;

uint16_t          ArchDay; 	   //День, начиная с 1900 года
uint32_t  	  ArchHalfSec;    //Количество полусекунд с начала суток

uint32_t 	  ArchRecCount;   //Запрашиваемое количество архивных записей
uint16_t	  ArchCellNumber; //Номер ячейки
uint16_t	  ArchCellCnt;    //Количество запрашиваемых ячеек
uint16_t 	  ArchRecSize;    //Размер архивной записи в Бт

ArchReqType_t     ArchReqType;
uint16_t	  ArchPeriod;

struct
  {
  uint32_t        BadVar;//Адреса переменных, при вычитывании которых были проблемы
  MTBusStatus_t   VarErr;
  }Problem[10]; //Не изменять этот индекс

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
uint8_t          *InitiatorAddr;//Адрес инициатора, вызвавшего обмен (адрес обьекта верхнего уровня)  
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

//******Параметры для чтения архива******
ArchSign_t	 ArchSign;
uint16_t 	 ArchDay; 	  //День, начиная с 1900 года
uint32_t 	 ArchHalfSec;     //Количество полусекунд с начала суток
uint32_t	 ArchRecCount;    //Запрашиваемое количество архивных записей
uint16_t         ArchCellNumber;  //Номер ячейки
uint16_t	 ArchCellCnt;     //Количество запрашиваемых ячеек
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
uint8_t            Signature;    //Байт синхронизации 0х50 (так же используется как признак завершенности разбора)
uint8_t            DevAddr;      //Адрес устройства
uint8_t            OldDevAddr;   //Старый адрес устройства
TbusCmd_t          Command;      //Комманда
uint8_t            Reserved;     //Зарезервированный байт для комманды cmdDataTransfer
MDTState_t         DTState;      //Состояние выполнения команды  cmdDataTransfer
uint8_t            FragCnt;      //Колличество фрагментов
uint8_t            FragIdx;      //Номер фрагмента
MProtType_t        ProtType;     //Тип протокола, наложенного на данные комманды cmdDataTransfer
uint8_t            DataCnt;
uint16_t           IdxRAS;       //Порядковый номер переменной в приборе
uint16_t           IdxMAS;       //Индекс в массиве
uint16_t           CntRAS;       //Колличество полученных переменных
uint32_t           VarAddr;      //Физический адрес переменной в памяти (для старых комманд)
uint8_t            VarCnt;       //Колличество переменных (если больше 1, то это массив из VarCnt элементов)
_VarType_t         VarType;      //Тип переменной
uint8_t            VarWRE;       //Разрешение на запись
uint8_t            VarSize;
uint8_t            data[256];

ArchSign_t	   ArchSign;	   //Признак в команде чтения архива
uint16_t 	   ArchDay; 	   //День, начиная с 1900 года
uint32_t  	   ArchHalfSec;    //Количество полусекунд с начала суток
uint32_t 	   ArchRecCount;   //Запрашиваемое количество архивных записей
uint16_t	   ArchCellNumber; //Номер ячейки
uint16_t	   ArchCellCnt;    //Количество запрашиваемых ячеек
uint16_t 	   ArchRecSize;    //Размер архивной записи в Бт
uint16_t 	   ArchPeriod;	   //Период архивирования	
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
uint16_t          FlowAnaliserTimeOut;//Время, за которое сбрасывается анализатор потока, если ниччего не приходит на вход
uint16_t          FlowAnaliserTimeOutCounter;
uint16_t          RxFirst;
uint16_t          RxLast;  
uint8_t           BadVarIdx;
uint8_t           Problem;
uint8_t           Succes;
uint8_t           TxBuff[256];
uint8_t           RxBuff[512];
uint8_t           AddedDevices[MaxDevices];
uint8_t           DeviceEnable;//Разрешение oпераций с текущим устройством
MTBusDescriptor_t LastTBusReq;
MTBusConfirm_t    TBusConfirm;
ArchConfirm_t     ArchConfirm;

void (*ArchConfirmCallBack)(ArchConfirm_t *ArchConfirm);//Указатель на подпрограмму, которая вызывается после получения архивного пакета
NotificationResp_t (*NoticationCallBack)(NotificationReq_t *Notification);
void (*ReqConf)(MTBusConfirm_t *TBusConf);
FlowAnaliser_t    FlowAnaliser;
volatile          uint16_t TimeOut_;
}MTBusModule_t;


extern MRAS_t  MRAS[];


extern void MTBusInit(MTBusModule_t *MTBusModule);//Инициализация
extern void MTBusDataReceiver(MTBusModule_t *MTBusModule,uint8_t byte);//Передавать по одному байту
extern void MTBusFlowAnaliser(MTBusModule_t *MTBusModule,uint8_t byte);
extern void MTBusRun(MTBusModule_t *MTBusModule);
extern void MTBusReq(MTBusModule_t *MTBusModule,MTBusDescriptor_t *Descriptor);//Формирователь запроса
extern void MTBusTimeOutCounter(MTBusModule_t *MTBusModule);
//---------------------------------------------------------------------------
#endif
