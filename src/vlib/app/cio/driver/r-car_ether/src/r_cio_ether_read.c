/*************************************************************************************************************
* cio_ether_read_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/*******************************************************************************
   Section: Includes
 */

#include "rcar-xos/osal/r_osal.h"
#include "ether_api.h"
#include "r_cio_bridge.h"
#include "r_cio_ether_main.h"
#include "r_cio_ether_read.h"
#include <string.h>

/*******************************************************************************
   Section: Global private functions
 */

/*******************************************************************************
   Function: R_CIO_ETHER_PRV_Read
 */

ssize_t R_CIO_ETHER_PRV_Read(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    ssize_t result = 0;
    #if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_OFF)
    r_cio_ether_receive* p_ether_rx = (r_cio_ether_receive*)Par1;
    Eth_Receive(p_ether_rx->CtrlIdx, &(p_ether_rx->RxStatus));
    #endif
    return result;
}
