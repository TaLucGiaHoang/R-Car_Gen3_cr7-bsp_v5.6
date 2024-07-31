/*************************************************************************************************************
* i2c_regs_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "r_i2c_regs.h"
#include "r_print_api.h"

void R_I2C_PRV_RegWrite32(uintptr_t Addr, uint32_t Data)
{
    *((volatile uint32_t *)Addr) = Data;
    return;
}

uint32_t R_I2C_PRV_RegRead32(uintptr_t Addr)
{
    return *((volatile uint32_t *)Addr);
}

uintptr_t R_I2C_PRV_GetRegbase(r_i2c_Unit_t I2cUnit)
{
    uintptr_t ret = 0;

    switch (I2cUnit) {
    case R_I2C_IF0:
        ret = R_I2C_IF0_BASE;
        break;
    case R_I2C_IF1:
        ret = R_I2C_IF1_BASE;
        break;
    case R_I2C_IF2:
        ret = R_I2C_IF2_BASE;
        break;
    case R_I2C_IF3:
        ret = R_I2C_IF3_BASE;
        break;
    case R_I2C_IF4:
        ret = R_I2C_IF4_BASE;
        break;
    case R_I2C_IF5:
        ret = R_I2C_IF5_BASE;
        break;
    case R_I2C_IF6:
        ret = R_I2C_IF6_BASE;
        break;
    case R_I2C_IF7:
        ret = R_I2C_IF7_BASE;
        break;
    default:
        R_PRINT_Log("[R_I2C_PRV_GetRegbase] : Wrong I2C Unit %d\r\n", I2cUnit);
        break;
    }

    return ret;
}
