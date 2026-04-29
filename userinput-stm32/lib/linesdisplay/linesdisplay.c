#include "linesdisplay.h"
#include "cursor.h"
#include <display.h>
#include <progbar.h>
#include <stdio.h>
#include <stops.h>
#include <string.h>

void linesdisplay_page(uint8_t page, uint16_t lines_selected,
                       const char *available_lines)
{
    /* count available lines (sentinel is '0') */
    uint8_t num_avail = 0;
    while (num_avail < 11 && available_lines[num_avail] != '0')
        num_avail++;

    display_clear();
    display_write("Choose lines...", 0, 0);

    uint8_t col = 0;
    uint8_t line = 1;

    for (uint8_t s = 0; s < num_avail && line < PAGE_NUMS; s++) {
        bool selected = (lines_selected >> s) & 1;
        char buf[8];
        snprintf(buf, sizeof(buf), "|%c%c%c%c",
                 cursor_pos == s ? '>' : ' ',
                 selected ? '[' : ' ',
                 available_lines[s],
                 selected ? ']' : ' ');

        uint8_t label_width = (uint8_t)(strlen(buf) * (CHAR_WIDTH + 1));

        if (col + label_width > DISPLAY_WIDTH) {
            display_write("|", col, line);
            col = 0;
            line++;
            if (line >= PAGE_NUMS) break;
        }

        display_write(buf, col, line);
        col += label_width;
    }

    /* "next" / done button */
    display_write("|", col, line);
    col += 8;
    display_write("next", col, line);
    display_write("|", col + 22, line);

    if (cursor_pos == num_avail) {
        for (uint8_t c = col; c < col + 22; c++)
            fb[line * PAGE_WIDTH + c] ^= 0xFF;
    }

    float scrolled_ratio = num_avail > 0 ? (float)cursor_pos / num_avail : 0;
    uint8_t bar_height = 4;
    uint8_t bar_width = 32;
    draw_progbar(DISPLAY_WIDTH - bar_width, DISPLAY_HEIGHT - bar_height,
                 bar_width, bar_height, (uint8_t)(scrolled_ratio * 100));

    display_flush();
}
