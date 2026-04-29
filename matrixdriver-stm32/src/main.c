#include "HUB75ELib.h"
#include "systick.h"
#include <displayarrival.h>
#include <ee14lib.h>
#include <font.h>
#include <icon.h>
#include <pixel.h>
#include <speedup.h>
#include <stdio.h>
#include <stdlib.h>
#include <stm32l432xx.h>
#include <subwayicons.h>
#include <subwayselector.h>

Pixel_T frameBuffer[PIXELS_COUNT];

uint8_t UART1_buffer[NUM_ARRIVALS * ARRIVAL_BYTES];
uint8_t UART1_buffer_len;

extern volatile uint8_t uart_rx_count;
extern volatile uint8_t uart_isr_count;
extern volatile uint8_t packet_ready;

int main(void)
{

    speedup();
    host_serial_init(USART2, 9600);
    host_serial_init(USART1, 9600);
    SysTick_initialize();

    // NVIC_SetPriority(USART2_IRQn, 2);
    // NVIC_EnableIRQ(USART2_IRQn);

    NVIC_SetPriority(USART1_IRQn, 2);
    NVIC_EnableIRQ(USART1_IRQn);

    HUB75E_Init();
    HUB75E_setDisplayBrightness(BrightnessLevel5);
    HUB75E_setAddressingMode(HUB75EAddressingModeABCDE);

    delay_ms(1000);

    while (1) {
        if (packet_ready) {
            NVIC_DisableIRQ(USART1_IRQn);
            packet_ready = 0;
            for (int i = 0; i < NUM_ARRIVALS * ARRIVAL_BYTES; i++) {
                printf("0x%x ", UART1_buffer[i]);
            }
            printf("\n");
            subwayselector_update();
            NVIC_EnableIRQ(USART1_IRQn);
        }
        
        subwayselector_display();

        delay_ms(100);
    }
}
