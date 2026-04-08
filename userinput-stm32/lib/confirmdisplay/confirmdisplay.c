#include <confirmdisplay.h>
#include <cursor.h>
#include <display.h>
#include <stdio.h>

void confirmdisplay_page(void)
{
    display_write("All done?", 36, 2);

    uint8_t prompt_start = 20;
    uint8_t prompt_line = 4;
    display_write("Press joystick", prompt_start, prompt_line);
    uint8_t prompt_length = 17 * (CHAR_WIDTH + 1);
    /* invert prompt in UI */
    for (uint8_t c = prompt_start - 2; c < prompt_length + 2; c++) {
        fb[prompt_line * PAGE_WIDTH + c] ^= 0xFF;
    }

    display_flush();
}
