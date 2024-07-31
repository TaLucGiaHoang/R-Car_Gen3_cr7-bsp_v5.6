/*************************************************************************************************************
* hdmi_api_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
*************************************************************************************************************/

#ifndef R_HDMI_H_
#define R_HDMI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r_du_config.h"

/**
 * \defgroup  HDMIgroup   HDMI driver
 * @ingroup drivers
 * @brief HDMI driver interface for R-Car H3, M3, and M3N
 * @{
 * @}
 */

/**
 * \defgroup HDMItypes   Data Types
 * @ingroup HDMIgroup
 * @{
 */

/**
 * HDMI error list
 */
typedef enum {
    R_HDMI_ERR_SUCCESS,                 /**< Success */
    R_HDMI_ERR_INVALID_UNIT,            /**< The unit provided is incorrect */
    R_HDMI_ERR_PHY_TIMEOUT,             /**< failed to set PHY register */
    R_HDMI_ERR_I2CSTAT_TIMEOUT,         /**< failed to set I2C register */
    R_HDMI_ERR_PHYSTAT_TIMEOUT,         /**< failed to set Phystat register */
    R_HDMI_ERR_I2CPHYSTAT_TIMEOUT,      /**< failed to set I2C registers */
} r_hdmi_Error_t;

/** @} types */
/**
 * \defgroup HDMIapi   API
 * @ingroup HDMIgroup
 * @{
 */

/**
 * Set up the Display unit mapped to the given HDMI unit
 *
 * HDMI driver does not have the DeInitialize API, therefore after called this function,
 * HDMI driver can not close.
 *
 * @param[in] DuUnit    - DU unit to be configured. Valid value 1, 2. See @ref r_du_Unit_t
 * @param[in] Timings   - pointer to a structure of type @ref r_du_DispTimings_t.(must not be NULL)
 *      If HdmiPllCtrl is 0, the driver will calculate the PLL settings based on
 *      the CurrentDispClock. Otherwise, the driver will program HdmiPllCtrl,
 *      HdmiPllCfg and HdmiPllDivider.
 *
 * @retval See @ref r_hdmi_Error_t
 */
r_hdmi_Error_t R_HDMI_Setup(r_du_Unit_t DuUnit, const r_du_DispTimings_t *Timings);

/**
 * Read the EDID from the attached display.
 * This function must only be called after R_HDMI_Setup() has been successful.
 *
 * @param[in] DuUnit    - DU unit to be configured.Valid value 1, 2. See @ref r_du_Unit_t
 * @param[in] Edid      - pointer to where the data will be stored, must be size long.(must not be NULL)
 * @param[in] size      - Number of bytes to be read.
 *
 * @retval See @ref r_hdmi_Error_t
 */
r_hdmi_Error_t R_HDMI_ReadEdid(r_du_Unit_t DuUnit, uint8_t *Edid, uint8_t size);

/** @} api */

#ifdef __cplusplus
}
#endif

#endif /* R_HDMI_H_ */

