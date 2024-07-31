/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */


	.global	dummy_vect
	.align	5

/*****************************************************************************
 *
 *****************************************************************************/
dummy_vect:
	b	loop				/* Reset */
	b	loop				/* Undefined Instruction */
	b	loop				/* Supervisor Call */
	b	loop				/* Prefetch Abort */
	b	DAbort				/* Data Abort */
	nop					/* Not used */
	b	loop				/* IRQ interrupt */
	b	loop				/* FIQ interrupt */

/*****************************************************************************
 *	Exception Handers
 *****************************************************************************/

DAbort:
	subs	pc, lr, #8

loop:
	nop
	b	loop

	.end
