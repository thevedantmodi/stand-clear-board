#include "cursor.h"
#include <ee14lib.h>
#include <stdio.h>
#include <stops.h>

extern volatile uint64_t ticks;
#define CURSOR_MOVE_INTERVAL_MS 150
#define NUM_ADC_PINS 2
#define DEADZONE 900
#define CENTER 2048

uint8_t cursor_pos = 0;
uint32_t selected;
screen_t current_screen = SCREEN_LINE;
const EE14Lib_Pin adc_pins[NUM_ADC_PINS] = {Y, X};

void cursor_init(void)
{
    gpio_config_mode(SW_PIN, INPUT);
    gpio_config_pullup(SW_PIN, PULL_UP);
}

joystick_pos_t get_joystick_pos(void)
{
    uint16_t adc_values[NUM_ADC_PINS] = {0};
    for (int i = 0; i < NUM_ADC_PINS; i++) {
        adc_config_single(adc_pins[i]);
        adc_values[i] = adc_read_single();
    }

    return (joystick_pos_t){.x = adc_values[1], .y = adc_values[0]};
}

void cursor_poll(void)
{
    static uint64_t last_move = 0;
    if (ticks - last_move < CURSOR_MOVE_INTERVAL_MS)
        return;

    joystick_pos_t pos = get_joystick_pos();

    if (pos.y > CENTER + DEADZONE) {
        cursor_pos = (cursor_pos + 1) % (LINE_6X + 1);
        last_move = ticks;
    } else if (pos.y < CENTER - DEADZONE) {
        if (cursor_pos > 0) {
            cursor_pos--;
        } else {
            cursor_pos = LINE_6X;
        }
        last_move = ticks;
    }
}

void switch_poll(void)
{
    static bool last = true;
    bool current = gpio_read(SW_PIN);
    if (!current && last) {
        if (cursor_pos == LINE_6X)
            current_screen = SCREEN_STOPS;
        else
            toggle_option(cursor_pos);
    }
    last = current;
}

/* option is a value from 0-28 */
void toggle_option(uint8_t option) { selected ^= (1 << option); }

bool get_option(uint8_t option) { return (selected >> option) & 1; }
