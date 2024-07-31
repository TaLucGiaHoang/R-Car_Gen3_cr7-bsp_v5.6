/*************************************************************************************************************
* vsp2_isr_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include "r_prr_api.h"
#include "r_vsp2_api.h"
#include "r_vsp2_regs.h"
#include "r_vsp2_dl.h"
#include "r_vsp2_vspd_prv.h"
#include "r_vsp2_vspi_prv.h"
#include "r_print_api.h"

uint32_t R_VSP2_Isr(r_vsp2_Unit_t Vsp2Unit,
                    uint32_t      Wpf0InUse,
                    uint32_t      Wpf1InUse)
{
    uint32_t ret = 0;
    uint32_t reg_val = 0;
    uint32_t vsp2_reg_base;

    if (Vsp2Unit >= R_VSP2_LAST) {
        /* ERROR */
        R_PRINT_Log("[R_VSP2_Isr] : VSP2 Unit No is Invalid. Failed(%d)\r\n", ret);
        return ret;
    }
    vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

    /*WPF0 - valid for all VSP2*/

    if (1U == Wpf0InUse) {
        reg_val = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_WPF0_IRQ_STA);

        if (reg_val & R_VSP2_VI6_WPF_IRQ_STA_UND) {
            /*Note: Insert call to call back if supported */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF0_IRQ_STA, ~R_VSP2_VI6_WPF_IRQ_STA_UND);
            ret = R_VSP_INT_UND_WPF0;
        }
        if (reg_val & R_VSP2_VI6_WPF_IRQ_STA_DFE) {
            /*Note: Insert call to call back if supported */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF0_IRQ_STA, ~R_VSP2_VI6_WPF_IRQ_STA_DFE);
            ret |= R_VSP_INT_DFE_WPF0;
        }
        if (reg_val & R_VSP2_VI6_WPF_IRQ_STA_FRE) {
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF0_IRQ_STA, ~R_VSP2_VI6_WPF_IRQ_STA_FRE);
            if ((R_VSP2_VSPI0 == Vsp2Unit) || (R_VSP2_VSPI1 == Vsp2Unit)) {
                R_VSP2_VSPI_PRV_FrameEndCallback(Vsp2Unit);
            }
            else if (0 == r_vsp2_ModuleDlAddr[Vsp2Unit]) {
                /* Update VSPD registers to reflect layers status */
                R_VSP2_VSPD_PRV_LayersUpdate(Vsp2Unit, 0);
                /* We are not using display lists, need to restart VSPD manually */
                R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_CMD0, R_VSP2_VI6_CMD_STRCMD);
                ret |= R_VSP_INT_FRE_WPF0;
            }
        }
    }

    if (R_VSP2_VSPD0 == Vsp2Unit) {
        if (1U == Wpf1InUse) {
            /*WPF1*/
            reg_val = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_WPF1_IRQ_STA);

            if (reg_val & R_VSP2_VI6_WPF_IRQ_STA_UND) {
                /*Note: insert call to callback isr */
                R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF1_IRQ_STA, ~R_VSP2_VI6_WPF_IRQ_STA_UND);
                ret |= R_VSP_INT_UND_WPF1;
            }
            if (reg_val & R_VSP2_VI6_WPF_IRQ_STA_DFE) {
                /*Note: insert call to callback isr */
                R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF1_IRQ_STA, ~R_VSP2_VI6_WPF_IRQ_STA_DFE);
                ret |= R_VSP_INT_DFE_WPF1;
            }
            if (reg_val & R_VSP2_VI6_WPF_IRQ_STA_FRE) {
                R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF1_IRQ_STA, ~R_VSP2_VI6_WPF_IRQ_STA_FRE);
                if (0 == r_vsp2_ModuleDlAddr[Vsp2Unit]) {
                    /* Update VSPD registers to reflect layers status */
                    R_VSP2_VSPD_PRV_LayersUpdate(Vsp2Unit, 1);
                    /* We are not using display lists, need to restart VSPD manually */
                    R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_CMD1, R_VSP2_VI6_CMD_STRCMD);
                    ret |= R_VSP_INT_FRE_WPF1;
                }
            }
        }
    }

    /* Enable all VSPD IRQ - implies that the BSP disables also all VSPd IRQ when one is received*/
    if (1U == Wpf0InUse) {
        R_VSP2_PRV_RegWrite((vsp2_reg_base + R_VSP2_VI6_WPF0_IRQ_ENB), 0x10003);
    }
    if (R_VSP2_VSPD0 == Vsp2Unit) {
        if (1U == Wpf1InUse) {
            R_VSP2_PRV_RegWrite((vsp2_reg_base + R_VSP2_VI6_WPF1_IRQ_ENB), 0x10003);
        }
    }
    return ret;
}
