/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "stdint.h"
#include "string.h"
#include "mmio.h"
#include "cpg_register.h"
#include "cpg_init.h"
#include "rpc_register.h"
#include "rpc_driver.h"
#include "debug.h"
#include "rcar_def.h"

#define CMNSR_TEND		(0x1U)
#define CMNCR_MD		(0x80000000U)
#define RPC_IF_BIT		((uint32_t)1U << 17U)
#define RPC_TEND		(0x1U)
#define RPC_PHYCNT_CAL		((uint32_t)1U << 31U)
#define RPC_PHYCNT_STRTIM_M3V1	((uint32_t)6U << 15U)
#define RPC_PHYCNT_STRTIM	((uint32_t)7U << 15U)


static void enableRPC(void);
static void setupRPC(void);

static void enableRPC(void)
{
	mstpcr_write(CPG_RMSTPCR9, CPG_MSTPSR9, RPC_IF_BIT);
}

static void setupRPC(void)
{
	uint32_t product;
	uint32_t cut;
	uint32_t reg;
	uint32_t phy_strtim;

	/* manual mode */
	if ((mmio_read_32(RPC_CMNCR) & CMNCR_MD) != 0U) {
		/* external address space read mode */
		mmio_write_32(RPC_CMNCR,
			mmio_read_32(RPC_CMNCR) & (~CMNCR_MD));
	}

	product = mmio_read_32(RCAR_PRR) & RCAR_PRODUCT_MASK;
	cut = mmio_read_32(RCAR_PRR) & RCAR_CUT_MASK;

	/* PHY Calibration */
	if ((product ==  RCAR_PRODUCT_M3) && (cut < RCAR_CUT_VER30)) {
		/* M3 Ver.1.0/1.1/1.2/1.3 */
		phy_strtim = RPC_PHYCNT_STRTIM_M3V1;
	} else {
		/* H3/M3 Ver.3.0/M3N/E3 */
		phy_strtim = RPC_PHYCNT_STRTIM;
	}

	reg = mmio_read_32(RPC_PHYCNT);
	reg &= ~RPC_PHYCNT_STRTIM;
	reg |= phy_strtim;
	mmio_write_32(RPC_PHYCNT, reg);
	reg |= RPC_PHYCNT_CAL;
	mmio_write_32(RPC_PHYCNT, reg);
}

void wait_endRPC(void)
{
	while ((mmio_read_32(RPC_CMNSR) & CMNSR_TEND) != RPC_TEND) {
		;
	}
}

void initRPC(void)
{
	enableRPC();
	setupRPC();
}

