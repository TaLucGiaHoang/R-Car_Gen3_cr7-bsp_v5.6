/*************************************************************************************************************
* WindowMng_wmdrv_sys_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_WMDRV_SYS_H_
#define R_WMDRV_SYS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Section Include
 */
#include "r_wm_config.h"
#include "r_wm_common.h"
#include "r_wm_bridge.h"
#include "r_wmdrv_api.h"

/*
 * Section: Global Functions
 */

/** \defgroup WMDrvSys WM driver Sys API
 * @ingroup middleware_wm
 *  @brief  WM Driver system dependent interface
 *
 *  Those functions depends on the OS and device used. Their implementation will vary
 *  accordingly.
 *
 *  @{
 */

/**
 * Initialise the System dependant part of a WM device
 *
 * @param[in] WmDev - Wm device. See @ref r_wm_Dev_t;
 * @retval    pointer to an internal Vout instance, which is system dependent.
 */
void * R_WMDRV_Sys_InstInit(r_wm_Dev_t WmDev);

/**
 * De-Initialise the System dependant part of a WM device
 *
 * @param[in] WmDev - Wm device. See @ref r_wm_Dev_t;
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_InstDeinit(r_wm_Dev_t WmDev);

/**
 * Initialise the given Vout instance
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] Timings - pointer to structure providing the display timings (system dependent)
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutInit(void *Inst, void *Timings);

/**
 * De-Initialise the given Vout instance
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutDeinit(void *Inst);

/**
 * Configure the VOUT with the provided timings
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] Timings - pointer to structure providing the display timings (system dependent)
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutTimingSet(void *Inst, void *Timings);

/**
 * Enable or disable the Vout instance
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] Enable - 1 if enabling, else 0
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutEnable (void *Inst, uint32_t Enable);

/**
 * Set the Vout background color
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] Color - 32-bit color
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutBkgColSet (void *Inst, uint32_t Color);

/**
 * Configure the VOut with the given color format
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] ColFmt - colo format
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutColFmtSet (void *Inst, uint32_t ColFmt);

/**
 * Return the display unit corresponding to the VOut instance
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @retval    display unit reference
 */
int32_t R_WMDRV_Sys_VoutUnitGet(void *Inst);

/**
 * Set the position of a surface on screen
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] PosZ - Layer we're working with
 * @param[in] PoyX - requested layer x position on display
 * @param[in] PosY - requested layer y position on display
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutLayerPosSet(void *Inst, uint32_t PosZ, uint32_t PosX, uint32_t PosY);

/**
 * Set the size of surface.
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] PosZ - Layer we're working with
 * @param[in] Width - requested layer width
 * @param[in] Height - requested layer height
 * @param[in] StrideY - requested layer strideY (Memory Stride of Source Picture Y/RGB Plane)
 * @param[in] StrideC - requested layer strideC (Memory Stride of Source Picture C Plane)
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutLayerSizeSet(void *Inst, uint32_t PosZ, uint32_t Width,
                        uint32_t Height, uint16_t StrideY, uint16_t StrideC);

/**
 * Set the framebuffer format of a layer
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] PosZ - Layer we're working with
 * @param[in] Format - Format of the framebuffer memory space
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutLayerFormatSet(void *Inst, uint32_t PosZ, r_wm_ColorFmt_t Format);

/**
 * Set the framebuffer address of a layer
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] PosZ - Layer we're working with
 * @param[in] Fb - pointer to the framebuffer memory space
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutLayerFbSet(void *Inst, uint32_t PosZ, uintptr_t Fb, uintptr_t C0, uintptr_t C1 );

/**
 * Flush the commands queued for a given Vout instance
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] Pid  - Process ID of the task requesting the flush
 *
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutFlush(void *Inst, uint32_t Pid);

/**
 * Initialise the given surface.
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] PosZ - Layer we're requesting
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutLayerGrab(void *Inst, uint32_t PosZ);

/**
 * De-Initialise the given surface.
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] PosZ - Layer we're releasing
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutLayerRelease(void *Inst, uint32_t PosZ);

/**
 * Enable/disable DISCOM for a specific layer
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] PosZ - Layer we're working with
 * @param[in] Enable - 1 to enable, 0 to disable
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutLayerDiscomEnable(void *Inst, uint32_t PosZ, bool Enable);


/**
 * Set the position of the DISCOM region of the layer
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] PosX - posX of the region. Must be a multiple of 2 for M3-W(+)
 * @param[in] PosY - posY of the region
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutLayerDiscomPosSet(void *Inst, uint32_t PosX, uint32_t PosY);

/**
 * Set the size of the DISCOM region of the layer
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] Width - The region's width in pixels. Must be a multiple of 2 for M3-W(+)
 * @param[in] Height - The region's Height in pixels
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutLayerDiscomSizeSet(void *Inst, uint32_t Width, uint32_t Height);

/**
 * Set the expected CRC
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] Crc - Expected CRC of the region
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutLayerDiscomCrcSet(void *Inst, uint32_t Crc);

/**
 * Set the callback when a CRC mismatch occurs
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] Callback - Callback called when mismatch occurs
 * @param[in] CbArg - Argument passed to the callback
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutLayerDiscomCbSet(void *Inst, r_wm_DiscomCb_t Callback, void *CbArg);

/**
 * Get the CRC of the DISCOM region
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[out] pCrc - CRC of the region
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutLayerDiscomCrcGet(void *Inst, uint32_t *pCrc);

/**
 * Clear the DISCOM comparison status
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutLayerDiscomClearStatus(void *Inst);

/**
 * Function: R_WMDRV_Sys_DocInit
 * @brief Initialize DOC driver.
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] DOC_unit - the DOC unit
 * @param[in] DU_ch - the channel of DU
 * @param[in] DU_param - the parameters of DU to set DOC's registers
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_DocInit(void * Inst, uint32_t DOC_unit, uint32_t DU_ch, r_wm_Doc_DUParam_t DU_param);

/**
 * Function: R_WMDRV_Sys_DocDeInit
 * @brief De-Initialize DOC driver.
 *
 * @param[in] DOC_unit - the DOC unit
 * @param[in] DU_ch - the channel of DU
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_DocDeInit(uint32_t DOC_unit, uint32_t DU_ch);

/**
 * Function: R_WMDRV_Sys_DocConfigureMonitor
 * @brief Set the parameter for a DOC monitor
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] DU_ch - the channel of DU
 * @param[in] params - the parameters for the video monitors
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_DocConfigureMonitor(void * Inst, uint32_t DU_ch, r_wm_Doc_MonitorParam_t params);

/**
 * Function: R_WMDRV_Sys_DocEnable
 * @brief Set the parameter for DOC driver.
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] Enable - the flag for DOC Enable/Disable
 * @param[in] VOM_num - the number of using Video Output Monitor(s)
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_DocEnable(void * Inst, uint32_t Enable, int VOM_num);

/**
 * Function: R_WMDRV_Sys_ActMonEnable
 * @brief Set the parameter for DOC driver.
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] DU_ch - the channel of DU
 * @param[in] Enable - the flag for DOC Enable/Disable
  * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_ActMonEnable(void *Inst, uint32_t DU_Ch, uint32_t Enable,
                        uint32_t upper_time, uint32_t lower_time);

/**
 * Function: R_WMDRV_Sys_DocCbSet
 * @brief Set the Callback and enable VOM interrupts
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] Callback - pointer to Callback function. See @ref r_wm_DocCb_t
 * @param[in] CbArg - parameter for the callback function.
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_DocCbSet(void *Inst, r_wm_DocCb_t Callback, void *CbArg);

/**
 * Function: R_WMDRV_Sys_DocClearStatus
 * @brief Clear all DOC errors
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_DocClearStatus(void *Inst);

/**
 * Function: R_WMDRV_Sys_DocGetStatus
 * @brief Get DOC status flags
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_DocGetStatus(void *Inst);

/**
 * Configure the Alpha for the window with the given value of alpha ratio
 *
 * @param[in] Inst - pointer to system dependent Vout inst. See @ref R_WMDRV_Sys_InstInit;
 * @param[in] Alpha - Alpha ratio value
 * @retval Wm error, see @ref r_wm_Error_t
 */
r_wm_Error_t R_WMDRV_Sys_VoutSurfaceAlphaSet (void * Inst, uint8_t Alpha, uint32_t rpfid);


/** @} */ /*end of group WMDrvSys*/

#ifdef __cplusplus
}
#endif

#endif /* R_WMDRV_SYS_H_ */
