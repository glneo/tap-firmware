/*
 * Initialization functions for the TAP (Technology Access Platform)
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

volatile bool i_am_master = true;
volatile unsigned int LRN_mode_triggered;

static void wait_awhile(unsigned int awhile)
{
	while (awhile--)
		__delay_cycles(1000);
}

static void init_clocks(uint32_t mclkFreq)
{
	UCS_initClockSignal(UCS_FLLREF,
	                    UCS_REFOCLK_SELECT,
	                    UCS_CLOCK_DIVIDER_1);

	UCS_initClockSignal(UCS_ACLK,
	                    UCS_REFOCLK_SELECT,
	                    UCS_CLOCK_DIVIDER_1);

	UCS_initFLLSettle(mclkFreq / 1000,
	                  mclkFreq / 32768);
}

void init_tap(void)
{
	packet_t packet;

	init_clocks(25000000); // Config clocks. MCLK=SMCLK=FLL=25MHz; ACLK=REFO=32kHz

	display_init();

	state_init();

	// Initialize input and output UART channels
	UART_init();

#ifdef USB_SUPPORT
	USB_init_tap();
#endif

#ifdef PHOTO_SUPPORT
	photo_init();
#endif

	// Enable interrupt on port 1 pin 1
	P1DIR &= ~BIT1;         // P1.1 Input
	P1IES |= BIT1;          // P1.1 Hi->lo edge
	P1IFG &= ~BIT1;         // P1.1 cleared
	P1IE |= BIT1;           // P1.1 interrupt enabled
	LRN_mode_triggered = 0;

	__bis_SR_register(GIE); // Enable global interrupts

	__enable_interrupt(); // Enable interrupts globally

	// Wait a little to ensure init has finished for everybody
	__delay_cycles(12000);

	// Own the TAP to the right
	packet.header = TAP_PACKET_SYNC_ESTABLISH | TAP_PACKET_FLAG_NOFORWARD;
	packet.size = 0;
	send_packet(&x_out, &packet);

	// Wait a little for the one to the left to do the same
	wait_awhile(1000);

	if (i_am_master)
	{
		// I am the leftmost TAP, set my position tell others theirs
		set_id(0, 0);
	}

	// Wait a little longer
	wait_awhile(1000);
}

/*
 ************ Port_1 *********************************************************
 *
 * Port 1 interrupt service routine
 *
 */
#if defined(__TI_COMPILER_VERSION__) || (__IAR_SYSTEMS_ICC__)
#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void)
#elif defined(__GNUC__) && (__MSP430__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
#else
#error Compiler not found!
#endif
{
	if (P1IFG & BIT1)
	{
		// Clear interrupt
		P1IFG &= ~BIT1;

		// Trigger LRN mode
		LRN_mode_triggered = 1;
	}
}
