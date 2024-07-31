/*************************************************************************************************************
* i2c_5p49v6901_api_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>

#include "r_5p49v6901_api.h"
#include "r_i2c_api.h"
#include "r_cpg_api.h"
#include "r_delay_api.h"
#include "r_print_api.h"

#define LOC_I2C_CHANNEL              R_I2C_IF4
#define IDT5P49V6901_I2C_ADDR        0xD4

typedef struct {
	uint8_t intdiv[2];  // Address: 0x2D/0x3D, 0x2E/0x3E
	uint8_t offset[4];  // Address: 0x22/0x32, 0x23/0x33, 0x24/0x34, 0x25/0x35
} Vclk5_Freq;

const Vclk5_Freq Vclk5_108000kHz = { {0x00, 0xC0}, {0x00, 0x9C, 0x71, 0xC4} };  // 108.0MHz
const Vclk5_Freq Vclk5_148500kHz = { {0x00, 0x90}, {0x00, 0x5D, 0x17, 0x44} };  // 148.5MHz 1920x1080p

const uint8_t VersaclockInitialValue[] = {
	0x61,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,  // 0x00-0x07
	0x00,0xFF,0x01,0xC0,0x00,0xB6,0xB4,0x92,  // 0x08-0x0F
	0x82,0x0C,0x81,0x80,0x00,0x05,0x8C,0x06,  // 0x10-0x17
	0x90,0x00,0x00,0x00,0x9F,0xFF,0xE0,0x80,  // 0x18-0x1F
	0x00,0x81,0x03,0x5A,0x81,0x48,0x00,0x00,  // 0x20-0x27
	0x00,0x00,0x04,0x00,0x00,0x00,0x80,0x00,  // 0x28-0x2F
	0x00,0x81,0x00,0x9C,0x71,0xC4,0x00,0x00,  // 0x30-0x37
	0x00,0x00,0x04,0x00,0x00,0x00,0xC0,0x00,  // 0x38-0x3F
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // 0x40-0x47
	0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,  // 0x48-0x4F
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // 0x50-0x57
	0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,  // 0x58-0x5F
	0x23,0x01,0x23,0x01,0x3B,0x00,0x3B,0x00,  // 0x60-0x67
	0xFC,0x64                                 // 0x68-0x69
};


static uint32_t loc_RegRead(r_i2c_Unit_t I2c, uint8_t SlaveAddr,
                uint8_t Addr, uint8_t *Data, uint32_t NumBytes);
static uint32_t loc_RegWrite(r_i2c_Unit_t I2c, uint8_t SlaveAddr,
                uint8_t Addr, uint8_t Data);
static uint32_t loc_I2cReset(r_i2c_Unit_t Unit);
static r_5p49v6901_Error_t loc_I2cClkEnable(r_i2c_Unit_t Unit);
static uint32_t loc_Write6901(uint8_t addr, uint8_t data);
static uint32_t loc_Read6901(uint8_t addr, uint8_t *pdata, uint32_t num);
static r_5p49v6901_Error_t loc_SetClock(uint8_t chn, const Vclk5_Freq* tbl);

static uint32_t loc_RegRead(r_i2c_Unit_t I2c, uint8_t SlaveAddr,
                uint8_t Addr, uint8_t *Data, uint32_t NumBytes)
{
    return R_I2C_ReadRegMap(I2c, SlaveAddr >> 1, Addr, Data, NumBytes);
}

static uint32_t loc_RegWrite(r_i2c_Unit_t I2c, uint8_t SlaveAddr,
                uint8_t Addr, uint8_t Data)
{
    uint32_t read;
    int8_t   no_bytes = 2;
    uint8_t  byte[no_bytes];
    uint32_t ret = 0;

    byte[0] = Addr;
    byte[1] = Data;

    ret = loc_I2cReset(I2c);
    if (0 == ret)
    {
        read = R_I2C_Write(I2c, (SlaveAddr >> 1), &byte[0], no_bytes);

        if(read == no_bytes) { /* OK */
            return 0;
        } else {
            R_PRINT_Log("[loc_RegWrite : 5p49v6901] R_I2C_Write return value(%u) is invalid. :  Failed(1)\r\n",read);
            return 1;
        }
    }

    return ret;
}

static uint32_t loc_I2cReset(r_i2c_Unit_t Unit)
{
    uint32_t ret = 0;
    r_cpg_Error_t cpg_rt = R_CPG_ERR_SUCCESS;

    switch (Unit) {
    case R_I2C_IF0:
        cpg_rt = R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT31, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        cpg_rt |= R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT31, 0);
        break;
    case R_I2C_IF1:
        cpg_rt = R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT30, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        cpg_rt |= R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT30, 0);
        break;
    case R_I2C_IF2:
        cpg_rt = R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT29, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        cpg_rt |= R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT29, 0);
        break;
    case R_I2C_IF3:
        cpg_rt = R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT28, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        cpg_rt |= R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT28, 0);
        break;
    case R_I2C_IF4:
        cpg_rt = R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT27, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        cpg_rt |= R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT27, 0);
        break;
    case R_I2C_IF5:
        cpg_rt = R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT19, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        cpg_rt |= R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT19, 0);
        break;
    case R_I2C_IF6:
        cpg_rt = R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT18, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        cpg_rt |= R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT18, 0);
        break;
    default:
        R_PRINT_Log("[loc_I2cReset : 5p49v6901] I2C Unit is invalid.\r\n");
        ret = 1;
        break;
    }

    if ((0 == ret) && (R_CPG_ERR_SUCCESS != cpg_rt))
    {
        R_PRINT_Log("[loc_I2cReset]NG. cpg_rt:%d\n", cpg_rt);
        ret = 1;
    }

    return ret;
}

static r_5p49v6901_Error_t loc_I2cClkEnable(r_i2c_Unit_t Unit)
{
    r_5p49v6901_Error_t ret = R_5P49V6901_ERR_SUCCESS;
    r_cpg_Error_t cpg_rt = R_CPG_ERR_SUCCESS;

    switch (Unit) {
    case R_I2C_IF0:
        cpg_rt = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT31, false);
        break;
    case R_I2C_IF1:
        cpg_rt = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT30, false);
        break;
    case R_I2C_IF2:
        cpg_rt = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT29, false);
        break;
    case R_I2C_IF3:
        cpg_rt = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT28, false);
        break;
    case R_I2C_IF4:
        cpg_rt = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT27, false);
        break;
    case R_I2C_IF5:
        cpg_rt = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT19, false);
        break;
    case R_I2C_IF6:
        cpg_rt = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT18, false);
        break;
    default:
        R_PRINT_Log("[loc_I2cClkEnable : 5p49v6901] I2C Unit is invalid.\r\n");
        break;
    }

    if ((R_5P49V6901_ERR_SUCCESS == ret) && (R_CPG_ERR_SUCCESS != cpg_rt))
    {
        R_PRINT_Log("[loc_I2cClkEnable]NG. cpg_rt:%d\n", cpg_rt);
        ret = R_5P49V6901_ERR_FAILED;
    }

    return ret;
}

static uint32_t loc_Write6901(uint8_t addr, uint8_t data)
{
    return loc_RegWrite(LOC_I2C_CHANNEL, IDT5P49V6901_I2C_ADDR, addr, data);
}

static uint32_t loc_Read6901(uint8_t addr, uint8_t *pdata, uint32_t num)
{
    return loc_RegRead(LOC_I2C_CHANNEL, IDT5P49V6901_I2C_ADDR, addr, pdata, num);
}

static r_5p49v6901_Error_t loc_SetClock(uint8_t chn, const Vclk5_Freq* tbl)
{
    uint8_t data;
    uint32_t loc_rt = 0;
    r_5p49v6901_Error_t ret = R_5P49V6901_ERR_SUCCESS;

    /* OUT1 */
    if (chn == 1) {
        /* Stop OUT1 */
        loc_Read6901(0x69, &data, 1);
        data &= ~(1 << 6);
        loc_rt = loc_Write6901(0x69, data);

        /* Set clock value */
        loc_rt |= loc_Write6901(0x2D, tbl->intdiv[0]);
        loc_rt |= loc_Write6901(0x2E, tbl->intdiv[1]);
        loc_rt |= loc_Write6901(0x22, tbl->offset[0]);
        loc_rt |= loc_Write6901(0x23, tbl->offset[1]);
        loc_rt |= loc_Write6901(0x24, tbl->offset[2]);
        loc_rt |= loc_Write6901(0x25, tbl->offset[3]);

        /* Start OUT1 */
        loc_Read6901(0x69, &data, 1);
        data |= (1 << 6);
        loc_rt |= loc_Write6901(0x69, data);
    }

    /* OUT2*/
    else if (chn == 2) {
        /* Stop OUT2 */
        loc_Read6901(0x69, &data, 1);
        data &= ~(1 << 5);
        loc_rt = loc_Write6901(0x69, data);

        /* Set clock value */
        loc_rt |= loc_Write6901(0x3D, tbl->intdiv[0]);
        loc_rt |= loc_Write6901(0x3E, tbl->intdiv[1]);
        loc_rt |= loc_Write6901(0x32, tbl->offset[0]);
        loc_rt |= loc_Write6901(0x33, tbl->offset[1]);
        loc_rt |= loc_Write6901(0x34, tbl->offset[2]);
        loc_rt |= loc_Write6901(0x35, tbl->offset[3]);

        /* Start OUT2 */
        loc_Read6901(0x69, &data, 1);
        data |= (1 << 5);
        loc_rt |= loc_Write6901(0x69, data);
    }
    else
    {
        R_PRINT_Log("[loc_SetClock] chn(%d) is out of range.\r\n", chn);
        ret = R_5P49V6901_ERR_INVALID_PARAMETER;
    }

    if (0 != loc_rt)
    {
        R_PRINT_Log("[loc_SetClock]  Error writing register.\r\n");
        ret = R_5P49V6901_ERR_FAILED;
    }

    return ret;
}

r_5p49v6901_Error_t R_5P49V6901_Init(uint8_t out_channel)
{
    uint32_t i;
    uint8_t data;
    uint32_t loc_rt = 0;
    r_5p49v6901_Error_t ret = R_5P49V6901_ERR_SUCCESS;

    ret = loc_I2cClkEnable(LOC_I2C_CHANNEL);

    if (R_5P49V6901_ERR_SUCCESS == ret)
    {
        /* Initialize 5P49V6901 */
        loc_Read6901(0x00, &data, 1);
        if (data != VersaclockInitialValue[0]) {
            for (i = 0; i <= 0x69; i++) {
                loc_rt |= loc_Write6901(i, VersaclockInitialValue[i]);
            }
        }

        if (0 != loc_rt)
        {
            R_PRINT_Log("[R_5P49V6901_Init]  Error writing register.\r\n");
            ret = R_5P49V6901_ERR_FAILED;
        }

        if (R_5P49V6901_ERR_SUCCESS == ret)
        {
            /* Set clock for SalvatorXS only */
            if (out_channel == 1) {
                ret = loc_SetClock(1, &Vclk5_148500kHz);
            }
            else if (out_channel == 2) {
                ret = loc_SetClock(2, &Vclk5_108000kHz);
            }
            else
            {
                R_PRINT_Log("[R_5P49V6901_Init] out_channel(%d) is out of range.\r\n", out_channel);
                ret = R_5P49V6901_ERR_INVALID_PARAMETER;
            }
        }
    }

    return ret;
}
