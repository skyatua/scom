#include "../inc/includes.h"

#include <stdio.h>
#include <unistd.h>

extern void USART6_IRQHandler(void);

float T = 0.0f,
      Rh = 0;

int main(int argc, char **argv)
{
//   StartUart1Manager();
//  StartUart3Manager();
    StartUart6Manager();
//
    MTBusManagerStart();
    TBusManagerInit();

    Archive_Init();

//    ND_Init();
//    ND_Bind();

	printf("\nInit modules - OK..\n");
        uint8_t DevAddr = 0x04;  // for DV05
        uint8_t * pData = (void*)0;
        uint32_t kk = 0;
        uint8_t status = 0,
                is_init_var = 0;

	while(1)
	{
        MTBusManagerTimeOutCounter();
        TBusManagerTimeOutTimer();

		UartDriverRun();
        MTBusManagerRun();
        TBusManagerRun();


		USART6_IRQHandler();

        // ND_GetValues();

        status = GetDevOnlineStatus(DevAddr);
        is_init_var = GetVariableStatus(DevAddr, 0x00);
   		//if ( status )
		  {
		    if (is_init_var == VARIABLE_IS_INIT_STATUS)
		      {
			if (pData == (void*)0)
			  {
			    SwitchAcces(DevAddr, 0x00, HOST_PORT_1);
                            pData = GetDataPointer();
			    printf("\n");
			  }
                        if (pData != (void*)0)
			  {
			    // uint8_t mod = 0;
			    // float temp = *(float*)pData;
                            //if (fabs(temp - T) > 0.01f)
			    //  {
			    T  = *(float*)pData;
                            //    mod = 1;

			    //   }
			    //T = temp;
			    //if (fabs(Rh - *(float*)(pData+4)) > 1)
                            //  {
                            Rh  = *(float*)(pData+4);
                            //    mod = 1;
			    //  }

			    //}
			  }
            }
		  }

		Archive_SaveRecord();
                if ((kk++ % 10000)==0)
		  printf("\nstatus= %d, init=%d,  T= %3.1f  ,Rh= %3.0f",status, is_init_var,  T, Rh);

#ifdef __LINX__
                usleep(1000);
#endif
		//test_wr();
		}

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
