/*************************************************************************************************************
* vsp2_api_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "r_prr_api.h"
#include "r_vsp2_api.h"
#include "r_vsp2_regs.h"
#include "r_vsp2_dpr.h"
#include "r_vsp2_rpf.h"
#include "r_vsp2_wpf.h"
#include "r_vsp2_lif.h"
#include "r_vsp2_bru.h"
#include "r_vsp2_dl.h"
#include "r_vsp2_brs.h"
#include "r_print_api.h"

/* Default wpf config for all VSPD */
static r_vsp2_Wpf_t loc_WpfVspd = {
    .source_rpf = R_VSP2_VI6_WPFn_SRCRPF_VIR_ACT_MASTER,
    .clip_h_enable = 0,
    .clip_h_size = 0,
    .clip_h_offset = 0,
    .clip_v_enable = 0,
    .clip_v_size = 0,
    .clip_v_offset = 0,
    .mem_addr_y_rgb = 0,
    .mem_addr_c0 = 0,
    .mem_addr_c1 = 0,
    .mem_stride_y_rgb = 0,
    .mem_stride_c = 0,
    .format = R_VSP2_VI6_WPFn_OUTFMT_WRFMT_ARGB8888,
    .writeback_enable = 0,
    .yc_swap = 0,
    .uv_swap = 0,
    .c_conversion = 0,
    .pxa = 0,
    .data_swap = 0
};

/*  BRUIN2 (guest os - set as destination, source overwritten)    -> ROP
    BRUIN0 ("CR7 layer" -set as destinatinon, source overwritten) -> UNIT_A
    ROP & UNIT_A output blended by UNIT_A -> and the output is output
    UNIT_C,D,E -> do nothing, destination data is used
 */

/* Default BRU config for all VSPD */
static r_vsp2_BruConfig_t loc_BruVspd = {
    .virt_rpf_size_w = 0,
    .virt_rpf_size_h = 0,
    .virt_rpf_pos_x = 0,
    .virt_rpf_pos_y = 0,
    .virt_rpf_color = 0xffffff00,
    .blend_rop_unit_a = {
        .operation_type = 0,
        .dst_side_input = R_VSP2_BRU_IN_VIRTUAL_RPF,
        .src_side_input = R_VSP2_BRU_IN_BRUIN4,
        .color_data_op = R_VSP2_BRU_OP_NOP,
        .alpha_data_op = R_VSP2_BRU_OP_NOP
    },
    .blend_rop_unit_b = {
        .operation_type = 0,
        .color_data_op = R_VSP2_BRU_OP_NOP,
        .alpha_data_op = R_VSP2_BRU_OP_NOP
    },
    .blend_rop_unit_c = {
        .operation_type = 0,
        .src_side_input = R_VSP2_BRU_IN_BRUIN2,
        .color_data_op = R_VSP2_BRU_OP_NOP,
        .alpha_data_op = R_VSP2_BRU_OP_NOP
    },
    .blend_rop_unit_d = {
        .operation_type = 0,
        .src_side_input = R_VSP2_BRU_IN_BRUIN1,
        .color_data_op = R_VSP2_BRU_OP_NOP,
        .alpha_data_op = R_VSP2_BRU_OP_NOP
    },
    .blend_rop_unit_e = {
        .operation_type = 0,
        .src_side_input = R_VSP2_BRU_IN_BRUIN0,
        .color_data_op = R_VSP2_BRU_OP_NOP,
        .alpha_data_op = R_VSP2_BRU_OP_NOP,
    },
    .rop_unit = {
        .dst_side_input = R_VSP2_BRU_IN_BRUIN3,
        .color_data_op = R_VSP2_BRU_OP_NOP,
        .alpha_data_op = R_VSP2_BRU_OP_NOP
    }
};

/* Default BRS config for VSPD0 */
static r_vsp2_BrsConfig_t loc_BrsVspd = {
    .virt_rpf_size_w = 0,
    .virt_rpf_size_h = 0,
    .virt_rpf_pos_x = 0,
    .virt_rpf_pos_y = 0,
    .virt_rpf_color = 0xffffff00,
    .blend_rop_unit_a = {
        .operation_type = 0,
        .dst_side_input = R_VSP2_BRS_IN_VIRTUAL_RPF,
        .src_side_input = R_VSP2_BRS_IN_BRSIN1,
        .color_data_op = R_VSP2_BRU_OP_NOP,
        .alpha_data_op = R_VSP2_BRU_OP_NOP
    },
    .blend_rop_unit_b = {
        .operation_type = 0,
        .src_side_input = R_VSP2_BRS_IN_BRSIN0,
        .color_data_op = R_VSP2_BRU_OP_NOP,
        .alpha_data_op = R_VSP2_BRU_OP_NOP
    }
};

static r_vsp2_Error_t loc_VspiInit(r_vsp2_Unit_t  Vsp2Unit,
                                   uint32_t       WpfId,
                                   r_vsp2_Dpr_Conf_t *Dpr_Conf,
                                   uint32_t       InitFlag)
{
    r_vsp2_Error_t e       = R_VSP2_ERR_SUCCESS;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);
    r_prr_DevName_t dev    = R_PRR_GetDevice();

    if (NULL == Dpr_Conf) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[loc_VspdInit] : VSP2 DPR configuration information is NULL. Failed(%d)\r\n", e);
    }

    if (R_VSP2_ERR_SUCCESS == e) {
        if ((R_PRR_RCARE3 == dev) || (R_PRR_RCARM3N == dev)) {
            if (R_VSP2_VSPI0 != Vsp2Unit) {
                e = R_VSP2_ERR_INVALID_PARAMETER;
            }
        }
        else {
            if ((R_VSP2_VSPI0 != Vsp2Unit) && (R_VSP2_VSPI1 != Vsp2Unit)) {
                e = R_VSP2_ERR_INVALID_PARAMETER;
            }
        }
    }

    if (R_VSP2_ERR_SUCCESS == e) {
        if (R_VSP2_WPF0 != WpfId) {
            e = R_VSP2_ERR_INVALID_PARAMETER;
        }
    }

    if (R_VSP2_ERR_SUCCESS == e) {
        /* Disable interrupts */
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF0_IRQ_ENB, 0);
        /* Clear Interrupts */
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF0_IRQ_STA, 0);
        /* [7] Always set H'0000 0500 to VI6_DPR_WPFn_FPORCH (n=0,1) in case
            * of using WPFn. */
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_WPF0_FPORCH, 0x00000500);

        e = R_VSP2_PRV_DprInit(Vsp2Unit, Dpr_Conf);
        if ( R_VSP2_ERR_SUCCESS != e ) {
            R_PRINT_Log("[loc_VspdInit] : Initialize Data Path Router (DPR). Failed(%d)\r\n", e);
        }
    }

    return e;
}

/* Configure the VSPD unit according to the device used */
static r_vsp2_Error_t loc_VspdInit(r_vsp2_Unit_t  Vsp2Unit,
                                   uint32_t       WpfId,
                                   uint32_t       ScreenWidth,
                                   uint32_t       ScreenHeight,
                                   r_vsp2_Dpr_Conf_t *Dpr_Conf,
                                   uint32_t       InitFlag)
{
    r_vsp2_Error_t e       = R_VSP2_ERR_SUCCESS;
    uint32_t reg_val       = 0u;
    uint32_t LifId         = R_VSP2_LIF0;
    uint32_t LifSettng     = R_VSP2_VI6_LIF_CTRL_OBTH_VSPD;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);
    r_prr_DevName_t dev    = R_PRR_GetDevice();
    uint32_t select_brs    = 0u;

    if (NULL == Dpr_Conf) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[loc_VspdInit] : VSP2 DPR configuration information is NULL. Failed(%d)\r\n", e);
    }

    if (R_VSP2_ERR_SUCCESS == e) {
        if ((0 == ScreenWidth) || (8190 < ScreenWidth) || (0 == ScreenHeight) || (8190 < ScreenHeight)) {
            e = R_VSP2_ERR_INVALID_PARAMETER;
            R_PRINT_Log("[R_VSP2_Init] :  ScreenWidth(%d), ScreenHeight(%d) range Failed(%d)\r\n", ScreenWidth, ScreenHeight, e);
        }
    }

    /* [8] For VSPD, always set D'3000 to VI6_LIF0_CTRL.OBTH[11:0] for
     * R-Car H3, R-Car M3-W, R-Car D3, R-Car M3-N or R-Car E3 in case of
     * using LIF0 (VI6_LIF0_CTRL.LIF_EN = 1). Always set D'1500 to
     * VI6_LIF0_CTRL.OBTH [11:0] for R-Car V3M or R-Car V3H in case of
     * using LIF (VI6_LIF0_CTRL.LIF_EN = 1).  For VSPDL, always set
     * D'1500 to VI6_LIFn_CTRL.OBTH[11:0] for R-Car H3 or R-Car M3-N in
     * case of using LIFn (VI6_LIFn_CTRL.LIF_EN = 1). */
    if (R_VSP2_ERR_SUCCESS == e) {
        if (R_VSP2_VSPD0 == Vsp2Unit) {
            if ((R_PRR_RCARH3 == dev) || (R_PRR_RCARM3N == dev)) {
                LifSettng = R_VSP2_VI6_LIF_CTRL_OBTH_VSPDL;
                if (R_VSP2_WPF1 == WpfId) {
                    LifId = R_VSP2_LIF1;
                }
            }
        }

        if (R_VSP2_WPF0 == WpfId) {
            /* Disable interrupts */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF0_IRQ_ENB, 0);
            /* Clear Interrupts */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF0_IRQ_STA, 0);
            /* [7] Always set H'0000 0500 to VI6_DPR_WPFn_FPORCH (n=0,1) in case
             * of using WPFn. */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_WPF0_FPORCH, 0x00000500);
        } else {
            /* Disable interrupts */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF1_IRQ_ENB, 0);
            /* Clear Interrupts */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF1_IRQ_STA, 0);
            /* [7] Always set H'0000 0500 to VI6_DPR_WPFn_FPORCH (n=0,1) in case
             * of using WPFn. */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DPR_WPF1_FPORCH, 0x00000500);
        }

        if (R_VSP2_LIF0 == LifId) {
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_LIF0_CTRL, LifSettng);
            /* [9] Always set H'8600 0000 to VI6_LIF0_LBA for R-Car V3M in case
             * of using LIF0 (VI6_LIF0_CTRL.LIF_EN = 1).  Always set H'0000 0000
             * to VI6_LIF0_LBA for R-Car M3-W in case of using LIF0
             * (VI6_LIF0_CTRL.LIF_EN = 1). */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_LIF0_LBA, 0x00000000);

            /* [10] Always set D'1 to VI6_LIFn_CTRL.REQSEL in case of using LIFn
             * (VI6_LIFn_CTRL.LIF_EN = 1). */
            reg_val = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_LIF0_CTRL);
            reg_val |= (1 << 1); /* set REQSEL bit */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_LIF0_CTRL, reg_val);

            /* [11] Always set D'0 to VI6_LIFn_CSBTH in case of using LIFn
             * (VI6_LIFn_CTRL.LIF_EN = 1). */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_LIF0_CSBTH, 0x00000000);

        } else {
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_LIF1_CTRL, LifSettng);

            reg_val = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_LIF1_CTRL);
            reg_val |= (1 << 1); /* set REQSEL bit */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_LIF1_CTRL, reg_val);
            /* [11] Always set D'0 to VI6_LIFn_CSBTH in case of using LIFn
             * (VI6_LIFn_CTRL.LIF_EN = 1). */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_LIF1_CSBTH, 0x00000000);
        }

        /* Initialize Data Path Router (DPR) */
        if ( 0u == InitFlag ) {
            e = R_VSP2_PRV_DprInit(Vsp2Unit, Dpr_Conf);
            if ( R_VSP2_ERR_SUCCESS != e ) {
                R_PRINT_Log("[loc_VspdInit] : Initialize Data Path Router (DPR). Failed(%d)\r\n", e);
            }
        }
    }
    if (R_VSP2_ERR_SUCCESS == e) {
        if (R_VSP2_VSPD0 == Vsp2Unit) {
            if (((R_VSP2_WPF0 == WpfId) && (R_VSP_DPR_ROUTE_WPF0 == Dpr_Conf->Dpr_Bru_Route)) ||
                ((R_VSP2_WPF1 == WpfId) && (R_VSP_DPR_ROUTE_WPF1 == Dpr_Conf->Dpr_Bru_Route))    ) {
                /* Initialize BRU */
                loc_BruVspd.virt_rpf_size_w = ScreenWidth;
                loc_BruVspd.virt_rpf_size_h = ScreenHeight;
                e = R_VSP2_PRV_BruInit(Vsp2Unit, &loc_BruVspd);
                if ( R_VSP2_ERR_SUCCESS != e ) {
                    R_PRINT_Log("[loc_VspdInit] : VSP D0 unit Initialize BRU. Failed(%d)\r\n", e);
                }
            } else if (((R_VSP2_WPF0 == WpfId) && (R_VSP_DPR_ROUTE_BRS_TO_WPF0 == Dpr_Conf->Dpr_Ilv_Brs_Route)) ||
                       ((R_VSP2_WPF1 == WpfId) && (R_VSP_DPR_ROUTE_BRS_TO_WPF1 == Dpr_Conf->Dpr_Ilv_Brs_Route))    ) {
                /* Initialize BRS */
                loc_BrsVspd.virt_rpf_size_w = ScreenWidth;
                loc_BrsVspd.virt_rpf_size_h = ScreenHeight;
                e = R_VSP2_PRV_BrsInit(Vsp2Unit, &loc_BrsVspd);
                if ( R_VSP2_ERR_SUCCESS != e) {
                    R_PRINT_Log("[loc_VspdInit] : VSP D0 unit Initialize BRS. Failed(%d)\r\n", e);
                }
                select_brs = 1u;
            }
        } else {
            /* Initialize BRU */
            loc_BruVspd.virt_rpf_size_w = ScreenWidth;
            loc_BruVspd.virt_rpf_size_h = ScreenHeight;
            e = R_VSP2_PRV_BruInit(Vsp2Unit, &loc_BruVspd);
            if ( R_VSP2_ERR_SUCCESS != e ) {
                R_PRINT_Log("[loc_VspdInit] : Initialize BRU. Failed(%d)\r\n", e);
            }
        }
    }

    if (R_VSP2_ERR_SUCCESS == e) {
        /* Initialize WPF and LIF */
        if (1u == select_brs) {
            loc_WpfVspd.source_rpf = R_VSP2_VI6_WPFn_SRCRPF_VIR_ACT2_MASTER;
        } else {
            loc_WpfVspd.source_rpf = R_VSP2_VI6_WPFn_SRCRPF_VIR_ACT_MASTER;
        }
        e = R_VSP2_PRV_WpfSet(Vsp2Unit, WpfId, &loc_WpfVspd);
        if (R_VSP2_ERR_SUCCESS == e) {
            e = R_VSP2_PRV_LifEnable(Vsp2Unit, LifId, R_VSP2_ENABLE);
            if (R_VSP2_ERR_SUCCESS == e) {
                /* Init Display List (return if no DL address was given) */
                e = R_VSP2_PRV_DlInit(Vsp2Unit, WpfId);
                if ( R_VSP2_ERR_SUCCESS != e ) {
                    R_PRINT_Log("[loc_VspdInit] : Init Display List. Failed(%d)\r\n", e);
            }
            } else {
                R_PRINT_Log("[loc_VspdInit] : R_VSP2_PRV_LifEnable Failed(%d)\r\n", e);
            }
        } else {
            R_PRINT_Log("[loc_VspdInit] : R_VSP2_PRV_WpfSet Failed(%d)\r\n", e);
        }
    }
    return e;
}

r_vsp2_Error_t R_VSP2_Start(r_vsp2_Unit_t Vsp2Unit, uint32_t WpfId)
{
    r_vsp2_Error_t e = R_VSP2_ERR_SUCCESS;
    uint32_t vsp2_reg_base = 0;
    // read out STATUS of VSPD to check if VSPD is properly started
    // This can be traced with the trace unit in real time.    
    volatile uint32_t start_value=0; 

    if (Vsp2Unit >= R_VSP2_LAST) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_Start] : VSP2 Unit No is Invalid. Failed(%d)\r\n", e);
    } else {
        vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);
        switch (WpfId) {
        case R_VSP2_WPF0:
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF0_IRQ_STA, 0); /* Clear interrupts */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF0_IRQ_ENB, (1 << 0)); /* Enable Frame End interrupt */
	    start_value = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_STATUS);
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_CMD0, R_VSP2_VI6_CMD_STRCMD);
	    start_value = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_STATUS);
            break;
        case R_VSP2_WPF1:
            /* Note: shall test the device. in case of V3x there is no WPF1 */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF1_IRQ_STA, 0); /* Clear interrupts */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF1_IRQ_ENB, (1 << 0)); /* Enable Frame End interrupt */
	    start_value = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_STATUS);
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_CMD1, R_VSP2_VI6_CMD_STRCMD);
	    start_value = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_STATUS);
            break;
        default:
            e = R_VSP2_ERR_INVALID_PARAMETER;
            R_PRINT_Log("[R_VSP2_Start] : Wpf number is Invalid. Failed(%d)\r\n", e);
            break;
        }
    }
    return e;
}

/* Init a VSP2 module. Note: Supports only VSPDs currently */
r_vsp2_Error_t R_VSP2_Init(r_vsp2_Unit_t      Vsp2Unit,
                           uint32_t           WpfId,
                           uint32_t           ScreenWidth,
                           uint32_t           ScreenHeight,
                           uintptr_t          DlAddr,
                           r_vsp2_Dpr_Conf_t *Dpr_Conf,
                           uint32_t           InitFlag)
{
    r_vsp2_Error_t e = R_VSP2_ERR_SUCCESS;
    uint32_t reg_val = 0;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

    if (R_VSP2_LAST <= Vsp2Unit) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_Init] : VSP2 Unit No is Invalid. Failed(%d)\r\n", e);
    } else {
        if (0 != DlAddr) {
            /* we currently don't support DL. Return with error*/
            e = R_VSP2_ERR_INVALID_PARAMETER;
            R_PRINT_Log("[R_VSP2_Init] : Display list address is Invalid. Failed(%d)\r\n", e);
        } else if (NULL == Dpr_Conf) {
            e = R_VSP2_ERR_INVALID_PARAMETER;
            R_PRINT_Log("[R_VSP2_Init] : VSP2 DPR configuration information is NULL. Failed(%d)\r\n", e);
        } else {
            ;   /* Nothing to do */
        }
    }

    if (R_VSP2_ERR_SUCCESS == e) {
        r_vsp2_ModuleDlAddr[Vsp2Unit] = DlAddr;

        if( 0u == InitFlag ) {
            /* Disable clock stop function */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_CLK_CTRL0, 0x10010f1f);
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_CLK_CTRL1, 0xff00ffff);
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_CLK_DCSWT, 0x00130808);
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_CLK_DCSM0, 0x1fff0f1f);
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_CLK_DCSM1, 0xff00ffff);

            /* Module reset */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_MRESET_ENB0, 0x30000f1f);
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_MRESET_ENB1, 0xff00ffff);
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_MRESET, 0x00000001);

           /* Software reset */
           if (R_VSP2_WPF0 == WpfId) {
                R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_SRESET, R_VSP2_VI6_SRESET_SRST0);
            } else {
                R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_SRESET, R_VSP2_VI6_SRESET_SRST1);
            }

            /* Set fixed value to following registers in any case (Sec.32.3.1 HW UM) */
            /* [1] Always set H'0000 0808 to VI6_CLK_DCSWT */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_CLK_DCSWT, 0x00000808);

            /* [2] Always set D'256 to VI6_DL_CTRL.AR_WAIT [7:0] in case of
             * using display list. */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DL_CTRL, (256 << 16));

            /* [3] Always set D'2 to VI6_DL_EXT_CTRLn.POLINT [5:0] in case of
             * using extended display list of WPFn. */
            reg_val = (2 << 8);

            /* [4] Always set D'1 to VI6_DL_EXT_CTRLn.DLPRI in case of using
             * extended display list of WPFn. */
            reg_val |= (1 << 5);

            /* [5] Always set D'0 to VI6_DL_EXT_CTRLn.EXPRI in case of using
             * extended display list of WPFn. */
            reg_val &= ~(1 << 4);

            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DL_EXT_CTRL0, reg_val);
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_DL_EXT_CTRL1, reg_val);

            if ((R_VSP2_VSPI0 == Vsp2Unit) || (R_VSP2_VSPI1 == Vsp2Unit)) {
                e = loc_VspiInit(Vsp2Unit, WpfId, Dpr_Conf, InitFlag);
                if ( R_VSP2_ERR_SUCCESS != e ) {
                    R_PRINT_Log("[R_VSP2_Init] : loc_VspiInit Failed(%d)\r\n", e);
                }
            }
            else {
                e = loc_VspdInit(Vsp2Unit, WpfId, ScreenWidth, ScreenHeight, Dpr_Conf, InitFlag);
                if ( R_VSP2_ERR_SUCCESS != e ) {
                    R_PRINT_Log("[R_VSP2_Init] : loc_VspdInit Failed(%d)\r\n", e);
                }
            }
        } else {
            if (R_VSP2_VSPD0 == Vsp2Unit) {
                /* Software reset */
                if (R_VSP2_WPF0 == WpfId) {
                    R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_SRESET, R_VSP2_VI6_SRESET_SRST0);
                } else {
                    R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_SRESET, R_VSP2_VI6_SRESET_SRST1);
                }
                e = loc_VspdInit(Vsp2Unit, WpfId, ScreenWidth, ScreenHeight, Dpr_Conf, InitFlag);
                if ( R_VSP2_ERR_SUCCESS != e ) {
                    R_PRINT_Log("[R_VSP2_Init] : VSP D0 unit loc_VspdInit Failed(%d)\r\n", e);
                }
            }
            // TODO What should be done here? Removed due to possible unexpected side effects
            // else {
            //     e = loc_VspiInit(Vsp2Unit, WpfId, Dpr_Conf, InitFlag);
            //     if ( R_VSP2_ERR_SUCCESS != e ) {
            //         R_PRINT_Log("[R_VSP2_Init] : loc_VspiInit Failed(%d)\r\n", e);
            //     }
            // }
        }
    }
    return e;
}
