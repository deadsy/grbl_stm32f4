//-----------------------------------------------------------------------------
/*

Delay Functions

*/
//-----------------------------------------------------------------------------

#include "stm32f4xx_hal.h"
#include "delay.h"

//-----------------------------------------------------------------------------

#define TICK_CLK 168 // systick count down clock in MHz

static uint64_t get_current_usecs(void)
{
    return (HAL_GetTick() * 1000) + 1000 - (SysTick->VAL / TICK_CLK);
}

//-----------------------------------------------------------------------------

void _delay_ms(unsigned _ms)
{
    HAL_Delay(_ms);
}

void _delay_us(unsigned _us)
{
    uint64_t timeout = get_current_usecs() + _us;
    while (get_current_usecs() < timeout);
}

//-----------------------------------------------------------------------------