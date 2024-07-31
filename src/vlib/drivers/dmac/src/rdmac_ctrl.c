/*************************************************************************************************************
* dmac_ctrl_c
* Copyright (c) [2017-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "drv.h"
#include "rdmac_common.h"
#include "rdmac_reg.h"
#include "rdmac_int.h"

uint16_t rcarDmacExec(uint8_t dev, uint8_t ch, rDmacCfg_t *cfg, rDmacDescCfg_t *descCfg, uintptr_t arg);
uint16_t rcarDmacStop(uint8_t dev, uint8_t ch);

/*!
 *  @brief          DMA intialize
 *  @details
 *  @param[in]      mode
 *  @return         drv_OK
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
uint16_t rcarDmacCtrlInit(uint8_t dev, rDmacPriorityMode_t mode)
{
    /* Set Priority Mode */
    rDmacUpdate_RDMOR(dev, mode);
    return drv_OK;
}

/*!
 *  @brief          DMA Execute
 *  @details
 *  @param[in]      cfg
 *  @param[in]      descCfg
 *  @param[in]      cb
 *  @return         drv_OK
 *  @return         drv_ERR_NOT_INITIALIZED
 *  @return         drv_RTDMAC_CH_BUSY
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
uint16_t rcarDmacExec(uint8_t dev, uint8_t ch, rDmacCfg_t *cfg, rDmacDescCfg_t *descCfg, uintptr_t arg)
{
    uint32_t Value;
    uint16_t ret = drv_OK;

    /* RTDMAC Initial Check */
    Value = rDmacGet_RDMOR(dev);
    if (0 == (Value & DRV_DMAC_REG_RDMOR_DME))
    {
        ret = drv_ERR_NOT_INITIALIZED;
    }
    else
    {
        /* CH Busy Check */
        Value = rDmacGet_RDMCHCR(dev, ch);
        if (0 != (Value & DRV_DMAC_REG_RDMCHCR_DE))
        {
            ret = drv_DMAC_CH_BUSY;
        }
        else
        {
            /* cfg Set */
            rDmacUpdate_RDMCHCLR(dev, ch);
            rDmacUpdate_RDMSAR(dev, ch, cfg->mSrcAddr);
            rDmacUpdate_RDMDAR(dev, ch, cfg->mDestAddr);
            rDmacUpdate_RDMTCR(dev, ch, cfg->mTransferCount);
            rDmacUpdate_RDMRS_ID(dev, ch, cfg->mResource);

            if (DRV_DMAC_DMA_NO_DESCRIPTOR != (cfg->mDMAMode && descCfg))
            {
                /* descCfg Set */
                rDmacUpdate_RDMCHCRB_RST(dev, ch);
                rDmacUpdate_RDMDPBASE(dev, ch, descCfg->mDescBaseAddr);
            }

            rDmacUpdate_RDMCHCRB(dev, ch, cfg, descCfg);
            if ( DRV_DMAC_DMA_DESC_READOUT == cfg->mDMAMode )
            {
                rDmacUpdate_RDMDPCR(dev, ch, descCfg->mDescIndex);
            }

            /* DMA Start */
            rDmacUpdate_RDMCHCR(dev, ch, cfg, descCfg);
        }
    }
    return ret;
}

/*!
 *  @brief          DMA Stop
 *  @details
 *  @return         drv_OK
 *  @return         drv_ERR_NOT_INITIALIZED
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
uint16_t rcarDmacStop(uint8_t dev, uint8_t ch)
{
    uint32_t Value;
    uint16_t ret = drv_OK;

    /* RTDMAC Initial Check */
    Value = rDmacGet_RDMOR(dev);
    if (0 == (Value & DRV_DMAC_REG_RDMOR_DME))
    {
        ret = drv_ERR_NOT_INITIALIZED;
    }
    else
    {
        /* DMA Stop */
        ret = rDmacClear_RDMCHCR_DE(dev, ch);
        rDmacUpdate_RDMCHCLR(dev, ch);
    }
    return ret;
}
