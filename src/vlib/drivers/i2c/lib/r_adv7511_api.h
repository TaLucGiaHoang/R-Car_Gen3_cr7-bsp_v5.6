/*************************************************************************************************************
* i2c_adv7511_api_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2018-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/


#ifndef R_ADV7511_API_H_
#define R_ADV7511_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup  ADV7511group   ADV7511 driver API
 * \ingroup  I2CMaingroup
 * @brief ADV7511 driver interface
 * @{
 */


#include <stdint.h>

/**
 * Function
 * @brief Initialisation of the ADV7511  dirver
 *
 * @param[in] Unit - I2C unit. See @ref r_i2c_Unit_t
 * @param[in] SlaveI2cAddr - address of I2C slave
 *
 * @retval 0 if successful, else -1
 */
int32_t R_ADV7511_Init(r_i2c_Unit_t Unit, uint32_t SlaveI2cAddr);

/** @} */ /*end of group ADV7511group */

#endif /* R_ADV7511_API_H */
