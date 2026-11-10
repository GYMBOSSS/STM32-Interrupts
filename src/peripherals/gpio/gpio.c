#include "gpio.h"

void gpio_clock_enable(GPIO_TypeDef *port)
{
    uint32_t bit = 0u;

    if      (port == GPIOA) bit = RCC_APB2ENR_IOPAEN;
    else if (port == GPIOB) bit = RCC_APB2ENR_IOPBEN;
    else if (port == GPIOC) bit = RCC_APB2ENR_IOPCEN;
    else if (port == GPIOD) bit = RCC_APB2ENR_IOPDEN;
    else                    return;

    SET_BIT(RCC->APB2ENR, bit);
    (void)READ_BIT(RCC->APB2ENR, bit);
}

void gpio_configure(GPIO_TypeDef *port, uint8_t pin, gpio_cfg_t cfg)
{
    volatile uint32_t *cr = (pin < 8u) ? &port->CRL : &port->CRH;
    uint32_t shift = (uint32_t)(pin & 7u) * 4u;

    MODIFY_REG(*cr, 0xFu << shift, (uint32_t)cfg << shift);
}

void gpio_write(GPIO_TypeDef *port, uint8_t pin, bool level)
{
    port->BSRR = level ? (1u << pin) : (1u << (pin + 16u));
}

void gpio_toggle(GPIO_TypeDef *port, uint8_t pin)
{
    uint32_t mask = 1u << pin;
    port->BSRR = (port->ODR & mask) ? (mask << 16u) : mask;
}

bool gpio_read(const GPIO_TypeDef *port, uint8_t pin)
{
    return ((port->IDR >> pin) & 1u) != 0u;
}