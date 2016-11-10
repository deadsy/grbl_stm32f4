//-----------------------------------------------------------------------------
/*

USART Driver

*/
//-----------------------------------------------------------------------------

#ifndef USART_H
#define USART_H

//-----------------------------------------------------------------------------

void usart_putc(char c);
void usart_flush(void);
int usart_tstc(void);
char usart_getc(void);
void usart_init(void);

ssize_t serial_write(const void *buf, size_t count);
int serial_flush(void);
int serial_getc(char *c, int timeout);

//-----------------------------------------------------------------------------


#endif // USART_H

//-----------------------------------------------------------------------------
