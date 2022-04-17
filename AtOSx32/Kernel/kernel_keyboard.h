#ifndef KERNEL_KEYBOARD_H
#define KERNEL_KEYBOARD_H

#include <stdbool.h>
#include "hal.h"

/*
Keyboard encoder is accessible through port 0x60
*/
enum KEYBOARD_ENCODER_IO {
  KEYBOARD_ENC_INPUT_BUF	=	0x60,
  KEYBOARD_ENC_CMD_REG	=	0x60
};
 
/*
Keyboard controller is accessible through port 0x64
*/
enum KEYBOARD_CTRL_IO {
  KEYBOARD_CTRL_STATS_REG =	0x64,
  KEYBOARD_CTRL_CMD_REG   =	0x64
};

/*
Status register flag locations
*/
enum KEYBOARD_CTRL_STATS_MASK {

  KEYBOARD_CTRL_STATS_MASK_OUT_BUF	=	1,		//00000001
  KEYBOARD_CTRL_STATS_MASK_IN_BUF		=	2,		//00000010
  KEYBOARD_CTRL_STATS_MASK_SYSTEM		=	4,		//00000100
  KEYBOARD_CTRL_STATS_MASK_CMD_DATA	=	8,		//00001000
  KEYBOARD_CTRL_STATS_MASK_LOCKED		=	0x10,	//00010000
  KEYBOARD_CTRL_STATS_MASK_AUX_BUF	=	0x20,	//00100000
  KEYBOARD_CTRL_STATS_MASK_TIMEOUT	=	0x40,	//01000000
  KEYBOARD_CTRL_STATS_MASK_PARITY		=	0x80	//10000000
};

uint16_t keyboard_ctrl_read_status();
uint16_t keyboard_enc_read_buffer();
void keyboard_ctrl_send_cmd(uint8_t cmd);
void keyboard_enc_send_cmd(uint8_t cmd);

#endif