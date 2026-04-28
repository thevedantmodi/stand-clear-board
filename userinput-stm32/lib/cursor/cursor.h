#ifndef __CURSOR__H
#define __CURSOR__H

#include <stdbool.h>
#include <stdint.h>

extern uint8_t cursor_pos;
extern uint8_t cursor_max;

typedef enum {
    SCREEN_LINE,
    SCREEN_STOPS,
    SCREEN_DONE,
    SCREEN_SUCCESS,
} screen_t;

extern screen_t current_screen;

void   cursor_init(void);
void   cursor_clear(uint8_t new_max);
void   cursor_poll(void);
bool   switch_poll(void);
int8_t side_poll(void);

void toggle_option(uint8_t option, uint64_t *sel);
bool get_option(uint8_t option, uint64_t sel);

#endif // __CURSOR__H
