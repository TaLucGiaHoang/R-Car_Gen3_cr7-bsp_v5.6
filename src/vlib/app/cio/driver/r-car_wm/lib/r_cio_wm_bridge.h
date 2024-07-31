/*************************************************************************************************************
* cio_wm_bridge
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_CIO_WM_BRIDGE_H_
#define R_CIO_WM_BRIDGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "r_wm_config.h"

/**@defgroup group1_wm CIO WM Driver Declaration
 * @ingroup cio_wm_driver
 *
 * @brief Bridge for the CIO server.
 *
 * Declaration of the WM driver constant. This is the only information that needs to be exposed to the CIO server

 * @{
 */

/**
 * Specify the available displays and their resolutions for this board.
 * This function is required by the WM driver lib
 *
 * Parameters:
 *  @param[in]  DuUnit - Display unit reference
 *  @param[in/out] Timings - Display information
 *     All fields except DotClkIn[] and Extal are set by this function.
 *  @return==0 - success
 *  @return!=0 - failed
 */
uint32_t R_CIO_BSP_WM_PRV_VideoOutDisplays(uint32_t DuUnit, r_wm_DispTiming_t *Timings);

/**
 * Initialise the Video output for the defined board. This function is
 * required by the WM driver lib
 *
 * Parameters:
 *  @param[in]  DuUnit - Display unit reference
 *  @param[in/out]  Timings - Display information
 *     DotClkIn[] and Extal fields are set by this function.
 *  @return==0 - success
 *  @return!=0 - failed
 */
uint32_t R_CIO_BSP_WM_PRV_VideoOutInit(uint32_t DuUnit, r_wm_DispTiming_t *Timings);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /*R_CIO_BSP_R-CARX3_CR7_VOUT_H_ */


