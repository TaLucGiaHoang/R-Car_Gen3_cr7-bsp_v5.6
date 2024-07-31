/*************************************************************************************************************
* vsp2_bru_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include "r_vsp2_bru.h"
#include "r_vsp2_regs.h"
#include "r_print_api.h"

r_vsp2_Error_t R_VSP2_BlendingEnable(r_vsp2_Unit_t Vsp2Unit, uint32_t Rpf,
                                     uint32_t Enable)
{
    r_vsp2_Error_t err = R_VSP2_ERR_SUCCESS;
    uint32_t reg_bld = 0;
    uint32_t reg_ctl = 0;
    uint32_t reg_val = 0;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

    switch (Rpf) {
    case 0:
        reg_bld = R_VSP2_VI6_BRUE_BLD;
        reg_ctl = R_VSP2_VI6_BRUE_CTRL;
        break;
    case 1:
        reg_bld = R_VSP2_VI6_BRUD_BLD;
        reg_ctl = R_VSP2_VI6_BRUD_CTRL;
        break;
    case 2:
        reg_bld = R_VSP2_VI6_BRUC_BLD;
        reg_ctl = R_VSP2_VI6_BRUC_CTRL;
        break;
    case 3:
        reg_bld = R_VSP2_VI6_BRUB_BLD;
        reg_ctl = R_VSP2_VI6_BRUB_CTRL;
        break;
    case 4:
        reg_bld = R_VSP2_VI6_BRUA_BLD;
        reg_ctl = R_VSP2_VI6_BRUA_CTRL;
        break;
    case 5:
        /* Virtual RPF no operation per default */
        break;
    default:
        err = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_BlendingEnable] : Rpf No is Invalid. Failed(%d)\r\n", err);
        break;
    }

    if (0 != reg_bld) {
        if (1 == Enable) {
            reg_val = 0;            // Fixed a value 1 = 0
            reg_val |= (0xFF) << 8;   // Fixed a value 0 = 255
            reg_val |= (0x4) << 16;   // Fixed a value 1
            reg_val |= (0x4) << 20;   // Fixed a value 0
            reg_val |= (0x2) << 24;   // src_a * src_c
            reg_val |= (0x3) << 28;   // (1-src_a) * dst_c
            R_VSP2_PRV_RegWrite(vsp2_reg_base + reg_bld, reg_val);

            reg_val = 1 << 31;
            reg_val |= R_VSP2_PRV_RegRead(vsp2_reg_base + reg_ctl);
            R_VSP2_PRV_RegWrite(vsp2_reg_base + reg_ctl, reg_val);
        } else {
            /* no operation*/
        }
    }

    return err;
}

r_vsp2_Error_t R_VSP2_PRV_BruInit(r_vsp2_Unit_t Vsp2Unit,
                                  const r_vsp2_BruConfig_t *Config)
{
    r_vsp2_Error_t e = R_VSP2_ERR_SUCCESS;
    uint32_t reg_val = 0;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

    if ((Vsp2Unit == R_VSP2_VSPI0) || (Vsp2Unit == R_VSP2_VSPI1) ||
        (Vsp2Unit == R_VSP2_VSPBS)) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_PRV_BruInit] : VSP2 Unit No is Invalid. Failed(%d)\r\n", e);
    } else {
        if (0 == Config) {
            e = R_VSP2_ERR_INVALID_PARAMETER;
            R_PRINT_Log("[R_VSP2_PRV_BruInit] : VSP2 BRU configuration information is NULL. Failed(%d)\r\n", e);
        }
    }

    if (R_VSP2_ERR_SUCCESS == e) {
        /* Virtual RPF size */
        reg_val = (((*Config).virt_rpf_size_w & 0x1fff) << 16) + (((*Config).virt_rpf_size_h & 0x1fff));
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRU_VIRRPF_SIZE, reg_val);

        /* Virtual RPF pos */
        reg_val = (((*Config).virt_rpf_pos_x & 0x1fff) << 16) + (((*Config).virt_rpf_pos_y & 0x1fff));
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRU_VIRRPF_LOC, reg_val);

        /* Virtual RPF color */
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRU_VIRRPF_COL, (*Config).virt_rpf_color);

        /* Config Blend/ROP unit A */
        reg_val = (*Config).blend_rop_unit_a.operation_type ? (1 << 31) : 0;
        reg_val |= (*Config).blend_rop_unit_a.dst_side_input << 20;
        reg_val |= (*Config).blend_rop_unit_a.src_side_input << 16;
        reg_val |= (*Config).blend_rop_unit_a.color_data_op << 4;
        reg_val |= (*Config).blend_rop_unit_a.alpha_data_op << 0;
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRUA_CTRL, reg_val);

        /* Config Blend/ROP unit B */
        reg_val = (*Config).blend_rop_unit_b.operation_type ? (1 << 31) : 0;
        reg_val |= (*Config).blend_rop_unit_b.color_data_op << 4;
        reg_val |= (*Config).blend_rop_unit_b.alpha_data_op << 0;
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRUB_CTRL, reg_val);

        /* Config Blend/ROP unit C */
        reg_val = (*Config).blend_rop_unit_c.operation_type ? (1 << 31) : 0;
        reg_val |= (*Config).blend_rop_unit_c.src_side_input << 16;
        reg_val |= (*Config).blend_rop_unit_c.color_data_op << 4;
        reg_val |= (*Config).blend_rop_unit_c.alpha_data_op << 0;
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRUC_CTRL, reg_val);

        /* Config Blend/ROP unit D */
        reg_val = (*Config).blend_rop_unit_d.operation_type ? (1 << 31) : 0;
        reg_val |= (*Config).blend_rop_unit_d.src_side_input << 16;
        reg_val |= (*Config).blend_rop_unit_d.color_data_op << 4;
        reg_val |= (*Config).blend_rop_unit_d.alpha_data_op << 0;
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRUD_CTRL, reg_val);

        /* Config Blend/ROP unit E */
        reg_val = (*Config).blend_rop_unit_e.operation_type ? (1 << 31) : 0;
        reg_val |= (*Config).blend_rop_unit_e.src_side_input << 16;
        reg_val |= (*Config).blend_rop_unit_e.color_data_op << 4;
        reg_val |= (*Config).blend_rop_unit_e.alpha_data_op << 0;
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRUE_CTRL, reg_val);

        /* Config ROP unit */
        reg_val = (*Config).rop_unit.dst_side_input << 20;
        reg_val |= (*Config).rop_unit.color_data_op << 4;
        reg_val |= (*Config).rop_unit.alpha_data_op << 0;
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRU_ROP, reg_val);

    }
    return e;
}

r_vsp2_Error_t R_VSP2_BkgColorSet(r_vsp2_Unit_t Vsp2Unit,
                                  uint32_t Color)
{
    r_vsp2_Error_t e = R_VSP2_ERR_SUCCESS;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

    if ((Vsp2Unit == R_VSP2_VSPI0) || (Vsp2Unit == R_VSP2_VSPI1) ||
        (Vsp2Unit == R_VSP2_VSPBS)) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_BkgColorSet] : VSP2 Unit No is Invalid. Failed(%d)\r\n", e);
    }

    if (R_VSP2_ERR_SUCCESS == e) {
        /* Virtual RPF color */
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRU_VIRRPF_COL, Color);

    }
    return e;
}

