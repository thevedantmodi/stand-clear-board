#ifndef __SUBWAY__ICONS
#define __SUBWAY__ICONS

#include <icon.h>

typedef enum {
    SUB_ONE,
    SUB_TWO,
    SUB_THREE,
    SUB_FOUR,
    SUB_FIVE,
    SUB_SIX,
    SUB_SIXX,
    SUB_SEVEN,
    SUB_SEVENX,
    SUB_A,
    SUB_B,
    SUB_C,
    SUB_D,
    SUB_E,
    SUB_F,
    SUB_FX,
    SUB_G,
    SUB_H,
    SUB_J,
    SUB_L,
    SUB_M,
    SUB_N,
    SUB_Q,
    SUB_R,
    SUB_S,
    SUB_SF,
    SUB_SIR,
    SUB_SR,
    SUB_T,
    SUB_W,
    SUB_Z,
} SubwayID_T;

#define RED ((Pixel_T){0b11, 0b00, 0b00})
#define GREEN ((Pixel_T){0b00, 0b11, 0b00})
#define PURPLE ((Pixel_T){0b11, 0b00, 0b11})
#define BLUE ((Pixel_T){0b00, 0b00, 0b11})
#define ORANGE ((Pixel_T){0b11, 0b01, 0b00})
#define LIMEGREEN ()

extern Icon_T subway_icons[31];

void subwayicons_init(void);

#endif // __SUBWAY__ICONS