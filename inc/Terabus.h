#ifndef _terabus_h
#define _terabus_h

#include <stdint.h>
//Last modified 16.05.11 by Ihor Bilorus
//Last modified 18.05.11 by Ihor Bilorus (added functions in TRAS)
//Last modified 23.05.11 by Ihor Bilorus (added commands cmdSetAddr, cmdGetVarList)
//Last modified 23.06.11 by Ihor Bilorus (revised command cmdWriteRAM, change TimeOutCounter)
//Last modified 01.08.11 by Ihor Bilorus (added command cmdReadRAM_EXT, added TBusDataReceiver,
//added the possibility of indirect addressing in command of cmdWriteRAM/cmdReadRAM)
//Last modified 20.12.11 by Ihor Bilorus (addef command cmdDataTransfer)
//Last modified 12.01.12 by Ihor Bilorus (modified command cmdDataTransfer)
//Last modified 12.01.12 by Ihor Bilorus (modified command cmdDataTransfer)
//Last modified 18.04.12 by Ihor Bilorus (addef command cmdGetArch)


//Модуль протокола TeraBus ver 4.0 (slave для master)beta
//****Краткое описание работы***
//Модуль состоит из следующих подпрограмм:
//1) TBusDataReceiver - приемник потока данных. Поток нужно передавать по одному байту.

//2) TBusFlowAnaliser - анализатор входного потока байт. Результатом работы есть дерево
//разбора входной последовательности Tree. Если байт сигнатуры (Tree.Signature)
//этого дерева не равен 0, то значит, что TBusFlowAnaliser разобрал фходной запрос и сформировал дерево

//3) TBusRun - анализирует дерево разбора и в зависимости от занесенной в него коммандыи формирует ответ,
//который потом передается по UART. Для комманды чтения архива ответ формируется по частям.
//Подпрограмму TBusRun необходимо поместить в цикл функции main();


//4) TBusTimeOutCounter - подпрограмма подсчета таймаута. Вызивать нужно с периодом 1 мс.


//Рекомендовано использовать этот модуль следующим образом:
//   ____________________
//  |  TBusManager.c (h) |
//  |____________________| - Настройки модуля TeraBus и управления им
//  |    TeraBus.c (h)   |
//  |____________________| - Модуль протокола TeraBus кроссплатформенный.
//  | UsartManager.c (h) |
//  |____________________| - Менеджер раборы с последовательным портом.
//  |  UsartDriver.c (h) |
//  |  (нижний уровень)  | - Драйвер последовательного порта. Для каждой платформы разный
//  |____________________|

//Для правильной работы модуля нужно:
//1)Поместить TBusRun() в цикл функции main()
//2)Поместить TBusTimeOutCounter() в обаботчик таймера 1 мс.
//3)Передавать подпрограмме TBusDataReceiver принятые данные по одному байту
//4)Настроить параметр CPU_BUS_LEN в зависимости от разрядности МК

//*********************************Примечание!!!********************************
//Модуль Terabus сохраняет свою работоспособность даже если не выполнять пункт 2



typedef enum
{
cmdReadRAM          = 0x02, //Чтение ОЗУ
cmdWriteRAM         = 0x03, //Запись в ОЗУ
cmdReadEEPROM       = 0x12, //Чтение EEPROM
cmdWriteEEPROM      = 0x13, //Запись EEPROM
cmdReadFlash        = 0x42, //Чтение Flash
cmdWriteFlash       = 0x43, //Запись во Flash
cmdId               = 0x20, //Идентификация прибора
cmdSetAddr          = 0x21, //Установка адреса прибора
//cmdArchParams       = 0x22, //Чтение конфигурации архива
//cmdArchData         = 0x23, //Чтение архивных записей
cmdGetArch          = 0x24,
cmdGetVarList       = 0x25, //Вычитывание списка переменных прибора
cmdReadRAM_EXT      = 0x26, //Расширенное чтение RAM
cmdWriteRAM_EXT     = 0x27, //Расширенная запись в ОЗУ
cmdDataTransfer     = 0x35  //Комманда транспортировки произвольных данных
}TeraBusCmd_t;

typedef enum
{
errNone             = 0x00,
errCHS              = 0xF0,
errUnknownCmd       = 0xF1,
//errInvalidCount     = 0xF2,
errInvalidType      = 0xF3,
//errInvalidDataLen   = 0xF4,
//errInvalidData      = 0xF5,
//errExecution        = 0xF6,
//errFrame            = 0xF7,
//errPending          = 0xF8
}TeraBusErr_t;

typedef enum
{
uint8_t_            = 0x00, //Беззнаковый байт
int8_t_             = 0x01, //Знаковый байт
uint16_t_           = 0x02,
int16_t_            = 0x03,
uint32_t_           = 0x04,
int32_t_            = 0x05,
float_              = 0x06,
DateTime_t_         = 0x08  //Дата и время
}VarType_t;

typedef enum 
{
NO_PROTOCOL         = 0x00, //Пртокол не наложен (просто данные)
BOOT_PROTOCOL       = 0x01  //Протококол обновления ПО
}ProtType_t;        //Тип протокола, наложенного на данные комманды cmdDataTransfer


typedef enum
{
Request = 0x01,
Answer  = 0x02,
Next  	= 0x03,
Retry   = 0x04
}DTState_t;   



typedef enum
{
SIGNATURE_ANALISE_STATE,        //Состояние анализа синхронизирующего байта
DEV_ADDR_ANALISE_STATE,         //Состояние анализа сетевого адреса устройства
NEW_DEV_ADDR_RECEIVE_STATE,     //Состояние приема нового сетевого адреса устройства (для комманды смены адреса)
COMMAND_ANALISE_STATE,          //Состояние анализа и приема комманды протокола
VAR_ADDR_RECEIVE_STATE,         //Состояние приема адреса переменной ОЗУ, FLASH или EEPROM
VAR_CNT_RECEIVE_STATE,          //Состояние приема колличества переменных которые нужно читать/записывать
VAR_TYPE_ANALISE_STATE,         //Состояние анализа и приема типа переменных
//ARCH_REC_NO_RECEIVE_STATE,    //Состояние приема номера архивной записи записи
//ARCH_REC_CNT_RECEIVE_STATE,   //Состояние приема колличества архивных записей
CHS_ANALISE_STATE,              //Состояние приема и анализа контрольной суммы
DATA_RECEIVE_STATE,             //Состояние приема данный (для комманд записи)
IDX_RAS_RECEIVE_STATE,          //Состояние приема порядкового номера переменной 
CNT_RAS_RECEIVE_STATE,          //Состояние приема колличества запрашиваемых переменных
IDX_MAS_RECEIVE_STATE,          //Состояние приема индекса масива 
RESERVED_RECEIVE_STATE,         //Состояние приема резервного байта для комманды cmdDataTransfer  
STATE_RECEIVE_STATE,            //Состояние приема состояния выполенения команды cmdDataTransfer
FRAG_CNT_IDX_RECEIVE_STATE,     //Состояние приема колличества фрагментов и номера текущего фрагмента
PROT_TYPE_RECEIVE_STATE,        //Состояние приема типа протокола, наложенного на данные комманды cmdDataTransfer  
DATA_CNT_RECEIVE_STATE,         //Состояние приема колличества байт комманды cmdDataTransfer
ARCH_SIGN_RECEIVE_STATE,
ARCH_DAY_RECEIVE_STATE,
ARCH_HALF_SEC_RECEIVE_STATE,
ARCH_REC_COUNT_RECEIVE_STATE,
ARCH_CELL_NUMBER_RECEIVE_STATE,
ARCH_CELL_CNT_RECEIVE_STATE
}ParcerState_t;


typedef enum 
{
DT_FREE_RUN                 = 0x00,
DT_WAIT_SLAVE_RESPONSE      = 0x01,
DT_SLAVE_RESPONSE_COMPLETED = 0x02,
DT_SLAVE_NOT_RESPONSE       = 0x03
}DtTransferState_t;

typedef enum
{
ARCH_REC_SIGN_ 	 	  = 0x01, //Признак чтения архивных записей
ARCH_CELL_FORMAT_SIGN_    = 0x02, //Признак чтения формата архивных ячеек
ARCH_BORDER_SIGN_	  = 0x04  //Признак граници переходов между различными конфигурациями архивов
}ArchSignType_t; //Признак в пакете команде чтения архива

typedef enum
{
ARCH_GET_REC_REQUEST_TYPE         = 0x01, //Запрос архивных записей
ARCH_GET_CELL_FORMAT_REQUEST_TYPE = 0x02, //Запрос формата ячейки
ARCH_GET_ARCH_PARAMS_REQUEST_TYPE = 0x03  //Запрос параметров архива
}ArchRequestType_t; //Тип запроса от модуля протокола модулю верхнего уровня


typedef struct
{
DtTransferState_t   State;    //Признак того, что данные уже можно отправлять  
uint8_t             DevAddr;  //Сетевой адрес прибора
uint8_t             Reserved; //Зарезервированный байт для комманды cmdDataTransfer
DTState_t           DTState;  //Состояние выполнения 
uint8_t             FragCnt;  //Колличество фрагментов
uint8_t             FragIdx;  //Номер фрагмента
ProtType_t          ProtType; //Тип протокола, наложенного на данные комманды cmdDataTransfer
uint8_t             *pData;   //Указатель на данные
uint8_t             DataCnt;  //Длина принятых даных
}DTConf_t;


//Дерево синтаксического разбора входного потока
typedef struct
{
uint8_t         data[256];     //Принятые данные (для комманд записи в ОЗУ, EEPROM, FLASH, cmdDataTransfer)
uint8_t         Signature;     //Байт синхронизации 0х50 (так же используется как признак завершенности разбора)
uint8_t         DevAddr;       //Принятый адрес устройства
uint8_t         NewDevAddr;    //Новый адрес устройства (при комманде смены адреса)
TeraBusCmd_t    Command;       //Комманда протокола
uint8_t         Reserved;      //Зарезервированный байт для комманды cmdDataTransfer
DTState_t       State;         //Состояние выполнения команды  cmdDataTransfer
uint8_t         FragCnt;       //Колличество фрагментов
uint8_t         FragIdx;       //Номер фрагмента
ProtType_t      ProtType;      //Тип протокола, наложенного на данные комманды cmdDataTransfer
uint16_t        IdxRAS;        //Индекс переменной в структуре RAS
uint16_t        CntRAS;        //Колличество переменных начиная с IdxRAS
uint16_t        IdxMAS;        //Индекс внутри массива
uint32_t        VarAddr;       //Адрес переменной которую читают или в которую пишут
uint8_t         VarCnt;        //Колличество переменных которые нужно прочитать или записать
VarType_t       VarType;       //Тип переменных которые нужно прочитать или записать
uint8_t         VarSize;       //Размер переменной, которую нужно прочитать/записать VarSize = sizeof(VarType)

//********Параметры для вычитывания архива********
ArchSignType_t  ArchSign;      //Признак в пакете команде чтения архива
uint16_t        ArchDay;       //День, начиная с 1990 года
uint32_t        ArchHalfSec;   //Количество полусекунд с начала суток

uint32_t        ArchRecCount;  //Запрашиваемое количество архивных записей
uint16_t        ArchCellNumber;//Номер ячейки
uint16_t        ArchCellCnt;   //Количество запрашиваемых ячеек
//************************************************
uint8_t         DataLen;       //Длина данных которые нужно вычитать или записать dataLen=VarCnt*sizeof(VarType)
uint16_t        chs;           //Принятая контрольная сумма
TeraBusErr_t    error;         //Сообщение об ошибке
}Tree_t;


//********************************************************************************
//**************Настройка модуля для разных микроконтроллеров*********************
//********************************************************************************
//#define CPU_BUS_LEN uint16_t //Для МК с 16-битной адресацией в памяти программ *
#define CPU_BUS_LEN uint32_t //Для МК с 32-битной адресацией в памяти программ   *
//********************************************************************************

#define MASTER_OPERATION_ENABLE //Разрешить связку с ведущим


#define TBusTimeOut 1000 //x10ms ~200ms //Таймаут
#define DTransferTimeOut 1000

typedef struct 
{
uint8_t           DevAddr;         //Адрес прибора
uint16_t          ArchDay;         //Дни, начиная с 1900 года
uint32_t          ArchHalfSec;     //Полусекунды с начала суток
ArchRequestType_t ArchReqType;     //Признак в пакете команде чтения архива
uint16_t          ArchCellNumber;  //Номер ячейки в архивной записи
uint32_t          ArchRecCnt;      //Колличество архивных записейв
uint8_t           *pData;          //Указатель
}ArchReq_t;

typedef struct
{
uint8_t  DevAddr;         //Адрес прибора
uint16_t ArchDay;         //Дни, начиная с 1900 года
uint32_t ArchHalfSec;     //Полусекунды с начала суток
uint16_t ArchCellCnt;     //Колличество архивных ячеек в архивной записи
uint16_t ArchRecSize;     //Размер архивной записи
uint16_t ArchPeriod;      //Период архивирования
uint8_t  ArchItem;        //Свойства ячейки
uint8_t  ArchvType;       //Тип архивируемой переменной
uint8_t  ArchChannel;     //Каналы
uint16_t ArchvIdx;        //Номер переменной в приборе
uint8_t  ArchIdxArray;    //Индекс массива 
uint8_t  ArchPointCast;   //Целые и знаки после запятой
}ArchConf_t;  
             

typedef struct 
{
//*******************Настраиваемые параметры***********************//  
uint8_t         NetworkDeviceAddr;//Сетевой адрес прибора
const uint8_t   *DevName;         //Имя прибора
const uint8_t   *ProgrammVersion; //Версия программы прибора
uint8_t         *TxBuff;          //Указатель на передающий буфер
uint16_t        TxBuffSize;       //Размер передающего буфера
uint8_t         *RxBuff;          //Указатель на приемный буфер
uint16_t        RxBuffSize;       //Размер приемного буфера
void            (*SenderFunct)(uint8_t *pData, uint16_t len);//Указатель на подпрограмму отправки len байт по интерфейсу
uint8_t         (*GetPortStatusFunct)(void);                 //Указатель на подпрограмму получения статуса отправщика (0 свободен, иначе занят)
void            (*SetPortStatusFunct)(uint8_t Status);       //Указатель на подпрограмму установки статуса отправщика (0 свободен, иначе занят)
void            (*DTransferRxCallBack)(DTConf_t *Conf);      //Указатель на подпрограмму, которая вызывается, когда пришли данные по команде cmdDataTransfer 
ArchConf_t      (*ArchCallBack)(ArchReq_t * ArchRequest);    //Указатель на подпрограмму, которая вызывается, когда запрашивают что-то, связанное с архивом

//*****************************************************************//

Tree_t Tree;//Дерево синтаксического разбора (содержит в себе результат синтаксического анализа)

volatile uint32_t Time;//Счетчик таймаута

uint16_t      RxFirst;
uint16_t      RxLast;

uint16_t      crc16;//Контрольная сумма (вычисляется)
uint8_t       *pReceive;   //Указатель общего назначения для приема
uint8_t       *pReceiveCHS;//Указатель для приема контрольной суммы
uint8_t       inc;  //инкремент общего назначения

ArchReq_t     ArchRequest;
ArchConf_t    ArchConfirm;

//#ifdef _archive_h
uint32_t      ArchRecCnt; //Колличество архивных записей, которые будут отправлены за 1 раз 
//#endif// _archive_h

uint16_t      MRasIdx;
}TBusSlave_t;


//extern void TBusFlowAnaliser(TBusSlave_t *TBusModule,uint8_t byte);//Прием байт (передавать по одному байту)

extern void TBusDataReceiver(TBusSlave_t *TBusModule,uint8_t byte);//Приемник байт входного потока данных

extern void TBusRun(TBusSlave_t *TBusModule);//Формирователь ответов по протоколу TeraBus

extern void TBusInit(TBusSlave_t *TBusModule);
//Вставить в цикл функции main
extern void TBusTimeOutCounter(TBusSlave_t *TBusModule);//Вызывать с периодом в 1 мс

extern void TbusTrDataReceive(DTConf_t *DTConf);

#endif
