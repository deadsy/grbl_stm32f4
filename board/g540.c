//-----------------------------------------------------------------------------
/*

Gecko G540 Stepper Controller

*/
//-----------------------------------------------------------------------------

#include "stm32f4xx_hal.h"
#include "g540.h"

//-----------------------------------------------------------------------------

#define PERIOD_VALUE (666 - 1)  // Period Value
#define PULSE1_VALUE 333        // Capture Compare 1 Value

void Error_Handler(void);

//-----------------------------------------------------------------------------
// A charge pump signal (>= 10KHz square wave) is used as a keepalive.
// When the e-stop is pressed we will stop it.
// we create this signal using the PWM output of TIM3.

static void keepalive_start(void)
{
    TIM_HandleTypeDef TimHandle;
    TIM_OC_InitTypeDef sConfig;
    uint32_t uhPrescalerValue = (uint32_t) ((SystemCoreClock / 2) / 21000000) - 1;

    // enable the peripheral clock: __TIM3_CLK_ENABLE()
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // Configure the timer
    TimHandle.Instance = TIM3;
    TimHandle.Init.Prescaler = uhPrescalerValue;
    TimHandle.Init.Period = 1000;
    TimHandle.Init.ClockDivision = 1;
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK) {
        Error_Handler();
    }

    // Configure PWM output on channel 1
    sConfig.OCMode = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;
    sConfig.Pulse = 500;
    if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }

    // Start PWM output on channel 1
    if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
}

void keepalive_stop(void)
{
}

//-----------------------------------------------------------------------------

void g540_init(void)
{
    keepalive_start();
}

//-----------------------------------------------------------------------------
