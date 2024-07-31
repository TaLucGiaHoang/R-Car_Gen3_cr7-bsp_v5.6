/*************************************************************************************************************
* cio_ether_connect_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/



/*******************************************************************************
   Section: Includes
 */

#include "rcar-xos/osal/r_osal.h"
#include "r_cio_bridge.h"
#include "r_cio_ether_public.h"
#include "r_cio_ether_connect.h"
#include "r_cio_ether_irq.h"
#include "r_print_api.h"


/*******************************************************************************
   Section: Global (private) functions
 */

/*******************************************************************************
   Function: R_CIO_ETHER_PRV_Open
 */

int R_CIO_ETHER_PRV_Open(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    int ret;
    ret = (int)R_CIO_ETHER_PRV_IrqInit();
    if (0 != ret)
    {
        R_PRINT_Log("[CioEtherConnect]: R_CIO_ETHER_PRV_Open R_CIO_ETHER_PRV_IrqInit failed(%d).\n", ret);
    }
    return ret;
}


/*******************************************************************************
   Function: R_CIO_ETHER_PRV_Close
 */

int R_CIO_ETHER_PRV_Close(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    int ret;
    ret = (int)R_CIO_ETHER_PRV_IrqDeInit();
    if (0 != ret)
    {
        R_PRINT_Log("[CioEtherConnect]: R_CIO_ETHER_PRV_Close R_CIO_ETHER_PRV_IrqDeInit failed(%d).\n", ret);
    }
    return ret;
}

