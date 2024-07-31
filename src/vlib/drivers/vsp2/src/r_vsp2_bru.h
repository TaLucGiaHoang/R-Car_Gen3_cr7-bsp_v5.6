/*************************************************************************************************************
* vsp2_bru_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_VSP2_BRU_H_
#define R_VSP2_BRU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r_vsp2_api.h"

typedef enum  {
    R_VSP2_BRU_IN_BRUIN0 = 0,
    R_VSP2_BRU_IN_BRUIN1,
    R_VSP2_BRU_IN_BRUIN2,
    R_VSP2_BRU_IN_BRUIN3,
    R_VSP2_BRU_IN_VIRTUAL_RPF,
    R_VSP2_BRU_IN_BRUIN4,
    R_VSP2_BRU_IN_LAST
} r_vsp2_BruInput_t;

typedef enum  {
    R_VSP2_BRU_OP_NOP = 0,      // NOP(D)
    R_VSP2_BRU_OP_AND,          // AND(S & D)
    R_VSP2_BRU_OP_AND_REV,      // AND_REVERSE(S & ~D)
    R_VSP2_BRU_OP_COPY,         // COPY(S)
    R_VSP2_BRU_OP_AND_INV,      // AND_INVERTED(~S & D)
    R_VSP2_BRU_OP_CLEAR,        // CLEAR(0)
    R_VSP2_BRU_OP_XOR,          // XOR(S ^ D)
    R_VSP2_BRU_OP_OR,           // OR(S | D)
    R_VSP2_BRU_OP_NOR,          // NOR(~(S | D)
    R_VSP2_BRU_OP_EQUIV,        // EQUIV(~(S ^ D))
    R_VSP2_BRU_OP_INVERT,       // INVERT(~D)
    R_VSP2_BRU_OP_OR_REV,       // OR_REVERSE(S | ~D)
    R_VSP2_BRU_OP_COPY_INV,     // COPY_INVERTED(~S)
    R_VSP2_BRU_OP_OR_INV,       // OR_INVERTED(~S | D)
    R_VSP2_BRU_OP_NAND,         // NAND(~(S & D))
    R_VSP2_BRU_OP_SET,          // SET(all1)
    R_VSP2_BRU_OP_LAST
} r_vsp2_BruDataOp_t;

typedef struct  {
    uint32_t operation_type; //0 = ROP, 1 = Blending
    r_vsp2_BruInput_t dst_side_input;
    r_vsp2_BruInput_t src_side_input;
    r_vsp2_BruDataOp_t color_data_op;
    r_vsp2_BruDataOp_t alpha_data_op;
} r_vsp2_BruBlendRopUnit_t;

typedef struct  {
    r_vsp2_BruInput_t dst_side_input;
    r_vsp2_BruDataOp_t color_data_op;
    r_vsp2_BruDataOp_t alpha_data_op;
} r_vsp2_BruRopUnit_t;

typedef struct {
    uint32_t virt_rpf_size_w;
    uint32_t virt_rpf_size_h;
    uint32_t virt_rpf_pos_x;
    uint32_t virt_rpf_pos_y;
    uint32_t virt_rpf_color; //ARGB
    r_vsp2_BruBlendRopUnit_t blend_rop_unit_a;
    r_vsp2_BruBlendRopUnit_t blend_rop_unit_b;
    r_vsp2_BruBlendRopUnit_t blend_rop_unit_c;
    r_vsp2_BruBlendRopUnit_t blend_rop_unit_d;
    r_vsp2_BruBlendRopUnit_t blend_rop_unit_e;
    r_vsp2_BruRopUnit_t rop_unit;
} r_vsp2_BruConfig_t;

r_vsp2_Error_t R_VSP2_PRV_BruInit(r_vsp2_Unit_t Vsp2Unit,
                                  const r_vsp2_BruConfig_t *Config);

#ifdef __cplusplus
}
#endif

#endif /* R_VSP2_BRU_H_ */
