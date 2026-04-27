#ifndef __DISPLAY__LINE__H
#define __DISPLAY__LINE__H

#include <stdint.h>
#include <subwayid.h>

typedef enum {
    LINE0,
    LINE1,
    LINE2,
    LINE3,
} DisplayLineId_T;

void displayline_arrival(SubwayId_T subway_id, const char *station,
                         uint16_t seconds_left, uint8_t display_line,
                         uint16_t flash_tick);

#endif
