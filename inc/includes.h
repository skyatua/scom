#ifndef _includes_h
#define _includes_h

//#define _DEBUG_UART_
#define _DEBUG_ND_

//#define __STM32__  // use in STM32
#define __LINX__   // use in Linux
//#define __WINX__     // use in Win32


#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#ifdef __LINX__
#include "/usr/include/mysql/mysql.h"
#endif


#include "main.h"
#include "MTBus.h"
#include "UartManager.h"
#include "CHS.h"
#include "connect.h"
#include "MTBusConfiguration.h"
#include "MTBusManager.h"
#include "MTBusServer.h"
#include "MTBusRoutingTable.h"
#include "Terabus.h"
#include "NetVarAccesManager.h"
#include "TBusManager.h"
#include "UartManager.h"
#include "UartDriver.h"
#include "netdev.h"
#include "archive.h"

#endif
