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

#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
extern "C"
{
#endif

void tickle_serial_interrupt(channel_t *c);
void UART_init();

#ifdef __cplusplus
}
#endif

#endif /* __UART_H */
