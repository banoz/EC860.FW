/**
 * \file ring_buffer.c
 * \brief Ring buffer implementation optimized for R8C/27
 */

#include "ring_buffer.h"

#define RING_BUFFER_MAX  2  /* TX and RX */

struct ring_buffer
{
    unsigned char *buf;
    volatile unsigned char head;
    volatile unsigned char tail;
    unsigned char mask;
};

static struct ring_buffer _rb[RING_BUFFER_MAX];
static unsigned char _rb_idx = 0;

int ring_buffer_init(rbd_t *rbd, rb_attr_t *attr)
{
    //if ((_rb_idx >= RING_BUFFER_MAX) || (rbd == NULL) || (attr == NULL)) {
    //    return -1;
    //}
    
    //if ((attr->buffer == NULL) || (attr->n_elem == 0)) {
    //    return -1;
    //}
    
    /* Check that size is power of 2 */
    if ((attr->n_elem & (attr->n_elem - 1)) != 0) {
        return -1;
    }
    
    _rb[_rb_idx].head = 0;
    _rb[_rb_idx].tail = 0;
    _rb[_rb_idx].buf = attr->buffer;
    _rb[_rb_idx].mask = attr->n_elem - 1;
    
    *rbd = _rb_idx++;
    return 0;
}

int ring_buffer_put(rbd_t rbd, unsigned char data)
{
    struct ring_buffer *rb;
    unsigned char next;
    
    if (rbd >= RING_BUFFER_MAX) {
        return -1;
    }
    
    rb = &_rb[rbd];
    next = (rb->head + 1) & rb->mask;
    
    if (next == rb->tail) {
        return -1;  /* Full */
    }
    
    rb->buf[rb->head] = data;
    rb->head = next;
    
    return 0;
}

int ring_buffer_get(rbd_t rbd, unsigned char *data)
{
    struct ring_buffer *rb;
    
    if (rbd >= RING_BUFFER_MAX) {
        return -1;
    }
    
    rb = &_rb[rbd];
    
    if (rb->head == rb->tail) {
        return -1;  /* Empty */
    }
    
    *data = rb->buf[rb->tail];
    rb->tail = (rb->tail + 1) & rb->mask;
    
    return 0;
}

unsigned char ring_buffer_available(rbd_t rbd)
{
    if (rbd >= RING_BUFFER_MAX) {
        return 0;
    }
    return (_rb[rbd].head - _rb[rbd].tail) & _rb[rbd].mask;
}

unsigned char ring_buffer_is_empty(rbd_t rbd)
{
    if (rbd >= RING_BUFFER_MAX) {
        return 1;
    }
    return (_rb[rbd].head == _rb[rbd].tail) ? 1 : 0;
}

unsigned char ring_buffer_is_full(rbd_t rbd)
{
    unsigned char next;
    
    if (rbd >= RING_BUFFER_MAX) {
        return 1;
    }
    
    next = (_rb[rbd].head + 1) & _rb[rbd].mask;
    return (next == _rb[rbd].tail) ? 1 : 0;
}

void ring_buffer_flush(rbd_t rbd)
{
    if (rbd < RING_BUFFER_MAX) {
        _rb[rbd].head = 0;
        _rb[rbd].tail = 0;
    }
}