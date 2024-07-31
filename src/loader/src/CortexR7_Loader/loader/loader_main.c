/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "stdint.h"
#include "string.h"
#include "loader_main.h"
#include "rcar_def.h"
#include "debug.h"
#include "mmio.h"
#include "cpg_init.h"
#include "cpg_register.h"
#include "pfc_init.h"
#include "scif.h"
#include "rcar_version.h"
#include "../ddr/boot_init_dram.h"
#include "../qos/qos_init.h"

#include "swdt.h"
#include "arm_gic.h"
#include "cpsr_acc.h"
#include "bl_common.h"
#include "rom_api.h"

/*****************************************************************************
 *	Definitions
 *****************************************************************************/
/* R-Car Seriese, 3rd Generation product check */
#if (RCAR_LSI == RCAR_H3) || (RCAR_LSI == RCAR_H3N)
#define TARGET_PRODUCT		RCAR_PRODUCT_H3
#define TARGET_NAME		"R-Car H3"
#elif RCAR_LSI == RCAR_M3
#define TARGET_PRODUCT		RCAR_PRODUCT_M3
#define TARGET_NAME		"R-Car M3"
#elif RCAR_LSI == RCAR_M3N
#define TARGET_PRODUCT		RCAR_PRODUCT_M3N
#define TARGET_NAME		"R-Car M3N"
#elif RCAR_LSI == RCAR_E3
#define TARGET_PRODUCT		RCAR_PRODUCT_E3
#define TARGET_NAME		"R-Car E3"
#elif RCAR_LSI == RCAR_AUTO
#define TARGET_NAME		"R-Car H3/M3/M3N"

#endif

/* CPG write protect registers */
#define	CPGWPR_PASSWORD		(0x5A5AFFFFU)
#define	CPGWPCR_PASSWORD	(0xA5A50000U)

/* MFIS Registers */
#define	MFISWPCNTR		(0xE6260900U)
#define	MFISWPCNTR_PASSWORD	(0xACCE0000U)

/* RST Registers */
#define	RST_BASE		(0xE6160000U)
#define	RST_WDTRSTCR		(RST_BASE + 0x0054U)
#define RST_CR7BAR		(RST_BASE + 0x0070U)
#define RST_CR7BAR_BAREN	((uint32_t)1U << 4)
#define	WDTRSTCR_PASSWORD	(0xA55A0000U)
#define	WDTRSTCR_RWDT_RSTMSK	((uint32_t)1U << 0U)

/* TCM Base address*/
#define TCM_BASE		(0xEB000000U)

/* SSGC for E3*/
#define RCAR_SSCG_MASK		(0x1000U)  /* MD12 */
#define RCAR_SSCG_ENABLE	(0x1000U)

#define WPD_DISABLE		(0x1U)

/*****************************************************************************
 *	Unions
 *****************************************************************************/


/*****************************************************************************
 *	Structs
 *****************************************************************************/


/*****************************************************************************
 *	Globals
 *****************************************************************************/


/*****************************************************************************
 *	Macros
 *****************************************************************************/


/*****************************************************************************
 *	Prototypes
 *****************************************************************************/


uint32_t loader_main(void)
{
	static const unsigned int irq_sec_array[] = {
		ARM_IRQ_WDT                /* 173          */
	};

	uint32_t reg;
	uint32_t lcs;
	uint32_t modemr;
	uint32_t modemr_boot_dev;
	uint32_t modemr_sscg;
	int32_t ret;
	uint32_t prr_val;
	char msg[128];
	const char *str;
	const char *product_h3      = "H3";
	const char *product_m3      = "M3";
	const char *product_m3n     = "M3N";
	const char *product_e3      = "E3";
	const char *lcs_cm          = "CM";
	const char *lcs_dm          = "DM";
	const char *lcs_sd          = "SD";
	const char *lcs_se          = "SE";
	const char *lcs_fa          = "FA";
#if RCAR_LSI == RCAR_E3
	const char *boot_hyper160   = "HyperFlash(150MHz)";
#else /* RCAR_LSI == RCAR_E3 */
	const char *boot_hyper160   = "HyperFlash(160MHz)";
#endif /* RCAR_LSI == RCAR_E3 */
	const char *boot_hyper80    = "HyperFlash(80MHz)";
	const char *boot_qspi40     = "QSPI Flash(40MHz)";
	const char *boot_qspi80     = "QSPI Flash(80MHz)";
	const char *unknown         = "unknown";
	const char *sscg_off        = "PLL1 nonSSCG Clock select";
	const char *sscg_on         = "PLL1 SSCG Clock select";

	modemr = mmio_read_32(RCAR_MODEMR);
	modemr_boot_dev = modemr & MODEMR_BOOT_DEV_MASK;

/*****************************************************************************
 *	Initialize Hardware
 *****************************************************************************/
	cpg_init();			/* CPG initialize */
	pfc_init();			/* Pin function initialize */
	(void)console_init();		/* SCIF initialize */
	aif_enable_a();			/* Enable Exception */

	/* GIC initialize		*/
	arm_gic_init(GICC_BASE, GICD_BASE, irq_sec_array
				, ARRAY_SIZE(irq_sec_array));
	/* GIC setup			*/
	arm_gic_setup();

	/* System WDT initialize	*/
	swdt_init();

	/* Enable IRQ interrupt		*/
	aif_enable_i();

	/* boot message */
	(void)sprintf(msg, "%s Initial Program Loader(CR7)\n",
			 TARGET_NAME);
	NOTICE("%s", msg);
	(void)sprintf(msg, "Initial Program Loader (Rev.%s)\n",
			 version_of_renesas);
	NOTICE("%s", msg);
	NOTICE("%s\n", build_message);

	/* R-Car Series, 3rd Generation product display & check */
	reg = mmio_read_32(RCAR_PRR);
	prr_val = reg;
	switch (reg & RCAR_PRODUCT_MASK) {
	case RCAR_PRODUCT_H3:
		str = product_h3;
		break;
	case RCAR_PRODUCT_M3:
		str = product_m3;
		break;
	case RCAR_PRODUCT_M3N:
		str = product_m3n;
		break;
	case RCAR_PRODUCT_E3:
		str = product_e3;
		break;
	default:
		str = unknown;
		break;
	}

	if ((RCAR_PRODUCT_M3 == (reg & RCAR_PRODUCT_MASK)) &&
		(RCAR_CUT_VER20 == (reg & RCAR_MAJOR_MASK))) {
		if (RCAR_M3_CUT_VER11 == (reg & RCAR_CUT_MASK)) {
			/* M3 Ver.1.1 or Ver.1.2 */
			(void)sprintf(msg,
				"PRR is R-Car %s Ver.1.1 / Ver.1.2\n",
				str);
		} else {
			/* M3 Ver.1.3 */
			(void)sprintf(msg, "PRR is R-Car %s Ver.1.%d\n",
			str,
			(prr_val & RCAR_MINOR_MASK) + RCAR_M3_MINOR_OFFSET);
		}
	} else {
		(void)sprintf(msg, "PRR is R-Car %s Ver.%d.%d\n", str,
			((prr_val & RCAR_MAJOR_MASK) >> RCAR_MAJOR_SHIFT)
			 + RCAR_MAJOR_OFFSET, (prr_val & RCAR_MINOR_MASK));
	}
	NOTICE("%s", msg);

#if RCAR_LSI != RCAR_AUTO
	if ((reg & RCAR_PRODUCT_MASK) != TARGET_PRODUCT) {
		ERROR("This IPL has been built for the %s.\n", TARGET_NAME);
		ERROR("Please write the correct IPL to flash memory.\n");
		panic();
	}
#endif /* RCAR_LSI != RCAR_AUTO */

	/* PLL1 clock select display (E3 only) */
	reg = mmio_read_32(RCAR_PRR);
	if ((reg & RCAR_PRODUCT_MASK) == RCAR_PRODUCT_E3) {
		modemr_sscg = mmio_read_32(RCAR_MODEMR);
		if ((modemr_sscg & RCAR_SSCG_MASK) == RCAR_SSCG_ENABLE) {
			str = sscg_on;
		} else {
			str = sscg_off;
		}
		(void)sprintf(msg, "%s\n", str);
		NOTICE("%s", msg);
	}

	switch (modemr_boot_dev) {
	case MODEMR_BOOT_DEV_HYPERFLASH160:
		str = boot_hyper160;
		break;
	case MODEMR_BOOT_DEV_HYPERFLASH80:
		str = boot_hyper80;
		break;
	case MODEMR_BOOT_DEV_QSPI_FLASH40:
		str = boot_qspi40;
		break;
	case MODEMR_BOOT_DEV_QSPI_FLASH80:
		str = boot_qspi80;
		break;
	default:
		str = unknown;
		break;
	}

	(void)sprintf(msg, "Boot device is %s\n", str);
	NOTICE("%s", msg);

	reg = ROM_GetLcs(&lcs);
	if (reg == 0U) {
		switch (lcs) {
		case LCS_CM:
			str = lcs_cm;
			break;
		case LCS_DM:
			str = lcs_dm;
			break;
		case LCS_SD:
			str = lcs_sd;
			break;
		case LCS_SE:
			str = lcs_se;
			break;
		case LCS_FA:
			str = lcs_fa;
			break;
		default:
			str = unknown;
			break;
		}
	} else {
		str = unknown;
	}
	NOTICE("LCM state is %s\n", str);

	/* DDR 4GB/8GB memory config log */
	reg = mmio_read_32(RCAR_PRR);
	/* H3 Ver.3.0 or later*/
	if (((reg & RCAR_PRODUCT_MASK) == RCAR_PRODUCT_H3) &&
		((reg & RCAR_CUT_MASK) >= RCAR_CUT_VER30)) {
#if (RCAR_DRAM_LPDDR4_MEMCONF == 0)
		/* 4GB(1GBx4) */
		NOTICE("CH0: 0x400000000 - 0x43fffffff, 1 GiB\n");
		NOTICE("CH1: 0x500000000 - 0x53fffffff, 1 GiB\n");
		NOTICE("CH2: 0x600000000 - 0x63fffffff, 1 GiB\n");
		NOTICE("CH3: 0x700000000 - 0x73fffffff, 1 GiB\n");
#elif (RCAR_DRAM_LPDDR4_MEMCONF == 1) && (RCAR_DRAM_CHANNEL == 5) && \
	(RCAR_DRAM_SPLIT == 2)
		/* 4GB(2GBx2 2ch split) */
		NOTICE("CH0: 0x400000000 - 0x47fffffff, 2 GiB\n");
		NOTICE("CH1: 0x500000000 - 0x57fffffff, 2 GiB\n");
#elif (RCAR_DRAM_LPDDR4_MEMCONF == 1) && (RCAR_DRAM_CHANNEL == 15)
		/* 8GB(2GBx4: default) */
		NOTICE("CH0: 0x400000000 - 0x47fffffff, 2 GiB\n");
		NOTICE("CH1: 0x500000000 - 0x57fffffff, 2 GiB\n");
		NOTICE("CH2: 0x600000000 - 0x67fffffff, 2 GiB\n");
		NOTICE("CH3: 0x700000000 - 0x77fffffff, 2 GiB\n");
#endif /* RCAR_DRAM_LPDDR4_MEMCONF == 0 */
	}
	/* For R-Car E3 */
	if ((reg & RCAR_PRODUCT_MASK) == RCAR_PRODUCT_E3) {
#if (RCAR_DRAM_DDR3L_MEMCONF == 0)
		/* 1GB(512MBx2) */
		NOTICE("0x400000000 - 0x43fffffff, 1 GiB\n");
#elif (RCAR_DRAM_DDR3L_MEMCONF == 1)
		/* 2GB(512MBx4) */
		NOTICE("0x400000000 - 0x47fffffff, 2 GiB\n");
#endif /* RCAR_DRAM_DDR3L_MEMCONF == 0 */
	}

	ret = InitDram();		/* DDR initialize */
	if (ret != INITDRAM_OK) {
		ERROR("Failed to DRAM initialize (%d)\n", ret);
		panic();
	}

#if RCAR_DDR_REG_CHECK == 1
	/* initialized reg check */
	reg = mmio_read_32(RCAR_PRR);
	switch (reg & RCAR_PRODUCT_MASK) {
#if (RCAR_LSI == RCAR_H3) || (RCAR_LSI == RCAR_H3N) \
	|| (RCAR_LSI == RCAR_M3) || (RCAR_LSI == RCAR_M3N) \
	|| (RCAR_LSI == RCAR_AUTO)
	case RCAR_PRODUCT_H3:
		/* No break */
	case RCAR_PRODUCT_M3:
		/* No break */
	case RCAR_PRODUCT_M3N:
		ret = InitDram_regcheck();
		if (ret != 0) {
			ERROR("Failed to DDRPHY REG check (%d)\n", ret);
			/*
			 * User need to implement what IPL should do
			 * if InitDram_regcheck() is failed.
			 */
		} else {
			NOTICE("DDRPHY REG check is OK (%d)\n", ret);
		}
		break;
#endif
#if RCAR_LSI == RCAR_E3
	case RCAR_PRODUCT_E3:
		break;
#endif
	default:
		break;
	}
#endif /* RCAR_DDR_REG_CHECK == 1 */

	qos_init();			/* QoS configuration */

/*****************************************************************************
 *	Unmask the detection of RWDT overflow
 *****************************************************************************/
	reg = mmio_read_32(RST_WDTRSTCR);
	reg &= ~WDTRSTCR_RWDT_RSTMSK;
	reg |= WDTRSTCR_PASSWORD;
	mmio_write_32(RST_WDTRSTCR, reg);

/*****************************************************************************
 *	Release CPG write protect
 *****************************************************************************/
	mmio_write_32(CPG_CPGWPR, CPGWPR_PASSWORD);
	mmio_write_32(CPG_CPGWPCR, CPGWPCR_PASSWORD);

/*****************************************************************************
 *	MFIS write protection setting
 *****************************************************************************/
	reg = mmio_read_32(RCAR_PRR);
	if (((RCAR_PRODUCT_M3 == (reg & RCAR_PRODUCT_MASK)) &&
		(RCAR_CUT_VER30 > (reg & RCAR_CUT_MASK))) ||
			((RCAR_PRODUCT_H3 == (reg & RCAR_PRODUCT_MASK)) &&
				(RCAR_CUT_VER20 > (reg & RCAR_CUT_MASK)))) {
		/* No need to disable MFIS write protection */
		;
	} else {
		/* Disable MFIS write protection */
		mmio_write_32(MFISWPCNTR, MFISWPCNTR_PASSWORD | WPD_DISABLE);
	}

	return 0U;
}
