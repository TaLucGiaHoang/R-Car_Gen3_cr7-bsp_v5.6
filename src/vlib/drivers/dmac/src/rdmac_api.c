/*************************************************************************************************************
* dmac_api
* Copyright (c) [2017-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <stdint.h>
#include <stddef.h>

#include "drv.h"
#include "rdmac_common.h"
#include "rdmac_ctrl.h"
#include "rdmac_reg.h"
#include "rtdmac.h"
#include "audmac.h"
#include "rcar-xos/osal/r_osal.h"

#include "rdmac_int.h"
#include "r_cpg_api.h"
#include "r_delay_api.h"
#include "r_print_api.h"

static uint16_t rDmacCheckInitParam(rDmacPriorityMode_t mode);
static uint16_t rDmacCheckExecParam(uint8_t dev, uint8_t ch, rDmacCfg_t *cfg, rDmacDescCfg_t *descCfg);
static uint16_t rDmacCheckStopParam(uint8_t dev, uint8_t ch);
static uint16_t rDmacCheckChParam(uint8_t dev, uint8_t ch);

extern osal_device_handle_t osal_devhandle_rtdmac0;
extern osal_device_handle_t osal_devhandle_rtdmac1;
extern osal_device_handle_t osal_devhandle_audmac0;
extern osal_device_handle_t osal_devhandle_audmac1;

static void modulePowerOnRTDMAC(R_RTDMAC_Dev_t dev)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    osal_device_handle_t osal_devhandle_dmac = NULL;
    e_osal_pm_required_state_t osal_state;
    e_osal_pm_reset_state_t osal_reset_state;

    if (RCAR_RT_DMAC0 == dev)
    {
        osal_devhandle_dmac = osal_devhandle_rtdmac0;
    }
    else if (RCAR_RT_DMAC1 == dev)
    {
        osal_devhandle_dmac = osal_devhandle_rtdmac1;
    }
    else
    {
        R_PRINT_Log("[modulePowerOnd]: Wrong device (%d)\r\n", dev);
    }
    /* Set RT-DMAC enable */
    osal_ret = R_OSAL_PmSetRequiredState(osal_devhandle_dmac, OSAL_PM_REQUIRED_STATE_REQUIRED, false);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[modulePowerOnd]: R_OSAL_PmSetRequiredState failed(%d), OSAL_PM_REQUIRED_STATE_REQUIRED\r\n", osal_ret);
    }
    else
    {
        /* Wait RT-DMAC enabled */
        do
        {
            R_OSAL_ThreadSleepForTimePeriod(1);
            osal_ret = R_OSAL_PmGetRequiredState(osal_devhandle_dmac, &osal_state);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[modulePowerOnd]: R_OSAL_PmGetRequiredState failed(%d)\r\n", osal_ret);
            }
        } while (OSAL_PM_REQUIRED_STATE_REQUIRED != osal_state);
    }

    /* Reset RT-DMAC */
    osal_ret = R_OSAL_PmSetResetState(osal_devhandle_dmac, OSAL_PM_RESET_STATE_APPLIED);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[modulePowerOnd]: R_OSAL_PmSetResetState failed(%d), OSAL_PM_RESET_STATE_APPLIED\r\n", osal_ret);
    }
    else
    {
        /* Wait RT-DMAC reset start */
        do
        {
            R_OSAL_ThreadSleepForTimePeriod(1);
            osal_ret = R_OSAL_PmGetResetState(osal_devhandle_dmac, &osal_reset_state);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[modulePowerOnd]: R_OSAL_PmGetResetState failed(%d), OSAL_PM_RESET_STATE_APPLIED\r\n", osal_ret);
            }
        } while (OSAL_PM_RESET_STATE_APPLIED != osal_reset_state);
    }
    delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
    osal_ret = R_OSAL_PmSetResetState(osal_devhandle_dmac, OSAL_PM_RESET_STATE_RELEASED);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[modulePowerOnd]: R_OSAL_PmSetResetState failed(%d), OSAL_PM_RESET_STATE_RELEASED\r\n", osal_ret);
    }
    else
    {
        /* Wait RT-DMAC reset end */
        do
        {
            R_OSAL_ThreadSleepForTimePeriod(1);
            osal_ret = R_OSAL_PmGetResetState(osal_devhandle_dmac, &osal_reset_state);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[modulePowerOnd]: R_OSAL_PmGetResetState failed(%d), OSAL_PM_RESET_STATE_RELEASED\r\n", osal_ret);
            }
        } while (OSAL_PM_RESET_STATE_RELEASED != osal_reset_state);
    }
}

static void modulePowerOnAUDMAC(R_AUDMAC_Dev_t dev)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    osal_device_handle_t osal_devhandle_dmac = NULL;
    e_osal_pm_required_state_t osal_state;
    e_osal_pm_reset_state_t osal_reset_state;

    if (RCAR_RT_DMAC0 == dev)
    {
        osal_devhandle_dmac = osal_devhandle_audmac0;
    }
    else if (RCAR_RT_DMAC1 == dev)
    {
        osal_devhandle_dmac = osal_devhandle_audmac1;
    }
    else
    {
        R_PRINT_Log("[modulePowerOnd]: Wrong device (%d)\r\n", dev);
    }
    /* Set RT-DMAC enable */
    osal_ret = R_OSAL_PmSetRequiredState(osal_devhandle_dmac, OSAL_PM_REQUIRED_STATE_REQUIRED, false);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[modulePowerOnd]: R_OSAL_PmSetRequiredState failed(%d), OSAL_PM_REQUIRED_STATE_REQUIRED\r\n", osal_ret);
    }
    else
    {
        /* Wait RT-DMAC enabled */
        do
        {
            R_OSAL_ThreadSleepForTimePeriod(1);
            osal_ret = R_OSAL_PmGetRequiredState(osal_devhandle_dmac, &osal_state);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[modulePowerOnd]: R_OSAL_PmGetRequiredState failed(%d)\r\n", osal_ret);
            }
        } while (OSAL_PM_REQUIRED_STATE_REQUIRED != osal_state);
    }

    /* Reset RT-DMAC */
    osal_ret = R_OSAL_PmSetResetState(osal_devhandle_dmac, OSAL_PM_RESET_STATE_APPLIED);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[modulePowerOnd]: R_OSAL_PmSetResetState failed(%d), OSAL_PM_RESET_STATE_APPLIED\r\n", osal_ret);
    }
    else
    {
        /* Wait RT-DMAC reset start */
        do
        {
            R_OSAL_ThreadSleepForTimePeriod(1);
            osal_ret = R_OSAL_PmGetResetState(osal_devhandle_dmac, &osal_reset_state);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[modulePowerOnd]: R_OSAL_PmGetResetState failed(%d), OSAL_PM_RESET_STATE_APPLIED\r\n", osal_ret);
            }
        } while (OSAL_PM_RESET_STATE_APPLIED != osal_reset_state);
    }
    delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
    osal_ret = R_OSAL_PmSetResetState(osal_devhandle_dmac, OSAL_PM_RESET_STATE_RELEASED);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[modulePowerOnd]: R_OSAL_PmSetResetState failed(%d), OSAL_PM_RESET_STATE_RELEASED\r\n", osal_ret);
    }
    else
    {
        /* Wait RT-DMAC reset end */
        do
        {
            R_OSAL_ThreadSleepForTimePeriod(1);
            osal_ret = R_OSAL_PmGetResetState(osal_devhandle_dmac, &osal_reset_state);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[modulePowerOnd]: R_OSAL_PmGetResetState failed(%d), OSAL_PM_RESET_STATE_RELEASED\r\n", osal_ret);
            }
        } while (OSAL_PM_RESET_STATE_RELEASED != osal_reset_state);
    }
}

static void modulePowerOffRTDMAC(R_RTDMAC_Dev_t dev)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    osal_device_handle_t osal_devhandle_dmac = NULL;
    e_osal_pm_required_state_t osal_state;
    e_osal_pm_reset_state_t osal_reset_state;

    if (RCAR_RT_DMAC0 == dev)
    {
        osal_devhandle_dmac = osal_devhandle_rtdmac0;
    }
    else if (RCAR_RT_DMAC1 == dev)
    {
        osal_devhandle_dmac = osal_devhandle_rtdmac1;
    }
    else
    {
        R_PRINT_Log("[modulePowerOffd]: Wrong device (%d)\r\n", dev);
    }

    /* Reset RT-DMAC */
    osal_ret = R_OSAL_PmSetResetState(osal_devhandle_dmac, OSAL_PM_RESET_STATE_APPLIED);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[modulePowerOffd]: R_OSAL_PmSetResetState failed(%d), OSAL_PM_RESET_STATE_APPLIED\r\n", osal_ret);
    }
    else
    {
        /* Wait RT-DMAC reset start */
        do
        {
            R_OSAL_ThreadSleepForTimePeriod(1);
            osal_ret = R_OSAL_PmGetResetState(osal_devhandle_dmac, &osal_reset_state);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[modulePowerOffd]: R_OSAL_PmGetResetState failed(%d), OSAL_PM_RESET_STATE_APPLIED\r\n", osal_ret);
            }
        } while (OSAL_PM_RESET_STATE_APPLIED != osal_reset_state);
    }
    delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
    osal_ret = R_OSAL_PmSetResetState(osal_devhandle_dmac, OSAL_PM_RESET_STATE_RELEASED);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[modulePowerOffd]: R_OSAL_PmSetResetState failed(%d), OSAL_PM_RESET_STATE_RELEASED\r\n", osal_ret);
    }
    else
    {
        /* Wait RT-DMAC reset end */
        do
        {
            R_OSAL_ThreadSleepForTimePeriod(1);
            osal_ret = R_OSAL_PmGetResetState(osal_devhandle_dmac, &osal_reset_state);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[modulePowerOffd]: R_OSAL_PmGetResetState failed(%d), OSAL_PM_RESET_STATE_RELEASED\r\n", osal_ret);
            }
        } while (OSAL_PM_RESET_STATE_RELEASED != osal_reset_state);
    }

    /* Set RT-DMAC disable */
    osal_ret = R_OSAL_PmSetRequiredState(osal_devhandle_dmac, OSAL_PM_REQUIRED_STATE_RELEASED, false);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[modulePowerOffd]: R_OSAL_PmSetRequiredState failed(%d), OSAL_PM_REQUIRED_STATE_RELEASED\r\n", osal_ret);
    }
    else
    {
        /* Wait RT-DMAC disabled */
        do
        {
            R_OSAL_ThreadSleepForTimePeriod(1);
            osal_ret = R_OSAL_PmGetRequiredState(osal_devhandle_dmac, &osal_state);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[modulePowerOffd]: R_OSAL_PmGetRequiredState failed(%d)\r\n", osal_ret);
            }
        } while (OSAL_PM_REQUIRED_STATE_RELEASED != osal_state);
    }
}

static void modulePowerOffAUDMAC(R_AUDMAC_Dev_t dev)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    osal_device_handle_t osal_devhandle_dmac = NULL;
    e_osal_pm_required_state_t osal_state;
    e_osal_pm_reset_state_t osal_reset_state;

    if (RCAR_AUDIO_DMAC0 == dev)
    {
        osal_devhandle_dmac = osal_devhandle_audmac0;
    }
    else if (RCAR_AUDIO_DMAC1 == dev)
    {
        osal_devhandle_dmac = osal_devhandle_audmac1;
    }
    else
    {
        R_PRINT_Log("[modulePowerOffd]: Wrong device (%d)\r\n", dev);
    }

    /* Reset RT-DMAC */
    osal_ret = R_OSAL_PmSetResetState(osal_devhandle_dmac, OSAL_PM_RESET_STATE_APPLIED);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[modulePowerOffd]: R_OSAL_PmSetResetState failed(%d), OSAL_PM_RESET_STATE_APPLIED\r\n", osal_ret);
    }
    else
    {
        /* Wait RT-DMAC reset start */
        do
        {
            R_OSAL_ThreadSleepForTimePeriod(1);
            osal_ret = R_OSAL_PmGetResetState(osal_devhandle_dmac, &osal_reset_state);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[modulePowerOffd]: R_OSAL_PmGetResetState failed(%d), OSAL_PM_RESET_STATE_APPLIED\r\n", osal_ret);
            }
        } while (OSAL_PM_RESET_STATE_APPLIED != osal_reset_state);
    }
    delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
    osal_ret = R_OSAL_PmSetResetState(osal_devhandle_dmac, OSAL_PM_RESET_STATE_RELEASED);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[modulePowerOffd]: R_OSAL_PmSetResetState failed(%d), OSAL_PM_RESET_STATE_RELEASED\r\n", osal_ret);
    }
    else
    {
        /* Wait RT-DMAC reset end */
        do
        {
            R_OSAL_ThreadSleepForTimePeriod(1);
            osal_ret = R_OSAL_PmGetResetState(osal_devhandle_dmac, &osal_reset_state);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[modulePowerOffd]: R_OSAL_PmGetResetState failed(%d), OSAL_PM_RESET_STATE_RELEASED\r\n", osal_ret);
            }
        } while (OSAL_PM_RESET_STATE_RELEASED != osal_reset_state);
    }

    /* Set RT-DMAC disable */
    osal_ret = R_OSAL_PmSetRequiredState(osal_devhandle_dmac, OSAL_PM_REQUIRED_STATE_RELEASED, false);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[modulePowerOffd]: R_OSAL_PmSetRequiredState failed(%d), OSAL_PM_REQUIRED_STATE_RELEASED\r\n", osal_ret);
    }
    else
    {
        /* Wait RT-DMAC disabled */
        do
        {
            R_OSAL_ThreadSleepForTimePeriod(1);
            osal_ret = R_OSAL_PmGetRequiredState(osal_devhandle_dmac, &osal_state);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[modulePowerOffd]: R_OSAL_PmGetRequiredState failed(%d)\r\n", osal_ret);
            }
        } while (OSAL_PM_REQUIRED_STATE_RELEASED != osal_state);
    }
}

/*!
 *  @brief          DMA intialize
 *  @details
 *  @param[in]      mode
 *  @return         drv_OK
 *  @return         drv_ERR_PARAM
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
static uint16_t __rcarDmacCtrlInit(uint8_t dev, rDmacPriorityMode_t mode)
{
    uint16_t  ret;

    /* Parameter Check */
    ret = rDmacCheckInitParam(mode);
    if (drv_OK == ret)
    {
        ret = rcarDmacCtrlInit(dev, mode);
    }

    return ret;
}

uint16_t R_AUDMAC_Init(R_AUDMAC_Dev_t dev, rDmacPriorityMode_t mode)
{
    modulePowerOnAUDMAC(dev);
	return __rcarDmacCtrlInit(dev + DRV_AUDMAC_REG0, mode);
}

uint16_t R_RTDMAC_Init(R_RTDMAC_Dev_t dev, rDmacPriorityMode_t mode)
{
    modulePowerOnRTDMAC(dev);
    return __rcarDmacCtrlInit((dev + DRV_RTDMAC_REG0), mode);
}

void R_AUDMAC_Release(R_AUDMAC_Dev_t dev)
{
	modulePowerOffAUDMAC(dev);
}

void R_RTDMAC_Release(R_RTDMAC_Dev_t dev)
{
    modulePowerOffRTDMAC(dev);
}




/*!
 *  @brief          DMA Execute
 *  @details
 *  @param[in]      chNo
 *  @param[in]      cfg
 *  @param[in]      descCfg
 *  @return         drv_OK
 *  @return         DRV_DMAC_CH_INVALID
 *  @return         drv_ERR_PARAM
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/

static uint16_t __rcarDmacExec(uint8_t dev, uint8_t chNo, rDmacCfg_t *cfg,
             rDmacDescCfg_t *descCfg, uintptr_t arg)
{
    uint16_t  ret;

    /* Parameter Check */
    ret = rDmacCheckExecParam(dev, chNo, cfg, descCfg);
    if (drv_OK == ret)
    {
        /* Control Process */
        ret = rcarDmacExec(dev, chNo, cfg, descCfg, arg);
    }
    return ret;
}

uint16_t R_RTDMAC_Exec(R_RTDMAC_Dev_t dev, uint8_t chNo, rDmacCfg_t *cfg,
             rDmacDescCfg_t *descCfg, uintptr_t arg)
{
    dev += DRV_RTDMAC_REG0;
    return __rcarDmacExec(dev, chNo, cfg, descCfg, arg);
}

uint16_t R_AUDMAC_Exec(R_AUDMAC_Dev_t dev, uint8_t chNo, rDmacCfg_t *cfg,
			   rDmacDescCfg_t *descCfg, uintptr_t arg)
{
    dev += DRV_AUDMAC_REG0;
    return __rcarDmacExec(dev, chNo, cfg, descCfg, arg);
}

/*!
 *  @brief          DMA Stop
 *  @details
 *  @param[in]      chNo
 *  @return         drv_OK
 *  @return         DRV_DMAC_CH_INVALID
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
static uint16_t __rcarDmacStop(uint8_t dev, uint8_t chNo)
{
    uint16_t  ret;

    /* Parameter Check */
    ret = rDmacCheckStopParam(dev, chNo);
    if (drv_OK == ret)
    {
        /* Control Process */
        ret = rcarDmacStop(dev, chNo);
    }
    return ret;
}

uint16_t R_AUDMAC_Stop(R_AUDMAC_Dev_t dev, uint8_t chNo)
{
	dev += DRV_AUDMAC_REG0;
	return __rcarDmacStop(dev, chNo);
}

uint16_t R_RTDMAC_Stop(R_RTDMAC_Dev_t dev, uint8_t chNo)
{
    dev += DRV_RTDMAC_REG0;
    return __rcarDmacStop(dev, chNo);
}
/*!
 *  @brief          CheckInitParam
 *  @details
 *  @param[in]      mode
 *  @return         drv_OK
 *  @return         drv_ERR_PARAM
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
static uint16_t rDmacCheckInitParam(rDmacPriorityMode_t mode)
{
    if (DRV_DMAC_PRIO_MAX <= mode)
    {
        return drv_ERR_PARAM;
    }
    return drv_OK;
}

/*!
 *  @brief          CheckExecParam
 *  @details
 *  @param[in]      chNo
 *  @param[in]      cfg
 *  @param[in]      descCfg
 *  @return         drv_OK
 *  @return         DRV_DMAC_CH_INVALID
 *  @return         drv_ERR_PARAM
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
static uint16_t rDmacCheckExecParam(uint8_t dev, uint8_t ch, rDmacCfg_t *cfg, rDmacDescCfg_t *descCfg)
{
    uint16_t ret;
    uintptr_t Addr;
    uint32_t Count;

    ret = rDmacCheckChParam(dev, ch);
    if (drv_OK != ret)
    {
        ret = drv_ERR_PARAM;
    }
    else if (0 != (cfg->mSrcAddr & ~DRV_DMAC_REG_MASK_ADDR))
    {
        ret = drv_ERR_PARAM;
    }
    else if (0 != (cfg->mDestAddr & ~DRV_DMAC_REG_MASK_ADDR))
    {
        ret = drv_ERR_PARAM;
    }
    else
    {
        if (DRV_DMAC_DMA_NO_DESCRIPTOR == cfg->mDMAMode)
        {
            if ((0 == cfg->mSrcAddr) || (0 == cfg->mDestAddr))
            {
                ret = drv_ERR_PARAM;
            }
        }
    }

    if (drv_OK == ret)
    {
        if (0 != (cfg->mTransferCount & ~DRV_DMAC_REG_MASK_COUNT))
        {
            ret = drv_ERR_PARAM;
        }
        else if (DRV_DMAC_DMA_MODE_MAX <= cfg->mDMAMode)
        {
            ret = drv_ERR_PARAM;
        }
        else if (DRV_DMAC_ADDR_MAX <= cfg->mSrcAddrMode)
        {
            ret = drv_ERR_PARAM;
        }
        else if (DRV_DMAC_ADDR_MAX <= cfg->mDestAddrMode)
        {
            ret = drv_ERR_PARAM;
        }
        else if (DRV_DMAC_TRANS_UNIT_MAX <= cfg->mTransferUnit)
        {
            ret = drv_ERR_PARAM;
        }
        else if (DRV_DMAC_SPEED_MAX <= cfg->mLowSpeed)
        {
            ret = drv_ERR_PARAM;
        }
        else if (DRV_DMAC_PRIORITY_MAX <= cfg->mPrioLevel)
        {
            ret = drv_ERR_PARAM;
        }
        else
        {
            if (DRV_DMAC_DMA_NO_DESCRIPTOR != cfg->mDMAMode)
            {
                if (!descCfg)
                {
                    ret = drv_ERR_PARAM;
                }
                else if ((0 != (descCfg->mDescBaseAddr & ~DRV_DMAC_REG_MASK_ADDR)) || (0 == descCfg->mDescBaseAddr))
                {
                    ret = drv_ERR_PARAM;
                }
                else
                {
                    Addr = descCfg->mDescBaseAddr & DRV_DMAC_REG_MASK_CHECK_ADDR;
                    if (Addr == DRV_RTDMAC_DSMEM_ADDR_RTDMAC1)
                    {
                        Count = DRV_DMAC_INT1_NUM_DESC;
                    }
                    else if (Addr == DRV_RTDMAC_DSMEM_ADDR_RTDMAC2)
                    {
                        Count = DRV_DMAC_INT2_NUM_DESC;
                    }
                    else
                    {
                        Count = DRV_DMAC_EXT_NUM_DESC;
                    }
                    if (descCfg->mDescCount >= Count)
                    {
                        ret = drv_ERR_PARAM;
                    }
                    else if (descCfg->mDescIndex > Count)
                    {
                        ret = drv_ERR_PARAM;
                    }
                    else
                    {
                        /* do nothing */
                    }
                }
            }
        }
    }
    return ret;
}

/*!
 *  @brief          CheckStopParam
 *  @details
 *  @param[in]      chNo
 *  @return         drv_OK
 *  @return         DRV_DMAC_CH_INVALID
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
static uint16_t rDmacCheckStopParam(uint8_t dev, uint8_t ch)
{
    return rDmacCheckChParam(dev, ch);
}

/*!
 *  @brief          CheckChParam
 *  @details
 *  @param[in]      chNo
 *  @return         drv_OK
 *  @return         DRV_DMAC_CH_INVALID
 *  @par    Refer   (none)
 *  @par    Modify  (none)
*/
static uint16_t rDmacCheckChParam(uint8_t dev, uint8_t ch)
{
    if ( (dev >= DRV_DMAC_UNIT_NUM) || (ch >= DRV_DMAC_MAX_CH) )
    {
        return drv_DMAC_CH_INVALID;
    }
    return drv_OK;
}
