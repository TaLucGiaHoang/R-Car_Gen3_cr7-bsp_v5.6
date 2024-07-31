/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#define	INT_ID_MASK	(0x3ff)
/* System watch dog timer for Assembly language */
#define	ARM_IRQ_WDT_A	(173)

	.globl	undef_report_exception
	.globl	irq_exception
	.globl	fiq_report_exception
	.globl	dabt_report_exception
	.globl	pabt_report_exception

	.extern swdt_exec
	.extern arm_gic_acknowledge_interrupt
	.extern undef_error
	.extern fiq_error
	.extern interrupt_error_id
	.extern	dabort_error
	.extern	pabort_error

undef_report_exception:
	b undef_error

fiq_report_exception:
	b fiq_error

irq_exception:
	bl	arm_gic_acknowledge_interrupt
	movw	r2, #INT_ID_MASK
	and	r0, r0, r2
	mov	r3, #ARM_IRQ_WDT_A
	cmp	r0, r3
	moveq	r0, r4
	beq	swdt_exec
rep_exec_panic_id:	/* interrupt id error	*/
	/* r0 is interrupt ID */
	b	interrupt_error_id

/********************************************************
 * abort exception
 ********************************************************/
dabt_report_exception:
	mrc	p15, 0, r1, c5, c0, 0	/* Read DFSR */
	mrc	p15, 0, r2, c6, c0, 0	/* Read DFAR */
	b dabort_error

pabt_report_exception:
	mrc	p15, 0, r0, c5, c0, 1	/* Read IFSR */
	mrc	p15, 0, r1, c6, c0, 2	/* Read IFAR */
	b pabort_error

	.end
