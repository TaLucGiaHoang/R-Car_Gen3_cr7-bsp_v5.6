/*************************************************************************************************************
* WindowMng_wmdrv_sys_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <string.h>

#include "rcar-xos/osal/r_osal.h"
#include "r_wm_config.h"
#include "r_wm_bridge.h"
#include "r_prr_api.h"
#include "r_du_config.h"
#include "r_du_api.h"
#include "r_vsp2_api.h"
#include "r_vsp2_vspd_api.h"
#include "r_wmdrv_api.h"
#include "r_wmdrv_sys.h"
#include "r_wmdrv_sys_irq.h"
#include "r_cpg_api.h"
#include "r_cio_wm_bridge.h"
#include "r_doc_api.h"
#include "r_delay_api.h"
#include "r_print_api.h"

/*
 * Section: function prototypes
 */
extern r_vsp2_Error_t R_VSP2_WinAlphaSet(r_vsp2_Unit_t Vsp2Unit, uint8_t Alpha, uint32_t rpfid);
static uint32_t loc_EnableModule(const r_wmdrv_Sys_Vout_t * Inst);
static r_wm_Error_t loc_InstConfigSet(r_wmdrv_Sys_Vout_t *Inst,
                const r_prr_DevName_t Dev, const r_wm_DispTiming_t *Timings);
static void loc_SetupDuTimings (r_du_Unit_t DuUnit, r_wm_DispTiming_t * WmTiming,
                r_du_DispTimings_t * DuTiming);

/*
 * Section: lobal variables
 */

static r_wmdrv_Sys_Vout_t       r_wmdrv_Sys_VoutConf[R_WM_DEV_LAST];
static r_wmdrv_Sys_VspdInfo_t   loc_VspdConf[R_VSPD_MAX_UNIT_NUM] =
{
                {
                                .VspdUnit = R_VSP2_VSPD0,
                                .Wpf0InUse = 0,
                                .Wpf1InUse = 0,
                                .IrqVspdThread = 0,
                                .IrqVspdQuit = -1,
                },
                {
                                .VspdUnit = R_VSP2_VSPD1,
                                .Wpf0InUse = 0,
                                .Wpf1InUse = 0,
                                .IrqVspdThread = 0,
                                .IrqVspdQuit = -1,
                },
                {
                                .VspdUnit = R_VSP2_VSPD2,
                                .Wpf0InUse = 0,
                                .Wpf1InUse = 0,
                                .IrqVspdThread = 0,
                                .IrqVspdQuit = -1,
                },
};
r_vsp2_Dpr_Conf_t          Dpr_Conf[R_VSPD_MAX_UNIT_NUM] =
{
    {/* DPR settings for VSPDL */
        R_VSP_DPR_ROUTE_BRU0,       /**< Vi6_Dpr_Rpf0_Route regster */
        R_VSP_DPR_ROUTE_BRU1,       /**< Vi6_Dpr_Rpf1_Route regster */
        R_VSP_DPR_ROUTE_BRU2,       /**< Vi6_Dpr_Rpf2_Route regster */
        R_VSP_DPR_ROUTE_BRS_ILV0,   /**< Vi6_Dpr_Rpf3_Route regster */
        R_VSP_DPR_ROUTE_BRS_ILV1,   /**< Vi6_Dpr_Rpf4_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Sru_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Uds_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Lut_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Clu_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Hst_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Hsi_Route regster */
        R_VSP_DPR_ROUTE_WPF0,       /**< Vi6_Dpr_Bru_Route regster */
        R_VSP_DPR_ROUTE_BRS_TO_WPF1,/**< Vi6_Dpr_Ilv_Brs_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Shp_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Uif4_Route regster */
        R_VSP_DPR_ROUTE_DISABLE     /**< Vi6_Dpr_Uif5_Route regster */
    },
    {/* DPR settings for VSPD1 */
        R_VSP_DPR_ROUTE_BRU0,       /**< Vi6_Dpr_Rpf0_Route regster */
        R_VSP_DPR_ROUTE_BRU1,       /**< Vi6_Dpr_Rpf1_Route regster */
        R_VSP_DPR_ROUTE_BRU2,       /**< Vi6_Dpr_Rpf2_Route regster */
        R_VSP_DPR_ROUTE_BRU3,       /**< Vi6_Dpr_Rpf3_Route regster */
        R_VSP_DPR_ROUTE_BRU4,       /**< Vi6_Dpr_Rpf4_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Sru_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Uds_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Lut_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Clu_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Hst_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Hsi_Route regster */
        R_VSP_DPR_ROUTE_WPF0,       /**< Vi6_Dpr_Bru_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Ilv_Brs_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Shp_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Uif4_Route regster */
        R_VSP_DPR_ROUTE_DISABLE     /**< Vi6_Dpr_Uif5_Route regster */
    },
    {/* DPR settings for VSPD2 */
        R_VSP_DPR_ROUTE_BRU0,       /**< Vi6_Dpr_Rpf0_Route regster */
        R_VSP_DPR_ROUTE_BRU1,       /**< Vi6_Dpr_Rpf1_Route regster */
        R_VSP_DPR_ROUTE_BRU2,       /**< Vi6_Dpr_Rpf2_Route regster */
        R_VSP_DPR_ROUTE_BRU3,       /**< Vi6_Dpr_Rpf3_Route regster */
        R_VSP_DPR_ROUTE_BRU4,       /**< Vi6_Dpr_Rpf4_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Sru_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Uds_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Lut_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Clu_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Hst_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Hsi_Route regster */
        R_VSP_DPR_ROUTE_WPF0,       /**< Vi6_Dpr_Bru_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Ilv_Brs_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Shp_Route regster */
        R_VSP_DPR_ROUTE_DISABLE,    /**< Vi6_Dpr_Uif4_Route regster */
        R_VSP_DPR_ROUTE_DISABLE     /**< Vi6_Dpr_Uif5_Route regster */
    }
};

extern r_wmdrv_Sys_DocInfo_t DocIrqInfo[R_WM_DOC_UNIT_NUM];

/*
 * Section: local Functions
 */

/**
 * Enable the DU and VSPD by powering them on and resetting the driver.
 */
static uint32_t loc_EnableModule(const r_wmdrv_Sys_Vout_t * Inst)
{
    uint32_t ret = 0;
    r_cpg_Error_t cpg_ret = R_CPG_ERR_SUCCESS;
    e_osal_return_t osal_ret;
    e_osal_pm_required_state_t osal_state;

    if (0 != Inst) {
        switch (Inst->DuUnit) {
        case R_DU_DU0:
            cpg_ret = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_7, R_CPG_BIT24, false);
            break;
        case R_DU_DU1:
            /* DU config*/
            cpg_ret = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_7, R_CPG_BIT23, false);
            break;
        case R_DU_DU2:
            /* DU config*/
            cpg_ret = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_7, R_CPG_BIT22, false);
            break;
        case R_DU_DU3:
            /* DU config*/
            cpg_ret = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_7, R_CPG_BIT21, false);
            break;
        default:
            ret = 1;
            R_PRINT_Log("[WMDrvSys]: loc_EnableModule DuUnit(%d) failed(%d)\r\n", Inst->DuUnit, ret);
            break;
        }
        if (R_CPG_ERR_SUCCESS != cpg_ret) {
           ret = 1;
           R_PRINT_Log("[WMDrvSys]: R_PMA_CPG_SetModuleStopStat failed(%d)\r\n", cpg_ret);
        }

        if (0 == ret) {
            switch(Inst->VspdConf->VspdUnit) {
            case R_VSP2_VSPD0:
                if (0 == Inst->VspdConf->InitFlag) {
                    /* Enable VSPD0 */
                    osal_ret = R_OSAL_PmSetRequiredState(Inst->VspdConf->device_handle, OSAL_PM_REQUIRED_STATE_REQUIRED, false);
                    if (OSAL_RETURN_OK == osal_ret) {
                        /* Wait VSPD0 enabled */
                        do
                        {
                            R_OSAL_ThreadSleepForTimePeriod(1);
                            osal_ret = R_OSAL_PmGetRequiredState(Inst->VspdConf->device_handle, &osal_state);
                            if (OSAL_RETURN_OK != osal_ret)
                            {
                                ret = 1;
                                R_PRINT_Log("[WMDrvSys]: R_OSAL_PmGetRequiredState failed(%d)\r\n", osal_ret);
                            }
                        } while (OSAL_PM_REQUIRED_STATE_REQUIRED != osal_state);

                        cpg_ret = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_6, R_CPG_BIT3, false);
                        if (R_CPG_ERR_SUCCESS != cpg_ret) {
                            ret = 1;
                            R_PRINT_Log("[WMDrvSys]: R_PMA_CPG_SetModuleStopStat failed(%d)\r\n", cpg_ret);
                        }
                    } else {
                        ret = 1;
                        R_PRINT_Log("[WMDrvSys]: R_OSAL_PmSetRequiredState failed(%d), OSAL_PM_REQUIRED_STATE_REQUIRED\r\n", osal_ret);
                    }
                }
                break;
            case R_VSP2_VSPD1:
                /* Enable VSPD1 */
                osal_ret = R_OSAL_PmSetRequiredState(Inst->VspdConf->device_handle, OSAL_PM_REQUIRED_STATE_REQUIRED, false);
                if (OSAL_RETURN_OK == osal_ret) {
                   /* Wait VSPD1 enabled */
                   do
                   {
                       R_OSAL_ThreadSleepForTimePeriod(1);
                       osal_ret = R_OSAL_PmGetRequiredState(Inst->VspdConf->device_handle, &osal_state);
                       if (OSAL_RETURN_OK != osal_ret)
                       {
                           ret = 1;
                           R_PRINT_Log("[WMDrvSys]: R_OSAL_PmGetRequiredState failed(%d)\r\n", osal_ret);
                       }
                   } while (OSAL_PM_REQUIRED_STATE_REQUIRED != osal_state);
                    cpg_ret = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_6, R_CPG_BIT2, false);
                    if (R_CPG_ERR_SUCCESS != cpg_ret) {
                        ret = 1;
                        R_PRINT_Log("[WMDrvSys]: R_PMA_CPG_SetModuleStopStat failed(%d)\r\n", cpg_ret);
                    }
                } else {
                    ret = 1;
                    R_PRINT_Log("[WMDrvSys]: R_OSAL_PmSetRequiredState failed(%d), OSAL_PM_REQUIRED_STATE_REQUIRED\r\n", osal_ret);
                }
                break;
            case R_VSP2_VSPD2:
                /* Enable VSPD2 */
                osal_ret = R_OSAL_PmSetRequiredState(Inst->VspdConf->device_handle, OSAL_PM_REQUIRED_STATE_REQUIRED, false);
                if (OSAL_RETURN_OK == osal_ret) {
                   /* Wait VSPD2 enabled */
                   do
                   {
                       R_OSAL_ThreadSleepForTimePeriod(1);
                       osal_ret = R_OSAL_PmGetRequiredState(Inst->VspdConf->device_handle, &osal_state);
                       if (OSAL_RETURN_OK != osal_ret)
                       {
                            ret = 1;
                           R_PRINT_Log("[WMDrvSys]: R_OSAL_PmGetRequiredState failed(%d)\r\n", osal_ret);
                       }
                   } while (OSAL_PM_REQUIRED_STATE_REQUIRED != osal_state);
                    cpg_ret = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_6, R_CPG_BIT1, false);
                    if (R_CPG_ERR_SUCCESS != cpg_ret) {
                        ret = 1;
                        R_PRINT_Log("[WMDrvSys]: R_PMA_CPG_SetModuleStopStat failed(%d)\r\n", cpg_ret);
                    }
                } else {
                    ret = 1;
                    R_PRINT_Log("[WMDrvSys]: R_OSAL_PmSetRequiredState failed(%d), OSAL_PM_REQUIRED_STATE_REQUIRED\r\n", osal_ret);
                }
                break;
            default:
                ret = 1;
                R_PRINT_Log("[WMDrvSys]: loc_EnableModule VspdUnit(%d) failed(%d)\r\n", Inst->VspdConf->VspdUnit, ret);
                break;
            }
            if (R_CPG_ERR_SUCCESS != cpg_ret) {
               ret = 1;
               R_PRINT_Log("[WMDrvSys]: loc_EnableModule CPG failed(%d)\r\n", ret);
            }
       }
    }
    return ret;
}

/**
 * Set the default DU configuration.
 */
static r_wm_Error_t loc_InstConfigSet(r_wmdrv_Sys_Vout_t *Inst,
                const r_prr_DevName_t Dev, const r_wm_DispTiming_t *Timings)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    uint32_t err = 0;

    if ((0 != Inst) && (0 != Timings)) {

        /* Check DU validity for this device */
        if (R_PRR_RCARD3 == Dev) {
            Inst->DuPlaneMsk = 0;
            ret = R_WM_ERR_WRONG_PARAMETER;
            R_PRINT_Log("[WMDrvSys]: loc_InstConfigSet Dev failed(%d)\r\n", ret);
            goto clean;
        }

        if (R_DU_DU2 == Inst->DuUnit) {
            if (R_PRR_RCARM3N == Dev) {
                /* Correct DuUnit in case of M3-N Vout2 is DU3 not Du2 as
                 * initialised per default*/
                Inst->DuUnit   = R_DU_DU3;
                Inst->VspdConf    = &loc_VspdConf[0];
            }
        }

        switch (Inst->DuUnit) {
        case R_DU_DU0:
            Inst->DuPlaneMsk = 1;
            Inst->DuPlaneCfg[0].vspd_unit = R_VSP2_VSPD0;
            Inst->DuPlaneCfg[0].format = R_DU_FORMAT_ARGB8888;
            Inst->DuPlaneCfg[0].blending = R_DU_BLEND_OPAQUE;
            Inst->DuPlaneCfg[0].size_w = Timings->ScreenWidth;
            Inst->DuPlaneCfg[0].size_h = Timings->ScreenHeight;
            Inst->DuPlaneCfg[0].pos_x = 0;
            Inst->DuPlaneCfg[0].pos_y = 0;
            break;
        case R_DU_DU1:
            Inst->DuPlaneMsk = 2;
            Inst->DuPlaneCfg[1].vspd_unit = R_VSP2_VSPD1;
            Inst->DuPlaneCfg[1].format = R_DU_FORMAT_ARGB8888;
            Inst->DuPlaneCfg[1].blending = R_DU_BLEND_OPAQUE;
            Inst->DuPlaneCfg[1].size_w = Timings->ScreenWidth;
            Inst->DuPlaneCfg[1].size_h = Timings->ScreenHeight;
            Inst->DuPlaneCfg[1].pos_x = 0;
            Inst->DuPlaneCfg[1].pos_y = 0;
            break;
        case R_DU_DU2:
            if ((R_PRR_RCARE3 == Dev) || (R_PRR_RCARM3N == Dev)) {
                Inst ->DuPlaneMsk = 0;
                ret = R_WM_ERR_WRONG_PARAMETER;
                R_PRINT_Log("[WMDrvSys]: loc_InstConfigSet Dev failed(%d)\r\n", ret);
            } else {
                Inst->DuPlaneMsk = 1;
                Inst->DuPlaneCfg[0].vspd_unit = R_VSP2_VSPD2;
                Inst->DuPlaneCfg[0].format = R_DU_FORMAT_ARGB8888;
                Inst->DuPlaneCfg[0].blending = R_DU_BLEND_OPAQUE;
                Inst->DuPlaneCfg[0].size_w = Timings->ScreenWidth;
                Inst->DuPlaneCfg[0].size_h = Timings->ScreenHeight;
                Inst->DuPlaneCfg[0].pos_x = 0;
                Inst->DuPlaneCfg[0].pos_y = 0;
            }
            break;
        case R_DU_DU3:
            if ((R_PRR_RCARM3W == Dev) || (R_PRR_RCARE3 == Dev)) {
                Inst ->DuPlaneMsk = 0;
                ret = R_WM_ERR_WRONG_PARAMETER;
                R_PRINT_Log("[WMDrvSys]: loc_InstConfigSet Dev failed(%d)\r\n", ret);
            } else {
                Inst->DuPlaneMsk = 2;
                Inst->DuPlaneCfg[1].vspd_unit = R_VSP2_VSPD0;
                Inst->DuPlaneCfg[1].format = R_DU_FORMAT_ARGB8888;
                Inst->DuPlaneCfg[1].blending = R_DU_BLEND_OPAQUE;
                Inst->DuPlaneCfg[1].size_w = Timings->ScreenWidth;
                Inst->DuPlaneCfg[1].size_h = Timings->ScreenHeight;
                Inst->DuPlaneCfg[1].pos_x = 0;
                Inst->DuPlaneCfg[1].pos_y = 0;
                /* Note Size of the plane will be set when timings is called
                 * Color format may be changed by user*/
            }
            break;
        default:
            Inst->DuPlaneMsk = 0;
            break;
        }

        /* Enable the CPG for HDMI or LVDS depending on the VOUT type required*/
        if (R_WM_VOUT_HDMI == Timings->VoutType)
        {
            if (R_DU_DU1 == Inst->DuUnit)
            {
                /* Enable HDMI Clock */
                err = R_PMA_CPG_SetClockStopStat(R_CPG_HDMI_CLOCK, false);
                /* Enable HDMI0 */
                err += R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_7, R_CPG_BIT29, false);
                /* Reset HDMI0 */
                err += R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT29, 1);
                delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
                err += R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT29, 0);
            }
            else if (R_DU_DU2 == Inst->DuUnit)
            {
                /* Enable HDMI Clock */
                err = R_PMA_CPG_SetClockStopStat(R_CPG_HDMI_CLOCK, false);
                /* Enable HDMI1 */
                err += R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_7, R_CPG_BIT28, false);
                /* Reset HDMI1 */
                err += R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT28, 1);
                delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
                err += R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT28, 0);
            }
        }
        else if (R_WM_VOUT_LVDS == Timings->VoutType)
        {
            if (R_DU_DU0 == Inst->DuUnit)
            {
                /* Enable LVDS0 */
                err = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_7, R_CPG_BIT27, false);
                /* Reset VVDS0 */
                err += R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT27, 1);
                delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
                err += R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT27, 0);
            }
            else if (R_DU_DU1 == Inst->DuUnit)
            {
                /* Enable LVDS1 */
                err = R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_7, R_CPG_BIT27, false);
                /* Reset LVDS1 */
                err += R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT27, 1);
                delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
                err += R_PMA_CPG_SetResetStat(R_CPG_REGIDX_7, R_CPG_BIT27, 0);
            }
        }
        else
        {
            /* VGA nothing to do */
        }
        if (0 != err) {
            R_PRINT_Log("[WMDrvSys]: loc_InstConfigSet CPG API failed(%d)\r\n", err);
            ret = R_WM_ERR_FAILED;
        }
    }

clean:
    return ret;
}

static void loc_SetupDuTimings (r_du_Unit_t DuUnit, r_wm_DispTiming_t * WmTiming,
                r_du_DispTimings_t * DuTiming)
{
    /* Screen size */
    DuTiming->ScreenWidth      = WmTiming->ScreenWidth;
    DuTiming->ScreenHeight     = WmTiming->ScreenHeight;
    /* Pixel clock */
    DuTiming->TargetDispClock  = WmTiming->PixelClock;
    DuTiming->CurrentDispClock = WmTiming->CurrentDispClock;

    /* DU Sync flags */
    /* FixMe, we shall convert r_wm_VoutFlags to r_du_VoutFlags.. Currently the
     * same but it may change can cause problem in the future.
     */
    DuTiming->VoutFlags        =  WmTiming->VoutFlags;
    /* Vout timings
     * Note: Du timings uses back porch (VoutXs and VoutYs) as WM use front
     * porch for compatibility with old DDB driver
     */
    DuTiming->VoutHc           = WmTiming->VoutH.TotalPixel;
    DuTiming->VoutHsw          = WmTiming->VoutH.SyncWidth;
    DuTiming->VoutXs           = WmTiming->VoutH.BackPorch; /* for some reason DU uses back porch... */
    DuTiming->VoutVc           = WmTiming->VoutV.TotalPixel;
    DuTiming->VoutVsw          = WmTiming->VoutV.SyncWidth;
    DuTiming->VoutYs           = WmTiming->VoutV.BackPorch;

    /* FixMe: we shall convert r_wm_VoutType to r_du_VoutType.. Currently the
     * same but it may change can cause problem in the future.
     */
    DuTiming->VoutType = (r_du_VoutType_t) WmTiming->VoutType;
    /* specific DU register settings*/
    DuTiming->DotClkIn[0] = WmTiming->DotClkIn[0];
    DuTiming->DotClkIn[1] = WmTiming->DotClkIn[1];
    DuTiming->DotClkIn[2] = WmTiming->DotClkIn[2];
    DuTiming->DotClkIn[3] = WmTiming->DotClkIn[3];
    DuTiming->Extal = WmTiming->Extal;

    if (R_DU_VOUT_LVDS == DuTiming->VoutType) {
        DuTiming->LvdsPllLvdpllcr = WmTiming->LvdsPllLvdpllcr;
        DuTiming->LvdsPllLvddiv = WmTiming->LvdsPllLvddiv;
    } else {
        DuTiming->HdmiPllCfg = WmTiming->HdmiPllCfg;
        DuTiming->HdmiPllCtrl = WmTiming->HdmiPllCtrl;
        DuTiming->HdmiPllDivider = WmTiming->HdmiPllDivider;
    }
}

static uint32_t loc_GetVspdUnitNo(r_vsp2_Unit_t VspdUnit)
{
    uint32_t VspdUnitNo;

    switch(VspdUnit) {
    case R_VSP2_VSPD0:
        VspdUnitNo = 0u;
        break;
    case R_VSP2_VSPD1:
        VspdUnitNo = 1u;
        break;
    case R_VSP2_VSPD2:
        VspdUnitNo = 2u;
        break;
    default:
        VspdUnitNo = 0u;
        break;
    }

    return VspdUnitNo;
}

/*
 * Global Functions
 */

void * R_WMDRV_Sys_InstInit(r_wm_Dev_t WmDev)
{
    int32_t  err = 0;
    void    *ret_ptr = 0;

    /* INFO: VOUT2 is DU3 in case M3N
     * Does not support superimpose plane. 1 VSPD per DU
     * Need to clean up to pass VOut to DUUnit.
     *
     */
    switch (WmDev) {
    case R_WM_DEV_VOUT0:
        r_wmdrv_Sys_VoutConf[WmDev].DuUnit      = R_DU_DU0;
        r_wmdrv_Sys_VoutConf[WmDev].DuInUse     = 0;
        r_wmdrv_Sys_VoutConf[WmDev].VspdConf    = &loc_VspdConf[0];
        break;
    case R_WM_DEV_VOUT1:
        r_wmdrv_Sys_VoutConf[WmDev].DuUnit      = R_DU_DU1;
        r_wmdrv_Sys_VoutConf[WmDev].DuInUse     = 0;
        r_wmdrv_Sys_VoutConf[WmDev].VspdConf    = &loc_VspdConf[1];
        break;
    case R_WM_DEV_VOUT2:
        r_wmdrv_Sys_VoutConf[WmDev].DuUnit      = R_DU_DU2;
        r_wmdrv_Sys_VoutConf[WmDev].DuInUse     = 0;
        r_wmdrv_Sys_VoutConf[WmDev].VspdConf    = &loc_VspdConf[2];
        break;
    case R_WM_DEV_VOUT3:
        r_wmdrv_Sys_VoutConf[WmDev].DuUnit      = R_DU_DU3;
        r_wmdrv_Sys_VoutConf[WmDev].DuInUse     = 0;
        r_wmdrv_Sys_VoutConf[WmDev].VspdConf    = &loc_VspdConf[0];
        break;
    default:
        err = -1;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_InstInit WmDev(%d) failed(%d)\r\n", WmDev, err);
        break;
    }
    if (0 == err) {
        ret_ptr = (void *) &r_wmdrv_Sys_VoutConf[WmDev];
    }

    return ret_ptr;
}

r_wm_Error_t R_WMDRV_Sys_InstDeinit(r_wm_Dev_t WmDev)
{
    r_wm_Error_t wm_err = R_WM_ERR_SUCCESS;

    return wm_err;
}

void loc_remap_vout_m3n(void *Inst){
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;
    r_prr_DevName_t     dev;

    /* Remap DU2 -> DU3 here for M3N */
    dev = R_PRR_GetDevice();
    if (R_DU_DU2 == loc_vout->DuUnit) {
	if (R_PRR_RCARM3N == dev) {
	    /* Correct DuUnit in case of M3-N Vout2 is DU3 not Du2 as
	     * initialised per default*/
	    loc_vout->DuUnit      = R_DU_DU3;
	    loc_vout->VspdConf    = &loc_VspdConf[0];
	}
    }
}

/**
 * Initialise the Display unit
 */
r_wm_Error_t R_WMDRV_Sys_VoutInit(void *Inst, void *Timings)
{
    r_wm_Error_t        ret = R_WM_ERR_SUCCESS;
    int32_t             err = 0;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

    loc_remap_vout_m3n(Inst);
    if (0 != loc_vout) {
        err = R_WMDRV_Sys_IrqInit(loc_vout);
        if (0 == err) {
            err = loc_EnableModule(loc_vout);
            if (0 != err) {
                ret = R_WM_ERR_FAILED;
                R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutInit loc_EnableModule failed(%d)\r\n", err);
            } else {
                loc_vout->DuInUse = 1;
            }

            err = R_WMDRV_Sys_DocIrqInit(loc_vout);
            if (0 != err) {
                ret = R_WM_ERR_FAILED;
                R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutInit R_WMDRV_Sys_DocIrqInit failed(%d)\r\n", err);
            }

            /* Setup the default Board Videoout timings */
            if (0 == ret) {
                ret = R_CIO_BSP_WM_PRV_VideoOutDisplays(loc_vout->DuUnit, (r_wm_DispTiming_t *)Timings);

                if (0 == ret) {
                    ret = R_WMDRV_Sys_VoutTimingSet(loc_vout, (r_wm_DispTiming_t *)Timings);
                    if (0 == ret) {
                        ret = R_WMDRV_Sys_VoutColFmtSet(loc_vout, R_WM_FMT_ARGB8888);
                        if (0 != ret) {
                            R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutColFmtSet failed(%d)\r\n", ret);
                        }
                    } else {
                        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutTimingSet failed(%d)\r\n", ret);
                    }

                    if (0 == ret) {
                        ret = R_WMDRV_Sys_VoutBkgColSet(loc_vout, 0xFF000000);
                        if (0 != ret) {
                            R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutBkgColSet failed(%d)\r\n", ret);
                        }
                    }
                } else {
                    R_PRINT_Log("[WMDrvSys]: R_CIO_BSP_WM_PRV_VideoOutDisplays failed(%d)\r\n", ret);
                }
            }
        } else {
            ret = R_WM_ERR_FAILED;
            R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutInit R_WMDRV_Sys_IrqInit failed(%d)\r\n", err);
        }
    }
    return ret;
}

/**
 * Initialise the Display unit
 */
r_wm_Error_t R_WMDRV_Sys_VoutDeinit(void *Inst)

{
    r_wm_Error_t       ret = R_WM_ERR_SUCCESS;
    r_prr_DevName_t    dev;
    uint32_t           deinit_vspd = 1;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

    if (0 != loc_vout) {
        /* Disable the VOut */
        R_DU_Enable(loc_vout->DuUnit, R_DU_DISABLE);

        dev = R_PRR_GetDevice();

        if ((R_DU_DU3 == loc_vout->DuUnit) || (R_DU_DU0 == loc_vout->DuUnit)) {
            /* DU3 and DU0 shares VSPD0 we have to check if they are
             * both initialised before disabling VSPD IRQ
             */
            if (R_PRR_RCARH3 == dev) {
                if ((0 != r_wmdrv_Sys_VoutConf[R_WM_DEV_VOUT0].DuInUse) &&
                                (0 != r_wmdrv_Sys_VoutConf[R_WM_DEV_VOUT3].DuInUse)) {
                    deinit_vspd = 0;
                }
            } else if (R_PRR_RCARM3N == dev) {
                if ((0 != r_wmdrv_Sys_VoutConf[R_WM_DEV_VOUT0].DuInUse) &&
                                (0 != r_wmdrv_Sys_VoutConf[R_WM_DEV_VOUT2].DuInUse)) {
                    deinit_vspd = 0;
                }
            }
        }
        /* De-initialise Vout: Vsp2 and DU */
        if (1 == loc_vout->VspdConf->InitFlag) {
            R_WMDRV_Sys_IrqDeinit(loc_vout, deinit_vspd);
        }

        R_WMDRV_Sys_DocIrqDeinit(loc_vout);

        loc_vout->DuInUse = 0;
        if (R_DU_DU3 == loc_vout->DuUnit) {
            loc_vout->VspdConf->Wpf1InUse = 0;
        } else {
            loc_vout->VspdConf->Wpf0InUse = 0;
        }
        loc_vout->VspdConf->InitFlag--;
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutDeinit prameter Inst failed(%d)\r\n", ret);
    }
    return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutTimingSet(void *Inst, void *Timings)
{
    r_wm_Error_t        ret = R_WM_ERR_SUCCESS;
    r_du_Error_t        du_err;
    r_vsp2_Error_t      vsp2_err = R_VSP2_ERR_SUCCESS;
    r_prr_DevName_t     dev;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;
    r_du_DispTimings_t  locTimings;
    uint32_t            WpfId = 0u;
    uint32_t            VspdUnitNo = loc_GetVspdUnitNo(loc_vout->VspdConf->VspdUnit);

    if (0 == Timings) {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutTimingSet parameter Timings failed(%d)\r\n", ret);
    } else {
        /* Initialise the Board Videoout HW */
        ret = R_CIO_BSP_WM_PRV_VideoOutInit(loc_vout->DuUnit, (r_wm_DispTiming_t *)Timings);

        if (R_WM_ERR_SUCCESS == ret) {
            /* Get device type */
            dev = R_PRR_GetDevice();
            ret = loc_InstConfigSet(loc_vout, dev, (r_wm_DispTiming_t *)Timings);
            loc_SetupDuTimings(loc_vout->DuUnit, (r_wm_DispTiming_t *) Timings,
                            &locTimings);
            if (R_WM_ERR_SUCCESS == ret) {
                du_err = R_DU_Init(loc_vout->DuUnit, &locTimings, loc_vout->DuPlaneMsk,
                                loc_vout->DuPlaneCfg, dev);
                if (R_DU_ERR_SUCCESS != du_err) {
                    ret = R_WM_ERR_DUINIT_FAILED;
                    R_PRINT_Log("[WMDrvSys]: R_DU_Init failed(%d)\r\n", ret);
                } else {
                    if (R_DU_DU3 == loc_vout->DuUnit) {
                        WpfId = 1u;
                    }

                    /* Init VSP2 */
                    vsp2_err = R_VSP2_Init(loc_vout->VspdConf->VspdUnit,
                                           WpfId,
                                           locTimings.ScreenWidth,
                                           locTimings.ScreenHeight,
                                           0,
                                           &Dpr_Conf[VspdUnitNo],
                                           loc_vout->VspdConf->InitFlag);
                    if (R_VSP2_ERR_SUCCESS == vsp2_err) {
                        if (1u == WpfId) {
                            loc_vout->VspdConf->Wpf1InUse = 1u;
                        } else {
                            loc_vout->VspdConf->Wpf0InUse = 1u;
                        }
                        vsp2_err = R_VSP2_Start(loc_vout->VspdConf->VspdUnit, WpfId);
                    }
                    if (R_VSP2_ERR_SUCCESS != vsp2_err) {
                        ret = R_WM_ERR_VSPDINIT_FAILED;
                        R_PRINT_Log("[WMDrvSys]: R_VSP2_Init or R_VSP2_Start failed(%d)\r\n", ret);
                    } else {
                        loc_vout->VspdConf->InitFlag++;
                    }
                }
            } else {
                R_PRINT_Log("[WMDrvSys]: loc_InstConfigSet failed(%d)\r\n", ret);
            }
        } else {
            R_PRINT_Log("[WMDrvSys]: R_CIO_BSP_WM_PRV_VideoOutInit failed(%d)\r\n", ret);
        }
    }
    return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutEnable (void * Inst, uint32_t Enable)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

    if (0 != loc_vout) {
        if (1 == Enable) {
            R_DU_Enable(loc_vout->DuUnit, R_DU_ENABLE);
        } else if (0 == Enable) {
            R_DU_Enable(loc_vout->DuUnit, R_DU_DISABLE);
        } else {
            ret = R_WM_ERR_WRONG_PARAMETER;
            R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutEnable parameter Enable failed(%d)\r\n", ret);
        }
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutEnable Inst NULL failed(%d)\r\n", ret);
    }

    return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutBkgColSet (void* Inst, uint32_t Color)
{
    r_wm_Error_t        ret        = R_WM_ERR_SUCCESS;
    r_vsp2_Error_t      vsp_ret    = R_VSP2_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout   = (r_wmdrv_Sys_Vout_t *)Inst;
    uint32_t            VspdUnitNo;

    if (0 != loc_vout) {
        VspdUnitNo = loc_GetVspdUnitNo(loc_vout->VspdConf->VspdUnit);
        R_DU_BkgColorSet(loc_vout->DuUnit, Color);
        if (0u == VspdUnitNo) {
            if (R_DU_DU0 == loc_vout->DuUnit) {
                if (R_VSP_DPR_ROUTE_WPF0 == Dpr_Conf[VspdUnitNo].Dpr_Bru_Route) {
                    vsp_ret = R_VSP2_BkgColorSet(loc_vout->VspdConf->VspdUnit, Color);
                    if (R_VSP2_ERR_SUCCESS != vsp_ret) {
                        ret = R_WM_ERR_FAILED;
                        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutBkgColSet R_VSP2_BkgColorSet failed(%d)\r\n", ret);
                    }
                } else if (R_VSP_DPR_ROUTE_BRS_TO_WPF0 == Dpr_Conf[VspdUnitNo].Dpr_Ilv_Brs_Route) {
                    vsp_ret = R_VSP2_BkgColorSet_Brs(loc_vout->VspdConf->VspdUnit, Color);
                    if (R_VSP2_ERR_SUCCESS != vsp_ret) {
                        ret = R_WM_ERR_FAILED;
                        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutBkgColSet R_VSP2_BkgColorSet_Brs failed(%d)\r\n", ret);
                    }
                } else {
                    ret = R_WM_ERR_WRONG_PARAMETER;
                    R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutBkgColSet setting WPF failed(%d)\r\n", ret);
                }
            } else if (R_DU_DU3 == loc_vout->DuUnit) {
                if (R_VSP_DPR_ROUTE_WPF1 == Dpr_Conf[VspdUnitNo].Dpr_Bru_Route) {
                    vsp_ret = R_VSP2_BkgColorSet(loc_vout->VspdConf->VspdUnit, Color);
                    if (R_VSP2_ERR_SUCCESS != vsp_ret) {
                        ret = R_WM_ERR_FAILED;
                        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutBkgColSet R_VSP2_BkgColorSet failed(%d)\r\n", ret);
                    }
                } else if (R_VSP_DPR_ROUTE_BRS_TO_WPF1 == Dpr_Conf[VspdUnitNo].Dpr_Ilv_Brs_Route) {
                    vsp_ret = R_VSP2_BkgColorSet_Brs(loc_vout->VspdConf->VspdUnit, Color);
                    if (R_VSP2_ERR_SUCCESS != vsp_ret) {
                        ret = R_WM_ERR_FAILED;
                        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutBkgColSet R_VSP2_BkgColorSet_Brs failed(%d)\r\n", ret);
                    }
                } else {
                    ret = R_WM_ERR_WRONG_PARAMETER;
                    R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutBkgColSet setting WPF failed(%d)\r\n", ret);
                }
            }
        } else {
            vsp_ret = R_VSP2_BkgColorSet(loc_vout->VspdConf->VspdUnit, Color);
            if (R_VSP2_ERR_SUCCESS != vsp_ret) {
                ret = R_WM_ERR_FAILED;
                R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutBkgColSet R_VSP2_BkgColorSet failed(%d)\r\n", ret);
            }
        }
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutBkgColSet parameter Inst NULL failed(%d)\r\n", ret);
    }

    return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutColFmtSet (void * Inst, uint32_t ColFmt)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
    r_du_Error_t  du_ret = R_DU_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;
    r_du_PlaneFormat_t duFmt;

    if (0 != loc_vout) {
        /* Get DU Color format */
        switch (ColFmt) {
        case R_WM_FMT_ARGB8888:
            duFmt = R_DU_FORMAT_ARGB8888;
            break;
        case R_WM_FMT_RGB888:
            duFmt = R_DU_FORMAT_RGB888;
            break;
        case R_WM_FMT_RGB666:
            duFmt = R_DU_FORMAT_RGB666;
            break;
        case R_WM_FMT_YCBCR444:
            duFmt = R_DU_FORMAT_YCBCR444;
            break;
        default:
            ret = R_WM_ERR_WRONG_PARAMETER;
            duFmt = R_DU_FORMAT_ARGB8888;
            R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutBkgColSet parameter ColFmt failed(%d)\r\n", ret);
            break;
        }
        if (R_WM_ERR_SUCCESS == ret) {
            du_ret = R_DU_ColorFmtSet(loc_vout->DuUnit, loc_vout->DuPlaneMsk, duFmt);
            if (R_DU_ERR_SUCCESS != du_ret) {
                ret = R_WM_ERR_FAILED;
                R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutBkgColSetR_DU_ColorFmtSet failed(%d)\r\n", ret);
            }
        }
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutBkgColSet parameter Inst failed(%d)\r\n", ret);
    }

    return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutSurfaceAlphaSet (void * Inst, uint8_t Alpha, uint32_t rpfid)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
    r_du_Error_t  du_ret = R_DU_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;
    uint32_t RpfId;
    uint32_t reg_val;
    r_vsp2_Error_t      vsp_ret    = R_VSP2_ERR_SUCCESS;
    uint32_t            VspdUnitNo;

    if (0 != loc_vout) {
        vsp_ret = R_VSP2_WinAlphaSet(loc_vout->VspdConf->VspdUnit, Alpha, rpfid);
        if (R_VSP2_ERR_SUCCESS != vsp_ret) {
            ret = R_WM_ERR_FAILED;
            R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutSurfaceAlphaSet R_VSP2_WinAlphaSet failed(%d)\r\n", ret);
            }
        }
        else {
            ret = R_WM_ERR_NULL_POINTER;
            R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutSurfaceAlphaSet parameter Inst failed(%d)\r\n", ret);
        }

    return ret;
}

int32_t R_WMDRV_Sys_VoutUnitGet(void *Inst)
{
    int32_t voutunit;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

    if (0 == loc_vout) {
        voutunit = -1;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutUnitGet parameter Inst voutunit(%d)\r\n", voutunit);
    } else {
        voutunit = loc_vout->DuUnit;
    }
    return voutunit;
}

r_wm_Error_t R_WMDRV_Sys_VoutLayerPosSet(void *Inst, uint32_t PosZ, uint32_t PosX, uint32_t PosY)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

    if (0 != loc_vout) {
        R_VSP2_VSPD_LayerPosXSet(loc_vout->VspdConf->VspdUnit, PosZ, PosX);
        R_VSP2_VSPD_LayerPosYSet(loc_vout->VspdConf->VspdUnit, PosZ, PosY);
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutLayerPosSet parameter Inst failed(%d)\r\n", ret);
    }
    return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutLayerSizeSet(void *Inst, uint32_t PosZ, uint32_t Width, uint32_t Height, uint16_t StrideY, uint16_t StrideC)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

    if (0 != loc_vout) {
        R_VSP2_VSPD_LayerWidthSet(loc_vout->VspdConf->VspdUnit, PosZ, Width, StrideY, StrideC);
        R_VSP2_VSPD_LayerHeightSet(loc_vout->VspdConf->VspdUnit, PosZ, Height);
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutLayerSizeSet parameter Inst failed(%d)\r\n", ret);
    }
    return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutLayerFormatSet(void *Inst, uint32_t PosZ, r_wm_ColorFmt_t Format)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

    if (0 != loc_vout) {
        R_VSP2_VSPD_LayerFormatSet(loc_vout->VspdConf->VspdUnit, PosZ, Format);
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutLayerFormatSet parameter Inst failed(%d)\r\n", ret);
    }
    return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutLayerFbSet(void *Inst, uint32_t PosZ, uintptr_t Fb, uintptr_t C0, uintptr_t C1)
{
    r_wm_Error_t              ret = R_WM_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

    if (0 != loc_vout) {
        if (0 != Fb) {
            if (R_DU_DU3 == loc_vout->DuUnit) {
                /* WPF1 */
                R_VSP2_VSPD_LayerAddrSet(loc_vout->VspdConf->VspdUnit, PosZ, 1, Fb, C0, C1);
            } else {
                /* WPF0 */
                R_VSP2_VSPD_LayerAddrSet(loc_vout->VspdConf->VspdUnit, PosZ, 0, Fb, C0, C1);
            }
            /* We have no other way of knowing when to enable, so when FB written... */
            R_VSP2_VSPD_LayerEnable(loc_vout->VspdConf->VspdUnit, PosZ, true);
        }
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutLayerFbSet parameter Inst failed(%d)\r\n", ret);
    }
    return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutFlush(void * Inst, uint32_t Pid)
{
    r_wm_Error_t        ret = R_WM_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;
    int32_t loc_duUnit;
    uint32_t i;
    r_wmdrv_Surface_t *pSurf;
    uint32_t flush_mask;

    if (0 != loc_vout) {
        /* Look for Surfaces which match the Pid */
        loc_duUnit = loc_vout->DuUnit;
        for (i = 0, flush_mask=0; i < R_WM_NUM_LAYER_PER_VOUT; i++) {
            pSurf = &r_wmdrv_InstSurface[loc_duUnit][i];
            if (Pid == pSurf->ProcessId) {
                flush_mask += (1 << (pSurf->PosZ));
            }
        }
        /*
         * Requesting a flush for this VSPD unit of this PID's layers
         * means that at the next Vsync all the "modified" layers will become
         * active in the VSPD hardware
         */
        R_VSP2_VSPD_Flush(loc_vout->VspdConf->VspdUnit, flush_mask);
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutFlush parameter Inst failed(%d)\r\n", ret);
    }
    return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutLayerGrab(void *Inst, uint32_t PosZ)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;
    r_vsp2_Error_t vsp2_err = R_VSP2_ERR_SUCCESS;

    uint32_t VspdUnitNo;

    if (0 != loc_vout) {
        VspdUnitNo = loc_GetVspdUnitNo(loc_vout->VspdConf->VspdUnit);

        /* request the desired layer */
        if (R_VSP2_VSPD_LayerAllocate(loc_vout->VspdConf->VspdUnit, PosZ, true)) {
            /* enable the blending of the layer */
            switch (PosZ) {
            case 0:
                if ((R_VSP_DPR_ROUTE_BRS_ILV0==Dpr_Conf[VspdUnitNo].Dpr_Rpf0_Route) ||
                    (R_VSP_DPR_ROUTE_BRS_ILV1==Dpr_Conf[VspdUnitNo].Dpr_Rpf0_Route)) {
                    vsp2_err = R_VSP2_BlendingEnable_Brs(loc_vout->VspdConf->VspdUnit, Dpr_Conf[VspdUnitNo].Dpr_Rpf0_Route);
                } else {
                    vsp2_err = R_VSP2_BlendingEnable(loc_vout->VspdConf->VspdUnit, PosZ, 1);
                }
                break;
            case 1:
                if ((R_VSP_DPR_ROUTE_BRS_ILV0==Dpr_Conf[VspdUnitNo].Dpr_Rpf1_Route) ||
                    (R_VSP_DPR_ROUTE_BRS_ILV1==Dpr_Conf[VspdUnitNo].Dpr_Rpf1_Route)) {
                    vsp2_err = R_VSP2_BlendingEnable_Brs(loc_vout->VspdConf->VspdUnit, Dpr_Conf[VspdUnitNo].Dpr_Rpf1_Route);
                } else {
                    vsp2_err = R_VSP2_BlendingEnable(loc_vout->VspdConf->VspdUnit, PosZ, 1);
                }
                break;
            case 2:
                if ((R_VSP_DPR_ROUTE_BRS_ILV0==Dpr_Conf[VspdUnitNo].Dpr_Rpf2_Route) ||
                    (R_VSP_DPR_ROUTE_BRS_ILV1==Dpr_Conf[VspdUnitNo].Dpr_Rpf2_Route)) {
                    vsp2_err = R_VSP2_BlendingEnable_Brs(loc_vout->VspdConf->VspdUnit, Dpr_Conf[VspdUnitNo].Dpr_Rpf2_Route);
                } else {
                    vsp2_err = R_VSP2_BlendingEnable(loc_vout->VspdConf->VspdUnit, PosZ, 1);
                }
                break;
            case 3:
                if ((R_VSP_DPR_ROUTE_BRS_ILV0==Dpr_Conf[VspdUnitNo].Dpr_Rpf3_Route) ||
                    (R_VSP_DPR_ROUTE_BRS_ILV1==Dpr_Conf[VspdUnitNo].Dpr_Rpf3_Route)) {
                    vsp2_err = R_VSP2_BlendingEnable_Brs(loc_vout->VspdConf->VspdUnit, Dpr_Conf[VspdUnitNo].Dpr_Rpf3_Route);
                } else {
                    vsp2_err = R_VSP2_BlendingEnable(loc_vout->VspdConf->VspdUnit, PosZ, 1);
                }
                break;
            case 4:
                if ((R_VSP_DPR_ROUTE_BRS_ILV0==Dpr_Conf[VspdUnitNo].Dpr_Rpf4_Route) ||
                    (R_VSP_DPR_ROUTE_BRS_ILV1==Dpr_Conf[VspdUnitNo].Dpr_Rpf4_Route)) {
                    vsp2_err = R_VSP2_BlendingEnable_Brs(loc_vout->VspdConf->VspdUnit, Dpr_Conf[VspdUnitNo].Dpr_Rpf4_Route);
                } else {
                    vsp2_err = R_VSP2_BlendingEnable(loc_vout->VspdConf->VspdUnit, PosZ, 1);
                }
                break;
            default:
                ret = R_WM_ERR_WRONG_PARAMETER;
                R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutLayerGrab parameter PosZ(%d) failed(%d)\r\n", PosZ, ret);
                break;
            }
            if (R_VSP2_ERR_SUCCESS != vsp2_err) {
                ret = R_WM_ERR_VSPDBLEND_FAILED;
                R_PRINT_Log("[WMDrvSys]: R_VSP2_BlendingEnable failed(%d)\r\n", ret);
            }
        } else {
            ret=R_WM_ERR_NO_SURF;
            R_PRINT_Log("[WMDrvSys]: R_VSP2_VSPD_LayerAllocate request the desired layer failed(%d)\r\n", ret);
        }
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutLayerGrab parameter Inst failed(%d)\r\n", ret);
    }
    return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutLayerRelease(void *Inst, uint32_t PosZ)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
    r_doc_Error_t doc_ret = R_DOC_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

    if (0 != loc_vout) {
        /* disable */
        if (NULL != loc_vout->doc_device_handle)
        {
            doc_ret = R_DOC_DisableMonitors(loc_vout->DocUnit, 0xFFFF);
            if (R_DOC_ERR_SUCCESS != doc_ret) {
                ret = R_WM_ERR_FAILED;
                R_PRINT_Log("[WMDrvSys]: R_DOC_DisableMonitors failed(%d)\r\n", ret);
            }
        }

        R_VSP2_VSPD_LayerEnable(loc_vout->VspdConf->VspdUnit, PosZ, false);
        /* flush the disable */
        R_VSP2_VSPD_Flush(loc_vout->VspdConf->VspdUnit, 1 << PosZ);
        /* deallocate */
        if (false == (R_VSP2_VSPD_LayerAllocate(loc_vout->VspdConf->VspdUnit, PosZ, false))) {
            ret = R_WM_ERR_FAILED;
            R_PRINT_Log("[WMDrvSys]: R_VSP2_VSPD_LayerAllocate failed(%d)\r\n", ret);
        }
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_VoutLayerRelease parameter Inst failed(%d)\r\n", ret);
    }
    return ret;
}


r_wm_Error_t R_WMDRV_Sys_VoutLayerDiscomEnable(void *Inst, uint32_t PosZ, bool Enable)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

    if (0 != loc_vout) {
    	R_VSP2_VSPD_DiscomEnable(loc_vout->VspdConf->VspdUnit, PosZ, Enable);
    } else {
        ret = R_WM_ERR_NULL_POINTER;
    }
    return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutLayerDiscomPosSet(void *Inst, uint32_t PosX, uint32_t PosY)
{
	r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
	r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

	if (0 != loc_vout) {
		R_VSP2_VSPD_DiscomSetPos(loc_vout->VspdConf->VspdUnit, PosX, PosY);
	} else {
		ret = R_WM_ERR_NULL_POINTER;
	}

	return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutLayerDiscomSizeSet(void *Inst, uint32_t Width, uint32_t Height)
{
	r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
	r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

	if (0 != loc_vout) {
		R_VSP2_VSPD_DiscomSetSize(loc_vout->VspdConf->VspdUnit, Width, Height);
	} else {
		ret = R_WM_ERR_NULL_POINTER;
	}

	return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutLayerDiscomCrcSet(void *Inst, uint32_t Crc)
{
	r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
	r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

	if (0 != loc_vout) {
		R_VSP2_VSPD_DiscomSetExpectedCrc(loc_vout->VspdConf->VspdUnit, Crc);
	} else {
		ret = R_WM_ERR_NULL_POINTER;
	}

	return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutLayerDiscomCbSet(void *Inst, r_wm_DiscomCb_t Callback, void *CbArg)
{
	r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
	r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

	if (0 != loc_vout) {
		loc_vout->VspdConf->DiscomCallback = Callback;
		loc_vout->VspdConf->DiscomCbArg = CbArg;
	} else {
		ret = R_WM_ERR_NULL_POINTER;
	}

	return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutLayerDiscomCrcGet(void *Inst, uint32_t *pCrc)
{
	r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
	r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

	if (0 != loc_vout) {
		*pCrc = R_VSP2_VSPD_DiscomGetCrc(loc_vout->VspdConf->VspdUnit);
	} else {
		ret = R_WM_ERR_NULL_POINTER;
	}

	return ret;
}

r_wm_Error_t R_WMDRV_Sys_VoutLayerDiscomClearStatus(void *Inst)
{
	r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
	r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

	if (0 != loc_vout) {
		R_VSP2_VSPD_DiscomClearStatus(loc_vout->VspdConf->VspdUnit);
	} else {
		ret = R_WM_ERR_NULL_POINTER;
	}

	return ret;
}

r_wm_Error_t R_WMDRV_Sys_DocInit(void * Inst, uint32_t DOC_unit, uint32_t DU_Ch, r_wm_Doc_DUParam_t DU_param)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    r_doc_Error_t doc_ret = R_DOC_ERR_SUCCESS;
    r_doc_Du_Params_t doc_du_params;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

    if (0 != loc_vout) {
        loc_vout->DocUnit = DOC_unit;
        DocIrqInfo[DOC_unit].DocUnit = DOC_unit;
        DocIrqInfo[DOC_unit].VideoChannel = DU_Ch;

        doc_du_params.ScreenWidth = DU_param.ScreenWidth;
        doc_du_params.ScreenHeight = DU_param.ScreenHeight;
        doc_du_params.HbackPorchOffset = DU_param.HbackPorchOffset;
        doc_du_params.VbackPorchOffset = DU_param.VbackPorchOffset;

        doc_ret = R_DOC_Init((r_doc_Unit_t)loc_vout->DocUnit, (r_doc_VideoChannel_t)DU_Ch, doc_du_params);
        if (R_DOC_ERR_SUCCESS != doc_ret) {
            ret = R_WM_ERR_FAILED;
            R_PRINT_Log("[WMDrvSys]: R_DOC_Init failed(%d)\r\n", ret);
        }
    }
    return ret;
}

r_wm_Error_t R_WMDRV_Sys_DocDeInit(uint32_t DOC_unit, uint32_t DU_Ch)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    r_doc_Error_t doc_ret = R_DOC_ERR_SUCCESS;

    doc_ret = R_DOC_DeInit(DOC_unit, (r_doc_VideoChannel_t)DU_Ch);
    if (R_DOC_ERR_SUCCESS != doc_ret) {
        ret = R_WM_ERR_FAILED;
        R_PRINT_Log("[WMDrvSys]: R_DOC_DeInit failed(%d)\r\n", ret);
    }
    return ret;
}

r_wm_Error_t R_WMDRV_Sys_DocConfigureMonitor(void * Inst, uint32_t DU_Ch, r_wm_Doc_MonitorParam_t params)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    r_doc_Error_t doc_ret = R_DOC_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;
    r_doc_Region_t region;
    r_doc_Ref_Colors_t colors[4];

    if (0 != loc_vout) {
        doc_ret = R_DOC_SetMonitorChannel((r_doc_Unit_t)loc_vout->DocUnit, (r_doc_VideoChannel_t)DU_Ch, params.vom_num);
        if (R_DOC_ERR_SUCCESS == doc_ret) {
            region.pos.pos_x = params.pos.pos_x;
            region.pos.pos_y = params.pos.pos_y;
            region.rect.height = params.rect.height;
            region.rect.width = params.rect.width;
            doc_ret = R_DOC_SetMonitorRegion((r_doc_Unit_t)loc_vout->DocUnit, params.vom_num, region);
            if (R_DOC_ERR_SUCCESS != doc_ret) {
                R_PRINT_Log("[WMDrvSys]: R_DOC_SetMonitorRegion failed(%d)\r\n", doc_ret);
            }
        } else {
            R_PRINT_Log("[WMDrvSys]: R_DOC_SetMonitorChannel failed(%d)\r\n", doc_ret);
        }

        if (R_DOC_ERR_SUCCESS == doc_ret) {
            int i;
            for (i=0; i<4; i++) {
                colors[i].red.upper = params.color[i].red.upper;
                colors[i].red.lower = params.color[i].red.lower;
                colors[i].green.upper = params.color[i].green.upper;
                colors[i].green.lower = params.color[i].green.lower;
                colors[i].blue.upper = params.color[i].blue.upper;
                colors[i].blue.lower = params.color[i].blue.lower;
            }
            doc_ret = R_DOC_SetMonitorColorInfo((r_doc_Unit_t)loc_vout->DocUnit, params.vom_num, region, params.threshold, colors, params.CLUT);
            if (R_DOC_ERR_SUCCESS != doc_ret) {
                R_PRINT_Log("[WMDrvSys]: R_DOC_SetMonitorColorInfo failed(%d)\r\n", doc_ret);
            }
        }
    }

    if (R_DOC_ERR_SUCCESS != doc_ret) {
        ret = R_WM_ERR_FAILED;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_DocConfigureMonitor failed(%d)\r\n", ret);
    }

    return ret;
}

r_wm_Error_t R_WMDRV_Sys_DocEnable(void *Inst, uint32_t Enable, int VOM_num)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    r_doc_Error_t doc_ret = R_DOC_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

    if (0 != loc_vout) {
        if (Enable) {
            doc_ret = R_DOC_EnableMonitors((r_doc_Unit_t)loc_vout->DocUnit, VOM_num);
            if (R_DOC_ERR_SUCCESS != doc_ret) {
                ret = R_WM_ERR_FAILED;
                R_PRINT_Log("[WMDrvSys]: R_DOC_EnableMonitors failed(%d)\r\n", ret);
            }
        } else {
            doc_ret = R_DOC_DisableMonitors((r_doc_Unit_t)loc_vout->DocUnit, VOM_num);
            if (R_DOC_ERR_SUCCESS != doc_ret) {
                ret = R_WM_ERR_FAILED;
                R_PRINT_Log("[WMDrvSys]: R_DOC_DisableMonitors failed(%d)\r\n", ret);
            }
        }
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_DocEnable parameter Inst failed(%d)\r\n", ret);
    }
    return ret;
}

r_wm_Error_t R_WMDRV_Sys_ActMonEnable(void *Inst, uint32_t DU_Ch, uint32_t Enable, uint32_t upper_time, uint32_t lower_time)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    r_doc_Error_t doc_ret = R_DOC_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

    if (0 != loc_vout) {
        if (Enable) {
            doc_ret = R_DOC_SetActMonitorTiming((r_doc_Unit_t)loc_vout->DocUnit, (r_doc_VideoChannel_t)DU_Ch, upper_time, lower_time);
            if (R_DOC_ERR_SUCCESS != doc_ret) {
                ret = R_WM_ERR_FAILED;
                R_PRINT_Log("[WMDrvSys]: R_DOC_SetActMonitorTiming failed(%d)\r\n", ret);
            }
            if (R_WM_ERR_SUCCESS == ret) {
                doc_ret = R_DOC_EnableActMonitor((r_doc_Unit_t)loc_vout->DocUnit, true);
                if (R_DOC_ERR_SUCCESS != doc_ret) {
                    ret = R_WM_ERR_FAILED;
                    R_PRINT_Log("[WMDrvSys]: R_DOC_EnableActMonitor failed(%d)\r\n", ret);
                }
            }
        } else {
            doc_ret = R_DOC_SetActMonitorTiming((r_doc_Unit_t)loc_vout->DocUnit, (r_doc_VideoChannel_t)DU_Ch, upper_time, lower_time);
            if (R_DOC_ERR_SUCCESS != doc_ret) {
                ret = R_WM_ERR_FAILED;
                R_PRINT_Log("[WMDrvSys]: R_DOC_SetActMonitorTiming failed(%d)\r\n", ret);
            }
            if (R_WM_ERR_SUCCESS == ret) {
                doc_ret = R_DOC_EnableActMonitor((r_doc_Unit_t)loc_vout->DocUnit, false);
                if (R_DOC_ERR_SUCCESS != doc_ret) {
                    ret = R_WM_ERR_FAILED;
                    R_PRINT_Log("[WMDrvSys]: R_DOC_EnableActMonitor failed(%d)\r\n", ret);
                }
            }
        }
    }
    return ret;
}

r_wm_Error_t R_WMDRV_Sys_DocCbSet(void *Inst, r_wm_DocCb_t Callback, void *CbArg)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    r_doc_Error_t doc_ret = R_DOC_ERR_SUCCESS;

    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

    if (0 != loc_vout) {
        DocIrqInfo[loc_vout->DocUnit].DocCallback = Callback;
        DocIrqInfo[loc_vout->DocUnit].DocCbArg = CbArg;

        doc_ret = R_DOC_ClearErrors(loc_vout->DocUnit, 1, 0xFFFF);
        if (R_DOC_ERR_SUCCESS != doc_ret) {
            ret = R_WM_ERR_FAILED;
            R_PRINT_Log("[WMDrvSys]: R_DOC_ClearErrors failed(%d)\r\n", ret);
        } else {
            if (NULL != Callback) {
                doc_ret = R_DOC_EnableIrq(loc_vout->DocUnit, true);
                if (R_DOC_ERR_SUCCESS != doc_ret) {
                    ret = R_WM_ERR_FAILED;
                    R_PRINT_Log("[WMDrvSys]: R_DOC_EnableIrq failed(%d)\r\n", ret);
                }
            }
        }
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_DocCbSet parameter Inst failed(%d)\r\n", ret);
    }
    return ret;
}

r_wm_Error_t R_WMDRV_Sys_DocClearStatus(void *Inst)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    r_doc_Error_t doc_ret = R_DOC_ERR_SUCCESS;

    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;

    if (0 != loc_vout) {
        doc_ret = R_DOC_ClearErrors(loc_vout->DocUnit, 0, 0xFFFF);
        if (R_DOC_ERR_SUCCESS != doc_ret) {
            ret = R_WM_ERR_FAILED;
            R_PRINT_Log("[WMDrvSys]: R_DOC_ClearErrors failed(%d)\r\n", ret);
        }
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_DocClearStatus parameter Inst failed(%d)\r\n", ret);
    }
    return ret;
}

r_wm_Error_t R_WMDRV_Sys_DocGetStatus(void *Inst)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;
    r_doc_Status_t doc_status;
    uint32_t i;

    if (0 != loc_vout) {
        doc_status = R_DOC_GetStatus(loc_vout->DocUnit);
        loc_vout->DocStatus.MonitorStatus = doc_status.activity_monitor;
        loc_vout->DocStatus.ParameterError = doc_status.parameter_error;
        loc_vout->DocStatus.VocMonitorStatus = doc_status.vom_status;

        for (i = 0; i < DOC_MAX_MONITORS; i++) {
            loc_vout->DocStatus.VomError[i] = doc_status.vom_error[i];
        }
        loc_vout->DocStatus.VocSelMon = doc_status.current_monitor;
        loc_vout->DocStatus.ActMonError1 = doc_status.ch1_error;
        loc_vout->DocStatus.ActMonError0 = doc_status.ch0_error;
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvSys]: R_WMDRV_Sys_DocGetStatus parameter Inst failed(%d)\r\n", ret);
    }
    return ret;
}
