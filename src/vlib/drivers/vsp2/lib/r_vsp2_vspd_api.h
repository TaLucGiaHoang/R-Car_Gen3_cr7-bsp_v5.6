/*************************************************************************************************************
* vsp2_vspd_api_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_VSP2_VSPD_API_H_
#define R_VSP2_VSPD_API_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "r_vsp2_api.h"

/**
 * \defgroup VSPDgroup VSPD specific API
 * @ingroup VSPgroup
 * @brief   VSPD driver interface
 *
 * @{
 * @}
 */

/**
 * \defgroup VSPDapi   API
 * @ingroup VSPDgroup
 * @{
 */

/**
 * Maximum number of VSPD Units
 */
#define R_VSPD_MAX_UNIT_NUM (3u)

/**
 * Set the Framebuffer address for the given layer
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   RpfId       - reference to the RPF to be used. Match the PosZ of the layer
 * @param[in]   WpfId       - reference to the WPF to be used
 * @param[in]   LayerAddr   - Address of the Frame buffer the RPF will be reading from
 * @param[in]   C0_Addr     - Address of the Frame buffer the RPF for C0 will be reading from
 * @param[in]   C1_Addr     - Address of the Frame buffer the RPF for C1 will be reading from
 * @retval      none
 */
void R_VSP2_VSPD_LayerAddrSet(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId,
                              uint32_t WpfId, uint32_t LayerAddr, uint32_t C0_Addr, uint32_t C1_Addr);

/**
 * Set the Framebuffer format for the given layer
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit, valid value R_VSP2_VSPD0/R_VSP2_VSPD1/R_VSP2_VSPD2.
 *                            See @ref r_vsp2_Unit_t
 * @param[in]   RpfId       - reference to the RPF to be used. Match the PosZ of the layer
 * @param[in]   LayerFormat - Format of the Frame buffer the RPF will be reading from
 * @retval      none
 */
void R_VSP2_VSPD_LayerFormatSet(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId,
                                r_wm_ColorFmt_t LayerFormat);

/**
 * Set the X position of the layer
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   RpfId       - reference to the RPF to be used. Match the PosZ of the layer
 * @param[in]   PosX        - posX of the Layer, range 0 - 8189
 * @retval      none
 */
void R_VSP2_VSPD_LayerPosXSet(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId,
                              uint32_t PosX);

/**
 * Set the Y position of the layer
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   RpfId       - reference to the RPF to be used. Match the PosZ of the layer
 * @param[in]   PosY        - posY of the Laye, range 0 - 8189
 * @retval      none
 */
void R_VSP2_VSPD_LayerPosYSet(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId,
                              uint32_t PosY);

/**
 * Set the Width of the layer
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   RpfId       - reference to the RPF to be used. Match the PosZ of the layer
 * @param[in]   Width       - Layer's width in pixels, range 1 - 8190
 * @param[in]   StrideY     - Layer's strideY in bytes (Memory Stride of Source Picture Y/RGB Plane)
 * @param[in]   StrideC     - Layer's strideC in bytes (Memory Stride of Source Picture C Plane)
 * @retval      none
 */
void R_VSP2_VSPD_LayerWidthSet(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId,
                               uint32_t Width, uint16_t StrideY, uint16_t StrideC);

/**
 * Set the Height of the layer
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   RpfId       - reference to the RPF to be used. Match the PosZ of the layer
 * @param[in]   Height      - Layer's Height in pixels, range 1 - 8190
 * @retval      none
 */
void R_VSP2_VSPD_LayerHeightSet(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId,
                                uint32_t Height);

/**
 * Enable/disable a Vsp2 layer Layer
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   RpfId       - reference to the RPF to be used. Match the PosZ of the layer
 * @param[in]   Enable      - 1 to enable, 0 to disable
 * @retval      none
 */
void R_VSP2_VSPD_LayerEnable(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId,
                             bool Enable);

/**
 * Flush the commands to the Vsp2.
 *
 * @note    this function mark the current RPF setting to be used at the next Vsync.
 *          The registers will be written to the corresponding registers and VSP2 restarted.
 *
 * @param[in]  Vsp2Unit - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]  Mask     - Specifies which RpfID units to be updated
 * @retval     none
 */
void R_VSP2_VSPD_Flush(r_vsp2_Unit_t Vsp2Unit, uint32_t Mask);

/**
 * Control the allocation of a layer.
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   RpfId       - reference to the RPF to be used. Match the PosZ of the layer
 * @param[in]   Allocate    - True = grab, Fals e= release
 * @retval      True        - if the layer is available, else False
 */
bool R_VSP2_VSPD_LayerAllocate(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId, bool Allocate);

/**
 * Enable/disable DISCOM for a specific layer
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   RpfId       - reference to the RPF to be used. Match the PosZ of the layer
 * @param[in]   Enable      - 1 to enable, 0 to disable
 * @retval      Success
 */
bool R_VSP2_VSPD_DiscomEnable(r_vsp2_Unit_t Vsp2Unit, uint32_t RpfId, bool Enable);

/**
 * Set the position of the DISCOM region of the layer
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   PosX        - posX of the region. Must be a multiple of 2 for M3-W(+)
 * @param[in]   PosY        - posY of the region
 */
void R_VSP2_VSPD_DiscomSetPos(r_vsp2_Unit_t Vsp2Unit, uint32_t PosX, uint32_t PosY);

/**
 * Set the size of the DISCOM region of the layer
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   Width       - The region's width in pixels. Must be a multiple of 2 for M3-W(+)
 * @param[in]   Height      - The region's Height in pixels
 */
void R_VSP2_VSPD_DiscomSetSize(r_vsp2_Unit_t Vsp2Unit, uint32_t Width, uint32_t Height);

/**
 * Get the CRC of the DISCOM region
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @retval      CRC of the region
 */
uint32_t R_VSP2_VSPD_DiscomGetCrc(r_vsp2_Unit_t Vsp2Unit);

/**
 * Set the expected CRC
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   Crc         - Expected CRC of the region
 */
void R_VSP2_VSPD_DiscomSetExpectedCrc(r_vsp2_Unit_t Vsp2Unit, uint32_t Crc);

/**
 * Enable the DISCOM IRQ
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   Enable      - 1 to enable, 0 to disable
 */
void R_VSP2_VSPD_DiscomEnableIrq(r_vsp2_Unit_t Vsp2Unit, bool Enable);

/**
 * Clear the compare status bit
 *
 * @param[in]   Vsp2Unit    - Vsp2 Unit @ref r_vsp2_Unit_t
 */
void R_VSP2_VSPD_DiscomClearStatus(r_vsp2_Unit_t Vsp2Unit);

/** @} */ /* end of group VSPDgroup */

#ifdef __cplusplus
}
#endif

#endif /* R_VSP2_VSPD_API_H_ */
