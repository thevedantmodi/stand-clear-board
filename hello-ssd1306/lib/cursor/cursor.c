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
uint8_t cursor_max = LINE_6X;
screen_t current_screen = SCREEN_LINE;
const EE14Lib_Pin adc_pins[NUM_ADC_PINS] = {Y, X};

void cursor_init(void)
{
    gpio_config_mode(SW_PIN, INPUT);
    gpio_config_pullup(SW_PIN, PULL_UP);
}

void cursor_clear(uint8_t new_max) {
    cursor_pos = 0;
    cursor_max = new_max;
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

    if (pos.x < CENTER - DEADZONE) {
        cursor_pos = (cursor_pos + 1) % (cursor_max + 1);
        last_move = ticks;
    } else if (pos.x > CENTER + DEADZONE) {
        if (cursor_pos > 0) {
            cursor_pos--;
        } else {
            cursor_pos = cursor_max;
        }
        last_move = ticks;
    }
}

bool switch_poll(void)
{
    static bool last = true;
    bool current = gpio_read(SW_PIN);
    bool pressed = !current && last;
    last = current;
    return pressed;
}

void toggle_option(uint8_t option, uint64_t *sel) { *sel ^= (1ULL << option); }

bool get_option(uint8_t option, uint64_t sel) { return (sel >> option) & 1; }

int8_t side_poll(void)
{
    static uint64_t last_move = 0;
    if (ticks - last_move < CURSOR_MOVE_INTERVAL_MS)
        return 0;

    joystick_pos_t pos = get_joystick_pos();

    if (pos.y < CENTER - DEADZONE) {
        last_move = ticks;
        return -1;
    } else if (pos.y > CENTER + DEADZONE) {
        last_move = ticks;
        return 1;
    }
    return 0;
}
