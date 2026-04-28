#ifndef __PS2__H
#define __PS2__H

#include <stdbool.h>
#include <stdint.h>

/*
 * PS/2 keyboard driver for STM32L432KC.
 *
 * Wiring:
 *   PS/2 Clock → D3 (PB0)   — drives EXTI0 falling-edge interrupt
 *   PS/2 Data  → D2 (PA12)
 *   PS/2 VCC   → 3.3V
 *   PS/2 GND   → GND
 */

void ps2_init(void);
void ps2_poll(void);

bool ps2_consume_up(void);
bool ps2_consume_down(void);
bool ps2_consume_left(void);
bool ps2_consume_right(void);
bool ps2_consume_enter(void);

#endif
