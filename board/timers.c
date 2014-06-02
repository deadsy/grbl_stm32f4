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

#if 0
TIM4->CR1    [0x40000800] = 0x00000001
TIM4->CR2    [0x40000804] = 0x00000000
TIM4->SMCR   [0x40000808] = 0x00000000
TIM4->DIER   [0x4000080c] = 0x00000000
TIM4->SR     [0x40000810] = 0x0000001f
TIM4->EGR    [0x40000814] = 0x00000000
TIM4->CCMR1  [0x40000818] = 0x00006800
TIM4->CCMR2  [0x4000081c] = 0x00000000
TIM4->CCER   [0x40000820] = 0x00000010
TIM4->CNT    [0x40000824] = 0x0000004d
TIM4->PSC    [0x40000828] = 0x00000022
TIM4->ARR    [0x4000082c] = 0x000000c7
TIM4->RCR    [0x40000830] = 0x00000000
TIM4->CCR1   [0x40000834] = 0x00000000
TIM4->CCR2   [0x40000838] = 0x00000064
TIM4->CCR3   [0x4000083c] = 0x00000000
TIM4->CCR4   [0x40000840] = 0x00000000
TIM4->BDTR   [0x40000844] = 0x00000000
TIM4->DCR    [0x40000848] = 0x00000000
TIM4->DMAR   [0x4000084c] = 0x00000001
TIM4->OR     [0x40000850] = 0x00000000
#endif

#if 0

#define CHARGE_PUMP_HZ 12000
#define PERIOD_COUNT 200

static void charge_pump_start(void)
{
    TIM_TypeDef* const TIMx = TIM4;

    // enable the peripheral clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    TIMx->CR1 = TIM_CR1_ARPE;
    TIMx->CR2 = 0; // todo

    // slave mode control register (not used)
    TIMx->SMCR = 0;
    // interrupts (disabled)
    TIMx->DIER = 0;
    TIMx->SR = 0;

    TIMx->CCMR1 = 0;
    TIMx->CCMR2 = 0;
    TIMx->CCER = 0;
    // setup the counter, reload value and prescalar
    TIMx->CNT = 0;
    TIMx->PSC = ((SystemCoreClock / 2) /(PERIOD_COUNT * CHARGE_PUMP_HZ)) - 1;
    TIMx->ARR = PERIOD_COUNT - 1;
    // setup the output compare values (channel2 only)
    TIMx->CCR1 = 0;
    TIMx->CCR2 = PERIOD_COUNT / 2;
    TIMx->CCR3 = 0;
    TIMx->CCR4 = 0;
    // dma control (not used)
    TIMx->DCR = 0;
    TIMx->DMAR = 0;

    // update the registers
    TIMx->EGR = TIM_EGR_UG;

    // turn on the channel2 output compare
    TIMx->CCER |= TIM_CCER_CC2E;
    // turn on the timer
    TIMx->CR1 |= TIM_CR1_CEN;
}

void charge_pump_stop(void)
{
    TIM_TypeDef* const TIMx = TIM4;
    // turn off the channel2 output compare
    TIMx->CCER &= ~TIM_CCER_CC2E;
    // turn off the timer
    TIMx->CR1 &= ~TIM_CR1_CEN;
}

#endif

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
