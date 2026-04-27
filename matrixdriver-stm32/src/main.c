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
    HUB75E_setDisplayBrightness(BrightnessLevel3);
    HUB75E_setAddressingMode(HUB75EAddressingModeABCDE);

    while (1) {

        for (int i = SUB_ONE; i <= SUB_Z; i++) {
            memset(frameBuffer, 0, sizeof(frameBuffer));
            icon_draw(frameBuffer, &subway_icons[i], 0, 0);
            HUB75E_setDisplayBuffer(frameBuffer);

            for (size_t j = 0; j < 100; j++) {
                HUB75E_displayBufferPixels();
                delay_ms(1);
            }
        }
    }
}
