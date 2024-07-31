/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "mmio.h"
#include "arm_gic.h"
#include "gic_v2.h"
#include "debug.h"
#include "swdt.h"
#include "rcar_def.h"
#include "cpsr_acc.h"

/* To change SWDT over flow sec, change the value of SWDT_COUNT_SEC */
#define SWDT_COUNT_SEC		(5U)		/* set param(1--10sec) */
#if (SWDT_COUNT_SEC < 1) || (10 < SWDT_COUNT_SEC)
#error The value of SWDT_COUNT_SEC is out of the valid range.
#endif /* (SWDT_COUNT_SEC < 1) || (10 < SWDT_COUNT_SEC) */

/* Use WDT Regsiters */
#define RST_BASE		(0xE6160000U)
#define RST_WDTRSTCR		(RST_BASE + 0x0054U)
#define SWDT_BASE		(0xE6030000U)
#define SWDT_WTCNT		(SWDT_BASE + 0x0000U)
#define SWDT_WTCSRA		(SWDT_BASE + 0x0004U)
#define SWDT_WTCSRB		(SWDT_BASE + 0x0008U)

/* Use GIC Regsiters */
#define SWDT_GICD_CTLR		(GICD_BASE + GICD_CTLR)
#define SWDT_GICD_IGROUPR	(GICD_BASE + GICD_IGROUPR)
#define SWDT_GICD_ISPRIORITYR	(GICD_BASE + GICD_IPRIORITYR)
#define SWDT_GICD_ITARGETSR	(GICD_BASE + GICD_ITARGETSR)
#define SWDT_GICC_CTLR		(GICC_BASE + GICC_CTLR)
#define SWDT_GICC_PMR		(GICC_BASE + GICC_PMR)
/* GIC Parameter */
#define IGROUPR_NUM		(16U)
#define IGROUPR_RELSET		(0xFFFFFFFF)
#define ISPRIORITY_NUM		(128U)
#define ITARGET_MASK		((uint32_t)0x03U)

/* WDT Parameter */
#define WDTRSTCR_UPPER_BYTE	(0xA55A0000U)
#define WTCSRA_UPPER_BYTE	(0xA5A5A500U)
#define WTCSRB_UPPER_BYTE	(0xA5A5A500U)
#define WTCNT_UPPER_BYTE	(0x5A5A0000U)
#define WTCNT_RESET_VALUE	(0xF488U)
#define WTCSRA_BIT_CKS		(0x0007U)
#define WTCSRB_BIT_CKS		(0x003FU)
#define SWDT_RSTMSK		((uint32_t)1U << 1U)
#define WTCSRA_WOVFE		((uint32_t)1U << 3U)
#define WTCSRA_WRFLG		((uint32_t)1U << 5U)
#define SWDT_ENABLE		((uint32_t)1U << 7U)

#define WDTRSTCR_MASK_ALL	(0x0000FFFFU)
#define WTCSRA_MASK_ALL		(0x000000FFU)
#define WTCNT_INIT_DATA		(WTCNT_UPPER_BYTE + WTCNT_RESET_VALUE)
#define WTCSRA_INIT_DATA	(WTCSRA_UPPER_BYTE + 0x0FU)
#define WTCSRB_INIT_DATA	(WTCSRB_UPPER_BYTE + 0x21U)

/* WDT Timeout Setting */
/* OSCCLK */
#define OSCCLK_131570HZ		(131570U)	/* MD13=H MD14=L*/
#define OSCCLK_130200HZ		(130200U)	/* other */
#define OSCCLK_125000HZ		(125000U)	/* E3 */

/* CKS0 setting */
#define RPHI_1			(1U)		/* 000:RPhi */
#define RPHI_4			(4U)		/* 001:RPhi4 */
#define RPHI_16			(16U)		/* 010:RPhi16 */
#define RPHI_32			(32U)		/* 011:RPhi32 */
#define RPHI_64			(64U)		/* 100:RPhi64 */
#define RPHI_128		(128U)		/* 101:RPhi128 */
#define RPHI_1024		(1024U)		/* 110:RPhi1024 */

#define SWTCSRA_CKS		(0x00000003U)	/* division ratio RPhi32*/

/* clock */
/* (micro sec / (Hz / RPhi) */
#define CLK_131570HZ		((uint32_t)((1U * 1000U * 1000U) \
					     / (OSCCLK_131570HZ / RPHI_32)))
#define CLK_130200HZ		((uint32_t)((1U * 1000U * 1000U) \
					     / (OSCCLK_130200HZ / RPHI_32)))
#define CLK_125000HZ		((uint32_t)((1U * 1000U * 1000U) \
				/ (OSCCLK_125000HZ / RPHI_32)))	/* E3 */

/* SWDT over flow sec need count*/
#define SWDT_COUNT_131570HZ	((uint32_t)((SWDT_COUNT_SEC * 1000U * 1000U) \
					     / CLK_131570HZ))
#define SWDT_COUNT_130200HZ	((uint32_t)((SWDT_COUNT_SEC * 1000U * 1000U) \
					     / CLK_130200HZ))
#define SWDT_COUNT_125000HZ	((uint32_t)((SWDT_COUNT_SEC * 1000U * 1000U) \
					     / CLK_125000HZ))	/* E3 */

/* SWDTCNT Setting param */
#define SWDTCNT_131570HZ	(0x10000U - SWDT_COUNT_131570HZ)
#define SWDTCNT_131570HZ_H3_VER10	((uint32_t)(0x10000U \
					     - (SWDT_COUNT_131570HZ/2U)))
#define SWDTCNT_130200HZ	(0x10000U - SWDT_COUNT_130200HZ)
#define SWDTCNT_130200HZ_H3_VER10	((uint32_t)(0x10000U \
					     - (SWDT_COUNT_130200HZ/2U)))
#define SWDTCNT_125000HZ	(0x10000U - \
				SWDT_COUNT_125000HZ)		/* E3 */

#define CHECK_MD13_MD14			(0x6000U)
#define FREQ_10p0M			(0x2000U)
#define FREQ_12p5M			(0x4000U)
#define FREQ_8p33M			(0x0000U)
#define FREQ_16p66M			(0x6000U)

/* Mode pin register				*/
#define	RCAR_MODEMR			(0xE6160060U)
/* Mode pin register for Assembly language	*/
#define	RCAR_MODEMR_A			(0xE6160060U)

/* Processer State Register*/
#define PSR_A			((uint32_t)1U << 8U)	/* Abort */
#define PSR_I			((uint32_t)1U << 7U)	/* IRQ */
#define PSR_F			((uint32_t)1U << 6U)	/* FIQ */

#define INITIAL_VALUE		(0U)

static void swdt_disable(void);

void swdt_init(void)
{
	uint32_t sr;
	uint32_t rmsk;
#if (RCAR_LSI != RCAR_E3)
	uint32_t product_cut = mmio_read_32((uintptr_t)RCAR_PRR)
				& (RCAR_PRODUCT_MASK | RCAR_CUT_MASK);
	uint32_t chk_data = mmio_read_32((uintptr_t)RCAR_MODEMR)
							& CHECK_MD13_MD14;
#endif /* RCAR_LSI != RCAR_E3 */

	if ((mmio_read_32(SWDT_WTCSRA) & SWDT_ENABLE) != 0U) {
		/* Stop SWDT	*/
		mmio_write_32(SWDT_WTCSRA, WTCSRA_UPPER_BYTE);
	}

	/* clock is OSC/16 and overflow interrupt is enabled	*/
	mmio_write_32(SWDT_WTCSRA, (WTCSRA_UPPER_BYTE | WTCSRA_WOVFE
							 | SWTCSRA_CKS));

	/* Set the overflow counter				*/
	/* The System Watchdog timer is a single-channel timer  */
	/* that uses the OSCCLK as an input clock and can be    */
	/* used as a watchdog timer.                            */
#if (RCAR_LSI == RCAR_E3)
	/* OSCCLK=125kHz					*/
	mmio_write_32(SWDT_WTCNT, (WTCNT_UPPER_BYTE | SWDTCNT_125000HZ));
#else /* RCAR_LSI == RCAR_E3 */
	switch (chk_data) {
	case FREQ_8p33M:	/* MD13=0 and MD14=0		*/
		/* No break */
	case FREQ_12p5M:	/* MD13=0 and MD14=1		*/
		mmio_write_32(SWDT_WTCNT,
				(WTCNT_UPPER_BYTE | SWDTCNT_130200HZ));
		break;
	case FREQ_10p0M:	/* MD13=1 and MD14=0		*/
		mmio_write_32(SWDT_WTCNT,
				(WTCNT_UPPER_BYTE | SWDTCNT_131570HZ));
		break;
	case FREQ_16p66M:	/* MD13=1 and MD14=1		*/
		/* OSCCLK=130.2kHz				*/
		if (product_cut == (RCAR_PRODUCT_H3 | RCAR_CUT_VER10)) {
			/* R-car H3 Ver.1.0			*/
			mmio_write_32(SWDT_WTCNT,
				(WTCNT_UPPER_BYTE | SWDTCNT_130200HZ_H3_VER10));
		} else {
			/* R-car H3 other			*/
			mmio_write_32(SWDT_WTCNT,
				(WTCNT_UPPER_BYTE | SWDTCNT_130200HZ));
		}
		break;
	default:
		/* Error					*/
		ERROR("CR7: MODEMR ERROR value=%x\n", chk_data);
		panic();
		break;
	}
#endif /* RCAR_LSI == RCAR_E3 */

	rmsk = mmio_read_32(RST_WDTRSTCR) & WDTRSTCR_MASK_ALL;
	mmio_write_32(RST_WDTRSTCR,
		(WDTRSTCR_UPPER_BYTE | (rmsk | SWDT_RSTMSK)));
	while ((mmio_read_8(SWDT_WTCSRA) & WTCSRA_WRFLG) != 0U) {
		/* Wait until the WTCNT is reflected		*/
		;
	}
	sr = mmio_read_32(SWDT_WTCSRA) & WTCSRA_MASK_ALL;
	/* Start the System WatchDog Timer			*/
	mmio_write_32(SWDT_WTCSRA, (WTCSRA_UPPER_BYTE | sr | SWDT_ENABLE));
}

static void swdt_disable(void)
{
	uint32_t rmsk;
	uintptr_t base = (uintptr_t)GICD_BASE;
	uint32_t id = (uint32_t)ARM_IRQ_WDT;

	/* Initialize the HW initial data, but SWDT is not moved	*/
	rmsk = mmio_read_32(RST_WDTRSTCR) & WDTRSTCR_MASK_ALL;
	mmio_write_32(RST_WDTRSTCR,
		(WDTRSTCR_UPPER_BYTE | (rmsk | SWDT_RSTMSK)));
	mmio_write_32(SWDT_WTCNT, WTCNT_INIT_DATA);
	mmio_write_32(SWDT_WTCSRA, WTCSRA_INIT_DATA);
	mmio_write_32(SWDT_WTCSRB, WTCSRB_INIT_DATA);
	/* Set the interrupt clear enable register			*/
	gicd_set_icenabler(base, id);
}

void swdt_release(void)
{
#if RCAR_LIFEC_NON_SECURE_MASTER
	uintptr_t base = (uintptr_t)GICD_BASE;
#endif
	uintptr_t p_gicd_ctlr = (uintptr_t)SWDT_GICD_CTLR;
	uintptr_t p_gicc_ctlr = (uintptr_t)SWDT_GICC_CTLR;
	uintptr_t p_pmr = (uintptr_t)SWDT_GICC_PMR;
	uintptr_t p_itargetsr = (uintptr_t)(SWDT_GICD_ITARGETSR
				+ (ARM_IRQ_WDT & (uint32_t)(~ITARGET_MASK)));

	aif_disable_i();
	swdt_disable();
	arm_gic_cpuif_deactivate();
#if RCAR_LIFEC_NON_SECURE_MASTER
	gicd_set_igroupr(base, ARM_IRQ_WDT);
	gicd_set_ipriorityr(base, ARM_IRQ_WDT, GIC_HIGHEST_NS_PRIORITY);
#endif
	mmio_write_32(p_itargetsr, INITIAL_VALUE);
	mmio_write_32(p_gicd_ctlr, INITIAL_VALUE);
	mmio_write_32(p_gicc_ctlr, INITIAL_VALUE);
	mmio_write_32(p_pmr, INITIAL_VALUE);
}

void swdt_exec(uint32_t occ_add)
{
	/* Clear the interrupt request	*/
	arm_gic_end_of_interrupt((uint32_t)ARM_IRQ_WDT);
	swdt_release();
	ERROR("\n");
	ERROR("CR7: System WDT overflow\n");
	ERROR("     Occurred address : 0x%x\n", occ_add);
	/* Endless loop		*/
	panic();
}
