/*************************************************************************************************************
* cio_vin_connect_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/**@defgroup group2_vin VIN Driver Connect Functions
 * @ingroup cio_vin_driver
 *
 * @brief Methods used by the CIO Server to initiate or finish communicatrion with the
 * VIN module
 *
 * Functions which a the CIO Server will call in order to initiate a connection
 * between a user applcation and the VIN macro.
 *
 * @{
 */
#ifndef R_CIO_VIN_CONNECT_H
#define R_CIO_VIN_CONNECT_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
   Global (private) functions
 */

/*******************************************************************************
 *  Function Name: R_CIO_VIN_PRV_Open
 */
/**
 * @brief VIN internal Open function.
 *
 * Starts the VIN (receive) operation.
 *
 * @param[in] Addr       - Channel id of the given VIN instance
 * @param[in] Par1       - Optional parameter (unused)
 * @param[in] Par2       - Optional parameter (unused)
 * @param[in] Par3       - optional parameter (unused)
 *
 * @return == R_CIO_VIN_ERR_OK
 * @return == R_CIO_VIN_ERR_FAILED
 */
int R_CIO_VIN_PRV_Open(size_t Addr, void *Par1, void *Par2, void *Par3);

/*******************************************************************************
 *  Function Name: R_CIO_VIN_PRV_Close
 */
/**
 * @brief VIN internal Close function.
 *
 * Stops one VIN channel operation.
 *
 * When R_CIO_VIN_PRV_Close is called, the processing will be interrupted
 * if there is a processing request immediately before.
 *
 * @param[in] Addr       - Channel number of the given VIN instance
 * @param[in] Par1       - Optional parameter (unused)
 * @param[in] Par2       - Optional parameter (unused)
 * @param[in] Par3       - optional parameter (unused)
 *
 * @return == R_CIO_VIN_ERR_OK
 * @return == R_CIO_VIN_ERR_FAILED
 */
int R_CIO_VIN_PRV_Close(size_t Addr, void *Par1, void *Par2, void *Par3);


#ifdef __cplusplus
}
#endif

#endif /* R_CIO_VIN_CONNECT_H */
/** @} */
