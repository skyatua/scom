#include "../inc/includes.h"

//#include "MTBusRoutingTable.h"
//#include "MTBusConfiguration.h"
//#include "netconfig.h"

#ifdef RoutingDisable
const uint16_t RoutingTableCnt = 1;
const RoutTable_t RoutingTable[] = 
{
//Addr èñò. Idx èñò.  Addr ïîëó÷. Idx ïîëó÷.  
{   0,       0,         0,         0},
};
#endif

#ifdef RoutingEnable
const RoutTable_t RoutingTable[] = 
{
//Addr èñò. Idx èñò.  Addr ïîëó÷. Idx ïîëó÷.  
  {   &NetConfigAKSNetAdrr[0], 41, &NetConfigAKSNetAdrr[1], 41},  // DamperOpenCmd
  {   &NetConfigAKSNetAdrr[0], 42, &NetConfigAKSNetAdrr[1], 42},  // DamperCloseCmd
};
const uint16_t RoutingTableCnt = sizeof(RoutingTable)/sizeof(RoutTable_t);
#endif