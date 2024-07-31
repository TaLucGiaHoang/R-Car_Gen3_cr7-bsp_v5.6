/*************************************************************************************************************
* lvds_regs_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include "r_lvds_regs.h"
#include "r_print_api.h"

uint32_t R_LVDS_PRV_RegRead(uintptr_t Address)
{
    return *((volatile uint32_t *)Address);
}

void R_LVDS_PRV_RegWrite(uintptr_t Address, uint32_t Value)
{
    *((volatile uint32_t *) Address)  = Value;
}

uintptr_t R_LVDS_PRV_GetRegBase(uint32_t LvdsUnit)
{
    uintptr_t base_address = 0;

    switch (LvdsUnit)
    {
    case 0:
        base_address = (uintptr_t)R_LVDS0_BASE;
        break;
    case 1:
        base_address = (uintptr_t)R_LVDS1_BASE;
        break;
    default:
        R_PRINT_Log("[R_LVDS_PRV_GetRegBase] : LVDS Unit No is Invalid\r\n");
        break;/* Wrong device unit */
    }
    return base_address;
}
