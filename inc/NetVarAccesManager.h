//Created by Ihor Bilorus at 28/02/2012
//Last modified by Ihor Bilorus at 02/03/2012 added GetOnlineDevCnt, GetMaxDevAddr,
//GetDevName, GetProgVer, GetVarAllCntOfDevice, GetHostPort, GetMaxDevAddr

#ifndef _NetVarAccesManager_h_
#define _NetVarAccesManager_h_

#include <stdint.h>
#include "MTBus.h"
#include "MTBusServer.h"

extern void SwitchAcces(uint8_t DevAddr,uint16_t IdxNetVar,uint8_t HostInterface);//Выбирает переменную как текущую
extern _VarType_t GetVarType(void);                //Возвращает тип выбранной переменной
extern uint8_t GetAccessVarType(void);             // тип доступа к переменной (0-R, 1-RW)
extern uint8_t GetDevAddr(uint8_t idx);            //адрес прибора по индексу
extern uint8_t *  GetDataPointer(void);            //Возвращает указатель на данные выбранной переменной
extern uint8_t *  GetDataPtr(uint8_t DevAddr,uint8_t vIdx);//Возвращает указатель на данные переменной vIdx прибора DevAddr
extern uint8_t *  GetTextInfo(void);               //Возвращает текстовое описание выбранной переменной
extern uint8_t    GetVarCnt(void);                 //Возращает количество элементов выбранной переменной (для массива больше 1) 
extern uint8_t    GetOnlineDevCnt(void);           //Возвращает количество онлайн устройств в сети
extern uint8_t    GetMaxDevAddr(HostPort_t Port);  //Возвращает наибольший адрес устройства, которое может быть в сети
extern uint8_t    *GetDevName(uint8_t DevAddr);    //Возвращает имя прибора с адресом DevAddr
extern uint8_t    *GetProgVer(uint8_t DevAddr);    //Возвращает версию программы прибора с адресом DevAddr
extern uint16_t   GetVarAllCntOfDevice(uint8_t DevAddr); //Возвращает количество всех переменных на борту прибора с адресом DevAddr
extern HostPort_t GetHostPort(uint8_t DevAddr);    //Возвращает индекс порта, к которому подключен прибор с адресом DevAddr 
extern uint8_t    GetMaxDevAddr(HostPort_t Port);  //Возвращает наибольший адрес прибора на выбранном порте
extern uint8_t    GetCurrSearchAddr(HostPort_t Port);//Возвращает текущий адрес прибора при поиске приборов, если 0 - то система сканирования остановлена
extern uint8_t    GetCurrReqAddr(HostPort_t Port);  //Возвращает текущий адрес опрашиваемого прибора, если 0 - то система опроса остановлена
extern void AddNetDevice(HostPort_t Port,uint8_t DevAddr);//Принудительное добавление устройства в сеть порта Port
extern void WriteValueToVariable(uint8_t DevAddr,uint16_t vIdx,uint8_t *pData);//Подпрограмма записи в переменную vIdx прибора DevAddr

extern void StartNetSearch(HostPort_t Port);//Принудительное сканирование приборов на порте Port
extern void SetNetSearchAddr(HostPort_t Port, uint8_t DevAddr); // установка адреса для сканирования        
extern uint16_t GetWrQueueVarCnt( uint8_t DevAddr ); // количество записей в очереди на отсылку по устройству
extern void WriteValueToMasVariable(uint8_t DevAddr,uint16_t vIdx,uint8_t IdxMas,uint8_t vCnt,uint8_t *pData);
extern uint8_t GetDevOnlineStatus(uint8_t DevAddr);
extern void DelNetDevice (HostPort_t Port,uint8_t DevAddr);
extern void RequestArchRecords(uint8_t DevAddr,uint16_t Day,uint32_t HalfSec,uint16_t RecCount);
extern void RequestArchCellFormat(uint8_t DevAddr,uint16_t Day,uint32_t HalfSec,uint16_t CellNumber,uint16_t CellCnt);
extern VariableStatus_t GetVariableStatus(uint8_t DevAddr,uint16_t vIdx);
extern _VarType_t GetVariableType(uint8_t DevAddr,uint16_t vIdx);//Получение типа переменной с указанием адреса устройства
extern uint8_t GetDevFirstEntryInNetwork(uint8_t DevAddr); // 0 - устройство еще не добавлено в MRAS
                                                           // 1 - устройство уже добавлено в MRAS
extern uint8_t GetRoutingMode(uint8_t DevAddr);            // текущий режим работы с таблицей маршрутизации
                                                           // 0 - работа с таблицей запрещена
                                                           // 1 - работа с таблицей разрешена 
extern void SetRoutingMode(uint8_t DevAddr, uint8_t Mode); // установка режима работы с таблицей маршрутизации
#endif

