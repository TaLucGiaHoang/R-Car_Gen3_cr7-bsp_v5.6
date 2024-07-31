/*************************************************************************************************************
* vsp2_wpf_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include "r_vsp2_api.h"
#include "r_vsp2_wpf.h"
#include "r_vsp2_regs.h"
#include "r_print_api.h"

r_vsp2_Error_t R_VSP2_PRV_WpfSet(r_vsp2_Unit_t Vsp2Unit, uint32_t Wpfid,
                                 const r_vsp2_Wpf_t *Config)
{
    r_vsp2_Error_t e = R_VSP2_ERR_SUCCESS;
    uint32_t reg_val = 0;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

    if (Vsp2Unit >= R_VSP2_LAST) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_PRV_WpfSet] : VSP2 Unit No is Invalid. Failed(%d)\r\n", e);
    } else if (0 == Config) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_PRV_WpfSet] : VSP2 WPF configuration information is NULL. Failed(%d)\r\n", e);
    } else {

        /* Horizontal clipping  */
        reg_val = ((*Config).clip_h_offset & 0xff) << 16; //HCL_OFST
        reg_val |= (*Config).clip_h_size & 0x1fff; //HCL_SIZE
        reg_val |= (*Config).clip_h_enable ? (1 << 28) : 0; //HCEN
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPFn_HSZCLIP(Wpfid), reg_val);

        /* Vertical clipping  */
        reg_val = ((*Config).clip_v_offset & 0xff) << 16; //VCL_OFST
        reg_val |= (*Config).clip_v_size & 0x1fff; //VCL_SIZE
        reg_val |= (*Config).clip_v_enable ? (1 << 28) : 0; //VCEN
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPFn_VSZCLIP(Wpfid), reg_val);

        /* Set WPF destination memory address */
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPFn_DSTM_ADDR_Y(Wpfid), (*Config).mem_addr_y_rgb);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPFn_DSTM_ADDR_C0(Wpfid), (*Config).mem_addr_c0);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPFn_DSTM_ADDR_C1(Wpfid), (*Config).mem_addr_c1);

        /* Set memory stride */
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPFn_DSTM_STRIDE_Y(Wpfid), (*Config).mem_stride_y_rgb);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPFn_DSTM_STRIDE_C(Wpfid), (*Config).mem_stride_c);

        /* Set writeback enable */
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPF1_WRBCK_CTRL(Wpfid), (*Config).writeback_enable);

        /* WPF swapping */
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPFn_DSWAP(Wpfid), (*Config).data_swap);

        /* WPF output format*/
        reg_val = (*Config).format;
        reg_val |= ((*Config).pxa << 23); // PAD Data select (PDV or DPR)
        switch ((*Config).format) {
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_RGB332:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_XRGB4444:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_ARGB8888:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_RGBA8888:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_RGB888:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_BGR888:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_ARGB4444:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_XRGB1555:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_ARGB1555:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_RGB565:
            break;
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV420:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV422:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV444:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV444ITL:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV422ITL:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV420ITL:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV444P:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV422P:
        case R_VSP2_VI6_WPFn_OUTFMT_WRFMT_YUV420P:
            reg_val |= ((*Config).yc_swap << 15);       // SPYCS
            reg_val |= ((*Config).uv_swap << 14);       // SPUVS
            reg_val |= ((*Config).c_conversion << 9);   // WRTM (CSC Conversion Method,0=BT.601, 2=BT.709)
            reg_val |= 0x1 << 8; /*((*Config).csc << 8);*/ // Always convert YUV to RGB
            break;
        default:
            e = R_VSP2_ERR_INVALID_PARAMETER;
            R_PRINT_Log("[R_VSP2_PRV_WpfSet] : WPF output format value(%d) is Invalid. Failed(%d)\r\n", (*Config).format, e);
            /* Fall back to ARGB888 */
            reg_val = R_VSP2_VI6_WPFn_OUTFMT_WRFMT_ARGB8888;
            break;
        }
        if (R_VSP2_ERR_SUCCESS == e) {
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPFn_OUTFMT(Wpfid), reg_val);

            /* Set Source RPFs */
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPFn_SRCRPF(Wpfid), (*Config).source_rpf);
        }
    }
    return e;
}

/**
 * Enable the given RPF/layer on the WPF
 *
 * Note: Accept all RPL/layer except for the the virtual RPF.
 *       The RPF will be SUB and not Master
 */
void R_VSP2_PRV_WpfEnableRpf(r_vsp2_Unit_t Vsp2Unit, uint32_t WpfId,
                             uint32_t RpfId, uint32_t Enable)
{
    /* Enable/disable layer */
    uint32_t srcrpf_rpfn_flag;
    uint32_t reg_val;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

    switch (RpfId) {
    case 0:
        srcrpf_rpfn_flag = R_VSP2_VI6_WPFn_SRCRPF_RPF0_ACT_SUB;
        break;
    case 1:
        srcrpf_rpfn_flag = R_VSP2_VI6_WPFn_SRCRPF_RPF1_ACT_SUB;
        break;
    case 2:
        srcrpf_rpfn_flag = R_VSP2_VI6_WPFn_SRCRPF_RPF2_ACT_SUB;
        break;
    case 3:
        srcrpf_rpfn_flag = R_VSP2_VI6_WPFn_SRCRPF_RPF3_ACT_SUB;
        break;
    case 4:
        srcrpf_rpfn_flag = R_VSP2_VI6_WPFn_SRCRPF_RPF4_ACT_SUB;
        break;
    default:
        R_PRINT_Log("[R_VSP2_PRV_WpfEnableRpf] : RpfId is Invalid.\r\n");
        srcrpf_rpfn_flag = 0;
        break;/* wrong rpf */
    }

    reg_val = R_VSP2_PRV_RegRead(vsp2_reg_base + R_VSP2_VI6_WPFn_SRCRPF(WpfId));
    if (R_VSP2_ENABLE == Enable) {
        reg_val |= srcrpf_rpfn_flag;
    } else {
        reg_val &= ~srcrpf_rpfn_flag;
    }
    R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_WPFn_SRCRPF(WpfId), reg_val);
}
