#include <confirmdisplay.h>
#include <cursor.h>
#include <ps2.h>
#include <display.h>
#include <ee14lib.h>
#include <linesdisplay.h>
#include <stdio.h>
#include <stdlib.h>
#include <stm32l432xx.h>
#include <stops.h>
#include <stopsdisplay.h>
#include <string.h>
#include <successdisplay.h>
#include <systick.h>
#include <transmitter.h>

#define FILTER_MAX 18

/* Case-insensitive substring search (ASCII). */
static bool search(const char *str, const char *substr)
{
    if (*substr == '\0') return true;
    for (; *str; str++) {
        const char *s = str, *p = substr;
        while (*s && *p && ((*s | 0x20) == (*p | 0x20))) { s++; p++; }
        if (*p == '\0') return true;
    }
    return false;
}

static void rebuild_filter(uint16_t *filtered, uint16_t *filtered_count,
                            const char *filter_buf)
{
    *filtered_count = 0;
    for (uint16_t i = 0; i < stops_count; i++) {
        if (search(stops[i], filter_buf))
            filtered[(*filtered_count)++] = i;
    }
}


void userinput(void)
{
    static screen_t last_screen    = SCREEN_STOPS;

    /* Stop selection state (SCREEN_STOPS) */
    static char     filter_buf[FILTER_MAX + 1] = {0};
    static uint8_t  filter_len    = 0;
    static uint16_t filter_cursor = 0;
    static uint16_t filtered[MAX_STOPS];
    static uint16_t filtered_count = 0;
    static uint16_t selected_stop  = 0;
    static bool     initialized    = false;

    /* Line selection state (SCREEN_LINE) */
    static uint16_t lines_selected     = 0;
    static uint8_t  num_available_lines = 0;

    if (!initialized) {
        rebuild_filter(filtered, &filtered_count, filter_buf);
        initialized = true;
    }

    while (1) {
        /* Poll input: cursor_poll drives the grid on SCREEN_LINE.
         * For all other screens call ps2_poll directly so left/right
         * are not consumed before side_poll can see them. */
        if (current_screen == SCREEN_LINE) {
            cursor_poll();
        } else {
            ps2_poll();
        }

        /* ---- SCREEN_STOPS: search all stops, select one ---- */
        if (current_screen == SCREEN_STOPS) {
            if (ps2_consume_up()) {
                if (filter_cursor > 0) filter_cursor--;
            } else if (ps2_consume_down()) {
                if (filtered_count > 0 && filter_cursor + 1 < filtered_count)
                    filter_cursor++;
            }

            char c = ps2_consume_char();
            if (c && filter_len < FILTER_MAX) {
                filter_buf[filter_len++] = (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c;
                filter_buf[filter_len] = '\0';
                rebuild_filter(filtered, &filtered_count, filter_buf);
                filter_cursor = 0;
            }

            if (ps2_consume_space() && filter_len < FILTER_MAX) {
                filter_buf[filter_len++] = ' ';
                filter_buf[filter_len] = '\0';
                rebuild_filter(filtered, &filtered_count, filter_buf);
                filter_cursor = 0;
            }

            if (ps2_consume_backspace() && filter_len > 0) {
                filter_buf[--filter_len] = '\0';
                rebuild_filter(filtered, &filtered_count, filter_buf);
                filter_cursor = 0;
            }

            /* Enter: select highlighted stop and advance */
            if (ps2_consume_enter() && filtered_count > 0) {
                selected_stop  = filtered[filter_cursor];
                current_screen = SCREEN_LINE;
            }

            /* Discard left/right have no meaning on the stop search screen */
            ps2_consume_left();
            ps2_consume_right();
        }

        /* ---- SCREEN_LINE: select lines available at the chosen stop ---- */
        else if (current_screen == SCREEN_LINE) {
            if (switch_poll()) {
                if (cursor_pos == num_available_lines && lines_selected != 0) {
                    current_screen = SCREEN_DONE;
                } else if (cursor_pos < num_available_lines) {
                    bool already = (lines_selected >> cursor_pos) & 1;
                    if (already || __builtin_popcount(lines_selected) < 4)
                        lines_selected ^= (1u << cursor_pos);
                }
            }
            if (ps2_consume_enter() && lines_selected != 0u)
                current_screen = SCREEN_DONE;
        }

        /* ---- SCREEN_DONE: any input resets to start ---- */
        else if (current_screen == SCREEN_DONE) {
            bool any = ps2_consume_enter() | ps2_consume_space()
                     | ps2_consume_up()    | ps2_consume_down()
                     | ps2_consume_left()  | ps2_consume_right()
                     | (ps2_consume_char() != 0);
            if (any) {
                lines_selected = 0;
                selected_stop  = 0;
                filter_len = 0; filter_buf[0] = '\0';
                rebuild_filter(filtered, &filtered_count, filter_buf);
                filter_cursor  = 0;
                current_screen = SCREEN_STOPS;
            }
        }

        /* ---- Screen transition setup ---- */
        if (current_screen != last_screen) {
            if (current_screen == SCREEN_STOPS) {
                filter_len = 0; filter_buf[0] = '\0';
                rebuild_filter(filtered, &filtered_count, filter_buf);
                filter_cursor = 0;
            } else if (current_screen == SCREEN_LINE) {
                if (last_screen == SCREEN_STOPS)
                    lines_selected = 0;
                num_available_lines = 0;
                while (num_available_lines < 11 &&
                       stops_to_lines[selected_stop][num_available_lines] != '0')
                    num_available_lines++;
                cursor_clear(num_available_lines);
            } else if (current_screen == SCREEN_DONE) {
                transmitter_sendselections(selected_stop, lines_selected);
            }
            last_screen = current_screen;
        }

        /* ---- Display dispatch ---- */
        if (current_screen == SCREEN_STOPS) {
            stopdisplay_page(filtered, filtered_count, filter_cursor, filter_buf);
        } else if (current_screen == SCREEN_LINE) {
            linesdisplay_page(0, lines_selected,
                              stops_to_lines[selected_stop]);
        } else if (current_screen == SCREEN_DONE) {
            display_clear();
            confirmdisplay_page();
        }

        delay_ms(16);
    }
}

int main(void)
{
    host_serial_init(USART2, 9600);
    host_serial_init(USART1, 9600);
    SysTick_initialize();
    display_init();
    cursor_init();
    transmitter_init();

    userinput();
}
