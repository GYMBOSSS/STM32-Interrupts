#include "led.h"
#include "gpio.h"

#define LED_PORT GPIOA
#define LED_PIN  5u

void led_init(void)
{
    gpio_clock_enable(LED_PORT);
    gpio_write(LED_PORT, LED_PIN, false);     /* no glitch on start */
    gpio_configure(LED_PORT, LED_PIN, GPIO_CFG_OUTPUT_PP_2MHZ);
}

void led_on(void)     { gpio_write(LED_PORT, LED_PIN, true); }
void led_off(void)    { gpio_write(LED_PORT, LED_PIN, false); }
void led_toggle(void) { gpio_toggle(LED_PORT, LED_PIN); }