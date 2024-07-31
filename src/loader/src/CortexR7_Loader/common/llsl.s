/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

    .global	__aeabi_llsl

/*****************************************************************************
 *      long long logical shift left
 *
 *	input:
 *	 r0:
 *	 r1:
 *       r2:
 *
 *	output
 *	 r0:
 *       r1:
 *****************************************************************************/

__aeabi_llsl:
	subs	r3, r2, #32
	rsb	ip, r2, #32
	movmi	r1, r1, lsl r2
	movpl   r1, r0, lsl r3
	orrmi   r1, r1, r0, lsr ip
	mov     r0, r0, lsl r2
	mov	pc, lr

    .end
