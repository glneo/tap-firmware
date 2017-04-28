/*
 * Main file for the TAP (Technology Access Platform)
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

volatile bool run_program = true;

unsigned int __attribute__((weak)) hold_frames = 1;

/* Can be overridden by user program */
void __attribute__((weak)) setup(void) {}

/* Can be overridden by user program */
void __attribute__((weak)) loop(void) {}

/* warning: called from interrupt context */
void __attribute__((weak)) handle_custom_packet(packet_t *p) {}

void main_loop(void)
{
	// Step the demo
	if (run_program && !(state.frame_sync % hold_frames))
		loop();

	state.frame_sync++;
	// Master sends ID and frame sync packets every 100 frames
	if (i_am_master && !(state.frame_sync % 100))
	{
		set_id(0, 0);
		send_frame_sync(state.frame_sync);
	}

	// Scan through all LEDs once
	scan_LEDs();
}

int main(void)
{
	WDT_A_hold(WDT_A_BASE); // Stop watchdog timer

	init_tap();

	// Initialize the demo
	setup();

	while (true)
#ifdef USB_SUPPORT
		// USB loop polls for input and calls main_loop()
		USB_loop();
#else // USB_SUPPORT
		main_loop();
#endif // USB_SUPPORT
}
