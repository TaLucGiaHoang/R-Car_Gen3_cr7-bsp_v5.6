/*
 * Copyright (c) 2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "debug.h"
#include "mmio.h"
#include "rcar_def.h"
#include "ecc_init.h"
#include "dram_dual_ecc.h"
#include "single_ecc.h"

void ecc_init(void)
{
#if RCAR_ECC_ENABLE == DRAM_SRAM_ECC_DISABLE
	/* Nothing to do */

#elif RCAR_ECC_ENABLE == SINGLE_ECC_OPT_ENABLE
	/* Single Channel ECC */

	uint32_t prr;

	/* get value of Product Register */
	prr = mmio_read_32(RCAR_PRR);
	switch (prr & RCAR_PRODUCT_MASK) {
	case RCAR_PRODUCT_H3:
		if (RCAR_CUT_VER20 > (prr & RCAR_CUT_MASK)) {
			/* H3 Ver.1.0/1.1 */
			ERROR("R-Car H3 Ver.1.0/1.1 is not supported "
			      "Single Channel ECC.\n");
			panic();
		} else {
			/* H3 Ver.2.0/3.0 */
			single_ecc_init();
		}
		break;
	case RCAR_PRODUCT_M3:
		if (RCAR_CUT_VER30 > (prr & RCAR_CUT_MASK)) {
			/* M3 Ver.1.0/1.1/1.2/1.3 */
			ERROR("R-Car M3 Ver.1.0/1.1/1.2/1.3 is not supported "
			      "Single Channel ECC.\n");
			panic();
		} else {
			/* M3 Ver.3.0 */
			single_ecc_init();
		}
		break;
	case RCAR_PRODUCT_M3N:
		/* No break */
	case RCAR_PRODUCT_E3:
		single_ecc_init();
		break;
	default:
		ERROR("Product ID Unknown.\n");
		panic();
		break;
	}

#elif RCAR_ECC_ENABLE == DUAL_ECC_OPT_ENABLE
	/* Dual Channel ECC */

	uint32_t prr;

	/* get value of Product Register */
	prr = mmio_read_32(RCAR_PRR);
	switch (prr & RCAR_PRODUCT_MASK) {
	case RCAR_PRODUCT_H3:
		/* No break */
	case RCAR_PRODUCT_M3:
		dram_dual_ecc_init();
		break;
	case RCAR_PRODUCT_M3N:
		/* If RCAR_LSI=AUTO is specified */
		ERROR("R-Car M3N is not supported Dual Channel ECC.\n");
		panic();
		break;
	default:
		ERROR("Product ID Unknown.\n");
		panic();
		break;
	}

#endif	/* RCAR_ECC_ENABLE */
}
