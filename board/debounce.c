//-----------------------------------------------------------------------------
/*

Switch Input Debounce

This code debounces up to 32 switch inputs (each switch state is a bit in a uint32_t).
To fit with a typical "input pulled high" scenario, off = 1 and on = 0.

The code samples the inputs in an ISR at an interval selectable by the user (typically 10-20ms)
and stores them in an array of DEBOUNCE_COUNT values.
When a debounced switch state is needed the samples in the array are ANDed together.
This implies the code is fast to recognise an ON (single 0) and slow to recognise
an off (DEBOUNCE_COUNT 1's).

*/
//-----------------------------------------------------------------------------

#include <string.h>
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "debounce.h"

//-----------------------------------------------------------------------------

DEBOUNCE_CTRL debounce;

//-----------------------------------------------------------------------------
// return the de-bounced state of the switches

uint32_t debounce_rd(void)
{
    uint32_t state = 0xffffffff;
    int i;
    for (i = 0; i < DEBOUNCE_COUNT; i ++) {
        state &= debounce.sample[i];
    }
    return state;
}

//-----------------------------------------------------------------------------
// read and store the current switch state
// this is called periodically (10-20ms) from a timer ISR

void debounce_isr(void)
{
    DEBOUNCE_CTRL *db = &debounce;

    // read and store the current input
    db->sample[db->idx] = debounce_input();
    db->idx = (db->idx == DEBOUNCE_COUNT - 1) ? 0 : db->idx + 1;

    if (db->monitor) {
        uint32_t state = debounce_rd();
        if (state != db->state) {
            uint32_t turn_on = db->state & ~state; // 1 to 0: switch is now ON
            uint32_t turn_off = ~db->state & state; // 0 to 1: switch is now OFF
            if (turn_on && db->action_on) {
                db->action_on(turn_on);
            }
            if (turn_off && db->action_off) {
                db->action_off(turn_off);
            }
            db->state = state;
        }
    }
}

//-----------------------------------------------------------------------------

void debounce_init(void)
{
    memset(&debounce, 0, sizeof(DEBOUNCE_CTRL));
}

//-----------------------------------------------------------------------------
