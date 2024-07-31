/****************************************************************************
PROJECT : VLIB (device and OS adaptation)
FILE    : $Id: r_davehe_driver.h 287 2018-03-13 12:23:51Z a5089413 $
============================================================================ 
DESCRIPTION
D/AVE HD Driver
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
  Title: r_davehd_driver.h

 D/AVE HD Driver Macro

*/
#ifndef R_DAVEHD_DRIVER_H_
#define R_DAVEHD_DRIVER_H_

/*
 * Macro definitions
 */

#define DHD_RESOURCE_PASSWORD    "!systempassword"
#define DHD_RESOURCE_CONNECTION  "DHD_Server"

#define DHD_RESOURCE_MEM_PASSWORD    "!systempassword"
#define DHD_RESOURCE_MEM_CONNECTION  "DHD_Mmap"

#define DHD_RESOURCE_SEM_PASSWORD   "!systempassword"
#define DHD_RESOURCE_SEM_MBI        "DHD_Server_MBI"


#define DHD_SERIAL_MAX_FILENAME_LENGTH 256
#define DHD_SERIAL_MAX_STRING_LENGTH   256



/* DaveHD configuration */

#ifndef DHD_RINGSIZE
    #define DHD_RINGSIZE            1024
#endif
#ifndef DHD_JOBSIZE
    /* Original for DHD_JOBSIZE was 512. Changed to compensate for the overhead of communicating
     * with the dhd kernel driver */
    #define DHD_JOBSIZE             (4* 8192) 
#endif
#ifndef DHD_JOBCOUNT
    #define DHD_JOBCOUNT            8
#endif

/* The chunk size specifies the maximum amount of data in bytes transfered in
 * a single call of UPLOAD and DOWNLOAD.
 */
#define DHD_CHUNK_SIZE          (1024*32)

/* This value is chosen based on an evaluation of dhd_drivertest plus a
 * huge safety margin. It should be easy to find the real maximum buffer size.
 */
#define DHD_COPY_BUFFER         ((1024*5) + DHD_CHUNK_SIZE)



#define DHD_COPY_BUFFER_SRV     (DHD_COPY_BUFFER)
/*
 * The maximum amount of clients that can be handled by the client and server
 * implementations.
 */
#define DHD_CLIENT_COUNT        12


/* Only show error messages as default */
#define R_DAVEHD_KERNEL_ERR_LOG(...) ((void)BSP_COMMON_KERNEL_LOG_ERROR(__VA_ARGS__))

#ifdef R_DAVEHD_DEBUG
#define R_DAVEHD_USER_INFO_LOG(...) ((void)BSP_COMMON_USER_LOG_ERROR(__VA_ARGS__))
#define R_DAVEHD_USER_VA_INFO_LOG(msg, va_list) BSP_COMMON_USER_LOG_VA_ERROR((msg), (va_list))
#define R_DAVEHD_USER_ERR_LOG(...) ((void)BSP_COMMON_USER_LOG_ERROR(__VA_ARGS__))
#define R_DAVEHD_USER_VA_ERR_LOG(msg, va_list)  BSP_COMMON_USER_LOG_VA_ERROR((msg), (va_list))
#elif defined R_DAVEHD_ERROR
#define R_DAVEHD_USER_ERR_LOG(...) ((void)BSP_COMMON_USER_LOG_ERROR(__VA_ARGS__))
#define R_DAVEHD_USER_VA_ERR_LOG(msg, va_list)  BSP_COMMON_USER_LOG_VA_ERROR((msg), (va_list))

#define R_DAVEHD_USER_INFO_LOG(...)
#define R_DAVEHD_USER_VA_INFO_LOG(msg, va_list)
#else
#define R_DAVEHD_USER_ERR_LOG(...)
#define R_DAVEHD_USER_VA_ERR_LOG(msg, va_list)
#define R_DAVEHD_USER_INFO_LOG(...)
#define R_DAVEHD_USER_VA_INFO_LOG(...)
#endif

#define DHD_HALT_ON_ERROR        0

/*******************************************************************************
 * Type definitions
 */
typedef enum {
    DHD_STATUS_RESET_HW   = 1,
    DHD_STATUS_SETUP_IRQ  = 2,
    DHD_STATUS_PROCESS_IRQ = 3,
    DHD_STATUS_MBI_OCCURRED = 4,
} DHD_SetStatus;

typedef enum {
    D_ALL,
    D_ERR,
    D_WARN,
    D_INFO,
} debug_level;

/* PRQA S 5209 2 *//* $Misra: Data type used for general-purpuose output. $*/
void debugf(debug_level Level, char* ModuleName, char* Message, ...);
void debugk(debug_level Level, char* ModuleName, const unsigned int CurrentLevel, char* Message, ...);

#endif /* R_DAVEHD_DRIVER_H_ */
