#ifndef _connect_h
#define _connect_h

#include "../inc/includes.h"

//#include <stdint.h>
//#include "main.h"

//#define RAS_Count 30

typedef	void (*func_t)(void * param, uint8_t index);

//#pragma pack (push,1)
typedef struct tRAS
{
  void * Addr;
  uint8_t vType;
  uint8_t vCount;
  func_t func;
  uint8_t vInfo[18];
}tRAS_t;
//#pragma pack (pop)

extern const tRAS_t RAS[];
extern const uint16_t RAS_Count;


//#include "rtc.h"

#endif

