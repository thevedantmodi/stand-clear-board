#include <stdint.h>
#include <ssd1306.h>
#include <stdbool.h>
#include <stm32l432xx.h>

/* The SSD1306 library needs a function of OLED_SendI2CFn type to use to send */
int32_t i2c_send(void *ctx, uint8_t addr7, const uint8_t *data, size_t len)
{
    (void)ctx;
    bool ok = i2c_write(I2C1, addr7, (unsigned char *)data, (unsigned char)len);
    return ok ? OLED_OK : OLED_ERR_IO;
}