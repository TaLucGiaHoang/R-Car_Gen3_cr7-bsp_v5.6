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
 * File Name    : dcu_sample_main.c
 * Version      : 1.0
 * Description  : Demonstrate how to use a DCU device.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdint.h>
#include "rcar-xos/osal/r_osal_api.h"
#include "r_dcu_api.h"
#include "r_print_api.h"
#include "dcu_sample.h"
#include "display.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* return result */
#define DCU_SAMPLE_OK  (0)
#define DCU_SAMPLE_ERR (1)

#define DCU_DISP_SYNC_WAIT (1)

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: Dcu_Main
 * Description  : main function of dcu sample app.
 * Arguments    : ConfigRef - Pointer to app configuration.
 * Return Value : void
 *********************************************************************************************************************/
void Dcu_Main(const dcu_Config_t * ConfigRef)
{
    int32_t              result = DCU_SAMPLE_OK;
    const dcu_Config_t * p_cfg;
    r_wm_Display_t       display;
    r_wm_Layer_t         layer;
    r_dcu_Config_t       conf;
    uint32_t             repeat_cnt;
    int32_t              img_idx;

    /* Get app config paramters */
    p_cfg = ConfigRef;
    if (!p_cfg) {
        result = DCU_SAMPLE_ERR;
    }

    R_PRINT_Log("=== DCU Sample Test ===\n");

    /* Initialize display and layer */
    R_PRINT_Log("[DCU] Initialize display and layer...\r\n");
    if (DCU_SAMPLE_OK == result) {
        if (R_WM_ERR_SUCCESS != R_WM_DisplayConfig(p_cfg->DisplayPort, &display)) {
            result = DCU_SAMPLE_ERR;
            R_PRINT_Log("[DCU] Failed to config display.\r\n");
        }
    }
    if (DCU_SAMPLE_OK == result) {
        display.BackgroundColor = 0xffffffff; /* white */
        if (R_WM_ERR_SUCCESS != R_WM_DisplayInit(&display)) {
            result = DCU_SAMPLE_ERR;
            R_PRINT_Log("[DCU] Failed to initialize display.\r\n");
        }
        else {
            if (R_WM_ERR_SUCCESS != R_WM_DisplayEnable(&display, true)) {
                result = DCU_SAMPLE_ERR;
                R_PRINT_Log("[DCU] Failed to enable display.\r\n");
            }
        }
    }
    if (DCU_SAMPLE_OK == result) {
        layer.ParentPtr             = &display;
        layer.VoutSurface.PosX      = p_cfg->LayerPosX;
        layer.VoutSurface.PosY      = p_cfg->LayerPosY;
        layer.VoutSurface.PosZ      = p_cfg->LayerZIndex;
        layer.VoutSurface.Width     = p_cfg->ImageWidth;
        layer.VoutSurface.Height    = p_cfg->ImageHeight;
        layer.VoutSurface.Fmt       = R_WM_FMT_ARGB8888;
        layer.VoutSurface.Handle    = 0;
        layer.VoutSurface.FBuf      = 0;
        layer.VoutSurface.FBufC0    = 0;
        layer.VoutSurface.FBufC1    = 0;
        layer.VoutSurface.StrideY   = 0x0E10;
        layer.VoutSurface.StrideC   = 0;
        if (R_WM_ERR_SUCCESS != R_WM_LayerInit(&layer)) {
            result = DCU_SAMPLE_ERR;
            R_PRINT_Log("[DCU] Failed to initialize layer.\r\n");
        }
    }

    /* Decompress image data by DCU */
    if (DCU_SAMPLE_OK == result) {
        R_PRINT_Log("[DCU] Start decompress...\r\n");
        R_DCU_Init();
        conf.swap        = 0;
        conf.length      = R_DCU_LENGTH_16BIT;
        conf.fmt         = R_DCU_FMT_ARGB8888;
        conf.output_addr = (uint32_t)p_cfg->FrameBuffer;

        repeat_cnt = p_cfg->RepeatCount;
        img_idx    = 0;
        while (repeat_cnt > 0) {
            conf.input_addr = (uint32_t)p_cfg->ImageDataListRef[img_idx];
            conf.input_size = *p_cfg->ImageSizeListRef[img_idx];
            if (R_DCU_SUCCESS == R_DCU_Config(&conf)) {
                R_DCU_Start();
                while (!R_DCU_CheckComplete()) {
                    R_OSAL_ThreadSleepForTimePeriod(1);
                }

                /* update display */
                if (R_WM_ERR_SUCCESS != R_WM_LayerFlush(&layer, p_cfg->FrameBuffer, DCU_DISP_SYNC_WAIT)) {
                    result = DCU_SAMPLE_ERR;
                    R_PRINT_Log("[DCU] Failed in flushing layer!\r\n");
                }

                /* update image */
                img_idx++;
                if (img_idx >= p_cfg->ImageNumber) {
                    img_idx = 0;
                    repeat_cnt--;
                    R_OSAL_ThreadSleepForTimePeriod(p_cfg->RepeatIntervalMs);
                }
            }
            else {
                result = DCU_SAMPLE_ERR;
                R_PRINT_Log("[DCU] Failed in decompressing!\r\n");
            }

            if (DCU_SAMPLE_OK != result) {
                break;
            }
        }

        /* release display layer */
        R_WM_LayerShow(&layer, false);
    }

    R_PRINT_Log("[DCU] Terminated...\r\n");
}
/***********************************************************************************************************************
 End of function Dcu_Main
 **********************************************************************************************************************/
