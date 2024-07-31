/*
 * Copyright (c) 2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#ifndef DRAM_DUAL_ECC_H__
#define DRAM_DUAL_ECC_H__

#include "ecc_init.h"

#if RCAR_ECC_ENABLE == DUAL_ECC_OPT_ENABLE
void dram_dual_ecc_init(void);
#endif

#endif /* DRAM_DUAL_ECC_H__ */
