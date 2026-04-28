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
#include <subwayselector.h>

Pixel_T frameBuffer[PIXELS_COUNT];

int main(void)
{

    speedup();
    host_serial_init(USART2, 9600);

    SysTick_initialize();
    HUB75E_Init();
    HUB75E_setDisplayBrightness(BrightnessLevel5);
    HUB75E_setAddressingMode(HUB75EAddressingModeABCDE);

    subwayselector_listen();
}
