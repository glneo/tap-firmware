/*
 * State handling for the TAP (Technology Access Platform)
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

state_t state;

/**
 * set_id() - Set my ID and neighbors to right and up if needed
 *
 * @x: X address
 * @y: Y address
 */
void set_id(int x, int y)
{
	packet_t packet;

	state.flags |= ID_ASSIGNED;
	state.x_address = x;
	state.y_address = y;

	// Inform immediate neighbors to the right and above
	packet.header = TAP_PACKET_SET_ID | TAP_PACKET_FLAG_IGNOREADDRESS | TAP_PACKET_FLAG_NOFORWARD;
	packet.address_x = x + 1;
	packet.address_y = y;
	packet.size = 0;
	send_packet(&x_out, &packet);
	if (state.x_address == 0) // only the bottom row propagates messages up
	{
		packet.address_x = x;
		packet.address_y = y + 1;
		send_packet(&y_out, &packet);
	}
}

void send_frame_sync(unsigned int frame)
{
	packet_t packet;

	packet.header = TAP_PACKET_SYNC_FRAME | TAP_PACKET_FLAG_IGNOREADDRESS;
	packet.size = 2;
	packet.data[0] = frame & 0xff;
	packet.data[1] = (frame >> 8) & 0xff;
	forward_packet(&packet);
}

/**
 * state_defaults() - Set state to defaults
 */
void state_defaults(state_t *state)
{
	state->tag[0] = 'T';
	state->tag[1] = 'A';
	state->tag[2] = 'P';
	state->flags = 0;
	state->x_address = 0;
	state->y_address = 0;
	state->frame_sync = 0;
}

/*
 ************ need_to_erase **************************************************
 *
 * Determine whether flash memory erasure is needed to store data
 *
 */
static int need_to_erase(char *old, char *new, unsigned int length)
{
	char c;

	for (; length-- > 0;)
	{
		// Are the bytes different?
		c = *old++ ^ *new;

		// Are we turning a zero into a one?  If so, must erase.
		if (c & *new++)
			return 1;
	}

	return 0;
}

/*
 ************ state_find *****************************************************
 *
 * Find last saved state in flash memory
 *
 */
state_t *state_find(int unused_only)
{
	state_t *limit;
	state_t *scanner;

	// Start at the base of the information segment
	scanner = (state_t *) FLASH_INFO_D;

	// This is the address beyond the highest possible copy
	limit = (state_t *) (FLASH_INFO_D + (128 / sizeof(state_t)) * sizeof(state_t));

	while (scanner < limit)
	{
		if (!unused_only && scanner->tag[0] == 'T' && scanner->tag[1] == 'A' && scanner->tag[2] == 'P')
			// Found a written record
			return scanner;

		if (scanner->tag[0] == 0xFF && scanner->tag[1] == 0xFF && scanner->tag[2] == 0xFF)
			// Found an unwritten record
			return scanner;

		// Otherwise, this entry has been deleted?
		if (scanner->tag[0] != 0 || scanner->tag[1] != 0 || scanner->tag[2] != 0)
			// Found a corrupt record, we did not initialize it
			return 0;

		// Advance to the next candidate
		scanner++;
	}

	// Did not find it, must be full of deleted records
	return 0;
}

/*
 ************ state_save *****************************************************
 *
 * Save state to flash memory
 *
 */
void state_save(void)
{
	state_t defaults;
	state_t *current_state;
	state_t *unused;
	int delete = 0;
	int erase = 0;
	int overwrite = 0;

	// Get default settings
	state_defaults(&defaults);

	// If the state matches the defaults, we do not need to store it
	if (!memcmp(&state_shadow, &defaults, sizeof(state_t)))
		return;

	// Find the current state record, if any
	current_state = state_find(0);
	if (current_state == 0)
	{
		// No undeleted state entries, must erase segment to create one
		erase = 1;

		// Write state when done
		overwrite = 1;
	}
	else
	{
//		if (matches_defaults)
//		{
//			if (current_state->tag[0] == 'T')
//				// We previously had a state record, just delete it
//				delete = 1;
//		}
//		else
		{
			// Can overwrite current state record (change 1's to 0's OK)?
			if (!need_to_erase((char *) current_state, (char *) &state_shadow, sizeof(state_t)))
			{
				// Can just reuse this record
				overwrite = 1;
				unused = current_state;
			}
			else
			{
				// Can delete this record and use the next one?
				unused = state_find(1);
				if (unused != 0)
					// Found unused record
					delete = 1;
				else
					// No more available
					erase = 1;

				overwrite = 1;
			}
		}
	}

	if (delete)
	{
		FCTL3 = FWKEY;			// Clear Lock bit
		FCTL1 = FWKEY + WRT;		// Set WRT bit for write operation

		current_state->tag[0] = 0;
		current_state->tag[1] = 0;
		current_state->tag[2] = 0;

		FCTL1 = FWKEY;			// Clear WRT bit
		FCTL3 = FWKEY + LOCK;		// Set LOCK bit
	}

	if (erase)
	{
		unused = (state_t *) FLASH_INFO_D;
		FCTL3 = FWKEY;			// Clear Lock bit
		FCTL1 = FWKEY + ERASE;	// Set Erase bit

		unused->tag[0] = 0;		// Dummy write to erase Flash seg

		FCTL1 = FWKEY;			// Clear WRT bit
		FCTL3 = FWKEY + LOCK;		// Set LOCK bit
	}

	if (overwrite)
	{
		FCTL3 = FWKEY;			// Clear Lock bit
		FCTL1 = FWKEY + WRT;		// Set WRT bit for write operation

		memcpy(unused, &state_shadow, sizeof(state_t));

		FCTL1 = FWKEY;			// Clear WRT bit
		FCTL3 = FWKEY + LOCK;		// Set LOCK bit
	}
}

void state_init()
{
	state_defaults(&state);
}
