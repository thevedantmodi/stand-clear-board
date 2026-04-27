#include "icon.h"
#include <HUB75ELib.h>
#include <font.h>
#include <string.h>

void icon_fill_rect(Pixel_T *fb, uint8_t x, uint8_t y, uint8_t w, uint8_t h,
                    Pixel_T color)
{
    for (uint8_t row = y; row < y + h && row < VERTICAL_PIXELS; row++) {
        for (uint8_t col = x; col < x + w && col < HORIZONTAL_PIXELS; col++) {
            fb[row * HORIZONTAL_PIXELS + col] = color;
        }
    }
}

void icon_draw(Pixel_T *fb, const Icon_T *icon, uint8_t x, uint8_t y)
{
    icon_fill_rect(fb, x, y, icon->width, icon->height, icon->bg);

    uint8_t text_w = (CHAR_WIDTH + 1) - 1;
    uint8_t text_h = 5;

    uint8_t text_x =
        x + (icon->width > text_w ? (icon->width - text_w) / 2 : 0);
    uint8_t text_y =
        y + (icon->height > text_h ? (icon->height - text_h) / 2 : 0);

    font_drawstr_color(fb, icon->label, text_x, text_y, icon->fg);
}