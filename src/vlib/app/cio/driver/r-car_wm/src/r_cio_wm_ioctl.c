/*************************************************************************************************************
* cio_wm_ioctl_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#include <string.h>

#include "rcar-xos/osal/r_osal.h"
#include "r_cio_bridge.h"
#include "r_wmdrv_api.h"
#include "r_print_api.h"

/** @defgroup CIO Window Manager
 * @brief CIO Window Manager device ioctl module
 * @{
 */

/**
 * Section: Types
 */

/**
 * @fn  r_cio_wm_IoctlFnp_t
 * @brief Pointer to a WM driver function.
 */
typedef r_wm_Error_t (*r_cio_wm_IoctlFnp_t)(r_wmdrv_VOutInstance_t *Inst,
                                            r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData);

/**
 * Section: local variables
 */
/**
 * @var     locWmDrvFunc
 * @brief   table of WM driver function pointers. Each of this function correspond
 *          to one WM command
   @ref     r_cio_wm_IoctlFnp_t
 */
static r_cio_wm_IoctlFnp_t locWmDrvFunc[R_WM_MSG_CMD_LAST] =
{
    &R_WMDRV_VoutInit,                          /* R_WM_MSG_CMD_INIT*/
    &R_WMDRV_VoutDeinit,                        /* R_WM_MSG_CMD_DEINIT */
    &R_WMDRV_VoutRegisterEvent,                 /* R_WM_MSG_CMD_REGISTER_EVENT*/
    &R_WMDRV_VoutEnable,                        /* R_WM_MSG_CMD_SC_ENABLE */
    &R_WMDRV_VoutBkgCol,                        /* R_WM_MSG_CMD_SC_BKGCL */
    &R_WMDRV_VoutTimings,                       /* R_WM_MSG_CMD_SC_TIMINGS */
    &R_WMDRV_VoutColFmt,                        /* R_WM_MSG_CMD_SC_FMT */
    &R_WMDRV_VoutLayerGet,                      /* R_WM_MSG_CMD_SURF_GET */
    &R_WMDRV_VoutLayerPos,                      /* R_WM_MSG_CMD_SURF_POS */
    &R_WMDRV_VoutLayerSize,                     /* R_WM_MSG_CMD_SURF_SIZE */
    &R_WMDRV_VoutLayerFb,                       /* R_WM_MSG_CMD_SURF_BUF */
    &R_WMDRV_VoutLayerFormat,                   /* R_WM_MSG_CMD_SURF_FMT */
    &R_WMDRV_VoutFlush,                         /* R_WM_MSG_CMD_SC_FLUSH */
    &R_WMDRV_VoutLayerRelease,                  /* R_WM_MSG_CMD_SURF_RELEASE */
    &R_WMDRV_DocConfigure,                      /* R_WM_MSG_CMD_DOC_CFG */
    &R_WMDRV_DocEnable,                         /* R_WM_MSG_CMD_DOC_ENABLE */
    &R_WMDRV_DocClearStatus,                    /* R_WM_MSG_CMD_DOC_CLEAR */
    &R_WMDRV_VoutLayerDiscomConfigure,          /* R_WM_MSG_CMD_SURF_DISCOM_CFG */
    &R_WMDRV_VoutSurfAlphaSet                   /* R_WM_MSG_CMD_SURFACE_ALPHA_SET */
};

/**
 * Section: Global functions
 */

/**
 * @brief R_CIO_WM_PRV_IoCtl
 */
ssize_t R_CIO_WM_PRV_IoCtl(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    ssize_t ret = 0;
    r_wmdrv_VOutInstance_t * inst;

    /* Par1:  r_wm_MsgTx_t - pointer to the Msg sent by the WM user lib
     * Par2:  r_wm_MsgRx_t - pointer to the Msg structure that the Wm server shall
     *                     shall write to, when a return value is required
     * Par3:  NULL      - Not used at the moment
     */
    r_wm_MsgTx_t * msgRead = Par1;
    r_wm_MsgRx_t * msgWrite = Par2;

    /* Get the WM Instance */
    inst = R_WMDRV_FindInstance((r_wm_Dev_t)Addr);
    if (0 == inst) {
        R_PRINT_Log("[CioWmIoctl]: R_CIO_WM_PRV_IoCtl R_WMDRV_FindInstance failed. Addr(%d)\r\n", Addr);
        ret = -1;
    } else {
        if (R_WM_MSG_CMD_LAST > msgRead->MsgCmd) {
            if (0 != locWmDrvFunc[msgRead->MsgCmd]) {
                msgWrite->Err = locWmDrvFunc[msgRead->MsgCmd](inst,
                                                              &(msgRead->Data), &(msgWrite->Data));
            } else {
                R_PRINT_Log("[CioWmIoctl]: R_CIO_WM_PRV_IoCtl locWmDrvFunc failed. MsgCmd(%d)\r\n", msgRead->MsgCmd);
                msgWrite->Err = R_WM_ERR_INVALID_CMD;
                ret = -1;
            }
        } else {
            msgWrite->Err = R_WM_ERR_INVALID_CMD;
            R_PRINT_Log("[CioWmIoctl]: R_CIO_WM_PRV_IoCtl Message command failed(%d).\r\n", msgRead->MsgCmd);
            ret = -1;
        }
    }
    return ret;
}


/** @} */
