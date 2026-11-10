#ifndef CLOCK_H
#define CLOCK_H

typedef enum {
    CLOCK_OK = 0,
    CLOCK_ERR_HSE,
    CLOCK_ERR_PLL,
    CLOCK_ERR_SWITCH
} clock_status_t;

/* Configures SYSCLK = 72 MHz from HSE (8 MHz) via PLL x9. */
clock_status_t clock_init(void);

#endif /* CLOCK_H */