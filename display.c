/*
 * Core firmware for the TAP (Technology Access Platform)
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

unsigned char display_buffer[NUM_ROWS][NUM_COLS][NUM_COLORS];

// Maps columns to hardware
struct
{
	volatile uint8_t *port;
	uint8_t bit;
} static const map_col_to_port[] =
{
	// col 0
	{ .port = &P3OUT, .bit = BIT1 }, // red
	{ .port = &P4OUT, .bit = BIT1 }, // green
	{ .port = &P1OUT, .bit = BIT6 }, // blue
	// col 1
	{ .port = &P4OUT, .bit = BIT0 }, // red
	{ .port = &P4OUT, .bit = BIT2 }, // green
	{ .port = &P1OUT, .bit = BIT7 }, // blue
	// col 2
	{ .port = &P4OUT, .bit = BIT3 }, // red
	{ .port = &P4OUT, .bit = BIT7 }, // green
	{ .port = &P4OUT, .bit = BIT6 }, // blue
	// col 3
	{ .port = &P3OUT, .bit = BIT0 }, // red
	{ .port = &P3OUT, .bit = BIT2 }, // green
	{ .port = &P1OUT, .bit = BIT5 }, // blue
	// col 4
	{ .port = &P1OUT, .bit = BIT4 }, // red
	{ .port = &P1OUT, .bit = BIT2 }, // green
	{ .port = &P1OUT, .bit = BIT3 }, // blue
	// col 5
	{ .port = &P5OUT, .bit = BIT0 }, // red
	{ .port = &P6OUT, .bit = BIT7 }, // green
#ifdef TAP_PRE_1_2
	{ .port = &P6OUT, .bit = BIT6 }, // blue
#else
	{ .port = &P2OUT, .bit = BIT4 }, // blue
#endif

	// col 6
	{ .port = &P6OUT, .bit = BIT0 }, // red
	{ .port = &P6OUT, .bit = BIT5 }, // green
	{ .port = &P6OUT, .bit = BIT4 }, // blue
	// col 7
	{ .port = &P6OUT, .bit = BIT1 }, // red
	{ .port = &P5OUT, .bit = BIT1 }, // green
	{ .port = &P6OUT, .bit = BIT3 }, // blue
};

#define CLOCK_U5() \
do { \
	PJOUT &= ~BIT0; \
	PJOUT |= BIT0; \
} while(false)

static void draw_row(int row, unsigned char *display_ptr)
{
	unsigned int col;
	unsigned char current_delay;

	// Turn on every LED with a non-zero value
	for (col = 0; col < ROW_SIZE; col++)
		if (display_ptr[col] > 0)
			*map_col_to_port[col].port |= map_col_to_port[col].bit;
		else
			__delay_cycles(LOW_POWER_DELAY);

	// Turn off LEDs as we count up to their value
	for (current_delay = 1; current_delay < MAX_DELAY; current_delay++)
	{
		for (col = 0; col < ROW_SIZE; col++)
			if (display_ptr[col] <= current_delay)
				*map_col_to_port[col].port &= ~map_col_to_port[col].bit;
			else
				__delay_cycles(LOW_POWER_DELAY);
	}

	// Turn off all remaining LEDs
	for (col = 0; col < ROW_SIZE; col++)
		*map_col_to_port[col].port &= ~map_col_to_port[col].bit;
}

/**
 * scan_LEDs() - Main screen refresh
 */
void scan_LEDs()
{
	unsigned int row;

	// Latch 0 into U5 B input - this will turn on power to first row
	PJOUT &= ~BIT3; // Make B input low
	CLOCK_U5(); // Clock U5

	// First row of LEDs now has power...

	PJOUT |= BIT3;	//make B input on U5 high so the shift register clocks a single zero across to power each row

	// Pulse all columns with required delay times and then power the next row
	for (row = 0; row < NUM_ROWS; row++)
	{
		draw_row(row, display_buffer[row]);

		// Enable power to next row of LEDs
		// Clock U5 moving the zero across the shift register
		CLOCK_U5();
	}

#ifdef	FRAME_RATE_TEST
	P6OUT ^= BIT2;				// Toggle P6.2 test point
#endif
}

// On reset U5 to all outputs low - bad! - all LED rows powered, fix this here.
static void init_U5()
{
	unsigned int x;

	// U5 (row select shift register)
	PJDIR |= BIT3; PJDIR |= BIT3; // Make B input of U5 high
	PJOUT |= BIT2; PJDIR |= BIT2;
	PJDIR |= BIT0; PJOUT |= BIT0;

	// Clock all 1s into U5 to turn all ROWS off
	for (x = 0; x < 8; x++)
		CLOCK_U5();
}

static void init_LEDports()
{
	// LED0
	P3DIR |= BIT1; P3OUT &= ~BIT1;
	P4DIR |= BIT1; P4OUT &= ~BIT1;
	P1DIR |= BIT6; P1OUT &= ~BIT6;

	// LED1
	P4DIR |= BIT0; P4OUT &= ~BIT0;
	P4DIR |= BIT2; P4OUT &= ~BIT2;
	P1DIR |= BIT7; P1OUT &= ~BIT7;

	// LED2
	P4DIR |= BIT3; P4OUT &= ~BIT3;
	P4DIR |= BIT7; P4OUT &= ~BIT7;
	P4DIR |= BIT6; P4OUT &= ~BIT6;

	// LED3
	P3DIR |= BIT0; P3OUT &= ~BIT0;
	P3DIR |= BIT2; P3OUT &= ~BIT2;
	P1DIR |= BIT5; P1OUT &= ~BIT5;

	// LED4
	P1DIR |= BIT4; P1OUT &= ~BIT4;
	P1DIR |= BIT2; P1OUT &= ~BIT2;
	P1DIR |= BIT3; P1OUT &= ~BIT3;

	// LED5
	P5DIR |= BIT0;P5OUT &= ~BIT0;
	P6DIR |= BIT7; P6OUT &= ~BIT7;
#ifdef TAP_PRE_1_2
	P6DIR |= BIT6; P6OUT &= ~BIT6;
#else
	P2DIR |= BIT4; P2OUT &= ~BIT4;
#endif

	// LED6
	P6DIR |= BIT0; P6OUT &= ~BIT0;
	P6DIR |= BIT5; P6OUT &= ~BIT5;
	P6DIR |= BIT4; P6OUT &= ~BIT4;

	// LED7
	P6DIR |= BIT1; P6OUT &= ~BIT1;
	P5DIR |= BIT1; P5OUT &= ~BIT1;
	P6DIR |= BIT3; P6OUT &= ~BIT3;
}

void display_init()
{
	init_U5(); // Clear shift register
	init_LEDports();

	// Clear the display
	memset(display_buffer, 0, sizeof(display_buffer));

#ifdef FRAME_RATE_TEST
	P6DIR |= BIT2; // Test point
#endif
}
