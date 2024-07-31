/*************************************************************************************************************
* cio_ether_status_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#ifndef R_CIO_ETHER_STATUS_H
#define R_CIO_ETHER_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
   Section: Includes
 */

#include "rcar-xos/osal/r_osal.h"
#include "r_cio_bridge.h"
#include "r_cio_ether_main.h"
#include "r_cio_ether_status.h"
#include "ether_api.h"
#include "r_print_api.h"


/*******************************************************************************
   Section: Global functions
 */

/*******************************************************************************
   Function: R_CIO_ETHER_PRV_Status
 */

ssize_t R_CIO_ETHER_PRV_Status(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    ssize_t result = 0;
#if (ETH_GET_ETHER_STATS_API == STD_ON)
    Std_ReturnType ret = E_OK;

    r_cio_ether_get_ether_stats *p_ether_get_ether_stats = (r_cio_ether_get_ether_stats*)Par1;
    ret = Eth_GetEtherStats(p_ether_get_ether_stats->CtrlIdx, &(p_ether_get_ether_stats->etherStats));

    if(ret != E_OK)
    {
        R_PRINT_Log("[CioEtherStatus]: R_CIO_ETHER_PRV_Status Eth_GetEtherStats failed(%d).\n", ret);
        result = -1;
    }
#endif
    return result;
}

#ifdef __cplusplus
}
#endif

#endif /* R_CIO_ETHER_STATUS_H */
