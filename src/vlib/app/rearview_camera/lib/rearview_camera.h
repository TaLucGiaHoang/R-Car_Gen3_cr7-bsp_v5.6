/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : rearview_camera.h
 * Version      : 1.0
 * Description  : List API functions of rearview camera demo app.
 *********************************************************************************************************************/

#ifndef REARVIEW_CAMERA_H
#define REARVIEW_CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "display.h"
#include "r_cio_vin_public.h"
#include "r_adv7482_api.h"
#include "r_vin_api.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define RVC_ID_LAYER_CLUSTER (0)
#define RVC_ID_LAYER_CID     (1)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef struct {
    r_wm_DisplayPort_t Port;
    r_wm_Surface_t VoutSurface;
} rvc_Layer_t;

typedef struct {
    r_wm_DisplayPort_t Port;
    r_wm_ColorFmt_t   Format;
    uint32_t          BackgroundColor;
} rvc_DispCfg_t;

typedef struct {
    rvc_DispCfg_t * ClusterDisplayPtr;
    rvc_DispCfg_t * CidDisplayPtr;
    rvc_Layer_t *   ClusterLayerPtr;
    rvc_Layer_t *   CidLayerPtr;
    int             settings_in_cluster;
    int             settings_in_cid;
} rvc_Config_t;

typedef struct {
    uint32_t        ImageInWidth;       /**< length of the camera horizontal line */
    uint32_t        ImageInHeight;      /**< length of the camera vertical line */
    uint32_t        ImageScaledWidth;   /**< length of the scaled image horizontal line */
    uint32_t        ImageScaledHeight;  /**< length of the scaled image vertical line */
    uint32_t        PosX;               /**< position on the X axis in pixels */
    uint32_t        PosY;               /**< position on the Y axis in pixels */
    uint32_t        PosZ;               /**< position on the Z axis. number must correspond to a Layer number */
    uint32_t        Width;              /**< Surface's width */
    uint32_t        Height;             /**< Surface's height */
    uint16_t        StrideY;            /**< Surface's stride_y Memory Stride of Source Picture Y/RGB Plane (unit:byte) */
    uint16_t        StrideC;            /**< Surface's stride_c Memory Stride of Source Picture C Plane (unit:byte) */
    uint32_t        PreClipPosX;        /**< Start Pixel Pre-Clip (SPPrC[11:0] of VnSPPrC) */
    uint32_t        PreClipPosY;        /**< Start Pixel Pre-Clip (SLPrC[11:0] of VnSLPrC) */
    uint32_t        Uvaof;              /**< Surface's UV Data Address Offset (unit:byte) */
    uint16_t        DataRate;           /**< MIPI input data rate (Mbps) from ADV7482 */
    uint8_t         Lanes;              /**< Number of lanes to use */
    bool            Interlaced;         /**< true:Interlace, false:Progressive */
    uint8_t         VmcInf;             /**< VIN VMC register INF bit */
    uint8_t         VmcIm;              /**< VIN VMC register IM bit */
    uint8_t         ColorSpaceOut;      /**< hdmi color space output from ADV7482 */
    uint8_t         BoundaryMbps;       /**< hdmi boundary 600Mbps from ADV7482 */
    r_wm_ColorFmt_t Fmt;                /**< Surface's color format. @ref r_wm_ColorFmt_t*/
    void           *Handle;             /**< pointer to a surface handle the user got from the CIO WM by calling
                                              the R_WM_SurfaceGet() function. */
    uintptr_t       FBuf;               /**< address to the Framebuffer */
    uintptr_t       FBufC0;             /**< address to the Framebuffer for C0 */
    uintptr_t       FBufC1;             /**< address to the Framebuffer for C1 */
    r_wm_Doc_t     *Doc;                /**< Surface's DOC configuration @ref r_wm_Doc_t */
} rvc_Surface_t;

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern void RVC_Main(const rvc_Config_t * ConfigPtr);
extern void RVC_ToggleDisplayLayer(uint8_t LayerIndex, bool On);

#ifdef __cplusplus
}
#endif

#endif /* REARVIEW_CAMERA_H */
