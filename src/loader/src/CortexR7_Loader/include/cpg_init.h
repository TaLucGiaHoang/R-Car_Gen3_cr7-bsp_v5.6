/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#ifndef	CPG_INIT_H__
#define	CPG_INIT_H__

void cpg_init(void);
void mstpcr_write(uint32_t mstpcr, uint32_t mstpsr, uint32_t target_bit);

#endif	/* CPG_INIT_H__ */
