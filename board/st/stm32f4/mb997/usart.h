//-----------------------------------------------------------------------------
/*

USART Driver

*/
//-----------------------------------------------------------------------------

#ifndef USART_H
#define USART_H

//-----------------------------------------------------------------------------

void usart_init(void);

void usart_putc(char c);
void usart_flush(void);

int usart_tstc(void);
char usart_getc(void);

int usart_rxbuf_used(void);
int usart_rxbuf_avail(void);
int usart_rxbuf_size(void);

int usart_txbuf_used(void);
int usart_txbuf_avail(void);
int usart_txbuf_size(void);

//-----------------------------------------------------------------------------

#endif // USART_H

//-----------------------------------------------------------------------------
