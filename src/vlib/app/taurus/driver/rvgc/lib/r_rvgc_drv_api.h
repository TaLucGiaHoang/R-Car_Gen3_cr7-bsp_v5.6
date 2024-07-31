/*************************************************************************************************************
* taurus_rvgc_drv_api
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/**@defgroup group1_taurus_rvgc Taurus RVGC server public library API
 * @ingroup taurus
 *
 * @brief This Cortex-R7 Taurus RVGC server is a part of the Taurus firmware.
 *
 * This server depends on CIO and ROS to communicate with the kernel driver
 * The Interface to the kernel is a typical I/O control with functions like
 * open, read, ioctl, etc.
 *
 * @{
 */

#ifndef R_RVGC_DRV_API_H
#define R_RVGC_DRV_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rcar-xos/osal/r_osal.h"

/*******************************************************************************
 * Function Name: R_Rvgc_DRV_Open
 */
/**
 * @brief Opens the Rvgc server. Implemented empty.
 *
 * @param[in] Guest       - Guest ID (not used)
 * @param[in] Cmd         - Pointer to the command to process (not used)
 *
 * @return  == 0 - OK
 * @return  != 0 - NG
 */
int R_Rvgc_DRV_Open(int Guest, R_TAURUS_CmdMsg_t *Cmd);

/*******************************************************************************
 * Function Name: R_Rvgc_DRV_Close
 */
/**
 * @brief Closes the Rvgc server. Implemented empty.
 *
 * @param[in] Guest       - Guest ID (not used)
 * @param[in] Cmd         - Pointer to the command to process (not used)
 *
 * @return  == 0 - OK
 * @return  != 0 - NG
 */
int R_Rvgc_DRV_Close(int Guest, R_TAURUS_CmdMsg_t *Cmd);

/*******************************************************************************
 * Function Name: R_Rvgc_DRV_Read
 */
/**
 * @brief Read request to the Rvgc server. Implemented empty.
 *
 * @param[in] Guest       - Guest ID (not used)
 * @param[in] Cmd         - Pointer to the command to process (not used)
 *
 * @return  == 0 - OK
 * @return  != 0 - NG
 */
ssize_t R_Rvgc_DRV_Read(int Guest, R_TAURUS_CmdMsg_t *Cmd);

/*******************************************************************************
 * Function Name: R_Rvgc_DRV_Write
 */
/**
 * @brief Write request to the Rvgc server. Implemented empty.
 *
 * @param[in] Guest       - Guest ID (not used)
 * @param[in] Cmd         - Pointer to the command to process (not used)
 *
 * @return  == 0 - OK
 * @return  != 0 - NG
 */
ssize_t R_Rvgc_DRV_Write(int Guest, R_TAURUS_CmdMsg_t *Cmd);

/*******************************************************************************
 * Function Name: R_Rvgc_DRV_IoCtl
 */
/**
 * @brief Request an IOCTL command to the Rvgc server
 *
 * @param[in] Guest       - Guest ID (not used)
 * @param[in] Cmd         - Pointer to the command to process (must not be NULL)
 *
 * @return  == 0 - OK
 * @return  != 0 - NG
 */
ssize_t R_Rvgc_DRV_IoCtl(int Guest, R_TAURUS_CmdMsg_t *Cmd);

/*******************************************************************************
 * Function Name: R_Rvgc_DRV_Status
 */
/**
 * @brief Query status of the Rvgc server. Implemented empty.
 *
 * @param[in] Guest       - Guest ID (not used)
 * @param[in] Cmd         - Pointer to the command to process (not used)
 *
 * @return  == 0 - OK
 * @return  != 0 - NG
 */
ssize_t R_Rvgc_DRV_Status(int Guest, R_TAURUS_CmdMsg_t *Cmd);

/*******************************************************************************
 * Function Name: R_Rvgc_DRV_SetSendSignalImpl
 */
/**
 * @brief Store the function pointer callback that is called from the server
 *
 * @param[in] SendSignalImpl       - Function callback
 *
 */
void R_Rvgc_DRV_SetSendSignalImpl(int (*SendSignalImpl)(int Guest, R_TAURUS_SignalId_t Signal, void* Option));

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* R_RVGC_DRV_API_H */
