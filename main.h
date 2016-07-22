/*
 * Main header for the TAP (Technology Access Platform)
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

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <msp430.h>
#include <driverlib.h>
#include <string.h>
#include <stdbool.h>

#define	FRAME_RATE_TEST /* Define to toggle pin 6.2 for each frame */
#define PHOTO_SUPPORT /* Define to include phototransistor input support */
#define USB_SUPPORT   /* Define to include USB support */

#include "channel.h"
#include "display.h"
#include "init.h"
#include "packet.h"
#include "photo.h"
#include "state.h"
#include "uart.h"
#include "usb.h"

extern volatile bool run_program;

void setup(void);
void loop(void);

void main_loop(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

