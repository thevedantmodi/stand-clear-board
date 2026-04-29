#ifndef __STOPS__DISPLAY__PAGE
#define __STOPS__DISPLAY__PAGE
#include <stdint.h>

void stopdisplay_page(const uint16_t *filtered, uint16_t filtered_count,
                      uint16_t filter_cursor, const char *filter_buf);

#endif // __STOPS__DISPLAY__PAGE
