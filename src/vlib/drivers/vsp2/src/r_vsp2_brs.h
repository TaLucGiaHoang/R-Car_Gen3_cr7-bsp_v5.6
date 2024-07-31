/*************************************************************************************************************
* vsp2_brs_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_VSP2_BRS_H_
#define R_VSP2_BRS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r_vsp2_api.h"

// 32.2.19.5 BRS Control Registers (VI6_BRSm_CTRL: m = A, B)
typedef enum  {
    R_VSP2_BRS_IN_BRSIN0 = 0,
    R_VSP2_BRS_IN_BRSIN1,
    R_VSP2_BRS_IN_VIRTUAL_RPF = 4,
    R_VSP2_BRS_IN_LAST
} r_vsp2_BrsInput_t;

typedef struct  {
    uint32_t operation_type; //0 = ROP, 1 = Blending
    r_vsp2_BrsInput_t dst_side_input;
    r_vsp2_BrsInput_t src_side_input;
    r_vsp2_BruDataOp_t color_data_op;
    r_vsp2_BruDataOp_t alpha_data_op;
} r_vsp2_BrsBlendRopUnit_t;

typedef struct {
    uint32_t virt_rpf_size_w;
    uint32_t virt_rpf_size_h;
    uint32_t virt_rpf_pos_x;
    uint32_t virt_rpf_pos_y;
    uint32_t virt_rpf_color; //ARGB
    r_vsp2_BrsBlendRopUnit_t blend_rop_unit_a;
    r_vsp2_BrsBlendRopUnit_t blend_rop_unit_b;
} r_vsp2_BrsConfig_t;

r_vsp2_Error_t R_VSP2_PRV_BrsInit(r_vsp2_Unit_t Vsp2Unit,
                                  const r_vsp2_BrsConfig_t *Config);

#ifdef __cplusplus
}
#endif

#endif /* R_VSP2_BRS_H_ */
