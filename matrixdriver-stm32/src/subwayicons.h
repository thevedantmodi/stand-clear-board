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

const Icon_T subway_icons[] = {
    {7, 7, PX_RED, PX_WHITE, "1"},    {7, 7, PX_RED, PX_WHITE, "2"},
    {7, 7, PX_RED, PX_WHITE, "3"},    {7, 7, PX_GREEN, PX_WHITE, "4"},
    {7, 7, PX_GREEN, PX_WHITE, "5"},  {7, 7, PX_GREEN, PX_WHITE, "6"},
    {7, 7, PX_GREEN, PX_WHITE, "6"},  {7, 7, PX_PURPLE, PX_WHITE, "7"},
    {7, 7, PX_PURPLE, PX_WHITE, "7"}, {7, 7, PX_BLUE, PX_WHITE, "A"},
    {7, 7, PX_ORANGE, PX_BLACK, "B"}, {7, 7, PX_BLUE, PX_WHITE, "C"},
    {7, 7, PX_ORANGE, PX_BLACK, "D"}, {7, 7, PX_BLUE, PX_WHITE, "E"},
    {7, 7, PX_ORANGE, PX_BLACK, "F"}, {7, 7, PX_ORANGE, PX_BLACK, "F"},
    {7, 7, PX_LGREEN, PX_WHITE, "G"}, {7, 7, PX_BLUE, PX_WHITE, "H"},
    {7, 7, PX_BROWN, PX_WHITE, "J"},  {7, 7, PX_GRAY, PX_WHITE, "L"},
    {7, 7, PX_ORANGE, PX_BLACK, "M"}, {7, 7, PX_YELLOW, PX_BLACK, "N"},
    {7, 7, PX_YELLOW, PX_BLACK, "Q"}, {7, 7, PX_YELLOW, PX_BLACK, "R"},
    {7, 7, PX_GRAY, PX_WHITE, "S"},   {7, 7, PX_BLUE, PX_WHITE, "S"},
    {7, 7, PX_BLUE, PX_WHITE, "S"},   {7, 7, PX_BLUE, PX_WHITE, "S"},
    {7, 7, PX_BLUE, PX_WHITE, "T"},   {7, 7, PX_YELLOW, PX_BLACK, "W"},
    {7, 7, PX_BROWN, PX_WHITE, "Z"},
};

#endif