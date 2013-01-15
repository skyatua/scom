#include "../inc/includes.h"

//!Tanya incs
#include <stdlib.h>
#include <signal.h>
#include <time.h>
//#include <mysql/mysql.h>

//#include "/usr/include/mysql/mysql.h"

char strQuery[255];
char timeSet = 0;

void timerFunc(int signum);
void formStrQuery(float * ft, float * frh);
void insertData();

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
      formStrQuery(&T,&Rh);
      insertData();
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

void formStrQuery(float * ft, float * frh)
{
#ifdef __LINX__
  char sDtTm[25];
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

  sprintf(strQuery, "INSERT INTO `temp` (tempt, Rh, dateT)\
                     VALUE (%3.1f, %3.0f, \"%s\");",
	  *ft, *frh, sDtTm);
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
