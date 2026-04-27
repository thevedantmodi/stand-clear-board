#ifndef ICON_H
#define ICON_H

#include <HUB75ELib.h>
#include <pixel.h>
#include <stdint.h>

typedef struct {
    uint8_t width;
    uint8_t height;
    Pixel_T bg;
    Pixel_T fg;
    const char *label;
} Icon_T;

/* Fill a rectangle into fb. */
void icon_fill_rect(Pixel_T *fb, uint8_t x, uint8_t y, uint8_t w, uint8_t h,
                    Pixel_T color);

/* Render a full icon (background rect + centered label) into fb. */
void icon_draw(Pixel_T *fb, const Icon_T *icon, uint8_t x, uint8_t y);

#endif /* ICON_H */
