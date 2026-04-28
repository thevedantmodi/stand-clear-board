#include "subwayselector.h"
#include <displayarrival.h>
#include <ee14lib.h>
#include <pixel.h>
#include <stdlib.h>
#include <subwayicons.h>

extern Pixel_T frameBuffer[PIXELS_COUNT];

void subwayselector_listen()
{

    // BYTE 0: minute
    // BYTE 1: line index
    // BYTE 2: direction (0 or 1)

    // BYTE 3: minute
    // BYTE 4: line index
    // BYTE 5: direction (0 or 1)

    // BYTE 6: minute
    // BYTE 7: line index
    // BYTE 8: direction (0 or 1)

    // BYTE 9: minute
    // BYTE 10: line index
    // BYTE 11: direction (0 or 1)

    uint16_t flash_tick = 0;
    while (1) {
        memset(frameBuffer, 0, sizeof(frameBuffer));
        displayline_arrival(SUB_A, "Uptown", 56, LINE0, flash_tick);
        displayline_arrival(SUB_ONE, "Downtown", 34, LINE1, flash_tick);
        displayline_arrival(SUB_FOUR, "Uptown", 1, LINE2, flash_tick);
        displayline_arrival(SUB_SEVEN, "Flushing", 1212, LINE3, flash_tick);
        HUB75E_setDisplayBuffer(frameBuffer);
        for (size_t j = 0; j < 100; j++) {
            HUB75E_displayBufferPixels();
            delay_ms(1);
        }
        flash_tick++;
    }
}