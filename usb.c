/*
 * USB control for the TAP (Technology Access Platform)
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

#ifdef USB_SUPPORT

#include "USB_config/descriptors.h"
#include "USB_API/USB_Common/device.h"
#include "USB_API/USB_Common/usb.h"                 // USB-specific functions
#include "USB_API/USB_CDC_API/UsbCdc.h"
#include "USB_app/usbConstructs.h"

/* Flag set by event handler to indicate data has been received into USB buffer */
volatile uint8_t bCDCDataReceived_event = FALSE;

/**
 * USB_receive_data() - Add data to USB channel
 */
static void USB_receive_data(unsigned char b)
{
	put_byte_input(&usb, b);
}

/* USB UNMI interrupt service routine */
#if defined(__TI_COMPILER_VERSION__) || (__IAR_SYSTEMS_ICC__)
#pragma vector = UNMI_VECTOR
__interrupt void UNMI_ISR(void)
#elif defined(__GNUC__) && (__MSP430__)
void __attribute__ ((interrupt(UNMI_VECTOR))) UNMI_ISR (void)
#else
#error Compiler not found!
#endif
{
	switch (__even_in_range(SYSUNIV, SYSUNIV_BUSIFG))
	{
	case SYSUNIV_NONE:
		__no_operation();
		break;
	case SYSUNIV_NMIIFG:
		__no_operation();
		break;
	case SYSUNIV_OFIFG:
		UCS_clearFaultFlag(UCS_XT2OFFG);
		UCS_clearFaultFlag(UCS_DCOFFG);
		SFR_clearInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);
		break;
	case SYSUNIV_ACCVIFG:
		__no_operation();
		break;
	case SYSUNIV_BUSIFG:
		// If the CPU accesses USB memory while the USB module is
		// suspended, a "bus error" can occur.  This generates an NMI.  If
		// USB is automatically disconnecting in your software, set a
		// breakpoint here and see if execution hits it.  See the
		// Programmer's Guide for more information.
		SYSBERRIV = 0; //clear bus error flag
		USB_disable(); //Disable
	}
}

void USB_loop()
{
	#define USB_BUFFER_SIZE 256
	char dataBuffer[USB_BUFFER_SIZE] = "";
	unsigned int x;

	uint8_t ReceiveError = 0, SendError = 0;
	uint16_t count;

	switch (USB_getConnectionState())
	{
	case ST_ENUM_ACTIVE:
		// Scan TAP if there are no bytes to process.
		__disable_interrupt();
		while (USB_getConnectionState() == ST_ENUM_ACTIVE && !USBCDC_getBytesInUSBBuffer(CDC0_INTFNUM))
		{
			__enable_interrupt();
			main_loop();
			__disable_interrupt();
		}
		__enable_interrupt();

		// Exit loop because of a data-receive event, and
		// fetch the received data
		if (bCDCDataReceived_event)
		{
			// Clear flag early -- just in case execution breaks
			// below because of an error
			bCDCDataReceived_event = FALSE;

			count = USBCDC_receiveDataInBuffer((uint8_t*) dataBuffer, USB_BUFFER_SIZE, CDC0_INTFNUM);

			// Accept packet data from USB
			for (x = 0; x < count; x++)
				USB_receive_data(dataBuffer[x]);
		}
		break;

		// Sleep if suspended
	case ST_ENUM_SUSPENDED:
		__bis_SR_register(LPM3_bits + GIE);
		_NOP();
		break;

		// These cases are executed while your device is disconnected from
		// the host (meaning, not enumerated);
		// or connected to a powered hub without a USB host present.
	case ST_PHYS_DISCONNECTED:
	case ST_USB_CONNECTED_NO_ENUM:
	case ST_PHYS_CONNECTED_NOENUM_SUSP:

		// The default is executed for the momentary state
		// ST_ENUM_IN_PROGRESS.  Usually, this state only last a few
		// seconds.  Be sure not to enter LPM3 in this state; USB
		// communication is taking place here, and therefore the mode must
		// be LPM0 or active-CPU.
	case ST_ENUM_IN_PROGRESS:
	default:
		main_loop();
		break;
	}

	if (ReceiveError || SendError)
	{
		// TO DO: User can place code here to handle error
	}
}

void USB_init_tap()
{
	// Minimum Vcore setting required for the USB API is PMM_CORE_LEVEL_2.
	PMM_setVCore(PMM_CORE_LEVEL_2);
	USB_setup(TRUE, TRUE); // Init USB & events; if a host is present, connect

	flush_channel(&usb);
}

#endif // USB_SUPPORT
