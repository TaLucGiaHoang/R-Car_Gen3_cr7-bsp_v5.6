/*************************************************************************************************************
* i2c_adv7482_api_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2018-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/


#include <stdint.h>

#include "r_adv7482_api.h"
#include "r_i2c_api.h"
#include "r_cpg_api.h"
#include "r_delay_api.h"
#include "rcar-xos/osal/r_osal.h"
#include "r_print_api.h"

#define CMD                     0xFF            /* CMD Flag (for [0]) */
#define WAIT_MS                 0x01            /* wait millisecond Flag (value=data) */
#define COLOR_SPACE_OUT         0x02            /* "RGB color space output" or "YCrCb color space output" */
#define BOUNDARY_600MBPS        0x03            /* "Over 600MBps" or "Up to 600Mbps" */

#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))

/**
 * Section: local Constant variable
 */

/**
 *
 *                    ADV7482 Initialize Script Table
 *
 */

/**
 * Reference : ADV7481ES3C-VER.3.6c.txt
 * http://www.analog.com/products/audio-video/analoghdmidvi-interfaces/adv7482.html#product-tools
 */


/* Table below lists the default init sequence for ADV7482 enabling following
 * path within the device [CVBS]:
 *
 *         [     ADV7482                    ]
 * CVBS -> [ AFE -> SDP -> MIPI CSI-2 (TxB) ] -> CLK/Data output
 *
 * SLA -> Internal I2C slave address mapping within ADV7482
 * ADDR -> Offset within the slave address
 * DATA -> Data to write
 *
 * Note: For HDMI input instead of CVBS, a different init sequence is needed for
 * ADV7482 + setting of EDID.
 */
/* [HDMI]:
 *  ; Supported Formats For Script Below - 1920x1080p60, 1920x1080p50, 1280x1024(SXGA)@60, 1600x1200(UXGA)@60
 *  :01-30 HDMI to MIPI TxA CSI 4-Lane - RGB888, Over 600Mbps:
 *  RGB888
 */
static const uint8_t loc_CvbsHdmiInput[][3] = {
    /* [0]   [1]   [2] */
    /* SLA   ADDR  DATA */
    { 0xE0, 0xFF, 0xFF },    /* [Cvbs,Hdmi] SW reset */
    { CMD, WAIT_MS, 0x05 },  /* [Cvbs,Hdmi] ** delay 5 ** */
    { 0xE0, 0x01, 0x76 },    /* [Cvbs,Hdmi] ADI Required Write */
    { 0xE0, 0x05, 0x96 },    /* [Hdmi] Setting Vid_Std to 1600x1200(UXGA)@60 */
    { 0xE0, 0x00, 0x40 },    /* [Cvbs,Hdmi] Disable chip powerdown & Enable HDMI Rx block */
    { 0xE0, 0xF2, 0x01 },    /* [Cvbs,Hdmi] Enable I2C Read Auto-Increment */
    { 0xE0, 0xF3, R_ADV7482_I2CADDR_DPLL },     /* [Cvbs,Hdmi] DPLL Map Address Set to 0x4C */
    { 0xE0, 0xF4, R_ADV7482_I2CADDR_CP },       /* [Cvbs,Hdmi] CP Map Address Set to 0x44 */
    { 0xE0, 0xF5, R_ADV7482_I2CADDR_HDMI_RX },  /* [Cvbs,Hdmi] HDMI RX Map Address Set to 0x68 */
    { 0xE0, 0xF6, R_ADV7482_I2CADDR_EDID },     /* [Cvbs,Hdmi] EDID Map Address Set to 0x6C */
    { 0xE0, 0xF7, R_ADV7482_I2CADDR_REPEATER }, /* [Cvbs,Hdmi] HDMI RX Repeater Map Address Set to 0x64 */
    { 0xE0, 0xF8, R_ADV7482_I2CADDR_INFO },     /* [Cvbs,Hdmi] HDMI RX Infoframe Map Address Set to 0x62 */
    { 0xE0, 0xF9, R_ADV7482_I2CADDR_CBUS },     /* [Cvbs,Hdmi] CBUS Map Address Set to 0xF0 */
    { 0xE0, 0xFA, R_ADV7482_I2CADDR_CEC },      /* [Cvbs,Hdmi] CEC Map Address Set to 0x82 */
    { 0xE0, 0xFB, R_ADV7482_I2CADDR_SDP },      /* [Cvbs,Hdmi] SDP Main Map Address Set to 0xF2 */
    { 0xE0, 0xFC, R_ADV7482_I2CADDR_CSI_TXB },  /* [Cvbs,Hdmi] CSI-TXB Map Address Set to 0x90 */
    { 0xE0, 0xFD, R_ADV7482_I2CADDR_CSI_TXA },  /* [Cvbs,Hdmi] CSI-TXA Map Address Set to 0x94 */
    { 0xE0, 0x0E, 0xDD }, /* [Cvbs,Hdmi]  LLC/PIX/SPI PINS TRISTATED AUD Outputs Enabled */
    { 0x64, 0x40, 0x83 }, /* [Hdmi] Enable HDCP 1.1   */
    { 0x68, 0x00, 0x08 }, /* [Hdmi] Foreground Channel = A */
    { 0x68, 0x98, 0xFF }, /* [Hdmi] ADI Required Write */
    { 0x68, 0x99, 0xA3 }, /* [Hdmi] ADI Required Write */
    { 0x68, 0x9A, 0x00 }, /* [Hdmi] ADI Required Write */
    { 0x68, 0x9B, 0x0A }, /* [Hdmi] ADI Required Write */
    { 0x68, 0x9D, 0x40 }, /* [Hdmi] ADI Required Write */
    { 0x68, 0xCB, 0x09 }, /* [Hdmi] ADI Required Write */
    { 0x68, 0x3D, 0x10 }, /* [Hdmi] ADI Required Write */
    { 0x68, 0x3E, 0x7B }, /* [Hdmi] ADI Required Write */
    { 0x68, 0x3F, 0x5E }, /* [Hdmi] ADI Required Write */
    { 0x68, 0x4E, 0xFE }, /* [Hdmi] ADI Required Write */
    { 0x68, 0x4F, 0x18 }, /* [Hdmi] ADI Required Write */
    { 0x68, 0x57, 0xA3 }, /* [Hdmi] ADI Required Write */
    { 0x68, 0x58, 0x04 }, /* [Hdmi] ADI Required Write */
    { 0x68, 0x85, 0x10 }, /* [Hdmi] ADI Required Write */
    { 0x68, 0x83, 0x00 }, /* [Hdmi] Enable All Terminations */
    { 0x68, 0xA3, 0x01 }, /* [Hdmi] ADI Required Write */
    { 0x68, 0xBE, 0x00 }, /* [Hdmi] ADI Required Write */
    { 0x68, 0x6C, 0x01 }, /* [Hdmi] HPA Manual Enable */
    { 0x68, 0xF8, 0x01 }, /* [Hdmi] HPA Asserted */
    { 0x68, 0x0F, 0x00 }, /* [Hdmi] Audio Mute Speed Set to Fastest (Smallest Step Size) */
    { CMD, COLOR_SPACE_OUT, 0x00 }, /* [Hdmi] RGB color space output or YCrCb color space output (CP_RGB_OUT)*/
    { 0xE0, 0x17, 0x80 }, /* [Hdmi] Luma & Chroma Values Can Reach 254d */
    { 0xE0, 0x03, 0x86 }, /* [Hdmi] CP-Insert_AV_Code */
    { 0x44, 0x7C, 0x00 }, /* [Hdmi] ADI Required Write */
    { 0xE0, 0x0C, 0xE0 }, /* [Hdmi] Enable LLC_DLL & Double LLC Timing */
    { 0xE0, 0x10, 0xE0 }, /* [Cvbs,Hdmi] Enable 4-lane CSI Tx & Pixel Port.
                             Enable 1-Lane MIPI Tx, enable pixel output and route SD through Pixel port */
    { 0x94, 0x00, 0x84 }, /* [Hdmi] Enable 4-lane MIPI */
    { 0x94, 0x00, 0xA4 }, /* [Hdmi] Set Auto DPHY Timing */
    { CMD, BOUNDARY_600MBPS, 0x00 }, /* [Hdmi] ADI Required Write ("Over 600MBps" or "Up to 600Mbps") */
    { 0x94, 0xD6, 0x07 },  /* [Hdmi] ADI Required Write */
    { 0x94, 0xC4, 0x0A },  /* [Hdmi] ADI Required Write */
    { 0x94, 0x71, 0x33 },  /* [Hdmi] ADI Required Write */
    { 0x94, 0x72, 0x11 },  /* [Hdmi] ADI Required Write */
    { 0x94, 0xF0, 0x00 },  /* [Hdmi] i2c_dphy_pwdn - 1'b0 */
    { 0x94, 0x31, 0x82 },  /* [Hdmi] ADI Required Write */
    { 0x94, 0x1E, 0x40 },  /* [Hdmi] ADI Required Write */
    { 0x94, 0xDA, 0x01 },  /* [Hdmi] i2c_mipi_pll_en - 1'b1 */
    { CMD, WAIT_MS, 0x02 },/* [Hdmi] ** delay 2 ** */
    { 0x94, 0x00, 0x24 },  /* [Hdmi] Power-up CSI-TX */
    { CMD, WAIT_MS, 0x01 },/* [Hdmi] ** delay 1 ** */
    { 0x94, 0xC1, 0x2B },  /* [Hdmi] ADI Required Write */
    { CMD, WAIT_MS, 0x01 },/* [Hdmi] ** delay 1 ** */
    { 0x94, 0x31, 0x80 },  /* [Hdmi] ADI Required Write}; */
    { 0xF2, 0x0F, 0x00 },  /* [Cvbs] Exit Power Down Mode */
    { 0xF2, 0x52, 0xCD },  /* [Cvbs] ADI Required Write */
    { 0xF2, 0x00, 0x07 },  /* [Cvbs] INSEL = CVBS in on Ain 8 *** changed from recommended value *** */
#ifdef ADV7482_NORMAL_LOCKING_SEQUENCE
    /* SDP lock optimization sequence */
    { 0xF2, 0x0E, 0x80 },  /* [Cvbs] ADI Required Write */
    { 0xF2, 0x9C, 0x00 },  /* [Cvbs] ADI Required Write */
    { 0xF2, 0x9C, 0xFF },  /* [Cvbs] ADI Required Write */
    { 0xF2, 0x0E, 0x00 },  /* [Cvbs] ADI Required Write */
#else
    /* SDP fast lock sequence to avoid blur */
    { 0xF2, 0x0E, 0x80 },  /* [Cvbs] ADI Required Write */
    { 0xF2, 0xD9, 0x44 },  /* [Cvbs] ADI Required Write */
    { 0xF2, 0x0E, 0x40 },  /* [Cvbs] ADI Required Write */
    { 0xF2, 0xE0, 0x01 },  /* [Cvbs] ADI Required Write */
    { 0xF2, 0x0E, 0x00 },  /* [Cvbs] ADI Required Write */
#endif
    { 0xF2, 0x80, 0x51 },  /* [Cvbs] ADI Required Write */
    { 0xF2, 0x81, 0x51 },  /* [Cvbs] ADI Required Write */
    { 0xF2, 0x82, 0x68 },  /* [Cvbs] ADI Required Write */
    { 0xF2, 0x03, 0x42 },  /* [Cvbs] Tri-S Output Drivers, PwrDwn 656 pads */
    { 0xF2, 0x04, 0x07 },  /* [Cvbs] Power-up INTRQ pad, & Enable SFL */
    { 0xF2, 0x13, 0x00 },  /* [Cvbs] ADI Required Write */
    { 0xF2, 0x17, 0x41 },  /* [Cvbs] Select SH1 */
    { 0xF2, 0x31, 0x12 },  /* [Cvbs] ADI Required Write */
    { 0x90, 0x00, 0x81 },  /* [Cvbs] Enable 1-lane MIPI */
    { 0x90, 0x00, 0xA1 },  /* [Cvbs] Set Auto DPHY Timing */
    { 0x94, 0xF0, 0x00 },  /* [Cvbs] ADI Required Write */
    { 0x90, 0xD2, 0x40 },  /* [Cvbs] ADI Required Write */
    { 0x90, 0xC4, 0x0A },  /* [Cvbs] ADI Required Write */
    { 0x90, 0x71, 0x33 },  /* [Cvbs] ADI Required Write */
    { 0x90, 0x72, 0x11 },  /* [Cvbs] ADI Required Write */
    { 0x90, 0xF0, 0x00 },  /* [Cvbs] i2c_dphy_pwdn - 1'b0 */
    { 0x90, 0x31, 0x82 },  /* [Cvbs] ADI Required Write */
    { 0x90, 0x1E, 0x40 },  /* [Cvbs] ADI Required Write */
    { 0x90, 0xDA, 0x01 },  /* [Cvbs] i2c_mipi_pll_en - 1'b1 */
    { CMD, WAIT_MS, 0x02 },/* [Cvbs] ** delay 2 ** OK */
    { 0x90, 0x00, 0x21 },  /* [Cvbs] Power-up CSI-TX OK */
    { CMD, WAIT_MS, 0x01 },/* [Cvbs] ** delay 1 ** OK */
    { 0x90, 0xC1, 0x2B },  /* [Cvbs] ADI Required Write OK */
    { CMD, WAIT_MS, 0x01 },/* [Cvbs] ** delay 1 ** OK */
    { 0x90, 0x31, 0x80 },  /* [Cvbs] ADI Required Write OK */
};


/**
 * Section: Local function Prototypes
 */
static r_i2c_Unit_t loc_I2cMap(r_adv7482_I2cChannel_t I2c);
static uint32_t loc_RegWrite(r_i2c_Unit_t I2c, uint32_t SlaveAddr,
                uint32_t Addr, uint32_t Data);
static uint32_t loc_RegRead(r_i2c_Unit_t I2c, uint8_t SlaveAddr,
                uint8_t Addr, uint8_t *Data, uint32_t NumBytes);
static r_adv7482_Error_t loc_I2cClkEnable(r_i2c_Unit_t Unit);
static uint32_t loc_I2cReset(r_i2c_Unit_t Unit);

/**
 * Section Local functions
 */
static r_i2c_Unit_t loc_I2cMap(r_adv7482_I2cChannel_t I2c)
{
    r_i2c_Unit_t i2cUnit;
    switch(I2c) {
    case R_ADV7482_I2C_CH0:
        i2cUnit = R_I2C_IF0;
        break;
    case R_ADV7482_I2C_CH1:
        i2cUnit = R_I2C_IF1;
        break;
    case R_ADV7482_I2C_CH2:
        i2cUnit = R_I2C_IF2;
        break;
    case R_ADV7482_I2C_CH3:
        i2cUnit = R_I2C_IF3;
        break;
    case R_ADV7482_I2C_CH4:
        i2cUnit = R_I2C_IF4;
        break;
    case R_ADV7482_I2C_CH5:
        i2cUnit = R_I2C_IF5;
        break;
    case R_ADV7482_I2C_CH6:
        i2cUnit = R_I2C_IF6;
        break;
    case R_ADV7482_I2C_CH7:
        i2cUnit = R_I2C_IF7;
        break;
    default:
        i2cUnit = R_I2C_LAST;
        R_PRINT_Log("[loc_I2cMap : adv7482] I2C Unit is invalid.\r\n");
        break;
    }

    return i2cUnit;
}


static uint32_t loc_RegWrite(r_i2c_Unit_t I2c, uint32_t SlaveAddr,
                uint32_t Addr, uint32_t Data)
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
            R_PRINT_Log("[loc_RegWrite : adv7482] R_I2C_Write return value(%u) is invalid. :  Failed(1)\r\n",read);
            return 1;
        }
    }

    return ret;
}

static uint32_t loc_RegRead(r_i2c_Unit_t I2c, uint8_t SlaveAddr,
                uint8_t Addr, uint8_t *Data, uint32_t NumBytes)
{
    return R_I2C_ReadRegMap(I2c, SlaveAddr >> 1, Addr, Data, NumBytes);
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
    case R_I2C_IF4:
        cpg_rt = R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT27, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        cpg_rt |= R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT27, 0);
        break;
    default:
        R_PRINT_Log("[loc_I2cReset : adv7482] I2C Unit is invalid.\r\n");
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

static r_adv7482_Error_t loc_I2cClkEnable(r_i2c_Unit_t Unit)
{
    r_adv7482_Error_t ret = R_ADV7482_ERR_SUCCESS;
    r_cpg_Error_t cpg_rt = R_CPG_ERR_SUCCESS;

    switch (Unit) {
    case R_I2C_IF0:
        cpg_rt = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT31, false);
        break;
    case R_I2C_IF4:
        cpg_rt = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT27, false);
        break;
    default:
        R_PRINT_Log("[loc_I2cClkEnable : adv7482] I2C Unit is invalid.\r\n");
        ret = R_ADV7482_ERR_INVALID_PARAMETER;
        break;
    }

    if ((R_ADV7482_ERR_SUCCESS == ret) && (R_CPG_ERR_SUCCESS != cpg_rt))
    {
        R_PRINT_Log("[loc_I2cClkEnable]NG. cpg_rt:%d\n", cpg_rt);
        ret = R_ADV7482_ERR_FAILED;
    }

    return ret;
}

/**
 * Section Global functions
 */
r_adv7482_Error_t R_ADV7482_Start(r_adv7482_I2cChannel_t I2c, uint8_t lane)
{
    int8_t addr = R_ADV7482_I2CADDR_CSI_TXB;
    r_i2c_Unit_t i2cUnit = loc_I2cMap(I2c);
    uint32_t loc_rt = 0;
    r_adv7482_Error_t ret = R_ADV7482_ERR_SUCCESS;

    /* Check the number of lane */
    if (1 == lane)
    {
        loc_rt = loc_RegWrite(i2cUnit, addr, 0x00, 0x80 | lane);  /* csitx_pwrdn : CSI TX Off */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0x00, 0xA0 | lane);  /* Enable and use automatically computed
                                                            DPHY timing parameters*/

        /* MIPI settings enable */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0xF0, 0x00);         /* dphy_pwdn : DPHY is not powered down */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0x31, 0x82);         /* ADI Required Write */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0x1E, 0x40);         /* ADI Required Write */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0xDA, 0x01);         /* mipi_pll_en : Power up MIPI PLL */
        delay_us(2000);                                  /* wait 2000us (min:1250us) */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0x00, 0x20 | lane);  /* csitx_pwrdn : CSI TX ON */

        loc_rt |= loc_RegWrite(i2cUnit, addr, 0xC1, 0x2B);         /* The clock output is allowed to toggle */

        loc_rt |= loc_RegWrite(i2cUnit, addr, 0x31, 0x80);         /* ADI Required Write */

        if ( 0 != loc_rt)
        {
            R_PRINT_Log("[R_ADV7482_Start] Error writing register.\r\n");
            ret = R_ADV7482_ERR_FAILED;
        }
    }
    else
    {
        R_PRINT_Log("[R_ADV7482_Start]NG. lane is %d.\r\n", lane);
        ret = R_ADV7482_ERR_INVALID_PARAMETER;
    }

    return ret;
}

r_adv7482_Error_t R_ADV7482_Stop(r_adv7482_I2cChannel_t I2c, uint8_t lane)
{
    int8_t addr = R_ADV7482_I2CADDR_CSI_TXB;
    r_i2c_Unit_t i2cUnit = loc_I2cMap(I2c);
    uint32_t loc_rt = 0;
    r_adv7482_Error_t ret = R_ADV7482_ERR_SUCCESS;

    /* Check the number of lane */
    if (1 == lane)
    {
        /* MIPI settings disable */
        loc_rt = loc_RegWrite(i2cUnit, addr, 0x31, 0x82);           /* ADI Required Write */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0x1E, 0x00);           /* ADI Required Write */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0x00, 0x80 | lane);    /* csitx_pwrdn : CSI TX Off, 1 Lane */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0xDA, 0x00);           /* mipi_pll_en : Power down MIPI PLL */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0xC1, 0x3B);           /* The clock output is forced to logic 0 */

        if ( 0 != loc_rt)
        {
            R_PRINT_Log("[R_ADV7482_Stop] Error writing register.\r\n");
            ret = R_ADV7482_ERR_FAILED;
        }
    }
    else
    {
        R_PRINT_Log("[R_ADV7482_Stop]NG. lane is %d.\r\n", lane);
        ret = R_ADV7482_ERR_INVALID_PARAMETER;
    }

    return ret;
}

r_adv7482_Error_t R_ADV7482_Init(r_adv7482_I2cChannel_t I2c, uint8_t ColorSpaceOut, uint8_t BoundaryMbps)
{
    int32_t i;
    int32_t loc_rt = 0;
    r_i2c_Unit_t i2cUnit = loc_I2cMap(I2c);
    uint8_t ColorSpaceOutValue;
    uint8_t CpOutModeValue;
    r_adv7482_Error_t ret = R_ADV7482_ERR_SUCCESS;

    ret = loc_I2cClkEnable(i2cUnit);

    if (R_ADV7482_ERR_SUCCESS == ret)
    {
        for (i = 0; i < ARRAY_SIZE(loc_CvbsHdmiInput); i++) {
            if ((loc_CvbsHdmiInput[i][0] == CMD) && (loc_CvbsHdmiInput[i][1] == WAIT_MS)) {
                delay_us(loc_CvbsHdmiInput[i][2] * 1000);
            } else if ((loc_CvbsHdmiInput[i][0] == CMD) && (loc_CvbsHdmiInput[i][1] == COLOR_SPACE_OUT)) {
                if(ColorSpaceOut == R_ADV7482_COLOR_SPACE_OUT_RGB)
                {
                    ColorSpaceOutValue = 0x02; /* RGB color space output */
                    CpOutModeValue = 0xF0;     /* SDR 444 (4-lane MIPI CSI output only) */
                }
                else /* R_ADV7482_COLOR_SPACE_OUT_YCRCB */
                {
                    ColorSpaceOutValue = 0x00; /* YPbPr color space output */
                    CpOutModeValue = 0xF2;     /* SDR 422 2ch (4-lane MIPI CSI output only) */
                }
                /* CP_RGB_OUT. set clolor space output */
                loc_rt = loc_RegWrite(i2cUnit, 0xE0,
                                        (uint8_t)0x04,
                                        ColorSpaceOutValue);
                if (0 != loc_rt)
                {
                    R_PRINT_Log("[R_ADV7482_Init:CP_RGB_OUT] loc_RegWrite failed(%d).\r\n", loc_rt);
                    ret = R_ADV7482_ERR_FAILED;
                }
                /* CP_OUT_MODE. set clolor space output */
                loc_rt = loc_RegWrite(i2cUnit, 0xE0,
                                        (uint8_t)0x12,
                                        CpOutModeValue);
                if (0 != loc_rt)
                {
                    R_PRINT_Log("[R_ADV7482_Init:CP_OUT_MODE] loc_RegWrite failed(%d).\r\n", loc_rt);
                    ret = R_ADV7482_ERR_FAILED;
                }
            } else if ((loc_CvbsHdmiInput[i][0] == CMD) && (loc_CvbsHdmiInput[i][1] == BOUNDARY_600MBPS)) {
                /* CP_RGB_OUT. set clolor space output */
                loc_rt = loc_RegWrite(i2cUnit, 0x94,
                                        (uint8_t)0xDB,
                                        BoundaryMbps);
                if (0 != loc_rt)
                {
                    R_PRINT_Log("[R_ADV7482_Init:CP_RGB_OUT] loc_RegWrite failed(%d).\r\n", loc_rt);
                    ret = R_ADV7482_ERR_FAILED;
                }
            } else {
                loc_rt = loc_RegWrite(i2cUnit, loc_CvbsHdmiInput[i][0],
                                       (uint8_t)loc_CvbsHdmiInput[i][1],
                                       (uint8_t)loc_CvbsHdmiInput[i][2]);
                if (0 != loc_rt) {
                    R_PRINT_Log("[R_ADV7482_Init] loc_RegWrite failed(%d). loop count is %d\r\n", loc_rt, i);
                    ret = R_ADV7482_ERR_FAILED;
                }
            }
        }
    }

    return ret;
}

r_adv7482_Error_t R_ADV7482_Start_csi40(r_adv7482_I2cChannel_t I2c, uint8_t lane)
{
    int8_t addr = R_ADV7482_I2CADDR_CSI_TXA;
    r_i2c_Unit_t i2cUnit = loc_I2cMap(I2c);
    uint32_t loc_rt = 0;
    r_adv7482_Error_t ret = R_ADV7482_ERR_SUCCESS;

    /* Check the number of lane */
    if ((1 == lane) || (2 == lane) || (4 == lane))
    {
        loc_rt = loc_RegWrite(i2cUnit, addr, 0x00, 0x80 | lane);  /* csitx_pwrdn : CSI TX Off */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0x00, 0xA0 | lane);  /* Enable and use automatically computed DPHY timing parameters */

        /* MIPI settings enable */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0xF0, 0x00);         /* dphy_pwdn : DPHY is not powered down */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0x31, 0x82);         /* ADI Required Write */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0x1E, 0x40);         /* ADI Required Write */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0xDA, 0x01);         /* mipi_pll_en : Power up MIPI PLL */
        delay_us(2000);                                  /* wait 2000us (min:1250us) */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0x00, 0x20 | lane);  /* csitx_pwrdn : CSI TX ON */

        loc_rt |= loc_RegWrite(i2cUnit, addr, 0xC1, 0x2B);         /* The clock output is allowed to toggle */

        loc_rt |= loc_RegWrite(i2cUnit, addr, 0x31, 0x80);         /* ADI Required Write */

        if ( 0 != loc_rt)
        {
            R_PRINT_Log("[R_ADV7482_Start_csi40] Error writing register.\r\n");
            ret = R_ADV7482_ERR_FAILED;
        }
    }
    else
    {
        R_PRINT_Log("[R_ADV7482_Start_csi40]NG. lane is %d.\r\n", lane);
        ret = R_ADV7482_ERR_INVALID_PARAMETER;
    }

    return ret;
}

r_adv7482_Error_t R_ADV7482_Stop_csi40(r_adv7482_I2cChannel_t I2c, uint8_t lane)
{
    int8_t addr = R_ADV7482_I2CADDR_CSI_TXA;
    r_i2c_Unit_t i2cUnit = loc_I2cMap(I2c);
    uint32_t loc_rt = 0;
    r_adv7482_Error_t ret = R_ADV7482_ERR_SUCCESS;

    /* Check the number of lane */
    if ((1 == lane) || (2 == lane) || (4 == lane))
    {
        /* MIPI settings disable */
        loc_rt = loc_RegWrite(i2cUnit, addr, 0x31, 0x82);               /* ADI Required Write */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0x1E, 0x00);               /* ADI Required Write */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0x00, 0x80 | lane);        /* csitx_pwrdn : CSI TX Off, 1 Lane */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0xDA, 0x00);               /* mipi_pll_en : Power down MIPI PLL */
        loc_rt |= loc_RegWrite(i2cUnit, addr, 0xC1, 0x3B);               /* The clock output is forced to logic 0 */

        if ( 0 != loc_rt)
        {
            R_PRINT_Log("[R_ADV7482_Stop_csi40] Error writing register.\r\n");
            ret = R_ADV7482_ERR_FAILED;
        }
    }
    else
    {
        R_PRINT_Log("[R_ADV7482_Stop_csi40]NG. lane is %d.\r\n", lane);
        ret = R_ADV7482_ERR_INVALID_PARAMETER;
    }

    return ret;
}

r_adv7482_Error_t R_ADV7482_tmds_stable_wait(r_adv7482_I2cChannel_t I2c)
{
    r_i2c_Unit_t i2cUnit = loc_I2cMap(I2c);
    uint8_t data[3] = {0};
    uint8_t tmp[5] = {0};
    uint8_t quit = 0;
    uint8_t timeout = 255;
    r_adv7482_Error_t ret = R_ADV7482_ERR_SUCCESS;

    do
    {
        loc_RegRead(i2cUnit, 0xE0, 0x71, &data[0], 1);
        /* slave addr:IO_MAP(0xE0), TMDS_CLK_A_RAW(addr:0x71, bit:3) */
        tmp[0] = (data[0] >> 3) & 0x1;
        /* slave addr:IO_MAP(0xE0), TMDSPLL_LCK_A_RAW(addr:0x71, bit:7) */
        tmp[1] = (data[0] >> 7) & 0x1;

        loc_RegRead(i2cUnit, 0xE0, 0x72, &data[1], 1);
        /* slave addr:IO_MAP(0xE0), TMDS_CLK_A_ST(addr:0x72, bit:3) */
        tmp[2] = (data[1] >> 3) & 0x1;
        /* slave addr:IO_MAP(0xE0), TMDSPLL_LCK_A_ST(addr:0x72, bit:7) */
        tmp[3] = (data[1] >> 7) & 0x1;

        loc_RegRead(i2cUnit, 0xE0, 0x8B, &data[2], 1);
        /* slave addr:IO_MAP(0xE0), NEW_TMDS_FRQ_ST(addr:0x8B, bit:1) */
        tmp[4] = (data[2] >> 1) & 0x1;

        /* check "Figure 54" ADV748x_HardwareManual_RevPrB_2014-07-11.pdf */
        if ((tmp[0] == 1) && /* Is TMDS_CLK_A_RAW set?:yes */
            (tmp[1] == 1) && /* Is TMDSPLL_LCK_A_RAW set?:yes */
            (tmp[2] == 0) && /* Is TMDS_CLK_A_ST set?:no */
            (tmp[3] == 0) && /* Is TMDSPLL_LCK_A_ST set?:no */
            (tmp[4] == 0))   /* Is NEW_TMDS_FRQ_ST set?:no */
        {
            quit = 1;
        }
        else
        {
            R_OSAL_ThreadSleepForTimePeriod(100); /* 100ms */
            timeout--;
        }
    } while ((0 == quit) && (timeout > 0));

    if ( 0 == timeout)
    {
        R_PRINT_Log("[R_ADV7482_tmds_stable_wait] Timed out while waiting for tmds to stabilize.\r\n");
        ret = R_ADV7482_ERR_SUCCESS;
    }

    return ret;
}
