/****************************************************************************
PROJECT : VLIB (device and OS adaptation)
FILE    : $Id: r_dhd_osal_resource.h 287 2018-03-13 12:23:51Z a5089413 $
============================================================================ 
DESCRIPTION
Malloc wrapper 
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

/****************************************************************************
  Title: r_dhd_osal_resource.h

 OSAL resource

*/
#ifndef R_DHD_OSAL_RESOURCE_H_
#define R_DHD_OSAL_RESOURCE_H_

#define RESOURCE_ID_2DG                              (0x9000U)

#define    OSAL_THREAD_DHD_KERNEL                    (RESOURCE_ID_2DG   + 0U)
#define    OSAL_THREAD_DHD_INTERRUPT00               (RESOURCE_ID_2DG   + 1U)

#define    OSAL_MUTEX_DHD_SYNC_INTERRUPT             (RESOURCE_ID_2DG   + 0U)
#define    OSAL_MUTEX_DHD_ERROR_INTERRUPT            (RESOURCE_ID_2DG   + 1U)
#define    OSAL_MUTEX_DHD_STOP_INTERRUPT             (RESOURCE_ID_2DG   + 2U)
#define    OSAL_MUTEX_DHD_WAKEUP_SERVER              (RESOURCE_ID_2DG   + 3U)
#define    OSAL_MUTEX_DHD_LOCK_DEVICE                (RESOURCE_ID_2DG   + 4U)
#define    OSAL_MUTEX_DHD_LOCK_ALLOCATOR             (RESOURCE_ID_2DG   + 5U)

#define    OSAL_COND_DHD_WAKEUP_SERVER               (RESOURCE_ID_2DG  +  0U)

#define    OSAL_MQ_DHD_SERVER                         (RESOURCE_ID_2DG  +  0U)
#define    OSAL_MQ_DHD_CLIENT_00                      (RESOURCE_ID_2DG  +  1U)
#define    OSAL_MQ_DHD_CLIENT_13                      (RESOURCE_ID_2DG  + 14U)
#define    OSAL_MQ_DHD_CLIENT_14                      (RESOURCE_ID_2DG  + 15U)
#define    OSAL_MQ_DHD_CLIENT_15                      (RESOURCE_ID_2DG  + 16U)
#define    OSAL_MQ_DHD_CLIENT_16                      (RESOURCE_ID_2DG  + 17U)
#define    OSAL_MQ_DHD_CLIENT_17                      (RESOURCE_ID_2DG  + 18U)
#define    OSAL_MQ_DHD_CLIENT_18                      (RESOURCE_ID_2DG  + 19U)
#define    OSAL_MQ_DHD_CLIENT_19                      (RESOURCE_ID_2DG  + 20U)
#define    OSAL_MQ_DHD_CLIENT_20                      (RESOURCE_ID_2DG  + 21U)
#define    OSAL_MQ_DHD_CLIENT_21                      (RESOURCE_ID_2DG  + 22U)
#define    OSAL_MQ_DHD_CLIENT_22                      (RESOURCE_ID_2DG  + 23U)
#define    OSAL_MQ_DHD_CLIENT_23                      (RESOURCE_ID_2DG  + 24U)
#define    OSAL_MQ_DHD_CLIENT_24                      (RESOURCE_ID_2DG  + 25U)

#endif /* R_DHD_OSAL_RESOURCE_H_ */
