#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
 
/* Check if the compiler thinks you are targeting the wrong operating system */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* Can only compile for 32-bit ix86 targets. */
#if !defined(__i386__)
#error "You need an ix86-elf compiler to build AtOS"
#endif
 
/* Hardware text mode color constants. */
enum vga_color {
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
};
 
/*
vga_entry_color generates a byte containing both given colors
Input: Character color, background color
Output: A byte, where the most significant half is the background color
and the least significant half is the character color
*/
static inline uint8_t vga_entry_color(enum vga_color ccolor, enum vga_color bcolor) {
	return ccolor | bcolor << 4;
}
 
/*
vga_entry generates a word containing a character and it's color attribute
Input: Character, color attribute of character
Output: A word, where the most significant half is the color attribute
and the least significant half is the character
*/
static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
	return (uint16_t) c | (uint16_t) color << 8;
}
 
/*
strlen returns the length of a given char array
Input: Character array
Output: Length of character array
*/
size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len]) {
		len++;
	}

	return len;
}
 
/* Constants */
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize() {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_DARK_GRAY);
	terminal_buffer = (uint16_t*) 0xB8000;

	/* Cleaning the screen and setting background color */
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
 /*
 terminal_setcolor sets the color of the terminal
 Input: Color to set the terminal with
 Output: None
 */
void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}
 
/*
terminal_putentryat puts a character in a given position on the screen
Input: Character to put, it's color, coords on screen
Output: None
*/
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

/*
terminal_putchar puts a character at the current position on the screen
Intput: Character to put
Output: None
*/
void terminal_putchar(char c) {
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

	// Reset indexes if we're out of bounds
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) {
			terminal_row = 0;
		}
	}
}
 
/*
terminal_write prints a given amount of characters from a character array
to the current index on the screen

Input: Character array to write, amount to write
Output: None
*/
void terminal_write(const char* data, size_t amount) {
	for (size_t i = 0; i < amount; i++) {
		terminal_putchar(data[i]);
	}
}

/*
terminal_writestring prints a character array to the screen
Input: Character array to print
Output: None
*/
void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}
 
int kmain(void) {
	/* Initialize terminal interface */
	terminal_initialize();
 
	/* Newline support is left as an exercise. */
	terminal_writestring("Hello, kernel World!\n");

	return 0;
}