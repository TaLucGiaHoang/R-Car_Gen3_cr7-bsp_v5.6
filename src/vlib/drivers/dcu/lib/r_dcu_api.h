/**
PROJECT : CR7 RGL
FILE   : $Id: r_dcu_api.h
Created by :
Version :
Update :
  - 1.12.2020 created
  - 9.12.2020 Clean-up
============================================================================
DESCRIPTION
DCU driver access API
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

#ifndef R_DCU_API_H_
#define R_DCU_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * \defgroup DCUgroup   DCU driver
 * @ingroup drivers
 * @brief DCU(DeCompression Unit) driver interface
 * @{
 * @}
 */

/**
 * \defgroup DCUtypes   Data Types
 * @ingroup DCUgroup
 * @{
 */

/**
 * @enum    r_dcu_Error_t
 * @brief   list of errors DCU API can return
 */
typedef enum {
    R_DCU_SUCCESS,                   /**< success */
    R_DCU_ERR_FMT,
    R_DCU_ERR_ADDR,
    R_DCU_ERR_TIMEOUT,
} r_dcu_Error_t;

/**
 * @enum    r_dcu_ColorFmt_t
 * @brief   list of color formats DCU API supports
 */
typedef enum {
    R_DCU_FMT_ARGB8888,  /**< ARGB 32-bit color format */
    R_DCU_FMT_RGB888,    /**< RGB 24-bit color format */
    R_DCU_FMT_RGB565,     /**< RGB 18-bit color format */
    R_DCU_FMT_A8,
} r_dcu_ColorFmt_t;

/**
 * @enum    r_dcu_Length_t
 * @brief   list of supported continuous pixel count lengths
 */
typedef enum {
    R_DCU_LENGTH_32BIT,
    R_DCU_LENGTH_24BIT,
    R_DCU_LENGTH_16BIT,
    R_DCU_LENGTH_8BIT,
}r_dcu_Length_t;

/**
 * @struct r_dcu_Config_t
 * @brief  DCU configuration parameters
 */
typedef struct {
    int swap;
    uint32_t input_addr;
    uint32_t input_size;
    uint32_t output_addr;
    r_dcu_ColorFmt_t fmt;
    r_dcu_Length_t length;
} r_dcu_Config_t;

/** @} data types */
/**
 * \defgroup DCUapi   API
 * @ingroup DCUgroup
 * @{
 */

/**
 * Function R_DCU_Start
 * @brief Start Decompression
 */
uint32_t R_DCU_Start(void);

/**
 * Function R_DCU_CheckComplete
 * @brief Check if DCU completion flag is set
 * @retval true=complete, otherwise=false
 */
bool R_DCU_CheckComplete(void);

/**
 * Function R_DCU_Stop
 * @brief Disables DCU (turns OFF DCU clock)
 * @retval always returns @ref R_DCU_SUCCESS
 */
uint32_t R_DCU_Stop(void);

/**
 * Function R_DCU_Init
 * @brief Enables DCU (turns ON DCU clock)
 * @retval always returns @ref R_DCU_SUCCESS
 */
uint32_t R_DCU_Init(void);

/**
 * Function R_DCU_Init
 * @brief Enables DCU (turns ON DCU clock)
 * @param[in] pconf     - DCU configration, see @ref r_duc_Config_t
 * @retval See @ref r_dcu_Error_t
 */
uint32_t R_DCU_Config(r_dcu_Config_t * pconf);

/** @} api */

#ifdef __cplusplus
}
#endif

#endif /* R_AUDIO_CORE_H_ */