/*************************************************************************************************************
* vsp2_vspi_api_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_VSP2_VSPI_API_H_
#define R_VSP2_VSPI_API_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "r_vsp2_api.h"
#include "r_wm_common.h"

/**
 * \defgroup VSPIgroup VSPI specific API
 * @ingroup VSPgroup
 * @brief   VSPI driver interface
 *
 * @{
 */

/**
 * Maximum number of VSPI Units
 */
#define R_VSPI_MAX_UNIT_NUM (2u)

typedef struct {
    uint32_t            Width;       /**< Surface's width */
    uint32_t            Height;      /**< Surface's height */
    uint16_t            StrideY;     /**< Surface's stride_y Memory Stride of Source Picture Y/RGB Plane (unit:byte) */
    uint16_t            StrideC;     /**< Surface's stride_c Memory Stride of Source Picture C Plane (unit:byte) */
    r_wm_ColorFmt_t     Fmt;         /**< Surface's color format. @ref r_wm_ColorFmt_t*/
    uintptr_t           FBuf;        /**< address to the Framebuffer */
    uintptr_t           FBufC0;      /**< address to the Framebuffer for C0 */
    uintptr_t           FBufC1;      /**< address to the Framebuffer for C1 */
} r_vspi_Surface_t;

/**
 * Initialize the specified VSPI unit (VSPI0 or VSPI1)
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @retval      none
 */
r_vsp2_Error_t R_VSP2_VSPI_Init(r_vsp2_Unit_t Vsp2Unit);

/**
 * Convert source surface to desired destination surface
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   src_surf    - Source surface to be used in RPF
 * @param[in]   dst_surf    - Destination surface to be used in WPF
 * @retval      none
 */
r_vsp2_Error_t R_VSP2_VSPI_ConvertSurface(r_vsp2_Unit_t Vsp2Unit, r_vspi_Surface_t * src_surf, r_vspi_Surface_t * dst_surf);

/** @} */ /* end of group VSPIgroup */

#ifdef __cplusplus
}
#endif

#endif /* R_VSP2_VSPI_API_H_ */
