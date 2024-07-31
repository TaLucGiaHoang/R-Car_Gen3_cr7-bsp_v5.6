/*
 * Copyright (c) 2015-2016, Renesas Electronics Corporation
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

//#include <arch.h>
//#include <asm_macros.S>
#include <scif.h>

/* Product Register */
#define	PRR			(0xFFF00044)
#define	PRR_PRODUCT_MASK	(0x00007FFF)
#define	PRR_PRODUCT_H3_ES_1_0	(0x00004F00)	/* R-Car H3 ES1.0 */


	.global	console_init
	.global	console_putc
	.global	console_wait

console_init:
	ldr	r0, =CPG_BASE
	ldr	r1, [r0, #CPG_SMSTPCR3]
	and	r1, r1, #~MSTP310		/* MSTP310=0 */
	mvn	r2, r1
	str	r2, [r0, #CPG_CPGWPR]
	str	r1, [r0, #CPG_SMSTPCR3]

	ldr	r0, =PFC_BASE
	ldr	r1, [r0, #MOD_SEL1]
	and	r1, r1, #~MOD_SEL1_SCIF2_MASK
	mvn	r2, r1
	str	r2, [r0, #PMMR]
	str	r1, [r0, #MOD_SEL1]

	ldr	r1, [r0, #IPSR12]
	and	r1, r1, #~IPSR12_MASK
	mov	r2, #IPSR12_INIT_DATA		/* IP12[7:4]=H'0, IP12[3:0]=H'0 */
	orr	r1, r1, r2
	mvn	r2, r1
	str	r2, [r0, #PMMR]
	str	r1, [r0, #IPSR12]

	ldr	r1, [r0, #GPSR5]
	and	r1, r1, #~GPSR5_MASK		/* GP2[12]=Peripheral function selected by IP4[23:22] */
	orr	r1, r1, #GPSR5_INIT_DATA	/* GP2[13]=Peripheral function selected by IP4[25:24] */
	mvn	r2, r1
	str	r2, [r0, #PMMR]
	str	r1, [r0, #GPSR5]

	ldr	r0, =SCIF2_BASE
	mov	r1, #0
	strh	r1, [r0, #SCIF_SCSMRIR]
	/* Clear bits TE and RE in SCSCR to 0 */
	mov	r1, #(SCSCR_TE_DIS + SCSCR_RE_DIS)	/* TE=0,RE=0 */
	strh	r1, [r0, #SCIF_SCSCR]
	/* Set bits TFRST and RFRST in SCFCR to 1 */
	ldrh	r1, [r0, #SCIF_SCFCR]
	orr	r1, r1, #(SCFCR_TFRST_EN + SCFCR_RFRS_EN)	/* TFRESET=1,RFRESET=1 */
	strh	r1, [r0, #SCIF_SCFCR]
	/* Read flags of ER, DR, BRK, and RDF in SCFSR and those of TO and ORER in SCLSR, then clear them to 0 */
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
	strh	r1, [r0, #SCIF_SCSMR]		/* CA=0,CHR=0,PE=0,STOP=0,CKS=P */
	/* Set value in SCBRR */
#if SCIF_CLK == SCIF_INTERNAL_CLK
	ldr	r1, =PRR
	ldr	r1, [r1]
	ldr	r2, =PRR_PRODUCT_MASK
	and	r1, r1, r2
	mov	r2, #PRR_PRODUCT_H3_ES_1_0
	cmp	r1, r2
	beq	3f
	mov	r1, #SCBRR_115200BPS		/* 115200bps */
	b	2f
3:
	mov	r1, #SCBRR_230400BPS		/* H3 ES1.0 sets clock to doubling */
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
	/* Set bits RTRG[1:0], TTRG[1:0], and MCE in SCFCR, and clear bits FRST and RFRST to 0 */
	mov	r1, #SCFCR_INIT_DATA
	strh	r1, [r0, #SCIF_SCFCR]
	/* Set bits TE and RE in SCSCR to 1 */
	ldrh	r1, [r0, #SCIF_SCSCR]
	orr	r1, r1, #(SCSCR_TE_EN + SCSCR_RE_EN)	/* TE=1,RE=1 */
	strh	r1, [r0, #SCIF_SCSCR]
	mov	r1, #1

	bx	lr


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

	bx	lr


console_wait:
	ldr	r1, =SCIF2_BASE
1:
	ldrh	r2, [r1, #SCIF_SCFDR]
	ubfx	r2, r2, #8, #5
	cmp	r2, #0
	bne	1b

	bx	lr


	.end
