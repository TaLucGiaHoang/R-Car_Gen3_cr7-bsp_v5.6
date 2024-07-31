/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

    .global	__aeabi_llsr

/*****************************************************************************
 *      long long logical shift right
 *****************************************************************************/


__aeabi_llsr:

	subs	r3, r2, #32
	rsb	ip, r2, #32
	movmi	r0, r0, lsr r2
	movpl	r0, r1, lsr r3
	orrmi	r0, r0, r1, lsl ip
	mov	r1, r1, lsr r2
	mov	pc, lr

    .end
