#ifndef USART_H
#define USART_H

#include <stdint.h>
#include <stdbool.h>

void usart_init(uint32_t baudrate);
bool usart_read_byte(uint8_t *byte);   /* non-blocking, false if nothing received */
void usart_write_byte(uint8_t byte);   /* blocking */
void usart_write_str(const char *str);

#endif /* USART_H */