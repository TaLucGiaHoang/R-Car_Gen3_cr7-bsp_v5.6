/*************************************************************************************************************
* vsp2_dl_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_VSP2_DL_H_
#define R_VSP2_DL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "r_vsp2_regs.h"

/**
 * This array keep tracks on the VSP2 module Display list address
 *          if 0  then no DL is used
 */
extern uintptr_t r_vsp2_ModuleDlAddr[R_VSP2_LAST];

typedef struct __attribute__((packed)) {
    uint32_t num_bytes;
    uintptr_t list_addr;
} r_vsp2_DlPlists_t;

typedef struct __attribute__((packed)) {
    uint32_t num_list_minus1;
    r_vsp2_DlPlists_t lists[8];
    uintptr_t next_header_addr;
    uint32_t frame_int_en_next_auto; //includes bits "current_frame_int_enable" and "next_frame_auto_start"
} r_vsp2_DlHeader_t;

r_vsp2_Error_t R_VSP2_PRV_DlInit(r_vsp2_Unit_t Vsp2Unit, uint32_t WpfId);

#ifdef __cplusplus
}
#endif

#endif /* R_VSP2_DL_H_ */
