//-----------------------------------------------------------------------------
/*

Dump stm32f4 peripheral register contents

*/
//-----------------------------------------------------------------------------

#include <stdio.h>

#include "stm32f4xx_hal.h"
#include "stm32f4_regs.h"

//-----------------------------------------------------------------------------

static void dump_reg(const char *base_name, const char *reg_name, const volatile uint32_t *ptr)
{
    char tmp[32];
    sprintf(tmp, "%s->%s", base_name, reg_name);
    printf("%-12s [%p] = 0x%08x\r\n", tmp, ptr, (unsigned int)*ptr);
}

//-----------------------------------------------------------------------------

static TIM_TypeDef *tim_base[] = {
    TIM1, TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM8, TIM9, TIM10, TIM11, TIM12, TIM13, TIM14
};

void display_tim(int num)
{
    TIM_TypeDef *tim;
    char base_name[16];

    if (num < 1 || num > 14) {
        return;
    }
    tim = tim_base[num - 1];

    sprintf(base_name, "TIM%d", num);

    dump_reg(base_name, "CR1", &tim->CR1);
    dump_reg(base_name, "CR2", &tim->CR2);
    dump_reg(base_name, "SMCR", &tim->SMCR);
    dump_reg(base_name, "DIER", &tim->DIER);
    dump_reg(base_name, "SR", &tim->SR);
    dump_reg(base_name, "EGR", &tim->EGR);
    dump_reg(base_name, "CCMR1", &tim->CCMR1);
    dump_reg(base_name, "CCMR2", &tim->CCMR2);
    dump_reg(base_name, "CCER", &tim->CCER);
    dump_reg(base_name, "CNT", &tim->CNT);
    dump_reg(base_name, "PSC", &tim->PSC);
    dump_reg(base_name, "ARR", &tim->ARR);
    dump_reg(base_name, "RCR", &tim->RCR);
    dump_reg(base_name, "CCR1", &tim->CCR1);
    dump_reg(base_name, "CCR2", &tim->CCR2);
    dump_reg(base_name, "CCR3", &tim->CCR3);
    dump_reg(base_name, "CCR4", &tim->CCR4);
    dump_reg(base_name, "BDTR", &tim->BDTR);
    dump_reg(base_name, "DCR", &tim->DCR);
    dump_reg(base_name, "DMAR", &tim->DMAR);
    dump_reg(base_name, "OR", &tim->OR);
}

//-----------------------------------------------------------------------------
