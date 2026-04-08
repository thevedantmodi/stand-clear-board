#ifndef __TRANSMITTER__H
#define __TRANSMITTER__H

#include <stops.h>
#include <stdint.h>

void transmitter_init(void);
int transmitter_sendselections(uint32_t lines_selected,
                                uint64_t *stops_per_line);

#endif // __TRANSMITTER__H