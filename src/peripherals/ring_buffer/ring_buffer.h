#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

#define RB_SIZE 64u /* must be a power of two */

/* Single producer / single consumer, safe between ISR and main. */
typedef struct {
  volatile uint8_t data[RB_SIZE];
  volatile uint16_t head; /* written by producer */
  volatile uint16_t tail; /* written by consumer */
} ring_buffer_t;

bool rb_push(ring_buffer_t *rb, uint8_t byte);
bool rb_pop(ring_buffer_t *rb, uint8_t *byte);

#endif /* RING_BUFFER_H */