#include <display.h>
#include <ee14lib.h>
#include <progbar.h>
#include <stdio.h>
#include <stdlib.h>
#include <stm32l432xx.h>
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

    display_write("Choose lines...", 0, 0);
    display_flush();
    // while (1) {
    uint8_t bar_height = 8;
    uint8_t bar_width = 32;
    volatile uint8_t x = 0;
    for (x = 100; x >= 0; x--) {
    // for (x = 0; x <= 100; x++) {
        // display_clear();
        draw_progbar(DISPLAY_WIDTH - bar_width, DISPLAY_HEIGHT - bar_height,
                     bar_width, bar_height, x);
        display_flush();
    }

    // }
}