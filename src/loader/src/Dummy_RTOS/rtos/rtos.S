/*
 * Copyright (c) 2015-2017, Renesas Electronics Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *   - Neither the name of Renesas nor the names of its contributors may be
 *     used to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#define	DRAM_BASE		(0x40000000)
#define	SYSRAM_BASE		(0xE6300000)
#define	DTCM_BASE		(0xEB020000)

#define	STACK_BASE_ABT		(DTCM_BASE | 0x7080)
#define	STACK_BASE_UND		(DTCM_BASE | 0x7100)
#define	STACK_BASE_FIQ		(DTCM_BASE | 0x7180)
#define	STACK_BASE_IRQ		(DTCM_BASE | 0x7200)
#define	STACK_BASE_SVC		(DTCM_BASE | 0x8000)

#define	SCTLR_I			(1 << 12)
#define	SCTLR_Z			(1 << 11)

	.global	Vector
	.global	Start
	.local	rtos_stacks

/*****************************************************************************
 *	Vector table
 *****************************************************************************/
	.align	5

Vector:
	b	Start				/* Reset */
	b	Undef				/* Undefined Instruction */
	b	SWI				/* Supervisor Call */
	b	PAbort				/* Prefetch Abort */
	b	DAbort				/* Data Abort */
	nop					/* Not used */
	b	IRQ				/* IRQ interrupt */
	b	FIQ				/* FIQ interrupt */


/*****************************************************************************
 *	Reset Hander
 *****************************************************************************/
Start:
	/* initialize registers*/
	mov	r0,  #0
	mov	r1,  #0
	mov	r2,  #0
	mov	r3,  #0
	mov	r4,  #0
	mov	r5,  #0
	mov	r6,  #0
	mov	r7,  #0
	mov	r8,  #0
	mov	r9,  #0
	mov	r10, #0
	mov	r11, #0
	mov	r12, #0
	mov	lr,  #0

	/* stack initialize */
	ldr	r0, =__STACKS_END__

	msr	CPSR_c, #(0x17 | 0x80 | 0x40)	/* ABT */
	mov	sp, r0				/* STACK_BASE_ABT */

	msr	CPSR_c, #(0x1B | 0x80 | 0x40)	/* UND */
	sub	r0, r0, #0x80			/* STACK_BASE_UND */
	mov	sp, r0

	msr	CPSR_c, #(0x11 | 0x80 | 0x40)	/* FIQ */
	sub	r0, r0, #0x80			/* STACK_BASE_FIQ */
	mov	sp, r0

	msr	CPSR_c, #(0x12 | 0x80 | 0x40)	/* IRQ */
	sub	r0, r0, #0x80			/* STACK_BASE_IRQ */
	mov	sp, r0

	msr	CPSR_c, #(0x13 | 0x80 | 0x40)	/* SVC */
	sub	r0, r0, #0x80			/* STACK_BASE_SVC */
	mov	sp, r0

	/* Instruction cache enable */
	mrc	p15, 0, r1, c1, c0, 0		/* SCTLR */
	orr 	r1, r1, #SCTLR_I		/* I=1 */
	orr 	r1, r1, #SCTLR_Z		/* Z=1 */
	mcr 	p15, 0, r0, c7, c5, 0		/* ICIALLU */
	mcr	p15, 0, r0, c7, c5, 6		/* BPIALL */
	mcr 	p15, 0, r1, c1, c0, 0		/* SCTLR */
	isb

	/* Loader Main */
	BL     rtos_main

NO_BOOT:
	nop
1:
	wfe
	b	1b


/*****************************************************************************
 *	Exception Handers
 *****************************************************************************/

	/* Undefined Instruction */
Undef:
	nop
1:
	wfe
	b	1b

	/* Supervisor Call */
SWI:
	nop
1:
	wfe
	b	1b

	/* Prefetch Abort */
PAbort:
	nop
1:
	wfe
	b	1b

	/* Data Abort */
DAbort:
	nop
1:
	wfe
	b	1b

	/* IRQ interrupt */
IRQ:
	nop
1:
	wfe
	b	1b

	/* FIQ interrupt */
FIQ:
	nop
1:
	wfe
	b	1b


/*****************************************************************************
 *	Define stack
 *****************************************************************************/
	.section tzfw_normal_stacks, "aw"
	.align   6

rtos_stacks:
	.space   4 * 1024

	.end
