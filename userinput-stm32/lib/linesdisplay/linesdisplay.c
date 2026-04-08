#include "linesdisplay.h"
#include "cursor.h"
#include <display.h>
#include <progbar.h>
#include <stdio.h>
#include <stops.h>
#include <string.h>

void linesdisplay_page(uint8_t page, uint64_t lines_selected)
{
    display_write("Choose lines...", 0, 0);

    uint8_t col = 0;
    uint8_t line = 1;

    /* truncates if there is no space */
    for (int s = 0; s < LINE_6X && line < PAGE_NUMS; s++) {
        char buf[8];
        snprintf(buf, sizeof(buf), "|%c%c%s%c", cursor_pos == s ? '>' : ' ',
                 get_option(s, lines_selected) ? '[' : ' ', subway_routes[s].route_id,
                 get_option(s, lines_selected) ? ']' : ' ');

        // printf("displaying line %s\n", subway_routes[s].route_id);

        /* because a char is now 6 bytes (5 bytes and padding)! */
        uint8_t label_width = (uint8_t)(strlen(buf) * (CHAR_WIDTH + 1));

        /* line break */
        if (col + label_width > DISPLAY_WIDTH) {
            display_write("|", col, line);
            col = 0;
            line++;
            /* truncation if we print too much */
            if (line >= PAGE_NUMS)
                break;
        }

        display_write(buf, col, line);
        col += label_width;
    }
    display_write("|", col, line);
    col += 8;
    display_write("next", col, line);
    display_write("|", col + 22, line);
    
    /* make it inverted when selected */
    if (cursor_pos == LINE_6X) {
        for (uint8_t c = col; c < col + 22; c++) {
            fb[line * PAGE_WIDTH + c] ^= 0xFF;
        }
    }

    float scrolled_ratio = (float)cursor_pos / (LINE_6X - 1);
    uint8_t bar_height = 4;
    uint8_t bar_width = 32;
    draw_progbar(DISPLAY_WIDTH - bar_width, DISPLAY_HEIGHT - bar_height,
                 bar_width, bar_height, (uint8_t)(scrolled_ratio * 100));

    display_flush();
}