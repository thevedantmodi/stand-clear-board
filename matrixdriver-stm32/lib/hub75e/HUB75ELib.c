/*
 * HUB75ELib.c
 *
 *  Created on: May 28, 2020
 *      Author: raeeskattali
 */

#include "HUB75ELib.h"
#include "HUB75E_Hal.h"
#include <string.h>

// Per-pixel color framebuffer. Each Pixel_T holds 2-bit BCM intensity per
// channel.
Pixel_T graphicsBuffer[PIXELS_COUNT];

HUB75EDisplayBrightnessLevel brightness;
HUB75EAddressingMode addressingMode;

void HUB75E_Init(void)
{
    HUB75E_GPIO_Init();

    HUB75E_setPin(PinCLK, 0);

    HUB75E_setPin(PinRed1, 0);
    HUB75E_setPin(PinRed2, 0);
    HUB75E_setPin(PinGreen1, 0);
    HUB75E_setPin(PinGreen2, 0);
    HUB75E_setPin(PinBlue1, 0);
    HUB75E_setPin(PinBlue2, 0);

    HUB75E_setPin(PinADDRA, 0);
    HUB75E_setPin(PinADDRB, 0);
    HUB75E_setPin(PinADDRC, 0);
    HUB75E_setPin(PinADDRD, 0);
}

void HUB75E_clearDisplayBuffer(void)
{
    memset(graphicsBuffer, 0, sizeof(graphicsBuffer));
}

void HUB75E_setDisplayBrightness(HUB75EDisplayBrightnessLevel b)
{
    brightness = b;
}

void HUB75E_setDisplayBuffer(Pixel_T pixels[])
{
    HUB75E_DisplayOFF();
    memcpy(graphicsBuffer, pixels, sizeof(graphicsBuffer));
}

void HUB75E_DisplayOFF(void) { HUB75E_setPin(PinOE, 1); }

void HUB75E_setAddressingMode(HUB75EAddressingMode addrMode)
{
    addressingMode = addrMode;
}

/*
 * 2-plane BCM refresh. Plane 0 held for 1 unit, plane 1 for 2 units.
 * Each pixel's Pixel_T fields are per-channel BCM intensity (0–3).
 * Upper half (rows 0–15) driven on R1/G1/B1, lower half (rows 16–31) on
 * R2/G2/B2.
 */
void HUB75E_displayBufferPixels(void)
{
    for (int plane = 0; plane < 2; plane++) {
        for (int row = 0; row < VERTICAL_PIXELS / 2;) {
            for (int col = 0; col < HORIZONTAL_PIXELS; col++) {
                Pixel_T px1 = graphicsBuffer[row * HORIZONTAL_PIXELS + col];
                Pixel_T px2 = graphicsBuffer[(row + VERTICAL_PIXELS / 2) *
                                                 HORIZONTAL_PIXELS +
                                             col];

                HUB75E_setPin(PinRed1, (px1.red >> plane) & 1);
                HUB75E_setPin(PinRed2, (px2.red >> plane) & 1);
                HUB75E_setPin(PinGreen1, (px1.green >> plane) & 1);
                HUB75E_setPin(PinGreen2, (px2.green >> plane) & 1);
                HUB75E_setPin(PinBlue1, (px1.blue >> plane) & 1);
                HUB75E_setPin(PinBlue2, (px2.blue >> plane) & 1);

                HUB75E_setPin(PinCLK, 1);
                HUB75E_setPin(PinCLK, 0);
            }

            switch (addressingMode) {
            case HUB75EAddressingModeABCDE:
                HUB75E_setPin(PinADDRA, row & 1);
                HUB75E_setPin(PinADDRB, (row >> 1) & 1);
                HUB75E_setPin(PinADDRC, (row >> 2) & 1);
                HUB75E_setPin(PinADDRD, (row >> 3) & 1);
                HUB75E_setPin(PinADDRE, (row >> 4) & 1);
                break;
            case HUB75EAddressingModeAC:
                HUB75E_setPin(PinADDRC, row == 0 ? 1 : 0);
                HUB75E_setPin(PinADDRA, 1);
                HUB75E_DelayUs(1);
                HUB75E_setPin(PinADDRA, 0);
                break;
            default:
                break;
            }

            HUB75E_setPin(PinLATCH, 1);
            row++;
            HUB75E_setPin(PinLATCH, 0);

            int delay_units = 1 << plane;
            HUB75E_setPin(PinOE, 0);
            switch (brightness) {
            case BrightnessLevel1:
                HUB75E_DelayUs(1 * delay_units);
                break;
            case BrightnessLevel2:
                HUB75E_DelayUs(10 * delay_units);
                break;
            case BrightnessLevel3:
                HUB75E_DelayUs(50 * delay_units);
                break;
            case BrightnessLevel4:
                HUB75E_DelayUs(150 * delay_units);
                break;
            case BrightnessLevel5:
                HUB75E_DelayUs(250 * delay_units);
                break;
            default:
                HUB75E_DelayUs(50 * delay_units);
                break;
            }
            HUB75E_setPin(PinOE, 1);
        }
    }
}