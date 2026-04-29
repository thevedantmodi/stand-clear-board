#include <cursor.h>
#include <display.h>
#include <stdbool.h>
#include <stdio.h>
#include <stops.h>
#include <stopsdisplay.h>
#include <string.h>

#define STOPS_VISIBLE 6
#define CHARS_PER_LINE (DISPLAY_WIDTH / (CHAR_WIDTH + 1))

void stopdisplay_page(uint8_t subway_route, uint64_t stops_selected,
                      const uint8_t *filtered, uint8_t filtered_count,
                      uint8_t filter_cursor, const char *filter_buf)
{
    subway_route_t rt_info = subway_routes[subway_route];
    char buf[24] = {0};

    display_clear();

    /* Row 0: header */
    snprintf(buf, sizeof(buf), "[ %c ]", rt_info.route_id[0]);
    display_write(buf, 0, 0);

    uint8_t selected_count = 0;
    for (uint8_t i = 0; i < rt_info.stop_count; i++) {
        if (get_option(i, stops_selected))
            selected_count++;
    }
    snprintf(buf, sizeof(buf), "%u sel", selected_count);
    display_write(buf, DISPLAY_WIDTH - 36, 0);

    /* Rows 1–6: filtered stop list with scroll window */
    uint8_t scroll = 0;
    if (filter_cursor >= STOPS_VISIBLE)
        scroll = filter_cursor - STOPS_VISIBLE + 1;

    for (uint8_t i = 0; i < STOPS_VISIBLE; i++) {
        uint8_t fi = scroll + i;
        if (fi >= filtered_count) break;

        uint8_t stop_idx = filtered[fi];
        bool is_selected = get_option(stop_idx, stops_selected);
        char cur = (fi == filter_cursor) ? '>' : ' ';
        const char *name = rt_info.stops[stop_idx];

        /* max displayable name chars: 1 (cursor) + 1 (space or '[') + name [+ ']'] */
        int max_name = CHARS_PER_LINE - 2 - (is_selected ? 1 : 0);
        char name_buf[22];
        if ((int)strlen(name) > max_name) {
            int n = max_name - 3;
            if (n < 0) n = 0;
            strncpy(name_buf, name, (size_t)n);
            strcpy(name_buf + n, "...");
            name = name_buf;
        }

        if (is_selected)
            snprintf(buf, sizeof(buf), "%c[%s]", cur, name);
        else
            snprintf(buf, sizeof(buf), "%c %s", cur, name);

        display_write(buf, 0, i + 1);
    }

    /* Row 7: search field */
    snprintf(buf, sizeof(buf), ">%s", filter_buf);
    display_write(buf, 0, 7);

    display_flush();
}
