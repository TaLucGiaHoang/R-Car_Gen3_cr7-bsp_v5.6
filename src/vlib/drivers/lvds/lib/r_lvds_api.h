/*************************************************************************************************************
* lvds_api_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_LVDS_H_
#define R_LVDS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "r_du_config.h"

/**
 * \defgroup  LVDSgroup   LVDS driver
 * @ingroup drivers
 * @brief     LVDS driver interface for R-Car E3
 * @{
 * @}
 */

/**
 * \defgroup LVDSapi   API
 * @ingroup LVDSgroup
 * @{
 */

/**
 * Configure a LVDS unit with the provided timings
 *
 * LVDS driver does not have the DeInitialize API, therefore after called this function,
 * LVDS driver can not close.
 *
 * @param[in]  LvdsUnit - number of the LVDS unit (Do not use the values in H3. Valid value: 0,1 )
 * @param[in]  Timings  - pointer to the display timing informaton. See @ref r_du_DispTimings_t
 *      On R-Car E3, Timings->CurrentDispClock will be set to the dot clock rate. (NULL is prohibited.)
 *
 * @retval 0 if successful else !=0
 */
int32_t R_LVDS_Setup(uint32_t LvdsUnit, r_du_DispTimings_t *Timings);

/** @} api */

#ifdef __cplusplus
}
#endif

#endif /* R_LVDS_H_ */
