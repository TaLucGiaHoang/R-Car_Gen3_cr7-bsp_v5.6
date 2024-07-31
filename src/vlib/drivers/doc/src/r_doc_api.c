/*************************************************************************************************************
* doc_api_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/*
 * Section: Includes
 */
#include "rcar-xos/osal/r_osal.h"
#include "r_doc_api.h"
#include "r_doc_regs.h"
#include "r_cpg_api.h"
#include "r_print_api.h"

/*
 * Section: Local Defines
 */
#define LOC_MUTEX_DRIVER_DOC    (0x8000U + 96U) /* OSAL_MUTEX_DRIVER_DOC */
#define LOC_DOC_MUTEX_TIMEOUT   (3000U)
#define LOC_DOC_MAX_INSTANCE_NUM (2)

/**
 * DOC IRQ Number for xOS OSAL
 */
#define LOC_DOC_DOC_IRQCHANNEL (0)
#define LOC_DOC_ACT_IRQCHANNEL (1)

/*
 * Section: Global parameter
 */
static uint32_t loc_doc_Status[2] = {0, 0};
static uint32_t loc_doc_Active[4] = {0, 0, 0, 0};

static uint32_t loc_docirq_error[2] = {0, 0};
static uint32_t loc_actirq_error[2] = {0, 0};

static r_doc_Unit_t err_DocUnit = 0xFF;
static r_doc_DocInterruptPhase_t err_DocPhase[2] = {R_DOC_INT_PHASE_INIT, R_DOC_INT_PHASE_INIT};
static uint32_t enable_list = 0;
static osal_mutex_handle_t DocMutex;
extern osal_device_handle_t DocDevHandle[LOC_DOC_MAX_INSTANCE_NUM];

/*
 * Section: local Functions
 */
static r_doc_Error_t loc_DOC_CheckVideoChannel(r_doc_VideoChannel_t doc_ch);
static void loc_DOC_ReadModWriteRegSet(r_doc_Unit_t doc_unit, uintptr_t Address, uint32_t Mask);
static void loc_DOC_ReadModWriteRegClear(r_doc_Unit_t doc_unit, uintptr_t Address, uint32_t Mask);

static r_doc_Error_t loc_DOC_CheckVideoChannel(r_doc_VideoChannel_t doc_ch)
{
    r_doc_Error_t ret = R_DOC_ERR_SUCCESS;

    if ((doc_ch != R_DOC_VCH0) && (doc_ch != R_DOC_VCH1))
    {
        ret = R_DOC_ERR_INVALID_PARAMETER;
        R_PRINT_Log("loc_DOC_CheckVideoChannel video channel error(%d)\n", ret);
    }
    return ret;
}

static void loc_DOC_ReadModWriteRegSet(r_doc_Unit_t doc_unit, uintptr_t Address, uint32_t Mask)
{
    uint32_t value = 0u;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    bool isISR = false;

    osal_ret = R_OSAL_InterruptIsISRContext(&isISR);
    if ((OSAL_RETURN_OK == osal_ret) && (false == isISR))
    {
        do {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(DocMutex, LOC_DOC_MUTEX_TIMEOUT);
        } while (OSAL_RETURN_TIME == osal_ret);
    }

    if (OSAL_RETURN_OK == osal_ret)
    {
        value = R_DOC_PRV_RegRead(Address);
        value |= Mask;
        R_DOC_PRV_RegWrite(Address, value);

        if (false == isISR)
        {
            osal_ret = R_OSAL_ThsyncMutexUnlock(DocMutex);
            if (OSAL_RETURN_OK == osal_ret)
            {
                if (1 == loc_docirq_error[doc_unit])
                {
                    osal_ret = R_OSAL_InterruptEnableIsr(DocDevHandle[doc_unit], LOC_DOC_DOC_IRQCHANNEL);
                    if (OSAL_RETURN_OK == osal_ret)
                    {
                        loc_docirq_error[doc_unit] = 0;
                    }
                    else
                    {
                        R_PRINT_Log("loc_DOC_ReadModWriteRegSet(DOC) R_OSAL_InterruptEnableIsr Error osal_ret = %d\n", osal_ret);
                    }
                }
                else if (1 == loc_actirq_error[doc_unit])
                {
                    osal_ret = R_OSAL_InterruptEnableIsr(DocDevHandle[doc_unit], LOC_DOC_ACT_IRQCHANNEL);
                    if (OSAL_RETURN_OK == osal_ret)
                    {
                        loc_actirq_error[doc_unit] = 0;
                    }
                    else
                    {
                        R_PRINT_Log("loc_DOC_ReadModWriteRegSet(ACT) R_OSAL_InterruptEnableIsr Error osal_ret = %d\n", osal_ret);
                    }
                }
            }
            else
            {
                R_PRINT_Log("loc_DOC_ReadModWriteRegSet MutexUnlock Error osal_ret = %d\n", osal_ret);
            }
        }
    }
    else
    {
        R_PRINT_Log("loc_DOC_ReadModWriteRegSet MutexLock Error osal_ret = %d\n", osal_ret);
    }
}

static void loc_DOC_ReadModWriteRegClear(r_doc_Unit_t doc_unit, uintptr_t Address, uint32_t Mask)
{
    uint32_t value = 0u;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    bool isISR = false;

    osal_ret = R_OSAL_InterruptIsISRContext(&isISR);
    if ((OSAL_RETURN_OK == osal_ret) && (false == isISR))
    {
        do {
            osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(DocMutex, LOC_DOC_MUTEX_TIMEOUT);
        } while (OSAL_RETURN_TIME == osal_ret);
    }

    if (osal_ret == OSAL_RETURN_OK)
    {
        value = R_DOC_PRV_RegRead(Address);
        value &= ~Mask;
        R_DOC_PRV_RegWrite(Address, value);

        if (false == isISR)
        {
            osal_ret = R_OSAL_ThsyncMutexUnlock(DocMutex);
            if (OSAL_RETURN_OK == osal_ret)
            {
                if (1 == loc_docirq_error[doc_unit])
                {
                    osal_ret = R_OSAL_InterruptEnableIsr(DocDevHandle[doc_unit], LOC_DOC_DOC_IRQCHANNEL);
                    if (OSAL_RETURN_OK == osal_ret)
                    {
                        loc_docirq_error[doc_unit] = 0;
                    }
                    else
                    {
                        R_PRINT_Log("loc_DOC_ReadModWriteRegClear(DOC) R_OSAL_InterruptEnableIsr Error osal_ret = %d\n", osal_ret);
                    }
                }
                else if (1 == loc_actirq_error[doc_unit])
                {
                    osal_ret = R_OSAL_InterruptEnableIsr(DocDevHandle[doc_unit], LOC_DOC_ACT_IRQCHANNEL);
                    if (OSAL_RETURN_OK == osal_ret)
                    {
                        loc_actirq_error[doc_unit] = 0;
                    }
                    else
                    {
                        R_PRINT_Log("loc_DOC_ReadModWriteRegClear(ACT) R_OSAL_InterruptEnableIsr Error osal_ret = %d\n", osal_ret);
                    }
                }
            }
            else
            {
                R_PRINT_Log("loc_DOC_ReadModWriteRegClear MutexUnlock Error osal_ret = %d\n", osal_ret);
            }
        }
    }
    else
    {
        R_PRINT_Log("loc_DOC_ReadModWriteRegClear MutexLock Error osal_ret = %d\n", osal_ret);
    }
}

r_doc_Error_t R_DOC_EnableMonitors(r_doc_Unit_t doc_unit, uint32_t monitors)
{
    r_doc_Error_t ret = R_DOC_ERR_SUCCESS;
    uint32_t doc_reg_base = R_DOC_PRV_GetRegBase(doc_unit);

    if ( (monitors & ~0xFFFF) > 0 )
    {
        ret = R_DOC_ERR_INVALID_PARAMETER;
        R_PRINT_Log("R_DOC_EnableMonitors parameter monitors(0x%08x) error(%d)\n", monitors, ret);
    }
    else
    {
        loc_DOC_ReadModWriteRegSet(doc_unit, (doc_reg_base + DOCnEN), monitors);
    }
    return ret;
}

r_doc_Error_t R_DOC_DisableMonitors(r_doc_Unit_t doc_unit, uint32_t monitors)
{
    r_doc_Error_t ret = R_DOC_ERR_SUCCESS;
    uint32_t doc_reg_base = R_DOC_PRV_GetRegBase(doc_unit);

    if ( (monitors & ~0xFFFF) > 0 )
    {
        ret = R_DOC_ERR_INVALID_PARAMETER;
        R_PRINT_Log("R_DOC_DisableMonitors parameter monitors(0x%08x) error(%d)\n", monitors, ret);
    }
    else
    {
        loc_DOC_ReadModWriteRegClear(doc_unit, (doc_reg_base + DOCnEN), monitors);
    }
    return ret;
}

r_doc_Error_t R_DOC_EnableActMonitor(r_doc_Unit_t doc_unit, bool Enable)
{
    r_doc_Error_t ret = R_DOC_ERR_SUCCESS;
    uint32_t doc_reg_base = R_DOC_PRV_GetRegBase(doc_unit);

    loc_DOC_ReadModWriteRegClear(doc_unit, (doc_reg_base + DOCnEN), (1 << 16));
    loc_DOC_ReadModWriteRegSet(doc_unit, (doc_reg_base + DOCnEN), (uint32_t)(Enable << 16));
    return ret;
}

r_doc_Error_t R_DOC_SetActMonitorTiming(r_doc_Unit_t doc_unit, r_doc_VideoChannel_t doc_ch,
                                        uint32_t upper_time, uint32_t lower_time)
{
    r_doc_Error_t ret = R_DOC_ERR_SUCCESS;
    uint32_t doc_reg_base = R_DOC_PRV_GetRegBase(doc_unit);
    uint32_t reg_val = 0;

    if (R_DOC_ERR_SUCCESS != loc_DOC_CheckVideoChannel(doc_ch))
    {
        ret = R_DOC_ERR_INVALID_PARAMETER;
        R_PRINT_Log("R_DOC_SetActMonitorTiming parameter doc_ch(%d) error(%d)\n", doc_ch, ret);
    }
    else if ((upper_time > 4095) || (lower_time > 4095))
    {
        ret = R_DOC_ERR_INVALID_PARAMETER;
        R_PRINT_Log("R_DOC_SetActMonitorTiming parameter upper_time(%d) lower_time(%d) error(%d)\n",
                upper_time, lower_time, ret);
    }
    else
    {
        reg_val = ((upper_time & 0xFFF) << 16) | (lower_time & 0xFFF);
        R_DOC_PRV_RegWrite( (doc_reg_base + DOCnTIME(doc_ch)), reg_val);
    }
    return ret;
}

r_doc_Error_t R_DOC_Init(r_doc_Unit_t doc_unit, r_doc_VideoChannel_t doc_ch, r_doc_Du_Params_t du_params)
{
    r_doc_Error_t ret = R_DOC_ERR_SUCCESS;
    uint32_t doc_reg_base = R_DOC_PRV_GetRegBase(doc_unit);
    uint32_t reg_val = 0;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    e_osal_pm_required_state_t osal_state;

    if (R_DOC_ERR_SUCCESS != loc_DOC_CheckVideoChannel(doc_ch))
    {
        ret = R_DOC_ERR_INVALID_PARAMETER;
        R_PRINT_Log("R_DOC_Init parameter doc_ch(%d) error(%d)\n", doc_ch, ret);
    }
    else if ((du_params.ScreenWidth > 4095) || (du_params.ScreenHeight > 2047))
    {
        ret = R_DOC_ERR_INVALID_PARAMETER;
        R_PRINT_Log("R_DOC_Init parameter ScreenWidth(%d) ScreenHeight(%d) error(%d)\n",
                du_params.ScreenWidth, du_params.ScreenHeight, ret);
    }
    else
    {
        /* Only for the first time */
        if ((0 == loc_doc_Active[0]) && (0 == loc_doc_Active[1]) &&
            (0 == loc_doc_Active[2]) && (0 == loc_doc_Active[3]))
        {
            osal_ret = R_OSAL_ThsyncMutexCreate((osal_mutex_id_t)LOC_MUTEX_DRIVER_DOC, &DocMutex);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("R_DOC_Init MutexCreate Error osal_ret = %d\n", osal_ret);
                ret = R_DOC_ERR_INVALID_PARAMETER;
            }
        }
        if (R_DOC_ERR_SUCCESS == ret)
        {
            /* Only for the first time for DOC0 or DOC1 */
            if (((0 == loc_doc_Active[0]) && (0 == loc_doc_Active[1]) && (R_DOC_DOC0 == doc_unit)) ||
                ((0 == loc_doc_Active[2]) && (0 == loc_doc_Active[3]) && (R_DOC_DOC1 == doc_unit)))
            {
                /* Enable DOC */
                osal_ret = R_OSAL_PmSetRequiredState(DocDevHandle[doc_unit], OSAL_PM_REQUIRED_STATE_REQUIRED, false);
                if(OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("R_DOC_Init R_OSAL_PmSetRequiredState failed(%d), OSAL_PM_REQUIRED_STATE_REQUIRED, doc_unit=%d\r\n", osal_ret, doc_unit);
                    ret = R_DOC_ERR_INVALID_PARAMETER;
                }
                else
                {
                    /* Wait DOC enabled */
                    do
                    {
                        R_OSAL_ThreadSleepForTimePeriod(1);
                        osal_ret = R_OSAL_PmGetRequiredState(DocDevHandle[doc_unit], &osal_state);
                        if (OSAL_RETURN_OK != osal_ret)
                        {
                            R_PRINT_Log("R_DOC_Init R_OSAL_PmGetRequiredState failed(%d)\r\n", osal_ret);
                            ret = R_DOC_ERR_INVALID_PARAMETER;
                        }
                    } while (OSAL_PM_REQUIRED_STATE_REQUIRED != osal_state);
                }

                if (R_DOC_ERR_SUCCESS == ret)
                {
                    /* software reset of DOC unit */
                    loc_DOC_ReadModWriteRegSet(doc_unit, (doc_reg_base + DOCnCTL), (1 << 29));

                    do {
                        reg_val = R_DOC_PRV_RegRead(doc_reg_base + DOCnCTL) & (1 << 29);
                    } while (0 != reg_val);
                }
                R_DOC_PRV_RegWrite(doc_reg_base + DOCnACT(R_DOC_VCH0), 0);
                R_DOC_PRV_RegWrite(doc_reg_base + DOCnACT(R_DOC_VCH1), 0);
            }
        }

        if (R_DOC_ERR_SUCCESS == ret)
        {
            /* enable video channel for DOC unit */
            reg_val = 0x1;
            R_DOC_PRV_RegWrite( (doc_reg_base + DOCnACT(doc_ch)), reg_val);

            if ((R_DOC_DOC0 == doc_unit) && (R_DOC_VCH0 == doc_ch))
            {
                loc_doc_Active[0] = 1;
            }
            else if ((R_DOC_DOC0 == doc_unit) && (R_DOC_VCH1 == doc_ch))
            {
                loc_doc_Active[1] = 1;
            }
            else if ((R_DOC_DOC1 == doc_unit) && (R_DOC_VCH0 == doc_ch))
            {
                loc_doc_Active[2] = 1;
            }
            else if ((R_DOC_DOC1 == doc_unit) && (R_DOC_VCH1 == doc_ch))
            {
                loc_doc_Active[3] = 1;
            }

            /* specify horizontal and vertical back porch offset of video channel */
            reg_val = (du_params.HbackPorchOffset << 16);
            reg_val |= (du_params.VbackPorchOffset << 0);
            R_DOC_PRV_RegWrite( (doc_reg_base + DOCnOFFS(doc_ch)), reg_val);

            /* specify size of image of video channel */
            reg_val = (du_params.ScreenWidth << 16);
            reg_val |= (du_params.ScreenHeight << 0);
            R_DOC_PRV_RegWrite( (doc_reg_base + DOCnDISP(doc_ch)), reg_val);
        }
    }
    return ret;
}

r_doc_Error_t R_DOC_SetMonitorColorInfo(r_doc_Unit_t doc_unit, uint32_t monitor_num,
                r_doc_Region_t region, uint32_t threshold, r_doc_Ref_Colors_t colors[4], uint8_t *clut)
{
    r_doc_Error_t ret = R_DOC_ERR_SUCCESS;
    uint32_t doc_reg_base = R_DOC_PRV_GetRegBase(doc_unit);
    uint32_t reg_val = 0;

    if ((monitor_num > 15) || (threshold > 0x3FFFF) || (NULL == clut))
    {
        ret = R_DOC_ERR_INVALID_PARAMETER;
        R_PRINT_Log("R_DOC_SetMonitorColorInfo monitor_num(%d) threshold(%d) clut(%p) error(%d)\n",
                monitor_num, threshold, clut, ret);
    }
    else
    {
        /* specify the threshold to be compared against the calculated discriminator */
        reg_val  = threshold;
        R_DOC_PRV_RegWrite( (doc_reg_base + DOCnMmCFG3(monitor_num)), reg_val );

        /* specify the minimum/maximum values of reference color 0 */
        reg_val = (colors[0].red.upper << 24);
        reg_val |= (colors[0].green.upper << 20);
        reg_val |= (colors[0].blue.upper << 16);
        reg_val |= (colors[0].red.lower << 8);
        reg_val |= (colors[0].green.lower << 4);
        reg_val |= (colors[0].blue.lower << 0);
        R_DOC_PRV_RegWrite( (doc_reg_base + DOCnMmCFG4(monitor_num)), reg_val );

        /* specify the minimum/maximum values of reference color 1 */
        reg_val = (colors[1].red.upper << 24);
        reg_val |= (colors[1].green.upper << 20);
        reg_val |= (colors[1].blue.upper << 16);
        reg_val |= (colors[1].red.lower << 8);
        reg_val |= (colors[1].green.lower << 4);
        reg_val |= (colors[1].blue.lower << 0);
        R_DOC_PRV_RegWrite( (doc_reg_base + DOCnMmCFG5(monitor_num)), reg_val );

        /* specify the minimum/maximum values of reference color 2 */
        reg_val = (colors[2].red.upper << 24);
        reg_val |= (colors[2].green.upper << 20);
        reg_val |= (colors[2].blue.upper << 16);
        reg_val |= (colors[2].red.lower << 8);
        reg_val |= (colors[2].green.lower << 4);
        reg_val |= (colors[2].blue.lower << 0);
        R_DOC_PRV_RegWrite( (doc_reg_base + DOCnMmCFG6(monitor_num)), reg_val );

        /* specify the minimum/maximum values of reference color 3 */
        reg_val = (colors[3].red.upper << 24);
        reg_val |= (colors[3].green.upper << 20);
        reg_val |= (colors[3].blue.upper << 16);
        reg_val |= (colors[3].red.lower << 8);
        reg_val |= (colors[3].green.lower << 4);
        reg_val |= (colors[3].blue.lower << 0);
        R_DOC_PRV_RegWrite( (doc_reg_base + DOCnMmCFG7(monitor_num)), reg_val );

        /* define the 2-bpp CLUT2 indices of the reference colors */
        uint32_t loop_num = (region.rect.width * region.rect.height) / 8;
        uint32_t address_idx = 0;
        uint32_t ram_idx = 0;
        int i;

        ram_idx = (monitor_num) * 1024;
        R_DOC_PRV_RegWrite( (doc_reg_base + DOCnMmCFG2(monitor_num)), ram_idx );
        for(i = 0; i < loop_num; i++)
        {
            reg_val   = ( (uint32_t)clut[address_idx]   << 14 );
            reg_val  |= ( (uint32_t)clut[address_idx+1] << 12 );
            reg_val  |= ( (uint32_t)clut[address_idx+2] << 10 );
            reg_val  |= ( (uint32_t)clut[address_idx+3] <<  8 );
            reg_val  |= ( (uint32_t)clut[address_idx+4] <<  6 );
            reg_val  |= ( (uint32_t)clut[address_idx+5] <<  4 );
            reg_val  |= ( (uint32_t)clut[address_idx+6] <<  2 );
            reg_val  |= ( (uint32_t)clut[address_idx+7] <<  0 );
            address_idx = address_idx + 8;

            /* give each monitor an equal slice of the reference RAM */
            if (ram_idx <= 4095)
            {
                R_DOC_PRV_RegWrite( (doc_reg_base + DOCnEXPD0000to4095(ram_idx)), reg_val);
            }
            else
            {
                R_DOC_PRV_RegWrite( (doc_reg_base + DOCnEXPD4096to16383(ram_idx)), reg_val);
            }
            ram_idx++;
        }
    }
    return ret;
}

r_doc_Error_t R_DOC_SetMonitorRegion(r_doc_Unit_t doc_unit, uint32_t monitor_num, r_doc_Region_t region)
{
    r_doc_Error_t ret = R_DOC_ERR_SUCCESS;
    uint32_t doc_reg_base = R_DOC_PRV_GetRegBase(doc_unit);
    uint32_t reg_val = 0;

    if ((monitor_num > 15) || (region.pos.pos_x > 0xFFF) || (region.pos.pos_y > 0x7FF) ||
        (region.rect.width > 0xFF)  || (region.rect.height > 0xFF))
    {
        ret = R_DOC_ERR_INVALID_PARAMETER;
        R_PRINT_Log("R_DOC_SetMonitorRegion monitor_num(%d) pos_x(%d) pos_y(%d) width(%d) height(%d) error(%d)\n",
                monitor_num, region.pos.pos_x, region.pos.pos_y, region.rect.width, region.rect.height, ret);
    }
    else
    {
        /* specify the screen area to be monitored by the Video Output Monitor */
        reg_val  = ( region.pos.pos_x << 16 );
        reg_val |= ( region.pos.pos_y << 0  );
        R_DOC_PRV_RegWrite( (doc_reg_base + DOCnMmCFG0(monitor_num)), reg_val );

        /* specify the area size to be monitored by the Video Output Monitor */
        reg_val  = ( region.rect.width  << 16 );
        reg_val |= ( region.rect.height  << 0  );
        R_DOC_PRV_RegWrite( (doc_reg_base + DOCnMmCFG1(monitor_num)), reg_val );
    }
    return ret;
}

r_doc_Error_t R_DOC_SetMonitorChannel(r_doc_Unit_t doc_unit, r_doc_VideoChannel_t doc_ch, uint32_t monitor_num)
{
    r_doc_Error_t ret = R_DOC_ERR_SUCCESS;
    uint32_t doc_reg_base = R_DOC_PRV_GetRegBase(doc_unit);

    if (R_DOC_ERR_SUCCESS != loc_DOC_CheckVideoChannel(doc_ch))
    {
        ret = R_DOC_ERR_INVALID_PARAMETER;
        R_PRINT_Log("R_DOC_SetMonitorChannel doc_ch(%d) error(%d)\n", doc_ch, ret);
    }
    else if (monitor_num > 15)
    {
        ret = R_DOC_ERR_INVALID_PARAMETER;
        R_PRINT_Log("R_DOC_SetMonitorChannel monitor_num(%d) error(%d)\n", monitor_num, ret);
    }
    else
    {
        if (doc_ch == R_DOC_VCH0)
        {
            loc_DOC_ReadModWriteRegClear(doc_unit, (doc_reg_base + DOCnCH), (1 << monitor_num));
        }
        else /* doc_ch == R_DOC_VCH1 */
        {
            loc_DOC_ReadModWriteRegSet(doc_unit, (doc_reg_base + DOCnCH), (1 << monitor_num));
        }
    }
    return ret;
}

r_doc_Status_t R_DOC_GetStatus(r_doc_Unit_t doc_unit)
{
    r_doc_Status_t status;
    uint32_t doc_reg_base = R_DOC_PRV_GetRegBase(doc_unit);
    uint32_t reg_val = 0;
    int i;

    reg_val = R_DOC_PRV_RegRead(doc_reg_base + DOCnSTR);
    status.activity_monitor = (reg_val & (1 << 24)) >> 24;
    status.parameter_error = (reg_val & (1 << 23)) >> 23;
    status.vom_status = (reg_val & (1 << 22)) >> 22;
    status.current_monitor = (reg_val & 0x3C0000) >> 18;
    status.ch1_error = (reg_val & (1 << 17)) >> 17;
    status.ch0_error = (reg_val & (1 << 16)) >> 16;
    for(i = 0; i < 16; i++)
    {
        status.vom_error[i] = (reg_val & (1 << i)) >> i;
    }

    return status;
}

r_doc_Error_t R_DOC_ClearErrors(r_doc_Unit_t doc_unit, bool parameter_error, uint32_t monitors)
{
    r_doc_Error_t ret = R_DOC_ERR_SUCCESS;
    uint32_t doc_reg_base = R_DOC_PRV_GetRegBase(doc_unit);
    uint32_t reg_val = 0;

    if ((monitors & ~0xFFFF) > 0)
    {
        ret = R_DOC_ERR_INVALID_PARAMETER;
        R_PRINT_Log("R_DOC_ClearErrors monitors(0x%08x) error(%d)\n", monitors, ret);
    }
    else
    {
        /* Clear error flags */
        reg_val = (parameter_error ? (1 << 18) : 0);
        reg_val |= monitors;
        reg_val |= (1 << 16); /* activity monitor, channel 0 */
        reg_val |= (1 << 17); /* activity monitor, channel 1 */
        loc_DOC_ReadModWriteRegSet(doc_unit, (doc_reg_base + DOCnCTL), reg_val);

        /* Release error flags */
        reg_val = R_DOC_PRV_RegRead(doc_reg_base + DOCnCTL);
        if (parameter_error)
        {
            reg_val &= ~(1 << 18);
        }
        reg_val &= ~monitors;

        reg_val &= ~(1 << 16); /* activity monitor, channel 0 */
        reg_val &= ~(1 << 17); /* activity monitor, channel 1 */
        R_DOC_PRV_RegWrite( (doc_reg_base + DOCnCTL), reg_val);
    }
    return ret;
}

r_doc_Error_t R_DOC_EnableIrq(r_doc_Unit_t doc_unit, bool enable)
{
    r_doc_Error_t ret = R_DOC_ERR_SUCCESS;
    uint32_t doc_reg_base = R_DOC_PRV_GetRegBase(doc_unit);
    uint32_t reg_val = (1 << 30);

    if (!enable)
    {
        loc_DOC_ReadModWriteRegSet(doc_unit, (doc_reg_base + DOCnCTL), reg_val);
    }
    else
    {
        loc_DOC_ReadModWriteRegClear(doc_unit, (doc_reg_base + DOCnCTL), reg_val);
    }
    return ret;
}

r_doc_Error_t R_DOC_ClearDocIntStatus(r_doc_Unit_t doc_unit, uint32_t *vc0_status, uint32_t *vc1_status)
{
    r_doc_Error_t ret = R_DOC_ERR_SUCCESS;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    uint32_t reg_val = 0;
    uint32_t du_status = 0u;
    uint32_t doc_reg_base;

    osal_ret = R_OSAL_ThsyncMutexTryLock(DocMutex);
    if (osal_ret == OSAL_RETURN_OK)
    {
        doc_reg_base = R_DOC_PRV_GetRegBase(doc_unit);
        err_DocUnit = doc_unit;
        err_DocPhase[doc_unit] = R_DOC_INT_PHASE_POSTPONE;
        loc_DOC_ReadModWriteRegSet(doc_unit, (doc_reg_base + DOCnCTL), (uint32_t)DOCCTL_VOCANMKINT);

        reg_val = R_DOC_PRV_RegRead(doc_reg_base + DOCnEN);
        enable_list = (reg_val &  DOCCH_VOCANCH_MASK);

        /* 1. Disable all Video Output Monitors by DOCnEN (n = 0, 1).VOCAnENm = 0. */
        loc_DOC_ReadModWriteRegClear(doc_unit, (doc_reg_base + DOCnEN), (uint32_t)DOCEN_VOCANEN_MASK);

        du_status = R_DOC_PRV_RegRead(doc_reg_base + DOCnSTR);

        *vc0_status = du_status;
        *vc1_status = du_status;

        osal_ret = R_OSAL_ThsyncMutexUnlock(DocMutex);
        if (OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("R_DOC_ClearDocIntStatus MutexUnlock Error osal_ret = %d\n", osal_ret);
        }
    }
    else
    {
        osal_ret = R_OSAL_InterruptDisableIsr(DocDevHandle[doc_unit], LOC_DOC_DOC_IRQCHANNEL);
        if (OSAL_RETURN_OK == osal_ret)
        {
            loc_docirq_error[doc_unit] = 1;
        }
        else
        {
            R_PRINT_Log("R_DOC_ClearDocIntStatus R_OSAL_InterruptDisableIsr Error osal_ret = %d\n", osal_ret);
        }
    }
    return ret;
}

r_doc_Error_t R_DOC_ClearDocIntStatusPost(r_doc_Unit_t doc_unit)
{
    r_doc_Error_t ret = R_DOC_ERR_SUCCESS;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    uint32_t reg_val = 0;
    uint32_t du_status = 0u;
    uint32_t doc_reg_base;

    osal_ret = R_OSAL_ThsyncMutexTryLock(DocMutex);
    if (osal_ret == OSAL_RETURN_OK)
    {
        if ((err_DocUnit != 0xFF) && (doc_unit == err_DocUnit))
        {
            doc_reg_base = R_DOC_PRV_GetRegBase(err_DocUnit);

            if (R_DOC_INT_PHASE_POSTPONE == err_DocPhase[doc_unit])
            {
                /* 2. Wait until Video Output Monitors are inactive, i.e. until DOCnSTR (n = 0, 1).VOCAnVOCSTR = 0. */
                reg_val = R_DOC_PRV_RegRead(doc_reg_base + DOCnSTR);
                if ((reg_val & DOCSTR_VOCANVOCSTR) == 0u)
                {
                    err_DocPhase[doc_unit] = R_DOC_INT_PHASE_CLEAR;
                    /* 3. Evaluate the error source via the error flags in the status register DOCnSTR (n = 0, 1). */
                    reg_val = R_DOC_PRV_RegRead(doc_reg_base + DOCnSTR);
                    du_status = reg_val & (DOCSTR_VOCANEM | DOCSTR_VOCANE18);
                    du_status &= DOCSTR_VOCANEM;

                    /*4. Clear all Video Output Monitor error flags by
                      - DOCnCTL (n = 0, 1).VOCAnCLm = 1 for content mismatch errors
                      - DOCnCTL (n = 0, 1).VOCAnCL18 = 1 for parameter error */
                    loc_DOC_ReadModWriteRegSet(doc_unit, (doc_reg_base + DOCnCTL), du_status | DOCCTL_VOCANCL18);

                    /* 5. Release all Video Output Monitor error flags clear by
                      - DOCnCTL (n = 0, 1).VOCAnCLm = 0 for content mismatch errors
                      - DOCnCTL (n = 0, 1).VOCAnCL18 = 0 for parameter error */
                    loc_DOC_ReadModWriteRegClear(doc_unit, (doc_reg_base + DOCnCTL), du_status | DOCCTL_VOCANCL18);
                }
            }

            if (R_DOC_INT_PHASE_CLEAR == err_DocPhase[doc_unit])
            {
                /* 6. Verify that all Video Output Monitor error flags are reset, i.e. that
                  - DOCnSTR (n = 0, 1).VOCAnEm = 0: no content mismatch error
                  - DOCnSTR (n = 0, 1).VOCAnE18 = 0: no parameter error */
                reg_val = R_DOC_PRV_RegRead(doc_reg_base + DOCnSTR);
                if ((reg_val & (DOCSTR_VOCANEM | DOCSTR_VOCANE18)) == 0u)
                {
                    /* 7. Enable required Video Output Monitor m by DOCnEN (n = 0, 1).VOCAnENm = 1. */
                    loc_DOC_ReadModWriteRegSet(doc_unit, (doc_reg_base + DOCnEN), enable_list);

                    reg_val = R_DOC_PRV_RegRead(doc_reg_base + DOCnCTL);
                    if( reg_val  & DOCCTL_VOCANMKINT){
                        R_DOC_PRV_RegWrite( (doc_reg_base + DOCnCTL), (reg_val & ~DOCCTL_VOCANMKINT));
                    }
                    err_DocUnit = 0xFF;
                    err_DocPhase[doc_unit] = R_DOC_INT_PHASE_INIT;
                }
            }
        }

        osal_ret = R_OSAL_ThsyncMutexUnlock(DocMutex);
        if (OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("R_DOC_ClearDocIntStatusPost MutexUnlock Error osal_ret = %d\n", osal_ret);
        }
    }
    return ret;
}

r_doc_Error_t R_DOC_ClearActIntStatus(r_doc_Unit_t doc_unit, uint32_t *status)
{
    r_doc_Error_t ret = R_DOC_ERR_SUCCESS;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    uint32_t doc_reg_base = R_DOC_PRV_GetRegBase(doc_unit);
    uint32_t reg_val = 0;
    uint32_t du_status = 0;

    osal_ret = R_OSAL_ThsyncMutexTryLock(DocMutex);
    if (osal_ret == OSAL_RETURN_OK)
    {
        /* (1) Activity Monitors error flag clearing procedure (HWM2.20)
          1. Disable the Activity Monitors by DOCnEN (n = 0, 1).VOCAnEN16 = 0. */
        loc_DOC_ReadModWriteRegClear(doc_unit, doc_reg_base + DOCnEN, DOCEN_VOCANEN16_ENABLE);
        du_status = R_DOC_PRV_RegRead(doc_reg_base + DOCnSTR);

        /* 2. Wait until Activity Monitors are inactive, i.e. until DOCnSTR (n = 0, 1).VOCAnACTSTR = 0. */
        do {
            reg_val = R_DOC_PRV_RegRead(doc_reg_base + DOCnSTR);
        } while((reg_val & DOCSTR_VOCANACTSTR) != 0);

        /* 3. Evaluate the error source via the error flags in the status register DOCnSTR (n = 0, 1). */
        reg_val = R_DOC_PRV_RegRead(doc_reg_base + DOCnSTR);
        du_status = reg_val & (DOCSTR_VOCANE16 | DOCSTR_VOCANE17);
        loc_doc_Status[doc_unit] &= ~(DOCSTR_VOCANE16 | DOCSTR_VOCANE17);
        loc_doc_Status[doc_unit] |= du_status;

        /* 4. Clear all Activity Monitor error flags by DOCnCTL (n = 0, 1).VOCAnCL[17:16] = 1. */
        loc_DOC_ReadModWriteRegSet(doc_unit, doc_reg_base + DOCnCTL, DOCCTL_VOCANCL17 | DOCCTL_VOCANCL16);

        do {
            /* 5. Release all Activity Monitor error flags clear by DOCnCTL (n = 0, 1).VOCAnCL[17:16] = 0. */
            loc_DOC_ReadModWriteRegClear(doc_unit, doc_reg_base + DOCnCTL, DOCCTL_VOCANCL17 | DOCCTL_VOCANCL16);

            /* 6. Verify that all Activity Monitor error flags are reset, i.e. that DOCnSTR (n = 0, 1).VOCAnE[17:16] = 0.
                  If any of the above error flags are not cleared, return to 5. */
            reg_val = R_DOC_PRV_RegRead(doc_reg_base + DOCnSTR);
        } while((reg_val & (DOCSTR_VOCANE17 | DOCSTR_VOCANE16)) != 0u);

        /* 7. Enable the Activity Monitors by DOCnEN (n = 0, 1).VOCAnEN16 = 1. */
        loc_DOC_ReadModWriteRegSet(doc_unit, doc_reg_base + DOCnEN, DOCEN_VOCANEN16_ENABLE);

        *status = du_status;

        osal_ret = R_OSAL_ThsyncMutexUnlock(DocMutex);
        if (OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("R_DOC_ClearActIntStatus MutexUnlock Error osal_ret = %d\n", osal_ret);
        }
    }
    else
    {
        osal_ret = R_OSAL_InterruptDisableIsr(DocDevHandle[doc_unit], LOC_DOC_ACT_IRQCHANNEL);
        if (OSAL_RETURN_OK == osal_ret)
        {
            loc_actirq_error[doc_unit] = 1;
        }
        else
        {
            R_PRINT_Log("R_DOC_ClearActIntStatus R_OSAL_InterruptDisableIsr Error osal_ret = %d\n", osal_ret);
        }
    }
    return ret;
}

r_doc_Error_t R_DOC_DeInit(r_doc_Unit_t doc_unit, r_doc_VideoChannel_t doc_ch)
{
    r_doc_Error_t ret = R_DOC_ERR_SUCCESS;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    uint32_t doc_reg_base = R_DOC_PRV_GetRegBase(doc_unit);
    uint32_t reg_val = 0;
    e_osal_pm_required_state_t osal_state;

    R_DOC_PRV_RegWrite( (doc_reg_base + DOCnACT(doc_ch)), 0);

    if ((R_DOC_DOC0 == doc_unit) && (R_DOC_VCH0 == doc_ch))
    {
        loc_doc_Active[0] = 0;
    }
    else if ((R_DOC_DOC0 == doc_unit) && (R_DOC_VCH1 == doc_ch))
    {
        loc_doc_Active[1] = 0;
    }
    else if ((R_DOC_DOC1 == doc_unit) && (R_DOC_VCH0 == doc_ch))
    {
        loc_doc_Active[2] = 0;
    }
    else if ((R_DOC_DOC1 == doc_unit) && (R_DOC_VCH1 == doc_ch))
    {
        loc_doc_Active[3] = 0;
    }

    /* Only for the last time for DOC0 or DOC1 */
    if (((0 == loc_doc_Active[0]) && (0 == loc_doc_Active[1]) && (R_DOC_DOC0 == doc_unit)) ||
        ((0 == loc_doc_Active[2]) && (0 == loc_doc_Active[3]) && (R_DOC_DOC1 == doc_unit)))
    {
        /* software reset of DOC unit */
        loc_DOC_ReadModWriteRegSet(doc_unit, doc_reg_base + DOCnCTL, 0x1 << 29);

        do {
            reg_val = R_DOC_PRV_RegRead(doc_reg_base + DOCnCTL) & (0x1 << 29);
        } while (0 != reg_val);

        /* Disable DOC */
        osal_ret = R_OSAL_PmSetRequiredState(DocDevHandle[doc_unit], OSAL_PM_REQUIRED_STATE_RELEASED, false);
        if(OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("R_DOC_DeInit R_OSAL_PmSetRequiredState failed(%d), OSAL_PM_REQUIRED_STATE_RELEASED, doc_unit=%d\r\n", osal_ret, doc_unit);
            ret = R_DOC_ERR_INVALID_PARAMETER;
        }
        else
        {
            /* Wait DOC disabled */
            do
            {
                R_OSAL_ThreadSleepForTimePeriod(1);
                osal_ret = R_OSAL_PmGetRequiredState(DocDevHandle[doc_unit], &osal_state);
                if (OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("R_DOC_DeInit R_OSAL_PmGetRequiredState failed(%d)\r\n", osal_ret);
                    ret = R_DOC_ERR_INVALID_PARAMETER;
                }
            } while (OSAL_PM_REQUIRED_STATE_RELEASED != osal_state);
        }
    }

    /* Only for the last time */
    if ((0 == loc_doc_Active[0]) && (0 == loc_doc_Active[1]) &&
        (0 == loc_doc_Active[2]) && (0 == loc_doc_Active[3]))
    {
        osal_ret = R_OSAL_ThsyncMutexDestroy(DocMutex);
        if (OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("R_DOC_DeInit MutexDestroy Error osal_ret = %d\n", osal_ret);
            ret = R_DOC_ERR_INVALID_PARAMETER;
        }
    }

    return ret;
}
