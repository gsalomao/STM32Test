/**
 * @brief   Circular buffer implementation.
 */

#ifndef CIRCBUF_H_
#define CIRCBUF_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/** Circular buffer. */
typedef struct
{
    uint8_t *mem;           /**< Memory area where data will be stored. */
    size_t size;            /**< Size of the memory area.  */
    size_t len;             /**< Number of item in the buffer. */
    size_t head;            /**< Head index. */
    size_t tail;            /**< Tail index. */

} CircularBuffer_t;

void CircBuf_Init(CircularBuffer_t *buf, void *mem, size_t size);
bool CircBuf_Write(CircularBuffer_t *buf, const void *data, size_t size);
bool CircBuf_Read(CircularBuffer_t *buf, void *data, size_t size);

#endif /* CIRCBUF_H_ */
