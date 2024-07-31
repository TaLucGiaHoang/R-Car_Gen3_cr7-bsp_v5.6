/*************************************************************************************************************
* cio_wm_main_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_CIO_WM_MAIN_H_
#define R_CIO_WM_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Main WM main API
 * @ingroup cio_wm_driver
 * @brief       Definition of the functions initialising and de-initialising the
 *              the CIO WM device
 * @{
 */

/******************************************************************************
 * Section: Global Functions
 */


/**
 * @brief       initialise the CIO WM Device
 *  If the WM driver is listed in the BSP code, this function is called
 *  per default when the CIO server starts
 *  It check the current RCar device and set-up the VOut accordingly
 * @param[in]   Addr - represent the WM device to be initialised (Vout instance)
 * @param[in]   IrqNum - interrupt number. Not used
 * @retval      0 if successful; -1 if failed
 */
int R_CIO_WM_PRV_Init(size_t Addr, int IrqNum);

/**
 * @brief       De-initialise the CIO WM Device
 * @param[in]   Addr - represent the WM device to be initialised (Vout instance)
 * @retval      0 if successful; -1 if failed
 */
int R_CIO_WM_PRV_DeInit(size_t Addr);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* R_CIO_WM_MAIN_H_ */

