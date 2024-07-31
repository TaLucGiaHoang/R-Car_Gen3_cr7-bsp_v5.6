/*
 * Copyright (c) 2014, ARM Limited and Contributors. All rights reserved.
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arm_gic.h>
#include <assert.h>
#include <bl_common.h>
#include <debug.h>
#include <gic_v2.h>
#include <stdint.h>

/* Value used to initialize Non-Secure IRQ priorities four at a time */
#define GICD_IPRIORITYR_DEF_VAL \
	(GIC_HIGHEST_NS_PRIORITY | \
	(GIC_HIGHEST_NS_PRIORITY << 8) | \
	(GIC_HIGHEST_NS_PRIORITY << 16) | \
	(GIC_HIGHEST_NS_PRIORITY << 24))

static uintptr_t g_gicc_base;
static uintptr_t g_gicd_base;
static const unsigned int *g_irq_sec_ptr;
static unsigned int g_num_irqs;


/*******************************************************************************
 * Enable secure interrupts and use FIQs to route them. Disable legacy bypass
 * and set the priority mask register to allow all interrupts to trickle in.
 ******************************************************************************/
void arm_gic_cpuif_setup(void)
{
	unsigned int val;

	assert(g_gicc_base);
	val = gicc_read_iidr(g_gicc_base);

	/*
	 * Not used FIQ for CR7Loader
	 * So do not enable FIQ
	 */
	val = ENABLE_GRP0 | FIQ_BYP_DIS_GRP0;
	val |= IRQ_BYP_DIS_GRP0 | FIQ_BYP_DIS_GRP1 | IRQ_BYP_DIS_GRP1;

	gicc_write_pmr(g_gicc_base, GIC_PRI_MASK);
	gicc_write_ctlr(g_gicc_base, val);
}

/*******************************************************************************
 * Place the cpu interface in a state where it can never make a cpu exit wfi as
 * as result of an asserted interrupt. This is critical for powering down a cpu
 ******************************************************************************/
void arm_gic_cpuif_deactivate(void)
{
	unsigned int val;

	/* Disable secure, non-secure interrupts and disable their bypass */
	assert(g_gicc_base);
	val = gicc_read_ctlr(g_gicc_base);
	val &= ~(ENABLE_GRP0 | ENABLE_GRP1);
	val |= FIQ_BYP_DIS_GRP1 | FIQ_BYP_DIS_GRP0;
	val |= IRQ_BYP_DIS_GRP0 | IRQ_BYP_DIS_GRP1;
	gicc_write_ctlr(g_gicc_base, val);

	val = gicc_read_iidr(g_gicc_base);
}

/*******************************************************************************
 * Per cpu gic distributor setup which will be done by all cpus after a cold
 * boot/hotplug. This marks out the secure interrupts & enables them.
 ******************************************************************************/
void arm_gic_pcpu_distif_setup(void)
{
	unsigned int index, irq_num, sec_ppi_sgi_mask;

	assert(g_gicd_base);
#if RCAR_LIFEC_NON_SECURE_MASTER
	/* Setup PPI priorities doing four at a time */
	for (index = 0; index < 32; index += 4) {
		gicd_write_ipriorityr(g_gicd_base, index,
				GICD_IPRIORITYR_DEF_VAL);
	}
#endif
	assert(g_irq_sec_ptr);
	sec_ppi_sgi_mask = 0;
	for (index = 0; index < g_num_irqs; index++) {
		irq_num = g_irq_sec_ptr[index];
		if (irq_num < MIN_SPI_ID) {
			/* We have an SGI or a PPI. They are Group0 at reset */
			sec_ppi_sgi_mask |= 1U << irq_num;
			gicd_set_ipriorityr(g_gicd_base, irq_num,
				GIC_HIGHEST_SEC_PRIORITY);
			gicd_set_isenabler(g_gicd_base, irq_num);
		}
	}

	/*
	 * Invert the bitmask to create a mask for non-secure PPIs and
	 * SGIs. Program the GICD_IGROUPR0 with this bit mask. This write will
	 * update the GICR_IGROUPR0 as well in case we are running on a GICv3
	 * system. This is critical if GICD_CTLR.ARE_NS=1.
	 */
#if RCAR_LIFEC_NON_SECURE_MASTER
	gicd_write_igroupr(g_gicd_base, 0, ~sec_ppi_sgi_mask);
#endif
}

/*******************************************************************************
 * Get the current CPU bit mask from GICD_ITARGETSR0
 ******************************************************************************/
static unsigned int arm_gic_get_cpuif_id(void)
{
	unsigned int val;

	val = gicd_read_itargetsr(g_gicd_base, 0);
	return val & GIC_TARGET_CPU_MASK;
}

/*******************************************************************************
 * Global gic distributor setup which will be done by the primary cpu after a
 * cold boot. It marks out the secure SPIs, PPIs & SGIs and enables them. It
 * then enables the secure GIC distributor interface.
 ******************************************************************************/
static void arm_gic_distif_setup(void)
{
	unsigned int num_ints, ctlr, index, irq_num;
	uint8_t target_cpu;

	/* Disable the distributor before going further */
	assert(g_gicd_base);
	ctlr = gicd_read_ctlr(g_gicd_base);
	ctlr &= ~(ENABLE_GRP0 | ENABLE_GRP1);
	gicd_write_ctlr(g_gicd_base, ctlr);

	/*
	 * Mark out non-secure SPI interrupts. The number of interrupts is
	 * calculated as 32 * (IT_LINES + 1). We do 32 at a time.
	 */
#if RCAR_LIFEC_NON_SECURE_MASTER
	num_ints = gicd_read_typer(g_gicd_base) & IT_LINES_NO_MASK;
	num_ints = (num_ints + 1) << 5;
	for (index = MIN_SPI_ID; index < num_ints; index += 32)
		gicd_write_igroupr(g_gicd_base, index, ~0);

	/* Setup SPI priorities doing four at a time */
	for (index = MIN_SPI_ID; index < num_ints; index += 4) {
		gicd_write_ipriorityr(g_gicd_base, index,
				GICD_IPRIORITYR_DEF_VAL);
	}
#else
	(void)num_ints;
#endif
	/* Read the target CPU mask */
	target_cpu = arm_gic_get_cpuif_id();

	/* Configure SPI secure interrupts now */
	assert(g_irq_sec_ptr);
	for (index = 0; index < g_num_irqs; index++) {
		irq_num = g_irq_sec_ptr[index];
		if (irq_num >= MIN_SPI_ID) {
			/* We have an SPI */
			gicd_clr_igroupr(g_gicd_base, irq_num);
			gicd_set_ipriorityr(g_gicd_base, irq_num,
				GIC_HIGHEST_SEC_PRIORITY);
			gicd_set_itargetsr(g_gicd_base, irq_num, target_cpu);
			gicd_set_isenabler(g_gicd_base, irq_num);
		}
	}

	/*
	 * Configure the SGI and PPI. This is done in a separated function
	 * because each CPU is responsible for initializing its own private
	 * interrupts.
	 */
	arm_gic_pcpu_distif_setup();

	gicd_write_ctlr(g_gicd_base, ctlr | ENABLE_GRP0);
}

/******************************************************************************
 * Initialize the ARM GIC driver with the provided platform inputs
 *****************************************************************************/
void arm_gic_init(uintptr_t gicc_base,
		  uintptr_t gicd_base,
		  const unsigned int *irq_sec_ptr,
		  unsigned int num_irqs)
{
	assert(gicc_base);
	assert(gicd_base);
	assert(irq_sec_ptr);

	g_gicc_base = gicc_base;
	g_gicd_base = gicd_base;

	g_irq_sec_ptr = irq_sec_ptr;
	g_num_irqs = num_irqs;
}

/******************************************************************************
 * Setup the ARM GIC CPU and distributor interfaces.
 *****************************************************************************/
void arm_gic_setup(void)
{
	arm_gic_cpuif_setup();
	arm_gic_distif_setup();
}

#if ARM_GIC_ARCH == 2
/*******************************************************************************
 * This functions reads the GIC cpu interface Interrupt Acknowledge register
 * to start handling the pending interrupt. It returns the contents of the IAR.
 ******************************************************************************/
uint32_t arm_gic_acknowledge_interrupt(void)
{
	assert(g_gicc_base);
	return gicc_read_IAR(g_gicc_base);
}

/*******************************************************************************
 * This functions writes the GIC cpu interface End Of Interrupt register with
 * the passed value to finish handling the active interrupt
 ******************************************************************************/
void arm_gic_end_of_interrupt(uint32_t id)
{
	assert(g_gicc_base);
	gicc_write_EOIR(g_gicc_base, id);
}

#else
#error "Invalid ARM GIC architecture version specified for platform port"
#endif /* ARM_GIC_ARCH */
