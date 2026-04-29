#ifndef __STOPS__H
#define __STOPS__H

#include <stdint.h>
#define NUM_ROUTES 29

#define MAX_STOPS 360

extern const char *const stops[];
extern const char stops_to_lines[][11];
extern const uint16_t stops_count;

#endif // __STOPS__H
