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

static uint8_t popcount(uint32_t word)
{
    if (word == 0) {
        return 0;
    } else {
        return (word & 1) + popcount(word >> 1);
    }
}

/* sizeof(word) == 8 */
static void send_word_be(uint64_t word)
{
    uint8_t buf[8] = {0};
    for (uint8_t i = 0; i < 8; i++) {
        buf[i] = (word >> (56 - (i * BITS_IN_BYTE))) & 0xFF;
    }
    transmitter_send(buf, 8);
}

/* assumes lines_selected is an integer where
   bit i == on iff the line with that number (defined in subway_line_t) is
   selected

   assumes stops_per_line is an array of uint64_ts where
     stops_per_line[i] contains a uint64_t where
     bit i == on iff the stop of the given line (defined in subway_route_t)
     is selected
*/
int transmitter_sendselections(uint32_t lines_selected,
                               uint64_t *stops_per_line)
{
    uint8_t num_lines_selected = popcount(lines_selected);

    /* START */
    transmitter_send(&bookend_byte, 1);

    /* SEND NUMBER OF LINES */
    serial_write(USART2, &num_lines_selected, 1);

    /* SEND THE STATIONS PER LINE */
    for (uint8_t line = 0; line < sizeof(lines_selected) * BITS_IN_BYTE;
         line++) {
        if ((lines_selected >> line) & 1) {
            uint8_t line_byte = (uint8_t)line;
            transmitter_send(&line_byte, 1);

            uint64_t stations = stops_per_line[line];
            send_word_be(stations);
        }
    }

    /* NEEDSWORK: send an XOR checksum here */

    /* STOP */
    transmitter_send(&bookend_byte, 1);

    return 0;
}