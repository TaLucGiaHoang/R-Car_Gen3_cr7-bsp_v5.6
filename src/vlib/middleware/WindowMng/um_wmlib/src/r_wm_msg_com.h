/*************************************************************************************************************
* WindowMng_wm_msg_com_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_WM_MSG_COM_H_
#define R_WM_MSG_COM_H_

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Title: Window Manager Message communication
 *        Contain the declarations of functions required to send a message
 *        to the CIO WM Device
 *
 */

/*
 * Section Include
 */
#include "r_wm_bridge.h"

/*
 * Section: Global Function Declarations
 */

/** \defgroup PrivFn WM User Bridge functions
 *  \ingroup  PrivDef
 *  @brief WM internal functions
 */

/** \defgroup WMMsgFn WM Private Msg functions
 *  \ingroup  PrivFn
 *  @brief Functions handling the communication with the WM driver
 *
 *  Those functions implementation depends is based on the OSAL.
 *
 *  @{
 */

/**
 * Initialise and open a communication channel with the WM Device
 *
 * @param[in]  WmUnit - Wm Device. @ref r_wm_Dev_t
 * @param[out] ComCh - pointer to the communication channel reference.
 *                     It will be initialised by the OSAL function opening the channel.
 *
 * @retval WM Error @ref r_wm_Error_t
 */
r_wm_Error_t R_WM_Priv_MsgComOpen(r_wm_Dev_t WmUnit, int32_t *ComCh);

/**
 * Close the existing communication channel between the current
 * process and the CIO WM Device
 *
 * @param[in] ComCh   - Comunication channel to be closed
 *
 * @retval WM Error @ref r_wm_Error_t
 */
r_wm_Error_t R_WM_Priv_MsgComClose(int32_t ComCh);

/**
 * Send the WM message to the CIO WM Device and wait for the return information
 *
 * @param[in] ComCh   - reference to the commmunication to be used.
 * @param[in] MsgTx   - pointer to the message to be transmitted. @ref r_wm_MsgTx_t
 * @param[in] MsgRx   - pointer to the a structure @ref r_wm_MsgRx_t where the return
 *                      Information shall be stored.
 * @retval WM Error @ref r_wm_Error_t
 */
r_wm_Error_t R_WM_Priv_MsgSendReceive(int32_t ComCh, r_wm_MsgTx_t *MsgTx,
                r_wm_MsgRx_t * MsgRx);

/** @} */ /* end group WMMsgFn */

#ifdef __cplusplus
}
#endif

#endif /* R_WM_MSG_COM_H_ */



