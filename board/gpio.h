//-----------------------------------------------------------------------------
/*

GPIO Control for the STM32F4 Discovery Board

Pin Assignments for Gecko G540 DB25 Port:

pin1 = OUTPUT 2, pin 6 on terminal block (output from PC)
pin2 = pe4 = X-AXIS STEP (output from PC)
pin3 = pe5 = X-AXIS DIRECTION (output from PC)
pin4 = pe6 = Y-AXIS STEP (output from PC)
pin5 = pe7 = Y-AXIS DIRECTION (output from PC)
pin6 = pe8 = Z-AXIS STEP (output from PC)
pin7 = pe9 = Z-AXIS DIRECTION (output from PC)
pin8 = pe10 = A-AXIS STEP (output from PC)
pin9 = pe11 = A-AXIS DIRECTION (output from PC)
pin10 = pd6 = INPUT 1, pin 1 on terminal block (input to PC)
pin11 = pd7 = INPUT 2, pin 2 on terminal block (input to PC)
pin12 = pd8 = INPUT 3, pin 3 on terminal block (input to PC)
pin13 = pd9 = INPUT 4, pin 4 on terminal block (input to PC)
pin14 = VFD PWM (50 Hz), pin 8 on terminal block (output from PC)
pin15 = pa15 = FAULT, pin 10 on terminal block (input to PC)
pin16 = pb7 = CHARGE PUMP (>10 kHz)
pin17 = OUTPUT 1, pin 5 on terminal block (output from PC)
pin18 = GND
pin19 = GND
pin20 = GND
pin21 = GND
pin22 = GND
pin23 = GND
pin24 = GND
pin25 = GND

Pin Assignments for STM32F4 Discovery Board
* = CNC function

PA0 = push button
PA1 = system_reset
PA2 =
PA3 =
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
PA15 = e-stop *

PB0
PB1
PB2
PB3 = swd
PB4
PB5
PB6 = codec
PB7 = g540 keepalive (charge pump) *
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

PD0 =
PD1 =
PD2 =
PD3 =
PD4 = codec
PD5 = usb
PD6 = limit_x *
PD7 = limit_y *
PD8 = limit_z *
PD9 = limit_a *
PD10 =
PD11 =
PD12 = led
PD13 = led
PD14 = led
PD15 = led

PE0 = accel
PE1 = accel
PE2
PE3 = accel
PE4 = step_x *
PE5 = dirn_x *
PE6 = step_y *
PE7 = dirn_y *
PE8 = step_z *
PE9 = dirn_z *
PE10 = step_a *
PE11 = dirn_a *
PE12 =
PE13 =
PE14 =
PE15 =

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

// standard board GPIO
#define LED_GREEN       GPIO_NUM(PORTD, 12)
#define LED_AMBER       GPIO_NUM(PORTD, 13)
#define LED_RED         GPIO_NUM(PORTD, 14)
#define LED_BLUE        GPIO_NUM(PORTD, 15)
#define PUSH_BUTTON     GPIO_NUM(PORTA, 0) // 0 = open, 1 = pressed

// machine switches
#define SWITCH_E_STOP   GPIO_NUM(PORTA, 15)

// all step bits must be on the same port
#define STEP_X          GPIO_NUM(PORTE, 4)
#define STEP_Y          GPIO_NUM(PORTE, 6)
#define STEP_Z          GPIO_NUM(PORTE, 8)
#define STEP_A          GPIO_NUM(PORTE, 10)

// all direction bits must be on the same port
#define DIRN_X          GPIO_NUM(PORTE, 5)
#define DIRN_Y          GPIO_NUM(PORTE, 7)
#define DIRN_Z          GPIO_NUM(PORTE, 9)
#define DIRN_A          GPIO_NUM(PORTE, 11)

// limit switches
#define LIMIT_X         GPIO_NUM(PORTD, 6)
#define LIMIT_Y         GPIO_NUM(PORTD, 7)
#define LIMIT_Z         GPIO_NUM(PORTD, 8)
#define LIMIT_A         GPIO_NUM(PORTD, 9)

// serial port
#define UART_TX         GPIO_NUM(PORTA, 2)
#define UART_RX         GPIO_NUM(PORTA, 3)

// misc
#define G540_KEEPALIVE  GPIO_NUM(PORTB, 7) // tim4, ch2, af2

//-----------------------------------------------------------------------------
// generic api functions

static inline void gpio_clr(int n)
{
    GPIO_BASE(n)->BSRR = 1 << (GPIO_PIN(n) + 16);
}

static inline void gpio_set(int n)
{
    GPIO_BASE(n)->BSRR = 1 << GPIO_PIN(n);
}

static inline void gpio_toggle(int n)
{
    GPIO_BASE(n)->ODR ^= GPIO_BIT(n);
}

static inline int gpio_rd(int n)
{
    return (GPIO_BASE(n)->IDR >> GPIO_PIN(n)) & 1;
}

static inline int gpio_rd_inv(int n)
{
    return (~(GPIO_BASE(n)->IDR) >> GPIO_PIN(n)) & 1;
}

void gpio_init(void);

//-----------------------------------------------------------------------------
// grbl specific api functions

#define X_STEP_BIT GPIO_PIN(STEP_X)
#define Y_STEP_BIT GPIO_PIN(STEP_Y)
#define Z_STEP_BIT GPIO_PIN(STEP_Z)
#define A_STEP_BIT GPIO_PIN(STEP_A)
#define STEP_MASK (GPIO_BIT(STEP_X) | GPIO_BIT(STEP_Y) | GPIO_BIT(STEP_Z) | GPIO_BIT(STEP_A))

static inline void step_wr(uint32_t x)
{
    uint32_t val = GPIO_BASE(STEP_X)->ODR;
    val &= ~STEP_MASK;
    GPIO_BASE(STEP_X)->ODR = (val | x);
}

#define X_DIRECTION_BIT GPIO_PIN(DIRN_X)
#define Y_DIRECTION_BIT GPIO_PIN(DIRN_Y)
#define Z_DIRECTION_BIT GPIO_PIN(DIRN_Z)
#define A_DIRECTION_BIT GPIO_PIN(DIRN_A)
#define DIRECTION_MASK (GPIO_BIT(DIRN_X) | GPIO_BIT(DIRN_Y) | GPIO_BIT(DIRN_Z) | GPIO_BIT(DIRN_A))

static inline void dirn_wr(uint32_t x)
{
    uint32_t val = GPIO_BASE(DIRN_X)->ODR;
    val &= ~DIRECTION_MASK;
    GPIO_BASE(DIRN_X)->ODR = (val | x);
}

//-----------------------------------------------------------------------------
// The input gpios are spread out across several ports. We read and pack them into a
// single uint32_t and debounce them together.

// bit assignment for the debounced state - not all of these are used
#define RESET_BIT 0
#define FEED_HOLD_BIT 1
#define CYCLE_START_BIT 2
#define SAFETY_DOOR_BIT 3
#define PROBE_BIT 4
#define X_LIMIT_BIT 5
#define Y_LIMIT_BIT 6
#define Z_LIMIT_BIT 7
#define A_LIMIT_BIT 8
#define PUSH_BUTTON_BIT 9

extern int controls_enabled;
extern int limits_enabled;

#define LIMIT_MASK ((1 << X_LIMIT_BIT) | (1 << Y_LIMIT_BIT) | (1 << Z_LIMIT_BIT) | (1 << A_LIMIT_BIT))
#define CONTROL_MASK ((1 << RESET_BIT) | (1 << FEED_HOLD_BIT) | (1 << CYCLE_START_BIT) | (1 << SAFETY_DOOR_BIT))
#define PROBE_MASK (1 << PROBE_BIT)

static inline uint32_t debounce_input(void)
{
    // map the gpio inputs to be debounced into the uint32_t debounce state
    return ((gpio_rd(LIMIT_X) << X_LIMIT_BIT) |
            (gpio_rd(LIMIT_Y) << Y_LIMIT_BIT) |
            (gpio_rd(LIMIT_Z) << Z_LIMIT_BIT) |
            (gpio_rd(LIMIT_A) << A_LIMIT_BIT) |
            (gpio_rd(SWITCH_E_STOP) << RESET_BIT) |
            (gpio_rd(PUSH_BUTTON) << PUSH_BUTTON_BIT));
}

//-----------------------------------------------------------------------------
// coolant and spindle controls - no-ops for now

static inline void coolant_flood_on(void) {}
static inline void coolant_flood_off(void) {}
static inline void coolant_mist_on(void) {}
static inline void coolant_mist_off(void) {}
static inline void spindle_on(void) {}
static inline void spindle_off(void) {}
static inline void spindle_fwd(void) {}
static inline void spindle_rev(void) {}
static inline void spindle_rpm(float rpm) {}

//-----------------------------------------------------------------------------
// stepper motor enable/disable
// Normally these would be used to control enable/disable gpio lines to the
// stepper motor driver board. For the G540 they are no-ops.

static inline void stepper_motor_disable(void) {}
static inline void stepper_motor_enable(void) {}

//-----------------------------------------------------------------------------

#endif // GPIO_H

//-----------------------------------------------------------------------------
