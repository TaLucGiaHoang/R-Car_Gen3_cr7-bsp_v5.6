/*
 * Copyright (c) 2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "stdint.h"
#include "mmio.h"
#include "debug.h"
#include "micro_wait.h"

/* In case of Prescaler is 0, Global Timer operates at 200 MHz. */
#define GLOBAL_TIMER_CLOCK		(200U)

#define GLOBAL_TIMER_COUNT_L		(0xF0000200U)

/* count_us can be set up to d'10000000 */
void micro_wait(uint32_t count_us)
{
	uint32_t count_init;
	uint32_t count = 0;
	uint32_t loop_num;

	count_init = mmio_read_32(GLOBAL_TIMER_COUNT_L);
	loop_num = count_us * GLOBAL_TIMER_CLOCK;

	while (loop_num > count) {
		count = mmio_read_32(GLOBAL_TIMER_COUNT_L);
		count -= count_init;
	}
}

uint32_t get_timer_count(void)
{
	/* Count value decrease in 200MHz */
	return mmio_read_32(GLOBAL_TIMER_COUNT_L);
}

