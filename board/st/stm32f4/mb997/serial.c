//-----------------------------------------------------------------------------
/*

Low level grbl serial routines implemented using the USART driver.

*/
//-----------------------------------------------------------------------------

#include "stm32f4xx_hal.h"
#include "usart.h"
#include "serial.h"

//-----------------------------------------------------------------------------

void serial_init(void) {
  // do nothing
}

// write a character to the tx buffer
void serial_write(uint8_t data) {
  usart_putc(data);
}

// read a character from the rx buffer
uint8_t serial_read(void) {
  if (usart_tstc()) {
    uint8_t c = usart_getc();
    if (!serial_rx_hook(c)) {
      return c;
    }
  }
  return SERIAL_NO_DATA;
}

// Reset and empty data in read buffer. Used by e-stop and reset.
void serial_reset_read_buffer(void) {
  while (usart_tstc()) {
    usart_getc();
  }
}

// Returns the number of bytes available in the RX serial buffer.
int serial_get_rx_buffer_available(void) {
  return usart_rxbuf_avail();
}

// Returns the number of bytes used in the RX serial buffer.
int serial_get_rx_buffer_count(void) {
  return usart_rxbuf_used();
}

// Returns the number of bytes used in the TX serial buffer.
int serial_get_tx_buffer_count(void) {
  return usart_txbuf_used();
}

//-----------------------------------------------------------------------------
