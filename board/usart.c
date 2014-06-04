//-----------------------------------------------------------------------------
/*

USART Driver

*/
//-----------------------------------------------------------------------------

#include "stm32f4xx_hal.h"
#include "usart.h"

//-----------------------------------------------------------------------------
// setup a serial port
// Using USART2/AF7 mapped to PA2(tx) PA3(rx).

void usart_tx(uint8_t data)
{
    USART_TypeDef* const usart = USART2;
    while ((usart->SR & USART_SR_TXE) == 0);
    usart->DR = data;
}

// return non-zero if we have rx data
int usart_tstc(void)
{
    USART_TypeDef* const usart = USART2;
    return (usart->SR & USART_SR_RXNE) != 0;
}

uint8_t usart_rx(void)
{
    USART_TypeDef* const usart = USART2;
    return usart->DR & 255;
}

void usart_init(void)
{
    USART_TypeDef* const usart = USART2;

    // clear the status register
    usart->SR = 0;

    usart->BRR = 0;
    usart->CR1 = 0;
    usart->CR2 = 0;
    usart->CR3 = 0;
    usart->GTPR = 0;
}

//-----------------------------------------------------------------------------
