/*************************************************************************************************************
* cio_wm_ioctl_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_CIO_WM_IOCTL_H_
#define R_CIO_WM_IOCTL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief CIO Window Manager device ioctl header
 */

/**
 * @defgroup     IOCTL WM IOCTL
 * @ingroup cio_wm_driver
 * @brief       Declaration of the IOCTL for the CIO WM device
 * @{
 */

/**
 * Section: Global Functions
 */
/**
 * @brief       This function check the WM Device command and call the appropriate
 *              WM Drv API to execute the given request
 * @param[in]   Addr - Reference to the WM device
 * @param[in]   Par1 - pointer to a WM Msg received
 * @param[in]   Par2 - pointer to a WM Msg to send
 * @param[in]   Par3 - not used
 * @retval      0 if successful; -1 if failed
 */
ssize_t R_CIO_WM_PRV_IoCtl(size_t Addr, void *Par1, void *Par2, void *Par3);

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* R_CIO_WM_IOCTL_H_ */

