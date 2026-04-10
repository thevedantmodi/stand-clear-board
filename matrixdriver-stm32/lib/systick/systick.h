#include <stdint.h>

#ifndef __SYSTICK__H
#define __SYSTICK__H

void SysTick_initialize(void);
void delay_ms(uint32_t millis);

#endif // __SYSTICK__H