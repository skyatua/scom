#ifndef _main_h
#define _main_h

#include <stdint.h>

#define ND_CNT 32

typedef struct sSens
{
  uint8_t DevAddr;
  uint8_t Online;
  float T[2];
  uint8_t ParDevAddr;
  int8_t  rssi;
  uint8_t lqi;

  float Inclin;
  float Offset;
}tSens;

extern tSens Sensors[];

#endif
