/*
 * Copyright (c) 2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#ifndef SINGLE_ECC_REG_H__
#define SINGLE_ECC_REG_H__

#include "mfis_register.h"
#include "axi_register.h"

/* DRAM FuSa Area Configuration Register n */
#define AXI_DFUSAAREACR		(AXI_BASE + (0x5000U))
#define DFUSAAREACR(n)		(AXI_DFUSAAREACR + ((n) * 0x4U)) /* n=0--15 */

/* DRAM ECC Area Configuration Register n */
#define AXI_DECCAREACR		(AXI_BASE + (0x5040U))
#define DECCAREACR(n)		(AXI_DECCAREACR + ((n) * 0x4U)) /* n=0--15 */

/* System RAM ECC Area Configuration Register n */
#define AXI_SFUSAAREACR		(AXI_BASE + (0x5100U))
#define SFUSAAREACR(n)		(AXI_SFUSAAREACR + ((n) * 0x4U)) /* n=0--7 */

/* System RAM ECC Area Configuration Register n */
#define AXI_SECCAREACR		(AXI_BASE + (0x5140U))
#define SECCAREACR(n)		(AXI_SECCAREACR + ((n) * 0x4U)) /* n=0--7 */


#endif	/* SINGLE_ECC_REG_H__ */
