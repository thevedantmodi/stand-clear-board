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
static volatile bool pending_enter = false; /* Enter key only — advance screen */
static volatile bool pending_space = false; /* Space key only — toggle at cursor */
static volatile char pending_char  = 0;     /* line-select character key */

static void set_char(char c)
{
    pending_char = c;
    char buf[3] = {c, '\r', '\n'};
}

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
                /* Enter advances screen; Space toggles at cursor */
                case 0x5A: pending_enter = true; break;
                case 0x29: pending_space = true; break;
                /* Number lines 1-7 */
                case 0x16: set_char('1'); break;
                case 0x1E: set_char('2'); break;
                case 0x26: set_char('3'); break;
                case 0x25: set_char('4'); break;
                case 0x2E: set_char('5'); break;
                case 0x36: set_char('6'); break;
                case 0x3D: set_char('7'); break;
                /* Letter lines A-Z (only those that exist as routes) */
                case 0x1C: set_char('A'); break;
                case 0x32: set_char('B'); break;
                case 0x21: set_char('C'); break;
                case 0x23: set_char('D'); break;
                case 0x24: set_char('E'); break;
                case 0x2B: set_char('F'); break;
                case 0x34: set_char('G'); break;
                case 0x33: set_char('H'); break;
                case 0x3B: set_char('J'); break;
                case 0x4B: set_char('L'); break;
                case 0x3A: set_char('M'); break;
                case 0x31: set_char('N'); break;
                case 0x15: set_char('Q'); break;
                case 0x2D: set_char('R'); break;
                case 0x1D: set_char('W'); break;
                case 0x1A: set_char('Z'); break;
            }
        }
        break;

    case E0:
        if (code == 0xF0) { state = E0_BREAK; }
        else {
            switch (code) {
                case 0x75: pending_up    = true; break;
                case 0x72: pending_down  = true; break;
                case 0x6B: pending_left  = true; break;
                case 0x74: pending_right = true; break;
            }
            state = IDLE;
        }
        break;

    case BREAK:    state = IDLE; break;
    case E0_BREAK: state = IDLE; break;
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
bool ps2_consume_space(void) { bool v = pending_space; pending_space = false; return v; }
char ps2_consume_char(void)  { char v = pending_char;  pending_char  = 0;     return v; }
