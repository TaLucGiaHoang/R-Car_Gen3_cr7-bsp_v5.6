/*************************************************************************************************************
* logo_main
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/*******************************************************************************
  Title: Main Module

  This module contains the main and the main loop functions.
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_cio_api.h"
#include "logo_image.h"
#include "logo.h"

#include "config.h"
#include "display.h"

#include "r_print_api.h"

/*------------------------------*/
/* User defined output funciton */
/*------------------------------*/
int logo_main(const logo_Config_t *p_cfg)
{
    r_wm_Display_t        display;
    int         frame_buf_size;
    r_wm_Layer_t          layer;

    R_PRINT_Log("========= Logo Disply Start ============= \r\n");
    if (R_WM_ERR_SUCCESS != R_WM_DisplayConfig(p_cfg->DisplayPort, &display)) {
          R_PRINT_Log("[Logo] Failed to config display.\r\n");
          return(-1);
    }

    if (R_WM_ERR_SUCCESS != R_WM_DisplayInit(&display)) {
          R_PRINT_Log("[Logo] Failed to initialize display.\r\n");
          return(-1);
    }

    if (R_WM_ERR_SUCCESS != R_WM_DisplayEnable(&display, true)) {
          R_PRINT_Log("[Logo] Failed to enable display.\r\n");
          return(-1);
    }

    frame_buf_size = p_cfg->ImageWidth * p_cfg->ImageHeight * sizeof(uint32_t);
    R_OSAL_CacheFlush((int)p_cfg->FrameBuffer, frame_buf_size, R_OSAL_CACHE_D);

    layer.ParentPtr             = &display;
    layer.VoutSurface.PosX      = p_cfg->LayerPosX;
    layer.VoutSurface.PosY      = p_cfg->LayerPosY;
    layer.VoutSurface.PosZ      = p_cfg->LayerZIndex;
    layer.VoutSurface.Width     = p_cfg->ImageWidth;
    layer.VoutSurface.Height    = p_cfg->ImageHeight;
    layer.VoutSurface.Fmt       = R_WM_FMT_ARGB8888;
    layer.VoutSurface.Handle    = 0;
    layer.VoutSurface.FBuf      = (uintptr_t)p_cfg->FrameBuffer;
    layer.VoutSurface.FBufC0    = 0;
    layer.VoutSurface.FBufC1    = 0;
    layer.VoutSurface.Alpha     = 0;
    layer.VoutSurface.StrideY   = p_cfg->ImageWidth * 4;
    layer.VoutSurface.StrideC   = 0;
    if (R_WM_ERR_SUCCESS != R_WM_LayerInit(&layer)) {
        R_PRINT_Log("[Logo] Failed to initialize layer.\r\n");
        return(-1);
    }

    R_PRINT_Log("Logo disply Stop \r\n");
    return 0;
}
