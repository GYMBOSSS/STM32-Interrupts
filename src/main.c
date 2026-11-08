#include <stdio.h>
#include <stdint.h>

#define RCC_BASE        0x40021000UL
#define GPIOA_BASE      0x40010800UL

#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_BSRR      (*(volatile uint32_t *)(GPIOA_BASE + 0x10))

#define RCC_APB2ENR_IOPAEN (1 << 2)

void delay(volatile int count)
{
    while (count--)
    {
        __asm__("nop");
    }
}

int main(void)
{
    RCC_APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA_CRL &= ~(0xF << 20);
    GPIOA_CRL |= (0x2 << 20);

    while(1)
    {
        GPIOA_BSRR = (1 << 5);
        delay(1000000);
        GPIOA_BSRR = (1 << (5 + 16));
        delay(1000000);
    }
}
