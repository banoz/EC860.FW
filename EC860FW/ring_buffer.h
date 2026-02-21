/**
 * \file ring_buffer.h
 * \brief Ring buffer API optimized for R8C/27
 */

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

/* Ring buffer descriptor */
typedef unsigned char rbd_t;

/* Ring buffer attributes */
typedef struct {
    unsigned char *buffer;    /* Pointer to buffer array */
    unsigned char n_elem;     /* Number of elements (must be power of 2) */
} rb_attr_t;

/**
 * \brief Initialize a ring buffer
 * \param[out] rbd - pointer to a ring buffer descriptor
 * \param[in] attr - ring buffer attributes
 * \return 0 on success, -1 otherwise
 */
int ring_buffer_init(rbd_t *rbd, rb_attr_t *attr);

/**
 * \brief Add a byte to the ring buffer
 * \param[in] rbd - the ring buffer descriptor
 * \param[in] data - the byte to add
 * \return 0 on success, -1 if buffer full
 */
int ring_buffer_put(rbd_t rbd, unsigned char data);

/**
 * \brief Get (and remove) a byte from the ring buffer
 * \param[in] rbd - the ring buffer descriptor
 * \param[out] data - pointer to store the byte
 * \return 0 on success, -1 if buffer empty
 */
int ring_buffer_get(rbd_t rbd, unsigned char *data);

/**
 * \brief Get number of bytes available in buffer
 * \param[in] rbd - the ring buffer descriptor
 * \return number of bytes available to read
 */
unsigned char ring_buffer_available(rbd_t rbd);

/**
 * \brief Check if buffer is empty
 * \param[in] rbd - the ring buffer descriptor
 * \return 1 if empty, 0 otherwise
 */
unsigned char ring_buffer_is_empty(rbd_t rbd);

/**
 * \brief Check if buffer is full
 * \param[in] rbd - the ring buffer descriptor
 * \return 1 if full, 0 otherwise
 */
unsigned char ring_buffer_is_full(rbd_t rbd);

/**
 * \brief Clear/flush the ring buffer
 * \param[in] rbd - the ring buffer descriptor
 */
void ring_buffer_flush(rbd_t rbd);

#endif /* RING_BUFFER_H */