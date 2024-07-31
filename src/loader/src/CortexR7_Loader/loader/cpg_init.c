/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "mmio.h"
#include "cpg_register.h"
#include "cpg_init.h"
#include "rcar_def.h"
#include "debug.h"

static void cpg_write(uintptr_t regadr, uint32_t regval);

#if (RCAR_LSI == RCAR_AUTO) || (RCAR_LSI == RCAR_H3) || (RCAR_LSI == RCAR_H3N)
static void realtime_cpg_init_h3(void);
#endif /*
	* (RCAR_LSI == RCAR_AUTO) || (RCAR_LSI == RCAR_H3)
	*			  || (RCAR_LSI == RCAR_H3N)
	*/
#if (RCAR_LSI == RCAR_AUTO) || (RCAR_LSI == RCAR_M3)
static void realtime_cpg_init_m3(void);
#endif /* (RCAR_LSI == RCAR_AUTO) || (RCAR_LSI == RCAR_M3) */
#if (RCAR_LSI == RCAR_AUTO) || (RCAR_LSI == RCAR_M3N)
static void realtime_cpg_init_m3n(void);
#endif /* (RCAR_LSI == RCAR_AUTO) || (RCAR_LSI == RCAR_M3N) */
#if (RCAR_LSI == RCAR_E3)
static void realtime_cpg_init_e3(void);
#endif  /* (RCAR_LSI == RCAR_E3) */

#if (RCAR_LSI == RCAR_AUTO) || (RCAR_LSI == RCAR_H3) || (RCAR_LSI == RCAR_H3N)
static void realtime_cpg_init_h3(void)
{
	uint32_t	prr;

	prr = mmio_read_32(RCAR_PRR) & RCAR_CUT_MASK;

	/* CPG (REALTIME) registers */

	if ((prr == RCAR_CUT_VER10) || (prr == RCAR_CUT_VER11)) {
		/* Realtime Module Stop Control Register 0 */
		cpg_write(RMSTPCR0,	0x00000000U);
	} else {
		/* Realtime Module Stop Control Register 0 */
		cpg_write(RMSTPCR0,	0x00010000U);
	}
	/* Realtime Module Stop Control Register 1 */
	cpg_write(RMSTPCR1,	0xFFFFFFFFU);
	/* Realtime Module Stop Control Register 2 */
	cpg_write(RMSTPCR2,	0x040E0FDCU);
	/* Realtime Module Stop Control Register 3 */
	cpg_write(RMSTPCR3,	0xFFFFFFDFU);
	/* Realtime Module Stop Control Register 4 */
	cpg_write(RMSTPCR4,	0x80000004U);
	/* Realtime Module Stop Control Register 5 */
	cpg_write(RMSTPCR5,	0xC3FFFFFFU);
	/* Realtime Module Stop Control Register 6 */
	cpg_write(RMSTPCR6,	0xFFFFFFFFU);
	/* Realtime Module Stop Control Register 7 */
	cpg_write(RMSTPCR7,	0xFFFFFFFFU);
	if ((prr == RCAR_CUT_VER10) || (prr == RCAR_CUT_VER11)) {
		/* Realtime Module Stop Control Register 8 */
		cpg_write(RMSTPCR8,	0x01F1FFF4U);
	} else {
		/* Realtime Module Stop Control Register 8 */
		cpg_write(RMSTPCR8,	0x01F1FFF7U);
	}
	/* Realtime Module Stop Control Register 9 */
	cpg_write(RMSTPCR9,	0xFFFFFFFFU);
	/* Realtime Module Stop Control Register 10 */
	cpg_write(RMSTPCR10,	0xFFFEFFE0U);
	/* Realtime Module Stop Control Register 11 */
	cpg_write(RMSTPCR11,	0x000000B7U);
}
#endif /*
	* (RCAR_LSI == RCAR_AUTO) || (RCAR_LSI == RCAR_H3)
	*			  || (RCAR_LSI == RCAR_H3N)
	*/

#if (RCAR_LSI == RCAR_AUTO) || (RCAR_LSI == RCAR_M3)
static void realtime_cpg_init_m3(void)
{
	/* CPG (REALTIME) registers */

	/* Realtime Module Stop Control Register 0 */
	cpg_write(RMSTPCR0,	0x00000000U);
	/* Realtime Module Stop Control Register 1 */
	cpg_write(RMSTPCR1,	0xFFFFFFFFU);
	/* Realtime Module Stop Control Register 2 */
	cpg_write(RMSTPCR2,	0x040E0FDCU);
	/* Realtime Module Stop Control Register 3 */
	cpg_write(RMSTPCR3,	0xFFFFFFDFU);
	/* Realtime Module Stop Control Register 4 */
	cpg_write(RMSTPCR4,	0x80000004U);
	/* Realtime Module Stop Control Register 5 */
	cpg_write(RMSTPCR5,	0xC3FFFFFFU);
	/* Realtime Module Stop Control Register 6 */
	cpg_write(RMSTPCR6,	0xFFFFFFFFU);
	/* Realtime Module Stop Control Register 7 */
	cpg_write(RMSTPCR7,	0xFFFFFFFFU);
	/* Realtime Module Stop Control Register 8 */
	cpg_write(RMSTPCR8,	0x01F1FFF7U);
	/* Realtime Module Stop Control Register 9 */
	cpg_write(RMSTPCR9,	0xFFFFFFFFU);
	/* Realtime Module Stop Control Register 10 */
	cpg_write(RMSTPCR10,	0xFFFEFFE0U);
	/* Realtime Module Stop Control Register 11 */
	cpg_write(RMSTPCR11,	0x000000B7U);
}
#endif /* (RCAR_LSI == RCAR_AUTO) || (RCAR_LSI == RCAR_M3) */

#if (RCAR_LSI == RCAR_AUTO) || (RCAR_LSI == RCAR_M3N)
static void realtime_cpg_init_m3n(void)
{
	/* CPG (REALTIME) registers */

	/* Realtime Module Stop Control Register 0 */
	cpg_write(RMSTPCR0,	0x00010000U);
	/* Realtime Module Stop Control Register 1 */
	cpg_write(RMSTPCR1,	0xFFFFFFFFU);
	/* Realtime Module Stop Control Register 2 */
	cpg_write(RMSTPCR2,	0x040E0FDCU);
	/* Realtime Module Stop Control Register 3 */
	cpg_write(RMSTPCR3,	0xFFFFFFDFU);
	/* Realtime Module Stop Control Register 4 */
	cpg_write(RMSTPCR4,	0x80000004U);
	/* Realtime Module Stop Control Register 5 */
	cpg_write(RMSTPCR5,	0xC3FFFFFFU);
	/* Realtime Module Stop Control Register 6 */
	cpg_write(RMSTPCR6,	0xFFFFFFFFU);
	/* Realtime Module Stop Control Register 7 */
	cpg_write(RMSTPCR7,	0xFFFFFFFFU);
	/* Realtime Module Stop Control Register 8 */
	cpg_write(RMSTPCR8,	0x00F1FFF7U);
	/* Realtime Module Stop Control Register 9 */
	cpg_write(RMSTPCR9,	0xFFFFFFFFU);
	/* Realtime Module Stop Control Register 10 */
	cpg_write(RMSTPCR10,	0xFFFFFFE0U);
	/* Realtime Module Stop Control Register 11 */
	cpg_write(RMSTPCR11,	0x000000B7U);
}
#endif /* (RCAR_LSI == RCAR_AUTO) || (RCAR_LSI == RCAR_M3N) */

#if (RCAR_LSI == RCAR_E3)
static void realtime_cpg_init_e3(void)
{
	/* CPG (REALTIME) registers */

	/* Realtime Module Stop Control Register 0 */
	cpg_write(RMSTPCR0,	0x00010000U);
	/* Realtime Module Stop Control Register 1 */
	cpg_write(RMSTPCR1,	0xFFFFFFFFU);
	/* Realtime Module Stop Control Register 2 */
	cpg_write(RMSTPCR2,	0x000E0FDCU);
	/* Realtime Module Stop Control Register 3 */
	cpg_write(RMSTPCR3,	0xFFFFFFDFU);
	/* Realtime Module Stop Control Register 4 */
	cpg_write(RMSTPCR4,	0x80000004U);
	/* Realtime Module Stop Control Register 5 */
	cpg_write(RMSTPCR5,	0xC3FFFFFFU);
	/* Realtime Module Stop Control Register 6 */
	cpg_write(RMSTPCR6,	0xFFFFFFFFU);
	/* Realtime Module Stop Control Register 7 */
	cpg_write(RMSTPCR7,	0xFFFFFFFFU);
	/* Realtime Module Stop Control Register 8 */
	cpg_write(RMSTPCR8,	0x00F1FFF7U);
	/* Realtime Module Stop Control Register 9 */
	cpg_write(RMSTPCR9,	0xFFFFFFDFU);
	/* Realtime Module Stop Control Register 10 */
	cpg_write(RMSTPCR10,	0xFFFFFFE8U);
	/* Realtime Module Stop Control Register 11 */
	cpg_write(RMSTPCR11,	0x000000B7U);
}
#endif /* RCAR_LSI == RCAR_E3 */

void cpg_init(void)
{
#if RCAR_LSI == RCAR_AUTO
	uint32_t	prr;
#endif /* RCAR_LSI == RCAR_AUTO */

#if RCAR_LSI == RCAR_AUTO
	prr = mmio_read_32(RCAR_PRR) & RCAR_PRODUCT_MASK;
	switch (prr) {
	case RCAR_PRODUCT_H3:
		realtime_cpg_init_h3();
		break;
	case RCAR_PRODUCT_M3:
		realtime_cpg_init_m3();
		break;
	case RCAR_PRODUCT_M3N:
		realtime_cpg_init_m3n();
		break;
	default:
		ERROR("LSI Product ID(PRR=0x%x) CPG\n", prr);
		ERROR("initialize not supported.\n");
		panic();
		break;
	}
#elif (RCAR_LSI == RCAR_H3) || (RCAR_LSI == RCAR_H3N)
	realtime_cpg_init_h3();
#elif RCAR_LSI == RCAR_M3
	realtime_cpg_init_m3();
#elif RCAR_LSI == RCAR_M3N
	realtime_cpg_init_m3n();
#elif RCAR_LSI == RCAR_E3
	realtime_cpg_init_e3();
#else /* RCAR_LSI == RCAR_XX */
#error "Don't have CPG initialize routine(unknown)."
#endif /* RCAR_LSI == RCAR_XX */
}

static void cpg_write(uintptr_t regadr, uint32_t regval)
{
	uint32_t value = (regval);

	mmio_write_32((uintptr_t)CPG_CPGWPR, ~value);
	mmio_write_32(regadr, value);
}

void mstpcr_write(uint32_t mstpcr, uint32_t mstpsr, uint32_t target_bit)
{
	uint32_t reg;

	reg = mmio_read_32(mstpcr);
	reg &= ~target_bit;
	cpg_write(mstpcr, reg);
	do {
		reg = mmio_read_32(mstpsr);
	} while ((reg & target_bit) != 0U);

}
