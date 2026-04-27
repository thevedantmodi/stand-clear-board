#ifndef __PIXEL__H
#define __PIXEL__H
#include <stdint.h>

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Pixel_T;

// BCM 2-bit color constants (values 0–3 per channel)
#define PX_RED    ((Pixel_T){0b11, 0b00, 0b00})
#define PX_GREEN  ((Pixel_T){0b00, 0b01, 0b00})
#define PX_PURPLE ((Pixel_T){0b01, 0b00, 0b01})
#define PX_BLUE   ((Pixel_T){0b00, 0b00, 0b11})
#define PX_ORANGE ((Pixel_T){0b11, 0b01, 0b00})
#define PX_LGREEN ((Pixel_T){0b10, 0b11, 0b01})
#define PX_GRAY   ((Pixel_T){0b01, 0b01, 0b01})
#define PX_BROWN  ((Pixel_T){0b10, 0b01, 0b00})
#define PX_YELLOW ((Pixel_T){0b11, 0b11, 0b00})
#define PX_TEAL   ((Pixel_T){0b00, 0b10, 0b01})
#define PX_WHITE  ((Pixel_T){0b11, 0b11, 0b11})
#define PX_BLACK  ((Pixel_T){0b00, 0b00, 0b00})

#endif // __PIXEL__H
