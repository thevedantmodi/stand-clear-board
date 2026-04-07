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

    // linesdisplay_page(0);

    // uint8_t bar_height = 8;
    // uint8_t bar_width = 32;
    // int8_t x;
    // for (x = 100; x >= 0; x--) {
    //     // for (x = 0; x <= 100; x++) {
    //     // display_clear();
    //     draw_progbar(DISPLAY_WIDTH - bar_width, DISPLAY_HEIGHT - bar_height,
    //                  bar_width, bar_height, x);
    //     display_flush();
    // }

    // while (1) {
    //     joystick_pos_t pos = get_joystick_pos();
    //     printf("X: %u, Y: %u\n", pos.x, pos.y);
    //     delay_ms(1000);
    // }

    // subway_line_t current_station = 0;
    // cursor_max = subway_routes[current_station].stop_count - 1;
    // while (1) {
    //     cursor_poll();
    //     switch_poll();
    //     stopdisplay_page(current_station);
    //     delay_ms(16);
    // }

    subway_line_t current_station = 0;
    screen_t last_screen = SCREEN_LINE;
    uint64_t lines_selected = 0;
    uint64_t stops_selected = 0;
    cursor_max = LINE_6X;
    while (1) {
        cursor_poll();

        /* handle switch inputs based on screen state */
        if (switch_poll()) {
            if (current_screen == SCREEN_LINE) {
                if (cursor_pos == LINE_6X) {
                    current_screen = SCREEN_STOPS;
                } else {
                    toggle_option(cursor_pos, &lines_selected);
                }
            } else if (current_screen == SCREEN_STOPS) {
                toggle_option(cursor_pos, &stops_selected);
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
                        current_station = next;
                        stops_selected = 0;
                        cursor_clear(subway_routes[current_station].stop_count -
                                     1);
                        break;
                    }
                }
            }
        }

        /* update screens upon new event */
        if (current_screen != last_screen) {
            if (current_screen == SCREEN_STOPS) {
                stops_selected = 0;
                cursor_clear(subway_routes[current_station].stop_count - 1);
            } else {
                cursor_clear(LINE_6X);
            }
            last_screen = current_screen;
        }

        /* dispatch to screen handler */
        if (current_screen == SCREEN_LINE) {
            linesdisplay_page(0, lines_selected);
        } else {
            stopdisplay_page(current_station, stops_selected);
        }
        delay_ms(16);
    }
}