//-----------------------------------------------------------------------------
/*

GPIO Control for the STM32F4 Discovery Board

Pin Assignments

PA0 = push button
PA1
PA2
PA3
PA4 = codec
PA5 = accel
PA6 = accel
PA7 = accel
PA8
PA9 = usb
PA10 = usb
PA11 = usb (not on header)
PA12 = usb (not on header)
PA13 = swd
PA14 = swd
PA15

PB0
PB1
PB2
PB3 = swd
PB4
PB5
PB6 = codec
PB7
PB8
PB9 = codec
PB10 = mic
PB11
PB12
PB13
PB14
PB15

PC0 = usb
PC1
PC2
PC3 = mic
PC4 = codec
PC5
PC6
PC7 = codec
PC8
PC9
PC10 = codec
PC11
PC12 = codec
PC13
PC14 = osc_in
PC15 = osc_out

PD0 = coolant_flood
PD1 = coolant_mist
PD2 = spindle_dirn
PD3 = spindle_ctrl
PD4 = codec
PD5 = usb
PD6 = limit_x
PD7 = limit_y
PD8 = limit_z
PD9 = limit_a
PD10 = limit_b
PD11 = limit_c
PD12 = led
PD13 = led
PD14 = led
PD15 = led

PE0 = accel
PE1 = accel
PE2
PE3 = accel
PE4 = step_x
PE5 = step_y
PE6 = step_z
PE7 = step_a
PE8 = step_b
PE9 = step_c
PE10 = dirn_x
PE11 = dirn_y
PE12 = dirn_z
PE13 = dirn_a
PE14 = dirn_b
PE15 = dirn_c

PH0 = ph0_osc_in
PH1 = ph1_osc_out

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

#define LED_GREEN       GPIO_NUM(PORTD, 12)
#define LED_AMBER       GPIO_NUM(PORTD, 13)
#define LED_RED         GPIO_NUM(PORTD, 14)
#define LED_BLUE        GPIO_NUM(PORTD, 15)
#define PUSH_BUTTON     GPIO_NUM(PORTA, 0)

#define LIMIT_X         GPIO_NUM(PORTD, 6)
#define LIMIT_Y         GPIO_NUM(PORTD, 7)
#define LIMIT_Z         GPIO_NUM(PORTD, 8)

// all step bits must be on the same port
#define STEP_X          GPIO_NUM(PORTE, 4)
#define STEP_Y          GPIO_NUM(PORTE, 5)
#define STEP_Z          GPIO_NUM(PORTE, 6)

// all direction bits must be on the same port
#define DIRN_X          GPIO_NUM(PORTE, 10)
#define DIRN_Y          GPIO_NUM(PORTE, 11)
#define DIRN_Z          GPIO_NUM(PORTE, 12)

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

static inline int gpio_rd(int n)
{
    return (GPIO_BASE(n)->IDR >> GPIO_PIN(n)) & 1;
}

void gpio_init(void);

//-----------------------------------------------------------------------------
// grbl specific api functions

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

#define X_LIMIT_BIT 3
#define Y_LIMIT_BIT 2
#define Z_LIMIT_BIT 1
#define PUSH_BUTTON_BIT 0

#define LIMIT_MASK ((1 << X_LIMIT_BIT) | (1 << Y_LIMIT_BIT) | (1 << Z_LIMIT_BIT))

static inline uint32_t debounce_input(void)
{
    // pack the gpio inputs to be debounced into the uint32_t debounce state
    return ((gpio_rd(LIMIT_X) << X_LIMIT_BIT) |
            (gpio_rd(LIMIT_Y) << Y_LIMIT_BIT) |
            (gpio_rd(LIMIT_Z) << Z_LIMIT_BIT) |
            (gpio_rd(PUSH_BUTTON) << PUSH_BUTTON_BIT));
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
