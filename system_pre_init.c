#include "msp430.h"
#include <stdint.h>

int16_t _system_pre_init(void)
{
	/*
	 * Disable Watchdog timer to prevent reset during
	 * long variable initialization sequences.
	 */
	WDTCTL = WDTPW | WDTHOLD;

	return 1;
}
