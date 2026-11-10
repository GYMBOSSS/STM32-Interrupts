#include "ring_buffer.h"

bool rb_push(ring_buffer_t *rb, uint8_t byte) {
  uint16_t next = (uint16_t)((rb->head + 1u) & (RB_SIZE - 1u));

  if (next == rb->tail) {
    return false; /* full */
  }
  rb->data[rb->head] = byte;
  rb->head = next;
  return true;
}

bool rb_pop(ring_buffer_t *rb, uint8_t *byte) {
  if (rb->head == rb->tail) {
    return false; /* empty */
  }
  *byte = rb->data[rb->tail];
  rb->tail = (uint16_t)((rb->tail + 1u) & (RB_SIZE - 1u));
  return true;
}