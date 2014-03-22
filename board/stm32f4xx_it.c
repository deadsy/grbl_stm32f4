//-----------------------------------------------------------------------------
/*

Interrupt Handlers

*/
//-----------------------------------------------------------------------------

#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"

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
    HAL_IncTick();
}

//-----------------------------------------------------------------------------
// STM32F4xx Peripherals Interrupt Handlers

extern PCD_HandleTypeDef hpcd;
extern UART_HandleTypeDef UartHandle;
extern TIM_HandleTypeDef TimHandle;

void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd);
}

void USARTx_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(UartHandle.hdmatx);
}

void USARTx_IRQHandler(void)
{
    HAL_UART_IRQHandler(&UartHandle);
}

void TIMx_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TimHandle);
}

//-----------------------------------------------------------------------------
