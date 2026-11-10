#include "clock.h"
#include "stm32f1xx.h"
#include <stdint.h>

/* Iteration limit for polling loops, not a time unit. */
#define CLOCK_TIMEOUT_LOOPS 0x5000u

static int wait_for(volatile uint32_t *reg, uint32_t mask, uint32_t value) {
  uint32_t timeout = CLOCK_TIMEOUT_LOOPS;

  while ((*reg & mask) != value) {
    if (timeout-- == 0u) {
      return 0;
    }
  }
  return 1;
}

clock_status_t clock_init(void) {
  SET_BIT(RCC->CR, RCC_CR_HSEON);
  if (!wait_for(&RCC->CR, RCC_CR_HSERDY, RCC_CR_HSERDY)) {
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON);
    return CLOCK_ERR_HSE;
  }

  /* 72 MHz requires 2 flash wait states; enable prefetch buffer. */
  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2);
  SET_BIT(FLASH->ACR, FLASH_ACR_PRFTBE);

  /* AHB = 72 MHz, APB1 = 36 MHz (max), APB2 = 72 MHz. */
  MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV2);
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV1);

  /* PLL source = HSE (not divided), multiplier x9: 8 MHz * 9 = 72 MHz. */
  SET_BIT(RCC->CFGR, RCC_CFGR_PLLSRC);
  CLEAR_BIT(RCC->CFGR, RCC_CFGR_PLLXTPRE);
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLMULL, RCC_CFGR_PLLMULL9);

  SET_BIT(RCC->CR, RCC_CR_PLLON);
  if (!wait_for(&RCC->CR, RCC_CR_PLLRDY, RCC_CR_PLLRDY)) {
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON);
    return CLOCK_ERR_PLL;
  }

  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
  if (!wait_for(&RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL)) {
    return CLOCK_ERR_SWITCH;
  }

  SystemCoreClockUpdate();
  return CLOCK_OK;
}