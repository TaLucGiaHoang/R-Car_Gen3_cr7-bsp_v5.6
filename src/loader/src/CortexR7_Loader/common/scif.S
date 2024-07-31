/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "scif.h"

/* Product Register */
#define	PRR			(0xFFF00044)
#define	PRR_PRODUCT_MASK	(0x00007F00)
#define PRR_CUT_MASK		(0x000000FF)
#define PRR_PRODUCT_CUT_MASK	(PRR_PRODUCT_MASK | PRR_CUT_MASK)
#define	PRR_PRODUCT_H3_VER_10	(0x00004F00)	/* R-Car H3 Ver.1.0 */
#define	PRR_PRODUCT_E3		(0x00005700)	/* R-Car E3 */

/* Pin functon */
#if !defined PFC_BASE
#define	PFC_BASE		(0xE6060000)
#endif
#define	PMMR			(0x0000)
#define	GPSR5			(0x0114)
#define	IPSR12			(0x0230)
#define	MOD_SEL1		(0x0504)

/* module stop */
#define	CPG_BASE		(0xE6150000)
#define	CPG_SMSTPCR3		(0x013C)
#define CPG_RMSTPCR3		(0x011C)
#define CPG_MSTPSR3		(0x0048)
#define	MSTP310			(1 << 10)
#define	CPG_CPGWPR		(0x0900)

/* SCIF2 */
#define	SCIF2_BASE		(0xE6E88000)	/* SCIF-2 base address */
#define	SCIF_BASE		(SCIF2_BASE)

/* Serial mode register */
#define	SCIF_SCSMR		(0x00)
/* Bit rate register */
#define	SCIF_SCBRR		(0x04)
/* Serial control register */
#define	SCIF_SCSCR		(0x08)
/* Transmit FIFO data register */
#define	SCIF_SCFTDR		(0x0C)
/* Serial status register */
#define	SCIF_SCFSR		(0x10)
/* Receive FIFO data register */
#define SCIF_SCFRDR		(0x14)
/* FIFO control register */
#define	SCIF_SCFCR		(0x18)
/* FIFO data count register */
#define	SCIF_SCFDR		(0x1C)
/* Serial port register */
#define SCIF_SCSPTR		(0x20)
/* Line status register */
#define	SCIF_SCLSR		(0x24)
/* Frequency division register */
#define	SCIF_DL			(0x30)
/* Clock Select register */
#define	SCIF_CKS		(0x34)

/* mode pin */
#define	RST_MODEMR		(0xE6160060)
#define	MODEMR_MD12		(0x00001000)	/* MD12 bit mask */

#define	SCSMR_CA_MASK		(1 << 7)
#define	SCSMR_CA_ASYNC		(0x0000)
#define	SCSMR_CHR_MASK		(1 << 6)
#define	SCSMR_CHR_8		(0x0000)
#define	SCSMR_PE_MASK		(1 << 5)
#define	SCSMR_PE_DIS		(0x0000)
#define	SCSMR_STOP_MASK		(1 << 3)
#define	SCSMR_STOP_1		(0x0000)
#define	SCSMR_CKS_MASK		(3 << 0)
#define	SCSMR_CKS_DIV1		(0x0000)
#define	SCSMR_INIT_DATA		(SCSMR_CA_ASYNC + SCSMR_CHR_8 + \
				SCSMR_PE_DIS + SCSMR_STOP_1 + \
				SCSMR_CKS_DIV1)

#define	SCBRR_230400BPS		(8)		/* 230400bps@66MHz */
#define	SCBRR_115200BPS_E3_SSCG	(15)		/* 115200bps@60MHz SSCG */
#define	SCBRR_115200BPS		(17)		/* 115200bps@66MHz */
#define	SCBRR_INIT_DATA		(SCBRR_230400BPS)

#define	SCSCR_TE_MASK		(1 << 5)
#define	SCSCR_TE_DIS		(0x0000)
#define	SCSCR_TE_EN		(0x0020)
#define	SCSCR_RE_MASK		(1 << 4)
#define	SCSCR_RE_DIS		(0x0000)
#define	SCSCR_RE_EN		(0x0010)
#define	SCSCR_CKE_MASK		(3 << 0)
#define SCSCR_CKE_INT		(0x0000)
#define SCSCR_CKE_BRG		(0x0002)
#if SCIF_CLK == SCIF_EXTARNAL_CLK
#define	SCSCR_CKE_INT_CLK	(SCSCR_CKE_BRG)
#else
#define	SCSCR_CKE_INT_CLK	(SCSCR_CKE_INT)
#endif

#define	SCFSR_TEND_MASK		(1 << 6)
#define	SCFSR_TEND_TRANS_END	(0x0040)
#define	SCFSR_INIT_DATA		(0x0000)

#define	SCFCR_TTRG_MASK		(3 << 4)
#define	SCFCR_TTRG_8		(0x0000)
#define	SCFCR_TTRG_0		(0x0030)
#define	SCFCR_TFRST_MASK	(1 << 2)
#define	SCFCR_TFRST_DIS		(0x0000)
#define	SCFCR_TFRST_EN		(0x0004)
#define	SCFCR_RFRS_MASK		(1 << 1)
#define	SCFCR_RFRS_DIS		(0x0000)
#define	SCFCR_RFRS_EN		(0x0002)
#define	SCFCR_INIT_DATA		(SCFCR_TTRG_8)

#define SCFDR_T_MASK		(0x1f << 8)

#define	DL_INIT_DATA		(8)		/* 14.7456MHz/(115200bps*16)*/

#define	CKS_CKS_DIV_MASK		(1 << 15)
#define	CKS_CKS_DIV_CLK		(0x0000)
#define	CKS_XIN_MASK		(1 << 14)
#define	CKS_XIN_SCIF_CLK		(0x0000)
#define	CKS_INIT_DATA		(CKS_CKS_DIV_CLK + CKS_XIN_SCIF_CLK)

#define	GPSR5_MASK		(3 << 10)
#define	GPSR5_RX2_A	(1 << 11)
#define	GPSR5_TX2_A	(1 << 10)
#define	GPSR5_SCK2	(1 << 9)
#if SCIF_CLK == SCIF_EXTARNAL_CLK
#define	GPSR5_INIT_DATA		(GPSR5_RX2_A + GPSR5_TX2_A + GPSR5_SCK2)
#else
#define	GPSR5_INIT_DATA		(GPSR5_RX2_A + GPSR5_TX2_A)
#endif

#define	IPSR12_MASK		(255 << 0)
#define	IPSR12_RX2_A		(0 << 4)
#define	IPSR12_TX2_A		(0 << 0)
#define	IPSR12_INIT_DATA	(IPSR12_RX2_A + IPSR12_TX2_A)

#define	MOD_SEL1_SCIF2_MASK	(1 << 12)
#define	MOD_SEL1_SCIF2		(0 << 12)



	.global	console_init
	.global	console_putc
	.global	console_wait

console_init:
	push	{r0, lr}
	ldr	r0, =(CPG_BASE | CPG_RMSTPCR3)
	ldr	r1, =(CPG_BASE | CPG_MSTPSR3)
	ldr	r2, =MSTP310
	bl	mstpcr_write

	ldr	r0, =SCIF2_BASE
	/* Clear bits TE and RE in SCSCR to 0 */
	mov	r1, #(SCSCR_TE_DIS + SCSCR_RE_DIS)	/* TE=0,RE=0 */
	strh	r1, [r0, #SCIF_SCSCR]
	/* Set bits TFRST and RFRST in SCFCR to 1 */
	ldrh	r1, [r0, #SCIF_SCFCR]
	/* TFRESET=1,RFRESET=1 */
	orr	r1, r1, #(SCFCR_TFRST_EN + SCFCR_RFRS_EN)
	strh	r1, [r0, #SCIF_SCFCR]
	/* Read flags of ER, DR, BRK, and RDF in SCFSR 		   */
	/* and those of TO and ORER in SCLSR, then clear them to 0 */
	mov	r1, #SCFSR_INIT_DATA
	strh	r1, [r0, #SCIF_SCFSR]
	mov	r1, #0
	strh	r1, [r0, #SCIF_SCLSR]
	/* Set bits CKE[1:0] in SCSCR */
	ldrh	r1, [r0, #SCIF_SCSCR]
	and	r1, r1, #~SCSCR_CKE_MASK
	mov	r2, #SCSCR_CKE_INT_CLK		/* CKE=00 */
	orr	r1, r1, r2
	strh	r1, [r0, #SCIF_SCSCR]
	/* Set data transfer format in SCSMR */
	mov	r1, #SCSMR_INIT_DATA
	/* CA=0,CHR=0,PE=0,STOP=0,CKS=P */
	strh	r1, [r0, #SCIF_SCSMR]
	/* Set value in SCBRR */
#if SCIF_CLK == SCIF_INTERNAL_CLK
	ldr	r1, =PRR
	ldr	r1, [r1]
	mov	r2, #PRR_PRODUCT_CUT_MASK
	and	r1, r1, r2
	mov	r2, #PRR_PRODUCT_H3_VER_10
	cmp	r1, r2
	beq	3f
	and	r1, r1, #PRR_PRODUCT_MASK
	mov	r2, #PRR_PRODUCT_E3
	cmp	r1, r2
	bne	4f

	ldr	r1, =RST_MODEMR
	ldr	r1, [r1]
	and	r1, r1, #MODEMR_MD12
	mov	r2, #MODEMR_MD12
	cmp	r1, r2
	bne	4f

	mov	r1, #SCBRR_115200BPS_E3_SSCG	/* 115200bps@60MHz SSCG */
	b	2f
4:
	mov	r1, #SCBRR_115200BPS		/* 115200bps */
	b	2f
3:
	/* H3 Ver.1.0 sets clock to doubling */
	mov	r1, #SCBRR_230400BPS
2:
	strb	r1, [r0, #SCIF_SCBRR]
#else
	mov	r1, #DL_INIT_DATA
	strh	r1, [r0, #SCIF_DL]
	mov	r1, #CKS_INIT_DATA		/* CKS=0,XIN=0 */
	strh	r1, [r0, #SCIF_CKS]
#endif
	/* 1-bit interval elapsed */
	mov	r1, #100			/* 100 times */
1:
	subs	r1, r1, #1
	bne	1b
	/* Set bits RTRG[1:0], TTRG[1:0], and MCE in SCFCR, */
	/* and clear bits FRST and RFRST to 0		    */
	mov	r1, #SCFCR_INIT_DATA
	strh	r1, [r0, #SCIF_SCFCR]
	/* Set bits TE and RE in SCSCR to 1 */
	ldrh	r1, [r0, #SCIF_SCSCR]
	orr	r1, r1, #(SCSCR_TE_EN + SCSCR_RE_EN)	/* TE=1,RE=1 */
	strh	r1, [r0, #SCIF_SCSCR]
	mov	r0, #1

	pop	{r0, pc}


console_putc:
	ldr	r1, =SCIF2_BASE
	cmp	r0, #0xA
	/* Prepend '\r' to '\n' */
	bne	2f
1:
	/* Check if the transmit FIFO is full */
	ldrh	r2, [r1, #SCIF_SCFDR]
	ubfx	r2, r2, #8, #5
	cmp	r2, #16
	bcs	1b
	mov	r2, #0x0D
	strb	r2, [r1, #SCIF_SCFTDR]
2:
	/* Check if the transmit FIFO is full */
	ldrh	r2, [r1, #SCIF_SCFDR]
	ubfx	r2, r2, #8, #5
	cmp	r2, #16
	bcs	2b
	strb	r0, [r1, #SCIF_SCFTDR]

	/* Clear TEND flag */
	ldrh	r2, [r1, #SCIF_SCFSR]
	and	r2, r2, #~SCFSR_TEND_MASK
	strh	r2, [r1, #SCIF_SCFSR]

	bx	lr


console_wait:
	ldr	r1, =SCIF2_BASE
1:
	/* Check TEND flag */
	ldrh	r2, [r1, #SCIF_SCFSR]
	and	r2, r2, #SCFSR_TEND_MASK
	cmp	r2, #SCFSR_TEND_TRANS_END
	bne	1b

	bx	lr


	.end
