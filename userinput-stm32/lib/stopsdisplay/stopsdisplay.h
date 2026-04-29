#ifndef __STOPS__DISPLAY__PAGE
#define __STOPS__DISPLAY__PAGE
#include <stdint.h>

/* takes in the subway line, current selection, and filter state */
void stopdisplay_page(uint8_t subway_route, uint64_t stops_selected,
                      const uint8_t *filtered, uint8_t filtered_count,
                      uint8_t filter_cursor, const char *filter_buf);

#endif // __STOPS__DISPLAY__PAGE