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
    } else if (usart == USART3) {
        RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    } else if (usart == UART4) {
        RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
    } else if (usart == UART5) {
        RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
    } else if (usart == USART6) {
        RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
    }
}

//-----------------------------------------------------------------------------

static void set_baud_rate(USART_TypeDef *usart, int baud)
{
    if (usart->CR1 & USART_CR1_OVER8) {
        if (usart == USART1 || usart == USART6) {
            usart->BRR = __UART_BRR_SAMPLING8(HAL_RCC_GetPCLK2Freq(), baud);
        } else {
            usart->BRR = __UART_BRR_SAMPLING8(HAL_RCC_GetPCLK1Freq(), baud);
        }
    } else {
        if (usart == USART1 || usart == USART6) {
            usart->BRR = __UART_BRR_SAMPLING16(HAL_RCC_GetPCLK2Freq(), baud);
        } else {
            usart->BRR = __UART_BRR_SAMPLING16(HAL_RCC_GetPCLK1Freq(), baud);
        }
    }
}

//-----------------------------------------------------------------------------
// setup a serial port
// Using USART2/AF7 mapped to PA2(tx) PA3(rx).

void usart_putc(char c)
{
    USART_TypeDef* const usart = USART2;
    while ((usart->SR & USART_SR_TXE) == 0);
    usart->DR = c;
}

void usart_flush(void)
{
}

// return non-zero if we have rx data
int usart_tstc(void)
{
    USART_TypeDef* const usart = USART2;
    return (usart->SR & USART_SR_RXNE) != 0;
}

char usart_getc(void)
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
    val &= ~(USART_CR1_SBK | USART_CR1_RWU | USART_CR1_IDLEIE | USART_CR1_RXNEIE |\
        USART_CR1_TCIE | USART_CR1_TXEIE | USART_CR1_PEIE | USART_CR1_PS | USART_CR1_PCE |\
        USART_CR1_WAKE | USART_CR1_M | USART_CR1_UE | USART_CR1_OVER8);
    val |= (USART_CR1_RE | USART_CR1_TE);
    usart->CR1= val;

    // set CR2
    val = usart->CR2;
    val &= ~(USART_CR2_ADD | USART_CR2_LBDL | USART_CR2_LBDIE | USART_CR2_LBCL |\
        USART_CR2_CPHA | USART_CR2_CPOL | USART_CR2_CLKEN | USART_CR2_STOP |\
        USART_CR2_LINEN);
    usart->CR2= val;

    // set CR3
    val = usart->CR3;
    val &= ~(USART_CR3_EIE | USART_CR3_IREN | USART_CR3_IRLP | USART_CR3_HDSEL |\
        USART_CR3_NACK | USART_CR3_SCEN | USART_CR3_DMAR | USART_CR3_DMAT |\
        USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_CTSIE | USART_CR3_ONEBIT);
    usart->CR3= val;

    // clear the status register
    val = usart->SR;
    val &= ~(USART_SR_PE | USART_SR_FE | USART_SR_NE | USART_SR_ORE | USART_SR_IDLE |\
        USART_SR_RXNE | USART_SR_TC | USART_SR_TXE | USART_SR_LBD | USART_SR_CTS);
    usart->SR= val;

    set_baud_rate(usart, 19200);

    // GTPR - no changes
    val = usart->GTPR;
    usart->GTPR = val;

    // enable the uart
    usart->CR1 |= USART_CR1_UE;
}

//-----------------------------------------------------------------------------
