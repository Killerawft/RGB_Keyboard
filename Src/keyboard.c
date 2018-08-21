/* keyboard.c
 * Functions to control the Keyboard
 * Made by: René Lüllau
*/

#include "keyboard.h"



/*
 * Keyboard Tasks
 */

void keyboard( void *pvParameters )
{
	for( ;; )
	{
		keyboard_check_pressed_keys();
		HAL_Delay(50);
	}

	/* Tasks must not attempt to return from their implementing
	function or otherwise exit.  In newer FreeRTOS port
	attempting to do so will result in an configASSERT() being
	called if it is defined.  If it is necessary for a task to
	exit then have the task call vTaskDelete( NULL ) to ensure
	its exit is clean. */
	vTaskDelete( NULL );
}


void keyboard_init(void)
{
	SET_ROW(ROW_0);
	SET_ROW(ROW_1);
}


GPIO_PinState keyboard_read_key(uint16_t row_pin, uint16_t col_pin)
{
	RESET_ROW(row_pin);
	GPIO_PinState col_state = HAL_GPIO_ReadPin(COL_GPIO, col_pin);
	SET_ROW(row_pin);

	return !col_state;
}


void keyboard_check_pressed_keys(void)
{
  // HID Keyboard Init
  keyboardHID.id = 1;
  keyboardHID.modifiers = 0;
  keyboardHID.key1 = 0;
  keyboardHID.key2 = 0;
  keyboardHID.key3 = 0;

  // HID Media Init
  mediaHID.id = 2;
  mediaHID.keys = 0;

	for (uint8_t row = 0; row < ROWS_CNT; row++)
	{
		for (uint8_t col = 0; col < COLUMS_CNT; col++)
		{
			if(read_key(layout_rows_pins[row], layout_columns_pins[col]))
			{
				if(keyboardHID.modifiers == 0 && layout_keycodes[row][col] == 2) //USB_HID_KEY_LEFTSHIFT)
				{
					keyboardHID.modifiers = layout_keycodes[row][col];
				}
				else if (keyboardHID.key1 == 0)
				{
					keyboardHID.key1 = layout_keycodes[row][col];
				}
				else if (keyboardHID.key2 == 0)
				{
					keyboardHID.key2 = layout_keycodes[row][col];
				}
				else if (keyboardHID.key3 == 0)
				{
					keyboardHID.key3 = layout_keycodes[row][col];
				}else if(keyboardHID.key1 != 0 && keyboardHID.key2 != 0 && keyboardHID.key3 != 0)
				{
					return;
				}
			}
		}
	}


	if (keyboardHID.key1 != 0 || keyboardHID.key2 != 0 || keyboardHID.key3 != 0)
	{
		 USBD_HID_SendReport(&hUsbDeviceFS, &keyboardHID, sizeof(struct keyboardHID_t));
		 HAL_Delay(30);
		 keyboardHID.modifiers = 0;
		 keyboardHID.key1 = 0;
		 keyboardHID.key2 = 0;
		 keyboardHID.key3 = 0;
		 USBD_HID_SendReport(&hUsbDeviceFS, &keyboardHID, sizeof(struct keyboardHID_t));
		 HAL_Delay(30);
	}

}
