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

#ifndef __INIT_H
#define __INIT_H

#ifdef __cplusplus
extern "C"
{
#endif

extern volatile bool i_am_master;
extern volatile unsigned int LRN_mode_triggered;

void init_heap(void);
void init_tap(void);

#ifdef __cplusplus
}
#endif

#endif /* __INIT_H */
