#ifndef __LOADING__BAR__H
#define __LOADING__BAR__H

#include <stdint.h>

/* Draws a loading bar starting from (col,row) on the display
 * Loading bar is of size (width x height)
 *
 */

void draw_progbar(uint8_t col, uint16_t row, uint8_t width, uint16_t height,
                  uint8_t percent);

#endif // __LOADING__BAR__H
