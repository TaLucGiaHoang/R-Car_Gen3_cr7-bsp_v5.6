/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#ifndef DMA_DRIVER_H__
#define DMA_DRIVER_H__

#include "stdint.h"
#include "ecc_init.h"

void initDMA(void);
void execDMA(uint32_t dst, uint32_t src, uint32_t len);
void exec_fillDMA(uint64_t dst, uint32_t len);

#if RCAR_ECC_ENABLE == SINGLE_ECC_OPT_ENABLE
void exec_fill_sram(uint64_t saddr, uint32_t size);
#endif /* RCAR_ECC_ENABLE == SINGLE_ECC_OPT_ENABLE */

#endif	/* DMA_DRIVER_H__ */
