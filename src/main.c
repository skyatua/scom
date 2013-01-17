#include "../inc/includes.h"

#include <stdio.h>
#include <unistd.h>

extern void USART3_IRQHandler(void);
extern void USART6_IRQHandler(void);

tSens Sensors[ND_CNT];

int main(int argc, char **argv)
{
//   StartUart1Manager();
  StartUart3Manager();
  StartUart6Manager();
//
    MTBusManagerStart();
    TBusManagerInit();

    Archive_Init();

//    ND_Init();
//    ND_Bind();

	printf("\nInit modules - OK..\n");
         
        uint32_t kk = 0;
        uint8_t idx = 0;
        
        for ( idx = 0; idx < ND_CNT; idx++) 
	  {  Sensors[idx].DevAddr = 0; }

        // ZigBit 
        Sensors[0].DevAddr = 16;
        Sensors[1].DevAddr = 21;
        Sensors[2].DevAddr = 22;
	Sensors[3].DevAddr = 23;


        
	while(1)
	{
        MTBusManagerTimeOutCounter();
        TBusManagerTimeOutTimer();

	UartDriverRun();
        MTBusManagerRun();
        TBusManagerRun();

        USART3_IRQHandler();
	USART6_IRQHandler();

        // ND_GetValues();

        for ( idx = 0; idx < ND_CNT; idx++)
	  {
            Sensors[idx].Online = 0;

            if ( Sensors[idx].DevAddr == 0 ) continue; 

	    Sensors[idx].Online = GetDevOnlineStatus(Sensors[idx].DevAddr);
            
            if ( Sensors[idx].Online ) 
	      {
                if ( GetVariableStatus(Sensors[idx].DevAddr, 0x00) == VARIABLE_IS_INIT_STATUS )
		  {
		    
		    SwitchAcces(Sensors[idx].DevAddr, 0x00, HOST_PORT_2);
		    Sensors[idx].T[0] = (float)(*(int16_t*)(GetDataPointer())/10.0f);
                    Sensors[idx].T[1] = (float)(*(int16_t*)(GetDataPointer()+2));  
                    
                    SwitchAcces(Sensors[idx].DevAddr, 0x01, HOST_PORT_2);
                    Sensors[idx].ParDevAddr = *(uint8_t*)(GetDataPointer());
                    
		    SwitchAcces(Sensors[idx].DevAddr, 0x02, HOST_PORT_2);
                    Sensors[idx].rssi = *(int8_t*)(GetDataPointer());
                    
                    SwitchAcces(Sensors[idx].DevAddr, 0x03, HOST_PORT_2);
                    Sensors[idx].lqi = *(uint8_t*)(GetDataPointer());
		    
                   }
	      }
	  }

	Archive_SaveRecord();
        if ( (kk++ % 10000)==0 )
	  {
	    for ( idx = 0; idx < ND_CNT; idx++)
	      {
		if ( Sensors[idx].Online == 0 ) continue;
		
		printf("\nAddr = %d, status = %d, T = %3.1f, Parent = %d, rssi = %d, lqi = %3.1f ",
		       Sensors[idx].DevAddr,
		       Sensors[idx].Online,
		       Sensors[idx].T[0],
		       Sensors[idx].ParDevAddr,
		       Sensors[idx].rssi,
		       (float)(Sensors[idx].lqi)/255.0f);
	      }
	  }
#ifdef __LINX__
                usleep(1000);
#endif
		//test_wr();
	}

    StopUart3Manager();
    StopUart6Manager();

/*
	int h = Open();
	char data;
	if (h != -1)
	{
	    printf("Port open!\n");

		while(1)
		{
			data = ReadChar(h);
			WriteChar(h,data);
			printf("\ndata = %d",data);
			}

		Close(h);
		printf("Port close!\n");
		}
	*/
	return 0;
}
