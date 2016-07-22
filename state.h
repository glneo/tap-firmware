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

#ifndef __STATE_H
#define __STATE_H

#ifdef __cplusplus
extern "C"
{
#endif

#define	FLASH_INFO_A	0x1980	/* Information memory segment B */
#define	FLASH_INFO_B	0x1900	/* Information memory segment B */
#define	FLASH_INFO_C	0x1880	/* Information memory segment C */
#define	FLASH_INFO_D	0x1800	/* Information memory segment D */

#define	ID_ASSIGNED     0x40

typedef struct state_s
{
	char tag[3];
	unsigned char flags;
	unsigned char x_address;
	unsigned char y_address;
	unsigned int frame_sync;
} state_t;

extern state_t state;
extern state_t state_shadow;

void set_id(int x, int y);
void send_frame_sync(unsigned int frame);
void state_defaults(state_t *state);
state_t *state_find(int unused_only);
void state_save(void);
void state_init();

#ifdef __cplusplus
}
#endif

#endif /* __STATE_H */
