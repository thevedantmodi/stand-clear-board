#ifndef __TRANSMITTER__H
#define __TRANSMITTER__H

#include <stops.h>
#include <stdint.h>

void transmitter_init(void);
int transmitter_sendselections(uint16_t stop_idx, uint16_t lines_selected);

#endif // __TRANSMITTER__H