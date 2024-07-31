/*************************************************************************************************************
* cio_ether_write_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/**@defgroup etherwrite Ether Driver data transmit
 * @ingroup cio_eth_driver
 *
 * @brief Write data to the Ether drivers's trasmit buffer
 *
 * @{
 */


#ifndef R_CIO_ETHER_WRITE_H
#define  R_CIO_ETHER_WRITE_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
   Section: Global private functions
 */

/*******************************************************************************
 * Function Name: R_CIO_ETHER_PRV_Write
 */
/**
 * @brief Write to the message buffer assigned to the given instance
 * and initiate transmission
 *
 * @param[in] Addr       - Channel id of the given ETHER instance
 * @param[in] Par1       - Optional parameter (unused)
 * @param[in] Par2       - Optional parameter (unused)
 * @param[in] Par3       - optional parameter (unused)
 *
 * @return == 0 - OK
 * @return != 0 - NG
 *
 */
ssize_t R_CIO_ETHER_PRV_Write(size_t Addr, void *Par1, void *Par2, void *Par3);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
