/*************************************************************************************************************
* cio_vin_read_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/**@defgroup vinread VIN Driver data read
 * @ingroup cio_vin_driver
 *
 * @brief Read from the VIN drivers's receive buffer
 *
 * @{
 */
#ifndef R_CIO_VIN_READ_H
#define R_CIO_VIN_READ_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
   Section: Global private functions
 */


/*******************************************************************************
 *  Function Name: R_CIO_VIN_PRV_Read
 */
/**
 * @brief  Read from the message buffer assigned to the given instance
 * Read from the VIN driver's receive buffer.
 *
 * This function returns always 0
 *
 * @param[in] Addr       - Channel number of the given VIN instance
 * @param[in] Par1       - optional parameter (unused)
 * @param[in] Par2       - optional parameter (unused)
 * @param[in] Par3       - optional parameter (unused)
 *
 * @return == 0 - OK
 */
ssize_t R_CIO_VIN_PRV_Read(size_t Addr, void *Par1, void *Par2, void *Par3);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
