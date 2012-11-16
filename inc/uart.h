#ifndef _uart_h
#define _uart_h

extern int  Open( void );
extern void Close( int hSerial );
extern char ReadChar( int hSerial );
extern int  WriteChar( int hSerial, char data );

#endif
