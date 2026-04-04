#include <display.h>
#include <ee14lib.h>
#include <ssd1306.h>
#include <stdbool.h>
#include <stdint.h>
#include <stm32l432xx.h>
#include <string.h>

/* The SSD1306 library needs a function of OLED_SendI2CFn type to use to send */
int32_t i2c_send(void *ctx, uint8_t addr7, const uint8_t *data, size_t len)
{
    (void)ctx;
    bool ok = i2c_write(I2C1, addr7, (unsigned char *)data, (unsigned char)len);
    return ok ? OLED_OK : OLED_ERR_IO;
}

OLED_Config oled = {.bus_type = OLED_BUS_I2C,
                    .width = 128,
                    .height = 64,
                    .user_context = NULL,
                    .transport.i2c = {
                        .i2c_address_7bit = 0x3C,
                        .send_fn = i2c_send,
                    }};

uint8_t fb[1024];

void display_init(void)
{
    i2c_init(I2C1, D1, D0);
    OLED_Init(&oled);
}

void display_write(const char *s, uint8_t col, uint8_t line)
{
    font_drawstr(fb, s, col, line);
    OLED_DrawBitmap(&oled, fb, sizeof(fb));
    memset(fb, 0, sizeof(fb));
}

void display_append(const char *s, uint8_t col, uint8_t line)
{
    font_drawstr(fb, s, col, line);
    OLED_DrawBitmap(&oled, fb, sizeof(fb));
}

void display_setpixel(uint8_t col, uint16_t row, uint8_t value)
{
    uint8_t page = row / PAGE_NUMS;
    uint8_t bit = row & (PAGE_NUMS - 1);

    fb[page * PAGE_WIDTH + col] |= ((value & 1) << bit);
    OLED_DrawBitmap(&oled, fb, sizeof(fb));
}

/* ==== TTY MODE ==== */
static uint8_t tty_col = 0;
static uint8_t tty_line = 0;

static void tty_newline(void)
{
    tty_col = 0;
    tty_line++;
    if (tty_line >= PAGE_NUMS) {
        memmove(fb, fb + PAGE_WIDTH, (PAGE_NUMS - 1) * PAGE_WIDTH);
        memset(fb + (PAGE_NUMS - 1) * PAGE_WIDTH, 0, PAGE_WIDTH);
        tty_line = PAGE_NUMS - 1;
    }
}

void display_tty(const char *s)
{
    while (*s) {
        if (*s == '\n') {
            tty_newline();
            s++;
            continue;
        }
        font_drawchar(fb, *s++, tty_col, tty_line);
        tty_col += CHAR_WIDTH + 1;
        if (tty_col > PAGE_WIDTH - (CHAR_WIDTH + 1)) {
            tty_newline();
        }
    }
    OLED_DrawBitmap(&oled, fb, sizeof(fb));
}

void display_invert(void) { OLED_SetInvert(&oled, 1); }
