/*
 * Display processing for the TAP (Technology Access Platform)
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

#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Set maximum LED ON time,
 * Set too large - get flicker,
 * Set too low - get more overhead and lose dynamic range of intensity
 */
#define MAX_DELAY   31
#define NUM_LEVELS  MAX_DELAY+1

#define	LOW_POWER_DELAY	3	/* Cycles for individual LED ON delay */
#define	BRIGHT_DELAY	4	/* Adjust when MAX_DELAY is changed */

#define	FULL_INTENSITY		MAX_DELAY
#define	HALF_INTENSITY		(FULL_INTENSITY / 2)
#define	QUARTER_INTENSITY	(HALF_INTENSITY / 2)
#define	EIGHTH_INTENSITY	(QUARTER_INTENSITY / 2)

//*********** For direct access to the display buffer ************************
#define	RED             0
#define	GREEN           1
#define	BLUE            2

#define NUM_COLS	8
#define	COL_SIZE        3
#define	COL0            (0*COL_SIZE)
#define	COL1		(1*COL_SIZE)
#define	COL2		(2*COL_SIZE)
#define	COL3		(3*COL_SIZE)
#define	COL4		(4*COL_SIZE)
#define	COL5		(5*COL_SIZE)
#define	COL6		(6*COL_SIZE)
#define	COL7		(7*COL_SIZE)

#define NUM_ROWS	8
#define	ROW_SIZE	(8*COL_SIZE)
#define	ROW0		(0*ROW_SIZE)
#define	ROW1		(1*ROW_SIZE)
#define	ROW2		(2*ROW_SIZE)
#define	ROW3		(3*ROW_SIZE)
#define	ROW4		(4*ROW_SIZE)
#define	ROW5		(5*ROW_SIZE)
#define	ROW6		(6*ROW_SIZE)
#define	ROW7		(7*ROW_SIZE)

extern unsigned char display_buffer[NUM_ROWS][NUM_COLS * COL_SIZE];

void scan_LEDs();
void display_init();

#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_H */
