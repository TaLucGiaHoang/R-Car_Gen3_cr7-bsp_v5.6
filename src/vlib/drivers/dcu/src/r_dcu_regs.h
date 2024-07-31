
/**
PROJECT : CR7 RGL
FILE   : $Id: r_dcu_regs.h
Created by :
Version :
Update :
  - 1.12.2020 created
  - 9.12.2020 Clean-up
============================================================================
DESCRIPTION
DCU driver Register definitions
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
#ifndef R_VIN_REGS_H_
#define R_VIN_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RCAR_DCU_BASE  0xE7901000
#define RCAR_DCU_IAR   0x00
#define RCAR_DCU_OAR   0x04
#define RCAR_DCU_DCR   0x08
#define RCAR_DCU_IASR  0x0C
#define RCAR_DCU_OASR  0x10
#define RCAR_DCU_DCSR  0x14
#define RCAR_DCU_MCR   0x18
#define RCAR_DCU_CMDR  0x1C
#define RCAR_DCU_STSR  0x24
#define RCAR_DCU_CSR   0x28
#define RCAR_DCU_INTSR 0x800
#define RCAR_DCU_INTCR 0x804
#define RCAR_DCU_INTER 0x808
#define RCAR_DCU_RSTR  0x80C

#ifdef __cplusplus
}
#endif

#endif /* R_DCU_REGS_H_ */
