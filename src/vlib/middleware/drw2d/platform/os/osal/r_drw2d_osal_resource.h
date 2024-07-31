/*
****************************************************************************
PROJECT : DRW2D
FILE    : r_drw2d_osal_resource.h 13650 2017-06-02 11:25:04Z florian.zimmermann $
============================================================================ 
DESCRIPTION
DRW2D OSAL resource
============================================================================
                            C O P Y R I G H T                            
============================================================================
                           Copyright (c) 2022
                                  by 
                    Renesas Electronics (Europe) GmbH. 
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
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
Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.     

****************************************************************************
*/

#ifndef R_DRW2D_OSAL_RESOURCE_H_
#define R_DRW2D_OSAL_RESOURCE_H_

#define RESOURCE_ID_2DG      0x9000U

#define    OSAL_MUTEX_DRW2D_LOCK_API_00              (RESOURCE_ID_2DG   + 6U)
#define    OSAL_MUTEX_DRW2D_LOCK_API_01              (RESOURCE_ID_2DG   + 7U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_00        (RESOURCE_ID_2DG   + 8U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_01        (RESOURCE_ID_2DG   + 9U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_02        (RESOURCE_ID_2DG   + 10U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_03        (RESOURCE_ID_2DG   + 11U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_04        (RESOURCE_ID_2DG   + 12U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_05        (RESOURCE_ID_2DG   + 13U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_06        (RESOURCE_ID_2DG   + 14U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_07        (RESOURCE_ID_2DG   + 15U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_08        (RESOURCE_ID_2DG   + 16U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_09        (RESOURCE_ID_2DG   + 17U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_10        (RESOURCE_ID_2DG   + 18U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_11        (RESOURCE_ID_2DG   + 19U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_12        (RESOURCE_ID_2DG   + 20U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_13        (RESOURCE_ID_2DG   + 21U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_14        (RESOURCE_ID_2DG   + 22U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_15        (RESOURCE_ID_2DG   + 23U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_16        (RESOURCE_ID_2DG   + 24U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_17        (RESOURCE_ID_2DG   + 25U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_18        (RESOURCE_ID_2DG   + 26U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_19        (RESOURCE_ID_2DG   + 27U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_20        (RESOURCE_ID_2DG   + 28U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_21        (RESOURCE_ID_2DG   + 29U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_22        (RESOURCE_ID_2DG   + 30U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_23        (RESOURCE_ID_2DG   + 31U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_24        (RESOURCE_ID_2DG   + 32U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_25        (RESOURCE_ID_2DG   + 33U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_26        (RESOURCE_ID_2DG   + 34U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_27        (RESOURCE_ID_2DG   + 35U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_28        (RESOURCE_ID_2DG   + 36U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_29        (RESOURCE_ID_2DG   + 37U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_30        (RESOURCE_ID_2DG   + 38U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_31        (RESOURCE_ID_2DG   + 39U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_32        (RESOURCE_ID_2DG   + 40U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_33        (RESOURCE_ID_2DG   + 41U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_34        (RESOURCE_ID_2DG   + 42U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_35        (RESOURCE_ID_2DG   + 43U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_36        (RESOURCE_ID_2DG   + 44U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_37        (RESOURCE_ID_2DG   + 45U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_38        (RESOURCE_ID_2DG   + 46U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_39        (RESOURCE_ID_2DG   + 47U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_40        (RESOURCE_ID_2DG   + 48U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_41        (RESOURCE_ID_2DG   + 49U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_42        (RESOURCE_ID_2DG   + 50U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_43        (RESOURCE_ID_2DG   + 51U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_44        (RESOURCE_ID_2DG   + 52U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_45        (RESOURCE_ID_2DG   + 53U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_46        (RESOURCE_ID_2DG   + 54U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_47        (RESOURCE_ID_2DG   + 55U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_48        (RESOURCE_ID_2DG   + 56U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_49        (RESOURCE_ID_2DG   + 57U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_50        (RESOURCE_ID_2DG   + 58U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_51        (RESOURCE_ID_2DG   + 59U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_52        (RESOURCE_ID_2DG   + 60U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_53        (RESOURCE_ID_2DG   + 61U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_54        (RESOURCE_ID_2DG   + 62U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_55        (RESOURCE_ID_2DG   + 63U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_56        (RESOURCE_ID_2DG   + 64U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_57        (RESOURCE_ID_2DG   + 65U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_58        (RESOURCE_ID_2DG   + 66U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_59        (RESOURCE_ID_2DG   + 67U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_60        (RESOURCE_ID_2DG   + 68U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_61        (RESOURCE_ID_2DG   + 69U)
#define    OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_62        (RESOURCE_ID_2DG   + 70U)

#endif /* R_DRW2D_OSAL_RESOURCE_H_ */
