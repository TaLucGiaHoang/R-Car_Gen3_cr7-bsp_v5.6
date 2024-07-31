/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include <stdint.h>
#include "dram_sub_func.h"

void dram_get_boot_status(uint32_t *status)
{
	*status = DRAM_BOOT_STATUS_COLD;
}

int32_t dram_update_boot_status(uint32_t status)
{
	int32_t ret = 0;
	return ret;
}
