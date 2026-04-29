#include "cursor.h"
#include <ps2.h>
#include <stops.h>

uint8_t  cursor_pos     = 0;
uint8_t  cursor_max     = LINE_6X;
screen_t current_screen = SCREEN_LINE;

void cursor_init(void)
{
    ps2_init();
}

void cursor_clear(uint8_t new_max)
{
    cursor_pos = 0;
    cursor_max = new_max;
}

/* Move the cursor through a grid with 4 columns, wrapping at edges. */
void cursor_poll(void)
{
    ps2_poll();

    const uint8_t COLS = 4;
    const uint8_t count = cursor_max + 1;

    uint8_t row = cursor_pos / COLS;
    uint8_t col = cursor_pos % COLS;

    if (ps2_consume_left()) {
        if (cursor_pos == 0) {
            cursor_pos = cursor_max;              /* wrap to last item */
        } else {
            cursor_pos--;
        }
    }
    else if (ps2_consume_right()) {
        cursor_pos = (cursor_pos + 1) % count;    /* wrap to first item */
    }
    else if (ps2_consume_up()) {
        if (cursor_pos < COLS) {
            /* wrap to bottom of same column */
            uint8_t last_row = cursor_max / COLS;
            cursor_pos = last_row * COLS + col;
            if (cursor_pos > cursor_max) {
                cursor_pos -= COLS;               /* adjust if column missing */
            }
        } else {
            cursor_pos -= COLS;
        }
    }
    else if (ps2_consume_down()) {
        if (cursor_pos + COLS > cursor_max) {
            /* wrap to top of same column */
            cursor_pos = col;
            if (cursor_pos > cursor_max) {
                cursor_pos = cursor_max;
            }
        } else {
            cursor_pos += COLS;
        }
    }
}

/* Returns true once per Space keypress (toggle at cursor).
 * Enter is handled separately in userinput() as a screen-advance key. */
bool switch_poll(void)
{
    return ps2_consume_space();
}

/* Returns +1 (D / Right-arrow) or -1 (A / Left-arrow), 0 if neither. */
int8_t side_poll(void)
{
    if (ps2_consume_right()) return  1;
    if (ps2_consume_left())  return -1;
    return 0;
}

void toggle_option(uint8_t option, uint64_t *sel) { *sel ^= (1ULL << option); }
bool get_option(uint8_t option, uint64_t sel)      { return (sel >> option) & 1; }
