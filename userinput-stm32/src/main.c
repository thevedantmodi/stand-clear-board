#include <confirmdisplay.h>
#include <cursor.h>
#include <ps2.h>
#include <display.h>
#include <ee14lib.h>
#include <linesdisplay.h>
#include <progbar.h>
#include <stdio.h>
#include <stdlib.h>
#include <stm32l432xx.h>
#include <stops.h>
#include <stopsdisplay.h>
#include <string.h>
#include <successdisplay.h>
#include <systick.h>
#include <transmitter.h>

#define FILTER_MAX 19  /* ">" prefix takes 1 char, leaving 20 of 21 col row */

static int8_t line_idx_for_char(char c)
{
    switch (c) {
        case '1': return 0;  case '2': return 1;  case '3': return 2;
        case '4': return 3;  case '5': return 4;  case '6': return 5;
        case '7': return 6;  case 'A': return 7;  case 'B': return 8;
        case 'C': return 9;  case 'D': return 10; case 'E': return 11;
        case 'F': return 12; case 'G': return 13; case 'H': return 14;
        case 'J': return 15; case 'L': return 16; case 'M': return 17;
        case 'N': return 18; case 'Q': return 19; case 'R': return 20;
        case 'W': return 21; case 'Z': return 22;
        default:  return -1;
    }
}

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

static void rebuild_filter(uint8_t route, uint8_t *filtered,
                            uint8_t *filtered_count, const char *filter_buf)
{
    *filtered_count = 0;
    subway_route_t rt = subway_routes[route];
    for (uint8_t i = 0; i < rt.stop_count; i++) {
        if (search(rt.stops[i], filter_buf))
            filtered[(*filtered_count)++] = i;
    }
}

void userinput(uint32_t *lines_selected, uint64_t stops_per_line[])
{
    static screen_t last_screen = SCREEN_LINE;
    static uint64_t stops_selected = 0;
    static uint8_t current_station = 0;

    /* Filter state for SCREEN_STOPS */
    static char filter_buf[FILTER_MAX + 1] = {0};
    static uint8_t filter_len = 0;
    static uint8_t filter_cursor = 0;
    static uint8_t filtered[64];
    static uint8_t filtered_count = 0;

    while (1) {
        /* Poll input — use cursor_poll for SCREEN_LINE.
         * For all other screens, call ps2_poll directly so that left/right
         * aren't consumed before side_poll gets them. */
        if (current_screen == SCREEN_LINE) {
            cursor_poll();
        } else {
            ps2_poll();
        }

        /* ---- SCREEN_LINE input ---- */
        if (current_screen == SCREEN_LINE) {
            if (switch_poll()) {
                if (cursor_pos == LINE_6X && (*lines_selected) != 0) {
                    current_screen = SCREEN_STOPS;
                } else {
                    toggle_option(cursor_pos, (uint64_t *)lines_selected);
                }
            }
            char c = ps2_consume_char();
            if (c) {
                int8_t idx = line_idx_for_char(c);
                if (idx >= 0) {
                    toggle_option((uint8_t)idx, (uint64_t *)lines_selected);
                }
            }
            if (ps2_consume_enter() && (*lines_selected) != 0) {
                current_screen = SCREEN_STOPS;
            }
        }

        /* ---- SCREEN_STOPS input ---- */
        else if (current_screen == SCREEN_STOPS) {
            /* Up/Down navigate filtered list */
            if (ps2_consume_up()) {
                if (filter_cursor > 0) filter_cursor--;
            } else if (ps2_consume_down()) {
                if (filtered_count > 0 && filter_cursor + 1 < filtered_count)
                    filter_cursor++;
            }

            /* Character keys: append to filter (lowercased) */
            char c = ps2_consume_char();
            if (c && filter_len < FILTER_MAX) {
                filter_buf[filter_len++] = (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c;
                filter_buf[filter_len] = '\0';
                rebuild_filter(current_station, filtered, &filtered_count, filter_buf);
                filter_cursor = 0;
            }

            /* Space: append space to filter */
            if (ps2_consume_space() && filter_len < FILTER_MAX) {
                filter_buf[filter_len++] = ' ';
                filter_buf[filter_len] = '\0';
                rebuild_filter(current_station, filtered, &filtered_count, filter_buf);
                filter_cursor = 0;
            }

            /* Backspace: delete last char */
            if (ps2_consume_backspace() && filter_len > 0) {
                filter_buf[--filter_len] = '\0';
                rebuild_filter(current_station, filtered, &filtered_count, filter_buf);
                filter_cursor = 0;
            }

            /* Enter: toggle highlighted stop; for single result, auto-advance */
            if (ps2_consume_enter() && filtered_count > 0) {
                uint8_t stop_idx = filtered[filter_cursor];
                toggle_option(stop_idx, &stops_selected);

                if (filtered_count == 1) {
                    /* Auto-advance: save state, find next selected line */
                    stops_per_line[current_station] = stops_selected;
                    bool found = false;
                    uint8_t rcount = (uint8_t)subway_route_count;
                    for (uint8_t i = 1; i <= rcount; i++) {
                        uint8_t next = (uint8_t)((current_station + i) % rcount);
                        if (get_option(next, *lines_selected)) {
                            if (next <= current_station) {
                                /* Wrapped past last selected line */
                                current_screen = SCREEN_DONE;
                            } else {
                                current_station = next;
                                stops_selected  = stops_per_line[current_station];
                                filter_len = 0; filter_buf[0] = '\0';
                                rebuild_filter(current_station, filtered,
                                               &filtered_count, filter_buf);
                                filter_cursor = 0;
                            }
                            found = true;
                            break;
                        }
                    }
                    if (!found) current_screen = SCREEN_DONE;
                }
            }

            /* Left/Right: switch between selected lines */
            int8_t dir = side_poll();
            if (dir != 0) {
                uint8_t rcount = (uint8_t)subway_route_count;
                stops_per_line[current_station] = stops_selected;
                for (uint8_t i = 1; i <= rcount; i++) {
                    uint8_t next;
                    if (dir > 0) {
                        next = (uint8_t)((current_station + i) % rcount);
                    } else {
                        next = (uint8_t)((current_station + rcount - i) % rcount);
                    }

                    if (get_option(next, *lines_selected)) {
                        if (dir > 0 && next <= current_station) {
                            current_screen = SCREEN_DONE;
                        } else {
                            current_station = next;
                            stops_selected  = stops_per_line[current_station];
                            filter_len = 0; filter_buf[0] = '\0';
                            rebuild_filter(current_station, filtered,
                                           &filtered_count, filter_buf);
                            filter_cursor = 0;
                        }
                        break;
                    }
                }
            }
        }

        /* ---- SCREEN_DONE input ---- */
        else if (current_screen == SCREEN_DONE) {
            if (switch_poll())
                current_screen = SCREEN_SUCCESS;
            int8_t dir = side_poll();
            if (dir < 0) {
                stops_selected = stops_per_line[current_station];
                filter_len = 0; filter_buf[0] = '\0';
                rebuild_filter(current_station, filtered, &filtered_count, filter_buf);
                filter_cursor = 0;
                current_screen = SCREEN_STOPS;
            }
        }

        /* ---- Screen transition setup ---- */
        if (current_screen != last_screen) {
            if (current_screen == SCREEN_STOPS) {
                if (last_screen == SCREEN_LINE) {
                    /* Find the first selected line */
                    for (uint8_t i = 0; i < (uint8_t)subway_route_count; i++) {
                        if (get_option(i, *lines_selected)) {
                            current_station = i;
                            break;
                        }
                    }
                    stops_selected = stops_per_line[current_station];
                    filter_len = 0; filter_buf[0] = '\0';
                    rebuild_filter(current_station, filtered, &filtered_count, filter_buf);
                    filter_cursor = 0;
                }
                /* Coming from SCREEN_DONE: state already set in the DONE handler */
            } else if (current_screen == SCREEN_LINE) {
                cursor_clear(LINE_6X);
            }
            last_screen = current_screen;
        }

        /* ---- Display dispatch ---- */
        if (current_screen == SCREEN_LINE) {
            linesdisplay_page(0, (*lines_selected));
        } else if (current_screen == SCREEN_STOPS) {
            stopdisplay_page(current_station, stops_selected,
                             filtered, filtered_count, filter_cursor, filter_buf);
        } else if (current_screen == SCREEN_DONE) {
            display_clear();
            confirmdisplay_page();
        } else if (current_screen == SCREEN_SUCCESS) {
            display_clear();
            successdisplay_page();
            delay_ms(3000);
            break;
        }
        delay_ms(16);
    }
}

int main(void)
{
    host_serial_init(USART2, 9600);
    SysTick_initialize();
    display_init();
    cursor_init();
    transmitter_init();

    uint32_t lines_selected = 0;
    uint64_t stops_per_line[NUM_ROUTES] = {0};
    userinput(&lines_selected, stops_per_line);
    transmitter_sendselections(lines_selected, stops_per_line);
}
