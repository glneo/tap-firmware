/*
 * Phototransistor control for the TAP (Technology Access Platform)
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

#ifdef PHOTO_SUPPORT

void photo_init()
{
	P2SEL &= (~BIT3); // Set P2.3 SEL as GPIO
	P2DIR &= (~BIT3); // Set P2.3 SEL as Input
	P2IES |= (BIT3); // Falling Edge 1 -> 0
	P2IFG &= (~BIT3); // Clear interrupt flag for P2.1
	P2IE |= (BIT3); // Enable interrupt for P2.1
}


/* Port 2 interrupt service routine */
#if defined(__TI_COMPILER_VERSION__) || (__IAR_SYSTEMS_ICC__)
#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void)
#elif defined(__GNUC__) && (__MSP430__)
void __attribute__ ((interrupt(PORT2_VECTOR))) PORT2_ISR (void)
#else
#error Compiler not found!
#endif
{
	P2IFG &= (~BIT3); // P2.1 IFG clear
}

#endif
