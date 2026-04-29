#include <ee14lib.h>
#include <stdio.h>
#include <stops.h>
#include <transmitter.h>

#define BITS_IN_BYTE 8

void transmitter_init(void)
{
    /* NEEDSWORK: for now we will use the serial_write with UART2 connections.
        change to UART1 for final product, since UART2 should be serial monitor
     */
    // host_serial_init(USART2, 9600);
}

void transmitter_send(const char *buf, int len)
{
    serial_write(USART1, buf, len);
}

/* send stop_idx (2 bytes BE) then up to 4 selected line chars, 0-padded */
int transmitter_sendselections(uint16_t stop_idx, uint16_t lines_selected)
{
    uint8_t buf[6] = {0};

    buf[0] = (stop_idx >> 8) & 0xFF;
    buf[1] = stop_idx & 0xFF;

    uint8_t out = 2;
    for (uint8_t i = 0; i < 11 && out < 6; i++) {
        if ((lines_selected >> i) & 1) {
            buf[out++] = (uint8_t)stops_to_lines[stop_idx][i];
        }
    }

    transmitter_send(buf, 6);

    return 0;
}