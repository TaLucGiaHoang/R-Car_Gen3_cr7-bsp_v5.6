/*************************************************************************************************************
* WindowMng_wmdrv_sys_irq_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_WMDRV_SYS_IRQ_H_
#define R_WMDRV_SYS_IRQ_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rcar-xos/osal/r_osal.h"

/*
 * Section Types
 */
#define R_WM_DOC_UNIT_NUM 2

/**
 * VSPD Info
 */
typedef struct {
    r_vsp2_Unit_t       VspdUnit;
    uint32_t            Wpf0InUse;
    uint32_t            Wpf1InUse;
    volatile int        IrqVspdQuit;
    volatile int        IrqVspdDiscomQuit;
    uint32_t            IrqVspdThread;
    uint32_t            IrqVspdDiscomThread;
    uint32_t            IrqVspdNum;
    uint32_t            IrqVspdDiscomNum;
    osal_mutex_handle_t SemaVspd;
    osal_mutex_handle_t SemaVspdDiscom;
    r_wm_DiscomCb_t     DiscomCallback;
    uint32_t            InitFlag;
    void                *DiscomCbArg;
    osal_device_handle_t device_handle;
    osal_device_handle_t discom_device_handle;
} r_wmdrv_Sys_VspdInfo_t;

/**
 * DU Info
 */
typedef struct {
    r_du_Unit_t         DuUnit;
    uint32_t            DuPlaneMsk;
    r_du_Plane_t        DuPlaneCfg[2];
    uint32_t            IrqDuNum;
    r_wmdrv_Sys_VspdInfo_t *VspdConf;
    uint32_t            DuInUse;
    uint32_t            IrqDocThread;
    osal_device_handle_t doc_device_handle;
    uint32_t            DocUnit;
    r_wm_DOC_Status_t   DocStatus;
} r_wmdrv_Sys_Vout_t;

/**
 * DOC Info
 */
typedef struct {
    uint32_t            DocUnit;
    uint32_t            VideoChannel;
    uint32_t            EnableCnt;
    r_wm_DocCb_t        DocCallback;
    void                *DocCbArg;
} r_wmdrv_Sys_DocInfo_t;

/*
 * Section: Global Functions
 */

/**
 * Initialise the IRQ for the given WM device/instance
 *
 * Parameters:
 *  Inst    - CIO WM instance
 *
 * Return
 *  int32_t - 0 on succes, -1 if failed
 */
int32_t R_WMDRV_Sys_IrqInit(void *Inst);

/**
 * Initialise the DOC IRQ for the given WM device/instance
 *
 * Parameters:
 *  Inst    - CIO WM instance
 *
 * Return
 *  int32_t - 0 on succes, -1 if failed
 */
int32_t R_WMDRV_Sys_DocIrqInit(void *Inst);

/**
 *  De-initialise the IRQ for the given WM device/instance
 *
 * Parameters:
 *  Inst       - CIO WM instance
 *  VspdDeinit - VSPD deinit flag
 *
 * Return
 *  none
 */
void R_WMDRV_Sys_IrqDeinit(void *Inst, uint32_t VspdDeinit);

/**
 *  De-initialise the DOC IRQ for the given WM device/instance
 *
 * Parameters:
 *  Inst       - CIO WM instance
 *
 * Return
 *  none
 */
void R_WMDRV_Sys_DocIrqDeinit(void *Inst);

#ifdef __cplusplus
}
#endif

#endif /* R_WMDRV_SYS_IRQ_H_ */
