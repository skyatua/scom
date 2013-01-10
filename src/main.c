#include "../inc/includes.h"

#include <stdio.h>
#include <unistd.h>
extern void USART6_IRQHandler(void);

int main(int argc, char **argv)
{
//   StartUart1Manager();
//  StartUart3Manager();
    StartUart6Manager();
//
    MTBusManagerStart();
    TBusManagerInit();

    ND_Bind();

	printf("\nInit modules - OK..\n");

	while(1)
	{
        MTBusManagerTimeOutCounter();
        TBusManagerTimeOutTimer();

		UartDriverRun();
        MTBusManagerRun();
        TBusManagerRun();


		USART6_IRQHandler();
   
		ND_GetValues();

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
