#include "stm32l432xx.h"

volatile uint64_t ticks = 0;

void SysTick_initialize(void)
{
    SysTick->CTRL = 0;    // disables the counter
    SysTick->LOAD = 3999; // sets the start value to load into val register upon
                          // overflow as 0
    NVIC_SetPriority(
        SysTick_IRQn,
        (1 << __NVIC_PRIO_BITS) -
            1); // sets the priority of the interrupt to be the least important
    SysTick->VAL = 0;                            // clears the tick value
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; // use the processor clock
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   // Each time the countdown gets
                                                 // to 0, trigger an interrupt
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;    // starts the counter
}

void SysTick_Handler(void) { ticks++; }

void delay_ms(uint32_t millis)
{
    uint64_t end = ticks + millis + 1;
    while (ticks < end) {
        // spin - nyoooo
    }
}