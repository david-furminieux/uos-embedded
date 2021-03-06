/*
 * CAN driver for Milandr 1986ВЕ91 microcontroller.
 *
 * Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
#ifndef __CAN_H_
#define __CAN_H_

/*
 * CAN data packet.
 */
typedef struct __attribute__((__packed__)) _can_frame_t {
	unsigned id;
#define	CAN_ID_EID_SHIFT	0		/* Сдвиг поля EID */
#define	CAN_ID_EID_MASK		0x3FFFF		/* Маска поля EID */
#define	CAN_ID_SID_SHIFT	18		/* Сдвиг поля SID */
#define	CAN_ID_SID_MASK		(0x7FF << 18)	/* Маска поля SID */

	unsigned dlc;
#define	CAN_DLC_LEN(n)		((n) & 15)	/* Длина данных в байтах */
#define	CAN_DLC_RTR		(1 << 8)	/* Запрос обратного ответа */
#define	CAN_DLC_IDE		(1 << 12)	/* Расширенный формат */

	unsigned data [2];			/* До 8 байт данных */
} can_frame_t;

/*
 * Queue of CAN packets.
 */
#ifndef CAN_QUEUE_SIZE
#define CAN_QUEUE_SIZE		64	/* max number of packets in queue */
#endif

typedef struct _can_queue_t {
	unsigned count;
	can_frame_t *tail;
	can_frame_t queue [CAN_QUEUE_SIZE];
} can_queue_t;

/*
 * CAN channel initialization flags
 */
#define	POLLONLY 	0x8000
#define NOMUTEX		0x4000
/* Also PORT_A, PORT_B, etc. constants can be used */

/*
 * Data structure for CAN channel.
 */
typedef struct _can_t {
	mutex_t lock;			/* interrupt goes here */

	unsigned port;			/* port number */
	unsigned flags;			/* flags */
	unsigned kbitsec;		/* data rate kbit/sec */
	int loop;			/* enable internal loopback */
	can_queue_t inq;		/* queue of received packets */

	/* Statistics. */
	unsigned long intr;		/* interrupt counter */
	unsigned rec, tec;		/* hardware rx/tx error counters */

	unsigned long in_packets;	/* received packets */
	unsigned long in_errors;	/* receive errors */
	unsigned long in_frame_errors;	/* receive frame errors */
	unsigned long in_crc_errors;	/* receive CRC errors */
	unsigned long in_bitstuff_errors; /* receive bit-stuff errors */
	unsigned long in_discards;	/* ignored packets, due to lack of memory */

	unsigned long out_packets;	/* transmitted packets */
	unsigned long out_errors;	/* transmit errors */
	unsigned long out_collisions;	/* lost arbitrage */
	unsigned long out_nacks;	/* no ack */
	unsigned long out_bit_errors;	/* transmit bit errors */
} can_t;

/*
 * User level API.
 */
void can_init (can_t *c, int port, unsigned kbitsec, unsigned flags);
void can_set_speed (can_t *c, unsigned kbitsec);
void can_set_loop (can_t *c, int on);
int can_output (can_t *c, const can_frame_t *fr);
void can_input (can_t *c, can_frame_t *fr);
int  can_poll (can_t *c, can_frame_t *fr);
void can_stop (can_t *c);		/* sets given channel disabled (channel is enabled after can_init()) */
void can_start (can_t *c);		/* sets given channel enabled (needed to call only after can_stop()) */
void can_set_filter (can_t *c, unsigned mask, unsigned pattern);

#endif /* !__CAN_H_ */
