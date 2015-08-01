//-----------------------------------------------------------------------------
/*

USART Driver

Using USART2/AF7 mapped to PA2(tx) PA3(rx).

*/
//-----------------------------------------------------------------------------

#include "stm32f4xx_hal.h"
#include "usart.h"
#include "serial.h"
#include "gpio.h"

//-----------------------------------------------------------------------------

//#define SERIAL_POLLED

//-----------------------------------------------------------------------------
// polled driver

#ifdef SERIAL_POLLED

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

//-----------------------------------------------------------------------------
// interrupt driven driver

#else

#define INC_MOD(x, s) (((x) + 1) & ((s) - 1))

#define TXBUF_SIZE 128 // must be a power of 2
#define RXBUF_SIZE 512 // must be a power of 2

static uint8_t txbuf[TXBUF_SIZE];
static uint8_t rxbuf[RXBUF_SIZE];
static volatile int rx_wr, rx_rd, tx_wr, tx_rd;
static int rx_errors;

#define cli() NVIC_DisableIRQ(USART2_IRQn)
#define sei() NVIC_EnableIRQ(USART2_IRQn)

void USART2_IRQHandler(void)
{
  USART_TypeDef* const usart = USART2;
  uint32_t status = usart->SR;

  // check for rx errors
  if (status & (USART_SR_ORE | USART_SR_PE | USART_SR_FE | USART_SR_NE)) {
    rx_errors ++;
  }

  // receive
  if (status & USART_SR_RXNE) {
    uint8_t c = usart->DR;
    int rx_wr_inc = INC_MOD(rx_wr, RXBUF_SIZE);
    if (rx_wr_inc != rx_rd) {
      rxbuf[rx_wr] = c;
      rx_wr = rx_wr_inc;
    } else {
      // rx buffer overflow
      rx_errors ++;
    }
  }

  // transmit
  if (status & USART_SR_TXE) {
    if (tx_rd != tx_wr) {
      usart->DR = txbuf[tx_rd];
      tx_rd = INC_MOD(tx_rd, TXBUF_SIZE);
    } else {
      // no more tx data, disable the tx empty interrupt
      usart->CR1 &= ~USART_CR1_TXEIE;
    }
  }

  // indicate any rx errors
  if (rx_errors) {
    gpio_set(LED_BLUE);
  }
}

void usart_putc(char c)
{
  USART_TypeDef* const usart = USART2;
  int tx_wr_inc = INC_MOD(tx_wr, TXBUF_SIZE);
  // wait for space
  while (tx_wr_inc == tx_rd);
  // put the character in the tx buffer
  cli();
  txbuf[tx_wr] = c;
  tx_wr = tx_wr_inc;
  sei();
  // enable the tx empty interrupt
  usart->CR1 |= USART_CR1_TXEIE;
}

void usart_flush(void)
{
}

// return non-zero if we have rx data
int usart_tstc(void)
{
  int status;
  cli();
  status = (rx_rd != rx_wr);
  sei();
  return status;
}

char usart_getc(void)
{
  char c;
  // wait for a character
  while (usart_tstc() == 0);
  cli();
  c = rxbuf[rx_rd];
  rx_rd = INC_MOD(rx_rd, RXBUF_SIZE);
  sei();
  return c;
}

#endif

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

    set_baud_rate(usart, 115200);

    // GTPR - no changes
    val = usart->GTPR;
    usart->GTPR = val;

#ifndef SERIAL_POLLED
    rx_wr = rx_rd = tx_wr = tx_rd = 0;
    rx_errors = 0;
    // enable the rx not empty interrupt
    usart->CR1 |= USART_CR1_RXNEIE;
    // enable USART2 interrupt
    HAL_NVIC_SetPriority(USART2_IRQn, 10, 0);
    NVIC_EnableIRQ(USART2_IRQn);
#endif

    // enable the uart
    usart->CR1 |= USART_CR1_UE;
}

//-----------------------------------------------------------------------------
// these are the serial api functions used by grbl and stdio

void serial_init(void)
{
    // do nothing
}

// write a character to the tx buffer
void serial_write(uint8_t data)
{
  usart_putc(data);
}

// hook up stdio output to the serial port
int __io_putchar(int ch)
{
  usart_putc(ch);
  return 0;
}

// read a character from the rx buffer
uint8_t serial_read(void)
{
  if (usart_tstc()) {
    uint8_t c = usart_getc();
    if (!serial_rx_hook(c)) {
      return c;
    }
  }
  return SERIAL_NO_DATA;
}

// Reset and empty data in read buffer. Used by e-stop and reset.
void serial_reset_read_buffer(void)
{
  while (usart_tstc()) {
    usart_getc();
  }
}

// Returns the number of bytes used in the RX serial buffer.
uint8_t serial_get_rx_buffer_count(void)
{
  // TODO
  return 0;
}

// Returns the number of bytes used in the TX serial buffer.
uint8_t serial_get_tx_buffer_count(void)
{
  // TODO
  return 0;
}

//-----------------------------------------------------------------------------
