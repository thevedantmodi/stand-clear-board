#include "HUB75ELib.h"
#include "systick.h"
#include <displayarrival.h>
#include <ee14lib.h>
#include <font.h>
#include <icon.h>
#include <pixel.h>
#include <speedup.h>
#include <stdio.h>
#include <stdlib.h>
#include <stm32l432xx.h>
#include <subwayicons.h>


Pixel_T frameBuffer[PIXELS_COUNT];

int main(void)
{

    speedup();
    host_serial_init(USART2, 9600);

    SysTick_initialize();
    HUB75E_Init();
    HUB75E_setDisplayBrightness(BrightnessLevel3);
    HUB75E_setAddressingMode(HUB75EAddressingModeABCDE);

    while (1)
    {
        printf("Helloworld!\n");
    }
    

    static Pixel_T white = {3, 3, 3};
    for (int i = 0; i < PIXELS_COUNT; i++)
        frameBuffer[i] = white;
    HUB75E_setDisplayBuffer(frameBuffer);
    while (1) {
        HUB75E_displayBufferPixels();
    }



    // memset(frameBuffer, 3, sizeof(frameBuffer));

    // while (1) {
    //     HUB75E_setDisplayBuffer(frameBuffer);
    //     HUB75E_displayBufferPixels();
    //     delay_ms(1);

    //     /* code */
    // }

    // uint16_t flash_tick = 0;
    // while (1) {
    //     memset(frameBuffer, 0, sizeof(frameBuffer));
    //     displayline_arrival(SUB_A, "Uptown", 56, LINE0, flash_tick);
    //     // displayline_arrival(SUB_ONE, "Downtown", 34, LINE1, flash_tick);
    //     // displayline_arrival(SUB_FOUR, "Uptown", 1, LINE2, flash_tick);
    //     // displayline_arrival(SUB_SEVEN, "Flushing", 1212, LINE3,
    //     flash_tick); HUB75E_setDisplayBuffer(frameBuffer); for (size_t j = 0;
    //     j < 100; j++) {
    //         HUB75E_displayBufferPixels();
    //         delay_ms(1);
    //     }
    //     flash_tick++;
    // }
}
