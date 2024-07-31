/*************************************************************************************************************
* WindowMng_wm_api_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_WM_API_H_
#define R_WM_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Section Include
 */
#include "r_wm_common.h"

/*
 * Section: Type Definition
 */

/**
 * \defgroup PublicDef WM User Mode Lib
 * @ingroup middleware_wm
 * @brief Description of the WM public types and User API.
 */


/**
 * \defgroup PublicTypes WM User Mode types
 * \ingroup PublicDef
 * @brief Wm Public Type definitions
 * @{
 */

typedef struct {
    r_wm_DocCb_t            Callback;         /**< Callback when DOC error occurs */
    void                    *CbArg;           /**< Argument for Callback function */
    r_wm_Doc_DocUnit_t      DocUnit;          /**< DOC Unit */
    r_wm_Doc_MonitorParam_t DocMonitors[DOC_MAX_MONITORS];
    r_wm_Doc_DUParam_t      DuParam;          /**< DOC DU parameters */
    bool                    Enable;           /**< Enable/Disable DOC */
    bool                    ActMonEnable;     /**< Enable/Disable ActiveMonitor */
    uint32_t                ActMonUpperTime;  /**< Active Monitor upper time */
    uint32_t                ActMonLowerTime;  /**< Active Monitor lower time */
} r_wm_Doc_t;

typedef struct {
    r_wm_DiscomCb_t Callback; /**< Callback when Crc mismatch occurs */
    void           *CbArg;  /**< Argument for Callback function */
    uint32_t        PosX;   /**< ROI position on the X axis within the surface, in pixels
                                 Must be a multiple of 2 for M3-W(+) */
    uint32_t        PosY;   /**< ROI position on the Y axis within the surface, in pixels */
    uint32_t        Width;  /**< ROI width */
    uint32_t        Height; /**< ROI height */
    uint32_t        Crc;    /**< Expected CRC */
    bool            Enable; /**< Enable/Disable DISCOM */
} r_wm_Discom_t;

/**
 * Structure describing the Surface configuration
 * @note    FBufC0, FBufC1 must be set to 0 for formats that do not require them.
 * @note    Alpha plane is not supported.
 */
typedef struct {
    uint32_t        PosX;   /**< position on the X axis in pixels */
    uint32_t        PosY;   /**< position on the Y axis in pixels */
    uint32_t        PosZ;   /**< position on the Z axis. number must correspond to a Layer number */
    uint32_t        Width;  /**< Surface's width */
    uint32_t        Height; /**< Surface's height */
    uint16_t        StrideY;/**< Surface's stride_y Memory Stride of Source Picture Y/RGB Plane (unit:byte) */
    uint16_t        StrideC;/**< Surface's stride_c Memory Stride of Source Picture C Plane (unit:byte) */
    r_wm_ColorFmt_t   Fmt;  /**< Surface's color format. @ref r_wm_ColorFmt_t*/
    void          * Handle; /**< pointer to a surface handle the user got from the CIO WM by calling
                                 the R_WM_SurfaceGet() function. */
    uintptr_t        FBuf;   /**< address to the Framebuffer */
    uintptr_t        FBufC0; /**< address to the Framebuffer for C0 */
    uintptr_t        FBufC1; /**< address to the Framebuffer for C1 */
    r_wm_Doc_t       *Doc;   /**< Surface's DOC configuration @ref r_wm_Doc_t */
    r_wm_Discom_t  *Discom; /**< Surface's DISCOM configuration. @ref r_wm_Discom_t */
    uint8_t         Alpha;   /**< Alpha ratio to be set to the window */
} r_wm_Surface_t;

/** @} */ /* end group Types*/

/*
 * Section: Global Function
 */

/**
 * \defgroup PublicFunc WM User API
 * \ingroup PublicDef
 * @brief WM User API descriptions
 */

/**
 * \defgroup WMDev WM Device Functions
 * \ingroup PublicFunc
 * @brief WM functions related to the WM device
 * @{
 */

/**
 * Initialise a Window Manager Device.
 *
 * This function will open a communication channel with the CIO WM Device and initialise
 * the corresponding DU.
 *
 * @param[in] WmUnit - WM Device to be initialised. Correspong to the DU unit.
 *                     Range R_WM_DEV_VOUT0 - R_WM_DEV_VOUT3. See @ref r_wm_Dev_t
 * @retval WM Error @ref r_wm_Error_t
 */
r_wm_Error_t R_WM_DevInit(r_wm_Dev_t WmUnit);

/**
 * De-initialise a Window Manager Device.
 *
 * This function will close the communication channel with the CIO WM Device and
 * the disable the corresponding DU, VSPD and DOC if they are not used by other application.
 * DU and VSPD do not have DeInitialize API, therefore the corresponded registers will not clear.
 * (Only disable DU and remove VSPD interrupt setting)
 *
 * @param[in] WmUnit - WM Device to be initialised. Correspong to the DU unit.
 *                     Range R_WM_DEV_VOUT0 - R_WM_DEV_VOUT3. See @ref r_wm_Dev_t
 * @retval WM error @ref r_wm_Error_t
 */
r_wm_Error_t R_WM_DevDeinit(r_wm_Dev_t WmUnit);

/**
 * Get information about the Window Manager  Device.
 *
 * @param[in]   WmUnit       - WM Device, range R_WM_DEV_VOUT0 - R_WM_DEV_VOUT3. See @ref r_wm_Dev_t
 * @param[out]  MaxLayerNum  - Max number of layers supported by the WM Device(must not be NULL)
 * @param[out]  ScreenWidth  - Screen width(must not be NULL)
 * @param[out]  ScreenHeight - Screen height(must not be NULL)
 * @retval WM error @ref r_wm_Error_t
 */
r_wm_Error_t R_WM_DevInfoGet(r_wm_Dev_t WmUnit, uint32_t * MaxLayerNum,
                uint32_t * ScreenWidth, uint32_t * ScreenHeight);

/**
 * Register an event with the current WM Device.
 * NOTE: currently only VBlank is supported
 *
 * @param[in] WmUnit      - WM Device, range R_WM_DEV_VOUT0 - R_WM_DEV_VOUT3. See @ref r_wm_Dev_t
 * @param[in] EventId     - Event to register
 * @param[in] Arg         - Arg, may be require in case of scanline event for example
 * @param[in] Mq          - Handle for Message Queue
 *                          This argument expects the address of OSAL Message Queue Handle (osal_mq_handle_t)
 * @retval WM error @ref r_wm_Error_t
 */
r_wm_Error_t R_WM_DevEventRegister (r_wm_Dev_t WmUnit, r_wm_EventId_t EventId,
                uint32_t *Arg, void *Mq);

/** @} */ /* endo groupt WMDev */

/**
 * \defgroup WMScreen WM Screen Functions
 * \ingroup PublicFunc
 * @brief WM functions handling the screen configuration
 * @{
 */

/**
 * Initialise the given property of the WM device screen with the given value.
 *
 * This function sends a message via the OS communication channel to the WM driver.
 * The property must be one of the define one, see @ref r_wm_ScreenProp_t
 * Depending on the property to be set, the Data passed shall be of a specific type:
 *     - WM_SCREEN_PROP_FMT      : pointer to the data for @ref r_wm_ColorFmt_t
 *     - WM_SCREEN_PROP_BKGCOLOR : pointer to the data for uint32_t which contains ARGB8888; Alpha value shall be ignored
 *     - WM_SCREEN_PROP_TIMING   : pointer to the data for @ref r_wm_DispTiming_t
 *                                 When called twice, the return value will be R_WM_ERR_UNIT_ALREADY_INIT
 *
 * @param[in] WmUnit      - WM Device, range R_WM_DEV_VOUT0 - R_WM_DEV_VOUT3. See @ref r_wm_Dev_t
 * @param[in] Prop        - Property to be configured
 * @param[in] Data        - Property Data. Can be a pointer to an integer or a structure
 *                          depending on the property to be set
 * @retval WM error @ref r_wm_Error_t
 */
r_wm_Error_t R_WM_ScreenPropertySet (r_wm_Dev_t WmUnit, r_wm_ScreenProp_t Prop,
                void * Data);

/**
 * Enable the WM device screen.
 *
 * This function sends a message via the OS communication channel to the WM driver.
 *
 * @param[in] WmUnit      - WM Device, range R_WM_DEV_VOUT0 - R_WM_DEV_VOUT3. See @ref r_wm_Dev_t
 * @param[in] Enable      - 1, enable WM. 0, disable WM
 *
 * @retval WM error @ref r_wm_Error_t
 */
r_wm_Error_t R_WM_ScreenEnable (r_wm_Dev_t WmUnit, uint32_t Enable);


/**
 * Flush the different change requests from the ring buffer
 *
 * All the surfaces linked to this WM Unit will have their change requests processed.
 * The function can work synchronously and wait for the VBlank to make sure the
 * changes are applied or asynchronously and return immediately. In that case
 * user must sync the application with the VBlank.
 *
 * @param[in] WmUnit  - WM Device, range R_WM_DEV_VOUT0 - R_WM_DEV_VOUT3. See @ref r_wm_Dev_t
 * @param[in] VSync   - 0, non-blocking, does not wait for VBlank.
 *                      1, wait for Vblank before returning
 *
 * @retval  WM error @ref r_wm_Error_t
 */
r_wm_Error_t R_WM_ScreenSurfacesFlush(r_wm_Dev_t WmUnit, uint32_t VSync);

/** @} */   /* End group WmScreen */

/**
 * \defgroup WmSurf WM Surface Functions
 * \ingroup PublicFunc
 * @brief WM functions related to Surfaces
 *
 * @{
 */

/**
 * Ask for a Layer of the given WM Dev.
 *
 * The function returns an error if none are available, else it will return a handle.
 *
 * @param[in]  WmUnit      - WM Device, range R_WM_DEV_VOUT0 - R_WM_DEV_VOUT3. @ref r_wm_Dev_t
 * @param[out] Surf        - Surface Handle provided by the CIO WM will be store in
 *                           this structure. @ref r_wm_Surface_t
 * @retval WM error @ref r_wm_Error_t
 */
r_wm_Error_t R_WM_SurfaceGet(r_wm_Dev_t WmUnit, r_wm_Surface_t * Surf);

/**
 * Set the Surface property.

 * This function saves the request in the ringbuffer. Only when the function
 * a screen flush is requested will the changes be sent to the driver via the OSAL
 * communication channel. The property must be one of the define one, see @ref r_wm_SurfaceProp_t.
 *     - R_WM_SURF_PROP_POS      : The values of PosX, PosY, PosZ in Surf are used.
 *     - R_WM_SURF_PROP_SIZE     : The values of Width, Height, StrideY, StrideC in Surf are used.
 *     - R_WM_SURF_PROP_COLORFMT : The values of Fmt in Surf are used.
 *     - R_WM_SURF_PROP_BUFFER   : The values of FBuf, FBufC0, FBufC1 in Surf are used.
 *                               : In case that a data of ARGB8888 format is set to this member variables,
 *                               : Transparent is enable by setting Alpha component.
 *     - R_WM_SURF_PROP_DOC      : The values of Doc in Surf are used. @ref r_wm_Doc_t.
 *
 * @param[in] WmUnit  - WM Device, range R_WM_DEV_VOUT0 - R_WM_DEV_VOUT3. @ref r_wm_Dev_t
 * @param[in] Prop    - Surface property to be set. @ref r_wm_SurfaceProp_t
 * @param[in] Surf    - pointer to a structure containting all the surface information.
 *                      @ref r_wm_Surface_t
 * @retval WM error @ref r_wm_Error_t
 */
r_wm_Error_t R_WM_SurfacePropertySet(r_wm_Dev_t WmUnit, r_wm_SurfaceProp_t Prop,
                r_wm_Surface_t *Surf);

/**
 * Release the given surface.
 *
 * @param[in] WmUnit      - WM Device, range R_WM_DEV_VOUT0 - R_WM_DEV_VOUT3. @ref r_wm_Dev_t
 * @param[in] Surf        - pointer to the surface structure @ref r_wm_Surface_t
 *
 * @retval WM error @ref r_wm_Error_t
 */
r_wm_Error_t R_WM_SurfaceRelease(r_wm_Dev_t WmUnit, r_wm_Surface_t * Surf);

/** @} */   /* End group WmSurf */

/**
 * Enable DOC function of VOUT Monitor and
 * then enable Active Monitor according to 'Surf' in parameters.
 *
 * @param[in] WmUnit      - WM Device, range R_WM_DEV_VOUT0 - R_WM_DEV_VOUT3. @ref r_wm_Dev_t
 * @param[in] Surf        - pointer to the surface structure @ref r_wm_Surface_t
 *
 * @retval WM error @ref r_wm_Error_t
 */
r_wm_Error_t R_WM_DocEnableMonitors(r_wm_Dev_t WmUnit, r_wm_Surface_t * Surf);

/**
 * Clear status for DOC used by given surface
 *
 * @param[in] WmUnit      - WM Device, range R_WM_DEV_VOUT0 - R_WM_DEV_VOUT3. @ref r_wm_Dev_t
 * @param[in] Surf        - pointer to the surface structure @ref r_wm_Surface_t
 *
 * @retval WM error @ref r_wm_Error_t
 */
r_wm_Error_t R_WM_DocClearStatus(r_wm_Dev_t WmUnit, r_wm_Surface_t * Surf);

/** @} */ /* End group WmScreen */

#ifdef __cplusplus
}
#endif

#endif /* R_WM_API_H */
