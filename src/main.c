#include "../inc/includes.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

extern void USART6_IRQHandler(void);

int main(int argc, char **argv)
{
//   StartUart1Manager();
//  StartUart3Manager();
    StartUart6Manager();
//
    MTBusManagerStart();
    TBusManagerInit();
    
    //    ND_Init();
     
    //ND_Bind();

	printf("\nInit modules - OK..\n");
        uint8_t DevAddr = 0x04;  // for DV05
        float T = 0.0f,
   	      Rh = 0.0f;
        uint8_t * pData = (void*)0;
        uint32_t kk = 0;
        //struct winsize ws;
        //ioctl(STDOUT_FILENO,TIOCGWINSZ, &ws);

	while(1)
	{
        MTBusManagerTimeOutCounter();
        TBusManagerTimeOutTimer();

		UartDriverRun();
        MTBusManagerRun();
        TBusManagerRun();


		USART6_IRQHandler();
   
		//		ND_GetValues();
                 
		if (GetDevOnlineStatus(DevAddr))
		  {
		    if (GetVariableStatus(DevAddr, 0x00) == VARIABLE_IS_INIT_STATUS)
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

			    //if (mod == 1)
			    if ((kk++ % 1000)==0)
			      printf("T= %3.1f  ,Rh= %3.0f \r", T, Rh);
			  }
    	                }
		  }  
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
