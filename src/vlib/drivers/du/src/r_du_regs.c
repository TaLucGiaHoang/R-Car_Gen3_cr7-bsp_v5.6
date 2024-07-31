/*************************************************************************************************************
* du_regs_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include "r_du_config.h"
#include "r_du_regs.h"
#include "r_print_api.h"

void R_DU_PRV_WriteReg(uintptr_t Address, uint32_t Value)
{
    *((volatile uint32_t *)Address) = Value;
}

uint32_t R_DU_PRV_ReadReg(uintptr_t Address)
{
    return *((volatile uint32_t *)Address);
}

uintptr_t R_DU_PRV_GetRegBase(r_du_Unit_t DuUnit)
{
    uintptr_t base_address = 0;

    switch (DuUnit) {
    case R_DU_DU0:
        base_address = (uintptr_t)R_DU0_BASE;
        break;
    case R_DU_DU1:
        base_address = (uintptr_t)R_DU1_BASE;
        break;
    case R_DU_DU2:
        base_address = (uintptr_t)R_DU2_BASE;
        break;
    case R_DU_DU3:
        base_address = (uintptr_t)R_DU3_BASE;
        break;
    default:
        R_PRINT_Log("Wrong device DuUnit %d\r\n", DuUnit);
        break;
        /* Wrong device unit */
    }
    return base_address;
}

uintptr_t R_DU_PRV_GetGroupRegBase(r_du_Unit_t DuUnit)
{
    uintptr_t base_address = 0;

    switch (DuUnit) {
    case R_DU_DU0:
    case R_DU_DU1:
        base_address = (uintptr_t)R_DU0_BASE;
        break;
    case R_DU_DU2:
    case R_DU_DU3:
        base_address = (uintptr_t)R_DU2_BASE;
        break;
    default:
        R_PRINT_Log("Wrong device DuUnit(Group) %d\r\n", DuUnit);
        break;  /* Wrong device unit */
    }
    return base_address;
}
