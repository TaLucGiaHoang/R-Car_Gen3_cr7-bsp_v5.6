/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */


#define	DRAM_BASE		(0x40000000)
#define	SYSRAM_BASE		(0xE6300000)
#define ITCM_BASE		(0xEB000000)
#define	DTCM_BASE		(0xEB020000)


#define	RTOS_BASE		(SYSRAM_BASE | 0x3000)
#define	CA5XLOADER_BASE		(DRAM_BASE | 0x03F20000)

#define	CPG_CPGWPR		(0xE6150900)
#define	APMU_CA57WUPCR		(0xE6152010)
#define	RST_CA57RESCNT		(0xE6160040)
#define	CA57CPU0BARH		(0xE61600C0)
#define	CA57CPU0BARL		(0xE61600C4)
#define	RST_CA53RESCNT		(0xE6160044)
#define	CA53CPU0BARH		(0xE6160080)
#define	CA53CPU0BARL		(0xE6160084)
#define	SYSC_SYSCISR		(0xE6180004)
#define	SYSC_SYSCSR		(0xE6180000)
#define	SYSC_SYSCIER		(0xE618000C)
#define	SYSC_SYSCIMR		(0xE6180010)
#define	SYSC_PWRONCR5		(0xE61801CC)
#define	SYSC_PWRER5		(0xE61801D4)

#define	RESCNT_KEYWORD		(0xA5A50000)

#define	STACK_BASE_ABT		(DTCM_BASE | 0x7080)
#define	STACK_BASE_UND		(DTCM_BASE | 0x7100)
#define	STACK_BASE_FIQ		(DTCM_BASE | 0x7180)
#define	STACK_BASE_IRQ		(DTCM_BASE | 0x7200)
#define	STACK_BASE_SVC		(DTCM_BASE | 0x8000)

#define SCTLR_I			(12)
#define SCTLR_Z			(11)

#define ITCMRR_ENABLE		(0x00000001)

	.global	TCM_Vector
	.global	do_panic

/*****************************************************************************
 *	Vector table
 *****************************************************************************/
	.align	5

TCM_Vector:
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
	bl	tcm_loader_main			/* r0 = RTOS boot address */

	/* Instruction cache invaldate & disable */
	mrc	p15, 0, r1, c1, c0, 0		/* SCTLR */
	bic	r1, r1, #0x1 << SCTLR_I		/* I = 0 */
	bic	r1, r1, #0x1 << SCTLR_Z		/* Z = 0 */
	mcr	p15, 0, r1, c1, c0, 0
	mcr	p15, 0, r1, c7, c5, 0		/* ICIALLU */
	mcr	p15, 0, r1, c7, c5, 6		/* BPIALL */
	isb

	/* Disable exception */
	cpsid a

	/* TCM configuration */
	ldr	r1, =(ITCM_BASE | ITCMRR_ENABLE)	/* ITCMRR_INIT_DATA */
	mcr	p15, 0, r1, c9, c1, 1

	/* Jump to RTOS */
	bx	r0				/* RTOS_BASE */

do_panic:
	nop
1:
	wfe
	b	1b


/*****************************************************************************
 *	Exception Handers
 *****************************************************************************/

	/* Undefined Instruction */
Undef:
	sub r0, lr, #4
	b	undef_report_exception

	/* Supervisor Call */
SWI:
	nop
1:
	wfe
	b	1b

	/* Prefetch Abort */
PAbort:
	sub	r0, lr, #4
	b	pabt_report_exception

	/* Data Abort */
DAbort:
	sub	r0, lr, #8
	b	dabt_report_exception

	/* IRQ interrupt */
IRQ:
	sub r4, lr, #4
	b	irq_exception

	/* FIQ interrupt */
FIQ:
	b	fiq_report_exception


	.end
