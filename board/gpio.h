//-----------------------------------------------------------------------------
/*

GPIO Control for the STM32F4 Discovery Board

*/
//-----------------------------------------------------------------------------

#ifndef GPIO_H
#define GPIO_H

//-----------------------------------------------------------------------------

#include "stm32f4xx_hal.h"

//-----------------------------------------------------------------------------
// port numbers

#define PORTA 0
#define PORTB 1
#define PORTC 2
#define PORTD 3
#define PORTE 4
#define PORTF 5
#define PORTG 6
#define PORTH 7
#define PORTI 8

//-----------------------------------------------------------------------------
// gpio macros

#define GPIO_NUM(port, pin) ((port << 4) | (pin))
#define GPIO_PORT(n) (n >> 4)
#define GPIO_PIN(n) (n & 0xf)
#define GPIO_BIT(n) (1 << GPIO_PIN(n))
#define GPIO_BASE(n) ((GPIO_TypeDef  *)(GPIOA_BASE + (GPIO_PORT(n) * 0x400)))

//-----------------------------------------------------------------------------
// gpio assignments

#define LED_GREEN   GPIO_NUM(PORTD, 12)
#define LED_AMBER   GPIO_NUM(PORTD, 13)
#define LED_RED     GPIO_NUM(PORTD, 14)
#define LED_BLUE    GPIO_NUM(PORTD, 15)

// all limit switches must be on the same port
#define LIMIT_X     GPIO_NUM(PORTA, 0)
#define LIMIT_Y     GPIO_NUM(PORTA, 1)
#define LIMIT_Z     GPIO_NUM(PORTA, 2)

// all step bits must be on the same port
#define STEP_X      GPIO_NUM(PORTB, 0)
#define STEP_Y      GPIO_NUM(PORTB, 1)
#define STEP_Z      GPIO_NUM(PORTB, 2)

// all direction bits must be on the same port
#define DIRN_X      GPIO_NUM(PORTC, 0)
#define DIRN_Y      GPIO_NUM(PORTC, 1)
#define DIRN_Z      GPIO_NUM(PORTC, 2)

// coolant control
#define COOLANT_FLOOD   GPIO_NUM(PORTD, 0)
#define COOLANT_MIST    GPIO_NUM(PORTD, 1)

// spindle control
#define SPINDLE_DIRN    GPIO_NUM(PORTD, 2)
#define SPINDLE_CTRL    GPIO_NUM(PORTD, 3)

//-----------------------------------------------------------------------------
// generic api functions

static inline void gpio_set(int n)
{
    GPIO_BASE(n)->BSRRH = GPIO_BIT(n);
}

static inline void gpio_clr(int n)
{
    GPIO_BASE(n)->BSRRL = GPIO_BIT(n);
}

static inline void gpio_toggle(int n)
{
    GPIO_BASE(n)->ODR ^= GPIO_BIT(n);
}

void gpio_init(void);

//-----------------------------------------------------------------------------
// grbl specific api functions

#define X_LIMIT_BIT GPIO_PIN(LIMIT_X)
#define Y_LIMIT_BIT GPIO_PIN(LIMIT_Y)
#define Z_LIMIT_BIT GPIO_PIN(LIMIT_Z)
#define LIMIT_MASK (GPIO_BIT(LIMIT_X) | GPIO_BIT(LIMIT_Y) | GPIO_BIT(LIMIT_Z))

static inline uint32_t limit_rd(void)
{
    uint32_t val = GPIO_BASE(LIMIT_X)->IDR;
    return val & LIMIT_MASK;
}

#define X_STEP_BIT GPIO_PIN(STEP_X)
#define Y_STEP_BIT GPIO_PIN(STEP_Y)
#define Z_STEP_BIT GPIO_PIN(STEP_Z)
#define STEP_MASK (GPIO_BIT(STEP_X) | GPIO_BIT(STEP_Y) | GPIO_BIT(STEP_Z))

static inline void step_wr(uint32_t x)
{
    uint32_t val = GPIO_BASE(STEP_X)->ODR;
    val &= ~STEP_MASK;
    GPIO_BASE(STEP_X)->ODR = (val | x);
}

#define X_DIRECTION_BIT GPIO_PIN(DIRN_X)
#define Y_DIRECTION_BIT GPIO_PIN(DIRN_Y)
#define Z_DIRECTION_BIT GPIO_PIN(DIRN_Z)
#define DIRECTION_MASK (GPIO_BIT(DIRN_X) | GPIO_BIT(DIRN_Y) | GPIO_BIT(DIRN_Z))

static inline void dirn_wr(uint32_t x)
{
    uint32_t val = GPIO_BASE(DIRN_X)->ODR;
    val &= ~DIRECTION_MASK;
    GPIO_BASE(DIRN_X)->ODR = (val | x);
}

static inline void coolant_flood_on(void) {gpio_set(COOLANT_FLOOD);}
static inline void coolant_flood_off(void) {gpio_clr(COOLANT_FLOOD);}
static inline void coolant_mist_on(void) {gpio_set(COOLANT_MIST);}
static inline void coolant_mist_off(void) {gpio_clr(COOLANT_MIST);}
static inline void spindle_on(void) {gpio_set(SPINDLE_CTRL);}
static inline void spindle_off(void) {gpio_clr(SPINDLE_CTRL);}
static inline void spindle_fwd(void) {gpio_set(SPINDLE_DIRN);}
static inline void spindle_rev(void) {gpio_clr(SPINDLE_DIRN);}

//-----------------------------------------------------------------------------

#endif // GPIO_H

//-----------------------------------------------------------------------------
