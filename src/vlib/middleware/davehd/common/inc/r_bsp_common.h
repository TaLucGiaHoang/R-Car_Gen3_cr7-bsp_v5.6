/*******************************************************************************
 * File Name   : r_bsp_common.h
 * Version     : 0.1
 * Description : BSP Common functions.
 ******************************************************************************/
/*****************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.06.2016 0.01    First Release
 ******************************************************************************/

/*******************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only
 * intended for use with Renesas products. No other uses are authorized. This
 * software is owned by Renesas Electronics Corporation and is protected under
 * all applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
 * LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
 * TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
 * ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
 * FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
 * ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software
 * and to discontinue the availability of this software. By using this software,
 * you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
 *******************************************************************************/

#ifndef R_BSP_COMMON_H
#define R_BSP_COMMON_H

#include "r_print_api.h"

/******************************************************************************
 Macro definitions
 ******************************************************************************/

 /*Drivers in kernel space will log error and info messages using these Macros.
 If a user does not want to use the bsp_common_console_log mechannism they can
 change these macros.*/
 #define BSP_COMMON_KERNEL_LOG_ERROR(...)   (bsp_common_console_log(__VA_ARGS__))
 
 /*Drivers in user space will log error and info messages using these Macros.
 Using this common macro makes it easier for a user to change this behaviour.*/
 #define BSP_COMMON_USER_LOG_ERROR(...)     ((void)R_PRINT_Log(__VA_ARGS__))
 #define BSP_COMMON_USER_LOG_INFO(...)      ((void)R_PRINT_Log(__VA_ARGS__))
 
 
 /*Drivers in user space will log error and info messages using these Macros.
 Using this common macro makes it easier for a user to change this behaviour.*/
 #define BSP_COMMON_USER_LOG_VA_ERROR(msg, va_list)     (void)vsnprintf(buff, DHD_MAX_DEBUG_MESSAGE_SIZE, (msg), (va_list));  \
                                                        (void)R_PRINT_Log(buff)
 #define BSP_COMMON_USER_LOG_VA_INFO(msg, va_list)      (void)vsnprintf(buff, DHD_MAX_DEBUG_MESSAGE_SIZE, (msg), (va_list));  \
                                                        (void)R_PRINT_Log(buff)

 /******************************************************************************
 Functions
 ******************************************************************************/
void bsp_common_console_log(const char *format, ...);

#endif
