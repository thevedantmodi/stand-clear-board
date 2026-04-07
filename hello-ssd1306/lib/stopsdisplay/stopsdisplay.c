#include <cursor.h>
#include <display.h>
#include <progbar.h>
#include <stdbool.h>
#include <stdio.h>
#include <stops.h>
#include <stopsdisplay.h>
#include <string.h>

#define STOPS_VISIBLE 7 /* Scroll otherwise */
#define CHARS_PER_LINE (DISPLAY_WIDTH / (CHAR_WIDTH + 1))
#define STOP_PREFIX 2     // "[ "
#define STOP_SUFFIX_SEL 1 // "]"
#define ELLIPSIS_LEN 3    // "..."

#define MAX_NAME_SEL (CHARS_PER_LINE - STOP_PREFIX - STOP_SUFFIX_SEL)
#define MAX_NAME_UNSEL (CHARS_PER_LINE - STOP_PREFIX)
#define TRUNC_NAME_SEL (MAX_NAME_SEL - ELLIPSIS_LEN)
#define TRUNC_NAME_UNSEL (MAX_NAME_UNSEL - ELLIPSIS_LEN)

void stopdisplay_page(uint8_t subway_route, uint64_t stops_selected)
{
    subway_route_t rt_info = subway_routes[subway_route];
    char buf[24] = {0};

    display_clear();

    /* PRINT HEADER */
    snprintf(buf, sizeof(buf), "[ %c ]", rt_info.route_id[0]);
    display_write(buf, 0, 0);

    uint8_t selected_count = 0;
    for (uint8_t i = 0; i < rt_info.stop_count; i++) {
        if (get_option(i, stops_selected))
            selected_count++;
    }
    snprintf(buf, sizeof(buf), "[ %d/%d ]", selected_count, rt_info.stop_count);
    display_write(buf, DISPLAY_WIDTH - 56, 0);

    /* PRINT BODY */

    /* creates sliding window of stops  */
    uint8_t scroll = 0;
    if (cursor_pos >= STOPS_VISIBLE) {
        scroll = cursor_pos - STOPS_VISIBLE + 1;
    }

    for (uint8_t i = 0; i < STOPS_VISIBLE; i++) {
        uint8_t stop_idx = scroll + i;
        /* looping over the stops visible, so break if we go out of bounds in the stops */
        if (stop_idx >= rt_info.stop_count) {
            break;
        }

        bool is_selected = get_option(stop_idx, stops_selected);
        char cur = (stop_idx == cursor_pos) ? '>' : ' ';
        const char *name = rt_info.stops[stop_idx];
        int max_name = is_selected ? MAX_NAME_SEL : MAX_NAME_UNSEL;

        // make a new string with ... at the end if the name is too long
        char name_buf[MAX_NAME_UNSEL + 1];
        if ((int)strlen(name) > max_name) {
            int n = max_name - ELLIPSIS_LEN;
            strncpy(name_buf, name, n);
            strcpy(name_buf + n, "...");
            name = name_buf;
        }

        if (is_selected) {
            snprintf(buf, sizeof(buf), "%c[%s]", cur, name);
        } else {
            snprintf(buf, sizeof(buf), "%c %s", cur, name);
        }

        /* plus 1 since the 0th line is first */
        display_write(buf, 0, i + 1);
    }

    /* PRINT FOOTER */

    float scrolled_ratio = (float)cursor_pos / (rt_info.stop_count);
    uint8_t bar_height = 4;
    uint8_t bar_width = 32;
    draw_progbar(DISPLAY_WIDTH - bar_width, DISPLAY_HEIGHT - bar_height,
                 bar_width, bar_height, (uint8_t)(scrolled_ratio * 100));

    display_flush();
}
