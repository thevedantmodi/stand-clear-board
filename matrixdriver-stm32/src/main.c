#include "HUB75ELib.h"
#include "systick.h"
#include <ee14lib.h>
#include <font.h>
#include <stm32l432xx.h>
#include <string.h>

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
    HUB75E_setDisplayColor(White);
    HUB75E_setAddressingMode(HUB75EAddressingModeABCDE);
    uint8_t myBitmap[256];
    memset(myBitmap, 0x00, 256);

    // font_drawchar(myBitmap, 'A', 0, 0);
    font_drawstr(myBitmap, "Grand Central-42 St", 0, 0);

    HUB75E_setDisplayBuffer((unsigned char *)myBitmap);

    // char c = 0x20;

    while (1) {

        // memset(myBitmap, 0, sizeof(myBitmap));
        // font_drawchar(myBitmap, c, 0, 0);
        // HUB75E_setDisplayBuffer((unsigned char *)myBitmap);

        // if (c == 0x7E) {
        //     c = 0x20;
        // } else {
        //     c++;
        // }

        for (int i = 0; i < 500; i++) {

            HUB75E_displayBufferPixels();
            delay_ms(1);
        }
    }
}