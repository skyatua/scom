#include <windows.h>

#include "uart.h"

int Open( void )
{
	int hSerial = (int) CreateFile
	(	"COM4",
	    GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		0,
		0);
	if (hSerial == -1)
	{  // can't open port
		return -1;
		}
		
	DCB dcbSP = {0};
	dcbSP.DCBlength = sizeof(dcbSP);
	if (!GetCommState((HANDLE)hSerial,&dcbSP))
	{  // could not get the state of the port
		return -1;
		}
	dcbSP.BaudRate = 9600;
	dcbSP.ByteSize = 8;
	dcbSP.StopBits = ONESTOPBIT;
	dcbSP.Parity = NOPARITY;
	
	if (!SetCommState((HANDLE)hSerial,&dcbSP))
	{  // analyse error
		return -1;
		}
/*
 * COMMTIMEOUTS timeouts={0};
timeouts.ReadIntervalTimeout=50;
timeouts.ReadTotalTimeoutConstant=50;
timeouts.ReadTotalTimeoutMultiplier=10;
timeouts.WriteTotalTimeoutConstant=50;
timeouts.WriteTotalTimeoutMultiplier=10;
if(!SetCommTimeouts(hSerial, &timeouts)){
    //handle error
}
return hSerial;
 * */		
	return hSerial;
	}
	
void Close(int hSerial)
{
	if (hSerial != -1)
	{
		CloseHandle((HANDLE)hSerial);
		}
	}
	
char ReadChar(int hSerial)
{
	char data = 0;
	int count = 0;
	if (hSerial != -1)
	{
		while(1)
		{   
			ReadFile((HANDLE)hSerial,&data,1,&count, NULL );
			
			if (count != 0)
				break;
			}
		}
	return data;
	}
	
int WriteChar(int hSerial, char data)
{
	int count = 0;
	if (hSerial != -1)
	{
		WriteFile((HANDLE)hSerial, &data, 1, &count, NULL);
		}
	return count;
	}




