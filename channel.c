/*
 * Communication channel for the TAP (Technology Access Platform)
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

#define	BUFFER_SIZE 128

/**
 * struct channel_s - Structure for each channels byte level communication
 * @buffer: Input buffer
 * @index: Index into input buffer
 * @escaped: Flag if our channel is currently SLIP escaped
 * @output_buffer: Output circular buffer
 * @output_write: Writer index into output circular buffer
 * @output_read: Reader index into output circular buffer
 * @output_full: Flag if our output circular buffer is full
 */
struct channel_s
{
	unsigned char buffer[BUFFER_SIZE];
	unsigned int index;
	bool escaped;
	unsigned char output_buffer[BUFFER_SIZE];
	unsigned int output_write;
	unsigned int output_read;
	bool output_full;
};

channel_t serial_x;
channel_t serial_y;

channel_t x_out;
channel_t y_out;

#ifdef PHOTO_SUPPORT
channel_t photo;
#endif

#ifdef USB_SUPPORT
channel_t usb;
#endif

static void slip_unesc(channel_t *c, unsigned char b)
{
	switch (b)
	{
	case SLIP_END:
		if (c->index > 3)
			process_packet((packet_t *)c->buffer);
		c->index = 0;
		c->escaped = false;
		return;
	case SLIP_ESC:
		c->escaped = true;
		return;
	case SLIP_ESC_END:
		c->escaped = false;
		b = SLIP_END;
		break;
	case SLIP_ESC_ESC:
		c->escaped = false;
		b = SLIP_ESC;
		break;
	}

	c->buffer[c->index++] = b;
}

/**
 * flush_channel() - Empty a channel
 */
void flush_channel(channel_t *c)
{
	c->index = 0;
	c->escaped = false;
	c->output_write = 0;
	c->output_read = 0;
	c->output_full = false;
}

/**
 * put_byte_input() - Insert a byte into the input buffer
 */
int put_byte_input(channel_t *c, unsigned char b)
{
	if (c->index >= BUFFER_SIZE)
		return -1; // Buffer is full

	slip_unesc(c, b);

	return 0;
}

/**
 * put_byte_output() - Insert a byte into the output buffer
 */
int put_byte_output(channel_t *c, unsigned char b)
{
	if (c->output_full)
		return -1; // Buffer is full

	c->output_buffer[c->output_write++] = b;
	c->output_write %= BUFFER_SIZE;

	if (c->output_write == c->output_read) // write head hit read head
		c->output_full = true;

	return 0;
}

/**
 * get_byte_output() - Get a byte from the output buffer
 */
int get_byte_output(channel_t *c, unsigned char *b)
{
	if (c->output_full)
		c->output_full = false;
	else // if we are not full
		if (c->output_read == c->output_write) // make sure we are not empty
			return -1; //Buffer is empty

	*b = c->output_buffer[c->output_read++];
	c->output_read %= BUFFER_SIZE;

	if (c->output_read == c->output_write)
		return -1;

	return 0;
}
