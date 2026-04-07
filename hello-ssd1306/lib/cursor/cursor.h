#ifndef __CURSOR__H
#define __CURSOR__H

#include <ee14lib.h>
#include <stdbool.h>
#include <stdint.h>

extern uint32_t selected;
extern uint8_t cursor_pos;

typedef enum {
    SCREEN_LINE,
    SCREEN_STOPS,
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

void toggle_option(uint8_t option);
bool get_option(uint8_t option);
void cursor_poll(void);
void switch_poll(void);

#endif // __CURSOR__H