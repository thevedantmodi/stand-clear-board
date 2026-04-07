#ifndef __STOPS__H
#define __STOPS__H

#include <stdint.h>

extern const char *const stops_line_1[];
extern const uint8_t stops_line_1_count;
extern const char *const stops_line_2[];
extern const uint8_t stops_line_2_count;
extern const char *const stops_line_3[];
extern const uint8_t stops_line_3_count;
extern const char *const stops_line_4[];
extern const uint8_t stops_line_4_count;
extern const char *const stops_line_5[];
extern const uint8_t stops_line_5_count;
extern const char *const stops_line_6[];
extern const uint8_t stops_line_6_count;
extern const char *const stops_line_6X[];
extern const uint8_t stops_line_6X_count;
extern const char *const stops_line_7[];
extern const uint8_t stops_line_7_count;
extern const char *const stops_line_7X[];
extern const uint8_t stops_line_7X_count;
extern const char *const stops_A[];
extern const uint8_t stops_A_count;
extern const char *const stops_B[];
extern const uint8_t stops_B_count;
extern const char *const stops_C[];
extern const uint8_t stops_C_count;
extern const char *const stops_D[];
extern const uint8_t stops_D_count;
extern const char *const stops_E[];
extern const uint8_t stops_E_count;
extern const char *const stops_F[];
extern const uint8_t stops_F_count;
extern const char *const stops_FS[];
extern const uint8_t stops_FS_count;
extern const char *const stops_FX[];
extern const uint8_t stops_FX_count;
extern const char *const stops_G[];
extern const uint8_t stops_G_count;
extern const char *const stops_GS[];
extern const uint8_t stops_GS_count;
extern const char *const stops_H[];
extern const uint8_t stops_H_count;
extern const char *const stops_J[];
extern const uint8_t stops_J_count;
extern const char *const stops_L[];
extern const uint8_t stops_L_count;
extern const char *const stops_M[];
extern const uint8_t stops_M_count;
extern const char *const stops_N[];
extern const uint8_t stops_N_count;
extern const char *const stops_Q[];
extern const uint8_t stops_Q_count;
extern const char *const stops_R[];
extern const uint8_t stops_R_count;
extern const char *const stops_SI[];
extern const uint8_t stops_SI_count;
extern const char *const stops_W[];
extern const uint8_t stops_W_count;
extern const char *const stops_Z[];
extern const uint8_t stops_Z_count;

typedef enum {
    LINE_1,
    LINE_2,
    LINE_3,
    LINE_4,
    LINE_5,
    LINE_6,
    LINE_7,
    LINE_A,
    LINE_B,
    LINE_C,
    LINE_D,
    LINE_E,
    LINE_F,
    LINE_G,
    LINE_H,
    LINE_J,
    LINE_L,
    LINE_M,
    LINE_N,
    LINE_Q,
    LINE_R,
    LINE_W,
    LINE_Z,
    LINE_6X,
    LINE_7X,
    LINE_FS,
    LINE_FX,
    LINE_GS,
    LINE_SI,

} subway_line_t;

typedef struct {
    const char *route_id;
    const char *const *stops;
    uint8_t stop_count;
} subway_route_t;

extern const uint8_t subway_route_count;
extern const subway_route_t subway_routes[];

#endif // __STOPS__H
