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
}HostPort_t; //Номера портов, по которым идет опрос (нумерация начинается с 1)


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
uint8_t * sDevAddr;//Сетевой адрес прибора-источника
uint16_t sVidx;   //Номер переменной на борту прибора-источника
uint8_t * dDevAddr;//Сетевой адрес прибора-получателя
uint16_t dVidx;   //Номер переменной на борту прибора-получателя
}RoutTable_t;


//*****Переманные, относящиеся к механизму отложенной транспортировки данных**//
typedef struct
{
uint8_t           DevAddr;  //Адрес прибора которому будут пересылаться данные командой _cmdDataTransfer
uint8_t           *pData;   //Указатель на данные, которые будут пересылаться
uint8_t           len;      //Длина даных в байтах
MProtType_t       ProtType;
uint8_t           Reserved;
MDTState_t        State;
uint8_t 	  FragCnt;
uint8_t           FragIdx;
}TtansferQueue_t;//Структура для хранения информации об отложенной команде _cmdDataTransfer

//************Переменные, относящиеся к механизму отложенной записи************//
typedef struct
{
uint16_t MRasIdx;
uint8_t  IdxMas;//Индекс элемента массива
uint8_t  vCnt;  //Колличество записываемых элементов начиная с IdxMas
uint8_t  *pData;
}WriteQueue_t; //Структура для хранения информации об отложенной команде записи





typedef struct
{
//Параметры инициализации
void (*SenderFunct)(uint8_t *pData, uint16_t len);//Указатель на подпрограмму отправки len байт по интерфейсу
uint8_t   MaxNetAddr;//Наибольший адрес прибора, который может быть в сети
uint16_t  MaxResponseTime;
uint8_t   RequestEnable;       //Разрешить или запретить опрос приборов (комманды записи и передачи остаются активными)
uint16_t  MaxReceiveInterval;        //Максимальный промежуток времени, за который система должна получить данные от прибора 
uint16_t  ReceiveIntervalMultFactor; //Число, обозначающее во сколько раз таймаут на снятие с прибора статуса Online будет превышать интервал приема
uint16_t  FlowAnaliserTimeOut; //Время, по истечению которого сбрасывается анализатор входного потока
uint16_t  NotResponceCnt;      //Количество запросов без ответа, поле которого устройство считается Offline  
uint8_t   ServiceKeyEnable;
uint8_t   WriteBuffStorageEnable;//Если 1, то после вызова процедуры записи переменной данные записываются и в буфер хранения данных, если 0, то только передаются, а в MRAS модификация происходит после вычитывания переменной 

void (*ArchConfirmCallBack)(ArchConfirm_t *ArchConfirm);//Указатель на подпрограмму, которая вызывается после получения архивного пакета
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
uint8_t             TRQueBuff[250*MaxTtansferQueue];//Буфер для хранения данных, передаваемых коммандой _cmdDataTransfer
TtansferQueue_t     TRQueue[MaxTtansferQueue];
uint8_t             TRQueBuffIdx;                   //Номер текущего элемента TRDataBuff
uint8_t             TRQueFirst;
uint8_t             TRQueLast;
//****************************************************************************//

MTBusDescriptor_t   MTBusDescriptor;
ArchReqDescriptor_t ArchReqDescriptor;
ArchStatus_t	    ArchStatus;

MTBusManagerState_t State;
uint8_t             DeferredDevAddr;//Адрес прибора, который стоит на очередь отложенного запроса

uint8_t             MTBusPostTaskEn;
uint8_t             NetDevCnt;
uint8_t             ScanDevIdx;
uint8_t             SearchDevAddr;
uint8_t             NetDevices[MaxDevices];//Устройство, которое было найдено при сканировании, или добавлено
        
uint16_t            msec;
uint16_t            RouteTableDataEn;      //! sky разрешение отправки данных таблици маршрутизации       

MTBusModule_t       MTBusModule;   //Модуль нижнего уровня протокола (файл MTerabus)
}MTBusServer_t;

typedef struct
{
HostPort_t    Port;      //Номер порта
MTBusServer_t *HostAddr; //Адрес сервера в памяти
}AssignHostPortAddr_t;//Структура сопоставления адреса программного сервера верхнего уровня с номером его порта
//1-вый порт присваевается тому серверу, который проинициализирован первым, 2-й тому, кто проинициализирован вторым и т.д.


typedef struct _DevId_
{
uint8_t       DevAddr;           //Адрес прибора
HostPort_t    Port;              //Номер порта, в сети которого находится прибор с адресом DevAddr
uint8_t       Online;            //1 - прибор в сети, 0 - не в сети (не отвечает)
uint8_t       NotRespCnt;        //Колличество запросов без ответа для устройства с адресом DevAddr
MTBusServer_t *HostAddr;         //Указатель на обьект, в подчинении которого находится прибор с адресом DevAddr
uint8_t       DevName[20];       //Имя прибора
uint8_t       ProgVer[10];       //Версия программы прибора
uint16_t      LastReceiveTime;   //Время от последнего полученного сообщения
uint16_t      OnlineTimeOut;     //Время, по истечении которого устройство станет не OnLine  
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

extern void MTBusArchRequest(MTBusServer_t *Server,ArchReqDescriptor_t *ArchReqDescriptor);//Запрос архивной комманды

extern void ChangeMRASValue(uint16_t MRASn,uint8_t *pData,uint8_t IdxMas,uint8_t vCnt);
extern void TransferData(TRData_t *TRData);

#endif
