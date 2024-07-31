/*
 * Copyright (c) 2015-2020, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef EMMC_REGISTERS_H
#define EMMC_REGISTERS_H

// #define RCAR_H3	0
// #define RCAR_M3	1
// #define RCAR_M3N	2
// #define RCAR_E3	3	/* NON */
// #define RCAR_H3N	4

// #define RCAR_LSI RCAR_H3

/* MMC channel select */
#define MMC_CH0		(0U)	/* SDHI2/MMC0 */
#define MMC_CH1		(1U)	/* SDHI3/MMC1 */

// #if (RCAR_LSI == RCAR_E3)  || (RCAR_LSI == RZ_G2M)
// #define USE_MMC_CH	(MMC_CH1)	/* R-Car E3 or RZ/G2M */
// #else /* RCAR_LSI == RCAR_E3 || RCAR_LSI == RZ_G2M */
// #define USE_MMC_CH	(MMC_CH0)	/* R-Car H3/M3/M3N */
// #endif /* RCAR_LSI == RCAR_E3 || RCAR_LSI == RZ_G2M */

#define USE_MMC_CH	(MMC_CH0)	/* R-Car H3/M3/M3N */

#define BIT0	(0x00000001U)
#define BIT1	(0x00000002U)
#define BIT2	(0x00000004U)
#define BIT3	(0x00000008U)
#define BIT4	(0x00000010U)
#define BIT5	(0x00000020U)
#define BIT6	(0x00000040U)
#define BIT7	(0x00000080U)
#define BIT8	(0x00000100U)
#define BIT9	(0x00000200U)
#define BIT10	(0x00000400U)
#define BIT11	(0x00000800U)
#define BIT12	(0x00001000U)
#define BIT13	(0x00002000U)
#define BIT14	(0x00004000U)
#define BIT15	(0x00008000U)
#define BIT16	(0x00010000U)
#define BIT17	(0x00020000U)
#define BIT18	(0x00040000U)
#define BIT19	(0x00080000U)
#define BIT20	(0x00100000U)
#define BIT21	(0x00200000U)
#define BIT22	(0x00400000U)
#define BIT23	(0x00800000U)
#define BIT24	(0x01000000U)
#define BIT25	(0x02000000U)
#define BIT26	(0x04000000U)
#define BIT27	(0x08000000U)
#define BIT28	(0x10000000U)
#define BIT29	(0x20000000U)
#define BIT30	(0x40000000U)
#define BIT31	(0x80000000U)

/* Clock Pulse Generator (CPG) registers */
#define CPG_BASE	(0xE6150000U)
/* Module stop status register 3 */
#define CPG_MSTPSR3	(CPG_BASE + 0x0048U)
/* System module stop control register 3 */
#define CPG_SMSTPCR3	(CPG_BASE + 0x013CU)
/* SDHI2 clock frequency control register */
#define CPG_SD2CKCR	(CPG_BASE + 0x0268U)
/* SDHI3 clock frequency control register */
#define CPG_SD3CKCR	(CPG_BASE + 0x026CU)
/* CPG Write Protect Register */
#define CPG_CPGWPR	(CPG_BASE + 0x0900U)

#if USE_MMC_CH == MMC_CH0
#define CPG_SDxCKCR		(CPG_SD2CKCR)	/* SDHI2/MMC0 */
#else /* USE_MMC_CH == MMC_CH0 */
#define CPG_SDxCKCR		(CPG_SD3CKCR)	/* SDHI3/MMC1 */
#endif /* USE_MMC_CH == MMC_CH0 */

/* Boot Status register */
#define  MFISBTSTSR			(0xE6260604U)

#define  MFISBTSTSR_BOOT_PARTITION	(0x00000010U)

/* eMMC registers */
#define MMC0_SD_BASE		(0xEE140000U)
#define MMC1_SD_BASE		(0xEE160000U)

#if USE_MMC_CH == MMC_CH0
#define MMC_SD_BASE		(MMC0_SD_BASE)
#else /* USE_MMC_CH == MMC_CH0 */
#define MMC_SD_BASE		(MMC1_SD_BASE)
#endif /* USE_MMC_CH == MMC_CH0 */

#define SD_CMD			(MMC_SD_BASE + 0x0000U)
#define SD_PORTSEL		(MMC_SD_BASE + 0x0008U)
#define SD_ARG			(MMC_SD_BASE + 0x0010U)
#define SD_ARG1			(MMC_SD_BASE + 0x0018U)
#define SD_STOP			(MMC_SD_BASE + 0x0020U)
#define SD_SECCNT		(MMC_SD_BASE + 0x0028U)
#define SD_RSP10		(MMC_SD_BASE + 0x0030U)
#define SD_RSP1			(MMC_SD_BASE + 0x0038U)
#define SD_RSP32		(MMC_SD_BASE + 0x0040U)
#define SD_RSP3			(MMC_SD_BASE + 0x0048U)
#define SD_RSP54		(MMC_SD_BASE + 0x0050U)
#define SD_RSP5			(MMC_SD_BASE + 0x0058U)
#define SD_RSP76		(MMC_SD_BASE + 0x0060U)
#define SD_RSP7			(MMC_SD_BASE + 0x0068U)
#define SD_INFO1		(MMC_SD_BASE + 0x0070U)
#define SD_INFO2		(MMC_SD_BASE + 0x0078U)
#define SD_INFO1_MASK		(MMC_SD_BASE + 0x0080U)
#define SD_INFO2_MASK		(MMC_SD_BASE + 0x0088U)
#define SD_CLK_CTRL		(MMC_SD_BASE + 0x0090U)
#define SD_SIZE			(MMC_SD_BASE + 0x0098U)
#define SD_OPTION		(MMC_SD_BASE + 0x00A0U)
#define SD_ERR_STS1		(MMC_SD_BASE + 0x00B0U)
#define SD_ERR_STS2		(MMC_SD_BASE + 0x00B8U)
#define SD_BUF0			(MMC_SD_BASE + 0x00C0U)
#define SDIO_MODE		(MMC_SD_BASE + 0x00D0U)
#define SDIO_INFO1		(MMC_SD_BASE + 0x00D8U)
#define SDIO_INFO1_MASK		(MMC_SD_BASE + 0x00E0U)
#define CC_EXT_MODE		(MMC_SD_BASE + 0x0360U)
#define SOFT_RST		(MMC_SD_BASE + 0x0380U)
#define VERSION			(MMC_SD_BASE + 0x0388U)
#define HOST_MODE		(MMC_SD_BASE + 0x0390U)
#define SDIF_MODE		(MMC_SD_BASE + 0x0398U)
#define DM_CM_DTRAN_MODE	(MMC_SD_BASE + 0x0820U)
#define DM_CM_DTRAN_CTRL	(MMC_SD_BASE + 0x0828U)
#define DM_CM_RST		(MMC_SD_BASE + 0x0830U)
#define DM_CM_INFO1		(MMC_SD_BASE + 0x0840U)
#define DM_CM_INFO1_MASK	(MMC_SD_BASE + 0x0848U)
#define DM_CM_INFO2		(MMC_SD_BASE + 0x0850U)
#define DM_CM_INFO2_MASK	(MMC_SD_BASE + 0x0858U)
#define DM_DTRAN_ADDR		(MMC_SD_BASE + 0x0880U)
#define SCC_DTCNTL			(MMC_SD_BASE + 0x1000U)
#define SCC_TAPSET			(MMC_SD_BASE + 0x1008U)
#define SCC_DT2FF			(MMC_SD_BASE + 0x1010U)
#define SCC_CKSEL			(MMC_SD_BASE + 0x1018U)
#define SCC_RVSCNTL			(MMC_SD_BASE + 0x1020U)
#define SCC_RVSREQ			(MMC_SD_BASE + 0x1028U)
#define SCC_SMPCMP			(MMC_SD_BASE + 0x1030U)
#define SCC_TMPPORT2		(MMC_SD_BASE + 0x1038U)
#define SCC_TMPPORT3		(MMC_SD_BASE + 0x1050U)
#define SCC_TMPPORT4		(MMC_SD_BASE + 0x1058U)
#define SCC_TMPPORT5		(MMC_SD_BASE + 0x1060U)
#define SCC_TMPPORT6		(MMC_SD_BASE + 0x1068U)
#define SCC_TMPPORT7		(MMC_SD_BASE + 0x1070U)

/* SD_INFO1 Registers */
#define SD_INFO1_HPIRES		0x00010000UL /* Response Reception Completion */
#define SD_INFO1_INFO10		0x00000400UL /* Indicates the SDDAT3 state */
#define SD_INFO1_INFO9		0x00000200UL /* SDDAT3 Card Insertion */
#define SD_INFO1_INFO8		0x00000100UL /* SDDAT3 Card Removal */
#define SD_INFO1_INFO7		0x00000080UL /* Write Protect */
#define SD_INFO1_INFO5		0x00000020UL /* Indicates the ISDCD state */
#define SD_INFO1_INFO4		0x00000010UL /* ISDCD Card Insertion */
#define SD_INFO1_INFO3		0x00000008UL /* ISDCD Card Removal */
#define SD_INFO1_INFO2		0x00000004UL /* Access end */
#define SD_INFO1_INFO0		0x00000001UL /* Response end */

/* SD_INFO2 Registers */
#define SD_INFO2_ILA		0x00008000UL /* Illegal Access Error */
#define SD_INFO2_CBSY		0x00004000UL /* Command Type Register Busy */
#define SD_INFO2_SCLKDIVEN	0x00002000UL
#define SD_INFO2_BWE		0x00000200UL /* SD_BUF Write Enable */
#define SD_INFO2_BRE		0x00000100UL /* SD_BUF Read Enable */
#define SD_INFO2_DAT0		0x00000080UL /* SDDAT0 */
#define SD_INFO2_ERR6		0x00000040UL /* Response Timeout */
#define SD_INFO2_ERR5		0x00000020UL /* SD_BUF Illegal Read Access */
#define SD_INFO2_ERR4		0x00000010UL /* SD_BUF Illegal Write Access */
#define SD_INFO2_ERR3		0x00000008UL /* Data Timeout */
#define SD_INFO2_ERR2		0x00000004UL /* END Error */
#define SD_INFO2_ERR1		0x00000002UL /* CRC Error */
#define SD_INFO2_ERR0		0x00000001UL /* CMD Error */
#define SD_INFO2_ALL_ERR	0x0000807FUL
#define SD_INFO2_CLEAR		0x00000800UL /* BIT11 write value should always be 1. HWM_0003 */

/* SOFT_RST */
#define SOFT_RST_SDRST		0x00000001UL

/* SD_CLK_CTRL */
#define SD_CLK_CTRL_SDCLKOFFEN		0x00000200UL
#define SD_CLK_CTRL_SCLKEN		0x00000100UL
#define SD_CLK_CTRL_CLKDIV_MASK		0x000000FFUL
#define SD_CLOCK_ENABLE			0x00000100UL
#define SD_CLOCK_DISABLE		0x00000000UL
#define SD_CLK_WRITE_MASK		0x000003FFUL
#define SD_CLK_CLKDIV_CLEAR_MASK	0xFFFFFF0FUL

/* SD_OPTION */
#define SD_OPTION_TIMEOUT_CNT_MASK	0x000000F0UL

/*
 * MMC Clock Frequency
 * 200MHz * 1/x = output clock
 */
#define MMC_CLK_OFF		0UL   /* Clock output is disabled */
#define MMC_400KHZ		512UL /* 200MHz * 1/512 = 390 KHz */
#define MMC_20MHZ		16UL  /* 200MHz * 1/16 = 12.5 MHz Normal speed mode */
#define MMC_26MHZ		8UL   /* 200MHz * 1/8 = 25 MHz HS mode 26Mhz */
#define MMC_52MHZ		4UL   /* 200MHz * 1/4 = 50 MHz HS mode 52Mhz */
#define MMC_100MHZ		2UL   /* 200MHz * 1/2 = 100 MHz */
#define MMC_200MHZ		1UL   /* 200MHz * 1/1 = 200 MHz */

#define MMC_FREQ_52MHZ		52000000UL
#define MMC_FREQ_26MHZ		26000000UL
#define MMC_FREQ_20MHZ		20000000UL

/* MMC Clock DIV */
#define MMC_SD_CLK_START	0x00000100UL	/* CLOCK On */
#define MMC_SD_CLK_STOP		(~0x00000100UL)	/* CLOCK stop */
#define MMC_SD_CLK_DIV1		0x000000FFUL	/* 1/1 */
#define MMC_SD_CLK_DIV2		0x00000000UL	/* 1/2 */
#define MMC_SD_CLK_DIV4		0x00000001UL	/* 1/4 */
#define MMC_SD_CLK_DIV8		0x00000002UL	/* 1/8 */
#define MMC_SD_CLK_DIV16	0x00000004UL	/* 1/16 */
#define MMC_SD_CLK_DIV32	0x00000008UL	/* 1/32 */
#define MMC_SD_CLK_DIV64	0x00000010UL	/* 1/64 */
#define MMC_SD_CLK_DIV128	0x00000020UL	/* 1/128 */
#define MMC_SD_CLK_DIV256	0x00000040UL	/* 1/256 */
#define MMC_SD_CLK_DIV512	0x00000080UL	/* 1/512 */

/* DM_CM_DTRAN_MODE */
#define DM_CM_DTRAN_MODE_CH0		0x00000000UL	/* CH0(downstream) */
#define DM_CM_DTRAN_MODE_CH1		0x00010000UL	/* CH1(upstream)   */
#define DM_CM_DTRAN_MODE_BIT_WIDTH	0x00000030UL

/* CC_EXT_MODE */
#define CC_EXT_MODE_DMASDRW_ENABLE	0x00000002UL	/* SD_BUF Read/Write DMA Transfer */
#define CC_EXT_MODE_CLEAR		0x00001010UL	/* BIT 12 & 4 always 1. */

/* SDIF_MODE */
#define SDIF_MODE_HS400			(1 << 0)

/* DM_CM_INFO_MASK */
#define DM_CM_INFO_MASK_CLEAR		0xFFFCFFFEUL
#define DM_CM_INFO_CH0_ENABLE		0x00010001UL
#define DM_CM_INFO_CH1_ENABLE		0x00020001UL

/* DM_DTRAN_ADDR */
#define DM_DTRAN_ADDR_WRITE_MASK	0xFFFFFFF8UL

/* DM_CM_DTRAN_CTRL */
#define DM_CM_DTRAN_CTRL_START		0x00000001UL

/* SCC_DTCNTL register */
#define SCC_DTCNTL_TAPEN	(1 << 0)

/* SCC_CKSEL register */
#define SCC_CKSEL_DTSEL       (1 << 0)

/* SCC_RVSCNTL register */
#define SCC_RVSCNTL_RVSEN     (1 << 0)

/* SCC_RVSREQ register */
#define SCC_RVSREQ_RVSERR     (1 << 2)
#define SCC_RVSREQ_REQTAPUP   (1 << 1)
#define SCC_RVSREQ_REQTAPDWN  (1 << 0)
#define SCC_RVSREQ_REQTAPMSK  (3 << 0)

/* SCC_SMPCMP register */
#define SCC_SMPCMP_CMD_ERR      ((1 << 24) | (1 << 8))
#define SCC_SMPCMP_CMD_REQUP     (1 << 24)
#define SCC_SMPCMP_CMD_REQDOWN   (1 << 8)

/* SCC_TMPPORT2 register */
#define SCC_TMPPORT2_HS400OSEL	(1 << 4)
#define SCC_TMPPORT2_HS400EN	(1 << 31)

/* SCC_TMPPORT3 register */
#define SCC_TMPPORT3_OFFSET_0	3
#define SCC_TMPPORT3_OFFSET_1	2
#define SCC_TMPPORT3_OFFSET_2	1
#define SCC_TMPPORT3_OFFSET_3	0

/* SCC_TMPPORT3 register */
#define SCC_TMPPORT3_OFFSET_0	3
#define SCC_TMPPORT3_OFFSET_1	2
#define SCC_TMPPORT3_OFFSET_2	1
#define SCC_TMPPORT3_OFFSET_3	0

/* SCC_TMPPORT4 register */
#define SCC_TMPPORT4_DLL_ACC_START  (1 << 0)

/* SCC_TMPPORT5 register */
#define SCC_TMPPORT5_DLL_RW_SEL_R	(1 << 8)
#define SCC_TMPPORT5_DLL_RW_SEL_W	(0 << 8)
#define SCC_TMPPORT5_DLL_ADR_MASK	0x3F

/* SCC register */
#define SCC_TMPPORT_DISABLE_WP_CODE	0xa5000000
#define SCC_TMPPORT_CALIB_CODE_MASK	0x1f
#define SCC_TMPPORT_MANUAL_MODE		(1 << 7)
#define CALIB_TABLE_MAX	            (SCC_TMPPORT_CALIB_CODE_MASK + 1)

/* CPG API */
#if USE_MMC_CH == MMC_CH0
#define CPG_MSTP_MMC_CLOCK  (R_CPG_SDIF2_CLOCK) /* SDHI2/MMC0 */
#else /* USE_MMC_CH == MMC_CH0 */
#define CPG_MSTP_MMC_CLOCK  (R_CPG_SDIF3_CLOCK) /* SDHI3/MMC1 */
#endif /* USE_MMC_CH == MMC_CH0 */

#endif /* EMMC_REGISTERS_H */
