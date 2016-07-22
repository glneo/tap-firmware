/*
 * Channel definitions for the TAP (Technology Access Platform)
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

#ifndef __CHANNEL_H
#define __CHANNEL_H

#ifdef __cplusplus
extern "C"
{
#endif

/* SLIP protocol characters. */
#define SLIP_END     0xC0       /* Frame End */
#define SLIP_ESC     0xDB       /* Frame Escape */
#define SLIP_ESC_END 0xDC       /* Transposed Frame End */
#define SLIP_ESC_ESC 0xDD       /* Transposed Frame Escape */

typedef struct channel_s channel_t;

extern channel_t serial_x;
extern channel_t serial_y;

extern channel_t x_out;
extern channel_t y_out;

#ifdef PHOTO_SUPPORT
extern channel_t photo;
#endif

#ifdef USB_SUPPORT
extern channel_t usb;
#endif

void flush_channel(channel_t *c);
int put_byte_input(channel_t *c, unsigned char b);
int put_byte_output(channel_t *c, unsigned char b);
int get_byte_output(channel_t *c, unsigned char *b);

#ifdef __cplusplus
}
#endif

#endif /* __CHANNEL_H */
