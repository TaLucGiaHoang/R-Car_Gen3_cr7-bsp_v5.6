/*
 * Copyright (c) 2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#ifndef SINGLE_ECC_CONF_H__
#define SINGLE_ECC_CONF_H__

#include "single_ecc.h"

/*****************************************************************/
/* The following definitions and declarations can be customized. */
/* Definitions of each values are defined in single_ecc.h.       */
/*****************************************************************/

/* Enable or Disable DRAM Single Channel ECC */
/* Write to bit31 of DFUSAAREACRn */
#define DFUSA0_EN		(SINGLE_ECC_ENABLE)	/* Area0  */
#define DFUSA1_EN		(SINGLE_ECC_DISABLE)	/* Area1  */
#define DFUSA2_EN		(SINGLE_ECC_DISABLE)	/* Area2  */
#define DFUSA3_EN		(SINGLE_ECC_DISABLE)	/* Area3  */
#define DFUSA4_EN		(SINGLE_ECC_DISABLE)	/* Area4  */
#define DFUSA5_EN		(SINGLE_ECC_DISABLE)	/* Area5  */
#define DFUSA6_EN		(SINGLE_ECC_DISABLE)	/* Area6  */
#define DFUSA7_EN		(SINGLE_ECC_DISABLE)	/* Area7  */
#define DFUSA8_EN		(SINGLE_ECC_DISABLE)	/* Area8  */
#define DFUSA9_EN		(SINGLE_ECC_DISABLE)	/* Area9  */
#define DFUSA10_EN		(SINGLE_ECC_DISABLE)	/* Area10 */
#define DFUSA11_EN		(SINGLE_ECC_DISABLE)	/* Area11 */
#define DFUSA12_EN		(SINGLE_ECC_DISABLE)	/* Area12 */
#define DFUSA13_EN		(SINGLE_ECC_DISABLE)	/* Area13 */
#define DFUSA14_EN		(SINGLE_ECC_DISABLE)	/* Area14 */
#define DFUSA15_EN		(SINGLE_ECC_DISABLE)	/* Area15 */

/* DRAM Fusa Area start address */
/* Write to bit [23:0] of DFUSAAREACRn */
#define DFUSA0_SADDR		(0x0400000000U)		/* Area0  */
#define DFUSA1_SADDR		(0x0U)			/* Area1  */
#define DFUSA2_SADDR		(0x0U)			/* Area2  */
#define DFUSA3_SADDR		(0x0U)			/* Area3  */
#define DFUSA4_SADDR		(0x0U)			/* Area4  */
#define DFUSA5_SADDR		(0x0U)			/* Area5  */
#define DFUSA6_SADDR		(0x0U)			/* Area6  */
#define DFUSA7_SADDR		(0x0U)			/* Area7  */
#define DFUSA8_SADDR		(0x0U)			/* Area8  */
#define DFUSA9_SADDR		(0x0U)			/* Area9  */
#define DFUSA10_SADDR		(0x0U)			/* Area10 */
#define DFUSA11_SADDR		(0x0U)			/* Area11 */
#define DFUSA12_SADDR		(0x0U)			/* Area12 */
#define DFUSA13_SADDR		(0x0U)			/* Area13 */
#define DFUSA14_SADDR		(0x0U)			/* Area14 */
#define DFUSA15_SADDR		(0x0U)			/* Area15 */

/* DRAM Fusa Area size */
/* Write to bit [27:24] of DFUSAAREACRn */
#define DFUSA0_SIZE		(DFUSA_SIZE_2MB)	/* Area0  */
#define DFUSA1_SIZE		(DFUSA_SIZE_1MB)	/* Area1  */
#define DFUSA2_SIZE		(DFUSA_SIZE_1MB)	/* Area2  */
#define DFUSA3_SIZE		(DFUSA_SIZE_1MB)	/* Area3  */
#define DFUSA4_SIZE		(DFUSA_SIZE_1MB)	/* Area4  */
#define DFUSA5_SIZE		(DFUSA_SIZE_1MB)	/* Area5  */
#define DFUSA6_SIZE		(DFUSA_SIZE_1MB)	/* Area6  */
#define DFUSA7_SIZE		(DFUSA_SIZE_1MB)	/* Area7  */
#define DFUSA8_SIZE		(DFUSA_SIZE_1MB)	/* Area8  */
#define DFUSA9_SIZE		(DFUSA_SIZE_1MB)	/* Area9  */
#define DFUSA10_SIZE		(DFUSA_SIZE_1MB)	/* Area10 */
#define DFUSA11_SIZE		(DFUSA_SIZE_1MB)	/* Area11 */
#define DFUSA12_SIZE		(DFUSA_SIZE_1MB)	/* Area12 */
#define DFUSA13_SIZE		(DFUSA_SIZE_1MB)	/* Area13 */
#define DFUSA14_SIZE		(DFUSA_SIZE_1MB)	/* Area14 */
#define DFUSA15_SIZE		(DFUSA_SIZE_1MB)	/* Area15 */

/* DRAM ECC Mode ( 8bit Data/5bit ECC or 64bit Data/8bit ECC) */
/* Write to bit31 of DFUSAAREACRn */
#define DECC0_MODE		(MODE_8_5)		/* Area0  */
#define DECC1_MODE		(MODE_8_5)		/* Area1  */
#define DECC2_MODE		(MODE_8_5)		/* Area2  */
#define DECC3_MODE		(MODE_8_5)		/* Area3  */
#define DECC4_MODE		(MODE_8_5)		/* Area4  */
#define DECC5_MODE		(MODE_8_5)		/* Area5  */
#define DECC6_MODE		(MODE_8_5)		/* Area6  */
#define DECC7_MODE		(MODE_8_5)		/* Area7  */
#define DECC8_MODE		(MODE_8_5)		/* Area8  */
#define DECC9_MODE		(MODE_8_5)		/* Area9  */
#define DECC10_MODE		(MODE_8_5)		/* Area10 */
#define DECC11_MODE		(MODE_8_5)		/* Area11 */
#define DECC12_MODE		(MODE_8_5)		/* Area12 */
#define DECC13_MODE		(MODE_8_5)		/* Area13 */
#define DECC14_MODE		(MODE_8_5)		/* Area14 */
#define DECC15_MODE		(MODE_8_5)		/* Area15 */

/* DRAM ECC Area start address */
/* Write to bit [23:0] of DECCAREACRn */
#define DECC0_SADDR		(0x043FE00000U)		/* Area0  */
#define DECC1_SADDR		(0x0U)			/* Area1  */
#define DECC2_SADDR		(0x0U)			/* Area2  */
#define DECC3_SADDR		(0x0U)			/* Area3  */
#define DECC4_SADDR		(0x0U)			/* Area4  */
#define DECC5_SADDR		(0x0U)			/* Area5  */
#define DECC6_SADDR		(0x0U)			/* Area6  */
#define DECC7_SADDR		(0x0U)			/* Area7  */
#define DECC8_SADDR		(0x0U)			/* Area8  */
#define DECC9_SADDR		(0x0U)			/* Area9  */
#define DECC10_SADDR		(0x0U)			/* Area10 */
#define DECC11_SADDR		(0x0U)			/* Area11 */
#define DECC12_SADDR		(0x0U)			/* Area12 */
#define DECC13_SADDR		(0x0U)			/* Area13 */
#define DECC14_SADDR		(0x0U)			/* Area14 */
#define DECC15_SADDR		(0x0U)			/* Area15 */

/* Enable or Disable filling Fusa Area with 0. */
#define DFUSA0_FILL		(FUSA_0FILL_ENABLE)	/* Area0  */
#define DFUSA1_FILL		(FUSA_0FILL_ENABLE)	/* Area1  */
#define DFUSA2_FILL		(FUSA_0FILL_ENABLE)	/* Area2  */
#define DFUSA3_FILL		(FUSA_0FILL_ENABLE)	/* Area3  */
#define DFUSA4_FILL		(FUSA_0FILL_ENABLE)	/* Area4  */
#define DFUSA5_FILL		(FUSA_0FILL_ENABLE)	/* Area5  */
#define DFUSA6_FILL		(FUSA_0FILL_ENABLE)	/* Area6  */
#define DFUSA7_FILL		(FUSA_0FILL_ENABLE)	/* Area7  */
#define DFUSA8_FILL		(FUSA_0FILL_ENABLE)	/* Area8  */
#define DFUSA9_FILL		(FUSA_0FILL_ENABLE)	/* Area9  */
#define DFUSA10_FILL		(FUSA_0FILL_ENABLE)	/* Area10 */
#define DFUSA11_FILL		(FUSA_0FILL_ENABLE)	/* Area11 */
#define DFUSA12_FILL		(FUSA_0FILL_ENABLE)	/* Area12 */
#define DFUSA13_FILL		(FUSA_0FILL_ENABLE)	/* Area13 */
#define DFUSA14_FILL		(FUSA_0FILL_ENABLE)	/* Area14 */
#define DFUSA15_FILL		(FUSA_0FILL_ENABLE)	/* Area15 */

/* Enable or Disable System RAM Single Channel ECC */
/* Write to bit31 of SFUSAAREACRn */
#define SFUSA0_EN		(SINGLE_ECC_DISABLE)	/* Area0  */
#define SFUSA1_EN		(SINGLE_ECC_DISABLE)	/* Area1  */
#define SFUSA2_EN		(SINGLE_ECC_DISABLE)	/* Area2  */
#define SFUSA3_EN		(SINGLE_ECC_DISABLE)	/* Area3  */
#define SFUSA4_EN		(SINGLE_ECC_DISABLE)	/* Area4  */
#define SFUSA5_EN		(SINGLE_ECC_DISABLE)	/* Area5  */
#define SFUSA6_EN		(SINGLE_ECC_DISABLE)	/* Area6  */
#define SFUSA7_EN		(SINGLE_ECC_DISABLE)	/* Area7  */

/* System RAM Fusa Area start address */
/* Write to bit [23:0] of SFUSAAREACRn */
#define SFUSA0_SADDR		(0x0U)			/* Area0  */
#define SFUSA1_SADDR		(0x0U)			/* Area1  */
#define SFUSA2_SADDR		(0x0U)			/* Area2  */
#define SFUSA3_SADDR		(0x0U)			/* Area3  */
#define SFUSA4_SADDR		(0x0U)			/* Area4  */
#define SFUSA5_SADDR		(0x0U)			/* Area5  */
#define SFUSA6_SADDR		(0x0U)			/* Area6  */
#define SFUSA7_SADDR		(0x0U)			/* Area7  */

/* System RAM Fusa Area size */
/* Write to bit [27:24] of SFUSAAREACRn */
#define SFUSA0_SIZE		(SFUSA_SIZE_4KB)	/* Area0  */
#define SFUSA1_SIZE		(SFUSA_SIZE_4KB)	/* Area1  */
#define SFUSA2_SIZE		(SFUSA_SIZE_4KB)	/* Area2  */
#define SFUSA3_SIZE		(SFUSA_SIZE_4KB)	/* Area3  */
#define SFUSA4_SIZE		(SFUSA_SIZE_4KB)	/* Area4  */
#define SFUSA5_SIZE		(SFUSA_SIZE_4KB)	/* Area5  */
#define SFUSA6_SIZE		(SFUSA_SIZE_4KB)	/* Area6  */
#define SFUSA7_SIZE		(SFUSA_SIZE_4KB)	/* Area7  */

/* System RAM ECC Mode ( 8bit Data/5bit ECC or 64bit Data/8bit ECC) */
/* Write to bit31 of SFUSAAREACRn */
#define SECC0_MODE		(MODE_8_5)		/* Area0  */
#define SECC1_MODE		(MODE_8_5)		/* Area1  */
#define SECC2_MODE		(MODE_8_5)		/* Area2  */
#define SECC3_MODE		(MODE_8_5)		/* Area3  */
#define SECC4_MODE		(MODE_8_5)		/* Area4  */
#define SECC5_MODE		(MODE_8_5)		/* Area5  */
#define SECC6_MODE		(MODE_8_5)		/* Area6  */
#define SECC7_MODE		(MODE_8_5)		/* Area7  */

/* System RAM ECC Area start address */
/* Write to bit [23:0] of SECCAREACRn */
#define SECC0_SADDR		(0x0U)			/* Area0  */
#define SECC1_SADDR		(0x0U)			/* Area1  */
#define SECC2_SADDR		(0x0U)			/* Area2  */
#define SECC3_SADDR		(0x0U)			/* Area3  */
#define SECC4_SADDR		(0x0U)			/* Area4  */
#define SECC5_SADDR		(0x0U)			/* Area5  */
#define SECC6_SADDR		(0x0U)			/* Area6  */
#define SECC7_SADDR		(0x0U)			/* Area7  */

/* Enable or Disable filling Fusa Area with 0. */
#define SFUSA0_FILL		(FUSA_0FILL_ENABLE)	/* Area0  */
#define SFUSA1_FILL		(FUSA_0FILL_ENABLE)	/* Area1  */
#define SFUSA2_FILL		(FUSA_0FILL_ENABLE)	/* Area2  */
#define SFUSA3_FILL		(FUSA_0FILL_ENABLE)	/* Area3  */
#define SFUSA4_FILL		(FUSA_0FILL_ENABLE)	/* Area4  */
#define SFUSA5_FILL		(FUSA_0FILL_ENABLE)	/* Area5  */
#define SFUSA6_FILL		(FUSA_0FILL_ENABLE)	/* Area6  */
#define SFUSA7_FILL		(FUSA_0FILL_ENABLE)	/* Area7  */

#endif /* SINGLE_ECC_CONF_H__ */

