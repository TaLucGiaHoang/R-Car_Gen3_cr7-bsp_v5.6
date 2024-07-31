/*************************************************************************************************************
* vsp2_lif_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include "r_vsp2_lif.h"
#include "r_vsp2_regs.h"
#include "r_print_api.h"

r_vsp2_Error_t R_VSP2_PRV_LifEnable(r_vsp2_Unit_t Vsp2Unit, uint32_t LifId,
                                    uint32_t Enable)
{
    r_vsp2_Error_t e = R_VSP2_ERR_SUCCESS;
    uint32_t reg_val = 0;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

    if ((Vsp2Unit != R_VSP2_VSPD0) &&
        (Vsp2Unit != R_VSP2_VSPD1) &&
        (Vsp2Unit != R_VSP2_VSPD2) &&
        (Vsp2Unit != R_VSP2_VSPDL)) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_PRV_LifEnable] : VSP2 Unit No is Invalid. Failed(%d)\r\n", e);
    } else {

        /* Enable/disable LIF (REQSEL[1] and LIF_EN[0] bits in VSP2_VI6_LIFn_CTRL)*/
        switch (LifId) {
        case 0:
            reg_val = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_LIF0_CTRL);
            reg_val |= Enable ? (3 << 0) : 0;
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_LIF0_CTRL, reg_val);
            break;
        case 1:
            reg_val = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_LIF1_CTRL);
            reg_val |= Enable ? (3 << 0) : 0;
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_LIF1_CTRL, reg_val);
            break;
        default:
            e = R_VSP2_ERR_INVALID_PARAMETER;
            R_PRINT_Log("[R_VSP2_PRV_LifEnable] : LifId is Invalid. Failed(%d)\r\n", e);
            break;
        }
    }

    return e;
}
