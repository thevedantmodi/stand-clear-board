#include <confirmdisplay.h>
#include <cursor.h>
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

// This function is called by printf() to handle the text string
// We want it to be sent over the serial terminal, so we just delegate to
// that function
int _write(int file, char *data, int len)
{
    serial_write(USART2, data, len);
    return len;
}

int main(void)
{
    host_serial_init(9600);
    SysTick_initialize();
    display_init();
    cursor_init();

    subway_line_t current_station = 0;
    screen_t last_screen = SCREEN_LINE;
    cursor_clear(LINE_6X);
    uint32_t lines_selected = 0;
    uint64_t stops_per_line[NUM_ROUTES] = {0};
    uint64_t stops_selected = 0;
    /* Listen for user input */
    while (1) {
        cursor_poll();

        /* handle switch inputs based on screen state */
        if (switch_poll()) {
            if (current_screen == SCREEN_LINE) {
                if (cursor_pos == LINE_6X && lines_selected != 0) {
                    current_screen = SCREEN_STOPS;
                } else {
                    toggle_option(cursor_pos, (uint64_t)&lines_selected);
                }
            } else if (current_screen == SCREEN_STOPS) {
                toggle_option(cursor_pos, &stops_selected);
            } else if (current_screen == SCREEN_DONE) {
                current_screen = SCREEN_SUCCESS;
            }
        }

        /* switch screens when we are choosing stops */
        if (current_screen == SCREEN_STOPS) {
            int8_t dir = side_poll();
            if (dir != 0) {
                uint8_t next = current_station;
                for (uint8_t i = 0; i < subway_route_count; i++) {
                    if (dir > 0) {
                        next = (next + 1) % subway_route_count;
                    } else if (next == 0) {
                        next = (subway_route_count - 1);
                    } else {
                        next += dir;
                    }
                    if (get_option(next, lines_selected)) {
                        stops_per_line[current_station] = stops_selected;
                        if (dir > 0 && next <= current_station) {
                            /* wrapped past last selected line — go to confirm
                             */
                            current_screen = SCREEN_DONE;
                        } else {
                            /* restore new state */
                            current_station = next;
                            stops_selected = stops_per_line[current_station];
                            cursor_clear(
                                subway_routes[current_station].stop_count - 1);
                        }
                        break;
                    }
                }
            }
        } else if (current_screen == SCREEN_DONE) {
            int8_t dir = side_poll();
            if (dir < 0) {
                /* go back to stops at the last selected station */
                stops_selected = stops_per_line[current_station];
                cursor_clear(subway_routes[current_station].stop_count - 1);
                current_screen = SCREEN_STOPS;
            }
        }

        /* update screens upon new event */
        if (current_screen != last_screen) {
            if (current_screen == SCREEN_STOPS) {
                stops_selected = 0;
                cursor_clear(subway_routes[current_station].stop_count - 1);
            } else if (current_screen == SCREEN_LINE) {
                cursor_clear(LINE_6X);
            }
            last_screen = current_screen;
        }

        /* dispatch to screen handler */
        if (current_screen == SCREEN_LINE) {
            linesdisplay_page(0, lines_selected);
        } else if (current_screen == SCREEN_STOPS) {
            stopdisplay_page(current_station, stops_selected);
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

    /* Start serializing UART */
    display_clear();
    display_flush();

}