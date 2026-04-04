#include <stm32l432xx.h>
#include <ee14lib.h>

// This function is called by printf() to handle the text string
// We want it to be sent over the serial terminal, so we just delegate to
// that function
int _write(int file, char *data, int len)
{
    serial_write(USART2, data, len);
    return len;
}