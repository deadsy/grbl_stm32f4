//-----------------------------------------------------------------------------
/*

CDC Interface Code

The original grbl code uses UART serial to communicate with the host.
We implement the original API here, but use the CDC interface to rx/tx
with the host.


*/
//-----------------------------------------------------------------------------

#include "stm32f4xx_hal.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_interface.h"
#include "serial.h"
#include "timers.h"

//-----------------------------------------------------------------------------

extern USBD_HandleTypeDef hUSBDDevice;

//-----------------------------------------------------------------------------
// Tx and Rx FIFOs

#define TX_FIFO_SIZE 2048
static uint8_t tx_fifo[TX_FIFO_SIZE];
static uint32_t tx_wr;
static uint32_t tx_rd;
uint32_t tx_overflow;

#define RX_FIFO_SIZE 512
static uint8_t rx_fifo[RX_FIFO_SIZE];
static uint32_t rx_wr;
static uint32_t rx_rd;
uint32_t rx_overflow;

static uint8_t rx_buffer[CDC_DATA_FS_OUT_PACKET_SIZE];

//-----------------------------------------------------------------------------

static USBD_CDC_LineCodingTypeDef LineCoding = {
    115200, // baud rate
    0x00,   // 1 stop bit
    0x00,   // no parity
    0x08    // 8 data bits
};

//-----------------------------------------------------------------------------

static int8_t CDC_Itf_Init(void)
{
    tx_wr = 0;
    tx_rd = 0;
    tx_overflow = 0;
    rx_wr = 0;
    rx_rd = 0;
    rx_overflow = 0;

    USBD_CDC_SetTxBuffer(&hUSBDDevice, tx_fifo, 0);
    USBD_CDC_SetRxBuffer(&hUSBDDevice, rx_buffer);

    cdc_timer_start();

    return USBD_OK;
}

static int8_t CDC_Itf_DeInit(void)
{
    return USBD_OK;
}

//-----------------------------------------------------------------------------

static int8_t CDC_Itf_Control (uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
    switch (cmd) {
        case CDC_SET_LINE_CODING: {
            LineCoding.bitrate = (uint32_t)(pbuf[0] | (pbuf[1] << 8) | (pbuf[2] << 16) | (pbuf[3] << 24));
            LineCoding.format = pbuf[4];
            LineCoding.paritytype = pbuf[5];
            LineCoding.datatype = pbuf[6];
            break;
        }
        case CDC_GET_LINE_CODING: {
            pbuf[0] = (uint8_t)(LineCoding.bitrate);
            pbuf[1] = (uint8_t)(LineCoding.bitrate >> 8);
            pbuf[2] = (uint8_t)(LineCoding.bitrate >> 16);
            pbuf[3] = (uint8_t)(LineCoding.bitrate >> 24);
            pbuf[4] = LineCoding.format;
            pbuf[5] = LineCoding.paritytype;
            pbuf[6] = LineCoding.datatype;
            break;
        }
        case CDC_SEND_ENCAPSULATED_COMMAND:
        case CDC_GET_ENCAPSULATED_RESPONSE:
        case CDC_SET_COMM_FEATURE:
        case CDC_GET_COMM_FEATURE:
        case CDC_CLEAR_COMM_FEATURE:
        case CDC_SET_CONTROL_LINE_STATE:
        case CDC_SEND_BREAK:
        default:
            break;
  }
  return USBD_OK;
}

//-----------------------------------------------------------------------------

static int8_t CDC_Itf_Receive(uint8_t* pbuf, uint32_t *Len)
{
    uint32_t n = *Len;
    uint32_t i;

    // Write the received buffer to the Rx fifo.
    for (i = 0; i < n; i ++) {
        if (serial_rx_hook(pbuf[i])) {
            // the character has been absorbed by the rx hook
            continue;
        }
        uint32_t rx_wr_inc = (rx_wr == (RX_FIFO_SIZE - 1)) ? 0 : rx_wr + 1;
        if (rx_wr_inc != rx_rd) {
            rx_fifo[rx_wr] = pbuf[i];
            rx_wr = rx_wr_inc;
        } else {
            rx_overflow += 1;
        }
    }

    USBD_CDC_SetRxBuffer(&hUSBDDevice, rx_buffer);
    USBD_CDC_ReceivePacket(&hUSBDDevice);
    return USBD_OK;
}

//-----------------------------------------------------------------------------

USBD_CDC_ItfTypeDef USBD_CDC_fops =
{
  CDC_Itf_Init,
  CDC_Itf_DeInit,
  CDC_Itf_Control,
  CDC_Itf_Receive
};

//-----------------------------------------------------------------------------

void cdc_timer_isr(void)
{
    uint32_t n;

    if(tx_wr != tx_rd) {
        if(tx_wr < tx_rd) {
            // The write index has wrapped around.
            // Tx from tx_rd to the end of fifo.
            n = TX_FIFO_SIZE - tx_rd;
        } else {
            // The write index is ahead of the read index
            // Tx from tx_rd to tx_wr - 1
            n = tx_wr - tx_rd;
        }

        USBD_CDC_SetTxBuffer(&hUSBDDevice, &tx_fifo[tx_rd], n);
        if(USBD_CDC_TransmitPacket(&hUSBDDevice) == USBD_OK) {
            tx_rd += n;
            if (tx_rd == TX_FIFO_SIZE) {
                tx_rd = 0;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void serial_init(void)
{
    // do nothing
}

// write a character to the tx fifo ring buffer
void serial_write(uint8_t data)
{
    uint32_t tx_wr_inc = (tx_wr == (TX_FIFO_SIZE - 1)) ? 0 : tx_wr + 1;
    int timeout = 50;

    while ((tx_wr_inc == tx_rd) && (timeout > 0)) {
        HAL_Delay(1);
        timeout -= 1;
    }

    if (timeout == 0) {
        tx_overflow += 1;
    } else {
        tx_fifo[tx_wr] = data;
        tx_wr = tx_wr_inc;
    }
}

// hook up stdio output to the serial port
int __io_putchar(int ch)
{
    serial_write(ch);
    return 0;
}

// read a character from the rx fifo ring buffer
uint8_t serial_read(void)
{
    uint8_t data = SERIAL_NO_DATA;
    if (rx_wr != rx_rd) {
        data = rx_fifo[rx_rd];
        rx_rd = (rx_rd == (RX_FIFO_SIZE - 1)) ? 0 : rx_rd + 1;
    }
    return data;
}

// Reset and empty data in read buffer. Used by e-stop and reset.
void serial_reset_read_buffer(void)
{
    rx_wr = 0;
    rx_rd = 0;
}

//-----------------------------------------------------------------------------
