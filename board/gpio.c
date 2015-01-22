//-----------------------------------------------------------------------------
/*

GPIO Control for the STM32F4 Discovery Board

*/
//-----------------------------------------------------------------------------

#include "stm32f4xx_hal.h"
#include "gpio.h"

//-----------------------------------------------------------------------------

typedef struct gpio_info {
    uint32_t num;       // gpio number - as defined in gpio.h
    uint32_t mode;      // input, output, etc.
    uint32_t pull;      // pull up/down, etc.
    uint32_t speed;     // slew rate
    uint32_t alt;       // alternate pin functions
    int init;           // initial pin value

} GPIO_INFO;

//-----------------------------------------------------------------------------
// gpio configuration info

static const GPIO_INFO gpio_info[] = {
    // leds
    {LED_RED, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FAST, 0, GPIO_PIN_RESET},
    {LED_BLUE, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FAST, 0, GPIO_PIN_RESET},
    {LED_GREEN, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FAST, 0, GPIO_PIN_RESET},
    {LED_AMBER, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FAST, 0, GPIO_PIN_RESET},
    // stepper step bits
    {STEP_X, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FAST, 0, GPIO_PIN_RESET},
    {STEP_Y, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FAST, 0, GPIO_PIN_RESET},
    {STEP_Z, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FAST, 0, GPIO_PIN_RESET},
    {STEP_A, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FAST, 0, GPIO_PIN_RESET},
    // stepper direction bits
    {DIRN_X, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FAST, 0, GPIO_PIN_RESET},
    {DIRN_Y, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FAST, 0, GPIO_PIN_RESET},
    {DIRN_Z, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FAST, 0, GPIO_PIN_RESET},
    {DIRN_A, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FAST, 0, GPIO_PIN_RESET},
    // push buttons
    {PUSH_BUTTON, GPIO_MODE_IT_FALLING, GPIO_NOPULL, 0, 0, -1},
    {SWITCH_E_STOP, GPIO_MODE_IT_FALLING, GPIO_NOPULL, 0, 0, -1},
    // g540 keepalive (tim4 function)
    {G540_KEEPALIVE, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_HIGH, GPIO_AF2_TIM4, -1},
    // serial port (usart2 function)
    {UART_TX, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_HIGH, GPIO_AF7_USART2, -1},
    {UART_RX, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_HIGH, GPIO_AF7_USART2, -1},
};

//-----------------------------------------------------------------------------

void gpio_init(void)
{
    int i;

    for (i = 0; i < sizeof(gpio_info)/sizeof(GPIO_INFO); i ++) {
        const GPIO_INFO *gpio = &gpio_info[i];
        GPIO_InitTypeDef GPIO_InitStruct;
        // enable the peripheral clock: __GPIOx_CLK_ENABLE()
        RCC->AHB1ENR |= (1 << GPIO_PORT(gpio->num));
        // setup the gpio port/pin
        GPIO_InitStruct.Pin = GPIO_BIT(gpio->num);
        GPIO_InitStruct.Mode = gpio->mode;
        GPIO_InitStruct.Pull = gpio->pull;
        GPIO_InitStruct.Speed = gpio->speed;
        GPIO_InitStruct.Alternate = gpio->alt;
        HAL_GPIO_Init(GPIO_BASE(gpio->num), &GPIO_InitStruct);
        // set any initial value
        if (gpio->init >= 0) {
            HAL_GPIO_WritePin(GPIO_BASE(gpio->num), GPIO_BIT(gpio->num), gpio->init);
        }
    }
}

//-----------------------------------------------------------------------------
