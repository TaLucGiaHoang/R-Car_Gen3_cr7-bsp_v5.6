/*************************************************************************************************************
* vsp2_rpf_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_VSP2_RPF_H_
#define R_VSP2_RPF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r_vsp2_api.h"

typedef struct {
    uint32_t maplha;
    uint32_t mCr;
    uint32_t mY;
    uint32_t mCb;
} r_vsp2_Mask_t;

typedef struct {
    uint32_t maplha;
    uint32_t mCr;
    uint32_t mY;
    uint32_t mCb;
} r_vsp2_Ckey_t;

typedef struct {
    uint32_t size_w;
    uint32_t size_h;
    uint32_t pos_x;
    uint32_t pos_y;
    uint32_t mem_addr_y_rgb;
    uint32_t mem_addr_c0;
    uint32_t mem_addr_c1;
    uint32_t mem_addr_Ai;
    uint32_t mem_stride;
    uint32_t mem_stride_alpha;
    uint32_t c_interpolation;
    uint32_t yc_swap;
    uint32_t uv_swap;
    uint32_t c_extention;
    uint32_t c_conversion;
    uint32_t csc;
    uint32_t format;
    uint32_t alphaSel;
    uint32_t irop;
    uint32_t bsel;
    uint32_t alphaExl;
    uint32_t alpha[2];
    uint32_t maskEnable;
    uint32_t mCr;
    uint32_t mY;
    uint32_t mCb;
    r_vsp2_Mask_t mask[2];
    uint32_t CKeySel;
    r_vsp2_Ckey_t CKey[2];
    uint32_t AlphaNorm;
    uint32_t RGBNorm;
    uint32_t AlphaRatio;
    uint32_t bpp;
    uint32_t data_swap;
    uint16_t stride_y;      /* setting in bytes */
    uint16_t stride_c;      /* setting in bytes */
} r_vsp2_Rpf_t;

r_vsp2_Error_t R_VSP2_PRV_RpfSet(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId,
                                 const r_vsp2_Rpf_t *Config);

r_vsp2_Error_t R_VSP2_WinAlphaSet(r_vsp2_Unit_t Vsp2Unit, uint8_t Alpha, uint32_t rpfid);

#ifdef __cplusplus
}
#endif

#endif /* R_VSP2_RPF_H_ */
