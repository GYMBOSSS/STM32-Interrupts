#ifndef GPIO_H
#define GPIO_H

#include "stm32f1xx.h"
#include <stdbool.h>
#include <stdint.h>

/* Values are the 4-bit CNF:MODE field of CRL/CRH. */
typedef enum {
  GPIO_CFG_INPUT_FLOATING = 0x4u,
  GPIO_CFG_INPUT_PULL = 0x8u, /* pull-up/down chosen via ODR */
  GPIO_CFG_OUTPUT_PP_2MHZ = 0x2u,
  GPIO_CFG_AF_PP_50MHZ = 0xBu
} gpio_cfg_t;

void gpio_clock_enable(GPIO_TypeDef *port);
void gpio_configure(GPIO_TypeDef *port, uint8_t pin, gpio_cfg_t cfg);
void gpio_write(GPIO_TypeDef *port, uint8_t pin, bool level);
void gpio_toggle(GPIO_TypeDef *port, uint8_t pin);
bool gpio_read(const GPIO_TypeDef *port, uint8_t pin);

#endif /* GPIO_H */