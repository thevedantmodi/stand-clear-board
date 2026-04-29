#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define CB_LEN 4096
// #define CB_LEN 2048

typedef char Buf_Elem_t;

extern Buf_Elem_t Buffer_buf[CB_LEN];
extern uint16_t Buffer_read;
extern uint16_t Buffer_write;

bool Buffer_empty(void);
uint16_t Buffer_length(void);
int Buffer_push(Buf_Elem_t e);
int Buffer_pop(void);

#endif