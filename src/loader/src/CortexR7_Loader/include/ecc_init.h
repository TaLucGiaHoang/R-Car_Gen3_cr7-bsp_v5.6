/*
 * Copyright (c) 2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#ifndef ECC_INIT_H__
#define ECC_INIT_H__

/* compile option definitions */
#define DRAM_SRAM_ECC_DISABLE	(0U)
#define SINGLE_ECC_OPT_ENABLE	(1U)
#define DUAL_ECC_OPT_ENABLE	(2U)

void ecc_init(void);


#endif /* ECC_INIT_H__ */
