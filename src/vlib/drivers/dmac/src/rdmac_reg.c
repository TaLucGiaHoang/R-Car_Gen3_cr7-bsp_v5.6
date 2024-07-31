/*************************************************************************************************************
* dmac_reg_c
* Copyright (c) [2017-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "drv.h"
#include "rdmac_common.h"
#include "rdmac_reg.h"

/***********************************************************
Macro definitions
***********************************************************/

/***********************************************************
Typedef definitions
***********************************************************/

/***********************************************************
Exported global variables (to be accessed by other files)
***********************************************************/

/***********************************************************
Private global variables and functions
***********************************************************/
static volatile uint32_t fBaseAddres[DRV_DMAC_UNIT_NUM] = {
    DRV_AUDMAC_REG_ADDR_DMAC0,
    DRV_AUDMAC_REG_ADDR_DMAC1,
    DRV_RTDMAC_REG_ADDR_DMAC0,
    DRV_RTDMAC_REG_ADDR_DMAC1
};

const uint32_t rDmacCH_MaskTable[] = {
    DRV_DMAC_REG_MASK_CH0,      /* CH0 use bit mask */
    DRV_DMAC_REG_MASK_CH1,      /* CH1 use bit mask */
    DRV_DMAC_REG_MASK_CH2,      /* CH2 use bit mask */
    DRV_DMAC_REG_MASK_CH3,      /* CH3 use bit mask */
    DRV_DMAC_REG_MASK_CH4,      /* CH4 use bit mask */
    DRV_DMAC_REG_MASK_CH5,      /* CH5 use bit mask */
    DRV_DMAC_REG_MASK_CH6,      /* CH6 use bit mask */
    DRV_DMAC_REG_MASK_CH7,      /* CH7 use bit mask */
    DRV_DMAC_REG_MASK_CH8,      /* CH8 use bit mask */
    DRV_DMAC_REG_MASK_CH9,      /* CH9 use bit mask */
    DRV_DMAC_REG_MASK_CH10,     /* CH10 use bit mask */
    DRV_DMAC_REG_MASK_CH11,     /* CH11 use bit mask */
    DRV_DMAC_REG_MASK_CH12,     /* CH12 use bit mask */
    DRV_DMAC_REG_MASK_CH13,     /* CH13 use bit mask */
    DRV_DMAC_REG_MASK_CH14,     /* CH14 use bit mask */
    DRV_DMAC_REG_MASK_CH15,     /* CH15 use bit mask */
};

static const uint32_t rDmacPriorityMode_Table[] = {
    DRV_DMAC_REG_PRIO_FIX,
    DRV_DMAC_REG_PRIO_RR,
};

static const uint32_t rDmacTransferUnit_Table[] = {
    DRV_DMAC_REG_TRANS_UNIT_1BYTE,
    DRV_DMAC_REG_TRANS_UNIT_2BYTE,
    DRV_DMAC_REG_TRANS_UNIT_4BYTE,
    DRV_DMAC_REG_TRANS_UNIT_8BYTE,
    DRV_DMAC_REG_TRANS_UNIT_16BYTE,
    DRV_DMAC_REG_TRANS_UNIT_32BYTE,
    DRV_DMAC_REG_TRANS_UNIT_64BYTE,
};

/*!
 *  @brief          Write RTDMAC register with the dev number
 *  @details
 *  @param[in]      dev  : dev number
 *  @param[in]      reg  : RTDMAC register
 *  @param[in]      data : data to write
 *  @return         None
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
static void rDmacUnitWriteReg32(uint8_t dev, uint32_t reg, uint32_t data)
{
    uint32_t regAddr = fBaseAddres[dev] + reg;
    *(volatile uint32_t*)regAddr = data;
}

/*!
 *  @brief          Read RTDMAC register with the dev number
 *  @details
 *  @param[in]      dev  : dev number
 *  @param[in]      reg  : RTDMAC register
 *  @return         data : data to be read
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
static uint32_t rDmacUnitReadReg32(uint8_t dev, uint32_t reg)
{
    uint32_t regAddr = fBaseAddres[dev] + reg;
    uint32_t data;
    data = *(volatile uint32_t*)regAddr;
    return data;
}

/*!
 *  @brief          Write RTDMAC register with the SubChannel number
 *  @details
 *  @param[in]      dev  : dev number
 *  @param[in]      ch   : Sub Ch number
 *  @param[in]      reg  : RTDMAC register
 *  @param[in]      data : data to write
 *  @return         None
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
static void rDmacWriteReg32(uint8_t dev, uint8_t ch, uint32_t reg, uint32_t data)
{
    uint32_t regAddr = fBaseAddres[dev] + DRV_DMAC_CH_REG_ADDR + ch * DRV_DMAC_CH_REG_SIZE + reg;
    *(volatile uint32_t*)regAddr = data;
}

/*!
 *  @brief          Read RTDMAC register with the SubChannel number
 *  @details
 *  @param[in]      dev  : dev number
 *  @param[in]      ch   : Sub Ch number
 *  @param[in]      reg  : RTDMAC register
 *  @return         data : data to be read
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
static uint32_t rDmacReadReg32(uint8_t dev, uint8_t ch, uint32_t reg)
{
    uint32_t regAddr = fBaseAddres[dev] + DRV_DMAC_CH_REG_ADDR + ch * DRV_DMAC_CH_REG_SIZE + reg;
    uint32_t data;
    data = *(volatile uint32_t*)regAddr;
    return data;
}

/*!
 *  @brief          Write RTDMAC register with the dev number
 *  @details
 *  @param[in]      dev  : dev number
 *  @param[in]      reg  : RTDMAC register
 *  @param[in]      data : data to write
 *  @return         None
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
static void rDmacUnitWriteReg16(uint8_t dev, uint32_t reg, uint32_t data)
{
    uint32_t regAddr = fBaseAddres[dev] + reg;
    *(volatile uint16_t*)regAddr = data;
}

/*!
 *  @brief          Read RTDMAC register with the dev number
 *  @details
 *  @param[in]      dev  : dev number
 *  @param[in]      reg  : RTDMAC register
 *  @return         data : data to be read
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
static uint16_t rDmacUnitReadReg16(uint8_t dev, uint32_t reg)
{
    uint32_t regAddr = fBaseAddres[dev] + reg;
    uint16_t data;
    data = *(volatile uint16_t*)regAddr;
    return data;
}

/*!
 *  @brief          Write RTDMAC register with the SubChannel number
 *  @details
 *  @param[in]      dev  : dev number
 *  @param[in]      ch   : Sub Ch number
 *  @param[in]      reg  : RTDMAC register
 *  @param[in]      data : data to write
 *  @return         None
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
static void rDmacWriteReg16(uint8_t dev, uint8_t ch, uint32_t reg, uint32_t data)
{
    uint32_t regAddr = fBaseAddres[dev] + DRV_DMAC_CH_REG_ADDR + ch * DRV_DMAC_CH_REG_SIZE + reg;
    *(volatile uint16_t*)regAddr = data;
}

/**
 * Get interrupt state of RDMISTA.
 *
 * The rDmacGet_RDMISTA() function.
 * @param[in]  dev : dev number
 * @return     interrupt state of RDMISTA.
 * @attention
 */
uint32_t rDmacGet_RDMISTA(uint8_t dev)
{
    return rDmacUnitReadReg32(dev, RDMISTA);
}

/**
 * Get operation register RDMOR.
 *
 * The rDmacGet_RDMOR() function.
 * @param[in]  dev : dev number
 * @return     operation register RDMOR.
 * @attention
 */
uint32_t rDmacGet_RDMOR(uint8_t dev)
{
    return (uint32_t)rDmacUnitReadReg16(dev, RDMOR);
}

/**
 * Get channel control register RDMCHCR.
 *
 * The rDmacGet_RDMCHCR() function.
 * @param[in]  dev : dev number
 * @param[in]  ch  : Sub Ch number
 * @return     channel control register RDMCHCR.
 * @attention
 */
uint32_t rDmacGet_RDMCHCR(uint8_t dev, uint8_t ch)
{
    return rDmacReadReg32(dev, ch, RDMCHCR);
}

/**
 * @brief       The rDmacGet_RDMDPCR() function.
 * @details     Get descriptor control register RDMDPCR.
 * @param[in]   dev : dev number
 * @param[in]   ch  : Sub Ch number
 * @return      channel control register RDMDPCR.
 * @attention
 */
uint32_t rDmacGet_RDMDPCR(uint8_t dev, uint8_t ch)
{
    return rDmacReadReg32(dev, ch, RDMDPCR);
}

/**
 *  @brief          Update RDMOR
 *  @details
 *  @param[in]      dev  : dev number
 *  @param[in]      mode : priority mode
 *  @return         void
 *  @par    Refer   (none)
 *  @par    Modify  (none)
 */
void rDmacUpdate_RDMOR(uint8_t dev, rDmacPriorityMode_t mode)
{
    uint8_t Value;
    regRDMOR_t rRDMOR;

    Value = rDmacPriorityMode_Table[mode];
    rRDMOR.INT = rDmacUnitReadReg16(dev, RDMOR);
    rRDMOR.bit.PR = Value;
    rRDMOR.bit.DME = DRV_DMAC_RDMOR_DME_ENABLE;
    rDmacUnitWriteReg16(dev, RDMOR, rRDMOR.INT);
}

/**
 *  @brief          Update RDMCHCLR
 *  @details
 *  @param[in]      dev : dev number
 *  @param[in]      ch  : Sub Ch number
 *  @return         void
 *  @par    Refer   (none)
 *  @par    Modify  (none)
 */
void rDmacUpdate_RDMCHCLR(uint8_t dev, uint8_t ch)
{
    regRDMCHCLR_t rRDMCHCLR;

    rRDMCHCLR.INT = 0;
    rRDMCHCLR.INT |= rDmacCH_MaskTable[ch];
    rDmacUnitWriteReg32(dev, RDMCHCLR, rRDMCHCLR.INT);
}

/**
 *  @brief          Update RDMSAR
 *  @details
 *  @param[in]      dev   : dev number
 *  @param[in]      ch    : Sub Ch number
 *  @param[in]      Value : src data address
 *  @return         void
 *  @par    Refer   (none)
 *  @par    Modify  (none)
 */
void rDmacUpdate_RDMSAR(uint8_t dev, uint8_t ch, uint64_t Value)
{
    rDmacWriteReg32(dev, ch, RDMSAR, (uint32_t)(Value & DRV_DMAC_REG_MASK_LOWER_RDMSAR));
    rDmacWriteReg32(dev, ch, RDMFIXSAR, ((Value >> 32) & DRV_DMAC_REG_MASK_UPPER_RDMSAR));
}

/**
 *  @brief          Update RDMDAR
 *  @details
 *  @param[in]      dev   : dev number
 *  @param[in]      ch    : Sub Ch number
 *  @param[in]      Value : dst data address
 *  @return         void
 *  @par    Refer   (none)
 *  @par    Modify  (none)
 */
void rDmacUpdate_RDMDAR(uint8_t dev, uint8_t ch, uint64_t Value)
{
    rDmacWriteReg32(dev, ch, RDMDAR, (uint32_t)(Value & DRV_DMAC_REG_MASK_LOWER_RDMDAR));
    rDmacWriteReg32(dev, ch, RDMFIXDAR, ((Value >> 32) & DRV_DMAC_REG_MASK_UPPER_RDMDAR));
}

/**
 *  @brief          Update RDMTCR
 *  @details
 *  @param[in]      dev   : dev number
 *  @param[in]      ch    : Sub Ch number
 *  @param[in]      Value : dst data address
 *  @return         void
 *  @par    Refer   (none)
 *  @par    Modify  (none)
 */
void rDmacUpdate_RDMTCR(uint8_t dev, uint8_t ch, uint32_t Value)
{
    rDmacWriteReg32(dev, ch, RDMTCR, Value);
}


/**
 *  @brief          Update RDMCHCR
 *  @details
 *  @param[in]      dev     : dev number
 *  @param[in]      ch      : Sub Ch number
 *  @param[in]      cfg     : dmac config
 *  @param[in]      descCfg : desc config
 *  @return         void
 *  @par    Refer   (none)
 *  @par    Modify  (none)
 */
void rDmacUpdate_RDMCHCR(uint8_t dev, uint8_t ch, rDmacCfg_t *cfg, rDmacDescCfg_t *descCfg)
{
    uint8_t Value;
    regRDMCHCR_t rRDMCHCR;

    rRDMCHCR.INT = rDmacReadReg32(dev, ch, RDMCHCR);
    Value = cfg->mDMAMode;
    if (DRV_DMAC_DMA_DESC_INFINITE == Value)
    {
        Value = DRV_DMAC_DMA_DESC_READOUT;
    }

    if ((DRV_DMAC_DMA_DESC_READOUT == Value) && (0 == descCfg->mStateEndEnable))
    {
    }
    else
    {
        rRDMCHCR.bit.IE = DRV_DMAC_RDMCHCR_IE_ENABLE;
    }

    rRDMCHCR.bit.DPM = Value;
    rRDMCHCR.bit.SM = cfg->mSrcAddrMode;
    rRDMCHCR.bit.DM = cfg->mDestAddrMode;
    Value = rDmacTransferUnit_Table[cfg->mTransferUnit];
    rRDMCHCR.bit.TS01 = Value & DRV_DMAC_REG_MASK_RDMCHCR_TS;
    rRDMCHCR.bit.TS23 = (Value >> 2) & DRV_DMAC_REG_MASK_RDMCHCR_TS;
    if (DRV_DMAC_MEMORY == cfg->mResource)
    {
        rRDMCHCR.bit.RS = DRV_DMAC_RDMCHCR_RS_AUTO_REQEST;
    }
    else
    {
        rRDMCHCR.bit.RS = DRV_DMAC_RDMCHCR_RS_EXTENDED_RESOUSE;
    }

    if (DRV_DMAC_DMA_NO_DESCRIPTOR != cfg->mDMAMode)
    {
        rRDMCHCR.bit.RPT2 = descCfg->mDescUpdate.mSrcAddrUpdate;
        rRDMCHCR.bit.RPT1 = descCfg->mDescUpdate.mDestAddrUpdate;
        rRDMCHCR.bit.RPT0 = descCfg->mDescUpdate.mTransCountUpdate;
        rRDMCHCR.bit.DPB = descCfg->mDescRead1st;
        rRDMCHCR.bit.DSIE = descCfg->mStateEndEnable;
    }
    rRDMCHCR.bit.CAIE = DRV_DMAC_RDMCHCR_CAIE_ENABLE;
    rRDMCHCR.bit.DE = DRV_DMAC_RDMCHCR_DE_ENABLE;
    rDmacWriteReg32(dev, ch, RDMCHCR, rRDMCHCR.INT);
}

/**
 *  @brief          Clear RDMCHCR_DE
 *  @details
 *  @param[in]      dev     : dev number
 *  @param[in]      ch      : Sub Ch number
 *  @return         drv_OK
 *  @par    Refer   (none)
 *  @par    Modify  (none)
 */
uint16_t rDmacClear_RDMCHCR_DE(uint8_t dev, uint8_t ch)
{
    uint8_t Retry;
    uint32_t Value;
    uint16_t ret;
    regRDMCHCR_t rRDMCHCR;

    Retry = DRV_DMAC_CLR_DE_RETRY;
    while (1)
    {
        rRDMCHCR.INT = rDmacReadReg32(dev, ch, RDMCHCR);
        rRDMCHCR.bit.DE = DRV_DMAC_RDMCHCR_DE_DISABLE;
        rDmacWriteReg32(dev, ch, RDMCHCR, rRDMCHCR.INT);

        Value = rDmacGet_RDMCHCR(dev, ch);
        if (0 == (Value & DRV_DMAC_REG_RDMCHCR_DE))
        {
            ret = drv_OK;
            break;
        }
        if (--Retry == 0)
        {
            ret = drv_FAIL;
            break;
        }
    }
    return ret;
}

/**
 *  @brief          Clear RDMCHCR_CAE
 *  @details
 *  @param[in]      dev     : dev number
 *  @param[in]      ch      : Sub Ch number
 *  @return         void
 *  @par    Refer   (none)
 *  @par    Modify  (none)
 */
void rDmacClear_RDMCHCR_CAE(uint8_t dev, uint8_t ch)
{
    regRDMCHCR_t rRDMCHCR;

    rRDMCHCR.INT = rDmacReadReg32(dev, ch, RDMCHCR);
    rRDMCHCR.bit.CAE = DRV_DMAC_RDMCHCR_CAE_CLEAR;
    rDmacWriteReg32(dev, ch, RDMCHCR, rRDMCHCR.INT);
}

/**
 *  @brief          Clear RDMCHCR_DSE
 *  @details
 *  @param[in]      dev     : dev number
 *  @param[in]      ch      : Sub Ch number
 *  @return         void
 *  @par    Refer   (none)
 *  @par    Modify  (none)
 */
void rDmacClear_RDMCHCR_DSE(uint8_t dev, uint8_t ch)
{
    regRDMCHCR_t rRDMCHCR;

    rRDMCHCR.INT = rDmacReadReg32(dev, ch, RDMCHCR);
    rRDMCHCR.bit.DSE = DRV_DMAC_RDMCHCR_DSE_CLEAR;
    rDmacWriteReg32(dev, ch, RDMCHCR, rRDMCHCR.INT);
}

/**
 *  @brief          Clear RDMCHCR_TE
 *  @details
 *  @param[in]      dev     : dev number
 *  @param[in]      ch      : Sub Ch number
 *  @return         void
 *  @par    Refer   (none)
 *  @par    Modify  (none)
 */
void rDmacClear_RDMCHCR_TE(uint8_t dev, uint8_t ch)
{
    regRDMCHCR_t rRDMCHCR;

    rRDMCHCR.INT = rDmacReadReg32(dev, ch, RDMCHCR);
    rRDMCHCR.bit.TE = DRV_DMAC_RDMCHCR_TE_CLEAR;
    rDmacWriteReg32(dev, ch, RDMCHCR, rRDMCHCR.INT);
}

/**
 *  @brief          Update RDMCHCRB
 *  @details
 *  @param[in]      dev     : dev number
 *  @param[in]      ch      : Sub Ch number
 *  @param[in]      cfg     : dmac config
 *  @param[in]      descCfg : desc config
 *  @return         void
 *  @par    Refer   (none)
 *  @par    Modify  (none)
 */

void rDmacUpdate_RDMCHCRB(uint8_t dev, uint8_t ch, rDmacCfg_t *cfg, rDmacDescCfg_t *descCfg)
{
    regRDMCHCRB_t rRDMCHCRB;

    rRDMCHCRB.INT = rDmacReadReg32(dev, ch, RDMCHCRB);
    rRDMCHCRB.bit.SLM = cfg->mLowSpeed;
    rRDMCHCRB.bit.PRI = cfg->mPrioLevel;
    if (DRV_DMAC_DMA_NO_DESCRIPTOR != cfg->mDMAMode) {
        rRDMCHCRB.bit.DCNT = descCfg->mDescCount;
    }
    rDmacWriteReg32(dev, ch, RDMCHCRB, rRDMCHCRB.INT);
}

/**
 *  @brief          Update RDMCHCRB_RST
 *  @details
 *  @param[in]      dev     : dev number
 *  @param[in]      ch      : Sub Ch number
 *  @return         void
 *  @par    Refer   (none)
 *  @par    Modify  (none)
 */

void rDmacUpdate_RDMCHCRB_RST(uint8_t dev, uint8_t ch)
{
    regRDMCHCRB_t rRDMCHCRB;

    /* Update RDMCHCRB.DRST */
    rRDMCHCRB.INT = rDmacReadReg32(dev, ch, RDMCHCRB);
    rRDMCHCRB.bit.DRST = 1;
    rDmacWriteReg32(dev, ch, RDMCHCRB, rRDMCHCRB.INT);
}

/**
 *  @brief          Update RDMRS.ID
 *  @details
 *  @param[in]      dev     : dev number
 *  @param[in]      ch      : Sub Ch number
 *  @param[in]      Value   : write data
 *  @return         void
 *  @par    Refer   (none)
 *  @par    Modify  (none)
 */
void rDmacUpdate_RDMRS_ID(uint8_t dev, uint8_t ch, uint8_t Value)
{
    rDmacWriteReg16(dev, ch, RDMRS, Value);
}

/**
 *  @brief          Update RDMDPBASE
 *  @details
 *  @param[in]      dev     : dev number
 *  @param[in]      ch      : Sub Ch number
 *  @param[in]      Value   : write data
 *  @return         void
 *  @par    Refer   (none)
 *  @par    Modify  (none)
 */
void rDmacUpdate_RDMDPBASE(uint8_t dev, uint8_t ch, uint64_t Value)
{
    rDmacWriteReg32(dev, ch, RDMDPBASE, (uint32_t)(Value & DRV_DMAC_REG_MASK_LOWER_RDMDPBASE));
    rDmacWriteReg32(dev, ch, RDMFIXDPBASE, ((Value >> 32) & DRV_DMAC_REG_MASK_UPPER_RDMDPBASE));
}

/**
 *  @brief          Update RDMDPCR
 *  @details
 *  @param[in]      dev     : dev number
 *  @param[in]      ch      : Sub Ch number
 *  @param[in]      Value   : write data
 *  @return         void
 *  @par    Refer   (none)
 *  @par    Modify  (none)
 */
void rDmacUpdate_RDMDPCR(uint8_t dev, uint8_t ch, uint8_t Value)
{
    regRDMDPCR_t rRDMDPCR;

    rRDMDPCR.INT = rDmacReadReg32(dev, ch, RDMDPCR);
    rRDMDPCR.bit.DIPT = Value;
    rDmacWriteReg32(dev, ch, RDMDPCR, rRDMDPCR.INT);
}

