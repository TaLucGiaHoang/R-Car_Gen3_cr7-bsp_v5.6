/*************************************************************************************************************
* cio_wm_main_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "rcar-xos/osal/r_osal.h"
#include "r_cio_bridge.h"
#include "r_wmdrv_api.h"
#include "r_cio_wm_main.h"
#include "r_cio_wm_ioctl.h"
#include "r_cio_wm_read.h"
#include "r_cio_wm_write.h"
#include "r_cio_wm_status.h"
#include "r_cio_wm_connect.h"
#include "r_print_api.h"

/* Forward declaration, .. */
const r_cio_Driver_t WmDriver;

/**@defgroup CIO Window Manager
 * @brief    CIO Window Manager device main module
 * @{
 */

/**
 * See @ref R_CIO_WM_PRV_Init
 */
int R_CIO_WM_PRV_Init(size_t Addr, int IrqNum)
{
    int ret = 0;
    r_wm_Dev_t wm_dev = (r_wm_Dev_t)Addr;

    R_PRINT_Log("CIO %s driver device %d init ... ", WmDriver.Name, Addr);

    ret = R_WMDRV_Init(wm_dev);
    if (0 == ret) {
        R_PRINT_Log(" OK\r\n");
    } else {
        R_PRINT_Log(" NG!\r\n");
    }

    return ret;
}

/**
 * See @ref R_CIO_WM_PRV_DeInit
 */
int R_CIO_WM_PRV_Deinit(size_t Addr)
{
    int ret = 0;
    r_wm_Dev_t wm_dev = (r_wm_Dev_t)Addr;

    R_PRINT_Log("CIO %s driver device %d de-init ... ", WmDriver.Name, Addr);

    ret = R_WMDRV_Deinit(wm_dev);
    if (0 == ret) {
        R_PRINT_Log(" OK\r\n");
    } else {
        R_PRINT_Log(" NG!\r\n");
    }

    return ret;
}

/**
 * See @ref WmDriver
 */
const r_cio_Driver_t WmDriver = {
    "R-Car_WM",
    R_CIO_WM_PRV_Init,
    R_CIO_WM_PRV_Deinit,
    R_CIO_WM_PRV_Open,
    R_CIO_WM_PRV_Close,
    R_CIO_WM_PRV_Read,
    R_CIO_WM_PRV_Write,
    R_CIO_WM_PRV_IoCtl,
    R_CIO_WM_PRV_Status
};

/** @} */
