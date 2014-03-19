//----------------------------------------------------------------------------
/*

Replacement serial functions for GRBL.
These will use the USB CDC device on STM32F4Discovery Board.

*/
//----------------------------------------------------------------------------

#include <inttypes.h>

//----------------------------------------------------------------------------

void serial_init(void)
{
}

void serial_write(uint8_t data)
{
}

uint8_t serial_read(void)
{
    return 0;
}

// Reset and empty data in read buffer. Used by e-stop and reset.
void serial_reset_read_buffer(void)
{
}

//----------------------------------------------------------------------------
