#ifndef __CURSOR__H
#define __CURSOR__H

#include <ee14lib.h>
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

typedef enum {
    X = A0,
    Y = A1,
} Axis;

#define SW_PIN D9

typedef struct {
    uint16_t x;
    uint16_t y;
} joystick_pos_t;

void cursor_init(void);
joystick_pos_t get_joystick_pos(void);

void toggle_option(uint8_t option, uint64_t *sel);
bool get_option(uint8_t option, uint64_t sel);
void cursor_poll(void);
bool switch_poll(void);
void cursor_clear(uint8_t new_max);
int8_t side_poll(void);

#endif // __CURSOR__H