/*
 * Copyright (c) 2014, ARM Limited and Contributors. All rights reserved.
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __ARM_GIC_H__
#define __ARM_GIC_H__

#include <stdint.h>

/*******************************************************************************
 * Function declarations
 ******************************************************************************/
void arm_gic_init(uintptr_t gicc_base,
		  uintptr_t gicd_base,
		  const unsigned int *irq_sec_ptr,
		  unsigned int num_irqs);
void arm_gic_setup(void);
void arm_gic_cpuif_deactivate(void);
void arm_gic_cpuif_setup(void);
void arm_gic_pcpu_distif_setup(void);


uint32_t arm_gic_acknowledge_interrupt(void);
void arm_gic_end_of_interrupt(uint32_t id);

#endif /* __GIC_H__ */
