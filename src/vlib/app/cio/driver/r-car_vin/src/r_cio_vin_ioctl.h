/*************************************************************************************************************
* cio_vin_ioctl_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/**@defgroup group4_vin VIN Driver Configuration Function
 * @ingroup cio_vin_driver
 *
 * @brief Provide a CIO interface to pass different configuartion parameter to the driver
 *
 * @{
 */
#ifndef R_CIO_VIN_IOCTL_H
#define R_CIO_VIN_IOCTL_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Function Name: R_CIO_VIN_PRV_IoCtl
 */
/**
 * @brief VIN settings function.
 *
 * Provide a CIO interface to pass different configuration parameter to the driver.
 *
 * @param [in] Addr      - Channel number of the given VIN instance
 * @param [in] Par1      - Set settings/functions. See @ref r_cio_vin_Ioctl_t.
 * @param [in] Par2      - Set r_cio_vin_Config_t. See @ref r_cio_vin_Config_t.
 * @param [in] Par3      - Optional parameter (unused)
 *
 * @return R_CIO_VIN_ERR_OK           - success
 * @return R_CIO_VIN_ERR_FAILED       - not success
 * @return R_CIO_VIN_ERR_IOC_UNKNOWN  - Incorrect value of Par1
 * @return R_CIO_VIN_ERR_NO_INST      - no instance
 * @return R_CIO_VIN_ERR_PARA         - Incorrect parameters
 * @return R_CIO_VIN_ERR_WRONG_STATE  - The state is not correct
 * @return R_CIO_VIN_ERR_IOC_CHN_CONFIGED - Config is already running
 * @return R_CIO_VIN_ERR_IOC_CHN_UNINIT   - uninit (status mismatch)
 * @return R_CIO_VIN_ERR_IOC_CHN_NOT_STARTED - not stated (status mismatch)
 */
ssize_t R_CIO_VIN_PRV_IoCtl(size_t Addr, void *Par1, void *Par2, void *Par3);


#ifdef __cplusplus
}
#endif

#endif /* R_CIO_VIN_IOCTL_H */

/** @} */
