#include "HUB75ELib.h"
#include "systick.h"
#include <ee14lib.h>
#include <font.h>
#include <icon.h>
#include <pixel.h>
#include <stm32l432xx.h>
#include <string.h>
#include <subwayicons.h>

Pixel_T frameBuffer[PIXELS_COUNT];

int main(void)
{
    // turns up the clock speed to 80 MHz
    RCC->CR |= RCC_CR_MSION;
    while ((RCC->CR & RCC_CR_MSIRDY) == 0)
        ;
    RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_MSI | (0 << RCC_PLLCFGR_PLLM_Pos) |
                   (40 << RCC_PLLCFGR_PLLN_Pos) | RCC_PLLCFGR_PLLREN;
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0)
        ;
    FLASH->ACR |= FLASH_ACR_LATENCY_4WS;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
        ;

    host_serial_init(USART2, 9600);

    SysTick_initialize();
    HUB75E_Init();
    // subwayicons_init();
    HUB75E_setDisplayBrightness(BrightnessLevel4);
    HUB75E_setAddressingMode(HUB75EAddressingModeABCDE);

    // Icon_T icon = {0, 0, 7, 7, {0b11, 0b00, 0b00}, {3, 3, 3}, "1"};
    
    // Icon_T icon = {0, 0, 7, 7, {0b11, 0b00, 0b00}, {3, 3, 3}, "2"};
    // Icon_T icon = {0, 0, 7, 7, {0b11, 0b00, 0b00}, {3, 3, 3}, "3"};
    // Icon_T icon = {0, 0, 7, 7, {0b00, 0b11, 0b00}, {3, 3, 3}, "4"};
    // Icon_T icon = {0, 0, 7, 7, {0b00, 0b11, 0b00}, {3, 3, 3}, "5"};
    // Icon_T icon = {0, 0, 7, 7, {0b00, 0b11, 0b00}, {3, 3, 3}, "6"};
    // Icon_T icon = {0, 0, 7, 7, PX_PURPLE, {3, 3, 3}, "7"};
    // Icon_T icon = {0, 0, 7, 7, PX_PURPLE, PX_WHITE, "7"};
    // Icon_T icon = {0, 0, 7, 7, PX_BLUE, PX_WHITE, "A"};
    // Icon_T icon = {0, 0, 7, 7, PX_ORANGE, PX_WHITE, "B"};
    // Icon_T icon = {0, 0, 7, 7, PX_BLUE, PX_WHITE, "C"};
    // Icon_T icon = {0, 0, 7, 7, PX_ORANGE, PX_WHITE, "D"};
    // Icon_T icon = {0, 0, 7, 7, PX_BLUE, PX_WHITE, "E"};
    // Icon_T icon = {0, 0, 7, 7, PX_ORANGE, PX_WHITE, "F"};
    // Icon_T icon = {0, 0, 7, 7, PX_LGREEN, PX_WHITE, "G"};
    // Icon_T icon = {0, 0, 7, 7, PX_BLUE, PX_WHITE, "H"};
    // Icon_T icon = {0, 0, 7, 7, PX_BROWN, PX_WHITE, "J"};
    // Icon_T icon = {0, 0, 7, 7, PX_GRAY, PX_WHITE, "L"};
    // Icon_T icon = {0, 0, 7, 7, PX_ORANGE, PX_WHITE, "M"};
    // Icon_T icon = {0, 0, 7, 7, PX_YELLOW, PX_WHITE, "N"};
    // Icon_T icon = {0, 0, 7, 7, PX_YELLOW, PX_WHITE, "Q"};
    // Icon_T icon = {0, 0, 7, 7, PX_YELLOW, PX_WHITE, "R"};
    // Icon_T icon = {0, 0, 7, 7, PX_GRAY, PX_WHITE, "S"};
    Icon_T icon = {0, 0, 7, 7, PX_YELLOW, PX_WHITE, "W"};
    // Icon_T icon = {0, 0, 7, 7, PX_BROWN, PX_WHITE, "Z"};








    // memset(frameBuffer, 0, sizeof(frameBuffer));
    // icon_draw(frameBuffer, &subway_icons[SUB_ONE]);
    memset(frameBuffer, 0, sizeof(frameBuffer));
    icon_draw(frameBuffer, &icon);
    HUB75E_setDisplayBuffer(frameBuffer);
    while (1) {
        HUB75E_displayBufferPixels();
        delay_ms(1);
    }
}
