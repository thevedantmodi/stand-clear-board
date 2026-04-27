#include <displayarrival.h>
#include <font.h>
#include <subwayicons.h>

extern Pixel_T frameBuffer[PIXELS_COUNT];

// Build a "Nm" string without snprintf. n must be 0-99.
static void fmt_minutes(char *buf, uint16_t n)
{
    if (n >= 10) {
        buf[0] = '>'; buf[1] = '9'; buf[2] = 'm'; buf[3] = '\0';
    } else {
        buf[0] = '0' + n; buf[1] = 'm'; buf[2] = '\0';
    }
}

void displayline_arrival(SubwayId_T subway_id, const char *station,
                         uint16_t seconds_left, uint8_t display_line,
                         uint16_t flash_tick)
{
    uint8_t row = display_line * SUBWAY_ICON_HEIGHT;

    icon_draw(frameBuffer, subway_icons + subway_id, 0, row);

    font_drawstr_color(frameBuffer, station, SUBWAY_ICON_WIDTH + 2,
                       row + 1, PX_WHITE);

    uint16_t minutes = seconds_left / 60;

    char minutes_str[4];
    fmt_minutes(minutes_str, minutes);

    uint8_t mins_col = (minutes >= 10)
        ? HORIZONTAL_PIXELS - 8 - CHAR_WIDTH - 1
        : HORIZONTAL_PIXELS - 8;

    Pixel_T min_color = (minutes < 1 && (flash_tick / 2) & 1)
        ? PX_RED : PX_WHITE;

    font_drawstr_color(frameBuffer, minutes_str, mins_col, row + 1, min_color);
}
