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
 * File Name    : dcu_sample.h
 * Version      : 1.0
 * Description  : List API functions of dcu sample demo app.
 *********************************************************************************************************************/
#ifndef DCU_SAMPLE_H
#define DCU_SAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "display.h"

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef struct {
    r_wm_DisplayPort_t DisplayPort;
    uint32_t           LayerZIndex;
    uint32_t           LayerPosX;
    uint32_t           LayerPosY;
    uint32_t           ImageWidth;
    uint32_t           ImageHeight;
    uint32_t           ImageNumber;
    const void **      ImageDataListRef;
    const uint32_t **  ImageSizeListRef;
    void *             FrameBuffer;
    uint32_t           RepeatCount;
    uint32_t           RepeatIntervalMs;
} dcu_Config_t;

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
void Dcu_Main(const dcu_Config_t * ConfigRef);

#ifdef __cplusplus
}
#endif

#endif /* DCU_SAMPLE_H */
