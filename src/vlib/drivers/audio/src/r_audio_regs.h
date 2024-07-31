/*************************************************************************************************************
* Copyright (c) [2023] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_AUDIO_REGS_H_
#define R_AUDIO_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r_audio_core.h"

/* SSIU */
#define	SSIU_SSI_MODE0			0xEC540800	/* R/W	32	SSI mode register 0 */
#define	SSIU_SSI_MODE1			0xEC540804	/* R/W	32	SSI mode register 1 */
#define	SSIU_SSI_MODE2			0xEC540808	/* R/W	32	SSI mode register 2 */
#define	SSIU_SSI_MODE3			0xEC54080C	/* R/W	32	SSI mode register 3 */
#define	SSIU_SSI_CONTROL		0xEC540810	/* R/W	32	SSI control register */
#define	SSIU_SSI_CONTROL2		0xEC540814	/* R/W	32	SSI control register2 */

/* SSI */
#define R_SSI0_BASE		0xEC541000		/* SSI0 base address */
#define R_SSI1_BASE		0xEC541040		/* SSI1 base address */
#define R_SSI2_BASE		0xEC541080		/* SSI2 base address */
#define R_SSI3_BASE		0xEC5410C0		/* SSI3 base address */
#define R_SSI4_BASE		0xEC541100		/* SSI4 base address */
#define R_SSI5_BASE		0xEC541140		/* SSI5 base address */
#define R_SSI6_BASE		0xEC541180		/* SSI6 base address */
#define R_SSI7_BASE		0xEC5411C0		/* SSI7 base address */
#define R_SSI8_BASE		0xEC541200		/* SSI8 base address */
#define R_SSI9_BASE		0xEC541240		/* SSI9 base address */

#define	SSI_CR				0x0000	/* Control register 0 */
#define	SSI_SR				0x0004	/* Status register 0 */
#define	SSI_TDR				0x0008	/* Transmit data register 0 */
#define	SSI_RDR				0x000C	/* Receive data register 0 */
#define	SSI_WSR				0x0020	/* WS mode register 0 */
#define	SSI_FMR				0x0024	/* FS mode register 0 */
#define	SSI_FSR				0x0028	/* FS status register 0 */
#define	SSI_CRE				0x0030	/* Control register extend 0 */

#define	SSI_SSITDR0_DMA			0xEC241008	/* R/W	32	Transmit data register 0 */
#define	SSI_SSIRDR0_DMA			0xEC24100C	/* R	32	Receive data register 0 */
#define	SSI_SSITDR1_DMA			0xEC241048	/* R/W	32	Transmit data register 1 */
#define	SSI_SSIRDR1_DMA			0xEC24104C	/* R	32	Receive data register 1 */
#define	SSI_SSITDR2_DMA			0xEC241088	/* R/W	32	Transmit data register 2 */
#define	SSI_SSIRDR2_DMA			0xEC24108C	/* R	32	Receive data register 2 */
#define	SSI_SSITDR3_DMA			0xEC2410C8	/* R/W	32	Transmit data register 3 */
#define	SSI_SSIRDR3_DMA			0xEC2410CC	/* R	32	Receive data register 3 */
#define	SSI_SSITDR4_DMA			0xEC241108	/* R/W	32	Transmit data register 4 */
#define	SSI_SSIRDR4_DMA			0xEC24110C	/* R	32	Receive data register 4 */
#define	SSI_SSITDR5_DMA			0xEC241148	/* R/W	32	Transmit data register 5 */
#define	SSI_SSIRDR5_DMA			0xEC24114C	/* R	32	Receive data register 5 */
#define	SSI_SSITDR6_DMA			0xEC241188	/* R/W	32	Transmit data register 6 */
#define	SSI_SSIRDR6_DMA			0xEC24118C	/* R	32	Receive data register 6 */
#define	SSI_SSITDR7_DMA			0xEC2411C8	/* R/W	32	Transmit data register 7 */
#define	SSI_SSIRDR7_DMA			0xEC2411CC	/* R	32	Receive data register 7 */
#define	SSI_SSITDR8_DMA			0xEC241208	/* R/W	32	Transmit data register 8 */
#define	SSI_SSIRDR8_DMA			0xEC24120C	/* R	32	Receive data register 8 */
#define	SSI_SSITDR9_DMA			0xEC241248	/* R/W	32	Transmit data register 9 */
#define	SSI_SSIRDR9_DMA			0xEC24124C	/* R	32	Receive data register 9 */

/* ADG */
#define	ADG_BRRA			    0xEC5A0000	/* R/W	32	BRGA baud rate setting register */
#define	ADG_BRRB			    0xEC5A0004	/* R/W	32	BRGB baud rate setting register */
#define	ADG_SSICKR			    0xEC5A0008	/* R/W	32	Clock select register */
#define	ADG_AUDIO_CLK_SEL0		0xEC5A000C	/* R/W	32	Audio clock select register 0 */
#define	ADG_AUDIO_CLK_SEL1		0xEC5A0010	/* R/W	32	Audio clock select register 1 */
#define	ADG_AUDIO_CLK_SEL2		0xEC5A0014	/* R/W	32	Audio clock select register 2 */
#define	ADG_DIV_EN			    0xEC5A0030	/* R/W	32	Audio clock frequency division enable register */
#define	ADG_SRCIN_TIMSEL0		0xEC5A0034	/* R/W	32	SRC input timing select register 0 */
#define	ADG_SRCIN_TIMSEL1		0xEC5A0038	/* R/W	32	SRC input timing select register 1 */
#define	ADG_SRCIN_TIMSEL2		0xEC5A003C	/* R/W	32	SRC input timing select register 2 */
#define	ADG_SRCIN_TIMSEL3		0xEC5A0040	/* R/W	32	SRC input timing select register 3 */
#define	ADG_SRCIN_TIMSEL4		0xEC5A0044	/* R/W	32	SRC input timing select register 4 */
#define	ADG_SRCOUT_TIMSEL0		0xEC5A0048	/* R/W	32	SRC output timing select register 0 */
#define	ADG_SRCOUT_TIMSEL1		0xEC5A004C	/* R/W	32	SRC output timing select register 1 */
#define	ADG_SRCOUT_TIMSEL2		0xEC5A0050	/* R/W	32	SRC output timing select register 2 */
#define	ADG_SRCOUT_TIMSEL3		0xEC5A0054	/* R/W	32	SRC output timing select register 3 */
#define	ADG_SRCOUT_TIMSEL4		0xEC5A0058	/* R/W	32	SRC output timing select register 4 */
#define	ADG_CMDOUT_TIMSEL		0xEC5A005C	/* R/W	32	CMD output timing select register */
#define	ADG_ADSP_TIMSEL			0xEC5A0060	/* R/W	32	ADSP outputtiming select register */
#define	ADG_DTCP_TIMSEL			0xEC5A0064	/* R/W	32	DTCP timing select register */
#define	ADG_ADSP_TIMSEL1		0xEC5A0080	/* R/W	32	ADSP outputtiming select register1 */
#define	ADG_ADSP_TIMSEL2		0xEC5A0084	/* R/W	32	ADSP outputtiming select register2 */
#define	ADG_ADSP_TIMSEL3		0xEC5A0088	/* R/W	32	ADSP outputtiming select register3 */
#define	ADG_SSICKR1			    0xEC5A0100	/* R/W	32	Clock select register1 */
#define	ADG_AVB_SYNC_SEL0		0xEC5A0104	/* R/W	32	AVB sync Select register0 */
#define	ADG_AVB_SYNC_SEL1		0xEC5A010C	/* R/W	32	AVB sync Select register1 */
#define	ADG_AVB_SYNC_SEL2		0xEC5A0110	/* R/W	32	AVB sync Select register2 */
#define	ADG_AVB_SYNC_DIV0		0xEC5A0114	/* R/W	32	AVB sync divide register0 */
#define	ADG_AVB_SYNC_DIV1		0xEC5A0118	/* R/W	32	AVB sync divide  register1 */
#define	ADG_AVB_CLK_DIV0		0xEC5A011C	/* R/W	32	AVB clock divider comparison value register0 */
#define	ADG_AVB_CLK_DIV1		0xEC5A0120	/* R/W	32	AVB clock divider comparison value register1 */
#define	ADG_AVB_CLK_DIV2		0xEC5A0124	/* R/W	32	AVB clock divider comparison value register2 */
#define	ADG_AVB_CLK_DIV3		0xEC5A0128	/* R/W	32	AVB clock divider comparison value register3 */
#define	ADG_AVB_CLK_DIV4		0xEC5A012C	/* R/W	32	AVB clock divider comparison value register4 */
#define	ADG_AVB_CLK_DIV5		0xEC5A0130	/* R/W	32	AVB clock divider comparison value register5 */
#define	ADG_AVB_CLK_DIV6		0xEC5A0134	/* R/W	32	AVB clock divider comparison value register6 */
#define	ADG_AVB_CLK_DIV7		0xEC5A0138	/* R/W	32	AVB clock divider comparison value register7 */
#define	ADG_AVB_CLK_CONFIG		0xEC5A013C	/* R/W	32	AVB clock divider configuration register */
#define	ADG_AVB_CLK_STATUS		0xEC5A0140	/* R/WC1	32	AVB clock divider status register */

void writel(const uint32_t Value, const uintptr_t Address);
uint32_t readl(const uintptr_t Address);

#ifdef __cplusplus
}
#endif

#endif /* R_AUDIO_REGS_H_ */

