/*
 * HUB75E_Hal.c
 *
 *  Created on: May 28, 2020
 *      Author: raeeskattali
 */
#include "HUB75E_Hal.h"
#include <ee14lib.h>
#include <stm32l432xx.h>
#include <systick.h>

// IO Pins & Ports
#define LED_MATRIX_B2_Pin D2
#define LED_MATRIX_G2_Pin D3
#define LED_MATRIX_B1_Pin D4
#define LED_MATRIX_R1_Pin D5
#define LED_MATRIX_R2_Pin D6
#define LED_MATRIX_G1_Pin D9
#define LED_MATRIX_LATCH_Pin D10
#define LED_MATRIX_ADDRESS_LINE_C_Pin D11
#define LED_MATRIX_CLK_Pin D13
#define LED_MATRIX_ADDRESS_LINE_A_Pin A6
#define LED_MATRIX_ADDRESS_LINE_B_Pin A3
// #define LED_MATRIX_ADDRESS_LINE_E_Pin A2 not used on 64x32 display
#define LED_MATRIX_ADDRESS_LINE_D_Pin A1
#define LED_MATRIX_OE_Pin A0

void HUB75E_GPIO_Init(void)
{
    gpio_config_mode(LED_MATRIX_B2_Pin, OUTPUT);
    gpio_config_mode(LED_MATRIX_G2_Pin, OUTPUT);
    gpio_config_mode(LED_MATRIX_B1_Pin, OUTPUT);
    gpio_config_mode(LED_MATRIX_R1_Pin, OUTPUT);
    gpio_config_mode(LED_MATRIX_R2_Pin, OUTPUT);
    gpio_config_mode(LED_MATRIX_G1_Pin, OUTPUT);
    gpio_config_mode(LED_MATRIX_LATCH_Pin, OUTPUT);
    gpio_config_mode(LED_MATRIX_ADDRESS_LINE_C_Pin, OUTPUT);
    gpio_config_mode(LED_MATRIX_CLK_Pin, OUTPUT);
    gpio_config_mode(LED_MATRIX_ADDRESS_LINE_A_Pin, OUTPUT);
    gpio_config_mode(LED_MATRIX_ADDRESS_LINE_B_Pin, OUTPUT);
    // gpio_config_mode(LED_MATRIX_ADDRESS_LINE_E_Pin, OUTPUT);
    gpio_config_mode(LED_MATRIX_ADDRESS_LINE_D_Pin, OUTPUT);
    gpio_config_mode(LED_MATRIX_OE_Pin, OUTPUT);

    gpio_config_otype(LED_MATRIX_B2_Pin, PUSH_PULL);
    gpio_config_otype(LED_MATRIX_G2_Pin, PUSH_PULL);
    gpio_config_otype(LED_MATRIX_B1_Pin, PUSH_PULL);
    gpio_config_otype(LED_MATRIX_R1_Pin, PUSH_PULL);
    gpio_config_otype(LED_MATRIX_R2_Pin, PUSH_PULL);
    gpio_config_otype(LED_MATRIX_G1_Pin, PUSH_PULL);
    gpio_config_otype(LED_MATRIX_LATCH_Pin, PUSH_PULL);
    gpio_config_otype(LED_MATRIX_ADDRESS_LINE_C_Pin, PUSH_PULL);
    gpio_config_otype(LED_MATRIX_CLK_Pin, PUSH_PULL);
    gpio_config_otype(LED_MATRIX_ADDRESS_LINE_A_Pin, PUSH_PULL);
    gpio_config_otype(LED_MATRIX_ADDRESS_LINE_B_Pin, PUSH_PULL);
    // gpio_config_otype(LED_MATRIX_ADDRESS_LINE_E_Pin, PUSH_PULL);
    gpio_config_otype(LED_MATRIX_ADDRESS_LINE_D_Pin, PUSH_PULL);
    gpio_config_otype(LED_MATRIX_OE_Pin, PUSH_PULL);
}

void HUB75E_DelayUs(int us)
{
    volatile int count = us;
    while (count--)
        ;
}

void HUB75E_setPin(HUB75EPin p, int state)
{

    switch (p) {
    case PinRed1:
        gpio_write(LED_MATRIX_R1_Pin, state);
        break;
    case PinRed2:
        gpio_write(LED_MATRIX_R2_Pin, state);
        break;
    case PinGreen1:
        gpio_write(LED_MATRIX_G1_Pin, state);
        break;
    case PinGreen2:
        gpio_write(LED_MATRIX_G2_Pin, state);
        break;
    case PinBlue1:
        gpio_write(LED_MATRIX_B1_Pin, state);
        break;
    case PinBlue2:
        gpio_write(LED_MATRIX_B2_Pin, state);
        break;
    case PinADDRA:
        gpio_write(LED_MATRIX_ADDRESS_LINE_A_Pin, state);
        break;
    case PinADDRB:
        gpio_write(LED_MATRIX_ADDRESS_LINE_B_Pin, state);
        break;
    case PinADDRC:
        gpio_write(LED_MATRIX_ADDRESS_LINE_C_Pin, state);
        break;
    case PinADDRD:
        gpio_write(LED_MATRIX_ADDRESS_LINE_D_Pin, state);
        break;
    // case PinADDRE:
    //     gpio_write(LED_MATRIX_ADDRESS_LINE_E_Pin, state);
    //     break;
    case PinCLK:
        gpio_write(LED_MATRIX_CLK_Pin, state);
        break;
    case PinLATCH:
        gpio_write(LED_MATRIX_LATCH_Pin, state);
        break;
    case PinOE:
        gpio_write(LED_MATRIX_OE_Pin, state);
        break;
    default:
        break;
    }
}
