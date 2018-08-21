/* display.c
 * Functions to control the Nextion Touch Display
 * Made by: René Lüllau
*/

#include "display.h"

void display_send_cmd(char *cmd, uint16_t size)
{
	char *cmd_cmplt = malloc(size + 3);
    if (cmd_cmplt == NULL)
    	return;
    memcpy(cmd_cmplt, cmd, size);
    memset(cmd_cmplt + size, 0xff, 3);

	HAL_UART_Transmit(&huart3, (uint8_t*)cmd_cmplt, size + 3, 10);

	free(cmd_cmplt);

}

void display_show_page(uint8_t page_id)
{
	char *cmd = malloc(8);
	if (cmd == NULL)
		return;

	sprintf(cmd, "page %i", page_id);
	display_send_cmd(cmd, strlen(cmd));
	free(cmd);
}


void display_ts_set_line(uint8_t line, char *text, uint16_t length)
{
	uint16_t cmd_length = 11 + length + 4;
	char *cmd = malloc(cmd_length);
	if (cmd == NULL)
		return;

	sprintf(cmd, "line%i.txt=\"%s\"", line, text);
	display_send_cmd(cmd, strlen(cmd));
	free(cmd);

}
