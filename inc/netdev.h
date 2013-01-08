#ifndef _netdev_h
#define _netdev_h

#define ZB_DEVICE_CNT 32
#define DV_DEVICE_CNT 32
#define ND_DEVICE_CNT 32
/////////////////////////////////////////////////////////
// for wileless devices
// v01, v02
typedef struct sZBRAS
{
 uint8_t  DevAddr;
 float    Temperature;
 uint16_t TTL;   // Time to live
 uint8_t  T_Status;
 uint8_t  ParDevAddr;
 int8_t   rssi;
 uint16_t SendInterval;
 uint8_t  Humibility;
 uint8_t  OnLine;
 uint8_t  lqi;
 float    Offset;
 float    Inclin;
 uint8_t  Service;
} tZBRAS[ZB_DEVICE_CNT];
//////////////////////////////////////////////////
// for DV05 - (t,rh)
///* 0*/  {&aVl,                 6,    2, 0                          , "T/RH"},
///* 1*/  {&Vl_Status,           0,    2, 0                          , "Status"},
///* 2*/  {&Loi,                 6,    2,(func_t)rSetLoi             , "Loi"},
///* 3*/  {&Hig,                 0,    2,(func_t)rSetHig             , "Hig"},
///* 4*/  {&kV_RH_0,             6,    1,(func_t)rSetkV_RH_0         , "kV_RH_0"},
///* 5*/  {&kV_RH_100,           6,    1,(func_t)rSetkV_RH_100       , "kV_RH_100"},
///* 6*/  {&R7,                  6,    1,(func_t)rSetR7              , "R7"},
///* 7*/  {&R8,                  6,    1,(func_t)rSetR8              , "R8"},
///* 8*/  {&R9,                  6,    1,(func_t)rSetR9              , "R9"},
///* 9*/  {&R10,                 6,    1,(func_t)rSetR10             , "R10"},
///*10*/  {&R11,                 6,    1,(func_t)rSetR11             , "R11"},
///*11*/  {&RhID,                2,    1,(func_t)rSetRhID            , "RhID"},
///*12*/  {&dV,                  6,    2,(func_t)rSetdV              , "dV"},
///*13*/  {&kV,                  6,    2,(func_t)rSetkV              , "kV"},
///*14*/  {&vRef,                6,    1, (func_t)rSetvRef           , "vRef"},
///*15*/  {&TypeSens,            0,    2, (func_t)rSetTypeSens       , "TypeSens"},
///*16*/  {&aAx,                 5,    4, 0                          , "aAx"},
typedef struct sDVRAS
{
 float    aVl[2];
 uint8_t  Vl_Status[2];
 float    Loi[2];   // Time to live
 float    Hig[2];
 float    kV_RH_0;
 float    kV_RH_100;
 float    R7;
 float    R8;
 float    R9;
 float    R10;
 float    R11;
 uint16_t RhID;
 float    dV[2];
 float    kV[2];
 float    vRef;
 uint8_t  TypeSens[2];
 int32_t  aAx[4];
}tDVRAS[DV_DEVICE_CNT];

//////////////////////////////////////////////////
//////////////////////////////////////////////////
typedef struct sNDSens
{
  uint8_t Addr;        //
  uint8_t *pName;      //
  uint8_t *pVersion;   //
  uint8_t Online;      //
  uint8_t Bind;        //
  uint8_t * pRAS;      //
} tNDSens;

typedef struct sND
{
  uint8_t  Cnt;
  uint8_t  Idx;
  tNDSens  Device;
}tND;

extern tND ND[];
extern uint32_t NDTime_Pr;

extern void ND_Bind( void );
extern void ND_GetValues( void );

#endif // _netdev_h
