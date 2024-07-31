/*************************************************************************************************************
* cio_ether_ioctl_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/**@defgroup group4_ether Ether Driver Configuration Function
 * @ingroup cio_eth_driver 
 *
 * @brief Provide a CIO interface to pass different configuartion parameter to the driver
 *
 * @{
 */



#ifndef R_CIO_ETHER_IOCTL_H
#define  R_CIO_ETHER_IOCTL_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Function Name: R_CIO_ETHER_PRV_IoCtl
 */
/**
 * @brief  ETHER  settings function.
 *
 * @param[in] Addr       - Channel id of the given ETHER instance
 * @param[in] Par1       - Setting/Function to be configured
 * @param[in] Par2       - Value to be set
 * @param[in] Par3       - optional parameter
 *
 * @return == 0 - OK
 * @return != 0 - NG
 */
ssize_t R_CIO_ETHER_PRV_IoCtl(size_t Addr, void *Par1, void *Par2, void *Par3);


#ifdef __cplusplus
}
#endif

#endif

/** @} */