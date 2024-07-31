/*************************************************************************************************************
* cio_wm
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/


#ifndef R_CIO_WM_H_
#define R_CIO_WM_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @file        r_cio_wm.h
 * @brief       Declaration of the CIO WM device
 */

#include "r_cio_bridge.h"

/******************************************************************************
 * Section: Global variables
 */

/**
 * @brief The WM CIO Driver of type contains the functions
 * pointer related to the WM Device.
 */
extern const r_cio_Driver_t WmDriver;


#ifdef __cplusplus
}
#endif

#endif /* R_CIO_WM_H_ */
