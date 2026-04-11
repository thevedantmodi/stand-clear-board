#ifndef FONT_H
#define FONT_H

#include <stdint.h>


/* does not include padding byte */
#define CHAR_WIDTH 3

/*
 * 5x7 font bitmap table for SSD1306 page-oriented display.
 *
 * Handles ASCII characters from 0x20 to 0x7E
 *
 * Each character is 5 bytes wide. Each byte represents
 * one fifth of the width of the character's visual representation.
 *
 * Within each byte, the bits are interpreted in descending order
 * (i.e. bit 0 is the bit for the top-most pixel). Each bit represents
 * one eighth of the height of the character's visual representation.
 *
 * The bottom-most pixel (bit 7) is always 0 to account for spacing.
 *
 * Works well for the SSD1306 OLED (128px x 64px) --
 * pixels are grouped into pages (128px x 8px blocks) on the display
 *
 * The height of each character's visual representation
 * is (conveniently) 8 pixels s.t. character height == page height
 *
 */
extern const uint8_t font3x5[][3];
extern const uint8_t font5x7[][5];


/* Draws a character c into the framebuffer fb.
 * Starts from the given col and line
 *
 * Adds a byte of padding (i.e., a space)
 * Recommended to have col be a multiple of 6 for even spacing
 */
void font_drawchar(uint8_t *fb, char c, uint8_t col, uint8_t line);

/* Draws a string s into the framebuffer fb.
 * Starts from the given col and line
 *
 * Adds a byte of padding (i.e., a space) per char
 * Recommended to have col be a multiple of 6 for even spacing
 *  (think about it like struct padding)
 *
 * col must be in [0,122]
 * line must be in [0,7]
 * 
 * DOES perform text wrapping
 * WILL truncate message if cannot fit on screen
 */
void font_drawstr(uint8_t *fb, const char *s, uint8_t col, uint8_t line);



#endif /* FONT_H */
