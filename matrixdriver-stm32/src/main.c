#include "HUB75ELib.h"
#include "systick.h"
#include <ee14lib.h>
#include <font.h>
#include <icon.h>
#include <pixel.h>
#include <stm32l432xx.h>
#include <string.h>

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
    HUB75E_setDisplayBrightness(BrightnessLevel4);
    HUB75E_setAddressingMode(HUB75EAddressingModeABCDE);

    while (1) {
        HUB75E_setDisplayBuffer(frameBuffer);
        for (int i = 0; i < 500; i++) {
            HUB75E_displayBufferPixels();
            delay_ms(1);
        }
    }
}
