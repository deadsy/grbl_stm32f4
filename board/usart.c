//-----------------------------------------------------------------------------
/*

USART Driver

*/
//-----------------------------------------------------------------------------

#include "stm32f4xx_hal.h"
#include "usart.h"

//-----------------------------------------------------------------------------

// enable the peripheral clock for the usart
static void enable_usart_clock(USART_TypeDef *usart)
{
    if (usart == USART1) {
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    } else if (usart == USART2) {
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    }
}

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
    uint32_t val;

    // enable peripheral clock
    enable_usart_clock(usart);

    // disable the uart
    usart->CR1 &= ~USART_CR1_UE;

    // set CR1
    val = usart->CR1;
    usart->CR1= val;

    // set CR2
    val = usart->CR2;
    val &= ~(USART_CR2_LINEN | USART_CR2_CLKEN);
    usart->CR2= val;

    // set CR3
    val = usart->CR3;
    val &= ~(USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN);
    usart->CR3= val;

    // clear the status register
    usart->SR = 0;

    usart->BRR = 0;
    usart->GTPR = 0;

    // enable the uart
    usart->CR1 |= USART_CR1_UE;
}

//-----------------------------------------------------------------------------
