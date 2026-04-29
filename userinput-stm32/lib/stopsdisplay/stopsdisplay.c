#include <display.h>
#include <stdbool.h>
#include <stdio.h>
#include <stops.h>
#include <stopsdisplay.h>
#include <string.h>

#define STOPS_VISIBLE 6
#define CHARS_PER_LINE (DISPLAY_WIDTH / (CHAR_WIDTH + 1))

void stopdisplay_page(const uint16_t *filtered, uint16_t filtered_count,
                      uint16_t filter_cursor, const char *filter_buf)
{
    char buf[24] = {0};

    display_clear();

    /* Row 0: header */
    snprintf(buf, sizeof(buf) + 14, "Select stop (%u)", filtered_count);
    display_write(buf, 0, 0);

    /* Rows 1-6: filtered stop list with scroll window */
    uint16_t scroll = 0;
    if (filter_cursor >= STOPS_VISIBLE)
        scroll = filter_cursor - STOPS_VISIBLE + 1;

    for (uint8_t i = 0; i < STOPS_VISIBLE; i++) {
        uint16_t fi = scroll + i;
        if (fi >= filtered_count) break;

        uint16_t stop_idx = filtered[fi];
        char cur = (fi == filter_cursor) ? '>' : ' ';
        const char *name = stops[stop_idx];

        /* truncate name to fit: 1 (cursor) + 1 (space) + name */
        int max_name = CHARS_PER_LINE - 2;
        char name_buf[22];
        if ((int)strlen(name) > max_name) {
            int n = max_name - 3;
            if (n < 0) n = 0;
            strncpy(name_buf, name, (size_t)n);
            strcpy(name_buf + n, "...");
            name = name_buf;
        }

        snprintf(buf, sizeof(buf), "%c %s", cur, name);
        display_write(buf, 0, i + 1);
    }

    /* Row 7: search field */
    snprintf(buf, sizeof(buf), "> %s", filter_buf);
    display_write(buf, 0, 7);

    display_flush();
}
