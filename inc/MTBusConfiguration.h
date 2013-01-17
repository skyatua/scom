//Created by Ihor Bilorus at 02/03/2012
#ifndef _MTBusConfiguration_h_
#define _MTBusConfiguration_h_



#define MaxHostPorts     2   //ћаксимальное колличество портов, по которым возможен опрос приборов

#define MaxDevices       10  //ћаксимальное колличество приборов во всех сет€х

#define MRAS_COUNT       500 //ћаксимальное колличество переменных во всех сет€х

#define BuffSizeToVar    64  // оличество байт во внутреннем буфере данных, отводимое дл€ одной переменной

#define MaxVarQueue      10  //ћаксимальное колличество переменных, которые можно поставить в очередь на запись
                             //(_cmdWriteRAM_EXT (27))

#define MaxTtansferQueue 4   //ћаксимальное колличество отложенных транспортировок данных
                             //(_cmdDataTransfer (35))

#define RoutingDisable     //«апретить маршрутизацию
//#define RoutingEnable        //–азрешить маршрутизацию

#endif
