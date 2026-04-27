#include "speedup.h"
#include <stm32l432xx.h>

void speedup()
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
    SystemCoreClockUpdate();
}