//-----------------------------------------------------------------------------
/*

IRQ Control for the STM32F4 Discovery Board

*/
//-----------------------------------------------------------------------------

#ifndef IRQ_H
#define IRQ_H

//-----------------------------------------------------------------------------

static inline uint32_t disable_irq(void)
{
  uint32_t x;
  asm volatile ("mrs %0, primask" : "=r" (x) );
  asm volatile ("cpsid i" : : : "memory");  
  return x;
}

static inline void restore_irq(uint32_t x)
{
  asm volatile ("msr primask, %0" : : "r" (x) : "memory"); 
}

//-----------------------------------------------------------------------------

#endif // IRQ_H

//-----------------------------------------------------------------------------
