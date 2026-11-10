#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

/* Starts a 1 ms system tick. Call after clock_init(). */
void systick_init(void);

/* Milliseconds since systick_init(). Wraps after ~49.7 days;
 * always compare with subtraction: (millis() - start) >= period. */
uint32_t millis(void);

/* Blocking delay, safe against counter wrap-around. */
void delay_ms(uint32_t ms);

#endif /* SYSTICK_H */