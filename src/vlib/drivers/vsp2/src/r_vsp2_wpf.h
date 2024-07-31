/*************************************************************************************************************
* vsp2_wpf_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_VSP2_WPF_H_
#define R_VSP2_WPF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r_vsp2_api.h"

#define R_VSP2_WPF0 0u
#define R_VSP2_WPF1 1u

typedef struct {
    uint32_t source_rpf; //Holds value for VI6_WPFn_SRCRPF reg
    uint32_t clip_h_enable;
    uint32_t clip_h_size;
    uint32_t clip_h_offset;
    uint32_t clip_v_enable;
    uint32_t clip_v_size;
    uint32_t clip_v_offset;
    uint32_t mem_addr_y_rgb;
    uint32_t mem_addr_c0;
    uint32_t mem_addr_c1;
    uint32_t mem_stride_y_rgb;
    uint32_t mem_stride_c;
    uint32_t format;
    uint32_t yc_swap;
    uint32_t uv_swap;
    uint32_t c_conversion;
    uint32_t writeback_enable;
    uint32_t pxa;
    uint32_t data_swap;
} r_vsp2_Wpf_t;

r_vsp2_Error_t R_VSP2_PRV_WpfSet(r_vsp2_Unit_t Vsp2Unit, uint32_t Wpfid,
                                 const r_vsp2_Wpf_t *Config);
void R_VSP2_PRV_WpfEnableRpf(r_vsp2_Unit_t Vsp2Unit, uint32_t WpfId,
                             uint32_t RpfId, uint32_t Enable);

#ifdef __cplusplus
}
#endif

#endif /* R_VSP2_WPF_H_ */
