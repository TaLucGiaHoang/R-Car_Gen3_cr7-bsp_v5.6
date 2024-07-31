/*************************************************************************************************************
* vsp2_regs_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include "r_vsp2_regs.h"
#include "r_print_api.h"

uint32_t R_VSP2_PRV_RegRead(uintptr_t Address)
{
    return *((volatile unsigned int *)Address);
}

void R_VSP2_PRV_RegWrite(uintptr_t Address, uint32_t Value)
{
    *((volatile unsigned int *)Address) = Value;
}

uintptr_t R_VSP2_PRV_GetRegBase(r_vsp2_Unit_t Vsp2Unit)
{
    uintptr_t base_address = 0;

    switch (Vsp2Unit) {
    case R_VSP2_VSPBC:
        base_address = R_VSPBC_BASE;
        break;
    case R_VSP2_VSPBD:
        base_address = R_VSPBD_BASE;
        break;
    case R_VSP2_VSPB:
        base_address = R_VSPB_BASE;
        break;
    case R_VSP2_VSPBS:
        base_address = R_VSPBS_BASE;
        break;
    case R_VSP2_VSPI0:
        base_address = R_VSPI0_BASE;
        break;
    case R_VSP2_VSPI1:
        base_address = R_VSPI1_BASE;
        break;
    case R_VSP2_VSPD0:
        base_address = R_VSPD0_BASE;
        break;
    case R_VSP2_VSPDL:
        base_address = R_VSPDL_BASE;
        break;
    case R_VSP2_VSPD1:
        base_address = R_VSPD1_BASE;
        break;
    case R_VSP2_VSPD2:
        base_address = R_VSPD2_BASE;
        break;
    default:
        R_PRINT_Log("[R_VSP2_PRV_GetRegBase] : VSP2 Unit No is Invalid.\r\n");
        break;
    }
    return base_address;
}
