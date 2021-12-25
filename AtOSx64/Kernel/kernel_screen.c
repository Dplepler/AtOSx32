#include "kernel_screen.h"
 
/*
vga_entry_color generates a byte containing both given colors
Input: Character color, background color
Output: A byte, where the most significant half is the background color
and the least significant half is the character color
*/
uint8_t vga_entry_color(enum vga_color ccolor, enum vga_color bcolor) {
	return ccolor | bcolor << 4;
}
 
/*
vga_entry generates a word containing a character and it's color attribute
Input: Character, color attribute of character
Output: A word, where the most significant half is the color attribute
and the least significant half is the character
*/
uint16_t vga_entry(unsigned char c, uint8_t color) {
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