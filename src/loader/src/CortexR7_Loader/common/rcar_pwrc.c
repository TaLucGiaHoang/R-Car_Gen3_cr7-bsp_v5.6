/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */


#include "mmio.h"
#include "debug.h"
#include "rcar_def.h"
#include "rcar_pwrc.h"

/************************************************************************/
/*									*/
/*	Definition of R-Car Series, 3rd Generation register address	*/
/*									*/
/************************************************************************/

/* Timer	*/
/* The base addess of generic timer control register			*/
#define	RCAR_CNTC_BASE		(0xE6080000U)

/* Reset	*/
/* CPG write protect register						*/
#define	RCAR_CPGWPR		(0xE6150900U)
/* Mode pin register							*/
#define	RCAR_MODEMR		(0xE6160060U)
/* Mode pin register for Assembly language				*/
#define	RCAR_MODEMR_A		(0xE6160060U)
/* Reset control register for A57					*/
#define	RCAR_CA57RESCNT		(0xE6160040U)
/* Reset control register for A53					*/
#define	RCAR_CA53RESCNT		(0xE6160044U)
/* Wake-up control register for A53					*/
#define	RCAR_CA53WUPCR		(0xE6151010U)
/* Wake-up control register for A57					*/
#define	RCAR_CA57WUPCR		(0xE6152010U)
/* Power status register for A53					*/
#define	RCAR_CA53PSTR		(0xE6151040U)
/* Power status register for A57					*/
#define	RCAR_CA57PSTR		(0xE6152040U)
/* The base address of CPU power status control register for A53	*/
#define	RCAR_CA53CPU0CR		(0xE6151100U)
/* The base address of CPU power status control register for A57	*/
#define	RCAR_CA57CPU0CR		(0xE6152100U)
/* Common power control register for A53				*/
#define	RCAR_CA53CPUCMCR	(0xE6151184U)
/* Common power control register for A57				*/
#define	RCAR_CA57CPUCMCR	(0xE6152184U)
/* Wake-up mask register for A57					*/
#define	RCAR_WUPMSKCA57		(0xE6180014U)
/* Wake-up mask register for A53					*/
#define	RCAR_WUPMSKCA53		(0xE6180018U)

/* SYSC		*/
/* Power status register CA53-SCU					*/
#define	RCAR_PWRSR3		(0xE6180140U)
/* Power status register CA57-SCU					*/
#define	RCAR_PWRSR5		(0xE61801C0U)
/* Interrupt enable register						*/
#define	RCAR_SYSCIER		(0xE618000CU)
/* Interrupt mask register						*/
#define	RCAR_SYSCIMR		(0xE6180010U)
/* SYSC status register							*/
#define	RCAR_SYSCSR		(0xE6180000U)
/* Power resume control register CA53-SCU				*/
#define	RCAR_PWRONCR3		(0xE618014CU)
/* Power resume control register CA57-SCU				*/
#define	RCAR_PWRONCR5		(0xE61801CCU)
/* Power shutoff/resume error register CA53-SCU				*/
#define	RCAR_PWRER3		(0xE6180154U)
/* Power shutoff/resume error register CA57-SCU				*/
#define	RCAR_PWRER5		(0xE61801D4U)
/* Interrupt status register						*/
#define	RCAR_SYSCISR		(0xE6180004U)
/* Interrupt status clear register					*/
#define	RCAR_SYSCISCR		(0xE6180008U)


#define	BIT_CA53_SCU		((uint32_t)1U<<21U)
#define	BIT_CA57_SCU		((uint32_t)1U<<12U)
#define	REQ_RESUME		((uint32_t)1U<<1U)
#define	REQ_OFF			((uint32_t)1U<<0U)
#define	STATUS_PWRUP		((uint32_t)1U<<1U)
#define	STATUS_PWRDOWN		((uint32_t)1U<<0U)

#define UPPER_BA_IS_ZERO	(0U)

#define ARMREG_COMMON_0		(0xE6270000U)
#define DMS_SW			((uint32_t)1U<<4U)

#define UPPER_32BIT_ADDR	(32U)
#define WUPCR_CPUWUP		(1U)
#define CA5X_CORE_NUM_MAX	(3U)
#define PWRONCR_PWRUP_START	(0x0001U)
#define PWRER_ERR		(0x0001U)

/* Boot CPU core number of CA57/CA53 */
#define	CPU_NUM			(0U)	/* core 0 */

#if RCAR_KICK_MAIN_CPU == MAIN_CPU_CA57
#define PRR_CA5XEN		((uint32_t)1U << 31U)
#define NO_CA5X_CPUS		(0x80000000U)
#define	RST_CA5XCPU0BARL	(0xE61600C4U)
#define	RST_CA5XCPU0BARH	(0xE61600C0U)
#define	RES_REG			((uintptr_t)RCAR_CA57RESCNT)
#define	ON_REG			((uintptr_t)RCAR_CA57WUPCR)
#define	UPPER_VALUE		((uint32_t)0xA5A50000U)

#define	REG_SYSC_BIT		((uint32_t)BIT_CA57_SCU)
#define	REG_PWRONCR		((uintptr_t)RCAR_PWRONCR5)
#define	REG_PWRSR		((uintptr_t)RCAR_PWRSR5)
#define	REG_PWRER		((uintptr_t)RCAR_PWRER5)

#elif RCAR_KICK_MAIN_CPU == MAIN_CPU_CA53
#define PRR_CA5XEN		((uint32_t)1U << 26U)
#define NO_CA5X_CPUS		(0x04000000U)
#define	RST_CA5XCPU0BARL	(0xE6160084U)
#define	RST_CA5XCPU0BARH	(0xE6160080U)
#define	RES_REG			((uintptr_t)RCAR_CA53RESCNT)
#define	ON_REG			((uintptr_t)RCAR_CA53WUPCR)
#define	UPPER_VALUE		((uint32_t)0x5A5A0000U)

#define	REG_SYSC_BIT		((uint32_t)BIT_CA53_SCU)
#define	REG_PWRONCR		((uintptr_t)RCAR_PWRONCR3)
#define	REG_PWRSR		((uintptr_t)RCAR_PWRSR3)
#define	REG_PWRER		((uintptr_t)RCAR_PWRER3)

#endif

#if (RCAR_KICK_MAIN_CPU != MAIN_CPU_NONE)
static void SCU_power_up(void);
#endif

void rcar_pwrc_cpuon(uint32_t boot_addr)
{
#if (RCAR_KICK_MAIN_CPU == MAIN_CPU_NONE)
	return;
#else
	uint32_t res_data;
	uint32_t prr;
	uint32_t reg;

	/* Check PRR whether there are CA57/53 CPUs */
	prr = mmio_read_32(RCAR_PRR);
	if ((prr & PRR_CA5XEN) == NO_CA5X_CPUS) {
		ERROR("There are no specified CA5x CPUs\n");
		panic();
	}

	/*
	 * CA57/CA53 boot address setting
	 * Boot address is must be 32 bit address space.
	 */
	mmio_write_32(RST_CA5XCPU0BARL, boot_addr);
	mmio_write_32(RST_CA5XCPU0BARH, UPPER_BA_IS_ZERO);

	SCU_power_up();

	/* DMS Disable for M3N */
	prr = mmio_read_32(RCAR_PRR);
	prr &= (RCAR_PRODUCT_MASK | RCAR_CUT_MASK);
	if (prr == RCAR_PRODUCT_M3N_CUT10) {
		reg = mmio_read_32(ARMREG_COMMON_0);
		reg = (reg & ~DMS_SW);
		mmio_write_32(ARMREG_COMMON_0, reg);
	}

	mmio_write_32(ON_REG, (uint32_t)((uint32_t)1U << CPU_NUM));
	/* WUPCR[0]=0? */
	while (((mmio_read_32(ON_REG)) &
		((uint32_t)1U << CPU_NUM))
		== WUPCR_CPUWUP) {
		;
	}
	res_data = mmio_read_32(RES_REG) | UPPER_VALUE;
	mmio_write_32(RES_REG, (res_data & (~((uint32_t)1U
					<< (CA5X_CORE_NUM_MAX - CPU_NUM)))));
#endif
}

#if (RCAR_KICK_MAIN_CPU != MAIN_CPU_NONE)
static void SCU_power_up(void)
{
	const uintptr_t reg_SYSCIER  = (uintptr_t)RCAR_SYSCIER;
	const uintptr_t reg_SYSCIMR  = (uintptr_t)RCAR_SYSCIMR;
	const uintptr_t reg_SYSCSR   = (uintptr_t)RCAR_SYSCSR;
	const uintptr_t reg_SYSCISR  = (uintptr_t)RCAR_SYSCISR;
	const uintptr_t reg_SYSCISCR = (uintptr_t)RCAR_SYSCISCR;

	if ((mmio_read_32(REG_PWRSR) & STATUS_PWRDOWN) != 0x0000U) {
		/* set SYSCIER and SYSCIMR */
		mmio_write_32(reg_SYSCIER,
			      (mmio_read_32(reg_SYSCIER) | REG_SYSC_BIT));
		mmio_write_32(reg_SYSCIMR,
			      (mmio_read_32(reg_SYSCIMR) | REG_SYSC_BIT));
		do {
			/* SYSCSR[1]=1? */
			while ((mmio_read_32(reg_SYSCSR)
			       & (uint32_t)REQ_RESUME) == 0U) {
				;
			}
			/* If SYSCSR[1]=1 then set bit in PWRONCRn to 1 */
			mmio_write_32(REG_PWRONCR, PWRONCR_PWRUP_START);
		} while ((mmio_read_32(REG_PWRER) & PWRER_ERR) != 0U);

		/* bit in SYSCISR=1 ? */
		while ((mmio_read_32(reg_SYSCISR) & REG_SYSC_BIT) == 0U) {
			;
		}
		/* clear bit in SYSCISR */
		mmio_write_32(reg_SYSCISCR, REG_SYSC_BIT);
	}
}
#endif
