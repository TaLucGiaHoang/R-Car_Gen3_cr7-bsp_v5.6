/*
 * Copyright (c) 2018, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <mmio.h>
#include "debug.h"
#include "micro_wait.h"

#define GLOBAL_TIMER_COUNT_L		(0xF0000200U)
#define GLOBAL_TIMER_COUNT_U		(0xF0000204U)
#define GLOBAL_TIMER_CONTROL_REG	(0xF0000208U)
#define GLOBAL_TIMER_EN			(0x1U)
/* In case of Prescaler is 0, Global Timer is 200 MHz. */
#define GLOBAL_TIMER_CLOCK		(200U)

#define UPPER_SHIFT			(32U)

void micro_wait_init(void)
{
	mmio_write_32(GLOBAL_TIMER_CONTROL_REG, GLOBAL_TIMER_EN);
}

void micro_wait(uint64_t count_us)
{
	uint64_t count_init;
	uint64_t count = 0;
	uint64_t loop_num;

	count_init = mmio_read_32(GLOBAL_TIMER_COUNT_U);
	count_init = count_init << UPPER_SHIFT;
	count_init += mmio_read_32(GLOBAL_TIMER_COUNT_L);
	loop_num = count_us * GLOBAL_TIMER_CLOCK;

	while( loop_num > count){
		count = mmio_read_32(GLOBAL_TIMER_COUNT_U);
		count = count << UPPER_SHIFT;
		count += mmio_read_32(GLOBAL_TIMER_COUNT_L);
		count -= count_init;
	}
}

