#include "kernel_keyboard.h"

/*
keyboard_ctrl_read_status reads the status of the keyboard controller
Output: Data inside port 0x64
*/
uint16_t keyboard_ctrl_read_status() {
  return inportb(KEYBOARD_CTRL_STATS_REG);
}

/*
keyboard_enc_read_buffer reads the keyboard encoder's buffer data
Output: Data inside port 0x60
*/
uint16_t keyboard_enc_read_buffer() {
  return inportb(KEYBOARD_ENC_INPUT_BUF);
}

/*
keyboard_ctrl_send_cmd sends a command to the keyboard controller
Input: Command to send to port 0x64
*/
void keyboard_ctrl_send_cmd(uint8_t cmd) {
   
  /* Wait for the keyboard controller buffer to be clear */
  while ((keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATS_MASK_IN_BUF) != 0) { }
 
  outportb(KEYBOARD_CTRL_CMD_REG, cmd);  // Send command
}

/*
keyboard_enc_send_cmd sends a command to the keyboard encoder
Input: Command to send to port 0x60
*/
void keyboard_enc_send_cmd(uint8_t cmd) {
 
  /* Wait for the keyboard controller buffer to be clear */

  /* Even though we are writing to the keyboard encoder, we still
  pass through the controller */
  
  while ((keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATS_MASK_IN_BUF) != 0) { }
 
  outportb(KEYBOARD_ENC_CMD_REG, cmd);  // Send command
}


