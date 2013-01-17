#include "../inc/includes.h"
 
//!Tanya incs
#include <stdlib.h>
#include <signal.h>
#include <time.h>
//#include <mysql/mysql.h>

//#include "/usr/include/mysql/mysql.h"

char strQuery[255];
char sDtTm[25];

char timeSet = 0;

void timerFunc(int signum);
void formStrQuery(float * ft, uint8_t addr);
void formStrQueryU(uint16_t battery, int8_t iRssi, float iLqi, uint8_t addr);
void insertData();
void getSDtTm();

void Archive_Init( void )
{
#ifdef __LINX__
 signal(SIGALRM, timerFunc); 
 alarm(60);

 printf("\nArchive_Init exec");
#endif
}

void Archive_SaveRecord( void )
{
#ifdef __LINX__
//  alarm(60);
  if (timeSet == 1)
    {
      timeSet = 0;
      printf("\nSave record...");
      getSDtTm(); 
      int idx;
      for (idx = 0; idx < ND_CNT; idx++)
      {
	//if ( Sensors[idx].DevAddr == 16 ) continue;
	if ( Sensors[idx].DevAddr == 0 ) continue;

         formStrQuery((float*)&Sensors[idx].T[0], Sensors[idx].DevAddr);
         insertData();
         formStrQueryU(Sensors[idx].T[1], 
                       Sensors[idx].rssi, 
     		       (float)Sensors[idx].lqi/255.0f, 
		       Sensors[idx].DevAddr);
 	 insertData();
      }
     alarm(60);
     }
#endif
 }

void timerFunc( int signum )
{
#ifdef __LINX__
  timeSet = 1;
#endif

}

void getSDtTm()
{
#ifdef __LINX__

time_t rawtime;
  struct tm * timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  sprintf(sDtTm, "%d-%d-%d %d:%d:%d",
          timeinfo->tm_year + 1900,
          timeinfo->tm_mon + 1,
          timeinfo->tm_mday,
          (timeinfo->tm_hour+2)%24,
          timeinfo->tm_min,
          timeinfo->tm_sec);

#endif

return;
}

void formStrQuery(float * ft, uint8_t addr)
{
#ifdef __LINX__

  sprintf(strQuery, "INSERT INTO `temp` (tempt, dateT, adrDatch)\
                     VALUE (%3.1f, \"%s\", %d);",
	  *ft, sDtTm, addr);
#endif


  return;
}

void formStrQueryU(uint16_t battery, int8_t iRssi, float iLqi, uint8_t addr)
{
#ifdef __LINX__

  sprintf(strQuery, "UPDATE `stat` SET battery=%d, Rssi=%d, lqi=%3.1f\
                     WHERE adrDatch=%d;",
                    battery, iRssi, iLqi, addr);
#endif


  return;
}




void insertData( void )
{
#ifdef __LINX__
  MYSQL *conn;  
  //MYSQL_RES *res; 
  MYSQL_ROW row;  
  char *server = "127.0.0.1";  
  char *user = "root";    
  char *password = "123456"; // set me first 
  char *database = "TRh"; 
  conn = mysql_init(NULL);   
  // Connect to database   
  if (!mysql_real_connect(conn, server, 
			  user, password, database, 0, NULL, 0)) { 
    fprintf(stderr, "%s\n", mysql_error(conn));  
    exit(1);  
  }       
  if (mysql_query(conn, strQuery)) {   
    fprintf(stderr, "%s\n", mysql_error(conn)); 
    exit(1);  
  }   
  mysql_close(conn);  
#endif

  return;
}
