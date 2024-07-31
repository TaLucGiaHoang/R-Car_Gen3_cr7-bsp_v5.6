/*************************************************************************************************************
* cio_vin_status_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/**@defgroup status_vin VIN Driver status
 * @ingroup cio_vin_driver
 *
 * @brief
 *
 * @{
 */
#ifndef R_CIO_VIN_STATUS_H
#define R_CIO_VIN_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
   Section: Global private functions
 */


/*******************************************************************************
   Function Name: R_CIO_VIN_PRV_Status
 */
/**
 * @brief  Return the current (error)status for the given instance
 * After call this function, the error status will be cleared.
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
ssize_t R_CIO_VIN_PRV_Status(size_t Addr, void *Par1, void *Par2, void *Par3);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
