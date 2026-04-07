#ifndef __DISPLAY__H
#define __DISPLAY__H
#include <font.h>
#include <stdint.h>

extern uint8_t fb[1024];

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

void display_init(void);

/* Sets the pixel at (col,row) to value. 
 * Prints an error message when extending past the bounds
 */
void display_setpixel(uint8_t col, uint16_t row, uint8_t value);

/* Draws a string s onto the fb
 * Starts from the given col and line
 *
 * Recommended to start col on a multiple of 6
 *
 * col must be in [0,122]
 * line must be in [0,7]
 *
 * DOES perform text wrapping
 * WILL truncate message if cannot fit on screen
 * 
 * DOES NOT flush to the display
 */
void display_write(const char *s, uint8_t col, uint8_t line);

/* Exactly like display_write, but persists the frame buffer between calls. */
void display_append(const char *s, uint8_t col, uint8_t line);

/* Appends a string to the display like a TTY terminal.
 * Text continues from the current cursor position.
 * Wraps to the next line at 21 characters.
 * Scrolls up when the last line is full.
 * Supports '\n' to force a new line.
 * Does NOT clear the framebuffer after flushing.
 */
void display_tty(const char *s);

/* updates the display to be consistent with fb. 
 * try to call this function as little as possible */
void display_flush(void);
void display_invert(void);
void display_clear(void);

#endif // __DISPLAY__H
