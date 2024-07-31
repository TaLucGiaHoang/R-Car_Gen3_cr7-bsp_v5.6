/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "axi_register.h"
#include "protection_setting.h"
#include "mmio.h"
#include "micro_wait.h"

#include "arm_gic.h"
#include "gic_v2.h"
#include "rcar_def.h"
#include "debug.h"
#include "bl_common.h"

#define CR7_GICD_CTLR		(GICD_BASE + GICD_CTLR)
#define CR7_GICD_IGROUPR	(GICD_BASE + GICD_IGROUPR)
#define CR7_GICD_ISENABLER	(GICD_BASE + GICD_ISENABLER)
#define CR7_GICD_IPRIORITYR	(GICD_BASE + GICD_IPRIORITYR)
#define CR7_GICD_ITARGETSR	(GICD_BASE + GICD_ITARGETSR)
#define CR7_GICC_CTLR		(GICC_BASE + GICC_CTLR)

#define TARGET_CPU		(0x01U)

#define SPI_NUM		(512U)

/* LIFEC0 (SECURITY) base address */
#define	LIFEC_SEC_BASE	(0xE6110000U)

/* Security attribute setting for master ports */
#define	SEC_SRC		(LIFEC_SEC_BASE + 0x0008U)

/* SRAM/DRAM protection setting */
void axi_security_setting(void)
{
	int32_t		i;
	int32_t		ie;

	/* AXI settings */
static const struct {
	uint32_t	adr;
	uint32_t	val;
} axi_reg_setting[] = {

	/* DRAM protection */

	/* AXI dram protected area division */
	{AXI_DPTDIVCR0,		0x0E0403F0U},
	{AXI_DPTDIVCR1,		0x0E0407E0U},
	{AXI_DPTDIVCR2,		0x0E080000U},
	{AXI_DPTDIVCR3,		0x0E080000U},
	{AXI_DPTDIVCR4,		0x0E080000U},
	{AXI_DPTDIVCR5,		0x0E080000U},
	{AXI_DPTDIVCR6,		0x0E080000U},
	{AXI_DPTDIVCR7,		0x0E080000U},
	{AXI_DPTDIVCR8,		0x0E080000U},
	{AXI_DPTDIVCR9,		0x0E080000U},
	{AXI_DPTDIVCR10,	0x0E080000U},
	{AXI_DPTDIVCR11,	0x0E080000U},
	{AXI_DPTDIVCR12,	0x0E080000U},
	{AXI_DPTDIVCR13,	0x0E080000U},
	{AXI_DPTDIVCR14,	0x0E080000U},

	/* AXI dram protected area setting */
	{AXI_DPTCR0,		0x0E000000U},
	{AXI_DPTCR1,		0x0E000E0EU},
	{AXI_DPTCR2,		0x0E000000U},
	{AXI_DPTCR3,		0x0E000000U},
	{AXI_DPTCR4,		0x0E000000U},
	{AXI_DPTCR5,		0x0E000000U},
	{AXI_DPTCR6,		0x0E000000U},
	{AXI_DPTCR7,		0x0E000000U},
	{AXI_DPTCR8,		0x0E000000U},
	{AXI_DPTCR9,		0x0E000000U},
	{AXI_DPTCR10,		0x0E000000U},
	{AXI_DPTCR11,		0x0E000000U},
	{AXI_DPTCR12,		0x0E000000U},
	{AXI_DPTCR13,		0x0E000000U},
	{AXI_DPTCR14,		0x0E000000U},
	{AXI_DPTCR15,		0x0E000000U},


	/* SRAM ptotection */

	/* AXI sram protected area division */
	{AXI_SPTDIVCR0,		0x0E0E6304U},
	{AXI_SPTDIVCR1,		0x0E0E6360U},
	{AXI_SPTDIVCR2,		0x0E0E6360U},
	{AXI_SPTDIVCR3,		0x0E0E6360U},
	{AXI_SPTDIVCR4,		0x0E0E6360U},
	{AXI_SPTDIVCR5,		0x0E0E6360U},
	{AXI_SPTDIVCR6,		0x0E0E6360U},
	{AXI_SPTDIVCR7,		0x0E0E6360U},
	{AXI_SPTDIVCR8,		0x0E0E6360U},
	{AXI_SPTDIVCR9,		0x0E0E6360U},
	{AXI_SPTDIVCR10,	0x0E0E6360U},
	{AXI_SPTDIVCR11,	0x0E0E6360U},
	{AXI_SPTDIVCR12,	0x0E0E6360U},
	{AXI_SPTDIVCR13,	0x0E0E6360U},
	{AXI_SPTDIVCR14,	0x0E0E6360U},

	/* AXI sram protected area setting */
	{AXI_SPTCR0,		0x0E000E0EU},
	{AXI_SPTCR1,		0x0E000000U},
	{AXI_SPTCR2,		0x0E000000U},
	{AXI_SPTCR3,		0x0E000000U},
	{AXI_SPTCR4,		0x0E000000U},
	{AXI_SPTCR5,		0x0E000000U},
	{AXI_SPTCR6,		0x0E000000U},
	{AXI_SPTCR7,		0x0E000000U},
	{AXI_SPTCR8,		0x0E000000U},
	{AXI_SPTCR9,		0x0E000000U},
	{AXI_SPTCR10,		0x0E000000U},
	{AXI_SPTCR11,		0x0E000000U},
	{AXI_SPTCR12,		0x0E000000U},
	{AXI_SPTCR13,		0x0E000000U},
	{AXI_SPTCR14,		0x0E000000U},
	{AXI_SPTCR15,		0x0E000000U}

};


	i = 0;
	ie = (int32_t)(ARRAY_SIZE(axi_reg_setting));
	for (; i < ie; i++) {
		mmio_write_32((uintptr_t)axi_reg_setting[i].adr,
			axi_reg_setting[i].val);
	}
}

void gic_security_setting(void)
{
	/* When use function of interruption after IPL,			    */
	/* set the value to GICD_IGROUPRn registers and GICC_PMR register   */
	/* while CR7 is Secure (before calling lifec_cr7_setting function). */
	gicc_write_pmr(GICC_BASE, GIC_PRI_MASK);
}

