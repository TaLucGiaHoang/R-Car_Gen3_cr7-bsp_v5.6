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
 * File Name    : rvc_cfg.c
 * Version      : 1.0
 * Description  : Configuration of fastboot_mono sub app.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#if FASTBOOT_CFG_APP_CAMERA_ENABLE

#include "fastboot_cfg.h"

#include "rearview_camera.h"
#include "r_prr_api.h"
#include "r_vin_api.h"
#include "r_adv7482_api.h"

#if defined(R_TARGET_BOARD_SALVATORXS)

static rvc_DispCfg_t loc_DispCluster =
{
#ifdef R_TARGET_BOARD_EBISU
    .Port = R_WM_DISP_PORT_LVDS0,
#else /* R_TARGET_BOARD_SALVATORXS */
    .Port = R_WM_DISP_PORT_HDMI1,
#endif
    .Format = R_WM_FMT_ARGB8888,
    .BackgroundColor = 0xFFFFFFFF, /* white */
};

static rvc_DispCfg_t loc_DispCid =
{
    .Port = R_WM_DISP_PORT_LVDS0,
    .Format = R_WM_FMT_ARGB8888,
    .BackgroundColor = 0xFFFFFFFF, /* white */
};

static rvc_Layer_t loc_LayerCluster =
{
    /* csi40 */
#ifdef R_TARGET_BOARD_EBISU
    .Port = R_WM_DISP_PORT_LVDS0,
#else /* R_TARGET_BOARD_SALVATORXS */
    .Port = R_WM_DISP_PORT_HDMI1,
#endif
};

static rvc_Layer_t loc_LayerCid =
{
    /* csi20 */
    .Port = R_WM_DISP_PORT_LVDS0,
};

#if defined(R_TARGET_BOARD_SALVATORXS) || defined(R_TARGET_BOARD_EBISU)
const rvc_Surface_t SurfaceCluster[] =
{ /* {ImageInWidth,ImageInHeight,ImageScaledWidth,ImageScaledHeight,PosX, PoxY, PosZ, Width, Height, StrideY, StrideC,PreClipPosX,PreClipPosY,     Uvaof, DataRate,Lanes,Interlaced,                  VmcInf,             VmcIm,                   ColorSpaceOut,                    BoundaryMbps,               Fmt, Handle, FBuf, FBufC0, FBufC1, Doc} */
#if defined(SCALE_IMAGE_CHANGE_USE)
    {        1920,         1080,          1920/2,           1080/2,   0,    0,    2,1920/2, 1080/2,1920/2*4,       0,           0,          0,        0,    900*4,    4,     false,    R_VIN_VMC_INF_RGB888, R_VMC_IM_ODD_EVEN,   R_ADV7482_COLOR_SPACE_OUT_RGB, R_ADV7482_BOUNDARY_OVER_600MBPS, R_WM_FMT_ARGB8888,      0,    0,      0,      0,   0}, /* (HDMI0,1080P)Change from 1920x1080 to 960x540 */
    {        1280,          720,            1920,             1080,   0,    0,    2,  1920,   1080,  1920*4,       0,           0,          0,        0,    450*4,    4,     false,R_VIN_VMC_INF_YUV8_BT601, R_VMC_IM_ODD_EVEN, R_ADV7482_COLOR_SPACE_OUT_YCRCB, R_ADV7482_BOUNDARY_UPTO_600MBPS, R_WM_FMT_ARGB8888,      0,    0,      0,      0,   0}, /* (HDMI0,720P)Change from 1280x720 to 1920x1080 */
    {        1920,         1080,          1920/2,           1080/2,   0,    0,    2,1920/2, 1080/2,1920/2*4,       0,           0,          0,        0,    450*4,    4,      true,    R_VIN_VMC_INF_RGB888,     R_VMC_IM_FULL,   R_ADV7482_COLOR_SPACE_OUT_RGB, R_ADV7482_BOUNDARY_UPTO_600MBPS, R_WM_FMT_ARGB8888,      0,    0,      0,      0,   0}, /* (HDMI0,1080I)Change from 1920x1080 to 960x540 */
    {        1280,          720,          1280/2,            720/2,   0,    0,    2,1280/2,  720/2,1280/2*4,       0,           0,          0,        0,    450*4,    4,     false,R_VIN_VMC_INF_YUV8_BT601, R_VMC_IM_ODD_EVEN, R_ADV7482_COLOR_SPACE_OUT_YCRCB, R_ADV7482_BOUNDARY_UPTO_600MBPS, R_WM_FMT_ARGB8888,      0,    0,      0,      0,   0}, /* (LVDS,720P)Change from 1280x720 to 640x360 */
#else
    {        1920,         1080,            1920,             1080,   0,    0,    2,  1920,    1080,   1920,    1920,           0,          0, 1920*1080,    450*4,    4,     false,R_VIN_VMC_INF_YUV8_BT601, R_VMC_IM_ODD_EVEN, R_ADV7482_COLOR_SPACE_OUT_YCRCB, R_ADV7482_BOUNDARY_UPTO_600MBPS, R_WM_FMT_YCBCR420,      0,    0,      0,      0,   0}, /* Settings for HDMI0 CSI40_720P */
    {        1920,         1080,            1920,             1080,   0,    0,    2,  1920,   1080,    1920,    1920,           0,          0,1920*1080,    900*4,    4,     false,    R_VIN_VMC_INF_RGB888, R_VMC_IM_ODD_EVEN,   R_ADV7482_COLOR_SPACE_OUT_RGB, R_ADV7482_BOUNDARY_OVER_600MBPS, R_WM_FMT_YCBCR420,      0,    0,      0,      0,   0}, /* Settings for HDMI0 CSI40_1080P */
    {        1280,          720,            1280,              720, 320,    0,    2,  1280,    720,    1280,    1280,           0,          0, 1280*720,    450*4,    4,     false,R_VIN_VMC_INF_YUV8_BT601, R_VMC_IM_ODD_EVEN, R_ADV7482_COLOR_SPACE_OUT_YCRCB, R_ADV7482_BOUNDARY_UPTO_600MBPS, R_WM_FMT_YCBCR420,      0,    0,      0,      0,   0}, /* Settings for HDMI0 CSI40_720P */
    {        1920,         1080,            1920,             1080,   0,    0,    2,  1920,   1080,    1920,    1920,           0,          0,1920*1080,    450*4,    4,      true,    R_VIN_VMC_INF_RGB888,     R_VMC_IM_FULL,   R_ADV7482_COLOR_SPACE_OUT_RGB, R_ADV7482_BOUNDARY_UPTO_600MBPS, R_WM_FMT_YCBCR420,      0,    0,      0,      0,   0}, /* Settings for HDMI0 CSI40_1080I */
    {        1280,          720,            1280,              720,   0,    0,    2,  1280,    720,    1280,    1280,           0,          0, 1280*720,    450*4,    4,     false,R_VIN_VMC_INF_YUV8_BT601, R_VMC_IM_ODD_EVEN, R_ADV7482_COLOR_SPACE_OUT_YCRCB, R_ADV7482_BOUNDARY_UPTO_600MBPS, R_WM_FMT_YCBCR420,      0,    0,      0,      0,   0}, /* Settings for LVDS CSI40_720P */
/*  {        1920,         1080,            1920,             1080,   0,    0,    2,  1920,   1080,  1920*4,       0,           0,          0,        0,    900*4,    4,     false,    R_VIN_VMC_INF_RGB888, R_VMC_IM_ODD_EVEN,   R_ADV7482_COLOR_SPACE_OUT_RGB, R_ADV7482_BOUNDARY_OVER_600MBPS, R_WM_FMT_ARGB8888,      0,    0,      0,      0,   0}, */
/*  {        1280,          720,            1280,              720, 320,    0,    2,  1280,    720,  1280*4,       0,           0,          0,        0,    450*4,    4,     false,R_VIN_VMC_INF_YUV8_BT601, R_VMC_IM_ODD_EVEN, R_ADV7482_COLOR_SPACE_OUT_YCRCB, R_ADV7482_BOUNDARY_UPTO_600MBPS, R_WM_FMT_ARGB8888,      0,    0,      0,      0,   0}, */
/*  {        xxxx,         xxxx,            xxxx,             xxxx, xxx,    0,    2,  xxxx,   xxxx,    xxxx,    xxxx,         xxx,        xxx,xxxx*xxxx,    xxx*4,    4,     false,    R_VIN_VMC_INF_XXXXXX, R_VMC_IM_ODD_EVEN,   R_ADV7482_COLOR_SPACE_OUT_XXX, R_ADV7482_BOUNDARY_XXXX_600MBPS, R_WM_FMT_XXXXXXXX,      0,    0,      0,      0,   0}, */
#endif
};

const rvc_Surface_t SurfaceCid[] =
{ /* {ImageInWidth,ImageInHeight,ImageScaledWidth,ImageScaledHeight,PosX, PoxY, PosZ, Width, Height, StrideY, StrideC,PreClipPosX,PreClipPosY,    Uvaof, DataRate,Lanes,Interlaced,                  VmcInf,         VmcIm, ColorSpaceOut, BoundaryMbps,               Fmt, Handle, FBuf, FBufC0, FBufC1, Doc} */
#if defined(SCALE_IMAGE_CHANGE_USE)
    {         720,          480,           720*2,            480*2, 240,   0,    2,  720*2,  480*2, 720*2*4,       0,          0,          0,        0,     216*1,   1,      true,R_VIN_VMC_INF_YUV8_BT601, R_VMC_IM_FULL,             0,            0, R_WM_FMT_ARGB8888,      0,    0,      0,      0,   0}, /* (HDMI1)Change from 720x480 to 1440x960 */
    {         720,          480,           720/2,            480/2,   0,   0,    3,  720/2,  480/2, 720/2*4,       0,          0,          0,        0,     216*1,   1,      true,R_VIN_VMC_INF_YUV8_BT601, R_VMC_IM_FULL,             0,            0, R_WM_FMT_ARGB8888,      0,    0,      0,      0,   0}, /* (DPAD)Change from 720x480 to 360x240 */
#else
    {         720,          480,             720,              480, 600,   0,    2,    720,    480,    1024,    1024,          0,          0, 1024*720,     216*1,   1,      true,R_VIN_VMC_INF_YUV8_BT601, R_VMC_IM_FULL,             0,            0, R_WM_FMT_YCBCR420,      0,    0,      0,      0,   0}, /* Settings for DPAD CSI20 */
/*  {         720,          480,             720,              480, 600,   0,    2,    720,    480,   720*4,       0,          0,          0,        0,     216*1,   1,      true,R_VIN_VMC_INF_YUV8_BT601, R_VMC_IM_FULL,             0,            0, R_WM_FMT_ARGB8888,      0,    0,      0,      0,   0}, */
/*  {         xxx,          xxx,             xxx,              xxx, xxx, xxx,    2,    xxx,    xxx,    xxxx,    xxxx,          x,          x,     xxxx,     216*1,   1,      true,R_VIN_VMC_INF_YUV8_BT601, R_VMC_IM_FULL,             0,            0, R_WM_FMT_YCBCR420,      0,    0,      0,      0,   0}, */
#endif
};
#else
#error No board or wrong board defined!
#endif

/**********************************************************************************************************************
 Exported global variables and functions
 *********************************************************************************************************************/
rvc_Config_t rvc_FastbootConfig =
{
    .ClusterDisplayPtr   = &loc_DispCluster,
    .CidDisplayPtr       = 0,
    .ClusterLayerPtr     = &loc_LayerCluster,
    .CidLayerPtr         = &loc_LayerCid,
    .settings_in_cluster = 0,
    .settings_in_cid     = 0
};

#endif /* defined(R_TARGET_BOARD_SALVATORXS) */
#endif /* FASTBOOT_CFG_APP_CAMERA_ENABLE */
