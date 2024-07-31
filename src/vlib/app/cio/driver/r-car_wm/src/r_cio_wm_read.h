/*************************************************************************************************************
* cio_wm_read_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_CIO_WM_READ_H_
#define R_CIO_WM_READ_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup     Read WM device read API
 * @ingroup cio_wm_driver
 * @file        r_cio_wm_read.h
 * @brief       Declaration of the CIO WM device Read function
 * @{
 */



/**
 * Section: Global Functions
 */
/**
 * @brief       This function shall read data specific to the WM device. It is
 *              not implemented.
 * @param[in]   Addr - Reference to the WM device
 * @param[in]   Par1 - not used
 * @param[in]   Par2 - not used
 * @param[in]   Par3 - not used
 * @retval      0 successful
 */
ssize_t R_CIO_WM_PRV_Read(size_t Addr, void *Par1, void *Par2, void *Par3);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* R_CIO_WM_READ_H_ */

