/*************************************************************************************************************
* vsp2_brs_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "r_vsp2_bru.h"
#include "r_vsp2_brs.h"
#include "r_vsp2_regs.h"
#include "r_vsp2_api.h"
#include "r_print_api.h"

r_vsp2_Error_t R_VSP2_BlendingEnable_Brs(r_vsp2_Unit_t Vsp2Unit, r_vsp2_Dpr_Route_t Dpr_Rpfn_Route)
{
    r_vsp2_Error_t err = R_VSP2_ERR_SUCCESS;
    uint32_t reg_bld = 0U;
    uint32_t reg_ctl = 0U;
    uint32_t reg_val = 0U;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

    if ((R_VSP2_VSPBS != Vsp2Unit) &&
        (R_VSP2_VSPD0 != Vsp2Unit) &&
        (R_VSP2_VSPDL != Vsp2Unit)) {
        err = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_BlendingEnable_Brs] : VSP2 Unit No is Invalid. Failed(%d)\r\n", err);
    }

    switch (Dpr_Rpfn_Route) {
    case R_VSP_DPR_ROUTE_BRS_ILV0:
        reg_bld = R_VSP2_VI6_BRSB_BLD;
        reg_ctl = R_VSP2_VI6_BRSB_CTRL;
        break;
    case R_VSP_DPR_ROUTE_BRS_ILV1:
        reg_bld = R_VSP2_VI6_BRSA_BLD;
        reg_ctl = R_VSP2_VI6_BRSA_CTRL;
        break;
    default:
        err = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_BlendingEnable_Brs] : DPR Route Config Setting Value is Invalid. Failed(%d)\r\n", err);
        break;
    }

    if (R_VSP2_ERR_SUCCESS == err){
        reg_val = 0;            // Fixed a value 1 = 0
        reg_val |= (0xFF) << 8;   // Fixed a value 0 = 255
        reg_val |= (0x4) << 16;   // Fixed a value 1
        reg_val |= (0x4) << 20;   // Fixed a value 0
        reg_val |= (0x2) << 24;   // src_a * src_c
        reg_val |= (0x3) << 28;   // (1-src_a) * dst_c

        R_VSP2_PRV_RegWrite(vsp2_reg_base + reg_bld, reg_val);
        reg_val = 1U << 31;
        reg_val |= R_VSP2_PRV_RegRead(vsp2_reg_base + reg_ctl);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + reg_ctl, reg_val);
    }

    return err;
}

r_vsp2_Error_t R_VSP2_PRV_BrsInit(r_vsp2_Unit_t Vsp2Unit, const r_vsp2_BrsConfig_t *Config)
{
    r_vsp2_Error_t e = R_VSP2_ERR_SUCCESS;
    uint32_t reg_val = 0U;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

    if ((R_VSP2_VSPBS != Vsp2Unit) &&
        (R_VSP2_VSPD0 != Vsp2Unit) &&
        (R_VSP2_VSPDL != Vsp2Unit)) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_PRV_BrsInit] : VSP2 Unit No is Invalid. Failed(%d)\r\n", e);
    } else {
        if (NULL == Config) {
            e = R_VSP2_ERR_INVALID_PARAMETER;
            R_PRINT_Log("[R_VSP2_PRV_BrsInit] : VSP2 BRS configuration information is NULL. Failed(%d)\r\n", e);
        }
    }

    if (R_VSP2_ERR_SUCCESS == e) {
        /* Virtual RPF size */
        reg_val = (((*Config).virt_rpf_size_w & 0x1fff) << 16) + (((*Config).virt_rpf_size_h & 0x1fff));
        if (0U != reg_val) {
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRS_VIRRPF_SIZE, reg_val);
        }

        /* Virtual RPF pos */
        reg_val = (((*Config).virt_rpf_pos_x & 0x1fff) << 16) + (((*Config).virt_rpf_pos_y & 0x1fff));
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRS_VIRRPF_LOC, reg_val);

        /* Virtual RPF color */
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRS_VIRRPF_COL, (*Config).virt_rpf_color);

        /* Config Blend/ROP unit A */
        reg_val = (*Config).blend_rop_unit_a.operation_type ? (1U << 31) : 0U;
        reg_val |= (*Config).blend_rop_unit_a.dst_side_input << 20;
        reg_val |= (*Config).blend_rop_unit_a.src_side_input << 16;
        reg_val |= (*Config).blend_rop_unit_a.color_data_op << 4;
        reg_val |= (*Config).blend_rop_unit_a.alpha_data_op << 0;
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRSA_CTRL, reg_val);

        /* Config Blend/ROP unit B */
        reg_val = (*Config).blend_rop_unit_b.operation_type ? (1U << 31) : 0U;
        reg_val |= (*Config).blend_rop_unit_b.src_side_input << 16;
        reg_val |= (*Config).blend_rop_unit_b.color_data_op << 4;
        reg_val |= (*Config).blend_rop_unit_b.alpha_data_op << 0;
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRSB_CTRL, reg_val);
    }
    return e;
}

r_vsp2_Error_t R_VSP2_BkgColorSet_Brs(r_vsp2_Unit_t Vsp2Unit, uint32_t Color)
{
    r_vsp2_Error_t e = R_VSP2_ERR_SUCCESS;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

    if ((R_VSP2_VSPBS != Vsp2Unit) &&
        (R_VSP2_VSPD0 != Vsp2Unit) &&
        (R_VSP2_VSPDL != Vsp2Unit)) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_BkgColorSet_Brs] : VSP2 Unit No is Invalid. Failed(%d)\r\n", e);
    }

    if (R_VSP2_ERR_SUCCESS == e) {
        /* Virtual RPF color */
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_BRS_VIRRPF_COL, Color);

    }
    return e;
}
