/*************************************************************************************************************
* i2c_adv7511_api_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2018-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "r_i2c_api.h"
#include "r_adv7511_api.h"
#include "r_cpg_api.h"
#include "r_delay_api.h"
#include "r_print_api.h"

/**
 * Section local function prototypes
 */
static int32_t loc_RegWrite(r_i2c_Unit_t Unit,
                             uint32_t SlaveI2cAddr,
                             uint8_t Reg,
                             uint8_t Value,
                             uint8_t Mask);
static void loc_I2cClkEnable(r_i2c_Unit_t Unit);
static void loc_I2cReset(r_i2c_Unit_t Unit);

/**
 * Section local functions
 */

static int32_t loc_RegWrite(r_i2c_Unit_t Unit,
                             uint32_t SlaveI2cAddr,
                             uint8_t Reg,
                             uint8_t Value,
                             uint8_t Mask)
{
    uint8_t  bytes[2];
    uint32_t i = 0;
    int32_t  ret = 0;

    if (0xff == Mask) {
        bytes[0] = Reg;
        bytes[1] = Value;
        loc_I2cReset(Unit);
        i = R_I2C_Write(Unit, SlaveI2cAddr, &bytes[0], 2);
        if (i < 2) {
            ret  = -1;
            R_PRINT_Log("[loc_RegWrite : adv7482] R_I2C_Write return value(%u) is invalid. :  Failed(-1)\r\n",i);
        }
    } else {
        /* set the new value only for those bits specified in the
         * mask */
        loc_I2cReset(Unit);
        i = R_I2C_ReadRegMap(Unit, SlaveI2cAddr, Reg, &bytes[1], 1);
        if (i < 1) {
            ret  = -1;
            R_PRINT_Log("[loc_RegWrite : adv7482] R_I2C_ReadRegMap return value(%u) is invalid. :  Failed(-1)\r\n",i);
        } else {

            bytes[0] = Reg;
            bytes[1] &= ~Mask;
            bytes[1] |= Value;
            loc_I2cReset(Unit);
            i = R_I2C_Write(Unit, SlaveI2cAddr, &bytes[0], 2);
            if (i < 2) {
                ret  = -1;
                R_PRINT_Log("[loc_RegWrite : adv7482] loc_RegWrite return value(%u) is invalid. :  Failed(-1)\r\n",i);
            }
        }
    }

    return ret;
}

static void loc_I2cReset(r_i2c_Unit_t Unit)
{

    switch (Unit) {
    case R_I2C_IF0:
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT31, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT31, 0);
        break;
    case R_I2C_IF4:
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT27, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT27, 0);
        break;
    default:
        R_PRINT_Log("[loc_I2cReset : adv7511] I2C Unit is invalid.\r\n");
        break;
    }

    return;
}

static void loc_I2cClkEnable(r_i2c_Unit_t Unit)
{
    switch (Unit) {
    case R_I2C_IF0:
        R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT31, false);
        break;
    case R_I2C_IF4:
        R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT27, false);
        break;
    default:
        R_PRINT_Log("[loc_I2cClkEnable : adv7511] I2C Unit is invalid.\r\n");
        break;
    }

    return;
}


/**
 * Section Global functions
 */
int32_t R_ADV7511_Init(r_i2c_Unit_t Unit, uint32_t SlaveI2cAddr)
{
    int ret = 0;

    loc_I2cClkEnable(Unit);

    /* Power-up the TX*/
    ret = loc_RegWrite(Unit, SlaveI2cAddr, 0x41, 0x00, 0x01 << 6);
    if (ret < 0) {
        goto exit;
    }

    /* Fixed registers that must be set on power-up */
    ret = loc_RegWrite(Unit, SlaveI2cAddr, 0x98, 0x03, 0xff);
    if (ret < 0) {
        goto exit;
    }

    ret = loc_RegWrite(Unit, SlaveI2cAddr, 0x9a, 0x7 << 5, 0x7 << 5);
    if (ret < 0) {
        goto exit;
    }

    ret = loc_RegWrite(Unit, SlaveI2cAddr, 0x9c, 0x30, 0xff);
    if (ret < 0) {
        goto exit;
    }

    ret = loc_RegWrite(Unit, SlaveI2cAddr, 0x9d, 0x01, 0x03);
    if (ret < 0) {
        goto exit;
    }

    ret = loc_RegWrite(Unit, SlaveI2cAddr, 0xa2, 0xa4, 0xff);
    if (ret < 0) {
        goto exit;
    }

    ret = loc_RegWrite(Unit, SlaveI2cAddr, 0xa3, 0xa4, 0xff);
    if (ret < 0) {
        goto exit;
    }

    ret = loc_RegWrite(Unit, SlaveI2cAddr, 0xe0, 0xd0, 0xff);
    if (ret < 0) {
        goto exit;
    }

    ret = loc_RegWrite(Unit, SlaveI2cAddr, 0xf9, 0x00, 0xff);
    if (ret < 0) {
        goto exit;
    }

    /* Mostly we use the default values. Just set the following few registers: */

    /* Set up Video Input mode */

    /* Video Format ID 0 -> 4:4:4 */
    ret = loc_RegWrite(Unit, SlaveI2cAddr, 0x15, 0x00, 0x0f);
    if (ret < 0) {
        goto exit;
    }

    /* Input Color Depth 8 bit */
    ret = loc_RegWrite(Unit, SlaveI2cAddr, 0x16, 0x03 << 4, 0x03 << 4);
    if (ret < 0) {
        goto exit;
    }

    /* Input Style 1 */
    ret = loc_RegWrite(Unit, SlaveI2cAddr, 0x16, 0x02 << 2, 0x03 << 2);
    if (ret < 0) {
        goto exit;
    }

    /* Set up Video Output mode */

    /* Output format */
    ret = loc_RegWrite(Unit, SlaveI2cAddr, 0x16, 0x00, 0x01 << 7);
    if (ret < 0) {
        goto exit;
    }

    /* DVI Mode Select */
    ret = loc_RegWrite(Unit, SlaveI2cAddr, 0xaf, 0x00, 0x02);
    if (ret < 0) {
        goto exit;
    }

    /* CEC Power Down */
    ret = loc_RegWrite(Unit, SlaveI2cAddr, 0xe2, 0x01, 0x01);
    if (ret < 0) {
        goto exit;
    }

exit:
    return ret;
}

