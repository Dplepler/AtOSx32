#ifndef KERNEL_SCREEN_H
#define KERNEL_SCREEN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "hal.h"

#define VGA_BUFFER_ADDR 0xC03FE000

/* Hardware text mode color constants. */
typedef enum vga_color {

	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GRAY = 7,
	VGA_COLOR_DARK_GRAY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,

} vga_color;

// typedef struct _SCREEN_ATTRIBUTES_STRUCT {

// 	size_t terminal_row;
// 	size_t terminal_column;
// 	uint8_t terminal_color;
// 	uint16_t* terminal_buffer;

// } screen_t;

//screen_t* vga_init(size_t row, size_t column, uint8_t color, uint16_t* buffer);

uint8_t vga_entry_color(vga_color ccolor, vga_color bcolor);
uint16_t vga_entry(unsigned char c, uint8_t color);
size_t strlen(const char* str);

void terminal_initialize();
void terminal_setcolor(vga_color color);
void terminal_putentryat(char c, uint8_t color, const uint8_t x, const uint8_t y);
void terminal_putchar(char c);
void terminal_put_colored_char_at(char c, uint8_t color, const uint8_t x, const uint8_t y);
void terminal_write(const char* data, size_t amount);
void terminal_writestring(const char* data);
void terminal_draw_square(const uint8_t x, const uint8_t y, const uint8_t destx, const uint8_t desty, const vga_color color);
bool terminal_special_chars(char c);

void cursor_enable(uint8_t cursor_start, uint8_t cursor_end);
void cursor_disable();
void cursor_update(int x, int y);
uint16_t cursor_get_position();


#endif