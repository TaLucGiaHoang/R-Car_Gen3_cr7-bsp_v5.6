/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#ifndef ABT_ERROR_H__
#define ABT_ERROR_H__

void undef_error(uint32_t occ_add);
void fiq_error(void);
void interrupt_error_id(uint32_t int_id, uint32_t occ_add);
void dabort_error(uint32_t occ_add, uint32_t dfsr, uint32_t dfar);
void pabort_error(uint32_t ifsr, uint32_t ifar);

#endif /* ABT_ERROR_H__ */
