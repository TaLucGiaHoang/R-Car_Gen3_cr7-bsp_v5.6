/*************************************************************************************************************
* cio_wm_connect_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "rcar-xos/osal/r_osal.h"
#include "r_cio_bridge.h"
#include "r_wmdrv_api.h"
#include "r_cio_wm_main.h"
#include "r_cio_wm_connect.h"
#include "r_print_api.h"


/**@defgroup CIO Window Manager
 *  @brief CIO Window Manager device connect module
 *  @{
 */

/*******************************************************************************
   See @ref R_CIO_WM_PRV_Open
 */

int R_CIO_WM_PRV_Open(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    int ret = 0;

    /* Add instance to wm device */
    ret = (int)R_WMDRV_AddProcToInstance((r_wm_Dev_t)Addr);
    R_PRINT_Log("CIO WM device %d open ... ", Addr);
    if (0 == ret) {
        R_PRINT_Log(" OK\r\n");
    } else {
        R_PRINT_Log(" NG!\r\n");
    }

    return ret;
}


/*******************************************************************************
   See @ref R_CIO_WM_PRV_Close
 */

int R_CIO_WM_PRV_Close(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    int x;

    /* Add instance to wm device */
    x = R_WMDRV_GetProcNbUsingInstance((r_wm_Dev_t)Addr);
    R_PRINT_Log("CIO WM device %d close ... ", Addr);
    if (0 == x) {
        R_PRINT_Log(" OK\r\n");
    } else {
        R_PRINT_Log(" NG!\r\n");
    }

    return x;
}


/**  @} */
