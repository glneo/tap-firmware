/*
 * Image demo for the TAP (Technology Access Platform)
 *
 * Copyright (C) 2016 TheLab.ms
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include "main.h"

#define IMAGE_WIDTH 16
#define IMAGE_HEIGHT 16

volatile bool hold_demo = true;
extern unsigned char user_data[IMAGE_WIDTH][IMAGE_WIDTH * COL_SIZE];

void setup()
{
	unsigned int col, row;

	for (row = 0; row < 8; row++)
		for (col = 0; col < 8; col++)
		{
			display_buffer[row][(col * COL_SIZE) + 0] = 3;
			display_buffer[row][(col * COL_SIZE) + 1] = 4;
			display_buffer[row][(col * COL_SIZE) + 2] = 5;
		}
}

void copy_frame()
{
	unsigned int col, row;
	unsigned int col_offset = state.x_address * 8;
	unsigned int row_offset = (1 - state.y_address) * 8;

//	unsigned char (*image_data)[IMAGE_WIDTH * COL_SIZE] = (unsigned char(*)[IMAGE_WIDTH * COL_SIZE])user_data;

	for (row = 0; row < 8; row++)
		for (col = 0; col < 8; col++)
		{
			display_buffer[row][(col * COL_SIZE) + 0] = user_data[row + row_offset][((col + col_offset) * COL_SIZE) + 0];
			display_buffer[row][(col * COL_SIZE) + 1] = user_data[row + row_offset][((col + col_offset) * COL_SIZE) + 1];
			display_buffer[row][(col * COL_SIZE) + 2] = user_data[row + row_offset][((col + col_offset) * COL_SIZE) + 2];
		}
}

void loop(void)
{
	if (hold_demo)
	{
		__delay_cycles(1000); // so transition frame is not longer
		return;
	}

	copy_frame();

	hold_demo = true;
}

/* warning: called from interrupt context */
void handle_custom_packet(packet_t *p)
{
	hold_demo = false;
}
