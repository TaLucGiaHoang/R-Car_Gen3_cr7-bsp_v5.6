/*************************************************************************************************************
* lvds_regs_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_LVDS_REGS_H_
#define R_LVDS_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* LVDS Registers */
#define R_LVDS0_BASE              (0xFEB90000u)
#define R_LVDS1_BASE              (0xFEB90100u)

#define R_LVDS_LVDCR0             (0x0000)
#define R_LVDS_LVDCR1             (0x0004)
#define R_LVDS_LVDPLLCR           (0x0008)
#define R_LVDS_LVDCTRCR           (0x000c)
#define R_LVDS_LVDCHCR            (0x0010)
#define R_LVDS_LVDSTRIPE          (0x0014)
#define R_LVDS_LVDSCR             (0x0018)
#define R_LVDS_LVDDIV             (0x001c)

uint32_t R_LVDS_PRV_RegRead(uintptr_t Address);
void R_LVDS_PRV_RegWrite(uintptr_t Address, uint32_t Value);
uintptr_t R_LVDS_PRV_GetRegBase(uint32_t LvdsUnit);


#ifdef __cplusplus
}
#endif
#endif /* R_LVDS_REGS_H_ */
