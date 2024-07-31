/*************************************************************************************************************
* WindowMng_wmdrv_api_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_WMDRV_API_H_
#define R_WMDRV_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Section Includes
 */
#include "r_wm_common.h"
#include "r_wm_bridge.h"
#include "r_wm_config.h"
#include "rcar-xos/osal/r_osal.h"

/** \defgroup WmDrvPublic WM driver API
*   @ingroup middleware_wm
*   @brief WM Driver public interface
*
*/

/*
 * Section: Global Defines
 */

/** \defgroup WMDrvTypes WM driver types
 *  \ingroup  WmDrvPublic
 *  @brief Types and macros of the WM Driver lib
 *
 *  @{
 */

/**
 * Maximum number of processes that can access a WM Instance.
 * @note As the RCAR VSPD supports a max of 5 Layers, we limited the number of tasks
 * to 5.
 */
#define R_WMDRV_MAX_THREAD_PER_INSTANCE 5

/*
 * Section: Global types
 */

/**
 * Wm Event description.
 *
 */
typedef struct {
    uint32_t Pid;        /**< process ID that requested the event */
    void    *MqHandle;   /**< Handle for Message Queue */
} r_wmdrv_Event_t;

/**
 * Wm Surface configuration.
 */
#define SURFACE_UNASSIGNED (0xFFFFFFFF)
typedef struct {
    uint32_t        PosZ;       /**< Surface position on the Z axe. Correspond to the layer number */
    uint32_t        ProcessId;  /**< Process ID of the application using the surface */
} r_wmdrv_Surface_t;

/**
 * This structure contain the information related to a WM Vout instance.
 * @note   There is one instance per Vout.
 */
typedef struct {
    /* Vout */
    r_wm_DispTiming_t   Timings;        /**< display timings. See @ref r_wm_DispTiming_t */
    uint32_t            BckgColor;      /**< Display background color */
    r_wm_ColorFmt_t     ColorFmt;       /**< Display color format. See @ref r_wm_ColorFmt_t */
    int32_t             Enable;         /**<  */
    void               *VoutConf;       /**<  */

    /* Instance flag */
    uint32_t                InUseCnt;   /**< Number of task using this instance */
    osal_mutex_handle_t     MutexHandle; /**< Mutex protecting the count */
    /* User registered Event */
    r_wmdrv_Event_t     UserEvent[R_WMDRV_MAX_THREAD_PER_INSTANCE][R_WM_EVENT_LAST]; /**< Table with the list of User register event per instance. */

} r_wmdrv_VOutInstance_t;

/** @} */ /* end of group WMDrvTypes */

extern r_wmdrv_VOutInstance_t r_wmdrv_Vout[R_WM_DEV_LAST];
extern r_wmdrv_Surface_t r_wmdrv_InstSurface[R_WM_DEV_LAST][R_WM_NUM_LAYER_PER_VOUT];

/*
 * Section: Global Function
 */

/** \defgroup WMDrvfn WM driver functions
 *  \ingroup  WmDrvPublic
 *  @brief WM driver API functions
 *
 *  @{
 */

/**
 * Find and return the Vout instance corresponding to the Wm Unit
 *
 * @param[in] WmUnit  - Wm unit. See @ref r_wm_Dev_t
 * @retval  see @ref r_wmdrv_VOutInstance_t
 *
 */
r_wmdrv_VOutInstance_t * R_WMDRV_FindInstance(r_wm_Dev_t WmUnit);

/**
 * Increment the number of process using a given WM instance
 *
 * @param[in] WmUnit  - Wm unit. See @ref r_wm_Dev_t
 * @retval  0 if successful else -1
 */
int32_t R_WMDRV_AddProcToInstance(r_wm_Dev_t WmUnit);

/**
 * Return the number of processes using this WM instance
 *
 * @param[in] WmUnit  - Wm unit. See @ref r_wm_Dev_t
 * @retval  total number of processes using this WM instance
 *
 */
int32_t R_WMDRV_GetProcNbUsingInstance(r_wm_Dev_t WmUnit);

/**
 * Initialise the WM Unit internal structure
 * It checks the current RCar device and configures acccordinly the WM instance
 * with default values
 *
 * @param[in] WmDev  - Wm unit, range R_WM_DEV_VOUT0-R_WM_DEV_VOUT3. See @ref r_wm_Dev_t
 * @retval 0 if successful else -1 is failed.
 */
int R_WMDRV_Init(r_wm_Dev_t WmDev);

/**
 * Clear the current Wm instance
 *
 * @param[in]  WmDev  - Wm unit. See @ref r_wm_Dev_t
 * @retval 0 if successful else -1 is failed.
 */
int R_WMDRV_Deinit(r_wm_Dev_t WmDev);

/**
 * Initialise the Video output instance
 *
 * @param[in]  Inst                - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData(not used)    - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used)   - pointer to the WM msg the Wm driver will send back to the User application.
 *                                   See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutInit(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * De-Initialise the Video output instance
 *
 * @param[in]  Inst                - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData              - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used)   - pointer to the WM msg the Wm driver will send back to the User application.
 *                                   See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutDeinit(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Get/Set the display timings
 *
 * @param[in]  Inst      - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData    - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData   - pointer to the WM msg the Wm driver will send back to the User application. See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutTimings (r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Enable or disable the Video output
 *
 * @param[in]  Inst                - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData              - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used)   - pointer to the WM msg the Wm driver will send back to the User application.
 *                                   See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutEnable (r_wmdrv_VOutInstance_t * Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Get/Set the background color of the display output
 *
 * @param[in]  Inst                - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData              - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used)   - pointer to the WM msg the Wm driver will send back to the User application.
 *                                   See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutBkgCol (r_wmdrv_VOutInstance_t * Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Get/Set the display output color format
 *
 * @param[in]  Inst                - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData              - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used)   - pointer to the WM msg the Wm driver will send back to the User application.
 *                                   See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutColFmt (r_wmdrv_VOutInstance_t * Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Set the alpha of the surface
 *
 * @param[in]  Inst                - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData              - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used)   - pointer to the WM msg the Wm driver will send back to the User application.
 *                                   See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutSurfAlphaSet (r_wmdrv_VOutInstance_t * Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Get a free layer from the Vout instance.
 *
 * @param[in]  Inst      - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData    - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData   - pointer to the WM msg the Wm driver will send back to the User application.  See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutLayerGet(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Configure the Layer/surface position on screen
 *
 * @param[in]  Inst                - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData              - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used)   - pointer to the WM msg the Wm driver will send back to the User application.
 *                                   See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutLayerPos(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Configure the Layer/surface size
 *
 * @param[in]  Inst                - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData              - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used)   - pointer to the WM msg the Wm driver will send back to the User application.
 *                                   See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutLayerSize(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Configure the Layer/surface format
 *
 * @param[in]  Inst                - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData              - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used)   - pointer to the WM msg the Wm driver will send back to the User application.
 *                                   See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutLayerFormat(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Get/Set the framebuffer address of a given surface
 *
 * @param[in]  Inst                - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData              - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used)   - pointer to the WM msg the Wm driver will send back to the User application.
 *                                   See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutLayerFb(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Execute all the display commands  in the queue.
 *
 * @param[in]  Inst                 - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData               - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used)    - pointer to the WM msg the Wm driver will send back to the User application.
 *                                    See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutFlush(r_wmdrv_VOutInstance_t * Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Release the given layer.
 * It will disable the surface and clean-up any related internal variables
 *
 * @param[in]  Inst                - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData              - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used)   - pointer to the WM msg the Wm driver will send back to the User application.
 *                                   See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutLayerRelease(r_wmdrv_VOutInstance_t * Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Register an event for the given VOut instance.
 *
 * @param[in]  Inst                - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData              - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used)   - pointer to the WM msg the Wm driver will send back to the User application.
 *                                   See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutRegisterEvent(r_wmdrv_VOutInstance_t * Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Configure DISCOM
 *
 * @param[in] Inst      - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in] InData    - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[in] OutData   - pointer to the WM msg the Wm driver will send back to the User application.  See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_VoutLayerDiscomConfigure(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Function R_WMDRV_DocConfigure
 * @brief Initialize DOC register and Set DOC Monitor Configuration
 *
 * @param[in]  Inst              - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData            - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used) - pointer to the WM msg the Wm driver will send back to the User application.
 *                                 See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_DocConfigure(r_wmdrv_VOutInstance_t *Inst,
                const r_wm_Msg_Generic_t * InData, r_wm_Msg_Generic_t * OutData);

/**
 * Function R_WMDRV_DocEnable
 * @brief Enable DOC function
 *
 * @param[in]  Inst              - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData            - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used) - pointer to the WM msg the Wm driver will send back to the User application).
 *                                 See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_DocEnable(r_wmdrv_VOutInstance_t *Inst,
                const r_wm_Msg_Generic_t * InData, r_wm_Msg_Generic_t * OutData);

/**
 * Function R_WMDRV_DocClearStatus
 * @brief Clear DOC errors
 *
 * @param[in]  Inst              - pointer to a VOut instance structure, see @ref r_wmdrv_VOutInstance_t
 * @param[in]  InData            - pointer to a WM msg received from the WM user lib. See @ref r_wm_Msg_Generic_t
 * @param[out] OutData(not used) - pointer to the WM msg the Wm driver will send back to the User application.
 *                                 See @ref r_wm_Msg_Generic_t
 *
 * @retval See @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_DocClearStatus(r_wmdrv_VOutInstance_t *Inst,
                const r_wm_Msg_Generic_t * InData, r_wm_Msg_Generic_t * OutData);

/** @} */ /* end of group WMDrvfn */

#ifdef __cplusplus
}
#endif

#endif /* R_WMDRV_API_H_ */
