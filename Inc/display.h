/* display.h
 * Header file for functions to controll the Nextion Touch Display
 * Made by: René Lüllau
*/

#ifndef DISPLAY_HEADER
#define DISPLAY_HEADER

#include "main.h"
#include "stm32f4xx_hal.h"
#include "usart.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Commonly used functions */
void display_send_cmd(char *cmd, uint16_t size);
void display_show_page(uint8_t page_id);

/* Functions to control he TeamSpeak Page */
void display_ts_set_line(uint8_t line, char *text, uint16_t length);




#endif
