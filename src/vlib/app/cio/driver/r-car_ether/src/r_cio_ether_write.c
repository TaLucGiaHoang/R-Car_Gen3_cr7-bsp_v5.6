/*************************************************************************************************************
* cio_ether_write_c
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
#include "r_cio_ether_main.h"
#include "r_cio_ether_write.h"
#include "ether_api.h"
#include "r_print_api.h"

#include <string.h>


/*******************************************************************************
   Function: R_CIO_ETHER_PRV_Write
 */

ssize_t R_CIO_ETHER_PRV_Write(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    ssize_t result = 0;
    r_cio_ether_transmit *p_ether_tx = (r_cio_ether_transmit *)Par1;
    Std_ReturnType ret;
    ret = Eth_Transmit (p_ether_tx->CtrlIdx, p_ether_tx->BufIdx, p_ether_tx->FrameType, p_ether_tx->TxConfirmation,
                p_ether_tx->LenByte, (uint8_t *)&(p_ether_tx->PhysAddrPtr));

    if(ret != E_OK)
    {
        R_PRINT_Log("[CioEtherWrite]: R_CIO_ETHER_PRV_Write Eth_Transmit failed(%d).\n", ret);
        result = -1;
    }
    return result;
}
