#include "ee14lib.h"
#include "ssd1306.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <systick.h>

extern int32_t i2c_send(void *ctx, uint8_t addr7, const uint8_t *data,
                        size_t len);

static uint8_t fb[1024];

#define APP_OLED_WIDTH 128U
#define APP_OLED_HEIGHT 64U
#define APP_OLED_BUFFER_SIZE ((APP_OLED_WIDTH * APP_OLED_HEIGHT) / 8U)

static void set_pixel(uint8_t *buffer, uint16_t x, uint16_t y, uint8_t on)
{
    size_t index;
    uint8_t mask;

    if ((buffer == NULL) || (x >= APP_OLED_WIDTH) || (y >= APP_OLED_HEIGHT)) {
        return;
    }

    index = (size_t)x + ((size_t)(y / 8U) * APP_OLED_WIDTH);
    mask = (uint8_t)(1U << (y % 8U));

    if (on != 0U) {
        buffer[index] |= mask;
    } else {
        buffer[index] &= (uint8_t)(~mask);
    }
}

static void build_demo_bitmap(uint8_t *buffer, size_t length)
{
    uint16_t x;
    uint16_t y;

    if ((buffer == NULL) || (length != APP_OLED_BUFFER_SIZE)) {
        return;
    }

    memset(buffer, 0, length);

    for (x = 0U; x < APP_OLED_WIDTH; x++) {
        set_pixel(buffer, x, 0U, 1U);
        set_pixel(buffer, x, APP_OLED_HEIGHT - 1U, 1U);
    }
    for (y = 0U; y < APP_OLED_HEIGHT; y++) {
        set_pixel(buffer, 0U, y, 1U);
        set_pixel(buffer, APP_OLED_WIDTH - 1U, y, 1U);
    }

    for (x = 0U; x < APP_OLED_WIDTH; x++) {
        set_pixel(buffer, x, (uint16_t)(x / 2U), 1U);
        set_pixel(buffer, x, (uint16_t)((APP_OLED_HEIGHT - 1U) - (x / 2U)), 1U);
    }
}

int main(void)
{
    host_serial_init(9600);
    SysTick_initialize();
    i2c_init(I2C1, D1, D0);

    OLED_Config oled = {.bus_type = OLED_BUS_I2C,
                        .width = 128,
                        .height = 64,
                        .user_context = NULL,
                        .transport.i2c = {
                            .i2c_address_7bit = 0x3C,
                            .send_fn = i2c_send,
                        }};

    OLED_Init(&oled);
    OLED_Fill(&oled, 0x55);

    build_demo_bitmap(fb, 1024);

    OLED_DrawBitmap(&oled, fb, sizeof(fb));

    while (1) {
        printf("filling display 0x%2x\n", 0x55);
        delay_ms(1000);
    }
}