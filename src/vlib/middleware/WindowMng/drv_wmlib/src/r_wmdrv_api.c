/*************************************************************************************************************
* WindowMng_wmdrv_api_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include <string.h>
#include "rcar-xos/osal/r_osal.h"
#include "r_wm_common.h"
#include "r_wmdrv_api.h"
#include "r_wmdrv_sys.h"
#include "r_print_api.h"



/* ResourceID */
#define WMDRV_RESOURCE_ID       0x8000U                   /* RESOURCE_ID_BSP   */
#define WMDRV_MUTEX_CREATE_01   (WMDRV_RESOURCE_ID + 26U) /* OSAL_MUTEX_WM1_01 */
#define WMDRV_MUTEX_CREATE_02   (WMDRV_RESOURCE_ID + 27U) /* OSAL_MUTEX_WM1_02 */
#define WMDRV_MUTEX_CREATE_03   (WMDRV_RESOURCE_ID + 28U) /* OSAL_MUTEX_WM1_03 */
#define WMDRV_MUTEX_CREATE_04   (WMDRV_RESOURCE_ID + 29U) /* OSAL_MUTEX_WM1_04 */


/*
 * Global Variables
 */


/**
 * list of the supported WM Vout instances.
 */
r_wmdrv_VOutInstance_t r_wmdrv_Vout[R_WM_DEV_LAST];

/**
 * list of surface per instance
 */
r_wmdrv_Surface_t r_wmdrv_InstSurface[R_WM_DEV_LAST][R_WM_NUM_LAYER_PER_VOUT];


/*
 * Section: global functions
 */

r_wmdrv_VOutInstance_t * R_WMDRV_FindInstance(r_wm_Dev_t WmUnit)
{
    r_wmdrv_VOutInstance_t *inst;

    inst = &r_wmdrv_Vout[WmUnit];

    return inst;

}

int32_t R_WMDRV_AddProcToInstance(r_wm_Dev_t WmUnit)
{
    r_wmdrv_VOutInstance_t *inst;
    int32_t                 ret = 0;
    e_osal_return_t         osal_ret = OSAL_RETURN_OK;

    inst = R_WMDRV_FindInstance(WmUnit);
    if (0 != inst) {
        osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(inst->MutexHandle, 3000);
        if (OSAL_RETURN_OK == osal_ret) {
            if (inst->InUseCnt < R_WMDRV_MAX_THREAD_PER_INSTANCE) {
                inst->InUseCnt++;
            } else {
                ret = -1;
                R_PRINT_Log("[WMDrvApi]: R_WMDRV_AddProcToInstance InUseCnt MAX failed(%d)\r\n", ret);
            }
            osal_ret = R_OSAL_ThsyncMutexUnlock(inst->MutexHandle);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("[WMDrvApi]: R_WMDRV_AddProcToInstance Mutex unlock failed(%d)\r\n", osal_ret);
            }
        }
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_AddProcToInstance Mutex lock failed(%d)\r\n", osal_ret);
            ret = -1;
        }
    } else {
        ret = -1;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_AddProcToInstance inst NULL failed(%d)\r\n", ret);
    }
    return ret;
}

int32_t R_WMDRV_GetProcNbUsingInstance(r_wm_Dev_t WmUnit)
{
    r_wmdrv_VOutInstance_t *inst;
    int32_t                 ret = 0;

    inst = R_WMDRV_FindInstance(WmUnit);

    ret = inst->InUseCnt;

    return ret;
}

/**
 * There must be only ONE calling thread (CIOserver) per display
 */
int R_WMDRV_Init(r_wm_Dev_t WmDev)
{
    int ret = 0;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    if (R_WM_DEV_LAST > WmDev) {
        void *ptr = 0;
        ptr = R_WMDRV_Sys_InstInit(WmDev);
        if (0 != ptr) {
            r_wmdrv_Vout[WmDev].VoutConf = ptr;
            r_wmdrv_Vout[WmDev].BckgColor = 0x00FF0000;
            r_wmdrv_Vout[WmDev].ColorFmt  = R_WM_FMT_ARGB8888;
            r_wmdrv_Vout[WmDev].InUseCnt  = 0;

            switch (WmDev) {
            case 0:
                osal_ret = R_OSAL_ThsyncMutexCreate(WMDRV_MUTEX_CREATE_01, &r_wmdrv_Vout[WmDev].MutexHandle);
                break;
            case 1:
                osal_ret = R_OSAL_ThsyncMutexCreate(WMDRV_MUTEX_CREATE_02, &r_wmdrv_Vout[WmDev].MutexHandle);
                break;
            case 2:
                osal_ret = R_OSAL_ThsyncMutexCreate(WMDRV_MUTEX_CREATE_03, &r_wmdrv_Vout[WmDev].MutexHandle);
                break;
            default: /* 3 */
                osal_ret = R_OSAL_ThsyncMutexCreate(WMDRV_MUTEX_CREATE_04, &r_wmdrv_Vout[WmDev].MutexHandle);
                break;
            }
            if (OSAL_RETURN_OK == osal_ret) {
                ret = osal_ret;
            } else {
                /* failed to create mutex */
                R_PRINT_Log("[WMDrvApi]: WMDRV_Init Mutex create failed(%d), WmDev(%d)\r\n", osal_ret, WmDev);
            }
        } else {
            ret = -1;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_Init VoutConf failed(%d), WmDev(%d)\r\n", ret, WmDev);
        }
    } else {
        ret = -1;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_Init WmDev failed(%d), WmDev(%d)\r\n", ret, WmDev);
    }

    return ret;
}

int R_WMDRV_Deinit(r_wm_Dev_t WmDev)
{
    r_wm_Error_t wm_err = R_WM_ERR_SUCCESS;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    osal_ret = R_OSAL_ThsyncMutexDestroy(r_wmdrv_Vout[WmDev].MutexHandle);
    if (OSAL_RETURN_OK == osal_ret) {
        wm_err = R_WMDRV_Sys_InstDeinit(WmDev);
        if (R_WM_ERR_SUCCESS != wm_err) {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_Deinit R_WMDRV_Sys_InstDeinit failed(%d)\r\n", wm_err);
        }
    } else {
        wm_err = -1;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_Deinit Mutex destroy failed(%d)\r\n", osal_ret);
    }
    return wm_err;
}

r_wm_Error_t R_WMDRV_VoutInit(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;

    if (0 != Inst) {
        if (1 == Inst->InUseCnt) {
            /* First time the instance is being called.
             *  initialisation of the IRQ and required modules.
             *   Proper initialisation of DU and VSPD will be done later
             *    when timings are set.
             */
            ret = R_WMDRV_Sys_VoutInit(Inst->VoutConf, &(Inst->Timings));
            if (R_WM_ERR_SUCCESS != ret) {
                R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutInit R_WMDRV_Sys_VoutInit failed(%d)\r\n", ret);
            }
        }
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutInit parameter Inst NULL failed(%d)\r\n", ret);
    }
    return ret;
}

r_wm_Error_t R_WMDRV_VoutDeinit(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    uint32_t      i;

    if ((0 != InData) && (0 != Inst)) {
        /* ToDo error 3 sec */
        osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(Inst->MutexHandle, 3000);
        if (OSAL_RETURN_OK == osal_ret) {
            if (1 < Inst->InUseCnt) {
                /* Instance still in use, we decrease the count and we look for events
                 * registered by this process and delete them
                 */
                Inst->InUseCnt--;
                osal_ret = R_OSAL_ThsyncMutexUnlock(Inst->MutexHandle);
                if (OSAL_RETURN_OK == osal_ret) {
                    for (i = 0; i < R_WMDRV_MAX_THREAD_PER_INSTANCE; i++) {
                        /* INFO: At the moment limited to 1 event. The loop shall go through all of them */
                        if (InData->Val == Inst->UserEvent[i][R_WM_EVENT_VBLANK].Pid) {
                            Inst->UserEvent[i][R_WM_EVENT_VBLANK].Pid = 0;
                            Inst->UserEvent[i][R_WM_EVENT_VBLANK].MqHandle = 0;
                            break;
                        }
                    }
                    ret = R_WM_ERR_UNIT_USED;
                } else {
                    R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutDeinit Mutex unlock failed(%d)\r\n", osal_ret);
                }
            } else if (1 == Inst->InUseCnt) {
                osal_ret = R_OSAL_ThsyncMutexUnlock(Inst->MutexHandle);
                if (OSAL_RETURN_OK == osal_ret) {
                    ret = R_WMDRV_Sys_VoutDeinit(Inst->VoutConf);
                    if (R_WM_ERR_SUCCESS == ret) {
                        /* clean up the Instance to 0. Inst->MutexHandle not init. */
                        Inst->BckgColor = 0;
                        Inst->ColorFmt = 0;
                        Inst->Enable = 0;
                        Inst->InUseCnt = 0;
                        (void)memset(&Inst->Timings, 0, sizeof(Inst->Timings));
                        (void)memset(&Inst->UserEvent, 0, sizeof(Inst->UserEvent));
                    } else {
                        R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_VoutDeinit failed(%d)\r\n", ret);
                    }
                } else {
                    R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutDeinit Mutex unlock failed(%d)\r\n", osal_ret);
                }
            } else {
                osal_ret = R_OSAL_ThsyncMutexUnlock(Inst->MutexHandle);
                if (OSAL_RETURN_OK != osal_ret) {
                    R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutDeinit Mutex unlock failed(%d)\r\n", osal_ret);
                }
                /* Was not initialised */
                ret = R_WM_ERR_NOT_INITIALIZED;
            }
        }
        if ((R_WM_ERR_SUCCESS == ret) && (OSAL_RETURN_OK != osal_ret)) {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutDeinit Mutex lock failed(%d)\r\n", osal_ret);
            ret = R_WM_ERR_OS_MUTEX;
        }
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutDeinit parameter InData Inst NULL failed(%d)\r\n", ret);
    }
    return ret;
}

r_wm_Error_t R_WMDRV_VoutTimings (r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;

    if (0 != Inst) {
        if (0 != Inst->Timings.ScreenWidth) {
            ret = R_WM_ERR_UNIT_ALREADY_INIT; /* Already Initialised */
        }
        else {
            if (0 != InData) {
                if (0 != InData->Ptr) {
                    /* save timings in current instance structure */
                    (void)memcpy(&(Inst->Timings), InData->Ptr, sizeof(r_wm_DispTiming_t));
                    ret = R_WMDRV_Sys_VoutTimingSet(Inst->VoutConf, &(Inst->Timings));
                } else {
                    ret = R_WM_ERR_NULL_POINTER;
                    R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutTimings parameter InData->Ptr NULL failed(%d)\r\n", ret);
                }
            } else {
                ret = R_WM_ERR_NULL_POINTER;
                R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutTimings parameter InData NULL failed(%d)\r\n", ret);
            }
        }
    } else {
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutTimings parameter Inst NULL failed(%d)\r\n", ret);
    }

    if (((R_WM_ERR_SUCCESS == ret) || ( R_WM_ERR_UNIT_ALREADY_INIT == ret)) && (0 != OutData)) {
        /* copy timing info */
        OutData->DevInfoData.ScreenWidth  = Inst->Timings.ScreenWidth;
        OutData->DevInfoData.ScreenHeight = Inst->Timings.ScreenHeight;
        OutData->DevInfoData.LayerNb      = R_WM_NUM_LAYER_PER_VOUT;
    }
    return ret;
}

r_wm_Error_t R_WMDRV_VoutEnable (r_wmdrv_VOutInstance_t * Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf)){
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutEnable parameter failed(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        ret = R_WMDRV_Sys_VoutEnable(Inst->VoutConf, InData->Val);
        if (R_WM_ERR_SUCCESS != ret) {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_VoutEnable failed(%d)\r\n", ret);
        }
    }

    return ret;
}

r_wm_Error_t R_WMDRV_VoutBkgCol (r_wmdrv_VOutInstance_t * Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf)){
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutBkgCol parameter failed(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        ret = R_WMDRV_Sys_VoutBkgColSet(Inst->VoutConf, InData->Val);
        if (R_WM_ERR_SUCCESS != ret) {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_VoutBkgColSet failed(%d)\r\n", ret);
        }
    }
    return ret;
}

r_wm_Error_t R_WMDRV_VoutColFmt (r_wmdrv_VOutInstance_t * Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf)){
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutColFmt parameter failed(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        ret = R_WMDRV_Sys_VoutColFmtSet(Inst->VoutConf, InData->Val);
        if (R_WM_ERR_SUCCESS != ret) {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_VoutColFmtSet failed(%d)\r\n", ret);
        }
    }
    return ret;
}

r_wm_Error_t R_WMDRV_VoutSurfAlphaSet (r_wmdrv_VOutInstance_t * Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf)){
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutSurfAlphaSet parameter failed(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        ret = R_WMDRV_Sys_VoutSurfaceAlphaSet(Inst->VoutConf, InData->SurfAlphaSetMsgData.Alpha, InData->SurfAlphaSetMsgData.rpfid);
        if (R_WM_ERR_SUCCESS != ret) {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutSurfAlphaSet failed(%d)\r\n", ret);
        }
    }
    return ret;
}

/**
 * This does NOT allocate any layer and exists for compatibility.
 * The actual layer allocation occurs on the first position
 * request to this instance
 */
r_wm_Error_t R_WMDRV_VoutLayerGet(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
    int32_t       vout_unit;
    uint32_t      i;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf) ||
        (0 == OutData)){
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerGet parameter failed(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        vout_unit = R_WMDRV_Sys_VoutUnitGet(Inst->VoutConf);
        if ((0 > vout_unit) || (R_WM_DEV_LAST <= vout_unit)) {
            ret = R_WM_ERR_WRONG_PARAMETER;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerGet vout_unit failed(%d)\r\n", ret);
        }
    }
    if (R_WM_ERR_SUCCESS == ret) {
        for (i = 0; i < R_WM_NUM_LAYER_PER_VOUT; i ++) {
            /* Get the next free data record */
            if (0 == r_wmdrv_InstSurface[vout_unit][i].ProcessId) {
                r_wmdrv_InstSurface[vout_unit][i].ProcessId = InData->Val;
                r_wmdrv_InstSurface[vout_unit][i].PosZ = SURFACE_UNASSIGNED;
                OutData->Ptr = (void *)(&(r_wmdrv_InstSurface[vout_unit][i]));
                break;
            }
        }
        if (R_WM_NUM_LAYER_PER_VOUT == i) {
            /* No surface available */
            ret = R_WM_ERR_NO_SURF;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerGet No surface available(%d)\r\n", ret);
        }
    }
    return ret;
}

r_wm_Error_t R_WMDRV_VoutLayerRelease(r_wmdrv_VOutInstance_t * Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t             ret = R_WM_ERR_SUCCESS;
    r_wm_Msg_Surf_Release_t const * pMsgData;
    r_wmdrv_Surface_t       *surf;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf)){
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerRelease parameter failed(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        pMsgData = &InData->SurfReleaseMsgData;
        if (0 == pMsgData->Handle) {
            ret = R_WM_ERR_NULL_POINTER;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerRelease pMsgData->Handle NULL failed(%d)\r\n", ret);
        } else {
            surf = (r_wmdrv_Surface_t *) pMsgData->Handle;
        }
    }

    if ((R_WM_ERR_SUCCESS == ret) &&
        (surf->ProcessId != pMsgData->Pid)) {
        /* Thread not allowed to access this Surface. */
        ret = R_WM_ERR_WRONG_PID;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerRelease ProcessId failed(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        /* stop using the surface and free the record */
        ret = R_WMDRV_Sys_VoutLayerRelease(Inst->VoutConf, surf->PosZ);
        if (R_WM_ERR_SUCCESS == ret) {
            surf->ProcessId = 0;
            surf->PosZ = SURFACE_UNASSIGNED;
        } else {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_VoutLayerRelease failed(%d)\r\n", ret);
        }
    }
    return ret;
}

r_wm_Error_t R_WMDRV_VoutLayerPos(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
    r_wm_Msg_Surf_Pos_t const * pMsgData;
    r_wmdrv_Surface_t *surf;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf)){
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerPos parameter failed(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        pMsgData = &InData->SurfPosSetMsgData;
        if (0 == pMsgData->Handle) {
            ret = R_WM_ERR_NULL_POINTER;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerPos pMsgData->Handle NULL failed(%d)\r\n", ret);
        } else {
            surf = (r_wmdrv_Surface_t *) pMsgData->Handle;
        }
    }

    if ((R_WM_ERR_SUCCESS == ret) &&
        (surf->ProcessId != pMsgData->Pid)) {
        /* Thread not allowed to access this Surface. */
        ret = R_WM_ERR_WRONG_PID;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerPos ProcessId failed(%d)\r\n", ret);
    }

    /* THIS is where we really request/allocate the VSPD layer */
    if ((R_WM_ERR_SUCCESS == ret) &&
        (SURFACE_UNASSIGNED == surf->PosZ)) {
        if (R_WM_ERR_SUCCESS == R_WMDRV_Sys_VoutLayerGrab(Inst->VoutConf, pMsgData->PosZ)) {
            surf->PosZ = pMsgData->PosZ;
            /* For backward compatiblity we may never be setting layer format...so set default here */
            ret = R_WMDRV_Sys_VoutLayerFormatSet(Inst->VoutConf, pMsgData->PosZ, R_WM_FMT_ARGB8888);
            if (R_WM_ERR_SUCCESS != ret) {
                R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerPos R_WMDRV_Sys_VoutLayerFormatSet failed(%d)\r\n", ret);
            }
        } else {
            /* we can't grab the requested layer */
            ret = R_WM_ERR_INVALID_PROP;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerPos grab the requested layer failed(%d)\r\n", ret);
        }
    }

    if ((R_WM_ERR_SUCCESS == ret) &&
        (surf->PosZ != pMsgData->PosZ)) {
            /* we aren't controlling the requested layer */
            ret = R_WM_ERR_INVALID_PROP;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerPos grab the requested layer failed(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        ret = R_WMDRV_Sys_VoutLayerPosSet(Inst->VoutConf, surf->PosZ,
                                          pMsgData->PosX,
                                          pMsgData->PosY);
        if (R_WM_ERR_SUCCESS != ret) {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_VoutLayerPosSet failed(%d)\r\n", ret);
        }
    }

    return ret;
}

r_wm_Error_t R_WMDRV_VoutLayerSize(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
    r_wm_Msg_Surf_Size_t const * pMsgData;
    r_wmdrv_Surface_t *surf;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf)){
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerSize parameter failed(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        pMsgData = &InData->SurfSizeSetMsgData;
        if (0 == pMsgData->Handle) {
            ret = R_WM_ERR_NULL_POINTER;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerSize pMsgData->Handle NULL failed(%d)\r\n", ret);
        } else {
            surf = (r_wmdrv_Surface_t *) pMsgData->Handle;
        }
    }

    if ((R_WM_ERR_SUCCESS == ret) &&
        (surf->ProcessId != pMsgData->Pid)) {
        /* Thread not allowed to access this Surface. */
        ret = R_WM_ERR_WRONG_PID;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerSize ProcessId failed(%d)\r\n", ret);
    }

    if ((R_WM_ERR_SUCCESS == ret) &&
        (SURFACE_UNASSIGNED == surf->PosZ)) {
        /* no surface assigned */
        ret = R_WM_ERR_NO_SURF;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerSize no surface assigned(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        ret = R_WMDRV_Sys_VoutLayerSizeSet(Inst->VoutConf, surf->PosZ,
                                           pMsgData->Width, pMsgData->Height,
                                           pMsgData->StrideY, pMsgData->StrideC);
        if (R_WM_ERR_SUCCESS != ret) {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_VoutLayerSizeSet failed(%d)\r\n", ret);
        }
    }
    return ret;
}

r_wm_Error_t R_WMDRV_VoutLayerFormat(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t  ret = R_WM_ERR_SUCCESS;
    r_wm_Msg_Surf_Format_t const * pMsgData;
    r_wmdrv_Surface_t *surf;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf)){
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerFormat parameter failed(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        pMsgData = &InData->SurfFormatSetMsgData;
        if (0 == pMsgData->Handle) {
            ret = R_WM_ERR_NULL_POINTER;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerFormat pMsgData->Handle NULL failed(%d)\r\n", ret);
        } else {
            surf = (r_wmdrv_Surface_t *) pMsgData->Handle;
        }
    }

    if ((R_WM_ERR_SUCCESS == ret) &&
        (surf->ProcessId != pMsgData->Pid)) {
        /* Thread not allowed to access this Surface. */
        ret = R_WM_ERR_WRONG_PID;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerFormat ProcessId failed(%d)\r\n", ret);
    }

    if ((R_WM_ERR_SUCCESS == ret) &&
        (SURFACE_UNASSIGNED == surf->PosZ)) {
        /* no surface assigned */
        ret = R_WM_ERR_NO_SURF;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerFormat no surface assigned(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        ret = R_WMDRV_Sys_VoutLayerFormatSet(Inst->VoutConf, surf->PosZ,
                                           pMsgData->Format);
        if (R_WM_ERR_SUCCESS != ret) {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_VoutLayerFormatSet failed(%d)\r\n", ret);
        }
    }
    return ret;
}

r_wm_Error_t R_WMDRV_VoutLayerFb(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t              ret = R_WM_ERR_SUCCESS;
    r_wm_Msg_Surf_Buf_t const * pMsgData;
    r_wmdrv_Surface_t     * surf;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf)){
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerFb parameter failed(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        pMsgData = &InData->SurfBufSetMsgData;
        if (0 == pMsgData->Handle) {
            ret = R_WM_ERR_NULL_POINTER;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerFb pMsgData->Handle NULL failed(%d)\r\n", ret);
        } else {
            surf = (r_wmdrv_Surface_t *) pMsgData->Handle;
        }
    }

    if ((R_WM_ERR_SUCCESS == ret) &&
        (surf->ProcessId != pMsgData->Pid)) {
        /* Thread not allowed to access this Surface. */
        ret = R_WM_ERR_WRONG_PID;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerFb ProcessId failed(%d)\r\n", ret);
    }

    if ((R_WM_ERR_SUCCESS == ret) &&
        (SURFACE_UNASSIGNED == surf->PosZ)) {
        /* no surface assigned */
        ret = R_WM_ERR_NO_SURF;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutLayerFb no surface assigned(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        ret = R_WMDRV_Sys_VoutLayerFbSet(Inst->VoutConf, surf->PosZ,
                                         pMsgData->Fb, pMsgData->C0, pMsgData->C1);
        if (R_WM_ERR_SUCCESS != ret) {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_VoutLayerFbSet failed(%d)\r\n", ret);
        }
    }
    return ret;
}

r_wm_Error_t R_WMDRV_VoutFlush(r_wmdrv_VOutInstance_t * Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    r_wm_Msg_ScreenFlush_t const * pMsgData;
    uint32_t i = 0;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf)){
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutFlush parameter failed(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        pMsgData = &InData->ScreenFlushData;
        ret = R_WMDRV_Sys_VoutFlush(Inst->VoutConf, pMsgData->Pid);
        if (R_WM_ERR_SUCCESS == ret) {
            if (0 != pMsgData->MqHandle) {
                /* register a semaphore as we'll be waiting
                   We are doing this AFTER the flush...ISR will flush
                   all active semaphores without interlocks. The worst
                   case timing is we may delay a full vsync IF we've just
                   flushed immediately prior to ISR...but should be no hazards */
                osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(Inst->MutexHandle, 3000);
                if (OSAL_RETURN_OK == osal_ret) {
                    for (i = 0; i < R_WMDRV_MAX_THREAD_PER_INSTANCE; i++) {
                        if (0 == Inst->UserEvent[i][R_WM_EVENT_FLUSHSYNC].MqHandle) {
                            Inst->UserEvent[i][R_WM_EVENT_FLUSHSYNC].Pid = pMsgData->Pid;
                            Inst->UserEvent[i][R_WM_EVENT_FLUSHSYNC].MqHandle = pMsgData->MqHandle;
                            break;
                        }
                    }
                    osal_ret = R_OSAL_ThsyncMutexUnlock(Inst->MutexHandle);
                    if (OSAL_RETURN_OK != osal_ret) {
                        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutFlush Mutex unlock failed(%d)\r\n", osal_ret);
                    }
                } else {
                    R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutFlush Mutex lock failed(%d)\r\n", osal_ret);
                }
                if (R_WMDRV_MAX_THREAD_PER_INSTANCE <= i) {
                    ret = R_WM_ERR_TOO_MANY_THREADS;
                    R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutFlush too many theads failed(%d)\r\n", ret);
                }
            }
        } else {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutFlush R_WMDRV_Sys_VoutFlush failed(%d)\r\n", ret);
        }

    }

    return ret;
}

r_wm_Error_t R_WMDRV_VoutRegisterEvent(r_wmdrv_VOutInstance_t * Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    r_wm_Msg_Event_t const * pMsgData;
    uint32_t i = 0;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf)){
        ret = R_WM_ERR_NULL_POINTER;
        R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutRegisterEvent parameter failed(%d)\r\n", ret);
    }

    if (R_WM_ERR_SUCCESS == ret) {
        pMsgData = &InData->EventRegMsgData;
        if (0 == pMsgData->MqHandle) {
            ret = R_WM_ERR_NULL_POINTER;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutRegisterEvent pMsgData->MqHandle NULL failed(%d)\r\n", ret);
        } else if (R_WM_EVENT_LAST <= pMsgData->EvId) {
            ret = R_WM_ERR_WRONG_PARAMETER;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutRegisterEvent pMsgData->EvId failed(%d)\r\n", ret);
        } else {
            /* Do nothing */
        }
    }

    if (R_WM_ERR_SUCCESS == ret) {
        /* find a home for the semaphore */
        osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(Inst->MutexHandle, 3000);
        if (OSAL_RETURN_OK == osal_ret) {
            for (i = 0; i < R_WMDRV_MAX_THREAD_PER_INSTANCE; i++) {
                if (0 == Inst->UserEvent[i][pMsgData->EvId].MqHandle) {
                    Inst->UserEvent[i][pMsgData->EvId].Pid = pMsgData->Pid;
                    Inst->UserEvent[i][pMsgData->EvId].MqHandle  = pMsgData->MqHandle;
                    break;
                }
            }
            osal_ret = R_OSAL_ThsyncMutexUnlock(Inst->MutexHandle);
            if (OSAL_RETURN_OK != osal_ret) {
                R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutRegisterEvent Mutex unlock failed(%d)\r\n", osal_ret);
            }
        } else {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutRegisterEvent Mutex lock failed(%d)\r\n", osal_ret);
        }
        if (R_WMDRV_MAX_THREAD_PER_INSTANCE <= i) {
            ret = R_WM_ERR_TOO_MANY_THREADS;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_VoutRegisterEvent too many theads failed(%d)\r\n", osal_ret);
        }
    }

    return ret;
}

r_wm_Error_t R_WMDRV_VoutLayerDiscomConfigure(r_wmdrv_VOutInstance_t *Inst,
                r_wm_Msg_Generic_t const * const InData, r_wm_Msg_Generic_t * const OutData)
{
    r_wm_Error_t      ret = R_WM_ERR_SUCCESS;
    r_wm_Msg_DiscomInfo_t const *const pMsgData = &InData->DiscomMsgData;
    r_wmdrv_Surface_t *surf;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == pMsgData->Handle) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf)){
        ret = R_WM_ERR_NULL_POINTER;
    } else {
        surf = (r_wmdrv_Surface_t *) pMsgData->Handle;
    }

    if ((R_WM_ERR_SUCCESS == ret) &&
        (surf->ProcessId != pMsgData->Pid)) {
        /* Thread not allowed to access this Surface. */
        ret = R_WM_ERR_WRONG_PID;
    }

    if ((R_WM_ERR_SUCCESS == ret) &&
        (SURFACE_UNASSIGNED == surf->PosZ)) {
        /* no surface assigned */
        ret = R_WM_ERR_NO_SURF;
    }

    if (R_WM_ERR_SUCCESS == ret) {
        ret = R_WMDRV_Sys_VoutLayerDiscomEnable(Inst->VoutConf, surf->PosZ, pMsgData->Enable);
        ret |= R_WMDRV_Sys_VoutLayerDiscomPosSet(Inst->VoutConf, pMsgData->PosX, pMsgData->PosY);
        ret |= R_WMDRV_Sys_VoutLayerDiscomSizeSet(Inst->VoutConf, pMsgData->Width, pMsgData->Height);
        ret |= R_WMDRV_Sys_VoutLayerDiscomCrcSet(Inst->VoutConf, pMsgData->Crc);
        ret |= R_WMDRV_Sys_VoutLayerDiscomCbSet(Inst->VoutConf, pMsgData->Callback, pMsgData->CbArg);
    }

    return ret;
}

r_wm_Error_t R_WMDRV_DocConfigure(r_wmdrv_VOutInstance_t *Inst,
                const r_wm_Msg_Generic_t * InData, r_wm_Msg_Generic_t * OutData)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    r_wm_Msg_DocInfo_t * DocMsg;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf)){
            ret = R_WM_ERR_NULL_POINTER;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_DocConfigure parameter failed(%d)\r\n", ret);
        }

    if (R_WM_ERR_SUCCESS == ret){
        DocMsg = (r_wm_Msg_DocInfo_t *) &(InData->DocMsgData);
        if (1 == DocMsg->Enable) {
            ret = R_WMDRV_Sys_DocInit(Inst->VoutConf, DocMsg->DOC_CH, DocMsg->DU_CH, DocMsg->DU_param);

            if (R_WM_ERR_SUCCESS == ret) {
                uint32_t i;
                for (i = 0; i < DOC_MAX_MONITORS; i++) {
                    if (DocMsg->DocMonitors[i].in_use) {
                        ret = R_WMDRV_Sys_DocConfigureMonitor(Inst->VoutConf, DocMsg->DU_CH, DocMsg->DocMonitors[i]);
                    }
                }
            }

            if (R_WM_ERR_SUCCESS == ret) {
                ret = R_WMDRV_Sys_DocCbSet(Inst->VoutConf, DocMsg->Callback, DocMsg->CbArg);
                if (R_WM_ERR_SUCCESS != ret) {
                    R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_DocCbSet failed(%d)\r\n", ret);
                }
            } else {
                R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_DocConfigureMonitor failed(%d)\r\n", ret);
            }
        } else {
            ret = R_WMDRV_Sys_DocDeInit(DocMsg->DOC_CH, DocMsg->DU_CH);
            if (R_WM_ERR_SUCCESS != ret) {
                R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_DocDeInit failed(%d)\r\n", ret);
            }
        }
    }

    return ret;
}

r_wm_Error_t R_WMDRV_DocEnable(r_wmdrv_VOutInstance_t *Inst,
                const r_wm_Msg_Generic_t * InData, r_wm_Msg_Generic_t * OutData)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;
    r_wm_Msg_DocInfo_t * DocMsg;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf)){
            ret = R_WM_ERR_NULL_POINTER;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_DocEnable parameter failed(%d)\r\n", ret);
        }

    if (R_WM_ERR_SUCCESS == ret) {
        uint32_t i;
        uint32_t on_list = 0;
        uint32_t off_list = 0;

        DocMsg = (r_wm_Msg_DocInfo_t *) &(InData->DocMsgData);

        for (i = 0; i < DOC_MAX_MONITORS; i++){
            if (DocMsg->DocMonitors[i].in_use) {
                if (DocMsg->DocMonitors[i].Enable) {
                    on_list |= (1 << i);
                } else {
                    off_list |= (1 << i);
                }
            }
        }

        ret = R_WMDRV_Sys_DocEnable(Inst->VoutConf, true, on_list);
        ret |= R_WMDRV_Sys_DocEnable(Inst->VoutConf, false, off_list);

        if (R_WM_ERR_SUCCESS == ret) {
            if (true == DocMsg->ActMonEnable) {
                ret = R_WMDRV_Sys_ActMonEnable(Inst->VoutConf, DocMsg->DU_CH, true, DocMsg->ActMonUpperTime, DocMsg->ActMonLowerTime);
                if (R_WM_ERR_SUCCESS != ret) {
                    R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_ActMonEnable failed(%d)\r\n", ret);
                }
            } else {
                ret = R_WMDRV_Sys_ActMonEnable(Inst->VoutConf, DocMsg->DU_CH, false, DocMsg->ActMonUpperTime, DocMsg->ActMonLowerTime);
                if (R_WM_ERR_SUCCESS != ret) {
                    R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_ActMonEnable failed(%d)\r\n", ret);
                }
            }
        } else {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_DocEnable failed(%d)\r\n", ret);
        }
    }

    return ret;
}

r_wm_Error_t R_WMDRV_DocClearStatus(r_wmdrv_VOutInstance_t *Inst,
                const r_wm_Msg_Generic_t * InData, r_wm_Msg_Generic_t * OutData)
{
    r_wm_Error_t ret = R_WM_ERR_SUCCESS;

    /* Do basic validation check */
    if ((0 == InData) ||
        (0 == Inst) ||
        (0 == Inst->VoutConf)){
            ret = R_WM_ERR_NULL_POINTER;
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_DocClearStatus parameter failed(%d)\r\n", ret);
        }

    if (R_WM_ERR_SUCCESS == ret) {
        ret = R_WMDRV_Sys_DocClearStatus(Inst->VoutConf);
        if (R_WM_ERR_SUCCESS != ret) {
            R_PRINT_Log("[WMDrvApi]: R_WMDRV_Sys_DocClearStatus failed(%d)\r\n", ret);
        }
    }

    return ret;
}
