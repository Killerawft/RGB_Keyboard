/* keyboard.h
 * Functions to control the Keyboard
 * Made by: René Lüllau
*/

#ifndef KEYBOARD_HEADER
#define KEYBOARD_HEADER

#include "usb_device.h"
#include "gpio.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "usb_hid_keycodes.h"
#include "keyboard_layout.h"


// HID Keyboard
 struct keyboardHID_t {
	  uint8_t id;
	  uint8_t modifiers;
	  uint8_t key1;
	  uint8_t key2;
	  uint8_t key3;
 };
 struct keyboardHID_t keyboardHID;

 // HID Media
 struct mediaHID_t {
	uint8_t id;
	uint8_t keys;
 };
 struct mediaHID_t mediaHID;


void keyboard_init(void);
void init_keypad(void);
GPIO_PinState read_key(uint16_t row_pin, uint16_t col_pin);
void check_pressed_keys(void);

#endif
