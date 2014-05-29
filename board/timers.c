//-----------------------------------------------------------------------------
/*

Timer Functions

The main stepper routines use timers and ISRs for pulse generation.
This file has the board specific routines to allow this.

*/
//-----------------------------------------------------------------------------

#include "stm32f4xx_hal.h"
#include "timers.h"

void Error_Handler(void);

//-----------------------------------------------------------------------------
// On the G540 a charge pump signal (>= 10KHz square wave) is used as a keepalive.
// When the e-stop is pressed we will stop it.

static TIM_HandleTypeDef charge_pump_timer;

#define CHARGE_PUMP_HZ 12000
#define PERIOD_CNT 200

#define TIMER_HZ (PERIOD_CNT * CHARGE_PUMP_HZ)
#define TIM_ARR (PERIOD_CNT - 1)
#define TIM_CCR (PERIOD_CNT / 2)

static void charge_pump_start(TIM_HandleTypeDef *tim)
{
    TIM_OC_InitTypeDef tim_cfg;

    // enable the peripheral clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    // Configure the timer
    tim->Instance = TIM4;
    tim->Init.Prescaler = ((SystemCoreClock / 2) / TIMER_HZ) - 1;
    tim->Init.Period = TIM_ARR;
    tim->Init.ClockDivision = 0;
    tim->Init.CounterMode = TIM_COUNTERMODE_UP;
    if (HAL_TIM_PWM_Init(tim) != HAL_OK) {
        Error_Handler();
    }

    // Configure PWM output
    tim_cfg.OCMode = TIM_OCMODE_PWM1;
    tim_cfg.OCIdleState = TIM_OUTPUTSTATE_ENABLE;
    tim_cfg.Pulse = TIM_CCR;
    tim_cfg.OCPolarity = TIM_OCPOLARITY_HIGH;
    if (HAL_TIM_PWM_ConfigChannel(tim, &tim_cfg, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }

    // Start PWM output
    if (HAL_TIM_PWM_Start(tim, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }
}

void charge_pump_stop(void)
{
    HAL_TIM_PWM_Stop(&charge_pump_timer, TIM_CHANNEL_2);
}

//-----------------------------------------------------------------------------

void stepper_isr_enable(void){}
void stepper_isr_disable(void){}
void set_step_period(uint32_t ticks) {}
void set_step_pulse_delay(uint32_t ticks) {}
void set_step_pulse_time(uint32_t ticks) {}

//-----------------------------------------------------------------------------

void timers_init(void)
{
    charge_pump_start(&charge_pump_timer);
}

//-----------------------------------------------------------------------------
