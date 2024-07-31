/*************************************************************************************************************
* Copyright (c) [2023] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "r_audio_common.h"

static rDmacCfg_t ssiAudmacParams = {
    .mDMAMode = DRV_DMAC_DMA_NO_DESCRIPTOR,
    .mSrcAddrMode = DRV_DMAC_ADDR_INCREMENTED,
    .mDestAddrMode = DRV_DMAC_ADDR_FIXED,
    .mTransferUnit = DRV_DMAC_TRANS_UNIT_4BYTE,
    .mResource = DRV_DMAC_MEMORY,
    .mLowSpeed = DRV_DMAC_SPEED_NORMAL,
    .mPrioLevel = 0,
};

typedef struct SsiChannel {
    uint32_t    base_address;   /* SSI Channel base address */
    uintptr_t   ssitdr;         /* SSITDRn address for audio-DMAC */
    uintptr_t   ssirdr;         /* SSITDRn address for audio-DMAC */
    uint8_t        audmacDev;        /* Device or controller */
    uint8_t        audmacCh;        /* Use a fixed Audmac channel for simplicity */
    uint8_t        audmacIndTx;    /* SSINDxTx */
    uint8_t        audmacIndRx;    /* SSINDxRx */
    uint32_t    audmacSem;      /* Audio-DMAC completion condition variable */
} SsiChannel_t; 

static SsiChannel_t xSsiChannel[] = {
    [R_AUDIO_SSI0] = {
        .base_address = R_SSI0_BASE,
        .ssitdr = SSI_SSITDR0_DMA,
        .ssirdr = SSI_SSIRDR0_DMA,
        .audmacDev = AUIDO_DMAC_DEV,
        .audmacCh = AUIDO_DMAC_CH0,
        .audmacIndTx = 1,
        .audmacIndRx = 2,
    },
    [R_AUDIO_SSI1] = {
        .base_address = R_SSI1_BASE,
        .ssitdr = SSI_SSITDR1_DMA,
        .ssirdr = SSI_SSIRDR1_DMA,
        .audmacDev = AUIDO_DMAC_DEV,
        .audmacCh = AUIDO_DMAC_CH1,
        .audmacIndTx = 1,
        .audmacIndRx = 2,
    },
    [R_AUDIO_SSI2] = {
        .base_address = R_SSI2_BASE,
        .ssitdr = SSI_SSITDR2_DMA,
        .ssirdr = SSI_SSIRDR2_DMA,        
        .audmacDev = AUIDO_DMAC_DEV,
        .audmacCh = AUIDO_DMAC_CH2,
        .audmacIndTx = 1,
        .audmacIndRx = 2,
    },
    [R_AUDIO_SSI3] = {
        .base_address = R_SSI3_BASE,
        .ssitdr = SSI_SSITDR3_DMA,
        .ssirdr = SSI_SSIRDR3_DMA,        
        .audmacDev = AUIDO_DMAC_DEV,
        .audmacCh = AUIDO_DMAC_CH3,
        .audmacIndTx = 1,
        .audmacIndRx = 2,
    },
    [R_AUDIO_SSI4] = {
        .base_address = R_SSI4_BASE,
        .ssitdr = SSI_SSITDR4_DMA,
        .ssirdr = SSI_SSIRDR4_DMA,        
        .audmacDev = AUIDO_DMAC_DEV,
        .audmacCh = AUIDO_DMAC_CH4,
        .audmacIndTx = 1,
        .audmacIndRx = 2,
    },
    [R_AUDIO_SSI5] = {
        .base_address = R_SSI5_BASE,
        .ssitdr = SSI_SSITDR5_DMA,
        .ssirdr = SSI_SSIRDR5_DMA,        
        .audmacDev = AUIDO_DMAC_DEV,
        .audmacCh = AUIDO_DMAC_CH5,
        .audmacIndTx = 1,
        .audmacIndRx = 2,
    },
    [R_AUDIO_SSI6] = {
        .base_address = R_SSI6_BASE,
        .ssitdr = SSI_SSITDR6_DMA,
        .ssirdr = SSI_SSIRDR6_DMA,        
        .audmacDev = AUIDO_DMAC_DEV,
        .audmacCh = AUIDO_DMAC_CH6,
        .audmacIndTx = 1,
        .audmacIndRx = 2,
    },
    [R_AUDIO_SSI7] = {
        .base_address = R_SSI7_BASE,
        .ssitdr = SSI_SSITDR7_DMA,
        .ssirdr = SSI_SSIRDR7_DMA,        
        .audmacDev = AUIDO_DMAC_DEV,
        .audmacCh = AUIDO_DMAC_CH7,
        .audmacIndTx = 1,
        .audmacIndRx = 2,
    },
    [R_AUDIO_SSI8] = {
        .base_address = R_SSI8_BASE,
        .ssitdr = SSI_SSITDR8_DMA,
        .ssirdr = SSI_SSIRDR8_DMA,        
        .audmacDev = AUIDO_DMAC_DEV,
        .audmacCh = AUIDO_DMAC_CH8,
        .audmacIndTx = 1,
        .audmacIndRx = 2,
    },
    [R_AUDIO_SSI9] = {
        .base_address = R_SSI9_BASE,
        .ssitdr = SSI_SSITDR9_DMA,
        .ssirdr = SSI_SSIRDR9_DMA,        
        .audmacDev = AUIDO_DMAC_DEV,
        .audmacCh = AUIDO_DMAC_CH9,
        .audmacIndTx = 1,
        .audmacIndRx = 2,
    },
};

static r_cpg_ClockName_t find_cpg_module(r_audio_channel_t ch)
{
    r_cpg_ClockName_t SsiModule = R_CPG_SSI_ALL_CLOCK;

    switch (ch) {
    case R_AUDIO_SSI0: 
        SsiModule = R_CPG_SSI0_CLOCK;
    break;

    case R_AUDIO_SSI1: 
        SsiModule = R_CPG_SSI1_CLOCK;
    break;

    case R_AUDIO_SSI2: 
        SsiModule = R_CPG_SSI2_CLOCK;
    break;

    case R_AUDIO_SSI3: 
        SsiModule = R_CPG_SSI3_CLOCK;
    break;

    case R_AUDIO_SSI4: 
        SsiModule = R_CPG_SSI4_CLOCK;
    break;

    case R_AUDIO_SSI5: 
        SsiModule = R_CPG_SSI5_CLOCK;
    break;

    case R_AUDIO_SSI6: 
        SsiModule = R_CPG_SSI6_CLOCK;
    break;

    case R_AUDIO_SSI7: 
        SsiModule = R_CPG_SSI7_CLOCK;
    break;

    case R_AUDIO_SSI8: 
        SsiModule = R_CPG_SSI8_CLOCK;
    break;

    case R_AUDIO_SSI9: 
        SsiModule = R_CPG_SSI9_CLOCK;
    break;

    default:
        SsiModule = R_CPG_SSI_ALL_CLOCK;
        break;
    }

    return SsiModule;
}

static uint32_t clkEnable(r_audio_channel_t ch)
{
    int ret = 0;
    r_cpg_ClockName_t SsiModule = R_CPG_SSI_ALL_CLOCK;

    SsiModule = find_cpg_module(ch);
    R_PMA_CPG_SetClockStopStat(SsiModule, false);

    return ret;
}

static uint32_t clkDisable(r_audio_channel_t ch)
{
    int ret = 0;
    r_cpg_ClockName_t SsiModule = R_CPG_SSI_ALL_CLOCK;

    SsiModule = find_cpg_module(ch);
    R_PMA_CPG_SetClockStopStat(SsiModule, true);

    return ret;
}

/* Output BICK,LRCK */
void R_SSI_Init(r_audio_channel_t chNo)
{
    SsiChannel_t *pxSsi = &xSsiChannel[chNo];
    uint32_t base_address = 0;

    base_address = pxSsi->base_address;

    /* Enable the SSI clock */
    clkEnable(R_AUDIO_SSI_ALL);
    clkEnable(chNo);

    InitAudioDMAC(AUIDO_DMAC_DEV);

    pxSsi->audmacSem = 1;

    /* Independent SSI transfer mode */
    writel(0x000003FF, SSIU_SSI_MODE0);

    /* Setup 44.1kHz sample playback */
    // TODO: support 48KHz sample playback
    writel(0x900BD122, (base_address + SSI_CR));

    /* 16 bits sample */
    writel(0x00000100, (base_address + SSI_WSR));
}

void R_SSI_Release(r_audio_channel_t chNo)
{
    ReleaseAudioDMAC(AUIDO_DMAC_DEV);

    clkDisable(chNo);
}

void R_SSI_StartDmac(r_audio_channel_t chNo, uint32_t play, 
            uintptr_t addr, uint32_t len)
{
    SsiChannel_t *pxSsi = &xSsiChannel[chNo];
    uint32_t dataL;
    uint32_t base_address = 0;

    base_address = pxSsi->base_address;

    /* Wait Idle Status Flag of SSI0(Play) */
    while (1) {
        dataL = readl(base_address + SSI_SR);
        if (dataL & BIT(0))
            break;
    }

    if (play) {
        ssiAudmacParams.mSrcAddr  = addr;
        ssiAudmacParams.mDestAddr = pxSsi->ssitdr;
        ssiAudmacParams.mResource = pxSsi->audmacIndTx;
    } else {
        ssiAudmacParams.mSrcAddr  = pxSsi->ssirdr;
        ssiAudmacParams.mDestAddr = addr;
        ssiAudmacParams.mResource = pxSsi->audmacIndRx;
    }

    ssiAudmacParams.mTransferCount = len / 4;

    /* SSIn(Play) Module Enable */
    writel(readl(base_address + SSI_CR) | BIT(0), (base_address + SSI_CR));
    /* SSIn(Play) not Mute */
    writel(readl(base_address + SSI_CR) & ~BIT(3), (base_address + SSI_CR));
    R_AUDMAC_CopyMemory(ssiAudmacParams.mDestAddr, ssiAudmacParams.mSrcAddr, len, ssiAudmacParams.mResource, (uintptr_t)pxSsi);
}

void R_SSI_StopDmac(r_audio_channel_t chNo)
{
    uint32_t base_address = 0;
    SsiChannel_t *pxSsi = &xSsiChannel[chNo];
    base_address = pxSsi->base_address;

    R_AUDMAC_Stop(pxSsi->audmacDev, pxSsi->audmacCh);

    writel(readl(base_address + SSI_CR) & ~BIT(0), (base_address + SSI_CR));
}