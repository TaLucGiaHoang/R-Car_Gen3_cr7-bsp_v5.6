/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */


	.global	memcpy
	.global	memset

/*****************************************************************************
 *	input:
 *	 r0: dest
 *	 r1: source
 *	 r2: copy bytes
 *
 *	output
 *	 r0: dest
 *****************************************************************************/
memcpy:
	mov	r12, r0		/* backup dest */

	cmp	r2, #4
	bcc	2f

	and	r3, r0, #0x03
	cmp	r3, #0
	bne	2f		/* unaligned dest address */

	and	r3, r1, #0x03
	cmp	r3, #0
	bne	2f		/* unaligned source address */

1:	/* aligned copy */
	ldr	r3, [r1], #4
	str	r3, [r0], #4
	subs	r2, r2, #4
	beq	3f
	cmp	r2, #4
	bcs	1b

2:	/* unaligned copy */
	ldrb	r3, [r1], #1
	strb	r3, [r0], #1
	subs	r2, r2, #1
	bne	2b
3:
	mov	r0, r12		/* restore dest */
	bx	lr


/*****************************************************************************
 *	input:
 *	 r0: dest
 *	 r1: val
 *	 r2: count
 *
 *	output
 *	 r0: dest
 *****************************************************************************/
memset:
	mov	r12, r0		/* backup dest */

	cmp	r2, #0
	beq	2f		/* no copy */
1:
	strb	r1, [r0], #1
	subs	r2, r2, #1
	bne	1b
2:
	mov	r0, r12		/* restore dest */
	bx	lr


	.end
