#include "clock.h"
#include "gpio.h"
#include "led.h"
#include "systick.h"
#include "usart.h"
#include <stdint.h>
#include <stdio.h>

#define REPORT_PERIOD_MS 4000u
#define BAUDRATE 115200u

static void report_clock(void) {
  char buf[48];

  snprintf(buf, sizeof buf, "Core Clock: %lu Hz\r\n",
           (unsigned long)SystemCoreClock);
  usart_write_str(buf);
}

static void process_command(uint8_t cmd) {
  switch (cmd) {
  case '1':
    led_on();
    break;
  case '0':
    led_off();
    break;
  case 't':
    led_toggle();
    break;
  default:
    break;
  }
  usart_write_byte(cmd); /* echo */
}

int main(void) {
  if (clock_init() != CLOCK_OK) {
    for (;;) {
    } /* TODO: signal the error with the LED */
  }
  systick_init();
  led_init();
  usart_init(BAUDRATE);

  uint32_t last_report = 0u;
  uint8_t byte;

  for (;;) {
    if ((uint32_t)(millis() - last_report) >= REPORT_PERIOD_MS) {
      last_report = millis();
      report_clock();
    }
    while (usart_read_byte(&byte)) {
      process_command(byte);
    }
  }
}