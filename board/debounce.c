//-----------------------------------------------------------------------------
/*

Switch Input Debounce

This code debounces up to 32 switch inputs (each switch state is a bit in a uint32_t).
To fit with a typical logic, off = 0 and on = 1.

The code samples the inputs in an ISR at an interval selectable by the user (typically 10-20ms)
and stores them in an array of DEBOUNCE_COUNT values.
When a debounced switch state is needed the samples in the array are ORed together.
This implies the code is fast to recognise an ON (single 1) and slow to recognise
an off (DEBOUNCE_COUNT 0's).

*/
//-----------------------------------------------------------------------------

#include <string.h>
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "debounce.h"

//-----------------------------------------------------------------------------

#define DEBOUNCE_COUNT 4

typedef struct debounce_ctrl {

    uint32_t sample[DEBOUNCE_COUNT];
    uint32_t state;
    int idx;

} DEBOUNCE_CTRL;

static DEBOUNCE_CTRL debounce;
static int debounce_ready = 0;

//-----------------------------------------------------------------------------
// null on/off handlers - to be provided by other code

__attribute__((weak)) void debounce_on_handler(uint32_t bits) {}
__attribute__((weak)) void debounce_off_handler(uint32_t bits) {}

//-----------------------------------------------------------------------------
// return the de-bounced state of the switches

uint32_t debounce_rd(void)
{
    uint32_t state = 0;
    int i;
    for (i = 0; i < DEBOUNCE_COUNT; i ++) {
        state |= debounce.sample[i];
    }
    return state;
}

//-----------------------------------------------------------------------------
// read and store the current switch state
// this is called periodically (10-20ms) from a timer ISR

void debounce_isr(void)
{
    DEBOUNCE_CTRL *db = &debounce;
    uint32_t state;

    if (!debounce_ready) {
        return;
    }

    // read and store the current input
    db->sample[db->idx] = debounce_input();
    db->idx = (db->idx == DEBOUNCE_COUNT - 1) ? 0 : db->idx + 1;

    state = debounce_rd();

    if (state != db->state) {
        uint32_t on_bits = ~db->state & state; // 0 to 1: switch is now ON
        uint32_t off_bits = db->state & ~state; // 1 to 0: switch is now OFF
        if (on_bits) {
            debounce_on_handler(on_bits);
        }
        if (off_bits) {
            debounce_off_handler(off_bits);
        }
        db->state = state;
    }
}

//-----------------------------------------------------------------------------

void debounce_init(void)
{
    memset(&debounce, 0, sizeof(DEBOUNCE_CTRL));
    debounce_ready = 1;
}

//-----------------------------------------------------------------------------
