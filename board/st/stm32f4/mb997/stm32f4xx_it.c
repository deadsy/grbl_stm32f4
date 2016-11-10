//-----------------------------------------------------------------------------
/*

Interrupt Handlers

*/
//-----------------------------------------------------------------------------

#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "debounce.h"

//-----------------------------------------------------------------------------

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    while (1);
}

void MemManage_Handler(void)
{
    while (1);
}

void BusFault_Handler(void)
{
    while (1);
}

void UsageFault_Handler(void)
{
    while (1);
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
    uint32_t ticks = HAL_GetTick();

    // blink the green led every 512 ms
    if ((ticks & 511) == 0) {
        gpio_toggle(LED_GREEN);
    }

    // sample debounced inputs every 16 ms
    if ((ticks & 15) == 0) {
        debounce_isr();
    }

    HAL_IncTick();
}

//-----------------------------------------------------------------------------
// STM32F4xx Peripherals Interrupt Handlers

#ifdef USB_SERIAL
extern PCD_HandleTypeDef hpcd;
void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd);
}
#endif

//-----------------------------------------------------------------------------
