/*************************************************************************************************************
* i2c_cs2000_c
* Copyright (c) [2020-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "rcar-xos/osal/r_osal.h"
#include "cs2000.h"
#include "r_i2c_api.h"
#include "r_cpg_api.h"
#include "r_delay_api.h"
#include "r_print_api.h"

/* CS2000-CP(Clock Synthesizer,Audio Clock) */
#define	CS2000CP_I2C_SLA		    0x9E

#define CS2000CP_DeviceID		    0x01
#define CS2000CP_DeviceCtrl		    0x02
#define CS2000CP_DeviceCfg1		    0x03
#define CS2000CP_DeviceCfg2		    0x04
#define CS2000CP_GlobalCfg		    0x05
#define CS2000CP_32BitRatio0_0		0x06	/* Ratio0-LSB */
#define CS2000CP_32BitRatio0_1		0x07
#define CS2000CP_32BitRatio0_2		0x08
#define CS2000CP_32BitRatio0_3		0x09	/* Ratio0-MSB */
#define CS2000CP_32BitRatio1_0		0x0A	/* Ratio1-LSB */
#define CS2000CP_32BitRatio1_1		0x0B
#define CS2000CP_32BitRatio1_2		0x0C
#define CS2000CP_32BitRatio1_3		0x0D	/* Ratio1-MSB */
#define CS2000CP_32BitRatio2_0		0x0E	/* Ratio2-LSB */
#define CS2000CP_32BitRatio2_1		0x0F
#define CS2000CP_32BitRatio2_2		0x10
#define CS2000CP_32BitRatio2_3		0x11	/* Ratio2-MSB */
#define CS2000CP_32BitRatio3_0		0x12	/* Ratio3-LSB */
#define CS2000CP_32BitRatio3_1		0x13
#define CS2000CP_32BitRatio3_2		0x14
#define CS2000CP_32BitRatio3_3		0x15	/* Ratio3-MSB */
#define CS2000CP_FunctCfg1		    0x16
#define CS2000CP_FunctCfg2		    0x17
#define CS2000CP_FunctCfg3		    0x1E

#define BIT(nr)         (1UL << (nr))

static uint32_t loc_RegWrite(r_i2c_Unit_t I2c, uint8_t SlaveAddr,
                uint8_t Addr, uint8_t Data);
static void loc_I2cClkEnable(r_i2c_Unit_t Unit);
static void loc_I2cReset(r_i2c_Unit_t Unit);

static uint32_t loc_RegWrite(r_i2c_Unit_t I2c, uint8_t SlaveAddr,
                uint8_t Addr, uint8_t Data)
{
    uint32_t read;
    int8_t   no_bytes = 2;
    uint8_t  byte[no_bytes];

    byte[0] = Addr;
    byte[1] = Data;

    loc_I2cReset(I2c);
    read = R_I2C_Write(I2c, (SlaveAddr >> 1), &byte[0], no_bytes);

    if(read == no_bytes) { /* OK */
        return 0;
    } else {
        R_PRINT_Log("[loc_RegWrite : c2000] loc_RegWrite return value(%u) is invalid. : Failed(1)\r\n",read);
        return 1;
    }
}

static uint32_t loc_RegRead(r_i2c_Unit_t I2c, uint8_t SlaveAddr,
                uint8_t Addr, uint8_t *Data, uint32_t NumBytes)
{
    return R_I2C_ReadRegMap(I2c, SlaveAddr, Addr, Data, NumBytes);
}


static void loc_I2cReset(r_i2c_Unit_t Unit)
{
    switch (Unit) {
    case R_I2C_IF0:
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT31, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT31, 0);
        break;
    case R_I2C_IF1:
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT30, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT30, 0);
        break;
    case R_I2C_IF2:
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT29, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT29, 0);
        break;
    case R_I2C_IF3:
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT28, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT28, 0);
        break;
    case R_I2C_IF4:
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT27, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT27, 0);
        break;
    case R_I2C_IF5:
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT19, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT19, 0);
        break;
    case R_I2C_IF6:
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT18, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT18, 0);
        break;
    default:
        R_PRINT_Log("[loc_I2cReset : c2000] I2C Unit is invalid.\r\n");
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
    case R_I2C_IF1:
        R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT30, false);
        break;
    case R_I2C_IF2:
        R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT29, false);
        break;
    case R_I2C_IF3:
        R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT28, false);
        break;
    case R_I2C_IF4:
        R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT27, false);
        break;
    case R_I2C_IF5:
        R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT19, false);
        break;
    case R_I2C_IF6:
        R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT18, false);
        break;
    default:
        R_PRINT_Log("[loc_I2cClkEnable : c2000] I2C Unit is invalid.\r\n");
        break;
    }

    return;
}

static int R_I2C_WriteAddr(r_i2c_Unit_t I2c, uint8_t SlaveAddr,
                uint8_t Addr, uint8_t *Data, uint32_t NumBytes)
{
    loc_RegWrite(I2c, (SlaveAddr >> 1), Addr, *Data);

    return 0;
}

static int R_I2C_ReadAddr(r_i2c_Unit_t I2c, uint8_t SlaveAddr,
                uint8_t Addr, uint8_t *Data, uint32_t NumBytes)
{
    loc_RegRead(I2c, (SlaveAddr >> 1), Addr, Data, NumBytes);

    return 0;
}

void StartCS2000CP(uint32_t ratio)
{
	uint32_t dataL;
	uint8_t wrData, rdData;
	uint8_t slAddr;

    r_i2c_Unit_t i2cCh = R_I2C_IF2;
	slAddr = CS2000CP_I2C_SLA;

   loc_I2cClkEnable(i2cCh);

	/* Device I.D. and Revision */
	/* [7:3] Device[4:0] */
	/* [2:0] Revision[2:0] */
	R_I2C_ReadAddr(i2cCh, slAddr, CS2000CP_DeviceID, &rdData, 1);
	if ((rdData&(BIT(7)|BIT(3))) != 0x00) {
		return;
	}

	/* Device Cfg 1 */
	/* [7:5] RModSel[2:0]   = 000 */
	/* [4:3] RSel[1:0]      = 00 (select Ratio0) */
	/* [2:1] AuxOutSrc[1:0] */
	/* [0]   EnDevCfg1      = 1 (must be set to 1) */
	wrData = 1;
	R_I2C_WriteAddr(i2cCh, slAddr, CS2000CP_DeviceCfg1, &wrData, 1);

	/* Global Configuration */
	/* [7:4] *Reserved* */
	/* [3]   FREEZE         = 0 (Register changes take effect immediately.) */
	/* [2:1] *Reserved* */
	/* [0]   EnDevCfg2      = 1 (must be set to 1) */
	wrData = 1;
	R_I2C_WriteAddr(i2cCh, slAddr, CS2000CP_GlobalCfg, &wrData, 1);

	/* Ratio0 = output_clk/input_clk * 2^20 */
	/* [out=49.1520MHz][in=24.5760MHz] : 49152000/24576000 * 2^20 = 2097152   = 0x00200000 */
	/* [out=24.5760MHz][in=24.5760MHz] : 24576000/24576000 * 2^20 = 1048576   = 0x00100000 */
	/* [out=12.2880MHz][in=24.5760MHz] : 12288000/24576000 * 2^20 =  524288   = 0x00080000 */
	/* [out=45.1584MHz][in=24.5760MHz] : 45158400/24576000 * 2^20 = 1926758.4 = 0x001D6666 */
	/* [out=22.5792MHz][in=24.5760MHz] : 22579200/24576000 * 2^20 =  963379.2 = 0x000EB333 */
	/* [out=11.2896MHz][in=24.5760MHz] : 11289600/24576000 * 2^20 =  481689.6 = 0x00075999 */
	dataL = ratio;
	wrData = (dataL >> 24) & 0xFF;
	R_I2C_WriteAddr(i2cCh, slAddr, CS2000CP_32BitRatio0_0, &wrData, 1);		/* 0x06 = MSB */
	wrData = (dataL >> 16) & 0xFF;
	R_I2C_WriteAddr(i2cCh, slAddr, CS2000CP_32BitRatio0_1, &wrData, 1);
	wrData = (dataL >> 8) & 0xFF;
	R_I2C_WriteAddr(i2cCh, slAddr, CS2000CP_32BitRatio0_2, &wrData, 1);
	wrData = (dataL >> 0) & 0xFF;
	R_I2C_WriteAddr(i2cCh, slAddr, CS2000CP_32BitRatio0_3, &wrData, 1);		/* 0x09 = LSB */

	/* Funct Cfg 1 */
	/* [7]   ClkSkipEn      = 1 */
	/* [6]   AuxLockCfg */
	/* [5]   *Reserved* */
	/* [4:3] RefClkDiv[1:0] = 01 (x 1/2) */
	/* [2:0] *Reserved* */
	wrData = (BIT(7)|BIT(3));
	R_I2C_WriteAddr(i2cCh, slAddr, CS2000CP_FunctCfg1, &wrData, 1);

	/* Funct Cfg 2 */
	/* [7:5] *Reserved* */
	/* [4]   ClkOutUnl   = 0 */
	/* [3]   LFRatioCfg  = 1 (select 12.20 format) */
	/* [2:0] *Reserved* */
	wrData = (BIT(3));
	R_I2C_WriteAddr(i2cCh, slAddr, CS2000CP_FunctCfg2, &wrData, 1);


	/* Device Cfg 2 */
	/* [7:3] *Reserved* */
	/* [2:1] LockClk[1:0]  = 00 (select Ratio0) */
	/* [0]   FracNSrc      = 0 (Static Ratio) */
	wrData = 0;
	R_I2C_WriteAddr(i2cCh, slAddr, CS2000CP_DeviceCfg2, &wrData, 1);

	/* wait PLL Locked */
	/* Device Ctrl */
	/* [7]   Unlock     = 0:PLL is Locked / 1:PLL is Unlocked */
	/* [6:2] *Reserved* */
	/* [1]   AuxOutDis  = 0:AUX_OUT output driver enabled */
	/* [0]   ClkOutDis  = 0:CLK_OUT output driver enabled */
	do {
		R_I2C_ReadAddr(i2cCh, slAddr, CS2000CP_DeviceCtrl, &rdData, 1);
		R_OSAL_ThreadSleepForTimePeriod(1);
	} while (rdData & BIT(7));	/* wait PLL Locked */

	/* CLK_OUT output driver enabled */
	wrData = 0x00;
	R_I2C_WriteAddr(i2cCh, slAddr, CS2000CP_DeviceCtrl, &wrData, 1);

}