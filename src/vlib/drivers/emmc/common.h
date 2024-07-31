#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdint.h>
#include <common/debug.h>

void mstpcr_write(uint32_t mstpcr, uint32_t mstpsr, uint32_t target_bit);

#endif /* _COMMON_H_ */