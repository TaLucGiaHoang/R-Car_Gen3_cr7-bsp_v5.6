/**
PROJECT : CR7 RGL
FILE   : $Id: r_dcu_api.c
Created by :
Version :
Update :
  - 1.12.2020 created
  - 9.12.2020 Clean-up
============================================================================
DESCRIPTION
DCU driver API functions
============================================================================
                            C O P Y R I G H T
============================================================================
                      Copyright (c) 2019 - 2020
                                  by
                       Renesas Electronics (Beijing)
============================================================================
Purpose: to be used as sample S/W

DISCLAIMER
This software is supplied by Renesas Electronics Corporation and is only
intended for use with Renesas products. No other uses are authorized. This
software is owned by Renesas Electronics Corporation and is protected under
all applicable laws, including copyright laws.
THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
Renesas reserves the right, without notice, to make changes to this software
and to discontinue the availability of this software. By using this software,
you agree to the additional terms and conditions found by accessing the
following link:
http://www.renesas.com/disclaimer *
Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
*/

#include <stdint.h>
#include "r_io.h"
#include "r_dcu_api.h"
#include "r_cpg_api.h"
#include "r_dcu_regs.h"

#define MAKEWORD(a, b) ((((((a)) & 0xff)) | (((((b)) & 0xff))) << 8))
/*
DCU_MD
BIT 2 to 0

000: Input data format: ARGB8888, length: 32bit
001: Input data format: ARGB8888, length: 32bit (A swapping)
010: Input data format: ARGB8888, length:16bit
011: Input data format: ARGB8888, length：16bit (A swapping)
100: Input data format: RGB565/RGBA4444/RGBA5551, length: 16bit
101: Input data format: A8, length: 24bit
110: Input data format: RGB888, length: 8bit
111: Setting prohibited
*/

#define MCR_MD_ARGB8888_32BIT       (0 << 0)
#define MCR_MD_ARGB8888_32BIT_A     (1 << 0)
#define MCR_MD_ARGB8888_16BIT       (2 << 0)
#define MCR_MD_ARGB8888_16BIT_A     (3 << 0)
#define MCR_MD_RGB565_16BIT         (4 << 0)
#define MCR_MD_A8_24BIT             (5 << 0)
#define MCR_MD_RGB888_8BIT          (6 << 0)
#define MCR_CS_EN                   (1 << 4)
#define MCR_CS_DE                   (0 << 4)
#define MCR_CS_MD_8BIT              (0 << 5)
#define MCR_CS_MD_16BIT             (1 << 5)
#define MCR_CS_MD_32BIT             (2 << 5)
#define MCR_CS_MD_DE                (3 << 5)

#define INTER_ERR_EN                (1 << 1)
#define INTER_CMP_EN                (1 << 0)

#define INTCR_CMP_CLR               (1 << 0)
#define INTCR_ERR_CLR               (1 << 1)

#define CMDR_STT_ACT                (1 << 0)

#define FMT_MD_ARGB8888_32BIT       MAKEWORD(R_DCU_FMT_ARGB8888, R_DCU_LENGTH_32BIT)
#define FMT_MD_ARGB8888_16BIT       MAKEWORD(R_DCU_FMT_ARGB8888, R_DCU_LENGTH_16BIT)
#define FMT_MD_RGB565_16BIT         MAKEWORD(R_DCU_FMT_RGB565, R_DCU_LENGTH_16BIT)
#define FMT_MD_A8_24BIT             MAKEWORD(R_DCU_FMT_A8, R_DCU_LENGTH_24BIT)
#define FMT_MD_RGB888_8BIT          MAKEWORD(R_DCU_FMT_RGB888,R_DCU_LENGTH_8BIT)

#define INTSR_TIMEOUT               1000

static void R_DCU_PRV_WriteReg(uint32_t value, uint32_t offset)
{
    writel(value, RCAR_DCU_BASE + offset);
}

static uint32_t R_DCU_PRV_ReadReg(uint32_t offset)
{
    return readl(RCAR_DCU_BASE + offset);
}

uint32_t R_DCU_Start(void)
{
    R_DCU_PRV_WriteReg(INTER_CMP_EN, RCAR_DCU_INTER);
    R_DCU_PRV_WriteReg(CMDR_STT_ACT, RCAR_DCU_CMDR);

    return R_DCU_SUCCESS;
}

uint32_t R_DCU_Stop(void)
{
    R_PMA_CPG_SetClockStopStat(R_CPG_DCU_CLOCK, true);
    return R_DCU_SUCCESS;
}

bool R_DCU_CheckComplete(void)
{
    bool ret = false;

    if (R_DCU_PRV_ReadReg(RCAR_DCU_INTSR) & 0x1) {
        R_DCU_PRV_WriteReg(INTCR_CMP_CLR, RCAR_DCU_INTCR);
        ret = true;
    }

    return ret;
}

uint32_t R_DCU_Config(r_dcu_Config_t * pconf)
{
    uint32_t mcr = 0;
    switch (MAKEWORD(pconf->fmt, pconf->length)) {
    case FMT_MD_ARGB8888_32BIT:
        mcr = MCR_MD_ARGB8888_32BIT;
        if(pconf->swap)
            mcr = MCR_MD_ARGB8888_32BIT_A;
        break;
    case FMT_MD_ARGB8888_16BIT:
        mcr = MCR_MD_ARGB8888_16BIT;
        if(pconf->swap)
            mcr = MCR_MD_ARGB8888_16BIT_A;
        break;
    case FMT_MD_RGB565_16BIT:
        mcr = MCR_MD_RGB565_16BIT;
        break;
    case FMT_MD_A8_24BIT:
        mcr = MCR_MD_A8_24BIT;
        break;
    case FMT_MD_RGB888_8BIT:
        mcr = MCR_MD_RGB888_8BIT;
        break;
    default:
        return R_DCU_ERR_FMT;
    }

    mcr |= MCR_CS_DE;
    mcr |= MCR_CS_MD_DE;
    R_DCU_PRV_WriteReg(mcr, RCAR_DCU_MCR);

    if(pconf->input_addr & 0x1f)
        return R_DCU_ERR_ADDR;

    if(pconf->output_addr & 0x1f)
        return R_DCU_ERR_ADDR;

    pconf->input_size += 8 - (pconf->input_size % 8);
    R_DCU_PRV_WriteReg(pconf->input_addr, RCAR_DCU_IAR);
    R_DCU_PRV_WriteReg(pconf->input_size, RCAR_DCU_DCR);
    R_DCU_PRV_WriteReg(pconf->output_addr, RCAR_DCU_OAR);

    return R_DCU_SUCCESS;

}

uint32_t R_DCU_Init(void)
{
    R_PMA_CPG_SetClockStopStat(R_CPG_DCU_CLOCK, false);
    return R_DCU_SUCCESS;
}
