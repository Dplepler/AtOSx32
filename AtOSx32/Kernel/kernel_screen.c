#include "kernel_screen.h"

// screen_t* vga_init(size_t row, size_t column, uint8_t color, uint16_t* buffer) {



// }

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;
 
/*
vga_entry_color generates a byte containing both given colors
Input: Character color, background color
Output: A byte, where the most significant half is the background color
and the least significant half is the character color
*/
uint8_t vga_entry_color(vga_color ccolor, vga_color bcolor) {
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
	while (str[len]) { len++; }

	return len;
}
 
void terminal_initialize() {

	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_DARK_GRAY);

	terminal_buffer = (uint16_t*) 0xC03FF000;	// Virtual address of buffer (Physical address is 0xB8000)

	/* Cleaning the screen and setting background color */
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}

	update_cursor(0, 0); 	// Set cursor position to the start of the screen
}

/*
enable_cursor turns on the cursor in text mode with a given size
Input: Start of cursor, end of cursor
Output: None
*/
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {

	/* Prepare and write beginning of cursor data */
	outportb(0x3D4, 0x0A); 
	outportb(0x3D5, (inportb(0x3D5) & 0xC0) | cursor_start); 	
 
	/* Prepare and write end of cursor data */
	outportb(0x3D4, 0x0B);
	outportb(0x3D5, (inportb(0x3D5) & 0xE0) | cursor_end);
}

/*
disable_cursor disables the current cursor in text mode
Input: None
Output: None
*/
void disable_cursor() {

	outportb(0x3D4, 0x0A);
	outportb(0x3D5, 0x20);
}

/*
update_cursor updates the coordinates of the cursor with a given new position
Input: Desired x coordinate of cursor, desired y coordinate of cursor
Output: None
*/
void update_cursor(int x, int y) {

	uint16_t pos = y * VGA_WIDTH + x; 	// Calculate desired position with the linear data
 
 	/* Input new y position */
	outportb(0x3D4, 0x0F);
	outportb(0x3D5, (uint8_t)(pos & 0xFF));

	/* Input new x position */
	outportb(0x3D4, 0x0E);
	outportb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}
 
/*
get_cursor_position gets the current cursor coordinates
Input: None
Output: 2 byte position (x, y)
*/
uint16_t get_cursor_position() {

    uint16_t pos = 0;

    /* Get y position */
    outportb(0x3D4, 0x0F);
    pos |= inportb(0x3D5);

    /* Get x position */
    outportb(0x3D4, 0x0E);
    pos |= ((uint16_t)inportb(0x3D5)) << 8;

    return pos;
}


/*
terminal_setcolor sets the color of the terminal
Input: Color to set the terminal with
Output: None
*/
void terminal_setcolor(vga_color color) {
	terminal_color = color;
}
 
/*
terminal_putentryat puts a character in a given position on the screen
Input: Character to put, it's color, coords on screen
Output: None
*/
void terminal_putentryat(char c, uint8_t color, const uint8_t x, const uint8_t y) {

	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, vga_entry_color(color, terminal_color));
}

/*
terminal_putchar puts a character at the current position on the screen
Intput: Character to put
Output: None
*/
void terminal_putchar(char c) {

	terminal_putentryat(c, VGA_COLOR_WHITE, terminal_column, terminal_row);

	// Reset indexes if we're out of bounds
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) {
			terminal_row = 0;
		}
	}
}

void terminal_put_colored_char_at(char c, uint8_t color, const uint8_t x, const uint8_t y) {

	uint8_t tcolor = terminal_color;

	terminal_setcolor(color);
	terminal_putentryat(c, terminal_color, x, y);
	terminal_setcolor(tcolor);
}
 
/*
terminal_write prints a given amount of characters from a character array
to the current index on the screen

Input: Character array to write, amount to write
*/
void terminal_write(const char* data, size_t amount) {

	terminal_putchar(data[0]);

	for (size_t i = 0; i < amount; i++) {
		terminal_putchar(data[i]);
	}
}

/*
terminal_writestring prints a character array to the screen
Input: Character array to print
*/
void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

void terminal_draw_square(const uint8_t x, const uint8_t y, const uint8_t destx, const uint8_t desty, const vga_color color) {

	for (uint8_t i = y; i < y + desty; i++) {
		for (uint8_t i2 = x; i2 < x + destx; i2++) {
			terminal_put_colored_char_at('p', vga_entry_color(color, color), x + i2, y + i);
		}
	}

}