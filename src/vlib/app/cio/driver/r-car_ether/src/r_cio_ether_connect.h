/*************************************************************************************************************
* cio_ether_connect_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/**@defgroup group2_ether Ether Driver Connect Functions
 * @ingroup cio_eth_driver
 *
 * @brief Methods used by the CIO Server to initiate or finish communicatrion with the
 * Ether module
 *
 * Functions which a the CIO Server will call in order to initiate a connection
 * between a user applcation and the Ether macro.
 *
 * @{
 */

#ifndef R_CIO_ETHER_CONNECT_H
#define  R_CIO_ETHER_CONNECT_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
   Global (private) functions
 */

/*******************************************************************************
 *  Function Name: R_CIO_ETHER_PRV_Open
 */
/**
 * @brief ETHER internal Open function.
 *
 * Starts the ETHER (receive) operation.
 *
 * @param[in] Addr       - Channel id of the given ETHER instance
 * @param[in] Par1       - Optional parameter (unused)
 * @param[in] Par2       - Optional parameter (unused)
 * @param[in] Par3       - optional parameter (unused)
 *
 * @return == 0 - OK
 */
int R_CIO_ETHER_PRV_Open(size_t Addr, void *Par1, void *Par2, void *Par3);

/*******************************************************************************
 *  Function Name: R_CIO_ETHER_PRV_Close
 */
/**
 * @brief ETHER internal Open function.
 *
 * Stops the ETHER (receive) operation.
 *
 * @param[in] Addr       - Channel id the given ETHER instance
 * @param[in] Par1       - Optional parameter (unused)
 * @param[in] Par2       - Optional parameter (unused)
 * @param[in] Par3       - optional parameter (unused)
 *
 * @return == 0 - OK
 */
int R_CIO_ETHER_PRV_Close(size_t Addr, void *Par1, void *Par2, void *Par3);


#ifdef __cplusplus
}
#endif

#endif
/** @} */
