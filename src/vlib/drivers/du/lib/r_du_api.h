/*************************************************************************************************************
* du_api_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_DU_API_H_
#define R_DU_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup DUgroup   DU driver
 * @ingroup drivers
 * @brief Declaration of the DU(Display Unit) driver APIs
 * @{
 * @}
 */

#include "r_du_config.h"
#include "r_prr_api.h"

/**
 * \defgroup DUtypes   Data Types
 * @ingroup DUgroup
 * @{
 */

typedef enum {
    R_DU_DISABLE,                   /**< Disable DU */
    R_DU_ENABLE,                    /**< Enable DU */
} r_du_Enable_t;

/**
 * DU Errors
 */
typedef enum {
    R_DU_ERR_SUCCESS,               /**< Success */
    R_DU_ERR_INVALID_PARAMETER,     /**< An input parameter is incorrect */
    R_DU_ERR_NULL_POINTER,          /**< input parameter is a Null pointer */
    R_DU_ERR_INVALID_CLOCK,         /**< Invalid clock */
    R_DU_ERR_SLOW_CLOCK,            /**< Issue with clock initialisation, slow clock selected */
    R_DU_ERR_WRONG_DEVICE,          /**< mismatch between device and DU */
    R_DU_ERR_WRONG_UNIT,            /**< incorrect unit */
    R_DU_ERR_UNKNOWN_VOUT_TYPE,     /**< Unknown Vout type was passed */
    R_DU_ERR_HDMI_FAILED,           /**< Failed to initialise HDMI */
    R_DU_ERR_HDMI_NOT_AVAILABLE,    /**< HDMI not available */
    R_DU_ERR_LVDS_FAILED,           /**< Failed to initialise LVDS */
    R_DU_ERR_INVALID_PLANE,         /**< A wrong plane number was requested */
    R_DU_ERR_MODULE_ENABLE_FAILED,  /**< Failed to enable the related DU module (CPG) */
    R_DU_ERR_FMT_DEFAULT,           /**< use default color format RGB888 */
    R_DU_ERR_BLEND_DEFAULT,         /**< Use the default blend method */
} r_du_Error_t;

/**
 * Interrupt types supported by DU.
 */
typedef enum {
    R_DU_INT_TVE  = (1 << 0),        /**< TV Synchronization Signal Error */
    R_DU_INT_FRM  = (1 << 1),        /**< Frame Flag */
    R_DU_INT_VBK  = (1 << 2),        /**< Vertical Blanking */
    R_DU_INT_RINT = (1 << 3),        /**< Raster Interrupt */
    R_DU_INT_HBK  = (1 << 4),        /**< Horizontal Blanking */
} r_du_Int_t;


/** @} data types */
/**
 * \defgroup DUapi   API
 * @ingroup DUgroup
 * @{
 */

/**
 * Initialise the given display unit.
 *
 * DU driver does not have the DeInitialize API, therefore after called this function,
 * target DU Unit can not close.
 *
 * @param[in]   DuUnit      See @ref r_du_Unit_t
 * @param[in]   Timings     See @ref r_du_DispTimings_t (must not be NULL)
 *      Timings->CurrentDispClock may be modified for calculated PLL settings.
 * @param[in]   DuPlaneMsk  list of planes to be used. 1: plane1, 2: plane3, 3: plane 1&3
 * @param[in]   DuPlaneCfg  pointer to the plane configuration. See @ref r_du_Plane_t (must not be NULL)
 * @param[in]   Dev         Device name, see @ref r_prr_DevName_t
 * @retval      see @ref r_du_Error_t
 */
r_du_Error_t R_DU_Init(r_du_Unit_t DuUnit, r_du_DispTimings_t *Timings,
                       uint32_t DuPlaneMsk, const r_du_Plane_t *DuPlaneCfg,
                       r_prr_DevName_t Dev);


/**
 *  Enable or disable the given display unit.
 *
 * @param[in]   DuUnit      See @ref r_du_Unit_t
 * @param[in]   Enable      See @ref r_du_Enable_t
 */
void R_DU_Enable(r_du_Unit_t DuUnit, r_du_Enable_t Enable);


/**
 * Reset Toggle the Display unit. This function shall be used when
 * the user want to write to some registers which are updated only after reset.
 *
 * @param[in]   DuUnit      See @ref r_du_Unit_t
 */
void R_DU_UpdateByResetToggle(r_du_Unit_t DuUnit);


/**
 * Set the output color format.
 *
 * @param[in]   DuUnit      See @ref r_du_Unit_t
 * @param[in]   DuPlaneMsk  list of plane to be used. 1: plane1, 2: plane3, 3: plane 1&3
 * @param[in]   DuFmt       color format, see @ref r_du_PlaneFormat_t
 * @retval      see @ref r_du_Error_t
 */
r_du_Error_t R_DU_ColorFmtSet(r_du_Unit_t DuUnit, uint32_t DuPlaneMsk,
                              r_du_PlaneFormat_t DuFmt);

/**
 * Set the display off mode output color.
 *
 * @param[in]   DuUnit      See @ref r_du_Unit_t
 * @param[in]   Color       Color for the display off mode output to set DOORn register
 *                          Red: Bit 23-18, Green: Bit 15-10, Blue: Bit 7-2
 * @retval      see @ref r_du_Error_t
 */
r_du_Error_t R_DU_BkgColorSet(r_du_Unit_t DuUnit, uint32_t Color);

/** @} api */

#ifdef __cplusplus
}
#endif

#endif /* R_DU_API_H */
