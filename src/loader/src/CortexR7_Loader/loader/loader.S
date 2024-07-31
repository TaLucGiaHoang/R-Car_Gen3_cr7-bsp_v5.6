/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#define MODE_FIQ		(0x11)
#define MODE_IRQ		(0x12)
#define MODE_SVC		(0x13)
#define MODE_ABT		(0x17)
#define MODE_UND		(0x1B)
#define MODE_SYS		(0x1F)

#define GLOBAL_TIMER_CTRL_REG	(0xF0000208U)
#define GLOBAL_TIMER_EN		(0x1U)

#define	ARMREG_P_CR7SS		(0xE6271800)
#define	SYSRAM_BASE		(0xE6300000)
#define	SCU_BASE		(0xF0000000)

#define	TCM_ENABLE		(0x00000001)
#define	ITCM_BASE		(0x00000000)
#define	DTCM_BASE		(0xEB020000)
#define	ITCMRR_INIT_DATA	(ITCM_BASE | TCM_ENABLE)
#define	DTCMRR_INIT_DATA	(DTCM_BASE | TCM_ENABLE)

#define WBWA			(0x00B)		/* Write-Back Write Allocate */
#define WT			(0x002)		/* Write-Through */
#define NC			(0x008)		/* Non-cacheable */
#define DEV			(0x001)		/* Device */
#define SO			(0x000)		/* Strongly Ordered */
#define SHARE			(0x004)		/* Shareable */
#define AP_FULL			(0x300)

#define	SCTLR_I			(1 << 12)
#define	SCTLR_Z			(1 << 11)
#define	SCTLR_M			(1 << 0)

#define IRQ_MASK		(0x80)
#define FIQ_MASK		(0x40)

#define STACK_BASE_SHIFT	(0x80)

#define ENABLE_ECC_ITCM		(1 << 10)
#define ENABLE_ECC_CACHE_DTCM	(1 << 9)
#define ACTLR_ECC_ENABLE	(ENABLE_ECC_ITCM | ENABLE_ECC_CACHE_DTCM)

#define DCACHE_MAX_WAY_MASK	(0x3FF)
#define DCACHE_SET_MASK		(0x7FFF)

#define LOCK_STEP_ENABLE	(0x80000000)

	.global	Startup
	.global	do_panic

	.align	5



/*****************************************************************************
 *	Reset Hander
 *****************************************************************************/
Startup:
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

	ldr r0, =__STACKS_END__

	/* stack initialize */
	msr	CPSR_c, #(MODE_ABT | IRQ_MASK | FIQ_MASK)	/* ABT */
	mov	sp, r0					/* STACK_BASE_ABT */

	msr	CPSR_c, #(MODE_UND | IRQ_MASK | FIQ_MASK)	/* UND */
	sub	r0, r0, #STACK_BASE_SHIFT		/* STACK_BASE_UND */
	mov	sp, r0

	msr	CPSR_c, #(MODE_FIQ | IRQ_MASK | FIQ_MASK)	/* FIQ */
	sub	r0, r0, #STACK_BASE_SHIFT		/* STACK_BASE_FIQ */
	mov	sp, r0

	msr	CPSR_c, #(MODE_IRQ | IRQ_MASK | FIQ_MASK)	/* IRQ */
	sub	r0, r0, #STACK_BASE_SHIFT		/* STACK_BASE_IRQ */
	mov	sp, r0

	msr	CPSR_c, #(MODE_SVC | IRQ_MASK | FIQ_MASK)	/* SVC */
	sub	r0, r0, #STACK_BASE_SHIFT		/* STACK_BASE_SVC */
	mov	sp, r0

	/* Start Global Timer of CR7 */
	ldr	r0, =GLOBAL_TIMER_CTRL_REG
	ldr	r1, [r0]
	orr	r1, r1, #GLOBAL_TIMER_EN	/* bit0 */
	str	r1, [r0]

	/* TCM configuration */
	mrc	p15, 0, r0, c9, c1, 0		/* DTCMRR */
	ldr	r1, =DTCMRR_INIT_DATA
	orr	r0, r0, r1
	mcr	p15, 0, r0, c9, c1, 0

	mrc	p15, 0, r0, c9, c1, 1		/* ITCMRR */
	ldr	r1, =ITCMRR_INIT_DATA
	orr	r0, r0, r1
	mcr	p15, 0, r1, c9, c1, 1

	/* Dual core lock step enable */
	ldr	r0, =ARMREG_P_CR7SS
	ldr	r1, [r0]
	orr	r1, r1, #LOCK_STEP_ENABLE	/* bit31 */
	str	r1, [r0]

	/* Cache/TCM ECC error detection enable */
	mov	r0, #0
	mcr	p15, 0, r0, c15, c2, 0		/* DEER0 */
	mcr	p15, 0, r0, c15, c2, 1		/* DEER1 */
	mcr	p15, 0, r0, c15, c2, 2		/* DEER2 */
	mcr	p15, 0, r0, c15, c3, 0		/* IEER0 */
	mcr	p15, 0, r0, c15, c3, 1		/* IEER1 */
	mcr	p15, 0, r0, c15, c3, 2		/* IEER2 */
	mcr	p15, 0, r0, c15, c4, 0		/* DTCMEER */
	mcr	p15, 0, r0, c15, c5, 0		/* ITCMEER */

	mrc	p15, 0, r0, c1, c0, 1		/* ACTLR */
	mov	r1, #ACTLR_ECC_ENABLE		/* bit10, bit9 */
	orr	r0, r0, r1
	mcr	p15, 0, r0, c1, c0, 1


	/* Instruction cache enable */
	mrc	p15, 0, r1, c1, c0, 0		/* SCTLR */
	orr 	r1, r1, #SCTLR_I		/* I=1 */
	orr 	r1, r1, #SCTLR_Z		/* Z=1 */
	mcr 	p15, 0, r0, c7, c5, 0		/* ICIALLU */
	mcr	p15, 0, r0, c7, c5, 6		/* BPIALL */
	mcr 	p15, 0, r1, c1, c0, 0		/* SCTLR */
	isb

	/* Invalidate D-Cache */
	mov	r10, #0
	mcr	p15, 2, r10, c0, c0, 0		/* CSSELR */
	isb

	mrc	p15, 1, r1, c0, c0, 0		/* CCSIDR */
	and	r2, r1, #7			/* b001=8word/line */
	add	r2, r2, #4			/* DCISW calc shift */
	ldr	r4, =DCACHE_MAX_WAY_MASK	/* mac way mask */
	ands	r4, r4, r1, LSR #3		/* r4 = way */
	clz	r5, r4
	ldr	r7, =DCACHE_SET_MASK
	ands	r7, r7, r1, LSR #13
inv_d_cache_loop2:
	mov	r9, r4
inv_d_cache_loop3:
	orr	r11, r10, r9, LSL r5
	orr	r11, r11, r7, LSL r2
	mcr	p15, 0, r11, c7, c6, 2
	subs	r9, r9, #1
	bge	inv_d_cache_loop3
	subs	r7, r7, #1
	bge	inv_d_cache_loop2

	/* clear bss section */
	mov	r0, #0x0
	ldr	r1, =__BSS_START__
	ldr	r2, =__BSS_SIZE__
bss_loop:
	subs	r2, r2, #4
	bcc	bss_end
	str	r0, [r1, +r2]
	b	bss_loop
bss_end:

	/* copy data section */
	ldr	r0, =__DATA_COPY_START__
	ldr	r1, =__DATA_START__
	ldr	r2, =__DATA_SIZE__
data_loop:
	subs	r2, r2, #4
	bcc	data_end
	ldr	r3, [r0, +r2]
	str	r3, [r1, +r2]
	b	data_loop

data_end:

	/* trap to exception */
	ldr	r0, =__TCM_LOADER_COPY_START__
	ldr	r1, =__TCM_LOADER_START__
	ldr	r2, =__DUMMY_VECTOR_SIZE__
	ldr	r3, =__TCM_LOADER_SIZE__
	mov	r4, r1
	add	r0, r0, r3
dummy_vet_cp_loop:
	subs	r2, r2, #4
	bcc	dummy_vet_cp_end
	ldr	r3, [r0, +r2]
	str	r3, [r1, +r2]
	b	dummy_vet_cp_loop
dummy_vet_cp_end:

	dsb
	mcr 	p15, 0, r0, c7, c5, 0		/* ICIALLU */
	mcr	p15, 0, r0, c7, c5, 6		/* BPIALL */
	isb
	cpsie	a
	nop
	cpsid	a

	/* Load TCM Loader from System RAM to Instruction TCM */
	ldr	r0, =__ITCM_BASE__
	ldr	r1, =__TCM_LOADER_COPY_START__
	ldr	r2, =__TCM_LOADER_SIZE__
	bl	tcmcpy

	/* Loader Main */
	bl	loader_main

	/* Instruction cache invalidate & branch predictor invalidate */
	mcr	p15, 0, r0, c7, c5, 0		/* ICIALLU */
	mcr	p15, 0, r0, c7, c5, 6		/* BPIALL */
	isb

	/* Jump to TCM Loader */
	ldr	pc, = TCM_Vector
	nop
	nop
	nop

	.end
