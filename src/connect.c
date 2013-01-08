#include "../inc/includes.h"

/*
void rm_SetDateTime(TDateTime * DateTime,uint8_t idx);
void rm_SetSensTypeUN(uint8_t * data,uint8_t idx);
void rm_SetOffsetUN(float * data,uint8_t idx);
void rm_SetInclinUN(float * data,uint8_t idx);
void rm_SetInclinXC(float * data,uint8_t idx);
void rm_SetOffsetXC(float * data,uint8_t idx);
void rm_SetInL(float * data,uint8_t idx);
void rm_SetInH(float * data,uint8_t idx);
void rm_SetLoiUN(float * data,uint8_t idx);
void rm_SetHigUN(float * data,uint8_t idx);
void rm_SetUnitChannel(uint8_t * data,uint8_t idx);
void rm_Setdi_Polarity(uint16_t * data,uint8_t idx);
void rm_Setdo_Polarity(uint16_t * data,uint8_t idx);
void rm_SetArchPeriod(uint16_t * data,uint8_t idx);
void rm_SetSAPt(uint8_t * data,uint8_t idx);
void rm_SetSmoothUN(uint8_t * data,uint8_t idx);
void rm_SetUTCClock(int8_t * data,uint8_t idx);
void rm_SetWSClock(uint8_t * data,uint8_t idx);
void rm_SetScaleType(uint8_t * data,uint8_t idx);
void rm_SetScreenMode(uint8_t * data,uint8_t idx);
void rm_SetSpeedMove(uint8_t * data,uint8_t idx);
void rm_SetScGrMin(float * data,uint8_t idx);
void rm_SetScGrMax(float * data,uint8_t idx);
*/

float Te = 10.1f;

const tRAS_t RAS[] = 
{
 // var                       type  cnt        funct                   info (18)
///* 0*/  {&DateTime           , 8,   1, (func_t )rm_SetDateTime  ,"Current date/time "},
 //------------------------------------------------------------------------------------

///*19*/  {&ArchPeriod,          2,   1,(func_t)rm_SetArchPeriod  ,"Period of archive "},

///*20*/  {&UTCClock,            1,   1,(func_t)rm_SetUTCClock    ,"UTC offset time   "},
///*21*/  {&WSClock,             0,   1,(func_t)rm_SetWSClock     ,"Summer/winter time"},
/*21*/  {&Te,                     6,   1, 0                        ,"T"},
};

const uint16_t RAS_Count = sizeof(RAS)/sizeof(tRAS_t);
/*
void rm_SetDateTime(TDateTime * dt,uint8_t idx)
{
  SetRTCClock(dt, &dispClock);
  GetRTCClock(&DateTime, &dispClock);
  InitArchive(1);

  asm("nop");
}

void rm_SetSensTypeUN(uint8_t * data,uint8_t idx)
{
  uint8_t tmp = SensTypeUN[idx];
  
  // äëÿ êàíàëîâ 1-8 äîïóñòèìûå òèïû 0-8, 20-28
   if ((*data == 0) ||
         (*data <= 8) ||
          ((*data >= 20)&&(*data <= 28)))
     { tmp = *data; }

  if (SensTypeUN[idx] != tmp)
  {
   SensTypeUN[idx] = tmp;
   e_Write(&SensTypeUN[idx],1, SensTypeUN_Addr + idx);

   CreateInquiries();
  }

  asm("nop");
}

void rm_SetScGrMax(float * data,uint8_t idx)
{
  if ((*data >= -999)&&(*data <= 9999))
  {
   ScGrMax[idx] = *data;
   e_Write(&ScGrMax[idx],4,ScGrMax_Addr+(idx<<0x02));
  }

  asm("nop");
}

void rm_SetScGrMin(float * data,uint8_t idx)
{
  if ((*data >= -999)&&(*data <= 9999))
  {
   ScGrMin[idx] = *data;
   e_Write(&ScGrMin[idx],4,ScGrMin_Addr+(idx<<0x02));
  }

  asm("nop");
}

void rm_SetOffsetUN(float * data,uint8_t idx)
{
  if ((*data <= 9.9f)&&(*data >= -9.9))
  {
   OffsetUN[idx] = *data;
   e_Write(&OffsetUN[idx],4,OffsetUN_Addr+(idx<<0x02));
  }

  asm("nop");
}
void rm_SetInclinUN(float * data,uint8_t idx)
{
  if ((*data <= 1.200f)&&(*data >= 0.800))
  {
   InclinUN[idx] = *data;
   e_Write(&InclinUN[idx],4,InclinUN_Addr+(idx<<0x02));
  }

  asm("nop");
}

void rm_SetOffsetXC(float * data,uint8_t idx)
{
  if ((*data <= 9.9f)&&(*data >= -9.9))
  {
   OffsetXC[idx] = *data;
   e_Write(&OffsetXC[idx],4,OffsetXC_Addr+(idx<<0x02));
  }
  asm("nop");
}

void rm_SetInclinXC(float * data,uint8_t idx)
{
  if ((*data <= 1.200f)&&(*data >= 0.800))
  {
   InclinXC[idx] = *data;
   e_Write(&InclinXC[idx],4,InclinXC_Addr+(idx<<0x02));
  }

  asm("nop");
}

void rm_SetInL(float * data,uint8_t idx)
{
  if ((*data <= -999.9)&&(*data >= 9999.9))
  {
   InL[idx] = *data;
   e_Write(&InL[idx],4,InL_Addr+(idx<<0x02));
  }
  asm("nop");
}

void rm_SetInH(float * data,uint8_t idx)
{
  if ((*data <= -999.9)&&(*data >= 9999.9))
  {
   InH[idx] = *data;
   e_Write(&InH[idx],4,InH_Addr+(idx<<0x02));
  }
  asm("nop");
}

void rm_SetLoiUN(float * data,uint8_t idx)
{
  if ((*data <= -999.9)&&(*data >= 9999.9))
  {
   LoiUN[idx] = *data;
   e_Write(&LoiUN[idx],4,LoiUN_Addr+(idx<<0x02));
  }

  asm("nop");
}
void rm_SetHigUN(float * data,uint8_t idx)
{
  if ((*data <= -999.9)&&(*data >= 9999.9))
  {
   HigUN[idx] = *data;
   e_Write(&HigUN[idx],4,HigUN_Addr+(idx<<0x02));
  }

  asm("nop");
}

void rm_SetUnitChannel(uint8_t * data,uint8_t idx)
{
  if (*data <= 42)
  {
   UnitChannel[idx] = *data;
   e_Write(&UnitChannel[idx],1,UnitChannel_Addr+idx);
  }

  asm("nop");
}

void rm_Setdi_Polarity(uint16_t * data,uint8_t idx)
{
  if (*data <= 0x000f)
  {
   di_Polarity = *data;
   e_Write(&di_Polarity,2,di_Polarity_Addr);
  }

  asm("nop");
}

void rm_Setdo_Polarity(uint16_t * data,uint8_t idx)
{
  if (*data <= 0x000f)
  {
   do_Polarity = *data;
   e_Write(&do_Polarity,2,do_Polarity_Addr);
  }

  asm("nop");
}

void rm_SetArchPeriod(uint16_t * data,uint8_t idx)
{
  char i;
  for (i = 0; i < lstArchPeriods_Count; i++)
  {
    if ( *data != lstArchPeriods[i] ) continue;
    else
      break;
  }

  if (i < lstArchPeriods_Count)
  {
   ArchPeriod = *data;
   e_Write(&ArchPeriod,2, ArchPeriod_Addr);
  }

  asm("nop");
}

void rm_SetSAPt(uint8_t * data,uint8_t idx)
{
  if (*data <= 2)
  {
   SAPt = *data;
   e_Write(&SAPt,1, SAPt_Addr);
  }

  asm("nop");
}
void rm_SetSmoothUN(uint8_t * data,uint8_t idx)
{
  if ((*data <= 25)&&(*data >= 1))
  {
   SmoothUN = *data;
   e_Write(&SmoothUN,1, SmoothUN_Addr);
  }

  asm("nop");
}

void rm_SetUTCClock(int8_t * data,uint8_t idx)
{
  if ((*data <= 12)&&(*data >= -11))
  {
   UTCClock = *data;
   e_Write(&UTCClock,1, UTCClock_Addr);
  }

  asm("nop");
}

void rm_SetWSClock(uint8_t * data,uint8_t idx)
{
  if (*data <= 1)
  {
   WSClock = *data;
   e_Write(&WSClock,1, WSClock_Addr);
  }

  asm("nop");
}

void rm_SetScaleType(uint8_t * data,uint8_t idx)
{
  if (*data <= 1)
  {
   ScaleType = *data;
   e_Write(&ScaleType,1, ScaleType_Addr);
  }

  asm("nop");
}

void rm_SetScreenMode(uint8_t * data,uint8_t idx)
{
  if (*data <= 2)
  {
   ScreenMode = *data;
   e_Write(&ScreenMode,1, ScreenMode_Addr);
  }

  asm("nop");
}

void rm_SetSpeedMove(uint8_t * data,uint8_t idx)
{
  if (*data <= 4)
  {
   SpeedMove = *data;
   e_Write(&SpeedMove,1, SpeedMove_Addr);
  }

  asm("nop");
}
*/