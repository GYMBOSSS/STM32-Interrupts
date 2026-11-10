#include "systick.h"
#include "stm32f1xx.h"

static volatile uint32_t tick_ms;

/* Not static: the name must match the vector table in the startup file. */
void SysTick_Handler(void)
{
    tick_ms++;
}

void systick_init(void)
{
    /* Sets LOAD, clears VAL, enables interrupt and counter with the core clock. */
    if (SysTick_Config(SystemCoreClock / 1000u) != 0u) {
        for (;;) { }   /* reload value does not fit into 24 bits */
    }
}

uint32_t millis(void)
{
    return tick_ms;
}

void delay_ms(uint32_t ms)
{
    uint32_t start = tick_ms;

    while ((uint32_t)(tick_ms - start) < ms) { }
}