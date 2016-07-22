/*
 * Packet handling for the TAP (Technology Access Platform)
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

#define USER_DATA_SIZE 1024
unsigned char user_data[USER_DATA_SIZE];
unsigned int user_data_index = 0;

/**
 * packet_CRC() - Calculates the CRC of the first packet in a channel
 */
unsigned int packet_CRC(channel_t *c)
{
//	unsigned int i;
//	unsigned int size;
//	unsigned int chunk_1_size;
//	unsigned int chunk_2_size;
//	unsigned char *chunk_1_address;
//	unsigned char *chunk_2_address;
//
//	// Seed the CRC
//	CRCINIRES = 0x1021;
//
//	// Compute the CRC
//	for (i = 0; i < chunk_1_size; i++)
//		CRCDI = *chunk_1_address++;
//	for (i = 0; i < chunk_2_size; i++)
//		CRCDI = *chunk_2_address++;

	return CRCINIRES;
}

/**
 * put_byte_output_slip() - Send a byte with slip encapsulation
 */
static void put_byte_output_slip(channel_t *c, unsigned char b)
{
	switch (b)
	{
	case SLIP_END:
		put_byte_output(c, SLIP_ESC);
		put_byte_output(c, SLIP_ESC_END);
		break;
	case SLIP_ESC:
		put_byte_output(c, SLIP_ESC);
		put_byte_output(c, SLIP_ESC_ESC);
		break;
	default:
		put_byte_output(c, b);
		break;
	}
}

/**
 * send_packet() - Send a packet using a channel
 */
void send_packet(channel_t *c, packet_t *p)
{
	int x;

	put_byte_output(c, SLIP_END);

	put_byte_output_slip(c, p->header);
	put_byte_output_slip(c, p->address_x);
	put_byte_output_slip(c, p->address_y);

	put_byte_output_slip(c, p->size);
	for (x = 0; x < p->size; x++)
		put_byte_output_slip(c, p->data[x]);

	put_byte_output(c, SLIP_END);

	tickle_serial_interrupt(c);
}

/**
 * forward_packet() - Forward a packet through both x and y output channels
 * @p:  Pointer to the packet to forward
 */
void forward_packet(packet_t *p)
{
	if (state.y_address == 0) // only the bottom row propagates messages over
		send_packet(&x_out, p);
	send_packet(&y_out, p);
}

/**
 * process_packet() - Process a data packet
 * @c:  Pointer to the packet to process
 *
 * Main packet handler. Called from the channel layer when a valid
 * packet has been received. This checks the packet type and performs
 * the needed action.
 */
void process_packet(packet_t *p)
{
	if (!(p->header & TAP_PACKET_FLAG_NOFORWARD))
		forward_packet(p);

	if ((state.flags & ID_ASSIGNED) && // we have an address and
	    !(p->header & TAP_PACKET_FLAG_IGNOREADDRESS) && // not a broadcast packet and
	    ((p->address_x != state.x_address) || (p->address_y != state.y_address))) // not addressed to us
		return; // then ignore packet

	switch (p->header & TAP_PACKET_TYPE_MASK)
	{
	case TAP_PACKET_FACTORY_RESET:
		state_defaults(&state);
		//re-init demo
		setup();
		break;

	case TAP_PACKET_SET_ID:
		set_id(p->address_x, p->address_y);
		break;

	case TAP_PACKET_SYNC_ESTABLISH:
		i_am_master = false;
		break;

	case TAP_PACKET_SYNC_FRAME:
		state.frame_sync = p->data[0] | (p->data[1] << 8);
		break;

	case TAP_PACKET_USER_DATA:
		if (user_data_index + p->size > USER_DATA_SIZE)
			break; // out of space
		memcpy(&user_data[user_data_index], p->data, p->size);
		user_data_index += p->size;
		break;

	case TAP_PACKET_USER_PROG_START:
		user_data_index = 0;
		run_program = true;
		break;

	case TAP_PACKET_USER_PROG_STOP:
		user_data_index = 0;
		run_program = false;
		break;

	case TAP_PACKET_USER_DEFINED:
		handle_custom_packet(p);
		break;
	}
}
