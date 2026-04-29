/* UART serial functions
 * www.ece.tufts.edu/ee/14
 * Steven Bell <sbell@ece.tufts.edu>, based almost entirely on work from Joel
 * Grodstein March 2026
 */
#include "ee14lib.h"
#include <buffer.h>
#include <subwayselector.h>

/**
 * Configure either USART with the "normal" defaults:
 * Set for 8 data bits, 1 start & 1 stop bit, 16x oversampling
 * And by default, we also get no parity, no hardware flow control (USART_CR3),
 * asynch mode (USART_CR2).
 *
 * :param USARTx: Pointer to the CMSIS USART struct, either USART1 or USART2
 * :param tx_en: True to enable TX channel
 * :param rx_en: True to enable RX channel
 * :param baud: Baud rate to use
 */
static void USART_Init(USART_TypeDef *USARTx, bool tx_en, bool rx_en, int baud)
{
    // Disable the USART.
    USARTx->CR1 &= ~USART_CR1_UE;

    // The "M" field is two bits, M1 and M0. We're setting it to 00 (which
    // is the reset value anyway), to use 8-bit words and one start bit.
    USARTx->CR1 &= ~USART_CR1_M;

    // Configure stop bits to 1 stop bit (which is the default). Other
    // choices are .5, 1.5 and 2 stop bits.
    USARTx->CR2 &= ~USART_CR2_STOP;

    // Set baudrate as desired. This is done by dividing down the APB1 clock.
    // E.g., 80MHz/9600 = 8333 = 0x208D.
    // (We're oversampling by 16; the calculation would be slightly
    // different if we were 8x mode).
    extern uint32_t SystemCoreClock;
    uint32_t val = SystemCoreClock / baud;
    USARTx->BRR = val;

    // Configure oversampling mode: Oversampling by 16 (which is the
    // default). This means that our Rx runs at 16x the nominal baud rate.
    // If we're not enabling the Rx anyway, this step is moot (but harmless).
    USARTx->CR1 &= ~USART_CR1_OVER8;

    // Turn on transmitter and receiver enables. Note that the entire USART
    // is still disabled, though. Turning on the Rx enable kicks off the Rx
    // looking for a stop bit.
    if (tx_en)
        USARTx->CR1 |= USART_CR1_TE;
    if (rx_en)
        USARTx->CR1 |= USART_CR1_RE;

    // We originally turned off the USART -- now turn it back on.
    // Note that page 1202 says to turn this on *before* asserting TE and/or RE.
    USARTx->CR1 |= USART_CR1_UE; // USART enable

    // Verify that the USART is ready to transmit...
    if (tx_en)
        while ((USARTx->ISR & USART_ISR_TEACK) == 0)
            ;
    // ... and to receive.
    if (rx_en)
        while ((USARTx->ISR & USART_ISR_REACK) == 0)
            ;
}

/**
 * Configure the serial connection to the host PC.
 * This will always be on USART 2 and GPIO pins A7 (PA2) and PA15, since those
 * are the wired connections to the host bridge chip on the Nucleo PCB.
 *
 * :param baud: The baud rate, typically 9600 or 115200.
 */
void host_serial_init(USART_TypeDef *USARTx, const unsigned int baud)
{
    if (USARTx == USART2) {
        // Enable USART 2 clock
        RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

        // Select SYSCLK as the USART2 clock source. The reset default is PCLK1;
        // we usually set both SYSCLK and PCLK1 to 80MHz anyway.
        RCC->CCIPR &= ~RCC_CCIPR_USART2SEL;
        RCC->CCIPR |= RCC_CCIPR_USART2SEL_0;

        // Connect the I/O pins to the serial peripheral
        // UART2_GPIO_Init();
        gpio_config_alternate_function(A7, 7);     // PA 2, AF7
        gpio_config_alternate_function(VCP_RX, 3); // PA 15, AF3

        USART_Init(USART2, 1, 1, baud); // Enable both Tx and Rx sides.
        USART2->CR1 |= USART_CR1_RXNEIE;
    } else {
        // Enable USART1 clock (on APB2)
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

        // Select SYSCLK as USART1 clock source
        RCC->CCIPR &= ~RCC_CCIPR_USART1SEL;
        RCC->CCIPR |= RCC_CCIPR_USART1SEL_0;

        // D1=PA9 TX AF7, D0=PA10 RX AF7
        gpio_config_alternate_function(D1, 7);
        gpio_config_alternate_function(D0, 7);

        USART_Init(USART1, 1, 1, baud);
        USART1->CR1 |= USART_CR1_RXNEIE;
        gpio_config_pullup(D0, PULL_UP);
    }
}

/**
 * Write a buffer to the serial port.  This function blocks until the
 * transmission is complete.
 *
 * :param USARTx: Pointer to the CMSIS USART struct, either USART1 or USART2
 * :param buffer: Pointer to array of bytes to send
 * :param len: Number of bytes to transmit
 */
void serial_write(USART_TypeDef *USARTx, const char *buffer, int len)
{
    // The main flag we use is Tx Empty (TXE). The HW sets it when the
    // transmit data register (TDR) is ready for more data. TXE is then
    // cleared when we write new data in (by a write to the USART_DR reg).
    // When the HW transfers the TDR into the shift register, it sets TXE=1.
    for (unsigned int i = 0; i < len; i++) {
        // Wait until the "Transmit data register empty" flag is set
        while (!(USARTx->ISR & USART_ISR_TXE)) {
        }

        // Put the next byte into the buffer; this will automatically clear the
        // TXE flag
        USARTx->TDR = buffer[i];
    }

    // RM0394 page 1203 says that you must wait for ISR.TC=1 before you shut
    // off the USART. We never shut off the USART... but we'll wait anyway.
    while (!(USARTx->ISR & USART_ISR_TC))
        ;
    USARTx->ISR &= ~USART_ISR_TC;
}

/**
 * Read a byte from the serial port.  This blocks until a byte comes in and
 * then returns that value.
 * Wouldn't it be nice if this were buffered and asynchronous too?
 *
 * :param USARTx: Pointer to the CMSIS USART struct, either USART1 or USART2
 *
 * :return: Returns a byte read from the serial port.
 */
char serial_read(USART_TypeDef *USARTx)
{
    // The SR_RXNE (Read data register not empty) bit is set by hardware.
    // We spin wait until that bit is set
    while (!(USARTx->ISR & USART_ISR_RXNE)) {
    }

    // Reading USART_DR automatically clears the RXNE flag
    return ((char)(USARTx->RDR & 0xFF));
}

/**
 * Write a buffer to the serial port.  Returns quickly, and pushes bytes to
 * the serial peripheral asynchronously.
 *
 * :param USARTx: Pointer to the CMSIS USART struct, either USART1 or USART2
 * :param buffer: Pointer to array of bytes to send
 * :param len: Number of bytes to transmit
 *
 * :return: The number of bytes successfully pushed to the UART TX buffer.  This
 * will equal `len` if the entire buffer was written.
 *
 * For the purposes of Lab 8, it's ok if you only implement this for UART2.
 * A more general solution would need multiple circular buffers and select
 * between them based on the value of USARTx.
 */
int serial_write_nonblocking(USART_TypeDef *USARTx, const char *buffer, int len)
{
    int i;
    int written = 0;

    for (i = 0; i < len; i++) {
        if (Buffer_push(buffer[i]) == EOF) {
            break;
        }

        written++;
    }
    // enable TXIE for USARTx -- indicates we are ready to start placing bytes
    // on the wire.
    USARTx->CR1 |= USART_CR1_TXEIE;

    return written;
}

extern uint8_t UART1_buffer[NUM_ARRIVALS * ARRIVAL_BYTES];
extern uint8_t UART1_buffer_len;

volatile uint8_t uart_rx_count = 0;
volatile uint8_t uart_isr_count = 0;

/* Interrupt handler for USART2: handles TX and RX */
// void USART2_IRQHandler(void)
// {
//     uart_isr_count++;
//     if (USART2->ISR & USART_ISR_TXE) {
//         if (Buffer_empty()) {
//             USART2->CR1 &= ~USART_CR1_TXEIE;
//         } else {
//             USART2->TDR = Buffer_pop();
//         }
//     }
//     // if (USART2->ISR & USART_ISR_RXNE) {
//     //     UART1_buffer[UART1_buffer_len++] = (uint8_t)(USART2->RDR & 0xFF);
//     //     uart_rx_count++;
//     //     if (UART1_buffer_len == (NUM_ARRIVALS * ARRIVAL_BYTES)) {
//     //         UART1_buffer_len = 0;
//     //         subwayselector_update();
//     //     }
//     // }
// }

volatile uint8_t packet_ready = 0;

void USART1_IRQHandler(void)
{
    if (USART1->ISR & USART_ISR_RXNE) {
        UART1_buffer[UART1_buffer_len++] = (uint8_t)(USART1->RDR & 0xFF);
        if (UART1_buffer_len == (NUM_ARRIVALS * ARRIVAL_BYTES)) {
            UART1_buffer_len = 0;
            packet_ready = 1;
        }
    }
}

// This function is called by printf() to handle the text string
// We want it to be sent over the serial terminal, so we just delegate to
// that function
int _write(int file, char *data, int len)
{
    // int written = 0;
    // /* keep writing until we have written all the bytes */
    // while (written < len) {
    //     /* only write the unwritten bytes */
    //     written +=
    //         serial_write_nonblocking(USART2, data + written, len - written);
    // }
    // return len;

    serial_write(USART2, data, len);

    return len;
}
