/*************************************************************************************************************
* i2c_5p49v6901_api_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_5P49V6901_API_H_
#define R_5P49V6901_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r_i2c_api.h"

/**
 * @struct  r_5p49v6901_Error_t
 * @brief  5P49V6901 Errors
 */
typedef enum {
    R_5P49V6901_ERR_SUCCESS,           /**< Success */
    R_5P49V6901_ERR_FAILED,            /**< failed */
    R_5P49V6901_ERR_INVALID_PARAMETER  /**< An input parameter is incorrect */
} r_5p49v6901_Error_t;

/**
 * Function R_5P49V6901_Init
 * @brief Initialise 5P49V6901
 *
 * @param[in] out_channel - out channel to be used for 5P49V6901.
 *
 * @retval    See  @ref r_5p49v6901_Error_t
 *
 */
r_5p49v6901_Error_t R_5P49V6901_Init(uint8_t out_channel);

#ifdef __cplusplus
}
#endif

#endif /* R_5P49V6901_API_H_ */
