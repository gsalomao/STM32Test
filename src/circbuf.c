/**
 * @brief   Circular buffer implementation.
 */

#include "circbuf.h"
#include "assert.h"
#include <string.h>

/** Minimum memory size to hold data in circular buffer */
#define CIRCBUF_MIN_MEM_SIZE            2

/**
 * Initialize circular buffer.
 *
 * @param   buf     Circular buffer to be initialized.
 * @param   mem     Memory area where the data will be stored.
 * @param   size    Size of the memory area.
 */
void CircBuf_Init(CircularBuffer_t *buf, void *mem, size_t size)
{
    ASSERT(buf);
    ASSERT(mem);
    ASSERT(size >= CIRCBUF_MIN_MEM_SIZE);

    /* Initialize buffer structure */
    buf->mem = (uint8_t *) mem;
    buf->size = size;
    buf->len = 0;
    buf->head = 0;
    buf->tail = 0;

    /* Clean buffer memory */
    memset(mem, 0, size);
}

/**
 * Write data to circular buffer.
 *
 * @param   buf     Circular buffer where data will be written.
 * @param   data    Data to be written in circular buffer.
 * @param   size    Size of the data to be written in circular buffer.
 *
 * @returns It returns 'true' if the data has been written with success.
 *          Otherwise, it returns 'false'.
 */
bool CircBuf_Write(CircularBuffer_t *buf, const void *data, size_t size)
{
    bool result = false;

    /* Check if there is available space in the buffer */
    if ((buf->len + size) <= buf->size)
    {
        size_t index;
        uint8_t *dataPtr = (uint8_t *) data;

        for (index = 0; index < size; index++)
        {
            buf->mem[buf->tail] = *dataPtr++;
            buf->len++;

            /* Increment tail and make sure it's not out of bounds */
            buf->tail = (buf->tail + 1) % buf->size;
        }

        result = true;
    }

    return result;
}

/**
 * Read data to circular buffer.
 *
 * @param   buf     Circular buffer where data will be read.
 * @param   data    Memory where the read data will be stored.
 * @param   size    Size of the data to be read in circular buffer.
 *
 * @returns It returns 'true' if the data has been read with success.
 *          Otherwise, it returns 'false'.
 */
bool CircBuf_Read(CircularBuffer_t *buf, void *data, size_t size)
{
    bool result = false;

    /* Check if there is elements in the buffer */
    if (size <= buf->len)
    {
        size_t index;
        uint8_t *dataPtr = (uint8_t *) data;

        for (index = 0; index < size; index++)
        {
            *dataPtr++ = buf->mem[buf->head];
            buf->len--;

            /* Increment head and make sure it's not out of bounds */
            buf->head = (buf->head + 1) % buf->size;
        }

        result = true;
    }

    return result;
}
