/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <mmio.h>
#include <debug.h>

#include "../ddr_b/boot_init_dram.c"
#include "init_dram_tbl_chk.h"

int32_t InitDram_regcheck(void)
{
	int32_t		err;

	uint32_t	i;
	uint32_t	phytype, dataL, ch, slice;

	err = 0;

	if (Prr_Product == PRR_PRODUCT_H3) {
		if (Prr_Cut <= PRR_PRODUCT_11) {
			phytype = 0;
		} else {
			phytype = 2;
		}
	} else if (Prr_Product == PRR_PRODUCT_M3) {
		phytype = 1;
	} else {
		phytype = 3;
	}

	if (((Prr_Product == PRR_PRODUCT_H3) && (Prr_Cut <= PRR_PRODUCT_11))
	 || ((Prr_Product == PRR_PRODUCT_M3) && (Prr_Cut < PRR_PRODUCT_30))) {
		/* non : H3 Ver.1.x/M3-W Ver.1.x not support */
	} else {
		mmio_write_32(DBSC_DBSYSCNT0, 0x00001234);
	}

	/* Unlock DDRPHY register */
	foreach_vch(ch)
		mmio_write_32(DBSC_DBPDLK(ch), 0x0000A55A);
	dsb_sev();

	for (i = 0; i < DDR_PHY_SLICE_REGSET_NUM; i++) {
		if (DDR_PHY_SLICE_REGCHK[phytype][i/32] & (1U << (i%32))) {
			foreach_vch(ch) {
				for (slice = 0; slice < 4; slice++) {
					dataL = reg_ddrphy_read(ch, i + DDR_PHY_SLICE_REGSET_OFS + DDR_PHY_SLICE_REGSET_SIZE * slice);
					if (dataL != _cnf_DDR_PHY_SLICE_REGSET[i]) {
						ERROR("PHY REG ch%d(data slice)[0x%04x] = 0x%08x ->  0x%08x\n",
							ch, i+DDR_PHY_SLICE_REGSET_OFS  + DDR_PHY_SLICE_REGSET_SIZE * slice,
							_cnf_DDR_PHY_SLICE_REGSET[i], dataL);
						err = 1;
					}
				}
			}
		}
	}

	for (i = 0; i < DDR_PHY_ADR_V_REGSET_NUM; i++) {
		if (DDR_PHY_ADR_V_REGCHK[phytype][i/32] & (1U << (i%32))) {
			foreach_vch(ch) {
				dataL = reg_ddrphy_read(ch, i + DDR_PHY_ADR_V_REGSET_OFS);
				if (dataL != _cnf_DDR_PHY_ADR_V_REGSET[i]) {
					ERROR("PHY REG ch%d(addr slice)[0x%04x] = 0x%08x ->  0x%08x\n",
						ch, i+DDR_PHY_ADR_V_REGSET_OFS,
						_cnf_DDR_PHY_ADR_V_REGSET[i], dataL);
					err = 1;
				}
			}
		}
	}

//	for (i = 0; i < DDR_PHY_ADR_I_REGSET_NUM; i++) {
//		if (DDR_PHY_ADR_I_REGCHK[phytype][i/32] & (1U << (i%32))) {
//			foreach_vch(ch) {
//				dataL = reg_ddrphy_read(ch, i + DDR_PHY_ADR_I_REGSET_OFS);
//				if (dataL != _cnf_DDR_PHY_ADR_I_REGSET[i]) {
//					ERROR("PHY REG ch%d(addr slice)[0x%04x] = 0x%08x ->  0x%08x\n",
//						ch, i+DDR_PHY_ADR_I_REGSET_OFS,
//						_cnf_DDR_PHY_ADR_I_REGSET[i], dataL);
//					err = 1;
//				}
//			}
//		}
//	}

	for (i = 0; i < DDR_PHY_ADR_G_REGSET_NUM; i++) {
		if (DDR_PHY_ADR_G_REGCHK[phytype][i/32] & (1U << (i%32))) {
			foreach_vch(ch) {
				dataL = reg_ddrphy_read(ch, i + DDR_PHY_ADR_G_REGSET_OFS);
				if (dataL != _cnf_DDR_PHY_ADR_G_REGSET[i]) {
					ERROR("PHY REG ch%d(addr slice)[0x%04x] = 0x%08x ->  0x%08x\n",
						ch, i+DDR_PHY_ADR_G_REGSET_OFS,
						_cnf_DDR_PHY_ADR_G_REGSET[i], dataL);
					err = 1;
				}
			}
		}
	}

	for (i = 0; i < DDR_PI_REGSET_NUM; i++) {
		if (DDR_PI_REGCHK[phytype][i/32] & (1U << (i%32))) {
			foreach_vch(ch) {
				dataL = reg_ddrphy_read(ch, i + DDR_PI_REGSET_OFS);
				if (dataL != _cnf_DDR_PI_REGSET[i]) {
					ERROR("PI REG ch%d[0x%04x] = 0x%08x ->  0x%08x\n",
						ch, i+DDR_PI_REGSET_OFS,
						_cnf_DDR_PI_REGSET[i], dataL);
					err = 1;
				}
			}
		}
	}

	foreach_vch(ch)
		mmio_write_32(DBSC_DBPDLK(ch), 0x00000000);

	if (((Prr_Product == PRR_PRODUCT_H3) && (Prr_Cut <= PRR_PRODUCT_11))
	  || ((Prr_Product == PRR_PRODUCT_M3) && (Prr_Cut < PRR_PRODUCT_30))) {
		/* non : H3 Ver.1.x/M3-W Ver.1.x not support */
	} else {
		mmio_write_32(DBSC_DBSYSCNT0, 0x00000000);
	}

	return err;
}
