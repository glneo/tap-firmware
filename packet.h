/*
 * Packet definitions for the TAP (Technology Access Platform)
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

#ifndef __PACKET_H
#define __PACKET_H

#ifdef __cplusplus
extern "C"
{
#endif

#define	TAP_PACKET_TYPE_MASK            0x1F

#define	TAP_PACKET_FACTORY_RESET        0x10
#define	TAP_PACKET_SET_ID               0x11
#define	TAP_PACKET_SYNC_ESTABLISH       0x12
#define	TAP_PACKET_SYNC_ORIGIN          0x13
#define	TAP_PACKET_USER_DATA            0x14
#define	TAP_PACKET_USER_PROG_START      0x15
#define	TAP_PACKET_USER_PROG_STOP       0x16
#define	TAP_PACKET_USER_DEFINED         0x17
#define	TAP_PACKET_SYNC_FRAME           0x18

#define	TAP_PACKET_FLAG_TEMPORARY       0x20
#define	TAP_PACKET_FLAG_NOFORWARD       0x40
#define	TAP_PACKET_FLAG_IGNOREADDRESS   0x80

#define	MAX_PACKET_DATA_SIZE            (256 - 8)

typedef struct packet_s
{
	uint8_t header;
	uint8_t address_x;
	uint8_t address_y;
	uint8_t size;
	uint8_t data[MAX_PACKET_DATA_SIZE];
} packet_t;

void send_packet(channel_t *c, packet_t *p);
void forward_packet(packet_t *c);
void process_packet(packet_t *c);
void process_packets(void);

void handle_custom_packet(packet_t *p);

#ifdef __cplusplus
}
#endif

#endif /* __PACKET_H */
