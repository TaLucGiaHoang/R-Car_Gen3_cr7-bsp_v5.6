
/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHLWIPER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : main.c
 * Version      : 1.00
 * Description  :
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 27.10.2020 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_osal_api.h"
#include "cio_server.h"
#include "r_cio_api.h"
#include "taurus_app.h"
#include "r_lwip_lib.h"
#include "rtp_player.h"
#include "r_osal_api.h"
#include "r_print_api.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#if !defined(ETHER_ENABLE) || !defined(LWIP_SUPPORTED) || !defined(RIVP_SUPPORTED)
#error "rtp_player requires the ETHER_ENABLE, LWIP_SUPPORTED, and RIVP_SUPPORTED options. Please execute 'cmake -DETHER_ENABLE=ON -DLWIP_SUPPORTED=ON -DRIVP_SUPPORTED=ON <build-dir>' to enable it."
#endif

/**********************************************************************************************************************
 Global variables
 *********************************************************************************************************************/
osal_memory_manager_handle_t g_hndl_mmngr = NULL;

/**********************************************************************************************************************
 Function definitions
 *********************************************************************************************************************/
int start_thread(void* arg)
{
    e_osal_return_t osal_ret;

    /* Init OSAL */
    osal_ret  = R_OSAL_Initialize();
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("OSAL Initialization failed with error %d\n", osal_ret);
        return(-1);
    }

    R_CIO_Init();

    rtp_player_main();
    
    return(0);
}


int main(void)
{
    R_OSAL_StartOS(start_thread);
    return 0;
}
