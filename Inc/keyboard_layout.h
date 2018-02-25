/* keyboard_layout.h
 * Keycode Layout of the keyboard
 * Made by: René Lüllau
*/

#ifndef KEYBOARD_LAYOUT
#define KEYBOARD_LAYOUT

#define ROWS_CNT    2
#define COLUMS_CNT  4

// Keyboard Pin Defines
#define ROW_GPIO		GPIOC
#define COL_GPIO		GPIOA
#define ROW_0			GPIO_PIN_0
#define ROW_1			GPIO_PIN_1
#define COL_0			GPIO_PIN_3
#define COL_1			GPIO_PIN_2
#define COL_2			GPIO_PIN_1
#define COL_3			GPIO_PIN_0
#define SET_ROW(PIN)	HAL_GPIO_WritePin(ROW_GPIO, PIN, GPIO_PIN_SET)
#define RESET_ROW(PIN)	HAL_GPIO_WritePin(ROW_GPIO, PIN, GPIO_PIN_RESET)

__ALIGN_BEGIN static uint8_t layout_keycodes[ROWS_CNT][COLUMS_CNT] = {
	{USB_HID_KEY_E, USB_HID_KEY_W, USB_HID_KEY_Q},
	{USB_HID_KEY_D, USB_HID_KEY_S, USB_HID_KEY_A, 2} //USB_HID_KEY_LEFTSHIFT}
};

__ALIGN_BEGIN static uint8_t layout_rows_pins[ROWS_CNT] = {
	ROW_0, ROW_1
};

__ALIGN_BEGIN static uint8_t layout_columns_pins[COLUMS_CNT] = {
	COL_0, COL_1, COL_2, COL_3
};


#endif //KEYBOARD_LAYOUT
