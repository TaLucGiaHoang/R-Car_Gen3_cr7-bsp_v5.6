/*************************************************************************************************************
* cio_wm_status_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_CIO_WM_STATUS_H_
#define R_CIO_WM_STATUS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup     Status WM device status API
 * @ingroup cio_wm_driver
 * @brief       Declaration of the CIO WM device Status function
 * @{
 */



/**
 * Section: Global Functions
 */
/**
 * @brief       This function shall the WM device status. It is
 *              not implemented.
 * @param[in]   Addr - Reference to the WM device
 * @param[in]   Par1 - not used
 * @param[in]   Par2 - not used
 * @param[in]   Par3 - not used
 * @retval      0 if successful;
 */
ssize_t R_CIO_WM_PRV_Status(size_t Addr, void *Par1, void *Par2, void *Par3);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* R_CIO_WM_STATUS_H_ */

