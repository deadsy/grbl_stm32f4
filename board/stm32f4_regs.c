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
// exceptions status

static const char *exception_name[] = {
    // system exceptions
    "-",
    "Reset",
    "NonMaskableInt",
    "HardFault",
    "MemoryManagement",
    "BusFault",
    "UsageFault",
    "-",
    "-",
    "-",
    "-",
    "SVCall",
    "DebugMonitor",
    "-",
    "PendSV",
    "SysTick",
    // stm32f407 interrupts
    "WWDG",
    "PVD",
    "TAMP_STAMP",
    "RTC_WKUP",
    "FLASH",
    "RCC",
    "EXTI0",
    "EXTI1",
    "EXTI2",
    "EXTI3",
    "EXTI4",
    "DMA1_Stream0",
    "DMA1_Stream1",
    "DMA1_Stream2",
    "DMA1_Stream3",
    "DMA1_Stream4",
    "DMA1_Stream5",
    "DMA1_Stream6",
    "ADC",
    "CAN1_TX",
    "CAN1_RX0",
    "CAN1_RX1",
    "CAN1_SCE",
    "EXTI9_5",
    "TIM1_BRK_TIM9",
    "TIM1_UP_TIM10",
    "TIM1_TRG_COM_TIM11",
    "TIM1_CC",
    "TIM2",
    "TIM3",
    "TIM4",
    "I2C1_EV",
    "I2C1_ER",
    "I2C2_EV",
    "I2C2_ER",
    "SPI1",
    "SPI2",
    "USART1",
    "USART2",
    "USART3",
    "EXTI15_10",
    "RTC_Alarm",
    "OTG_FS_WKUP",
    "TIM8_BRK_TIM12",
    "TIM8_UP_TIM13",
    "TIM8_TRG_COM_TIM14",
    "TIM8_CC",
    "DMA1_Stream7",
    "FSMC",
    "SDIO",
    "TIM5",
    "SPI3",
    "UART4",
    "UART5",
    "TIM6_DAC",
    "TIM7",
    "DMA2_Stream0",
    "DMA2_Stream1",
    "DMA2_Stream2",
    "DMA2_Stream3",
    "DMA2_Stream4",
    "ETH",
    "ETH_WKUP",
    "CAN2_TX",
    "CAN2_RX0",
    "CAN2_RX1",
    "CAN2_SCE",
    "OTG_FS",
    "DMA2_Stream5",
    "DMA2_Stream6",
    "DMA2_Stream7",
    "USART6",
    "I2C3_EV",
    "I2C3_ER",
    "OTG_HS_EP1_OUT",
    "OTG_HS_EP1_IN",
    "OTG_HS_WKUP",
    "OTG_HS",
    "DCMI",
    "HASH_RNG",
    "FPU",
};

#define NUM_EXCEPTIONS (sizeof(exception_name) / sizeof(char *))
#define NUM_SYS_EXC 16

// __NVIC_PRIO_BITS = 4 for this processor
static const char *priority_str[] = {
    "pppp....", // 0
    "pppp....", // 1
    "pppp....", // 2
    "pppp....", // 3
    "ppps....", // 4
    "ppss....", // 5
    "psss....", // 6
    "ssss....", // 7
};

#define NUM_GROUPS (sizeof(priority_str) / sizeof(char *))

static void print_bit(int x, char c)
{
    if (x == 0) {
        printf(".");
    } else if (x == 1) {
        printf("%c", c);
    } else {
        printf(" ");
    }
}

void display_exceptions(void)
{
    uint32_t group = NVIC_GetPriorityGrouping();
    uint32_t *vector = (uint32_t *)SCB->VTOR;
    int i;

    printf("%-19s: %ld %s\r\n", "priority grouping", group, priority_str[group % NUM_GROUPS]);
    printf("%-19s: %08lx\r\n", "vector table", (uint32_t)vector);
    printf("Name                 Exc Irq EPA Prio Vector\r\n");
    for (i = 0; i < NUM_EXCEPTIONS; i ++) {
        int irq = i - NUM_SYS_EXC;
        int priority, enabled, pending, active;
        uint32_t pre, sub;
        char tmp[16];

        // skip reserved exceptions
        if (*exception_name[i] == '-') {
            continue;
        }
        // name
        printf("%-19s: ", exception_name[i]);
        // exception number
        printf("%-3d ", i);
        // irq number
        if (irq >= 0) {
            printf("%-3d ", i - NUM_SYS_EXC);
        } else {
            printf("-   ");
        }
        // enabled/pending/active
        enabled = pending = active = -1;
        if (irq >= 0) {
            int idx = (irq >> 5) & 7;
            int shift = irq & 31;
            enabled = (NVIC->ISER[idx] >> shift) & 1;
            pending = (NVIC->ISPR[idx] >> shift) & 1;
            active = (NVIC->IABR[idx] >> shift) & 1;
        } else {
            switch (irq) {
                case NonMaskableInt_IRQn: {
                    enabled = 1;
                    pending = (SCB->ICSR >> 31) & 1;
                    break;
                }
                case MemoryManagement_IRQn: {
                    enabled = (SCB->SHCSR >> 16) & 1;
                    pending = (SCB->SHCSR >> 13) & 1;
                    active = (SCB->SHCSR >> 0) & 1;
                    break;
                }
                case BusFault_IRQn: {
                    enabled = (SCB->SHCSR >> 17) & 1;
                    pending = (SCB->SHCSR >> 14) & 1;
                    active = (SCB->SHCSR >> 1) & 1;
                    break;
                }
                case UsageFault_IRQn: {
                    enabled = (SCB->SHCSR >> 18) & 1;
                    pending = (SCB->SHCSR >> 12) & 1;
                    active = (SCB->SHCSR >> 3) & 1;
                    break;
                }
                case SVCall_IRQn: {
                    pending = (SCB->SHCSR >> 15) & 1;
                    active = (SCB->SHCSR >> 7) & 1;
                    break;
                }
                case DebugMonitor_IRQn: {
                    active = (SCB->SHCSR >> 8) & 1;
                    break;
                }
                case PendSV_IRQn: {
                    pending = (SCB->ICSR >> 28) & 1;
                    break;
                }
                case SysTick_IRQn: {
                    enabled = (SysTick->CTRL >> 1) & 1;
                    pending = (SCB->ICSR >> 26) & 1;
                    break;
                }
            }
        }
        print_bit(enabled, 'e');
        print_bit(pending, 'p');
        print_bit(active, 'a');
        printf(" ");
        // priority
        switch (irq) {
            case -15 /*reset*/: priority = -3; break;
            case -14 /*nmi*/: priority = -2; break;
            case -13 /*hardfault*/: priority = -1; break;
            default: priority = NVIC_GetPriority(irq); break;
        }
        if (priority < 0) {
            sprintf(tmp, "%-4d", priority);
        } else {
            NVIC_DecodePriority(priority, group, &pre, &sub);
            sprintf(tmp, "%ld.%ld", pre, sub);
        }
        printf("%-4s ", tmp);
        // vector
        printf("%08lx ", vector[i] & ~1);
        printf("\r\n");
    }
}

//-----------------------------------------------------------------------------
// TIM Registers

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
// USART Registers

static USART_TypeDef *usart_base[] = {
    USART1, USART2, USART3, UART4, UART5, USART6
};

void display_usart(int num)
{
    USART_TypeDef *usart;
    char base_name[16];

    if (num < 1 || num > 6) {
        return;
    }
    usart = usart_base[num - 1];

    if (num == 4 || num == 5) {
        sprintf(base_name, "UART%d", num);
    } else {
        sprintf(base_name, "USART%d", num);
    }

    dump_reg(base_name, "SR", &usart->SR);
    dump_reg(base_name, "DR", &usart->DR);
    dump_reg(base_name, "BRR", &usart->BRR);
    dump_reg(base_name, "CR1", &usart->CR1);
    dump_reg(base_name, "CR2", &usart->CR2);
    dump_reg(base_name, "CR3", &usart->CR3);
    dump_reg(base_name, "GTPR", &usart->GTPR);
}

//-----------------------------------------------------------------------------
