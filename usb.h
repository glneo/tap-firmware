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

#ifndef __USB_H
#define __USB_H

#ifdef __cplusplus
extern "C"
{
#endif

void USB_loop();
void USB_init_tap();

#ifdef __cplusplus
}
#endif

#endif /* __USB_H */
