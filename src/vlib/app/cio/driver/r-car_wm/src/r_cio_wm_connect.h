/*************************************************************************************************************
* cio_wm_connect_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_CIO_WM_CONNECT_H_
#define R_CIO_WM_CONNECT_H_

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @defgroup Connect WM Connections
 * @ingroup cio_wm_driver
 * @brief       Declaration of the connection functions for the CIO WM device
 * @{
 */

/**
 * @brief       This function register a new channel to the given WM Device
 *
 * @param[in]   Addr - Reference to the WM device
 * @param[in]   Par1 - not used
 * @param[in]   Par2 - not used
 * @param[in]   Par3 - not used
 * @retval      0 if successful; -1 if failed
 */
int R_CIO_WM_PRV_Open(size_t Addr, void *Par1, void *Par2, void *Par3);


/**
 * @brief       This function de-register a channel to the given WM device
 *              If there are still open channels, it returns 1 through Par1.
 *              If Par1=1 is returned the WM device shall be de-initialised
 * @param[in]   Addr - Reference to the WM device
 * @param[out]  Par1 - 0 if the WM device has no connection, else 1.
 * @param[in]   Par2 - not used
 * @param[in]   Par3 - not used
 * @retval      0 if successful; -1 if failed
 */
int R_CIO_WM_PRV_Close(size_t Addr, void *Par1, void *Par2, void *Par3);

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* R_CIO_WM_CONNECT_H_ */

