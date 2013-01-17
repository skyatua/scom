//Created by Ihor Bilorus at 02/03/2012
#ifndef _MTBusConfiguration_h_
#define _MTBusConfiguration_h_



#define MaxHostPorts     2   //������������ ����������� ������, �� ������� �������� ����� ��������

#define MaxDevices       10  //������������ ����������� �������� �� ���� �����

#define MRAS_COUNT       500 //������������ ����������� ���������� �� ���� �����

#define BuffSizeToVar    64  //���������� ���� �� ���������� ������ ������, ��������� ��� ����� ����������

#define MaxVarQueue      10  //������������ ����������� ����������, ������� ����� ��������� � ������� �� ������
                             //(_cmdWriteRAM_EXT (27))

#define MaxTtansferQueue 4   //������������ ����������� ���������� ��������������� ������
                             //(_cmdDataTransfer (35))

#define RoutingDisable     //��������� �������������
//#define RoutingEnable        //��������� �������������

#endif
