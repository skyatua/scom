//Created by Ihor Bilorus at 28/02/2012
//Last modified by Ihor Bilorus at 02/03/2012 added GetOnlineDevCnt, GetMaxDevAddr,
//GetDevName, GetProgVer, GetVarAllCntOfDevice, GetHostPort, GetMaxDevAddr

#ifndef _NetVarAccesManager_h_
#define _NetVarAccesManager_h_

#include <stdint.h>
#include "MTBus.h"
#include "MTBusServer.h"

extern void SwitchAcces(uint8_t DevAddr,uint16_t IdxNetVar,uint8_t HostInterface);//�������� ���������� ��� �������
extern _VarType_t GetVarType(void);                //���������� ��� ��������� ����������
extern uint8_t GetAccessVarType(void);             // ��� ������� � ���������� (0-R, 1-RW)
extern uint8_t GetDevAddr(uint8_t idx);            //����� ������� �� �������
extern uint8_t *  GetDataPointer(void);            //���������� ��������� �� ������ ��������� ����������
extern uint8_t *  GetDataPtr(uint8_t DevAddr,uint8_t vIdx);//���������� ��������� �� ������ ���������� vIdx ������� DevAddr
extern uint8_t *  GetTextInfo(void);               //���������� ��������� �������� ��������� ����������
extern uint8_t    GetVarCnt(void);                 //��������� ���������� ��������� ��������� ���������� (��� ������� ������ 1) 
extern uint8_t    GetOnlineDevCnt(void);           //���������� ���������� ������ ��������� � ����
extern uint8_t    GetMaxDevAddr(HostPort_t Port);  //���������� ���������� ����� ����������, ������� ����� ���� � ����
extern uint8_t    *GetDevName(uint8_t DevAddr);    //���������� ��� ������� � ������� DevAddr
extern uint8_t    *GetProgVer(uint8_t DevAddr);    //���������� ������ ��������� ������� � ������� DevAddr
extern uint16_t   GetVarAllCntOfDevice(uint8_t DevAddr); //���������� ���������� ���� ���������� �� ����� ������� � ������� DevAddr
extern HostPort_t GetHostPort(uint8_t DevAddr);    //���������� ������ �����, � �������� ��������� ������ � ������� DevAddr 
extern uint8_t    GetMaxDevAddr(HostPort_t Port);  //���������� ���������� ����� ������� �� ��������� �����
extern uint8_t    GetCurrSearchAddr(HostPort_t Port);//���������� ������� ����� ������� ��� ������ ��������, ���� 0 - �� ������� ������������ �����������
extern uint8_t    GetCurrReqAddr(HostPort_t Port);  //���������� ������� ����� ������������� �������, ���� 0 - �� ������� ������ �����������
extern void AddNetDevice(HostPort_t Port,uint8_t DevAddr);//�������������� ���������� ���������� � ���� ����� Port
extern void WriteValueToVariable(uint8_t DevAddr,uint16_t vIdx,uint8_t *pData);//������������ ������ � ���������� vIdx ������� DevAddr

extern void StartNetSearch(HostPort_t Port);//�������������� ������������ �������� �� ����� Port
extern void SetNetSearchAddr(HostPort_t Port, uint8_t DevAddr); // ��������� ������ ��� ������������        
extern uint16_t GetWrQueueVarCnt( uint8_t DevAddr ); // ���������� ������� � ������� �� ������� �� ����������
extern void WriteValueToMasVariable(uint8_t DevAddr,uint16_t vIdx,uint8_t IdxMas,uint8_t vCnt,uint8_t *pData);
extern uint8_t GetDevOnlineStatus(uint8_t DevAddr);
extern void DelNetDevice (HostPort_t Port,uint8_t DevAddr);
extern void RequestArchRecords(uint8_t DevAddr,uint16_t Day,uint32_t HalfSec,uint16_t RecCount);
extern void RequestArchCellFormat(uint8_t DevAddr,uint16_t Day,uint32_t HalfSec,uint16_t CellNumber,uint16_t CellCnt);
extern VariableStatus_t GetVariableStatus(uint8_t DevAddr,uint16_t vIdx);
extern _VarType_t GetVariableType(uint8_t DevAddr,uint16_t vIdx);//��������� ���� ���������� � ��������� ������ ����������
extern uint8_t GetDevFirstEntryInNetwork(uint8_t DevAddr); // 0 - ���������� ��� �� ��������� � MRAS
                                                           // 1 - ���������� ��� ��������� � MRAS
extern uint8_t GetRoutingMode(uint8_t DevAddr);            // ������� ����� ������ � �������� �������������
                                                           // 0 - ������ � �������� ���������
                                                           // 1 - ������ � �������� ��������� 
extern void SetRoutingMode(uint8_t DevAddr, uint8_t Mode); // ��������� ������ ������ � �������� �������������
#endif

