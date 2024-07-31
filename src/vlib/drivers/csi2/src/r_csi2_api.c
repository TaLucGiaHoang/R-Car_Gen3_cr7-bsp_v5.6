/*************************************************************************************************************
* csi2_api_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "r_csi2_api.h"
#include "r_io.h"
#include "r_csi2_regs.h"
#include "r_cpg_api.h"
#include "r_prr_api.h"
#include "r_delay_api.h"
#include "r_print_api.h"

#define BIT(nr)         (1UL << (nr))
#define BIT_MASK(nr)        (BIT(nr) - 1)

#define ECC_EN          BIT(1)
#define CRC_EN          BIT(0)
#define VCTD_EN         BIT(15)

#define DWEN            BIT(24)
#define CWEN            BIT(8)

#define ENABLECLK       BIT(4)

#define ICLK_NONSTOP    BIT(24)
#define MONIPACT_EN	    BIT(25)

#define SHUTDOWNZ       BIT(17)
#define RSTZ            BIT(16)

#define STOPSTATECLK    BIT(0)

#define ECC_EN          BIT(1)
#define CRC_EN          BIT(0)

#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))

#define R_CSI2_CSI20    0u
#define R_CSI2_CSI40    1u


static r_Csi2_t xCsi2[] = {
	/* e.g H3 */
    [RCAR_CSI20] = {
        .Base = (uint32_t)RCAR_CSI2_CSI20_BASE,
        .Lanes = 0,
        .DataRate = 0,
        .Interlaced = false,
        .CpgModule = R_CSI2_CSI20,     /**< CSI unit 0 */
        .Vcdt = VCTD_EN,
        .Vcdt2 = 0,
    },
    /* Below settings were not verified */
    /* e.g. E3 */
    [RCAR_CSI40] = {
        .Base = (uint32_t)RCAR_CSI2_CSI40_BASE,
        .Lanes = 0,
        .DataRate = 0,
        .Interlaced = false,
        .CpgModule = R_CSI2_CSI40,
        .Vcdt = 0x81008000,
        .Vcdt2 = 0x83008200,
    },
};

/* Tables 25.9 (3/H3-N/V3H/M3-N) and 25.10 (M3-W) */
static uint16_t hsFreqRange[][3]= {
	/* Mbps, H3/H3-N/V3H/M3-N, M3-W */
	{   80, 0x00, 0x00 },
	{   90, 0x10, 0x10 },
	{  100, 0x20, 0x20 },
	{  110, 0x30, 0x30 },
	{  120, 0x01, 0x01 },
	{  130, 0x11, 0x11 },
	{  140, 0x21, 0x21 },
	{  150, 0x31, 0x31 },
	{  160, 0x02, 0x02 },
	{  170, 0x12, 0x12 },
	{  180, 0x22, 0x22 },
	{  190, 0x32, 0x32 },
	{  205, 0x03, 0x03 },
	{  220, 0x13, 0x13 },
	{  235, 0x23, 0x23 },
	{  250, 0x33, 0x33 },
	{  275, 0x04, 0x04 },
	{  300, 0x14, 0x14 },
	{  325, 0x25, 0x05 },
	{  350, 0x35, 0x15 },
	{  400, 0x05, 0x25 },
	{  450, 0x16, 0x06 },
	{  500, 0x26, 0x16 },
	{  550, 0x37, 0x07 },
	{  600, 0x07, 0x17 },
	{  650, 0x18, 0x08 },
	{  700, 0x28, 0x18 },
	{  750, 0x39, 0x09 },
	{  800, 0x09, 0x19 },
	{  850, 0x19, 0x29 },
	{  900, 0x29, 0x39 },
	{  950, 0x3a, 0x0a },
	{ 1000, 0x0a, 0x1a },
	{ 1050, 0x1a, 0x2a },
	{ 1100, 0x2a, 0x3a },
	{ 1150, 0x3b, 0x0b },
	{ 1200, 0x0b, 0x1b },
	{ 1250, 0x1b, 0x2b },
	{ 1300, 0x2b, 0x3b },
	{ 1350, 0x3c, 0x0c },
	{ 1400, 0x0c, 0x1c },
	{ 1450, 0x1c, 0x2c },
	{ 1500, 0x2c, 0x3c },
};


#define RCAR_CSI2(Ch, off)  ((Ch)->Base + RCAR_CSI2_##off)


static void R_CSI_PRV_WriteReg(r_Csi2_t *Ch, uint32_t value, uint32_t offset)
{
    writel(value, Ch->Base + offset);
}

static uint32_t R_CSI_PRV_ReadReg(r_Csi2_t *Ch, uint32_t offset)
{
    return readl(Ch->Base + offset);
}

static void loc_PhyStart(r_Csi2_t *Ch)
{
    uint32_t mbpsPerLane = Ch->DataRate / Ch->Lanes;
    uint32_t phypll = 0xff;
    int i;
    int hsFreq_index;

	if (R_PRR_GetDevice() == R_PRR_RCARM3W) {
		hsFreq_index = 2;
	} else {
		hsFreq_index = 1;
	}

    /*
     * H3 specific sequence. Adapt accordingly for other SoCs if needed
     */
    /* Write a fixed sequence to Test Interface write register & verify */
    R_CSI_PRV_WriteReg(Ch, 0x01CC01E2, RCAR_CSI2_PHTW);
    while((DWEN | CWEN) & R_CSI_PRV_ReadReg(Ch, RCAR_CSI2_PHTW));

    R_CSI_PRV_WriteReg(Ch, 0x010101E3, RCAR_CSI2_PHTW);
    while((DWEN | CWEN) & R_CSI_PRV_ReadReg(Ch, RCAR_CSI2_PHTW));

    R_CSI_PRV_WriteReg(Ch, 0x011101E4, RCAR_CSI2_PHTW);
    while((DWEN | CWEN) & R_CSI_PRV_ReadReg(Ch, RCAR_CSI2_PHTW));

    R_CSI_PRV_WriteReg(Ch, 0x010101E5, RCAR_CSI2_PHTW);
    while((DWEN | CWEN) & R_CSI_PRV_ReadReg(Ch, RCAR_CSI2_PHTW));

    R_CSI_PRV_WriteReg(Ch, 0x01100104, RCAR_CSI2_PHTW);
    while((DWEN | CWEN) & R_CSI_PRV_ReadReg(Ch, RCAR_CSI2_PHTW));

    if (mbpsPerLane <= 250) {
        R_CSI_PRV_WriteReg(Ch, 0x01390105, RCAR_CSI2_PHTW);
        while((DWEN | CWEN) & R_CSI_PRV_ReadReg(Ch, RCAR_CSI2_PHTW));
    }

    R_CSI_PRV_WriteReg(Ch, 0x01380108, RCAR_CSI2_PHTW);
    while((DWEN | CWEN) & R_CSI_PRV_ReadReg(Ch, RCAR_CSI2_PHTW));

    R_CSI_PRV_WriteReg(Ch, 0x01010100, RCAR_CSI2_PHTW);
    while((DWEN | CWEN) & R_CSI_PRV_ReadReg(Ch, RCAR_CSI2_PHTW));

    R_CSI_PRV_WriteReg(Ch, 0x014B01AC, RCAR_CSI2_PHTW);
    while((DWEN | CWEN) & R_CSI_PRV_ReadReg(Ch, RCAR_CSI2_PHTW));

    R_CSI_PRV_WriteReg(Ch, 0x01030100, RCAR_CSI2_PHTW);
    while((DWEN | CWEN) & R_CSI_PRV_ReadReg(Ch, RCAR_CSI2_PHTW));

    R_CSI_PRV_WriteReg(Ch, 0x01800107, RCAR_CSI2_PHTW);
    while((DWEN | CWEN) & R_CSI_PRV_ReadReg(Ch, RCAR_CSI2_PHTW));

    /* Set HSFREQRANGE bits in PHYPLL based on the input Rx freq */
    for (i = 0; i < ARRAY_SIZE(hsFreqRange); i++) {
        if (mbpsPerLane <= hsFreqRange[i][0]) {
            phypll = hsFreqRange[i][hsFreq_index] << 16;
            break;
        }
    }

    R_CSI_PRV_WriteReg(Ch, phypll, RCAR_CSI2_PHYPLL);

    /* Set CSI0CLKFREQRANGE tp 0x20 - fixed value */
    R_CSI_PRV_WriteReg(Ch, 0x20 << 16, RCAR_CSI2_CSI0CLKFCPR);

    /* Set desired lanes & ENABLECLK */
    R_CSI_PRV_WriteReg(Ch, ENABLECLK | BIT_MASK(Ch->Lanes), RCAR_CSI2_PHYCNT);
    /* Set both of the SHUTDOWNZ and RSTZ bits in PHYCNT to 1 */
    R_CSI_PRV_WriteReg(Ch, SHUTDOWNZ | R_CSI_PRV_ReadReg(Ch, RCAR_CSI2_PHYCNT), RCAR_CSI2_PHYCNT);
    R_CSI_PRV_WriteReg(Ch, RSTZ | R_CSI_PRV_ReadReg(Ch, RCAR_CSI2_PHYCNT), RCAR_CSI2_PHYCNT);
}

static r_csi2_Error_t loc_CsiPowerOn(r_Csi2_t *Ch)
{
    r_cpg_Error_t cpg_rt = R_CPG_ERR_SUCCESS;
    r_csi2_Error_t ret = R_CSI2_ERR_SUCCESS;

    /* Stop CSI0 clock */
    cpg_rt = R_PMA_CPG_SetClockStopStat(R_CPG_CSI0_CLOCK, true);

    /* Enable CSI0 clock */
    cpg_rt |= R_PMA_CPG_SetClockStopStat(R_CPG_CSI0_CLOCK, false);

    if (R_CSI2_CSI20 == Ch->CpgModule)
    {
        /* Enable CSI20 */
        cpg_rt |= R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_7, R_CPG_BIT14, false);

        /* Reset CSI20 */
        cpg_rt |= R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT14, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        cpg_rt |= R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT14, 0);
    }
    else
    {
        /* Enable CSI40 */
        cpg_rt |= R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_7, R_CPG_BIT16, false);

        /* Reset CSI40 */
        cpg_rt |= R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT16, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        cpg_rt |= R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT16, 0);
    }

    if (R_CPG_ERR_SUCCESS != cpg_rt)
    {
        R_PRINT_Log("[loc_CsiPowerOn]NG. cpg_rt:%d\n", cpg_rt);
        ret = R_CSI2_ERR_FAILED;
    }

    return ret;
}

static r_csi2_Error_t loc_Csi2Init(r_Csi2_t *Ch)
{
    r_csi2_Error_t ret = R_CSI2_ERR_SUCCESS;
    int timeout = 100;

    ret = loc_CsiPowerOn(Ch);

    if (R_CSI2_ERR_SUCCESS == ret)
    {
        /* Reset channel */
        R_CSI_PRV_WriteReg(Ch, 0x1, RCAR_CSI2_TREF);

        R_CSI_PRV_WriteReg(Ch, BIT(0), RCAR_CSI2_SRST);

        delay_us(100);

        R_CSI_PRV_WriteReg(Ch, 0x0, RCAR_CSI2_SRST);

        R_CSI_PRV_WriteReg(Ch, 0x0, RCAR_CSI2_PHTC);
        if (true == Ch->Interlaced)
        {
            R_CSI_PRV_WriteReg(Ch, 0x1000f, RCAR_CSI2_FLD);
        }
        /* Start PHY & get it stable */
        loc_PhyStart(Ch);

        /* ECC, CRC ON */
        R_CSI_PRV_WriteReg(Ch, ECC_EN | CRC_EN, RCAR_CSI2_CHKSUM);

        R_CSI_PRV_WriteReg(Ch, Ch->Vcdt, RCAR_CSI2_VCDT);
        /* Enable Virtual channel 1 only */
        R_CSI_PRV_WriteReg(Ch, Ch->Vcdt2, RCAR_CSI2_VCDT2);

        /* Frame start = 0x0, Frame end = 0x1 */
        R_CSI_PRV_WriteReg(Ch, 0x00010000, RCAR_CSI2_FRDT);
        delay_us(100);
        R_CSI_PRV_WriteReg(Ch, 0x81000000, RCAR_CSI2_LINKCNT);

        /* Read the PHY clock lane monitor register (PHCLM). */
        while (!(R_CSI_PRV_ReadReg(Ch, RCAR_CSI2_PHCLM) & 0x01) && timeout)
        {
            delay_us(100);
            timeout--;
        }
        if (timeout == 0)
        {
            R_PRINT_Log("Timeout of reading the PHY clock lane \r\n");
            ret = R_CSI2_ERR_PHY_TIMEOUT;
        }
        timeout = 100;

        /* Read the PHY data lane monitor register (PHDLM). */
        while (!(R_CSI_PRV_ReadReg(Ch, RCAR_CSI2_PHDLM) & 0x01) && timeout)
        {
            delay_us(10);
            timeout--;
        }
        if (timeout == 0)
        {
            R_PRINT_Log("Timeout of reading the PHY data lane \r\n");
            ret = R_CSI2_ERR_PHY_TIMEOUT;
        }
    }

    return ret;
}

r_csi2_Error_t R_CSI2_Init(r_csi2_Channel_t Ch, uint16_t DataRate, bool Interlaced, uint8_t Lanes)
{
    r_csi2_Error_t ret = R_CSI2_ERR_SUCCESS;

    /* Check the number of lanes */
    if (RCAR_CSI20 == Ch)
    {
        if (1 != Lanes)
        {
            ret = R_CSI2_ERR_INVALID_PARAMETER;
        }
    }
    else
    {
        if (!((1 == Lanes) || (2 == Lanes) || (4 == Lanes)))
        {
            /* Lanes is a value other than 1,2,4 */
            ret = R_CSI2_ERR_INVALID_PARAMETER;
        }
    }

    if (R_CSI2_ERR_SUCCESS == ret)
    {
        /* DataRate settings */
        xCsi2[Ch].DataRate = DataRate;
        /* Lanes settings */
        xCsi2[Ch].Lanes = Lanes;
        /* Interlaced settings */
        xCsi2[Ch].Interlaced = Interlaced;

        ret = loc_Csi2Init(&xCsi2[Ch]);
    }
    else
    {
        R_PRINT_Log("[R_CSI2_Init]NG. Lanes is %d.\r\n", Lanes);
    }

    return ret;
}

r_csi2_Error_t R_CSI2_DeInit(r_csi2_Channel_t Ch)
{
    r_cpg_Error_t cpg_rt = R_CPG_ERR_SUCCESS;
    r_csi2_Error_t ret = R_CSI2_ERR_SUCCESS;

    if (RCAR_CSI20 == Ch)
    {
        /* Reset CSI20 */
        cpg_rt = R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT14, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        cpg_rt |= R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT14, 0);
    }
    else
    {
        /* Reset CSI40 */
        cpg_rt = R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT16, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        cpg_rt |= R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT16, 0);
    }

    if (R_CPG_ERR_SUCCESS != cpg_rt)
    {
        R_PRINT_Log("[R_CSI2_DeInit]NG. cpg_rt:%d\n", cpg_rt);
        ret = R_CSI2_ERR_FAILED;
    }

    return ret;
}

