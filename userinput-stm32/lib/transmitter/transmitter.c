#include <ee14lib.h>
#include <stdio.h>
#include <transmitter.h>

#define BITS_IN_BYTE 8
const uint8_t bookend_byte = 0xFF;

void transmitter_init(void)
{
    /* NEEDSWORK: for now we will use the serial_write with UART2 connections.
        change to UART1 for final product, since UART2 should be serial monitor
     */
    // host_serial_init(USART2, 9600);
}

void transmitter_send(const char *buf, int len)
{
    serial_write(USART2, buf, len);
}

/* send stop_idx (2 bytes BE) then lines_selected (4 bytes BE) */
int transmitter_sendselections(uint16_t stop_idx, uint32_t lines_selected)
{
    uint8_t buf[6];

    buf[0] = (stop_idx >> 8) & 0xFF;
    buf[1] = (stop_idx     ) & 0xFF;
    buf[2] = (lines_selected >> 24) & 0xFF;
    buf[3] = (lines_selected >> 16) & 0xFF;
    buf[4] = (lines_selected >>  8) & 0xFF;
    buf[5] = (lines_selected      ) & 0xFF;

    transmitter_send(&bookend_byte, 1);
    transmitter_send(buf, 6);
    transmitter_send(&bookend_byte, 1);

    return 0;
}