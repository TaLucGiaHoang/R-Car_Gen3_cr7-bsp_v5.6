/*************************************************************************************************************
* taurus_camera_drv_api
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/**@defgroup group1_taurus_camera Taurus Camera server public library API
 * @ingroup taurus
 *
 * @brief This Cortex-R7 Taurus Camera server is a part of the Taurus firmware.
 *
 * This server depends on CIO and ROS to communicate with the kernel driver
 * The Interface to the kernel is a typical I/O control with functions like
 * open, read, ioctl, etc.
 *
 * @{
 */

#ifndef R_CAMERA_DRV_API_H
#define R_CAMERA_DRV_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rcar-xos/osal/r_osal.h"

/*******************************************************************************
 * Function Name: R_Camera_DRV_Open
 */
/**
 * @brief Opens the Camera server. Implemented empty.
 *
 * @param[in] Guest       - Guest ID (not used)
 * @param[in] Cmd         - Pointer to the command to process (not used)
 *
 * @return  == 0 - OK
 * @return  != 0 - NG
 */
int R_Camera_DRV_Open(int Guest, R_TAURUS_CmdMsg_t *Cmd);

/*******************************************************************************
 * Function Name: R_Camera_DRV_Close
 */
/**
 * @brief Closes the Camera server. Implemented empty.
 *
 * @param[in] Guest       - Guest ID (not used)
 * @param[in] Cmd         - Pointer to the command to process (not used)
 *
 * @return  == 0 - OK
 * @return  != 0 - NG
 */
int R_Camera_DRV_Close(int Guest, R_TAURUS_CmdMsg_t *Cmd);

/*******************************************************************************
 * Function Name: R_Camera_DRV_Read
 */
/**
 * @brief Read request to the Camera server. Implemented empty.
 *
 * @param[in] Guest       - Guest ID (not used)
 * @param[in] Cmd         - Pointer to the command to process (not used)
 *
 * @return  == 0 - OK
 * @return  != 0 - NG
 */
ssize_t R_Camera_DRV_Read(int Guest, R_TAURUS_CmdMsg_t *Cmd);

/*******************************************************************************
 * Function Name: R_Camera_DRV_Write
 */
/**
 * @brief Write request to the Camera server. Implemented empty.
 *
 * @param[in] Guest       - Guest ID (not used)
 * @param[in] Cmd         - Pointer to the command to process (not used)
 *
 * @return  == 0 - OK
 * @return  != 0 - NG
 */
ssize_t R_Camera_DRV_Write(int Guest, R_TAURUS_CmdMsg_t *Cmd);

/*******************************************************************************
 * Function Name: R_Camera_DRV_IoCtl
 */
/**
 * @brief Request an IOCTL command to the Camera server.
 *
 * @param[in] Guest       - Guest ID (not used)
 * @param[in] Cmd         - Pointer to the command to process (must not be NULL)
 *
 * @return  == 0 - OK
 * @return  != 0 - NG
 */
ssize_t R_Camera_DRV_IoCtl(int Guest, R_TAURUS_CmdMsg_t *Cmd);

/*******************************************************************************
 * Function Name: R_Camera_DRV_Status
 */
/**
 * @brief Query status of the Camera server. Implemented empty.
 *
 * @param[in] Guest       - Guest ID (not used)
 * @param[in] Cmd         - Pointer to the command to process (not used)
 *
 * @return  == 0 - OK
 * @return  != 0 - NG
 */
ssize_t R_Camera_DRV_Status(int Guest, R_TAURUS_CmdMsg_t *Cmd);

/*******************************************************************************
 * Function Name: R_Camera_DRV_SetSendSignalImpl
 */
/**
 * @brief Store the function pointer callback that is called from the server
 *
 * @param[in] SendSignalImpl       - Function callback
 */
void R_Camera_DRV_SetSendSignalImpl(int (*SendSignalImpl)(int Guest, R_TAURUS_SignalId_t Signal, void* Option));

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* R_CAMERA_DRV_API_H */
