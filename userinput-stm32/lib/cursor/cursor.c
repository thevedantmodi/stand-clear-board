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

/* Move the cursor up (W / Up-arrow) or down (S / Down-arrow). */
void cursor_poll(void)
{
    ps2_poll();
    if (ps2_consume_up()) {
        if (cursor_pos > 0) cursor_pos--;
        else                cursor_pos = cursor_max;
    } else if (ps2_consume_down()) {
        cursor_pos = (cursor_pos + 1) % (cursor_max + 1);
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
