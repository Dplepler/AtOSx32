#include "kernel_keyboard.h"



/*
keyboard_ctrl_read_status reads the status of the keyboard controller
Input: None
Output: Data inside port 0x64
*/
uint8_t keyboard_ctrl_read_status() {
 
	return inportb(KEYBOARD_CTRL_STATS_REG);
}

/*
keyboard_enc_read_buffer reads the keyboard encoder's buffer data
Input: None
Output: Data inside port 0x60
*/
uint8_t keyboard_enc_read_buffer() {
 
	return inportb(KEYBOARD_ENC_INPUT_BUF);
}

/*
keyboard_ctrl_send_cmd sends a command to the keyboard controller
Input: Command to send to port 0x64
Output: None
*/
void keyboard_ctrl_send_cmd(uint8_t cmd) {
 	
 	// Wait for the keyboard controller buffer to be clear
	while ((keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATS_MASK_IN_BUF) != 0) { }
 
 	// Send command
	outportb(KEYBOARD_CTRL_CMD_REG, cmd);
}

/*
keyboard_enc_send_cmd sends a command to the keyboard encoder
Input: Command to send to port 0x60
Output: None
*/
void keyboard_enc_send_cmd(uint8_t cmd) {
 
	// Wait for the keyboard controller buffer to be clear

	// Even though we are writing to the keyboard encoder, we still
	// pass through the controller
	while ((keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATS_MASK_IN_BUF) != 0) { }
 
	// Send command
	outportb(KEYBOARD_ENC_CMD_REG, cmd);
}

void keyboard_set_scroll_led() {

	keyboard_enc_send_cmd(0xED);
	keyboard_enc_send_cmd(keyboard_enc_read_buffer() | 1);
}

void keyboard_set_num_led() {

	keyboard_enc_send_cmd(0xED);
	keyboard_enc_send_cmd(keyboard_enc_read_buffer() | 2);
}

/*
keyboard_set_caps_led turns on the capslock LED on the keyboard
Input: None
Output: None
*/
void keyboard_set_caps_led(bool flag) {

	uint8_t data = 0b00000111;

	keyboard_enc_send_cmd(0xED);
	keyboard_enc_send_cmd(data);		
}

