//-----------------------------------------------------------------------------
/*

GPIO Control for the STM32F4 Discovery Board

*/
//-----------------------------------------------------------------------------

#ifndef GPIO_H
#define GPIO_H

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

//-----------------------------------------------------------------------------
// api functions

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

#endif // GPIO_H

//-----------------------------------------------------------------------------
