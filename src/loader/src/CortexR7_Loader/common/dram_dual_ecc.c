/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "debug.h"
#include "mmio.h"
#include "rcar_def.h"
#include "dma_driver.h"
#include "dram_dual_ecc.h"
#include "mfis_register.h"
#include "axi_register.h"

#if RCAR_ECC_ENABLE == DUAL_ECC_OPT_ENABLE

/* compile option definitions */
#define	RCAR_DRAM_SPLIT_LINEAR		(0U)
#define	RCAR_DRAM_SPLIT_4CH		(1U)
#define	RCAR_DRAM_SPLIT_2CH		(2U)
#define RCAR_DRAM_SPLIT_AUTO		(3U)
#define	DRAM_DUAL_CHANNEL_ECC		(2U)
#define SDRAM_4GB			(0U)

/* AXI-Bus registers definitions */
#define FUSACR_EFUSASEL(x)		(((x) & 0xFFU) << 24U)
#define FUSACR_DFUSASEL(x)		(((x) & 0xFFU) << 16U)

#define	ADSPLCR_ADRMODE_DEFAULT		((uint32_t)0U << 31U)
#define	ADSPLCR_ADRMODE_GEN2		((uint32_t)1U << 31U)
#define	ADSPLCR_SPLITSEL(x)		((uint32_t)(x) << 16U)
#define	ADSPLCR_AREA(x)			((uint32_t)(x) <<  8U)
#define	ADSPLCR_SWP			(0x0CU)

/* area size 2^27(128MB) */
#define ADSPLCR0_AREA_1GB		(0x1BU)
/* area size 2^24(16MB) */
#define ADSPLCR3_AREA_1GB		(0x18U)
/* area size 2^28(256MB) */
#define ADSPLCR0_AREA_2GB		(0x1CU)
/* area size 2^25(32MB) */
#define ADSPLCR3_AREA_2GB		(0x19U)
/* area size 2^29(512MB) */
#define ADSPLCR0_AREA_4GB		(0x1DU)
/* area size 2^26(64MB) */
#define ADSPLCR3_AREA_4GB		(0x1AU)

/* Use Number */
#define ON_BIT				(1U)
#define OFF_BIT				(0U)
#define AREA_NUM			(8U)
#define REVERSE_SET(x)			(7U - (uint32_t)(x))
#define BIT_SHIFT(x)			((uint32_t)1U << (uint32_t)(x))
#define ECC_AREA_ENDADD(x)		((uint32_t)(x) - 1U)
#define EXTRA_AREA_NO_SPLIT		(0U)
#define EXTRA_AREA_CH_MASK		(0xFFU)

/* Error Detection Register */
#define MFI_BITSET			((uint32_t)1U << 24U)

#define EDCEN_MEM			((uint32_t)1U << 15U)
#define EDCINTEN1_DRAM			(0xE0000000U)

/* ECC register */
#define ECC_BASE1			(0x0400000000U)
#define ECC_BASE2			(0x0500000000U)

/* Strings of 0fill log output */
#define DO_FILL				"Fill with 0"
#define NO_FILL				"No fill"

#if RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_AUTO
static void dual_ecc_setting_4ch(void);
static void dual_ecc_setting_2ch(void);

#elif RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_4CH
static void dual_ecc_setting_4ch(void);

#elif RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_2CH
static void dual_ecc_setting_2ch(void);

#else
static void dual_ecc_setting_linear(void);

#endif /* RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_AUTO */

/*
 * Area ECC config
 * 1 = enable	0 = disable
 * When area_ecc_enable_bit[n] and extra_area_ecc_enable_bit[n]
 * 1 is set together. area_ecc_enable_bit[n] is 0 set
 */
static const uint32_t area_ecc_enable_bit[AREA_NUM] =   {0U, /* area7 */
							 0U, /* area6 */
							 0U, /* area5 */
							 0U, /* area4 */
							 0U, /* area3 */
							 0U, /* area2 */
							 0U, /* area1 */
							 0U}; /* area0 */

/*
 * Area 0fill config
 * 1 = enable	0 = disable
 * When area_fill_enable_bit[n]=1, fill Fusa Area with 0
 */
static const uint32_t area_fill_enable_bit[AREA_NUM] = {1U, /* area7 */
							1U, /* area6 */
							1U, /* area5 */
							1U, /* area4 */
							1U, /* area3 */
							1U, /* area2 */
							1U, /* area1 */
							1U}; /* area0 */

/*
 * Extra Split config
 * 1 = enable	0 = disable
 * Only one extra split area is effective
 * When a plural set, minimum number area is effective
 */
static const uint32_t extra_split_enable_bit[AREA_NUM] = {0U, /* area7 */
							  0U, /* area6 */
							  0U, /* area5 */
							  0U, /* area4 */
							  0U, /* area3 */
							  0U, /* area2 */
							  0U, /* area1 */
							  1U}; /* area0 */

/*
 * Extra Area ECC config
 * 1 = enable	0 = disable
 */
static const uint32_t extra_area_ecc_enable_bit[AREA_NUM] = {
							 0U, /* area7 */
							 0U, /* area6 */
							 0U, /* area5 */
							 0U, /* area4 */
							 0U, /* area3 */
							 0U, /* area2 */
							 0U, /* area1 */
							 1U}; /* area0 */

/*
 * Extra Area 0fill config
 * 1 = enable	0 = disable
 * When area_fill_enable_bit[n]=1, fill Fusa Area with 0
 */
static const uint32_t extra_area_fill_enable_bit[AREA_NUM] = {
							 1U, /* area7 */
							 1U, /* area6 */
							 1U, /* area5 */
							 1U, /* area4 */
							 1U, /* area3 */
							 1U, /* area2 */
							 1U, /* area1 */
							 1U}; /* area0 */

#if RCAR_DRAM_SPLIT != RCAR_DRAM_SPLIT_LINEAR
/*
 * Extra Area CH Split config
 * 1 = 4ch(or 2ch)	0 = linear
 */
static const uint32_t extra_area[AREA_NUM] =   {1U, /* area7 */
						1U, /* area6 */
						1U, /* area5 */
						1U, /* area4 */
						1U, /* area3 */
						1U, /* area2 */
						1U, /* area1 */
						1U}; /* area0 */

#endif /* RCAR_DRAM_SPLIT != RCAR_DRAM_SPLIT_LINEAR */

/* area ecc config param */
static uint32_t area_ecc_enable;
/* extra area ecc config param */
static uint32_t extra_area_ecc_enable;
/* ADSPLCR0 or ADSPLCR1 AREA */
static uint32_t area_size;
/* ADSPLCR3 AREA */
static uint32_t extra_area_size;

void dram_dual_ecc_init(void)
{
	/* area size to byte size */
	static uint32_t area_size_byte;
	/* extra area size to byte size */
	static uint32_t extra_area_size_byte;

	uint32_t fusa_check;
	uint32_t reg;
	uint32_t prr;
	uint32_t i;
	uint32_t j;

	char msg[128];
	const char *str;

	/*
	 * Error Detection Setting
	 * MFIS error check register
	 */
	reg = mmio_read_32(MFIS_MFIERRCTLR4);
	reg = (reg | MFI_BITSET);
	mmio_write_32(MFIS_MFIERRCTLR4, reg);

	reg = mmio_read_32(MFIS_MFIERRTGTR4);
	reg = (reg | MFI_BITSET);
	mmio_write_32(MFIS_MFIERRTGTR4, reg);

	/* Error Detection Circuit */
	reg = mmio_read_32(EDC_EDCEN0);
	reg = (reg | EDCEN_MEM);
	mmio_write_32(EDC_EDCEN0, reg);

	reg = mmio_read_32(EDC_EDCINTEN1);
	reg = (reg | EDCINTEN1_DRAM);
	mmio_write_32(EDC_EDCINTEN1, reg);

	/* PRR get */
	prr = mmio_read_32(RCAR_PRR);
	switch (prr & RCAR_PRODUCT_MASK) {
	case RCAR_PRODUCT_H3:
		/* H3 size get */
		if (RCAR_CUT_VER30 > (prr & RCAR_CUT_MASK)) {
			/* less than H3 Ver.3.0 */
			area_size = ADSPLCR0_AREA_1GB;
			extra_area_size = ADSPLCR3_AREA_1GB;
		} else {
#if RCAR_DRAM_LPDDR4_MEMCONF == SDRAM_4GB
			/* H3 Ver.3.0 SDRAM:1GBx4ch */
			area_size = ADSPLCR0_AREA_1GB;
			extra_area_size = ADSPLCR3_AREA_1GB;
#else
			/* H3 Ver.3.0 SDRAM:2GBx4ch */
			area_size = ADSPLCR0_AREA_2GB;
			extra_area_size = ADSPLCR3_AREA_2GB;
#endif
		}
		break;
	case RCAR_PRODUCT_M3:
		/* M3 size get */
		if (RCAR_CUT_VER30 > (prr & RCAR_CUT_MASK)) {
			/* less than M3 Ver.3.0 */
			area_size = ADSPLCR0_AREA_2GB;
			extra_area_size = ADSPLCR3_AREA_2GB;
		} else {
			/* M3 Ver.3.0 */
			area_size = ADSPLCR0_AREA_4GB;
			extra_area_size = ADSPLCR3_AREA_4GB;
		}
		break;
	default:
		ERROR("Product ID Unknown.\n");
		panic();
		break;
	}

	/* AXI bus register setting for split and ECC */
	prr = mmio_read_32(RCAR_PRR);
#if RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_AUTO
	if (RCAR_PRODUCT_H3 == (prr & RCAR_PRODUCT_MASK)) {
		dual_ecc_setting_4ch();
	} else {
		dual_ecc_setting_2ch();
	}
#elif RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_4CH
	dual_ecc_setting_4ch();
#elif RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_2CH
	dual_ecc_setting_2ch();
#else
	dual_ecc_setting_linear();
#endif

	/* fusa check */
	fusa_check = area_ecc_enable & extra_area_ecc_enable;
	if (fusa_check != OFF_BIT) {
		ERROR("DRAM ECC area setting error\n");
		panic();
	}

	reg = mmio_read_32(AXI_FUSACR);
	reg = reg | FUSACR_EFUSASEL(extra_area_ecc_enable)
		  | FUSACR_DFUSASEL(area_ecc_enable);
	mmio_write_32(AXI_FUSACR, reg);

	/* log output */
	NOTICE("DRAM Dual ECC enable\n");

	area_size_byte = BIT_SHIFT(area_size);
	extra_area_size_byte = BIT_SHIFT(extra_area_size);
	prr = mmio_read_32(RCAR_PRR) & RCAR_PRODUCT_MASK;

	/* ECC area fill and area terminal output */
	for (i = 0U; i < AREA_NUM; i++) {
		if ((area_ecc_enable_bit[AREA_NUM - 1U - i] == ON_BIT)
			&& (extra_split_enable_bit[AREA_NUM - 1U - i]
				 == OFF_BIT)){

			str = NO_FILL;
			if (area_fill_enable_bit[AREA_NUM - 1U - i] == ON_BIT) {
				exec_fillDMA(ECC_BASE1
					+ ((uint64_t)area_size_byte
						* (uint64_t)i),
					area_size_byte);
				str = DO_FILL;
				if (prr == RCAR_PRODUCT_H3) {
					exec_fillDMA(ECC_BASE2
						+ ((uint64_t)area_size_byte
							* (uint64_t)i),
						area_size_byte);
				}
			}
			(void)sprintf(msg, "Area%d:0x04_%x - 0x04_%x %s\n",
					i,
					(area_size_byte * i),
					((area_size_byte * i)
					 + ECC_AREA_ENDADD(area_size_byte)),
					str);
			NOTICE("%s", msg);
			if (prr == RCAR_PRODUCT_H3) {
				(void)sprintf(msg,
					"      0x05_%x - 0x05_%x %s\n",
					(area_size_byte * i),
					((area_size_byte * i)
					 + ECC_AREA_ENDADD(area_size_byte)),
					str);
				NOTICE("%s", msg);
			}
		}

		if (extra_split_enable_bit[AREA_NUM - 1U - i] == ON_BIT) {
			NOTICE("Extra split enable Area%d\n", i);
			for (j = 0U; j < AREA_NUM; j++) {
				if (extra_area_ecc_enable_bit[AREA_NUM - 1U - j]
					 == ON_BIT) {

					str = NO_FILL;
					if (extra_area_fill_enable_bit[AREA_NUM - 1U - j]
						 == ON_BIT){
						exec_fillDMA(ECC_BASE1
						    + ((uint64_t)area_size_byte
						       * (uint64_t)i)
						    + ((uint64_t)extra_area_size_byte
						       * (uint64_t)j),
							extra_area_size_byte);
						str = DO_FILL;
						if (prr == RCAR_PRODUCT_H3) {
							exec_fillDMA(ECC_BASE2
						    + ((uint64_t)area_size_byte
							* (uint64_t)i)
						    + ((uint64_t)extra_area_size_byte
							* (uint64_t)j),
							extra_area_size_byte);
						}
					}
					(void)sprintf(msg,
						"Extra Area %d-%d:0x04_%x - "
						"0x04_%x %s\n",
						i,
						j,
						((area_size_byte * i)
						 + (extra_area_size_byte * j)),
						((area_size_byte * i)
						 + ((extra_area_size_byte * j)
						 + ECC_AREA_ENDADD(extra_area_size_byte))),
						str);
					NOTICE("%s", msg);
					if (prr == RCAR_PRODUCT_H3) {
						(void)sprintf(msg,
							"               0x05_%x"
							" - 0x05_%x %s\n",
							((area_size_byte * i)
							+ (extra_area_size_byte
							   * j)),
							((area_size_byte * i)
							+ ((extra_area_size_byte
							   * j)
							 + ECC_AREA_ENDADD(extra_area_size_byte))),
							str);
						 NOTICE("%s", msg);
					}
				}
			}
		}
	}
}

#if (RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_AUTO) \
	|| (RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_4CH)
static void dual_ecc_setting_4ch(void)
{
	/* extra split config param */
	uint32_t extra_split_enable = 0U;
	/* extra area ch split config param */
	uint32_t extra_area_ch = 0U;

	uint32_t reg;
	uint32_t prr;
	uint32_t i;

	/* register param get */
	for (i = 0U; i < AREA_NUM; i++) {
		extra_split_enable	|= (extra_split_enable_bit[i]
					 << REVERSE_SET(i));
		area_ecc_enable		|= (area_ecc_enable_bit[i]
					 << REVERSE_SET(i));
		extra_area_ch		|= (extra_area[i]
					 << REVERSE_SET(i));
		extra_area_ecc_enable	|= (extra_area_ecc_enable_bit[i]
					 << REVERSE_SET(i));
	}

	/* H3 Ver.2.0 extra split ch set Linear */
	prr = mmio_read_32(RCAR_PRR);
	if (RCAR_PRODUCT_H3_CUT20
			 == (prr & (RCAR_PRODUCT_MASK | RCAR_CUT_MASK))) {
		extra_area_ch = OFF_BIT;
	}

	/* extra split check */
	for (i = 0U; i < AREA_NUM; i++) {
		if ((extra_split_enable & BIT_SHIFT(i)) != OFF_BIT) {
			/* extra split enable area must be one */
			extra_split_enable = extra_split_enable & BIT_SHIFT(i);
			break;
		}
	}

	/* register param set */
	reg = mmio_read_32(AXI_ADSPLCR1);
	reg = reg | ADSPLCR_ADRMODE_DEFAULT
		  | ADSPLCR_SPLITSEL(extra_split_enable)
		  | ADSPLCR_AREA(area_size)
		  | ADSPLCR_SWP;
	mmio_write_32(AXI_ADSPLCR1, reg);

	reg = mmio_read_32(AXI_ADSPLCR3);
	reg = reg | ADSPLCR_ADRMODE_DEFAULT
		  | ADSPLCR_SPLITSEL(extra_area_ch)
		  | ADSPLCR_AREA(extra_area_size)
		  | ADSPLCR_SWP;
	mmio_write_32(AXI_ADSPLCR3, reg);
}
#endif /*
	* (RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_AUTO) \
	* || (RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_4CH)
	*/

#if (RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_AUTO) \
	|| (RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_2CH)
static void dual_ecc_setting_2ch(void)
{
	/* extra split config param */
	uint32_t extra_split_enable = 0U;
	/* extra area ch split config param */
	uint32_t extra_area_ch = 0U;

	uint32_t reg;
	uint32_t prr;
	uint32_t i;

	/* register param get */
	for (i = 0U; i < AREA_NUM; i++) {
		extra_split_enable	|= (extra_split_enable_bit[i]
					 << REVERSE_SET(i));
		area_ecc_enable		|= (area_ecc_enable_bit[i]
					 << REVERSE_SET(i));
		extra_area_ch		|= (extra_area[i]
					 << REVERSE_SET(i));
		extra_area_ecc_enable	|= (extra_area_ecc_enable_bit[i]
					 << REVERSE_SET(i));
	}

	/*
	 * If ECC is enable at extra_area,
	 * the extra_area can't be set DRAM 2ch split setting
	 */
	extra_area_ch = extra_area_ch & (~extra_area_ecc_enable);
	extra_area_ch &= EXTRA_AREA_CH_MASK;

	/* H3 Ver.2.0 extra split ch set Linear */
	prr = mmio_read_32(RCAR_PRR);
	if (RCAR_PRODUCT_H3_CUT20
			 == (prr & (RCAR_PRODUCT_MASK | RCAR_CUT_MASK))) {
		extra_area_ch = OFF_BIT;
	}

	/* extra split check */
	for (i = 0U; i < AREA_NUM; i++) {
		if ((extra_split_enable & BIT_SHIFT(i)) != OFF_BIT) {
			/* extra split enable area must be one */
			extra_split_enable = extra_split_enable & BIT_SHIFT(i);
			break;
		}
	}

	/* register param set */
	reg = mmio_read_32(AXI_ADSPLCR0);
	reg = reg | ADSPLCR_ADRMODE_DEFAULT
		  | ADSPLCR_SPLITSEL(extra_split_enable)
		  | ADSPLCR_AREA(area_size)
		  | ADSPLCR_SWP;
	mmio_write_32(AXI_ADSPLCR0, reg);

	reg = mmio_read_32(AXI_ADSPLCR1);
	reg = (reg ^ ADSPLCR_SPLITSEL(area_ecc_enable)) & reg;
	mmio_write_32(AXI_ADSPLCR1, reg);

	reg = mmio_read_32(AXI_ADSPLCR3);
	reg = reg | ADSPLCR_ADRMODE_DEFAULT
		  | ADSPLCR_SPLITSEL(extra_area_ch)
		  | ADSPLCR_AREA(extra_area_size)
		  | ADSPLCR_SWP;
	mmio_write_32(AXI_ADSPLCR3, reg);
}
#endif /*
	* (RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_AUTO) \
	* || (RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_2CH)
	*/

#if RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_LINEAR
static void dual_ecc_setting_linear(void)
{
	/* extra split config param */
	uint32_t extra_split_enable = 0U;

	uint32_t reg;
	uint32_t i;

	/* register param get */
	for (i = 0U; i < AREA_NUM; i++) {
		extra_split_enable	|= (extra_split_enable_bit[i]
					 << REVERSE_SET(i));
		area_ecc_enable		|= (area_ecc_enable_bit[i]
					 << REVERSE_SET(i));
		extra_area_ecc_enable	|= (extra_area_ecc_enable_bit[i]
					 << REVERSE_SET(i));
	}

	/* extra split check */
	for (i = 0U; i < AREA_NUM; i++) {
		if ((extra_split_enable & BIT_SHIFT(i)) != OFF_BIT) {
			/* extra split enable area must be one */
			extra_split_enable = extra_split_enable & BIT_SHIFT(i);
			break;
		}
	}

	/* register param set */
	reg = mmio_read_32(AXI_ADSPLCR0);
	reg = reg | ADSPLCR_ADRMODE_DEFAULT
		  | ADSPLCR_SPLITSEL(extra_split_enable)
		  | ADSPLCR_AREA(area_size)
		  | ADSPLCR_SWP;
	mmio_write_32(AXI_ADSPLCR0, reg);

	reg = mmio_read_32(AXI_ADSPLCR1);
	reg = reg | ADSPLCR_ADRMODE_DEFAULT
		  | ADSPLCR_SPLITSEL(extra_split_enable)
		  | ADSPLCR_AREA(area_size)
		  | ADSPLCR_SWP;
	mmio_write_32(AXI_ADSPLCR1, reg);

	reg = mmio_read_32(AXI_ADSPLCR3);
	reg = reg | ADSPLCR_ADRMODE_DEFAULT
		  | ADSPLCR_AREA(extra_area_size)
		  | ADSPLCR_SWP;
	mmio_write_32(AXI_ADSPLCR3, reg);
}
#endif /* RCAR_DRAM_SPLIT == RCAR_DRAM_SPLIT_LINEAR */

#endif /* RCAR_ECC_ENABLE == DUAL_ECC_OPT_ENABLE */
