#include "subwayselector.h"
#include <displayarrival.h>
#include <ee14lib.h>
#include <pixel.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <subwayicons.h>
#include <systick.h>

#define NUM_ARRIVALS 4
#define ARRIVAL_BYTES 3

extern Pixel_T frameBuffer[PIXELS_COUNT];

typedef struct {
    uint8_t minutes_remaining;
    SubwayId_T line_id;
    const char *direction;
} ArrivalInfo_T;

/* serial_read will spin wait, so we do not need to implement a traditional
 * state machine...the system only works in one way */
/* 0xAA is a predefined start code */
static bool receive_ready_check() { return (serial_read(USART2) == 0xAA); }

/* Reads arrival info from serial according to the following:
BYTE 0: minute
BYTE 1: line index
BYTE 2: direction (0 or 1)

reads the values into buffer, and returns the length written
*/
static int read_arrival_info(uint8_t *buffer)
{
    int i;
    for (i = 0; i < ARRIVAL_BYTES; i++) {
        buffer[i] = serial_read(USART2);
    }

    return i;
}

static ArrivalInfo_T parse_arrival_info(uint8_t *buffer, int len)
{
    uint8_t minutes_byte = buffer[0];
    uint8_t line_index_byte = buffer[1];
    uint8_t direction_byte = buffer[2];

    return (ArrivalInfo_T){
        .minutes_remaining = minutes_byte,
        .line_id = (SubwayId_T)line_index_byte,
        .direction = direction_byte ? "Downtown" : "Uptown",
    };
}

// assume that UART2 is active with 9600 baud
void subwayselector_listen()
{
    static ArrivalInfo_T arrival_infos[NUM_ARRIVALS] = {0};

    // spin until ready
    receive_ready_check();

    for (int arrival_pos = 0; arrival_pos < NUM_ARRIVALS; arrival_pos++) {
        uint8_t arrival_buffer[ARRIVAL_BYTES];
        int arrival_buffer_len = read_arrival_info(arrival_buffer);
        arrival_infos[arrival_pos] =
            parse_arrival_info(arrival_buffer, arrival_buffer_len);
    }

    uint16_t flash_tick = 0;
    while (1) {
        memset(frameBuffer, 0, sizeof(frameBuffer));

        for (int arrival_pos = 0; arrival_pos < NUM_ARRIVALS; arrival_pos++) {
            displayline_arrival(arrival_infos[arrival_pos].line_id,
                                arrival_infos[arrival_pos].direction,
                                arrival_infos[arrival_pos].minutes_remaining,
                                arrival_infos[arrival_pos].line_id, flash_tick);
        }

        HUB75E_setDisplayBuffer(frameBuffer);
        for (size_t j = 0; j < 100; j++) {
            HUB75E_displayBufferPixels();
            delay_ms(1);
        }
        flash_tick++;
    }
}