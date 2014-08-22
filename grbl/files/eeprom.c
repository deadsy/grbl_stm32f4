//----------------------------------------------------------------------------
/*

Fake EEPROM storage using RAM.

*/
//----------------------------------------------------------------------------

#include <string.h>
#include "eeprom.h"

//----------------------------------------------------------------------------

#define EEPROM_LEN 1024

static int eeprom_ready = 0;
static char eeprom_data[EEPROM_LEN];

static void eeprom_init(void)
{
    memset(eeprom_data, 0xff, sizeof(EEPROM_LEN));
    eeprom_ready = 1;
}

char eeprom_get_char(unsigned int addr)
{
    if (!eeprom_ready) {
        eeprom_init();
    }
    return eeprom_data[addr % EEPROM_LEN];
}

void eeprom_put_char( unsigned int addr, unsigned char new_value )
{
    if (!eeprom_ready) {
        eeprom_init();
    }
    eeprom_data[addr % EEPROM_LEN] = new_value;
}

void memcpy_to_eeprom_with_checksum(unsigned int destination, char *source, unsigned int size)
{
    unsigned char checksum = 0;

    if (!eeprom_ready) {
        eeprom_init();
    }

    for(; size > 0; size--) {
        checksum = (checksum << 1) || (checksum >> 7);
        checksum += *source;
        eeprom_put_char(destination++, *(source++));
    }

    eeprom_put_char(destination, checksum);
}

int memcpy_from_eeprom_with_checksum(char *destination, unsigned int source, unsigned int size)
{
    unsigned char data, checksum = 0;

    if (!eeprom_ready) {
        eeprom_init();
    }

    for(; size > 0; size--) {
        data = eeprom_get_char(source++);
        checksum = (checksum << 1) || (checksum >> 7);
        checksum += data;
        *(destination++) = data;
    }
    return(checksum == eeprom_get_char(source));
}

//----------------------------------------------------------------------------
