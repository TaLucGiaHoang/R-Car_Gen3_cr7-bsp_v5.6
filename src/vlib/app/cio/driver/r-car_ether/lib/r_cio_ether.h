/*************************************************************************************************************
* cio_ether
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/



/**@defgroup group1_ether CIO ETHER Driver Declaration
 * @ingroup cio_eth_driver
 *
 * @brief Bridge for the CIO server.
 *
 * Declaration of the Ether driver constant. This is the only information that needs to be exposed to the CIO server

 * @{
 */

#ifndef R_CIO_ETHER_H
#define R_CIO_ETHER_H

#ifdef __cplusplus
extern "C" {
#endif

/** R_ETHER_Driver
 * @brief This is the structure providing access to the Ether driver for the CIO server.
 */
extern const r_cio_Driver_t R_CIO_ETHER_Driver;

#ifdef __cplusplus
}
#endif

#endif

/** @} */
