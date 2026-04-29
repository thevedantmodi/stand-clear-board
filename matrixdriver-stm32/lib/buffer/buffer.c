#include "buffer.h"

/* Implemented as a circular buffer */
Buf_Elem_t Buffer_buf[CB_LEN];
uint16_t Buffer_read = 0;
uint16_t Buffer_write = 0;

bool Buffer_empty(void) { return Buffer_read == Buffer_write; }
uint16_t Buffer_length(void)
{
    return (Buffer_write - Buffer_read) & (CB_LEN - 1);
}

/* writes to buffer
 *
 * returns e if pushed successfully
 * returns EOF if failed
 *
 */
int Buffer_push(Buf_Elem_t e)
{
    uint16_t next_write = (Buffer_write + 1) & (CB_LEN - 1);
    if (next_write == Buffer_read) {
        /* full! */
        return EOF;
    }

    Buffer_buf[Buffer_write] = e;
    Buffer_write = next_write;

    return e;
}

/* reads from buffer */
int Buffer_pop(void)
{
    if (Buffer_empty()) {
        return EOF;
    }

    char c = Buffer_buf[Buffer_read];
    Buffer_read = (Buffer_read + 1) & (CB_LEN - 1);
    return (int)c;
}
