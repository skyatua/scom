#include "../inc/includes.h"

tDVDevice DVDevice;
tZBDevice ZBDevice;

tND ND_DV,  // DV05 - th, rh
    ND_ZB;  // ZigBit sensors

uint32_t NDTime_Pr;

void ND_Init( void )
{
    ND_DV.Cnt = 0;
    ND_DV.Idx = 0;
    ND_DV.Device = (uint8_t*)&DVDevice;

    ND_ZB.Cnt = 0;
    ND_ZB.Idx = 0;
    ND_ZB.Device = (void*)0;
}

void ND_Bind( void )
{
uint8_t  Cnt     = 1, // NetConfigGetCntDevice(),
         DevAddr = 0,
         i       = 0,
         zb_idx  = 0;

 if ( ND_DV.Device != (void*)0 )
 {
  tDVDevice * pDevice = (tDVDevice*)ND_ZB.Device;

  for ( i = 0; i < DV_DEVICE_CNT; i++ )
   { pDevice[i]->Addr = 0; }

  if ( Cnt > 0 )
  {
   for ( i = 0; i < Cnt; i++)
   {
     DevAddr = i+10;//NetConfigGetAddrDevice( i );

     //if ((DevAddr > 0) &&
     //     (HOST_PORT_2 == NetConfigGetHostDevice( DevAddr )))
      {
       pDevice[zb_idx]->Addr = DevAddr;

       if (++zb_idx < ZB_DEVICE_CNT - 1)
       {
         ND_ZB.Cnt = zb_idx;
       }
       else
       { break; }
      }
   } // for ( i = 0; i < Cnt; i++)
  } // if ( Cnt > 0 )
 } // if ( ND_ZB.Device != (void*)0 )

 if ( ND_ZB.Device != (void*)0 )
 {
  tZBDevice * pDevice = (tZBDevice*)ND_ZB.Device;

  for ( i = 0; i < ZB_DEVICE_CNT; i++ )
   { pDevice[i]->Addr = 0; }

  if ( Cnt > 0 )
  {
   for ( i = 0; i < Cnt; i++)
   {
     DevAddr = i+10;//NetConfigGetAddrDevice( i );

     //if ((DevAddr > 0) &&
     //     (HOST_PORT_2 == NetConfigGetHostDevice( DevAddr )))
      {
       pDevice[zb_idx]->Addr = DevAddr;

       if (++zb_idx < ZB_DEVICE_CNT - 1)
       {
         ND_ZB.Cnt = zb_idx;
       }
       else
       { break; }
      }
   } // for ( i = 0; i < Cnt; i++)
  } // if ( Cnt > 0 )
 } // if ( ND_ZB.Device != (void*)0 )

}

void ND_GetValues( void )
{
 uint8_t   n       = 0,
           idx     = 0,
           zb_idx  = 0,
           DevAddr = 0;
 uint8_t * pDevName,
         * pProgVer;

 HostPort_t host = HOST_PORT_UNKNOWN;

  //if (NDTime_Pr == 0) return;
  //NDTime_Pr = 0;

  n = GetOnlineDevCnt();
  for ( idx = 0; idx < n; idx++ )
  {
    DevAddr = GetDevAddr(idx);

    if (DevAddr == 0) continue;

    host =  GetHostPort(DevAddr);
    pDevName = GetDevName(DevAddr);
    pProgVer = GetProgVer(DevAddr);

    //
    if (HOST_PORT_1 == host)
    {
      // DV05 sensors
      if  (( ((uint8_t *)pDevName)[0] == 'D') &&
             (((uint8_t *)pDevName)[1] == 'V') )
      {
        if ( ND_DV.Device == (void*)0 ) continue;

        tDVDevice * pDevice = ((tDVDevice*)ND_DV.Device);

        for ( zb_idx = 0; zb_idx < ND_DV.Cnt; zb_idx++ )
        {
         if ( DevAddr == pDevice[zb_idx]->Addr)
          { break; }
         }

        if ((zb_idx >= ND_DV.Cnt) ||
            (zb_idx >= DV_DEVICE_CNT)) continue;

        pDevice[zb_idx]->pName     = pDevName;
        pDevice[zb_idx]->pVersion  = pProgVer;
        pDevice[zb_idx]->Online    = GetDevOnlineStatus(DevAddr);

        if ( pDevice[zb_idx]->Online )
        {

        //if ( ((uint8_t*)pDevice[zb_idx]->pName)[2] == 'S')
        { // sensors

         // if ( ((uint8_t*)pDevice[zb_idx]->pVersion)[9] == '1')
          {
            SwitchAcces(pDevice[zb_idx]->Addr, 0x00, host);
            pDevice[zb_idx]->RAS.aVl[0] = (*(float*)GetDataPointer());
            pDevice[zb_idx]->RAS.aVl[1] = (*(float*)(GetDataPointer()+4));

            SwitchAcces(pDevice[zb_idx]->Addr, 0x01, host);
            pDevice[zb_idx]->RAS.Vl_Status[0] = (*(uint8_t*)GetDataPointer());
            pDevice[zb_idx]->RAS.Vl_Status[1] = (*(uint8_t*)(GetDataPointer()+1));

            SwitchAcces(pDevice[zb_idx]->Addr, 0x02, host);
            pDevice[zb_idx]->RAS.Loi[0] = (*(float*)GetDataPointer());
            pDevice[zb_idx]->RAS.Loi[1] = (*(float*)(GetDataPointer()+4));

            SwitchAcces(pDevice[zb_idx]->Addr, 0x03, host);
            pDevice[zb_idx]->RAS.Hig[0] = (*(float*)GetDataPointer());
            pDevice[zb_idx]->RAS.Hig[1] = (*(float*)(GetDataPointer()+4));

            SwitchAcces(pDevice[zb_idx]->Addr, 0x04, host);
            pDevice[zb_idx]->RAS.kV_RH_0 = *(float*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x05, host);
            pDevice[zb_idx]->RAS.kV_RH_100 = *(float*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x06, host);
            pDevice[zb_idx]->RAS.R7 = *(float*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x07, host);
            pDevice[zb_idx]->RAS.R8 = *(float*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x08, host);
            pDevice[zb_idx]->RAS.R9 = *(float*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x09, host);
            pDevice[zb_idx]->RAS.R10 = *(float*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x0A, host);
            pDevice[zb_idx]->RAS.R11 = *(float*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x0B, host);
            pDevice[zb_idx]->RAS.RhID = *(uint16_t*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x0C, host);
            pDevice[zb_idx]->RAS.dV[0] = *(float*)GetDataPointer();
            pDevice[zb_idx]->RAS.dV[1] = (*(float*)(GetDataPointer()+4));

            SwitchAcces(pDevice[zb_idx]->Addr, 0x0D, host);
            pDevice[zb_idx]->RAS.kV[0] = *(float*)GetDataPointer();
            pDevice[zb_idx]->RAS.kV[1] = (*(float*)(GetDataPointer()+4));

            SwitchAcces(pDevice[zb_idx]->Addr, 0x0E, host);
            pDevice[zb_idx]->RAS.vRef = *(float*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x0F, host);
            pDevice[zb_idx]->RAS.TypeSens[0] = *(uint8_t*)GetDataPointer();
            pDevice[zb_idx]->RAS.TypeSens[1] = (*(uint8_t*)(GetDataPointer()+1));

            SwitchAcces(pDevice[zb_idx]->Addr, 0x10, host);
            pDevice[zb_idx]->RAS.aAx[0] = *(int32_t*)GetDataPointer();
            pDevice[zb_idx]->RAS.aAx[1] = (*(int32_t*)(GetDataPointer()+4));
            pDevice[zb_idx]->RAS.aAx[2] = (*(int32_t*)(GetDataPointer()+8));
            pDevice[zb_idx]->RAS.aAx[3] = (*(int32_t*)(GetDataPointer()+12));

          }// if ( MDevId[i].ProgVer[9] == '1')
        } // if ( ((uint8_t*)pDevice[zb_idx]->pName)[2] == 'S')
       } // if ( pDevice[zb_idx]->Online )
      } // DV05 sensors
    } // if (HOST_PORT_1 == host)

    // Wireles devices
    if (HOST_PORT_2 == host)
    {

      if ( ND_ZB.Device == (void*)0 ) continue;

      tZBDevice * pDevice = ((tZBDevice*)ND_ZB.Device);

      for ( zb_idx = 0; zb_idx < ND_ZB.Cnt; zb_idx++ )
      {
       if ( DevAddr == pDevice[zb_idx]->Addr)
        { break; }
      }

      if ((zb_idx >= ND_ZB.Cnt) ||
           (zb_idx >= ZB_DEVICE_CNT)) continue;

      pDevice[zb_idx]->pName     = pDevName;
      pDevice[zb_idx]->pVersion  = pProgVer;
      pDevice[zb_idx]->Online    = GetDevOnlineStatus(DevAddr);

      if ( pDevice[zb_idx]->Online )
      {

        if ( ((uint8_t*)pDevice[zb_idx]->pName)[2] == 'S')
        { // sensors

          if ( ((uint8_t*)pDevice[zb_idx]->pVersion)[9] == '1')
          {
            SwitchAcces(pDevice[zb_idx]->Addr, 0x00, HOST_PORT_2);
            pDevice[zb_idx]->RAS.Temperature = (*(float*)GetDataPointer());

            SwitchAcces(pDevice[zb_idx]->Addr, 0x01,HOST_PORT_2);
            pDevice[zb_idx]->RAS.T_Status = *(uint8_t*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x02,HOST_PORT_2);
            pDevice[zb_idx]->RAS.ParDevAddr = *(uint8_t*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x03,HOST_PORT_2);
            pDevice[zb_idx]->RAS.rssi = *(int8_t*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x04,HOST_PORT_2);
            pDevice[zb_idx]->RAS.SendInterval = *(uint16_t*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x05,HOST_PORT_2);
            pDevice[zb_idx]->RAS.Humibility = *(uint8_t*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x06,HOST_PORT_2);
            pDevice[zb_idx]->RAS.OnLine = *(uint8_t*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x07,HOST_PORT_2);
            pDevice[zb_idx]->RAS.lqi = *(uint8_t*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x08,HOST_PORT_2);
            pDevice[zb_idx]->RAS.Offset = *(float*)GetDataPointer();

            SwitchAcces(pDevice[zb_idx]->Addr, 0x09,HOST_PORT_2);
            pDevice[zb_idx]->RAS.Inclin = *(float*)GetDataPointer();
          }// if ( MDevId[i].ProgVer[9] == '1')
          else
            if (((uint8_t*)pDevice[zb_idx]->pVersion)[9] == '2')
            {
              SwitchAcces(pDevice[zb_idx]->Addr, 0x00, HOST_PORT_2);

              pDevice[zb_idx]->RAS.Temperature = (float)(*(int16_t*)(GetDataPointer()))/10.0f;
              if ( pDevice[zb_idx]->RAS.Temperature == -999)
              {
                pDevice[zb_idx]->RAS.T_Status = 1;
              }
              else
              {
                pDevice[zb_idx]->RAS.T_Status = 0;
              }

              pDevice[zb_idx]->RAS.TTL = (*(int16_t*)(GetDataPointer()+2));

              SwitchAcces(pDevice[zb_idx]->Addr, 0x01,HOST_PORT_2);
              pDevice[zb_idx]->RAS.ParDevAddr = *(uint8_t*)GetDataPointer();

              SwitchAcces(pDevice[zb_idx]->Addr, 0x02,HOST_PORT_2);
              pDevice[zb_idx]->RAS.rssi = *(int8_t*)GetDataPointer();

              SwitchAcces(pDevice[zb_idx]->Addr, 0x03,HOST_PORT_2);
              pDevice[zb_idx]->RAS.lqi = *(uint8_t*)GetDataPointer();

              SwitchAcces(pDevice[zb_idx]->Addr, 0x04,HOST_PORT_2);
              pDevice[zb_idx]->RAS.Service = *(uint8_t*)GetDataPointer();

              if ( pDevice[zb_idx]->RAS.Service == 1)
              {
                SwitchAcces(pDevice[zb_idx]->Addr, 0x05,HOST_PORT_2);
                pDevice[zb_idx]->RAS.SendInterval = *(uint16_t*)GetDataPointer();

                SwitchAcces(pDevice[zb_idx]->Addr, 0x06,HOST_PORT_2);
                pDevice[zb_idx]->RAS.Offset = *(float*)GetDataPointer();

                SwitchAcces(pDevice[zb_idx]->Addr, 0x07,HOST_PORT_2);
                pDevice[zb_idx]->RAS.Inclin = *(float*)GetDataPointer();
              }
            } // if ( MDevId[i].ProgVer[9] == '2')

        } // if ( ((uint8_t*)ZB.Sens[zb_idx].pName)[2] == 'S')

        if ( ((uint8_t*)pDevice[zb_idx]->pName)[2] == 'R')
        { // router
           pDevice[zb_idx]->RAS.T_Status = 0x03;

           SwitchAcces(pDevice[zb_idx]->Addr, 0x00,HOST_PORT_2);
           pDevice[zb_idx]->RAS.ParDevAddr = *(uint8_t*)GetDataPointer();

           SwitchAcces(pDevice[zb_idx]->Addr, 0x01,HOST_PORT_2);
           pDevice[zb_idx]->RAS.rssi = *(int8_t*)GetDataPointer();

           SwitchAcces(pDevice[zb_idx]->Addr, 0x02,HOST_PORT_2);
           pDevice[zb_idx]->RAS.lqi = *(uint8_t*)GetDataPointer();

          // SwitchAcces(ZB.Sens[zb_idx].Addr, 0x04,HOST_PORT_2);
          // ZB.Sens[zb_idx].RAS.Service = *(uint8_t*)GetDataPointer();
           pDevice[zb_idx]->RAS.Service = 1;
           if ( pDevice[zb_idx]->RAS.Service == 1)
            {
              SwitchAcces(pDevice[zb_idx]->Addr, 0x03,HOST_PORT_2);
              pDevice[zb_idx]->RAS.SendInterval = *(uint16_t*)GetDataPointer();

              //SwitchAcces(ZB.Sens[zb_idx].Addr, 0x06,HOST_PORT_2);
              //ZB.Sens[zb_idx].RAS.Offset = *(float*)GetDataPointer();

              //SwitchAcces(ZB.Sens[zb_idx].Addr, 0x07,HOST_PORT_2);
              //ZB.Sens[zb_idx].RAS.Inclin = *(float*)GetDataPointer();
              }
        }
      }

     // if (ZB.Cnt < ++zb_idx)
     //   ZB.Cnt = zb_idx;
    }
  }

}

