/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#ifndef BL2_SWDT_H__
#define BL2_SWDT_H__

void swdt_init(void);

void swdt_enable(void);

void swdt_release(void);

void swdt_exec(uint32_t occ_add);

#endif /* BL2_SWDT_H__ */
