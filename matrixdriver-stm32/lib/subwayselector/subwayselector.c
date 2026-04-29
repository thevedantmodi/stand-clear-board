#include "subwayselector.h"
#include <displayarrival.h>
#include <ee14lib.h>
#include <pixel.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <subwayicons.h>
#include <systick.h>

extern Pixel_T frameBuffer[PIXELS_COUNT];
extern uint8_t UART1_buffer[NUM_ARRIVALS * ARRIVAL_BYTES];
extern uint8_t UART1_buffer_len;

typedef struct {
    uint8_t minutes_remaining;
    SubwayId_T line_id;
    const char *direction;
} ArrivalInfo_T;

ArrivalInfo_T arrival_infos[NUM_ARRIVALS] = {
    [0] = {.minutes_remaining = 3, .direction = "uptown", .line_id = SUB_A},
    [1] = {.minutes_remaining = 3,
           .direction = "downtown",
           .line_id = SUB_FIVE},
    [2] = {.minutes_remaining = 3,
           .direction = "flushing",
           .line_id = SUB_SEVEN},
    [3] = {.minutes_remaining = 3, .direction = "uptown", .line_id = SUB_J},
};

static ArrivalInfo_T parse_arrival_info(uint8_t *buffer, int len)
{
    // BYTE 0: minute
    // BYTE 1: line index
    // BYTE 2: direction (0 or 1)

    uint8_t minutes_byte = buffer[0];
    uint8_t line_index_byte = buffer[1];
    uint8_t direction_byte = buffer[2];

    return (ArrivalInfo_T){
        .minutes_remaining = minutes_byte,
        .line_id = (SubwayId_T)line_index_byte,
        .direction = direction_byte ? "Downtown" : "Uptown",
    };
}

void subwayselector_update()
{
    for (int arrival_pos = 0; arrival_pos < NUM_ARRIVALS; arrival_pos++) {
        arrival_infos[arrival_pos] = parse_arrival_info(
            UART1_buffer + (arrival_pos * ARRIVAL_BYTES), ARRIVAL_BYTES);
    }
}

extern volatile uint8_t uart_rx_count;

void subwayselector_display()
{
    uint16_t flash_tick = 0;
    while (1) {
        memset(frameBuffer, 0, sizeof(frameBuffer));
        uint32_t enable = NVIC_GetEnableIRQ(USART2_IRQn);
        NVIC_DisableIRQ(USART2_IRQn);
        for (int arrival_pos = 0; arrival_pos < NUM_ARRIVALS; arrival_pos++) {
            if (arrival_infos[arrival_pos].minutes_remaining == ARRIVAL_INVALID)
                continue;
            displayline_arrival(arrival_infos[arrival_pos].line_id,
                                arrival_infos[arrival_pos].direction,
                                arrival_infos[arrival_pos].minutes_remaining,
                                arrival_pos, flash_tick);
        }
        if (enable) {
            NVIC_EnableIRQ(USART2_IRQn);
        }
        HUB75E_setDisplayBuffer(frameBuffer);
        HUB75E_displayBufferPixels();

        if (uart_rx_count > 1) {
            printf("rx=%d\n", uart_rx_count);
        }
        flash_tick++;
    }
}