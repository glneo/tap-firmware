/*
 * UART control for the TAP (Technology Access Platform)
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

/* Y UART interrupt service routine */
#if defined(__TI_COMPILER_VERSION__) || (__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__) && (__MSP430__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not found!
#endif
{
	unsigned char ch;

	switch (__even_in_range(UCA0IV, 4))
	{
	case 0:							// Vector 0 - no interrupt
		break;
	case 2:							// Vector 2 - RXIFG
		put_byte_input(&serial_y, UCA0RXBUF);
		break;
	case 4:							// Vector 4 - TXIFG
		// Transmit data
		if (get_byte_output(&y_out, &ch))
			UCA0IE &= ~UCTXIE; // Disable interrupt

		UCA0TXBUF = ch;
		break;
	default:
		break;
	}
}

/* X UART interrupt service routine */
#if defined(__TI_COMPILER_VERSION__) || (__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__) && (__MSP430__)
void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR (void)
#else
#error Compiler not found!
#endif
{
	unsigned char ch = 0;

	switch (__even_in_range(UCA1IV, 4))
	{
	case 0:							// Vector 0 - no interrupt
		break;
	case 2:							// Vector 2 - RXIFG
		put_byte_input(&serial_x, UCA1RXBUF);
		break;
	case 4:							// Vector 4 - TXIFG
		// Transmit data
		if (get_byte_output(&x_out, &ch)) // no more bytes
			UCA1IE &= ~UCTXIE; // Disable interrupt

		UCA1TXBUF = ch;
		break;

	default:
		break;
	}
}

/* Enable serial transmit interrupt if necessary */
void tickle_serial_interrupt(channel_t *c)
{
	if (c == &x_out)
	{
		// Enable interrupt
		UCA1IE |= UCTXIE;
	}
	else if (c == &y_out)
	{
		// Enable interrupt
		UCA0IE |= UCTXIE;
	}
}

void UART_init()
{
	P3SEL = (BIT3 + BIT4);         // P3.3,4 = USCI_A0 TXD/RXD
	UCA0CTL1 |= UCSWRST;           // **Put state machine in reset**
	UCA0CTL1 |= UCSSEL__SMCLK;     // Select SMCLK as our clock
	UCA0BR0 = 217;                 // 25MHz 115200 baud (see User's Guide)
	UCA0BR1 = 0;                   // 25MHz 115200 baud (see User's Guide)
	UCA0MCTL = UCBRF_0;            // Modln UCBRSx=0, UCBRFx=0
	UCA0CTL1 &= ~UCSWRST;          // **Initialize USCI state machine**
	UCA0IE |= UCRXIE;              // Enable USCI_A0 RX interrupt

	P4SEL = (BIT4 + BIT5);         // P4.4,5 = USCI_A1 TXD/RXD
	UCA1CTL1 |= UCSWRST;           // **Put state machine in reset**
	UCA1CTL1 |= UCSSEL__SMCLK;     // SMCLK
	UCA1BR0 = 217;                 // 25MHz 115200 baud (see User's Guide)
	UCA1BR1 = 0;                   // 25MHz 115200 baud (see User's Guide)
	UCA0MCTL = UCBRF_0;            // Modln UCBRSx=0, UCBRFx=9, over sampling
	UCA1CTL1 &= ~UCSWRST;          // **Initialize USCI state machine**
	UCA1IE |= UCRXIE;              // Enable USCI_A1 RX interrupt

	flush_channel(&serial_x);
	flush_channel(&serial_y);

	flush_channel(&x_out);
	flush_channel(&y_out);
}
