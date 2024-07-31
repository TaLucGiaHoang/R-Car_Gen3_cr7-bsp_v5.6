/*************************************************************************************************************
* i2c_ak4613_c
* Copyright (c) [2020-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "ak4613.h"
#include "r_i2c_api.h"
#include "r_cpg_api.h"
#include "r_delay_api.h"
#include "r_print_api.h"

#define	AK4613_I2C_ADDR		0x20		/* 8bits */

#define	AK4613_PWR_MGMT1	0x00
#define	AK4613_PWR_MGMT2	0x01
#define	AK4613_PWR_MGMT3	0x02
#define	AK4613_CTRL1		0x03
#define	AK4613_CTRL2		0x04
#define	AK4613_DE_EMPH1		0x05
#define	AK4613_DE_EMPH2		0x06
#define	AK4613_OVERFLOW		0x07
#define	AK4613_ZERO_DET		0x08
#define	AK4613_IN_CTRL		0x09
#define	AK4613_OUT_CTRL		0x0A
#define	AK4613_LOUT_VOL(ch)	(0x0B + (ch) - 1)
#define	AK4613_ROUT_VOL(ch)	(0x0C + (ch) - 1)

/* sampling rate */
#define AUDIO_44_1kHz		0x0  /*  44.1kHz */
#define AUDIO_48_0kHz		0x3  /*  48.0kHz */

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
        R_PRINT_Log("[loc_RegWrite : ak4613] R_I2C_Write return value(%u) is invalid. :  Failed(1)\r\n",read);
        return 1;
    }
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
        R_PRINT_Log("[loc_I2cReset : ak4613] I2C Unit is invalid.\r\n");
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
        R_PRINT_Log("[loc_I2cClkEnable : ak4613] I2C Unit is invalid.\r\n");
        break;
    }

    return;
}

static uint32_t delay_after_power_us;
static uint32_t delay_after_attn_ms;

static int WriteAk4613(uint8_t wrAddr, uint8_t wrData)
{
    return loc_RegWrite(R_I2C_IF2, AK4613_I2C_ADDR, wrAddr, wrData);
}

/*
 * AK4613 Audio Codec
 * Sets up channel 1 only as stereo output.
 */
uint32_t ak4613_init(uint32_t mode)
{
    loc_I2cClkEnable(R_I2C_IF2);

    /*
     * Calculate the time to wait after changing power 5/fs (min), and also
     * soft mute/attenuation => 256/fs (min), see Digital attenuator
     * transition time. Note the code is written like this to help the
     * compiler generate constants.
     */
    switch (mode) {
    case AUDIO_44_1kHz:
        delay_after_power_us = (5 * 1000000) / 44100;
        delay_after_attn_ms = (256 * 1000) / 44100;
        break;
    case AUDIO_48_0kHz:
        delay_after_power_us = (5 * 1000000) / 48000;
        delay_after_attn_ms = (256 * 1000) / 48000;
        break;
    }

    /* (0x00) Power Management 1 (default=0x0F) */
    /*   [7:4] fixed =0 */
    /*   [3]   PMVR  =1      Power management of reference voltage (1: Normal operation, 0: Power-down) */
    /*   [2]   PMADC =1      Power management of ADC               (1: Normal operation, 0: Power-down) */
    /*   [1]   PMDAC =1      Power management of DAC               (1: Normal operation, 0: Power-down) */
    /*   [0]   RSTN  =1      Internal timing reset                 (1: Normal operation, 0: Reset, but registers are not initialized) */
    WriteAk4613(AK4613_PWR_MGMT1, 0x0F);

    /* (0x01) Power Management 2 (default=0x07) */
    /*   [7:3] fixed =0 */
    /*   [2]   fixed =1 */
    /*   [1]   PMAD2 =0      Power management of ADC2 (0: Power-down, 1: Normal operation) */
    /*   [0]   PMAD1 =1      Power management of ADC1 (0: Power-down, 1: Normal operation) */
    WriteAk4613(AK4613_PWR_MGMT2, 0x05);

    /* (0x02) Power Management 3 (default=0x3F) */
    /*   [7:6] fixed =0 */
    /*   [5]   PMDA6 =0      Power management of DAC6 (0: Power-down, 1: Normal operation) */
    /*   [4]   PMDA5 =0      Power management of DAC5 (0: Power-down, 1: Normal operation) */
    /*   [3]   PMDA4 =0      Power management of DAC4 (0: Power-down, 1: Normal operation) */
    /*   [2]   PMDA3 =0      Power management of DAC3 (0: Power-down, 1: Normal operation) */
    /*   [1]   PMDA2 =0      Power management of DAC2 (0: Power-down, 1: Normal operation) */
    /*   [0]   PMDA1 =0      Power management of DAC1 (0: Power-down, 1: Normal operation) */
    WriteAk4613(AK4613_PWR_MGMT3, 0x00);

    /* (0x03) Control 1 (default=0x20) */
    /*   [7:6] TDM1-0 =00    TDM Format Select                  (00:Stereo, 01:TDM512, 10:TDM256, 11:TDM128) */
    /*   [5:3] DIF2-0 =011   Audio Data Interface Modes         (011: 24bit, Left justified) */
    /*   [2:1] ATS1-0 =11    Digital attenuator transition time (00:4096/fs, 01:2048/fs, 10:512/fs, 11:256/fs) */
    /*   [0]   SMUTE  =1     Soft Mute Enable                   (0: Normal operation, 1: All DAC outputs soft-muted) */
    WriteAk4613(AK4613_CTRL1, 0x1f);

    /* (0x04) Control 2 (default=0x20) */
    /*   [7]   fixed  =0 */
    /*   [6]   MCKO   =0     Master clock output enable          (0:Output"L", 1:Output "MCKO") */
    /*   [5:4] CKS1-0 =00    Master Clock Input Frequency Select (00:Normal Speed=256fs, Double Speed=256fs, Quad Speed=128fs) */
    /*   [3:2] DFS1-0 =00/01 Sampling speed mode                 (00:Normal Speed, 01:Double Speed, 10:Quad Speed, 11:N/A) */
    /*                       DFS is ignored at ACKS bit=1.         Normal Speed : 32kHz~48kHz, */
    /*                                                             Double Speed : 64kHz~96kHz */
    /*                                                             Quad Speed   : 128kHz~192kHz */
    /*   [1]   ACKS   =0     Master Clock Frequency Auto Setting (0:Disable, 1:Enable) */
    /*   [0]   DIV    =0     Output of Master clock frequency    (0:x1, 1:x1/2) --> do not use MCKO, then set default */
    switch (mode) {
    case AUDIO_44_1kHz:
    case AUDIO_48_0kHz:
        WriteAk4613(AK4613_CTRL2, 0x00);  /* Normal Speed Mode=32kHz~48kHz, Sampling speed=256fs */
        break;
    }

    /* (0x05) De-emphasis1 (default=0x55) */
    /*   [7:6] DEM41-0 =01   De-emphasis response control for DAC4 data on SDTI1 (01:OFF) */
    /*   [5:4] DEM31-0 =01   De-emphasis response control for DAC3 data on SDTI1 (01:OFF) */
    /*   [3:2] DEM21-0 =01   De-emphasis response control for DAC2 data on SDTI1 (01:OFF) */
    /*   [1:0] DEM11-0 =01   De-emphasis response control for DAC1 data on SDTI1 (01:OFF) */
    WriteAk4613(AK4613_DE_EMPH1, 0x55);

    /* (0x06) De-emphasis2 (default=0x05) */
    /*   [7:4] fixed   =0 */
    /*   [3:2] DEM61-0 =01   De-emphasis response control for DAC6 data on SDTI1 (01:OFF) */
    /*   [1:0] DEM51-0 =01   De-emphasis response control for DAC5 data on SDTI1 (01:OFF) */
    WriteAk4613(AK4613_DE_EMPH2, 0x05);

    /* (0x07) Overflow Detect (default=0x07) */
    /*   [7:4] fixed   =0 */
    /*   [3]   OVFE    =0    Overflow detection enable   (0:Disable, 1:Enable) */
    /*   [2:0] OVFM2-0 =111  Overflow detect mode select (111:Disable) */
    WriteAk4613(AK4613_OVERFLOW, 0x07);

    /* (0x08) Zero Detect (default=0x0F) */
    /*   [7:6] LOOP1-0 =00   Loopback mode enable (00:Normal, No loop back) */
    /*   [5:4] fixed   =0 */
    /*   [3:0] DZFM3-0 =1111 Zero detect mode select (1111:Disable) */
    WriteAk4613(AK4613_ZERO_DET, 0x0F);

    /* (0x09) Output Control (default=0x07) */
    /*   [7:3] fixed =0 */
    /*   [2]   fixed =1 */
    /*   [1]   DIE2  =0      ADC2 Differential Input Enable (0:Single-End Input, 1:Differential Input) */
    /*   [0]   DIE1  =0      ADC1 Differential Input Enable (0:Single-End Input, 1:Differential Input) */
    WriteAk4613(AK4613_IN_CTRL, 0x04);

    /* (0x0A) Output Control (default=0x3F) */
    /*   [7:6] fixed =0 */
    /*   [5]   DOE6  =0      DAC6 Differential Output Enable (0:Single-End Input, 1:Differential Input) */
    /*   [4]   DOE5  =0      DAC5 Differential Output Enable (0:Single-End Input, 1:Differential Input) */
    /*   [3]   DOE4  =0      DAC4 Differential Output Enable (0:Single-End Input, 1:Differential Input) */
    /*   [2]   DOE3  =0      DAC3 Differential Output Enable (0:Single-End Input, 1:Differential Input) */
    /*   [1]   DOE2  =0      DAC2 Differential Output Enable (0:Single-End Input, 1:Differential Input) */
    /*   [0]   DOE1  =0      DAC1 Differential Output Enable (0:Single-End Input, 1:Differential Input) */
    WriteAk4613(AK4613_OUT_CTRL, 0x00);

    /* (0x0B-0x16) Volume Control (default=0x00) */
    /*   [7:0] ATT7-0        Attenuation Level */
    /*         ==> 0x00:0.0dB, 0x01:-0.5dB, 0x02:-1.0dB, ..., 0xFD:-126.5dB, 0xFE:-127.0dB, 0xFF:MUTE */
    WriteAk4613(AK4613_LOUT_VOL(1), 0x00);
    WriteAk4613(AK4613_ROUT_VOL(1), 0x00);

    return 0;
}

static void StartAk4613LineOut(void)
{
    /* Power up DAC1 */
    WriteAk4613(AK4613_PWR_MGMT3, 0x01);
    delay_us(delay_after_power_us);
    /* Dummy Write (recommended) */
    /* A power-down release command must be written again after 5 LRCK cycle or later from the first command. */
    WriteAk4613(AK4613_CTRL1, 0x1f);

    /* Unmute DAC1 */
    WriteAk4613(AK4613_CTRL1, 0x1e);
}

static void StopAk4613LineOut(void)
{
    /* all DAC muted */
    WriteAk4613(AK4613_CTRL1, 0x1f);
    delay_us(delay_after_attn_ms);

    /* Power down DAC1 */
    WriteAk4613(AK4613_PWR_MGMT3, 0x00);
}

void StartCodec(void)
{
    StartAk4613LineOut();
}

void StopCodec(void)
{
    StopAk4613LineOut();
}
