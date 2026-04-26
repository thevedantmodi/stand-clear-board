#include <pixel.h>
#include <subwayicons.h>

#define W 7
#define H 7
#define FG {0b11, 0b11, 0b11}

Icon_T subway_icons[31] = {
    {0, 0, W, H, {0b11, 0b00, 0b00}, FG, "1"},
    {0, 0, W, H, {0b11, 0b00, 0b00}, FG, "2"},
    [SUB_THREE]  = {0, 0, W, H, {0b11, 0b00, 0b00}, FG, "3"},
    [SUB_FOUR]   = {0, 0, W, H, {0b00, 0b11, 0b00}, FG, "4"},
    [SUB_FIVE]   = {0, 0, W, H, {0b00, 0b11, 0b00}, FG, "5"},
    [SUB_SIX]    = {0, 0, W, H, {0b00, 0b11, 0b00}, FG, "6"},
    [SUB_SIXX]   = {0, 0, W, H, {0b00, 0b11, 0b00}, FG, "6X"},
    [SUB_SEVEN]  = {0, 0, W, H, {0b11, 0b00, 0b11}, FG, "7"},
    [SUB_SEVENX] = {0, 0, W, H, {0b11, 0b00, 0b11}, FG, "7X"},
    [SUB_A]      = {0, 0, W, H, {0b00, 0b00, 0b11}, FG, "A"},
    [SUB_B]      = {0, 0, W, H, {0b11, 0b01, 0b00}, FG, "B"},
    [SUB_C]      = {0, 0, W, H, {0b00, 0b00, 0b11}, FG, "C"},
    [SUB_D]      = {0, 0, W, H, {0b11, 0b01, 0b00}, FG, "D"},
    [SUB_E]      = {0, 0, W, H, {0b00, 0b00, 0b11}, FG, "E"},
    [SUB_F]      = {0, 0, W, H, {0b11, 0b01, 0b00}, FG, "F"},
    [SUB_FX]     = {0, 0, W, H, {0b11, 0b01, 0b00}, FG, "FX"},
    [SUB_G]      = {0, 0, W, H, {0b10, 0b11, 0b01}, FG, "G"},
    [SUB_H]      = {0, 0, W, H, {0b01, 0b01, 0b01}, FG, "H"},
    [SUB_J]      = {0, 0, W, H, {0b10, 0b01, 0b00}, FG, "J"},
    [SUB_L]      = {0, 0, W, H, {0b01, 0b01, 0b01}, FG, "L"},
    [SUB_M]      = {0, 0, W, H, {0b11, 0b01, 0b00}, FG, "M"},
    [SUB_N]      = {0, 0, W, H, {0b11, 0b11, 0b00}, FG, "N"},
    [SUB_Q]      = {0, 0, W, H, {0b11, 0b11, 0b00}, FG, "Q"},
    [SUB_R]      = {0, 0, W, H, {0b11, 0b11, 0b00}, FG, "R"},
    [SUB_S]      = {0, 0, W, H, {0b01, 0b01, 0b01}, FG, "S"},
    [SUB_SF]     = {0, 0, W, H, {0b01, 0b01, 0b01}, FG, "SF"},
    [SUB_SIR]    = {0, 0, W, H, {0b00, 0b00, 0b11}, FG, "SI"},
    [SUB_SR]     = {0, 0, W, H, {0b01, 0b01, 0b01}, FG, "SR"},
    [SUB_T]      = {0, 0, W, H, {0b00, 0b10, 0b01}, FG, "T"},
    [SUB_W]      = {0, 0, W, H, {0b11, 0b11, 0b00}, FG, "W"},
    [SUB_Z]      = {0, 0, W, H, {0b10, 0b01, 0b00}, FG, "Z"},
};

void subwayicons_init(void) {}
