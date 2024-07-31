/*************************************************************************************************************
* cio_vin_write_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/**@defgroup write_vin VIN Driver data transmit
 * @ingroup cio_vin_driver
 *
 * @brief Write data to the VIN drivers's trasmit buffer
 *
 * @{
 */
#ifndef R_CIO_VIN_WRITE_H
#define R_CIO_VIN_WRITE_H

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
   Section: Global private functions
 */


/*******************************************************************************
 * Function Name: R_CIO_VIN_PRV_Write
 */
/**
 * @brief Write to the message buffer assigned to the given instance
 * and initiate transmission
 *
 * This function returns always 0
 *
 * @param[in] Addr       - Channel number of the given VIN instance
 * @param[in] Par1       - Optional parameter (unused)
 * @param[in] Par2       - Optional parameter (unused)
 * @param[in] Par3       - Optional parameter (unused)
 *
 * @return == 0 - OK
 */
ssize_t R_CIO_VIN_PRV_Write(size_t Addr, void *Par1, void *Par2, void *Par3);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
