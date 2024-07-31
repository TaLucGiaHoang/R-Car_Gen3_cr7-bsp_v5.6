/*************************************************************************************************************
* vsp2_rpf_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include "r_vsp2_api.h"
#include "r_vsp2_rpf.h"
#include "r_vsp2_regs.h"
#include "r_print_api.h"

r_vsp2_Error_t R_VSP2_PRV_RpfSet(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId,
                                 const r_vsp2_Rpf_t *Config)
{
    r_vsp2_Error_t e = R_VSP2_ERR_SUCCESS;
    uint32_t reg_val = 0;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

    if (Vsp2Unit >= R_VSP2_LAST) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_PRV_RpfSet] : VSP2 Unit No is Invalid. Failed(%d)\r\n", e);
    } else if (0 == Config) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_PRV_RpfSet] : VSP2 RPF configuration information is NULL. Failed(%d)\r\n", e);
    }

    if (R_VSP2_ERR_SUCCESS == e) {
        /* Set basic and extended read size */
        reg_val = (((*Config).size_w & 0x1fff) << 16) | ((*Config).size_h & 0x1fff);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_SRC_BSIZE(RpfId), reg_val);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_SRC_ESIZE(RpfId), reg_val);

        /* RPF input format - set parameters for YCbCr (YUV) formats*/
        if (0x40 <= ((*Config).format))
        {
            reg_val = ((*Config).c_interpolation << 16);
            reg_val |= ((*Config).yc_swap << 15);
            reg_val |= ((*Config).uv_swap << 14);
            reg_val |= ((*Config).c_extention << 12);
            reg_val |= ((*Config).c_conversion << 9);
            reg_val |= 0x1 << 8; /*((*Config).csc << 8);*/ // Always convert YUV to RGB
            reg_val |= (*Config).format & 0x007F;
        }
        else
        {
            reg_val = (*Config).format & 0x007F;
        }
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_INFMT(RpfId), reg_val);

        /* RPF swapping */
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_DSWAP(RpfId), (*Config).data_swap);

        /* RPF display location */
        reg_val = (((*Config).pos_x & 0x1fff) << 16) | ((*Config).pos_y & 0x1fff);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_LOC(RpfId), reg_val);

        /* RPF stride */
        reg_val = (*Config).mem_stride;
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_SRCM_PSTRIDE(RpfId), reg_val);

        /* RPF alpha stride */
        reg_val = ((*Config).mem_stride_alpha & 0xffff);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_SRCM_ASTRIDE(RpfId), reg_val);

        /* Set RPF memory address */
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_SRCM_ADDR_Y(RpfId), (*Config).mem_addr_y_rgb);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_SRCM_ADDR_C0(RpfId), (*Config).mem_addr_c0);
        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_SRCM_ADDR_C1(RpfId), (*Config).mem_addr_c1);

        if (R_VSP2_VI6_RPFn_INFMT_RDFMT_YUV420 == ((*Config).format))
        {
            /* Set AlphaSel */
            reg_val = (((*Config).alphaSel & 0x0007) << 28);
            reg_val |= (((*Config).irop & 0x000F) << 24);
            reg_val |= (((*Config).bsel) << 23);
            reg_val |= (((*Config).alphaExl) << 18);
            reg_val |= (((*Config).alpha[1]) << 8);
            reg_val |= ((*Config).alpha[0]);
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_ALPH_SEL(RpfId), reg_val);

            /* Set Mask Control */
            reg_val = 0x00000000;
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_MSKCTRL(RpfId), reg_val);

//            /* Set IROP-SRC Input Value 0 */
//            reg_val = (((*Config).mask[0].malpha) << 24);
//            reg_val |= (((*Config).mask[0].mCr) << 16);
//            reg_val |= (((*Config).mask[0].mY) << 8);
//            reg_val |= ((*Config).mask[0].mCb);
//            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_MSKSET0(RpfId), reg_val);
//
//            /* Set IROP-SRC Input Value 1 */
//            reg_val = (((*Config).mask[1].malpha) << 24);
//            reg_val |= (((*Config).mask[1].mCr) << 16);
//            reg_val |= (((*Config).mask[1].mY) << 8);
//            reg_val |= ((*Config).mask[1].mCb);
//            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_MSKSET1(RpfId), reg_val);

            /* Set Color Keying Control */
            reg_val = 0x00000000;
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_CKEY_CTRL(RpfId), reg_val);

//            /* Set ColorKeyring Color Setting 0 */
//            reg_val = (((*Config).CKey[0].malpha) << 24);
//            reg_val |= (((*Config).CKey[0].mCr) << 16);
//            reg_val |= (((*Config).CKey[0].mY) << 8);
//            reg_val |= ((*Config).CKey[0].mCb);
//            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_CKEY_SET0(RpfId), reg_val);
//
//            /* Set ColorKeyring Color Setting 1 */
//            reg_val = (((*Config).CKey[1].malpha) << 24);
//            reg_val |= (((*Config).CKey[1].mCr) << 16);
//            reg_val |= (((*Config).CKey[1].mY) << 8);
//            reg_val |= ((*Config).CKey[1].mCb);
//            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_CKEY_SET1(RpfId), reg_val);

            /* Set Multiple Alpha Control */
            reg_val = 0x00000000;
            R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_MULT_ALPH(RpfId), reg_val);
        }
    }
    return e;
}

r_vsp2_Error_t R_VSP2_WinAlphaSet(r_vsp2_Unit_t Vsp2Unit, uint8_t Alpha, uint32_t RpfId)
{
    r_vsp2_Error_t e = R_VSP2_ERR_SUCCESS;
    uint32_t reg_val;
    uint32_t vsp2_reg_base = R_VSP2_PRV_GetRegBase(Vsp2Unit);

    if ((Vsp2Unit == R_VSP2_VSPI0) || (Vsp2Unit == R_VSP2_VSPI1) ||
        (Vsp2Unit == R_VSP2_VSPBS)) {
        e = R_VSP2_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VSP2_WinAlphaSet] : VSP2 Unit No is Invalid. Failed(%d)\r\n", e);
    }

    if (R_VSP2_ERR_SUCCESS == e) {
        reg_val = 0x00000000;
        reg_val |= (1 << 12); //A_MMD = 1
        //reg_val |= (1 << 8); //P_MMD = 0, this is already 0
        reg_val |= Alpha; //alpha ratio (mul)

        R_VSP2_PRV_RegWrite(vsp2_reg_base + R_VSP2_VI6_RPFn_MULT_ALPH(RpfId), reg_val);
     }
    return e;
}

