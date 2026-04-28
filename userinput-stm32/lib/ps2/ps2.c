#include "ps2.h"
#include <ee14lib.h>
#include <stm32l432xx.h>

#define BUF_SIZE 16

// Circular buffer to store data from keyboard
static volatile uint8_t buf[BUF_SIZE];
static volatile uint8_t buf_head = 0;
static volatile uint8_t buf_tail = 0;

static volatile uint16_t ps2_shift_reg = 0;
static volatile uint8_t  ps2_bit_count = 0;

// EXTI0 interrupt handler: one call per PS2 clock falling edge
void EXTI0_IRQHandler(void)
{
    if (!(EXTI->PR1 & EXTI_PR1_PIF0)) return;
    EXTI->PR1 = EXTI_PR1_PIF0;

    uint8_t bit = gpio_read(D2);
    ps2_shift_reg |= ((uint16_t)bit << ps2_bit_count);
    ps2_bit_count++;

    if (ps2_bit_count < 11) return;

    uint8_t start  = (ps2_shift_reg >> 0)  & 0x01;
    uint8_t data   = (ps2_shift_reg >> 1)  & 0xFF;
    uint8_t parity = (ps2_shift_reg >> 9)  & 0x01;
    uint8_t stop   = (ps2_shift_reg >> 10) & 0x01;

    if (start == 0 && stop == 1 &&
        (__builtin_popcount(data) + parity) % 2 == 1) {
        uint8_t next = (buf_head + 1) % BUF_SIZE;
        if (next != buf_tail) { // Drop if full
            buf[buf_head] = data;
            buf_head = next;
        }
    }

    ps2_bit_count = 0;
    ps2_shift_reg = 0;
}

static volatile bool pending_up    = false;
static volatile bool pending_down  = false;
static volatile bool pending_left  = false;
static volatile bool pending_right = false;
static volatile bool pending_enter = false;

static void process_scancode(uint8_t code)
{
    typedef enum { IDLE, E0, BREAK, E0_BREAK } st_t;
    static st_t state = IDLE;

    switch (state) {
    case IDLE:
        if      (code == 0xE0) { state = E0; }
        else if (code == 0xF0) { state = BREAK; }
        else {
            switch (code) {
                case 0x1D: pending_up    = true; serial_write(USART2, "UP\r\n",    4); break; // W
                case 0x1B: pending_down  = true; serial_write(USART2, "DOWN\r\n",  6); break; // S
                case 0x1C: pending_left  = true; serial_write(USART2, "LEFT\r\n",  6); break; // A
                case 0x23: pending_right = true; serial_write(USART2, "RIGHT\r\n", 7); break; // D
                case 0x5A: pending_enter = true; serial_write(USART2, "ENTER\r\n", 7); break; // Enter
                case 0x29: pending_enter = true; serial_write(USART2, "ENTER\r\n", 7); break; // Space
            }
        }
        break;

    case E0:
        if (code == 0xF0) { state = E0_BREAK; }
        else {
            switch (code) {
                case 0x75: pending_up    = true; serial_write(USART2, "UP\r\n",    4); break;
                case 0x72: pending_down  = true; serial_write(USART2, "DOWN\r\n",  6); break;
                case 0x6B: pending_left  = true; serial_write(USART2, "LEFT\r\n",  6); break;
                case 0x74: pending_right = true; serial_write(USART2, "RIGHT\r\n", 7); break;
            }
            state = IDLE;
        }
        break;

    case BREAK:    state = IDLE; break; // Ignore key up
    case E0_BREAK: state = IDLE; break; // Ignore extended key up
    }
}

// Drain the ring buffer
void ps2_poll(void)
{
    while (buf_tail != buf_head) {
        uint8_t code = buf[buf_tail];
        buf_tail = (buf_tail + 1) % BUF_SIZE;
        process_scancode(code);
    }
}

void ps2_init(void)
{
    // D3 = PB0 = CLK, D2 = PA12 = DATA.
    gpio_config_mode(D3, INPUT);
    gpio_config_pullup(D3, PULL_OFF);
    gpio_config_mode(D2, INPUT);
    gpio_config_pullup(D2, PULL_OFF);

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
    SYSCFG->EXTICR[0] |=  SYSCFG_EXTICR1_EXTI0_PB;
    EXTI->FTSR1 |=  EXTI_FTSR1_FT0;
    EXTI->RTSR1 &= ~EXTI_RTSR1_RT0;
    EXTI->IMR1  |=  EXTI_IMR1_IM0;

    NVIC_SetPriority(EXTI0_IRQn, 1);
    NVIC_EnableIRQ(EXTI0_IRQn);
}

bool ps2_consume_up(void)    { bool v = pending_up;    pending_up    = false; return v; }
bool ps2_consume_down(void)  { bool v = pending_down;  pending_down  = false; return v; }
bool ps2_consume_left(void)  { bool v = pending_left;  pending_left  = false; return v; }
bool ps2_consume_right(void) { bool v = pending_right; pending_right = false; return v; }
bool ps2_consume_enter(void) { bool v = pending_enter; pending_enter = false; return v; }
