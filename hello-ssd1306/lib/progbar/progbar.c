#include <display.h>
#include <progbar.h>
#include <stdio.h>

#define HUNDO_P 100

void draw_progbar(uint8_t start_col, uint16_t start_row, uint8_t width,
                  uint16_t height, uint8_t percent)
{
    uint16_t end_col = start_col + width - 1;
    uint16_t end_row = start_row + height - 1;
    uint16_t percent_end_col =
        start_col +
        (uint16_t)((float)(end_col - start_col) * (percent / 100.0));
    uint16_t hundred_percent_end_col = end_col;
    ;

    for (uint16_t col = start_col; col <= end_col; col++) {
        for (uint16_t row = start_row; row <= end_row; row++) {

            /* fill the border */
            uint8_t is_border = (col == start_col) || (col == end_col) ||
                                (row == start_row) || (row == end_row);

            if (is_border) {
                display_setpixel(col, row, 1);
            }

            /* clear all percent bars before 
             * s.t. percent can be updated without
             * clearing the frame buffer  */
            if (col <= end_col) {
                display_setpixel(col, row, 0);
            }
            if (col <= percent_end_col) {
                display_setpixel(col, row, 1);
            }
        }
    }
}