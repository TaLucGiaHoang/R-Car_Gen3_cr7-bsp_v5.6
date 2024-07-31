/*
 * Copyright (c) 2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#ifndef SINGLE_ECC_H__
#define SINGLE_ECC_H__

#include "ecc_init.h"

#if RCAR_ECC_ENABLE == SINGLE_ECC_OPT_ENABLE
void single_ecc_init(void);
#endif

/**************************************************/
/* The following definitions can't be customized. */
/**************************************************/

/* The number of Fusa/ECC Area in DRAM/System RAM. */
#define DRAM_AREA_NUM		(16U)
#define SRAM_AREA_NUM		(8U)

/* Enable or Disable filling Fusa Area with 0. */
#define FUSA_0FILL_ENABLE	(1U)
#define FUSA_0FILL_DISABLE	(0U)

/* Write to bit31 of DFUSAAREACRn or SFUSAAREACRn */
#define SINGLE_ECC_DISABLE	((uint32_t)0U << 31U)
#define SINGLE_ECC_ENABLE	((uint32_t)1U << 31U)

/* Write to bit31 of DECCAREACRn or SECCAREACRn */
#define MODE_8_5		((uint32_t)0U << 31U)
#define MODE_64_8		((uint32_t)1U << 31U)

/* DRAM Fusa Area size */
/* Write to bit [27:24] of DFUSAAREACRn */
#define DFUSA_SIZE_1MB		(0U)
#define DFUSA_SIZE_2MB		(1U)
#define DFUSA_SIZE_4MB		(2U)
#define DFUSA_SIZE_8MB		(3U)
#define DFUSA_SIZE_16MB		(4U)
#define DFUSA_SIZE_32MB		(5U)
#define DFUSA_SIZE_64MB		(6U)
#define DFUSA_SIZE_128MB	(7U)
#define DFUSA_SIZE_256MB	(8U)
#define DFUSA_SIZE_512MB	(9U)

/* System RAM Fusa Area size */
/* Write to bit [27:24] of SFUSAAREACRn */
#define SFUSA_SIZE_4KB		(0U)
#define SFUSA_SIZE_8KB		(1U)
#define SFUSA_SIZE_16KB		(2U)
#define SFUSA_SIZE_32KB		(3U)
#define SFUSA_SIZE_64KB		(4U)
#define SFUSA_SIZE_128KB	(5U)
#define SFUSA_SIZE_256KB	(6U)

#endif /* SINGLE_ECC_H__ */

