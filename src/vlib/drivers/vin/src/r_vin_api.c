/*************************************************************************************************************
* vin_api_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2018-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/**
 * Section Includes
 */
#include "rcar-xos/osal/r_osal.h"
#include "r_vin_api.h"
#include "r_vin_regs.h"
#include "r_cpg_api.h"
#include "r_io.h"
#include "r_delay_api.h"
#include "r_print_api.h"

#define R_VIN_SCALED_SIZE_MIN (4u)    /* Minimum size after scale-up/down */
#define R_VIN_SCALED_SIZE_MAX (2048u) /* Maximum size after scale-up/down */

#define SCLE    (1 << 26) /* Enables Scaling by the UDS */

/* Video n Module Status Register bits */
#define VMS_FBS_MASK        (3 << 3)
#define VMS_FBS_SHIFT       3
#define VMS_AV              (1 << 1)
#define VMS_CA              (1 << 0) /* Capture Active */

/* Video n Frame Capture Register bits */
#define VNFC_C_FRAME        (1 << 1)
#define VNFC_S_FRAME        (1 << 0)

/* Video n Main Control Register bits */
#define VMC_DPINE           (1UL << 27) /* Gen3 specific */
#define VMC_SCLE            (1 << 26) /* Gen3 specific */
#define VMC_FOC             (1 << 21)
#define VMC_YCAL            (1 << 19)
#define VMC_INF_YUV8_BT656  (0 << 16)
#define VMC_INF_YUV8_BT601  (1 << 16)
#define VMC_INF_YUV10_BT656 (2 << 16)
#define VMC_INF_YUV10_BT601 (3 << 16)
#define VMC_INF_RAW8        (4 << 16)
#define VMC_INF_YUV16       (5 << 16)
#define VMC_INF_RGB888      (6 << 16)
#define VMC_INF_RGB666      (7 << 16)
#define VMC_INF_MASK        (7 << 16)
#define VMC_VUP             (1 << 10)
#define VMC_IM_MASK         0x18
#define VMC_BPS             (1 << 1)
#define VMC_ME              (1UL << 0)

#define VCSI_IFMD_DES1      (1 << 26) /* CSI20 */
#define VCSI_IFMD_DES0      (1 << 25) /* H3:CSI40/41, M3:CSI40, V3M:CSI40 */

#define VCSI_IFMD_CSI_CHSEL(n)  (n << 0)
#define VCSI_IFMD_SEL_NUMBER    5

/* Video n Interrupt Enable Register bits */
#define VIE_FIE          (1 << 4)
#define VIE_EFE          (1 << 1)
#define VIE_FOE          (1 << 0)
#define VIE_MASK         0x8003001F

/* Video n Interrupt Status Register bits */
#define VINTS_FIS        (1 << 4)
#define VINTS_EFS        (1 << 1)
#define VINTS_FOS        (1 << 0)
#define VINTS_MASK       0x8003001F

/* Video n Data Mode Register bits */
#define VDMR_EXRGB        (1 << 8)
#define VDMR_BPSM         (1 << 4)
#define VDMR_DTMD_YCSEP   (1 << 1)
#define VDMR_DTMD_ARGB    (1 << 0)
#define VDMR_DTMD_YCSEP_YCBCR420    (3 << 0)
#define VDMR_DTMD_YCSEP_YCBCR422    (2 << 0)
#define VDMR_A8BIT        (0xFF << 24)
#define VDMR_ABIT         (1 << 2)

/* Video n Data Mode Register 2 bits */
#define VDMR2_VPS         (1 << 30)
#define VDMR2_HPS         (1 << 29)
#define VDMR2_CES         (1 << 28)
#define VDMR2_DES         (1 << 27)
#define VDMR2_CHS         (1 << 23)
#define VDMR2_FTEV        (1 << 17)
#define VDMR2_VLV(n)      ((n & 0xf) << 12)

/* Video n Passband Register */
#define VUDS_PASS_BWIDTH_BWIDTH_H(n)  ((n & 0x7f) << 16)
#define VUDS_PASS_BWIDTH_BWIDTH_V(n)  ((n & 0x7f) << 0)

/* Video n UDS Output Size Clipping Register */
#define VUDS_CLIP_SIZE_CL_HSIZE(n)  ((n & 0xfff) << 16)
#define VUDS_CLIP_SIZE_CL_VSIZE(n)  ((n & 0xfff) << 0)

/* Video n Scaling Control Register */
#define VUDS_CTRL_AMD        (1 << 30)

/* VIN deice handle */
#define R_VIN_MAX_INSTANCE_NUM            (8)
extern osal_device_handle_t VIN_device_handle[R_VIN_MAX_INSTANCE_NUM];


static uint8_t mask_vin_power = 0x0;
/**
 * Section local functions prototypes
 */
static r_vin_Error_t loc_VinClkEnable(r_vin_Channel_t VinCh);
static r_vin_Error_t loc_VinClkDisable(r_vin_Channel_t VinCh);
static r_vin_Error_t loc_VinReset(r_vin_Channel_t VinCh);
static r_vin_Error_t loc_VinPowerOn(r_vin_Channel_t VinCh);
static r_vin_Error_t loc_VinPowerOff(r_vin_Channel_t VinCh);
static int32_t loc_VinInit(r_vin_Channel_t Ch, r_vin_Config_t *Conf,
                           uintptr_t BufferAddr1, uintptr_t BufferAddr2,
                           uintptr_t BufferAddr3);

static uint32_t R_VIN_PRV_ReadReg32(const uintptr_t Address)
{
    return readl(Address);
}

static void R_VIN_PRV_WriteReg32(uint32_t Address, uint32_t Value)
{
    writel(Value, Address);
}

static uintptr_t R_VIN_PRV_GetRegBase(const r_vin_Channel_t VinCh)
{
    uintptr_t base_address = 0;
    switch (VinCh) {
    case R_VIN0:
        base_address = (uintptr_t)R_VIN0_BASE;
        break;
    case R_VIN1:
        base_address = (uintptr_t)R_VIN1_BASE;
        break;
    case R_VIN2:
        base_address = (uintptr_t)R_VIN2_BASE;
        break;
    case R_VIN3:
        base_address = (uintptr_t)R_VIN3_BASE;
        break;
    case R_VIN4:
        base_address = (uintptr_t)R_VIN4_BASE;
        break;
    case R_VIN5:
        base_address = (uintptr_t)R_VIN5_BASE;
        break;
    case R_VIN6:
        base_address = (uintptr_t)R_VIN6_BASE;
        break;
    case R_VIN7:
        base_address = (uintptr_t)R_VIN7_BASE;
        break;
    default:
        R_PRINT_Log("[R_VIN_PRV_GetRegBase]NG. VinCh:%d\r\n", VinCh);
        break;/* Wrong device unit */
    }
    return base_address;
}


/**
 * Section local functions
 */
static r_vin_Error_t loc_VinClkEnable(r_vin_Channel_t VinCh)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    e_osal_pm_required_state_t osal_state;
    r_vin_Error_t ret = R_VIN_ERR_SUCCESS;

    if (VinCh < R_VIN_LAST)
    {
        /* Enable VIN */
        osal_ret = R_OSAL_PmSetRequiredState(VIN_device_handle[VinCh], OSAL_PM_REQUIRED_STATE_REQUIRED, false);
        if(OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("[loc_VinClkEnable] R_OSAL_PmSetRequiredState failed(%d), OSAL_PM_REQUIRED_STATE_REQUIRED\r\n", osal_ret);
            ret = R_VIN_ERR_FAILED;
        }
        else
        {
            /* Wait VIN enabled */
            do
            {
                R_OSAL_ThreadSleepForTimePeriod(1);
                osal_ret = R_OSAL_PmGetRequiredState(VIN_device_handle[VinCh], &osal_state);
                if (OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[loc_VinClkEnable]: R_OSAL_PmGetRequiredState failed(%d)\r\n", osal_ret);
                    ret = R_VIN_ERR_FAILED;
                    break;
                }
            } while (OSAL_PM_REQUIRED_STATE_REQUIRED != osal_state);
        }
    }
    else /* Wrong device unit */
    {
        R_PRINT_Log("[loc_VinClkEnable]NG. VinCh:%d\r\n", VinCh);
        ret = R_VIN_ERR_INVALID_PARAMETER;
    }

    return ret;
}

static r_vin_Error_t loc_VinClkDisable(r_vin_Channel_t VinCh)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    e_osal_pm_required_state_t osal_state;
    r_vin_Error_t ret = R_VIN_ERR_SUCCESS;

    if (VinCh < R_VIN_LAST)
    {
        /* Disable VIN */
        osal_ret = R_OSAL_PmSetRequiredState(VIN_device_handle[VinCh], OSAL_PM_REQUIRED_STATE_RELEASED, false);
        if(OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("[loc_VinClkDisable] R_OSAL_PmSetRequiredState failed(%d), OSAL_PM_REQUIRED_STATE_RELEASED\r\n", osal_ret);
            ret = R_VIN_ERR_FAILED;
        }
        else
        {
            /* Wait VIN disabled */
            do
            {
                R_OSAL_ThreadSleepForTimePeriod(1);
                osal_ret = R_OSAL_PmGetRequiredState(VIN_device_handle[VinCh], &osal_state);
                if (OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[loc_VinClkDisable]: R_OSAL_PmGetRequiredState failed(%d)\r\n", osal_ret);
                    ret = R_VIN_ERR_FAILED;
                    break;
                }
            } while (OSAL_PM_REQUIRED_STATE_RELEASED != osal_state);
        }
    }
    else /* Wrong device unit */
    {
        R_PRINT_Log("[loc_VinClkDisable] NG. VinCh:%d\r\n", VinCh);
        ret = R_VIN_ERR_INVALID_PARAMETER;
    }

    return ret;
}

static r_vin_Error_t loc_VinReset(r_vin_Channel_t VinCh)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    e_osal_pm_reset_state_t osal_reset_state;
    r_vin_Error_t ret = R_VIN_ERR_SUCCESS;

    if (VinCh < R_VIN_LAST)
    {
        /* VIN module reset */
        osal_ret = R_OSAL_PmSetResetState(VIN_device_handle[VinCh], OSAL_PM_RESET_STATE_APPLIED);
        if (OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("[loc_VinReset]: R_OSAL_PmSetResetState failed(%d), OSAL_PM_RESET_STATE_APPLIED\n", osal_ret);
            ret = R_VIN_ERR_FAILED;
        }
        else
        {
            /* Wait VIN reset start */
            do
            {
                R_OSAL_ThreadSleepForTimePeriod(1);
                osal_ret = R_OSAL_PmGetResetState(VIN_device_handle[VinCh], &osal_reset_state);
                if (OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[loc_VinReset]: R_OSAL_PmGetResetState failed(%d), OSAL_PM_RESET_STATE_APPLIED\r\n", osal_ret);
                }
            } while (OSAL_PM_RESET_STATE_APPLIED != osal_reset_state);
        }

        if (R_VIN_ERR_SUCCESS == ret)
        {
            delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
            osal_ret = R_OSAL_PmSetResetState(VIN_device_handle[VinCh], OSAL_PM_RESET_STATE_RELEASED);
            if (OSAL_RETURN_OK != osal_ret)
            {
                R_PRINT_Log("[loc_VinReset]: R_OSAL_PmSetResetState failed(%d), OSAL_PM_RESET_STATE_RELEASED\n", osal_ret);
                ret = R_VIN_ERR_FAILED;
            }
            else
            {
                /* Wait VIN reset end */
                do
                {
                    R_OSAL_ThreadSleepForTimePeriod(1);
                    osal_ret = R_OSAL_PmGetResetState(VIN_device_handle[VinCh], &osal_reset_state);
                    if (OSAL_RETURN_OK != osal_ret)
                    {
                        R_PRINT_Log("[loc_VinReset]: R_OSAL_PmGetResetState failed(%d), OSAL_PM_RESET_STATE_RELEASED\r\n", osal_ret);
                    }
                } while (OSAL_PM_RESET_STATE_RELEASED != osal_reset_state);
            }
        }
    }
    else /* Wrong device unit */
    {
        R_PRINT_Log("[loc_VinReset]NG. VinCh:%d\r\n", VinCh);
        ret = R_VIN_ERR_INVALID_PARAMETER;
    }

    return ret;
}


static r_vin_Error_t loc_VinPowerOn(r_vin_Channel_t VinCh)
{
    r_vin_Error_t ret = R_VIN_ERR_SUCCESS;

    /* Clk enable & reset */
    ret = loc_VinClkEnable(VinCh);
    ret |= loc_VinReset(VinCh);

    return ret;
}

static r_vin_Error_t loc_VinPowerOff(r_vin_Channel_t VinCh)
{
    r_vin_Error_t ret = R_VIN_ERR_SUCCESS;

    ret = loc_VinReset(VinCh);
    ret |= loc_VinClkDisable(VinCh);

    return ret;
}


static int32_t loc_VinInit(r_vin_Channel_t Ch, r_vin_Config_t *Conf,
            uintptr_t BufferAddr1, uintptr_t BufferAddr2, uintptr_t BufferAddr3)
{
    uintptr_t baseAddr;
    uint32_t dmr2;
    bool input_is_yuv = false;
    bool output_is_yuv = false;
    uint32_t vmc = 0;

    /* Check if the buffer address is a multiple of 128 bytes and a non-zero value */
    if ((0u == BufferAddr1) ||
        (0u == BufferAddr2) ||
        (0u == BufferAddr3) ||
        ((BufferAddr1 & 0x0000007Fu) != 0u) ||
        ((BufferAddr2 & 0x0000007Fu) != 0u) ||
        ((BufferAddr3 & 0x0000007Fu) != 0u))
    {
        R_PRINT_Log("[loc_VinInit]:NG.Addr1:0x%x,2:0x%x,3:0x%x.\r\n",
                     BufferAddr1, BufferAddr2, BufferAddr3);
        return 1;
    }

    baseAddr = R_VIN_PRV_GetRegBase(Ch);

    if (R_VIN_VMC_INF_YUV8_BT601 == Conf->VmcInf)
    {
        input_is_yuv = true;
    }
    else /* R_VIN_OUTF_ARGB8888 */
    {
        input_is_yuv = false;
    }

    if (R_VIN_OUTF_YCBCR420 == Conf->OutputFmt)
    {
        output_is_yuv = true;
    }
    else /* R_VIN_OUTF_ARGB8888 */
    {
        output_is_yuv = false;
    }

    vmc = R_VIN_PRV_ReadReg32(baseAddr + R_VIN_VMC);
    vmc |= (Conf->VmcIm & VMC_IM_MASK);

    if(Conf->VmcInf == R_VIN_VMC_INF_RGB888)
    {
        vmc |= VMC_INF_RGB888;
    }
    else /* R_VIN_VMC_INF_YUV8_BT601 */
    {
        vmc |= VMC_INF_YUV8_BT601;
    }
    vmc |= VMC_VUP;
    vmc &= ~VMC_DPINE;
    /* input:Conf->VmcInf, output:Conf->Fmt */
    if (input_is_yuv == output_is_yuv)
    {
        /* Both Input and output are YUV or RGB */
        vmc |= VMC_BPS;
    }

    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VMC, vmc);

    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VINTS, 0);
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VIE, 0);

    /* dmr2 = VDMR2_FTEV | VDMR2_VPS | VDMR2_HPS | VDMR2_CES; */
    dmr2 = VDMR2_FTEV;
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VDMR2, dmr2);

    if ((Conf->Cam_Res_X == Conf->Scaled_X) &&
        (Conf->Cam_Res_Y == Conf->Scaled_Y))
    {
        /* Configure clipping. Specific to Camera */
        R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VSLPrC, Conf->pos_y);
        if (true == Conf->Interlaced)
        {
            /* In the case of interlace, halve */
            R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VELPrC, ((Conf->Cam_Res_Y / 2) - 1));
        }
        else
        {
            R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VELPrC, (Conf->Cam_Res_Y - 1));
        }
        /* This setting worked nice */
        R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VSPPrC, Conf->pos_x);
        R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VEPPrC, (Conf->Cam_Res_X - 1));
    }

    /* Set stride (matching with VSPD/DU settings) */
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VIS, Conf->Stride);

    /* Conf->Offset : Specify the physical address in 128-byte units. */
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VUVAOF, Conf->Offset);

    /* All three base address point to the same region */
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VMB1, BufferAddr1);
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VMB2, BufferAddr2);
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VMB3, BufferAddr3);

    return 0;
}

static uint32_t loc_VinGetBwidth(uint32_t ratio)
{
    uint32_t bwidth = 0;
    uint32_t mant = 0, frac = 0;
    uint32_t mant_dush = 0;

    mant = (ratio & 0xF000) >> 12;
    frac = ratio & 0x0FFF;
    if (0 != mant)
    {
        /* Table 26.18 of HWM */
        switch (mant)
        {
        case 1:
        case 2:
        case 3:
            mant_dush = 1;
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            mant_dush = 2;
            break;
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            mant_dush = 4;
            break;
        default:
            R_PRINT_Log("[loc_VinGetBwidth]NG. mant:%d\n", mant);
            break;
        }

        bwidth = 64 * 4096 * mant_dush / (4096 * mant + frac);
    }
    else
    {
        bwidth = 64;
    }
    return bwidth;
}

static uint32_t loc_VinComputeRatio(uint32_t input, uint32_t output, bool horizontal )
{
    uint32_t ratio = 0;
    uint32_t mant = 0;
    uint32_t frac = 0;

    if (0 == output)
    {
         R_PRINT_Log("[loc_VinComputeRatio]NG. ouput is 0.\n");
         return 0;
    }

    ratio = input * 4096 / output;

    /* scale down range check */
    if (ratio >= 0x10000)
    {
         /* 0xF of 0xF000:mant, 0xFFF of 0x0FFF:frac */
         ratio = 0xFFFF;

         R_PRINT_Log("[loc_VinComputeRatio]NG. ratio is out of range. changed ratio to 0xFFFF.\n");
    }
    else
    {
        mant = (ratio & 0xF000) >> 12;

        /* scale up range check */
        if (0 == mant)
        {
            frac = ratio & 0x0FFF;

            /* range check(HFRAC of VnUDS_SCALE) */
            if ((horizontal == true) && (0x800 > frac))
            {
                ratio = 0x800;
                R_PRINT_Log("[loc_VinComputeRatio]NG. ratio is out of range. changed ratio to 0x800.\n");
            }
            /* range check(VFRAC of VnUDS_SCALE) */
            else if ((horizontal == false) && (0x556 > frac))
            {
                ratio = 0x556;
                R_PRINT_Log("[loc_VinComputeRatio]NG. ratio is out of range. changed ratio to 0x556.\n");
            }
        }
    }

    return (ratio);
}

/**
 * Section Global Functions
 */
void R_VIN_Start(r_vin_Channel_t Ch, r_vin_Config_t *Conf)
{
    uint32_t dmr;
    uintptr_t baseAddr;

    baseAddr = R_VIN_PRV_GetRegBase(Ch);
    if (R_VIN_OUTF_ARGB8888 == Conf->OutputFmt)
    {
        dmr = VDMR_A8BIT | VDMR_EXRGB | VDMR_DTMD_ARGB;
    }
    else /* R_VIN_OUTF_YCBCR420 */
    {
        dmr = VDMR_DTMD_YCSEP_YCBCR420 | VDMR_ABIT;
    }

    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VDMR, dmr);
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VMC, R_VIN_PRV_ReadReg32(baseAddr + R_VIN_VMC) | VMC_ME);
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VFC, VNFC_C_FRAME);
}

r_vin_Error_t R_VIN_Stop(r_vin_Channel_t Ch)
{
    uint32_t val;
    uintptr_t baseAddr;
    uint32_t timeout;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    r_vin_Error_t ret = R_VIN_ERR_SUCCESS;

    baseAddr = R_VIN_PRV_GetRegBase(Ch);

    val = R_VIN_PRV_ReadReg32(baseAddr + R_VIN_VMC);
    val &= ~(VMC_ME | VMC_VUP);
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VMC, val);
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VFC, 0x00000000);

    timeout = 100;
    while ((R_VIN_PRV_ReadReg32(baseAddr + R_VIN_VMS) & VMS_CA) && (timeout > 0))
    {
        osal_ret = R_OSAL_ThreadSleepForTimePeriod(1);
        if (OSAL_RETURN_OK != osal_ret)
        {
            R_PRINT_Log("[R_VIN_Stop]NG: osal_ret:%d\r\n", osal_ret);
            ret = R_VIN_ERR_FAILED;
            break;
        }
        timeout --;
    }
    if(timeout == 0)
    {
        R_PRINT_Log("[R_VIN_Stop]NG: CA bit of VMS register check Timeout\r\n");
        ret = R_VIN_ERR_TIMEOUT;
    }

    return ret;
}

/* NOTE: Should be called before starting VIN */
r_vin_Error_t R_VIN_ScaleImage(r_vin_Channel_t Ch, r_vin_Config_t *Conf)
{
    uint32_t val;
    uintptr_t baseAddr;
    uint32_t ratio_h = 0;
    uint32_t ratio_v = 0;
    uint32_t bwidth_h = 0;
    uint32_t bwidth_v = 0;
    r_vin_Error_t ret = R_VIN_ERR_SUCCESS;

    switch (Ch)
    {
    case R_VIN0:
    case R_VIN1:
    case R_VIN4:
    case R_VIN5:
        /* Ch that supports scale change */
        break;
    case R_VIN2:
    case R_VIN3:
    case R_VIN6:
    case R_VIN7:
    default:
        /* Ch that does not support scale change */
        ret = R_VIN_ERR_INVALID_PARAMETER;
        R_PRINT_Log("[R_VIN_ScaleImage]NG: ch:%d\n", Ch);
        break;
    }

    if (R_VIN_ERR_SUCCESS == ret)
    {
        baseAddr = R_VIN_PRV_GetRegBase(Ch);

        /* 1: Enables scaling by the UDS. */
        val = SCLE;
        R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VMC, val);

        /* Try upscale it */
        R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VUDS_CTRL, VUDS_CTRL_AMD);

        /* Configure clipping. Specific to Camera */
        R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VSLPrC, Conf->pos_y);
        if (true == Conf->Interlaced)
        {
            /* In the case of interlace, halve */
            R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VELPrC, ((Conf->Cam_Res_Y / 2) - 1));
        }
        else
        {
            R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VELPrC, (Conf->Cam_Res_Y - 1));
        }
        /* This setting worked nice */
        R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VSPPrC, Conf->pos_x);
        R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VEPPrC, (Conf->Cam_Res_X - 1));

        /* Check the size after scale-up/down */
        if (R_VIN_SCALED_SIZE_MAX < Conf->Scaled_X)
        {
            ret = R_VIN_ERR_OUT_OF_RANGE;
            R_PRINT_Log("[R_VIN_ScaleImage]NG. Out of range. Scaled_X:%d.\n", Conf->Scaled_X);
        }
        else if (R_VIN_SCALED_SIZE_MIN > Conf->Scaled_X)
        {
            ret = R_VIN_ERR_OUT_OF_RANGE;
            R_PRINT_Log("[R_VIN_ScaleImage]NG. Out of range. Scaled_X:%d.\n", Conf->Scaled_X);
        }

        if (R_VIN_SCALED_SIZE_MAX < Conf->Scaled_Y)
        {
            ret = R_VIN_ERR_OUT_OF_RANGE;
            R_PRINT_Log("[R_VIN_ScaleImage]NG. Out of range. Scaled_Y:%d.\n", Conf->Scaled_Y);
        }
        else if (R_VIN_SCALED_SIZE_MIN > Conf->Scaled_Y)
        {
            ret = R_VIN_ERR_OUT_OF_RANGE;
            R_PRINT_Log("[R_VIN_ScaleImage]NG. Out of range. Scaled_Y:%d.\n", Conf->Scaled_Y);
        }

        if (R_VIN_ERR_SUCCESS == ret)
        {
            /* H - scale factor, V - scale factor */
            ratio_h = loc_VinComputeRatio(Conf->Cam_Res_X, Conf->Scaled_X, true);
            ratio_v = loc_VinComputeRatio(Conf->Cam_Res_Y, Conf->Scaled_Y, false);
            val = (ratio_h << 16) | ratio_v;
            R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VUDS_SCALE, val);

            /* Standard for upscale */
            bwidth_h = loc_VinGetBwidth(ratio_h);
            bwidth_v = loc_VinGetBwidth(ratio_v);
            val = VUDS_PASS_BWIDTH_BWIDTH_H(bwidth_h)
                | VUDS_PASS_BWIDTH_BWIDTH_V(bwidth_v);
            R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VUDS_PASS_BWIDTH, val);

            /* Set value to VnUDS_CLIP_SIZE */
            val = VUDS_CLIP_SIZE_CL_HSIZE(Conf->Out_Res_X);
            if (true == Conf->Interlaced)
            {
                /* In the case of interlace, halve */
                val |= VUDS_CLIP_SIZE_CL_VSIZE(Conf->Out_Res_Y / 2);
            }
            else
            {
                val |= VUDS_CLIP_SIZE_CL_VSIZE(Conf->Out_Res_Y);
            }
            R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VUDS_CLIP_SIZE, val);
        }
    }

    return ret;
}

void R_VIN_EnableInterrupt(r_vin_Channel_t Ch, uint32_t VnIE)
{
    uintptr_t baseAddr;
    uint32_t val;

    baseAddr = R_VIN_PRV_GetRegBase(Ch);
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VINTS, (VnIE & VINTS_MASK));

    val = R_VIN_PRV_ReadReg32(baseAddr + R_VIN_VIE);
    val |= VnIE;
    val &= VIE_MASK;
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VIE, val);
}

void R_VIN_DisableInterrupt(r_vin_Channel_t Ch, uint32_t VnIE)
{
    uintptr_t baseAddr;
    uint32_t val;

    baseAddr = R_VIN_PRV_GetRegBase(Ch);
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VINTS, (VnIE & VINTS_MASK));

    val = R_VIN_PRV_ReadReg32(baseAddr + R_VIN_VIE);
    val &= ~VnIE;
    val &= VIE_MASK;
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VIE, val);
}

void R_VIN_ClearInterrupt(r_vin_Channel_t Ch)
{
    uint32_t val;
    uintptr_t baseAddr;

    baseAddr = R_VIN_PRV_GetRegBase(Ch);

    val = R_VIN_PRV_ReadReg32(baseAddr + R_VIN_VINTS);
    R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VINTS, (val & VINTS_MASK));
}


r_vin_Error_t R_VIN_Init(r_vin_Channel_t Ch, r_vin_Config_t *Conf,
                         uintptr_t BufferAddr)
{
    r_vin_Error_t ret = R_VIN_ERR_SUCCESS;

    ret = loc_VinInit(Ch, Conf, BufferAddr, BufferAddr, BufferAddr);
    if (0 != ret)
    {
        R_PRINT_Log("[R_VIN_Init]NG. loc_VinInit ret:%d\n", ret);
        ret = R_VIN_ERR_FAILED;
    }
    return ret;
}

r_vin_Error_t R_VIN_InitEx(r_vin_Channel_t Ch, r_vin_Config_t *Conf,
            uintptr_t BufferAddr1, uintptr_t BufferAddr2, uintptr_t BufferAddr3)
{
    r_vin_Error_t ret = R_VIN_ERR_SUCCESS;

    ret = loc_VinInit(Ch, Conf, BufferAddr1, BufferAddr2, BufferAddr3);
    if (0 != ret)
    {
        R_PRINT_Log("[R_VIN_InitEx]NG. loc_VinInit ret:%d\n", ret);
        ret = R_VIN_ERR_FAILED;
    }
    return ret;
}

r_vin_Error_t R_VIN_Prepare(r_vin_Channel_t Ch, uint8_t Chsel, r_vin_Config_t *Conf)
{
    uintptr_t baseAddr;
    r_vin_Channel_t parentCh;
    r_vin_Error_t ret = R_VIN_ERR_SUCCESS;

    baseAddr = R_VIN_PRV_GetRegBase(Ch);
    ret = loc_VinPowerOn(Ch);

    if (R_VIN_ERR_SUCCESS == ret)
    {
        parentCh = Ch < R_VIN4 ? R_VIN0 : R_VIN4;
        if(!((1 << parentCh) & mask_vin_power))
        {
            if(parentCh != Ch)
            {
                ret = loc_VinPowerOn(parentCh);
            }

            if (R_VIN_ERR_SUCCESS == ret)
            {
                writel(VCSI_IFMD_DES1 | VCSI_IFMD_DES0 | VCSI_IFMD_CSI_CHSEL(Chsel),
                       R_VIN_PRV_GetRegBase(parentCh) + R_VIN_VCSI_IFMD);
                mask_vin_power |= (1 << parentCh);
             }
        }

        R_VIN_PRV_WriteReg32(baseAddr + R_VIN_VIE, 0);
    }

    return ret;
}

int32_t R_VIN_GetLatestFrameBufIndex(r_vin_Channel_t Ch)
{
    uint32_t val;
    uintptr_t baseAddr;

    baseAddr = R_VIN_PRV_GetRegBase(Ch);
    val = R_VIN_PRV_ReadReg32(baseAddr + R_VIN_VMS);

    return (val >> 3) & 0x3;
}

uintptr_t R_VIN_GetLatestFrameBuffer(r_vin_Channel_t Ch)
{
    uint32_t slot;
    uintptr_t pbuf;
    uintptr_t baseAddr;

    baseAddr = R_VIN_PRV_GetRegBase(Ch);
    slot = R_VIN_PRV_ReadReg32(baseAddr + R_VIN_VMS);
    slot = (slot >> 3) & 0x3;

    if (slot == 0)
    {
        pbuf = R_VIN_PRV_ReadReg32(baseAddr + R_VIN_VMB1);
    }
    else if (slot == 1)
    {
        pbuf = R_VIN_PRV_ReadReg32(baseAddr + R_VIN_VMB2);
    }
    else
    {
        pbuf = R_VIN_PRV_ReadReg32(baseAddr + R_VIN_VMB3);
    }

    return pbuf;
}

r_vin_Error_t R_VIN_Release(r_vin_Channel_t Ch)
{
    r_vin_Channel_t parentVin;
    r_vin_Error_t ret = R_VIN_ERR_SUCCESS;

    /* Reset module */
    ret = loc_VinPowerOff(Ch);

    if (R_VIN_ERR_SUCCESS == ret)
    {
        if (Ch < R_VIN4)
        {
            /* VIN0-3 needs VIN0 register for channel selection */
            parentVin = R_VIN0;
        }
        else
        {
            /* VIN4-7 needs VIN4 register for channel selection */
            parentVin = R_VIN4;
        }

        ret = loc_VinPowerOff(parentVin);
    }

    return ret;
}

r_vin_Error_t R_VIN_DeInit(r_vin_Channel_t Ch)
{
    r_vin_Channel_t parentCh;
    r_vin_Error_t ret = R_VIN_ERR_SUCCESS;

    /* Stop the video capture */
    ret = R_VIN_Stop(Ch);

    /* Release the video input channel */
    ret |= R_VIN_Release(Ch);

    /* clear mask_vin_power */
    parentCh = Ch < R_VIN4 ? R_VIN0 : R_VIN4;
    mask_vin_power &= ~(1 << parentCh);

    return ret;
}
