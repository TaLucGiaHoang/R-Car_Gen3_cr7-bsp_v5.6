/*************************************************************************************************************
* WindowMng_wmdrv_sys_irq_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "rcar-xos/osal/r_osal.h"
#include "r_du_api.h"
#include "r_vsp2_api.h"
#include "r_doc_api.h"
#include "r_wmdrv_sys.h"
#include "r_wmdrv_sys_irq.h"
#include "r_prr_api.h"
#include "r_print_api.h"


/*
 * Section: Local Defines
 */
#define R_WM_DOC_MAX_INSTANCE_NUM (2)


/**
 * DOC IRQ Number for xOS OSAL
 */
#define WM_DOC_IRQCHANNEL (0)
#define WM_ACT_IRQCHANNEL (1)

#define WM_DISCOM_IRQCHANNEL (1)

/**
 * WM MQ message size
 */
#define WM_MQ_MSG_SIZE      sizeof(uint32_t)

/**
 * Section: Global Variables
 */
osal_device_handle_t DocDevHandle[R_WM_DOC_MAX_INSTANCE_NUM];
r_wmdrv_Sys_DocInfo_t DocIrqInfo[R_WM_DOC_UNIT_NUM] = {{0, 0, 0, NULL, NULL}, {0, 0, 0, NULL, NULL}};

/*
 * Section: local functions
 */

static void *loc_WMVspdIrqThread(osal_device_handle_t device_handle, uint64_t irq_channel, void* irq_arg);
static void *loc_WMDocDocIrqThread(osal_device_handle_t device_handle, uint64_t irq_channel, void* irq_arg);
static void *loc_WMDocActIrqThread(osal_device_handle_t device_handle, uint64_t irq_channel, void* irq_arg);
static void loc_WmVspSemHandler(const r_vsp2_Unit_t VspUnit, const uint32_t WpfId);


static void *loc_WMDocDocIrqThread(osal_device_handle_t device_handle, uint64_t irq_channel, void* irq_arg)
{
    r_wmdrv_Sys_DocInfo_t *loc_docinfo;
    r_wm_DOC_Status_t doc_status;
    uint32_t status0 = 0;
    uint32_t status1 = 0;
    uint32_t work_status = 0;
    uint32_t i = 0;

    loc_docinfo = (r_wmdrv_Sys_DocInfo_t *)irq_arg;

    R_DOC_ClearDocIntStatus(loc_docinfo->DocUnit, &status0, &status1);

    /* DU0 Video Channel 0 */
    if (0 == loc_docinfo->VideoChannel) {
        work_status = status0;
    /* DU1 Video Channel 1 */
    } else {
        work_status = status1;
    }

    doc_status.MonitorStatus = (work_status & (1 << 24)) >> 24;
    doc_status.ParameterError = (work_status & (1 << 23)) >> 23;
    doc_status.VocMonitorStatus = (work_status & (1 << 22)) >> 22;
    doc_status.VocSelMon = (work_status & 0x3C0000) >> 18;
    doc_status.ActMonError1 = (work_status & (1 << 17)) >> 17;
    doc_status.ActMonError0 = (work_status & (1 << 16)) >> 16;

    for(i = 0; i < 16; i++) {
        doc_status.VomError[i] = (work_status & (1 << i)) >> i;
    }

    if (loc_docinfo->DocCallback) {
        loc_docinfo->DocCallback(loc_docinfo->DocCbArg, doc_status);
    }
    return 0;
}

static void *loc_WMDocActIrqThread(osal_device_handle_t device_handle, uint64_t irq_channel, void* irq_arg)
{
    r_wmdrv_Sys_DocInfo_t *loc_docinfo;
    r_wm_DOC_Status_t doc_status;
    uint32_t status = 0u;
    uint32_t i = 0;

    loc_docinfo = (r_wmdrv_Sys_DocInfo_t *)irq_arg;

    R_DOC_ClearActIntStatus(loc_docinfo->DocUnit, &status);

    doc_status.MonitorStatus = (status & (1 << 24)) >> 24;
    doc_status.ParameterError = (status & (1 << 23)) >> 23;
    doc_status.VocMonitorStatus = (status & (1 << 22)) >> 23;
    doc_status.VocSelMon = ((status & 0x3C0000)) >> 18;
    doc_status.ActMonError0 = (status & (1 << 17)) >> 17;
    doc_status.ActMonError0 = (status & (1 << 16))  >> 16;
    for(i = 0; i < 16; i++) {
        doc_status.VomError[i] = (status & (1 << i))  >> i;
    }
    if (loc_docinfo->DocCallback) {
        loc_docinfo->DocCallback(loc_docinfo->DocCbArg, doc_status);
    }
    return 0;
}

static void *loc_WMVspdDiscomIrqThread(osal_device_handle_t device_handle, uint64_t irq_channel, void* irq_arg)
{
    r_wmdrv_Sys_Vout_t *loc_vout;
    uint32_t pCrc = 0;

    loc_vout = (r_wmdrv_Sys_Vout_t *)irq_arg;
    R_WMDRV_Sys_VoutLayerDiscomClearStatus(irq_arg);

    if (loc_vout->VspdConf->DiscomCallback) {
        R_WMDRV_Sys_VoutLayerDiscomCrcGet(loc_vout, &pCrc);
        loc_vout->VspdConf->DiscomCallback(loc_vout->VspdConf->DiscomCbArg, pCrc);
    }
    return 0;
}

/**
 * Handle the VSP semaphore.
 * This function is called by the VSP ISR
 *
 * @param[in] VspUnit    - reference to the VSP unit
 * @param[in] WpfId      - reference to the VSP WPF  unit
 */
static void loc_WmVspSemHandler(const r_vsp2_Unit_t VspUnit, const uint32_t WpfId)
{
    r_wm_Dev_t wm_dev = R_WM_DEV_VOUT0;
    uint32_t j;
    r_wmdrv_Sys_Vout_t * loc_vout;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    uint32_t cmd = 0;
    bool result;
    r_prr_DevName_t dev    = R_PRR_GetDevice();

    /* Could be a bit more generic with a forloop and testing if the WMDev
     * match the VSPDunit and release the sem
     * But we are trying to speed up the IRQ and make it simpler.
     * And the current RCar mapping is fixed so we can assume a lot.
     * And this is rcar system dependant code...
     */
    switch (VspUnit) {
    case R_VSP2_VSPD0:
        if (1 == WpfId) {
	    if (R_PRR_RCARH3 != dev && R_PRR_RCARM3N != dev) {
		// Error, this should not happen
		while(1){}
	    }
	    if (R_PRR_RCARH3 == dev) {
		wm_dev = R_WM_DEV_VOUT3;
	    } else {
		wm_dev = R_WM_DEV_VOUT2;
            }
	    loc_vout = (r_wmdrv_Sys_Vout_t *) r_wmdrv_Vout[wm_dev].VoutConf;
	    if (R_VSP2_VSPD0 != loc_vout->VspdConf->VspdUnit) {
		// Error, this should not happen
		while(1){}
	    }
        } else if (0 == WpfId) {
            wm_dev = R_WM_DEV_VOUT0;
        }
        break;
    case R_VSP2_VSPD1:
        wm_dev = R_WM_DEV_VOUT1;
        break;
    case R_VSP2_VSPD2:
        wm_dev = R_WM_DEV_VOUT2;
        break;
    default:
        return;
    }
    loc_vout = (r_wmdrv_Sys_Vout_t *) r_wmdrv_Vout[wm_dev].VoutConf;
    /* release the wmdev related Vsync sema*/
    if (0 != r_wmdrv_Vout[wm_dev].InUseCnt) {
        /* release the user wmdev sema */
        for (j = 0; j < R_WMDRV_MAX_THREAD_PER_INSTANCE; j++) {
            if (0 != r_wmdrv_Vout[wm_dev].UserEvent[j][R_WM_EVENT_VBLANK].MqHandle) {
                /* If there are messages left in the queue, do not send new messages. */
                osal_ret = R_OSAL_MqIsEmpty(r_wmdrv_Vout[wm_dev].UserEvent[j][R_WM_EVENT_VBLANK].MqHandle, &result);
                if(OSAL_RETURN_OK != osal_ret) {
                    R_PRINT_Log("WMSYS(%d):VBLANK MqIsEmpty=%d\r\n", wm_dev, osal_ret);
                }
                if( true == result ) {
                    osal_ret = R_OSAL_MqSendForTimePeriod(r_wmdrv_Vout[wm_dev].UserEvent[j][R_WM_EVENT_VBLANK].MqHandle,
                                                            1000u, &cmd, WM_MQ_MSG_SIZE);
                    if(OSAL_RETURN_OK != osal_ret) {
                        R_PRINT_Log("WMSYS(%d):VBLANK MqSend=%d\r\n",wm_dev,osal_ret);
                    }
                }
            }
            if (0 != r_wmdrv_Vout[wm_dev].UserEvent[j][R_WM_EVENT_FLUSHSYNC].MqHandle) {
                osal_ret = R_OSAL_MqSendForTimePeriod(r_wmdrv_Vout[wm_dev].UserEvent[j][R_WM_EVENT_FLUSHSYNC].MqHandle,
                                                        1000u, &cmd, WM_MQ_MSG_SIZE);
                if(OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("WMSYS(%d):FLUSH MqSend=%d\r\n",wm_dev,osal_ret);
                }
                r_wmdrv_Vout[wm_dev].UserEvent[j][R_WM_EVENT_FLUSHSYNC].MqHandle = 0;
            }
        }
    }

    /* If DOC error is occurred and DOC registers are not cleared, this function clears DOC registers */
    // TODO Add DOC
    // R_DOC_ClearDocIntStatusPost(loc_vout->DocUnit);
}

/**
 * This function registers a call back for VSync and waits on a VSPD Interrupt.
 *
 * @param[in]  Arg - pointer to the system Vspd instance
 */
static void *loc_WMVspdIrqThread(osal_device_handle_t device_handle, uint64_t irq_channel, void* irq_arg)
{
    r_wmdrv_Sys_VspdInfo_t   *inst;
    int32_t ret;
    inst = (r_wmdrv_Sys_VspdInfo_t *)irq_arg;

    /* update the layers if the correct FRE ISR was received */
    ret = R_VSP2_Isr(inst->VspdUnit,
                     inst->Wpf0InUse,
                     inst->Wpf1InUse);

    /* Check which interrupt was received
     * and release the related semaphore(s)
     */
    /* wpf0 */
    if(0 != (ret & R_VSP_INT_FRE_WPF0)) {
        loc_WmVspSemHandler(inst->VspdUnit, 0);
    }
    /* wpf1 */
    if (0 != (ret & R_VSP_INT_FRE_WPF1)) {
        loc_WmVspSemHandler(inst->VspdUnit, 1);
    }
    return 0;
}

/**
 * @brief Device opening function
 * - Get the number of device which related to "vspd" device type by using R_OSAL_IoGetNumOfDevices
 * - Get the required size of the buffer which use to store list of "vspd" devices by using R_OSAL_IoGetDeviceList
 * - Get the list of "vspd" devices by using R_OSAL_IoGetDeviceList
 * - Convert the device list to an array of device ID type
 * - Open "vspd_00" which is the first element of the device ID array above by using R_OSAL_IoDeviceOpen
 *
 * @param[in] device_type Device type
 * @param[in] device_channel Device channel
 * @param[in, out] device_handle To set the address of osal_device_handle_t
 * @return 0 on success
 * @return -1 on failure
 */
int loc_WMDeviceOpen(char *device_type, int device_channel, osal_device_handle_t *device_handle)
{
    /* local variable */
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    int app_ret = 0;
    size_t numOfDevice = 0;
    size_t numOfByte = 0;
    size_t i = 0;
    size_t j = 0;
    osal_device_handle_t local_handle = OSAL_DEVICE_HANDLE_INVALID;
    char devicelist[200];

    osal_ret = R_OSAL_IoGetNumOfDevices(device_type, &numOfDevice);
    if(OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("WMSYS:GetNumOfDevices=%d\r\n", osal_ret);
        app_ret = -1;
    } else {
        osal_ret = R_OSAL_IoGetDeviceList(device_type, &devicelist[0], sizeof(devicelist), &numOfByte);
        if(OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("WMSYS:GetDeviceList=%d\r\n", osal_ret);
            app_ret = -1;
        } else {
            char *deviceList[numOfDevice + 1];
            deviceList[0] = &devicelist[0];
            for (i = 0; i < numOfByte; i++) {
                if (devicelist[i] == '\n') {
                    j++;
                    devicelist[i] = '\0';
                    deviceList[j] = &devicelist[i+1];
                }
            }

            if(j > numOfDevice) {
                app_ret = -1;
                R_PRINT_Log("WMSYS:numOfDevice=%d\r\n", app_ret);
            } else {
                osal_ret = R_OSAL_IoDeviceOpen(deviceList[device_channel], &local_handle);
                if(OSAL_RETURN_OK != osal_ret) {
                    R_PRINT_Log("WMSYS:R_OSAL_IoDeviceOpen=%d\r\n", osal_ret);
                    app_ret = -1;
                } else {
                    *device_handle = local_handle;
                }
            }
        }
    }

    if(0 != app_ret) {
        *device_handle = OSAL_DEVICE_HANDLE_INVALID;
    }

    return app_ret;
}


/*
 * Section: Global Functions
 */

int32_t R_WMDRV_Sys_IrqInit(void *Inst)
{
    int32_t ret = 0;
    e_osal_return_t osal_err = OSAL_RETURN_OK;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;
    int device_channel = 0;
    char taskname[] = "vspd";

    if (0 != loc_vout) {
        osal_err = R_OSAL_ThsyncMutexLockForTimePeriod(r_wmdrv_Vout[loc_vout->DuUnit].MutexHandle, 3000);
        if (OSAL_RETURN_OK == osal_err) {
            /* Set-up IRQ call back for VSPD */
            if (-1 == loc_vout->VspdConf->IrqVspdQuit) {
                loc_vout->VspdConf->IrqVspdQuit = 0;
                loc_vout->VspdConf->IrqVspdDiscomQuit = 0;

                osal_err = R_OSAL_ThsyncMutexUnlock(r_wmdrv_Vout[loc_vout->DuUnit].MutexHandle);
                if (OSAL_RETURN_OK == osal_err) {

                    switch(loc_vout->VspdConf->VspdUnit) {
                    case R_VSP2_VSPD0:
                       if (R_DU_DU0 == loc_vout->DuUnit) {
                            device_channel = 0;
                        } else if (R_DU_DU3 == loc_vout->DuUnit) {
                            device_channel = 0;
                        }
                        break;
                    case R_VSP2_VSPD1:
                        device_channel = 1;
                        break;
                    case R_VSP2_VSPD2:
                        device_channel = 2;
                        break;
                    default:
                        R_PRINT_Log("WMSYS: R_WMDRV_Sys_IrqInit VspdUnit is invalid(%d)\r\n",loc_vout->VspdConf->VspdUnit);
                        device_channel = 0;
                        break;
                    }

                    ret = loc_WMDeviceOpen(taskname, device_channel, &loc_vout->VspdConf->device_handle);
                    if (0 == ret) {
                        osal_err = R_OSAL_InterruptRegisterIsr(loc_vout->VspdConf->device_handle,
                                                               0,
                                                               0,
                                                               (p_osal_isr_func_t)loc_WMVspdIrqThread,
                                                               (void*)(loc_vout->VspdConf));
                        if (OSAL_RETURN_OK == osal_err) {
                            osal_err = R_OSAL_InterruptEnableIsr(loc_vout->VspdConf->device_handle, 0);
                            if (OSAL_RETURN_OK != osal_err) {
                                R_PRINT_Log("WMSYS:VSPDIRQ EnableIsr=%d\r\n", osal_err);
                                ret = -1;
                            }

                            osal_err = R_OSAL_InterruptRegisterIsr(loc_vout->VspdConf->device_handle,
                                                       WM_DISCOM_IRQCHANNEL,
                                                       0,
                                                       (p_osal_isr_func_t)loc_WMVspdDiscomIrqThread,
													   (void*)loc_vout);
                            if (OSAL_RETURN_OK == osal_err) {
                                osal_err = R_OSAL_InterruptEnableIsr(loc_vout->VspdConf->device_handle, WM_DISCOM_IRQCHANNEL);
                            } else {
                                R_PRINT_Log("WMSYS:DISCOMIRQ EnableIsr=%d\r\n", osal_err);
                                ret = -1;
                            }

                        } else {
                            R_PRINT_Log("WMSYS:VSPDIRQ RegisterIsr=%d\r\n", osal_err);
                            ret = -1;
                        }
                    } else {
                        R_PRINT_Log("WMSYS:DEVOpen VSPD failed\r\n");
                        ret = -1;
                    }
                } else {
                    R_PRINT_Log("WMSYS:MutexUnLock=%d\r\n", osal_err);
                    ret = -1;
                }
            } else {
                osal_err = R_OSAL_ThsyncMutexUnlock(r_wmdrv_Vout[loc_vout->DuUnit].MutexHandle);
                if (OSAL_RETURN_OK != osal_err) {
                    R_PRINT_Log("WMSYS:MutexUnlock=%d\r\n", osal_err);
                    ret = -1;
                }
                /* VSPD IRQ has already been set-up, no need to re-do it */
            }
        } else {
            R_PRINT_Log("WMSYS:MutexLock=%d\r\n", osal_err);
            ret = -1;
        }
    } else {
        ret = -1;
        R_PRINT_Log("WMSYS:R_WMDRV_Sys_IrqInit parameter Inst NULL failed(%d)\r\n", ret);
    }
    return ret;
}

int32_t R_WMDRV_Sys_DocIrqInit(void *Inst)
{
    int32_t ret = 0;
    e_osal_return_t osal_err = OSAL_RETURN_OK;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;
    int doc_device_channel = 0;
    char doc_taskname[] = "doc";

    if (0 != loc_vout) {
        switch(loc_vout->DuUnit) {
        case R_DU_DU0:
            doc_device_channel = 0;
            break;
        case R_DU_DU1:
            doc_device_channel = 0;
            break;
        case R_DU_DU2:
            doc_device_channel = 1;
            break;
        case R_DU_DU3:
            doc_device_channel = 1;
            break;
        default:
            R_PRINT_Log("WMSYS: R_WMDRV_Sys_DocIrqInit DuUnit is invalid(%d)\r\n", loc_vout->DuUnit);
            doc_device_channel = 0;
            ret = -1;
            break;
        }

        if (0 == ret) {
            if (0 == DocIrqInfo[doc_device_channel].EnableCnt) {
                ret = loc_WMDeviceOpen(doc_taskname, doc_device_channel, &DocDevHandle[doc_device_channel]);
                if (0 == ret) {
                    DocIrqInfo[doc_device_channel].EnableCnt++;

                    /* DOC interrupt setting */
                    osal_err = R_OSAL_InterruptRegisterIsr(DocDevHandle[doc_device_channel],
                                                           WM_DOC_IRQCHANNEL,
                                                           0,
                                                           (p_osal_isr_func_t)loc_WMDocDocIrqThread,
                                                           (void*)&DocIrqInfo[doc_device_channel]);
                    if (OSAL_RETURN_OK == osal_err) {
                        osal_err = R_OSAL_InterruptEnableIsr(DocDevHandle[doc_device_channel], WM_DOC_IRQCHANNEL);
                        if (OSAL_RETURN_OK != osal_err) {
                            R_PRINT_Log("WMSYS:R_WMDRV_Sys_DocIrqInit(DOC) EnableIsr=%d\r\n", osal_err);
                            ret = -1;
                        }
                    } else {
                        R_PRINT_Log("WMSYS:R_WMDRV_Sys_DocIrqInit(DOC) RegisterIsr=%d\r\n", osal_err);
                        ret = -1;
                    }

                    if (0 == ret) {
                        /* ActiveMonitor interrupt setting */
                        osal_err = R_OSAL_InterruptRegisterIsr(DocDevHandle[doc_device_channel],
                                                               WM_ACT_IRQCHANNEL,
                                                               0,
                                                               (p_osal_isr_func_t)loc_WMDocActIrqThread,
                                                               (void*)&DocIrqInfo[doc_device_channel]);
                        if (OSAL_RETURN_OK == osal_err) {
                            osal_err = R_OSAL_InterruptEnableIsr(DocDevHandle[doc_device_channel], WM_ACT_IRQCHANNEL);
                            if (OSAL_RETURN_OK != osal_err) {
                                R_PRINT_Log("WMSYS:R_WMDRV_Sys_DocIrqInit(ACT) EnableIsr=%d\r\n", osal_err);
                                ret = -1;
                            }
                        } else {
                            R_PRINT_Log("WMSYS:R_WMDRV_Sys_DocIrqInit(ACT) RegisterIsr=%d\r\n", osal_err);
                            ret = -1;
                        }
                    }
                } else {
                    R_PRINT_Log("WMSYS:R_WMDRV_Sys_DocIrqInit loc_WMDeviceOpen DOC failed\r\n");
                }
            } else {
                DocIrqInfo[doc_device_channel].EnableCnt++;
            }
        }
    } else {
        ret = -1;
        R_PRINT_Log("WMSYS:R_WMDRV_Sys_DocIrqInit parameter Inst NULL failed(%d)\r\n", ret);
    }
    return ret;
}

void R_WMDRV_Sys_IrqDeinit(void *Inst, uint32_t VspdDeinit)
{
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;
    e_osal_return_t osal_err = OSAL_RETURN_OK;

    if (0 != loc_vout) {
        if (1 == VspdDeinit) {
            /* De-init VSPD */
            osal_err = R_OSAL_InterruptDisableIsr(loc_vout->VspdConf->device_handle, 0);
            if (OSAL_RETURN_OK == osal_err) {
                osal_err = R_OSAL_InterruptUnregisterIsr(loc_vout->VspdConf->device_handle,
                                                         0,
                                                         (p_osal_isr_func_t)loc_WMVspdIrqThread);
                if (OSAL_RETURN_OK == osal_err) {
                    osal_err = R_OSAL_ThsyncMutexLockForTimePeriod(
                                            r_wmdrv_Vout[loc_vout->DuUnit].MutexHandle,
                                            3000);
                    if (OSAL_RETURN_OK == osal_err) {
                        loc_vout->VspdConf->IrqVspdQuit = -1;
                    } else {
                        R_PRINT_Log("WMSYS:DeinitVSPD MutexLock=%d\r\n", osal_err);
                    }
                    osal_err = R_OSAL_ThsyncMutexUnlock(r_wmdrv_Vout[loc_vout->DuUnit].MutexHandle);
                    if (OSAL_RETURN_OK != osal_err) {
                        R_PRINT_Log("WMSYS:DeinitVSPD MutexUnlock=%d\r\n", osal_err);
                    }
                } else {
                    R_PRINT_Log("WMSYS:DeinitVSPD InterruptUnregisterIsr=%d\r\n", osal_err);
                }
            } else {
                R_PRINT_Log("WMSYS:DeinitVSPD InterruptDisableIsr=%d\r\n", osal_err);
            }

            /* De-init DISCOM */
            osal_err = R_OSAL_InterruptDisableIsr(loc_vout->VspdConf->discom_device_handle, 0);
            if (OSAL_RETURN_OK == osal_err) {
                osal_err = R_OSAL_InterruptUnregisterIsr(loc_vout->VspdConf->discom_device_handle, 0, 0);
                if (OSAL_RETURN_OK == osal_err) {
                    loc_vout->VspdConf->IrqVspdDiscomQuit = 1;
                }
            }

            /* Close VSPD device */
            osal_err = R_OSAL_IoDeviceClose(loc_vout->VspdConf->device_handle);
            if (OSAL_RETURN_OK != osal_err)
            {
                R_PRINT_Log("WMSYS:DeviceCloseVSPD R_OSAL_IoDeviceClose=%d\r\n", osal_err);
            }
        }
    }
}

void R_WMDRV_Sys_DocIrqDeinit(void *Inst)
{
    int32_t ret = 0;
    r_wmdrv_Sys_Vout_t *loc_vout = (r_wmdrv_Sys_Vout_t *) Inst;
    e_osal_return_t osal_err = OSAL_RETURN_OK;
    int doc_device_channel = 0;

    if (0 != loc_vout) {
        switch(loc_vout->DuUnit) {
        case R_DU_DU0:
            doc_device_channel = 0;
            break;
        case R_DU_DU1:
            doc_device_channel = 0;
            break;
        case R_DU_DU2:
            doc_device_channel = 1;
            break;
        case R_DU_DU3:
            doc_device_channel = 1;
            break;
        default:
            R_PRINT_Log("WMSYS: R_WMDRV_Sys_DocIrqInit DuUnit is invalid(%d)\r\n", loc_vout->DuUnit);
            doc_device_channel = 0;
            ret = -1;
            break;
        }
        if (0 == ret) {
            if (1 == DocIrqInfo[doc_device_channel].EnableCnt) {
                /* De-init DOC */
                osal_err = R_OSAL_InterruptDisableIsr(DocDevHandle[doc_device_channel], WM_DOC_IRQCHANNEL);
                if (OSAL_RETURN_OK == osal_err) {
                    osal_err = R_OSAL_InterruptUnregisterIsr(DocDevHandle[doc_device_channel],
                                                            WM_DOC_IRQCHANNEL,
                                                            (p_osal_isr_func_t)loc_WMDocDocIrqThread);
                    if (OSAL_RETURN_OK != osal_err) {
                        R_PRINT_Log("WMSYS:R_WMDRV_Sys_DocIrqDeinit(DOC) R_OSAL_InterruptUnregisterIsr =%d\r\n", osal_err);
                    }
                } else {
                    R_PRINT_Log("WMSYS:R_WMDRV_Sys_DocIrqDeinit(DOC) R_OSAL_InterruptDisableIsr =%d\r\n", osal_err);
                }

                /* De-init ACT */
                osal_err = R_OSAL_InterruptDisableIsr(DocDevHandle[doc_device_channel], WM_ACT_IRQCHANNEL);
                if (OSAL_RETURN_OK == osal_err) {
                    osal_err = R_OSAL_InterruptUnregisterIsr(DocDevHandle[doc_device_channel],
                                                            WM_ACT_IRQCHANNEL,
                                                            (p_osal_isr_func_t)loc_WMDocActIrqThread);
                    if (OSAL_RETURN_OK != osal_err) {
                        R_PRINT_Log("WMSYS:R_WMDRV_Sys_DocIrqDeinit(ACT) R_OSAL_InterruptUnregisterIsr=%d\r\n", osal_err);
                    }
                } else {
                    R_PRINT_Log("WMSYS:R_WMDRV_Sys_DocIrqDeinit(ACT) InterruptDisableIsr=%d\r\n", osal_err);
                }

                /* Close DOC device */
                osal_err = R_OSAL_IoDeviceClose(DocDevHandle[doc_device_channel]);
                if (OSAL_RETURN_OK != osal_err)
                {
                    R_PRINT_Log("WMSYS:R_WMDRV_Sys_DocIrqDeinit R_OSAL_IoDeviceClose=%d\r\n", osal_err);
                } else {
                    DocIrqInfo[doc_device_channel].EnableCnt--;
                }
            } else {
                DocIrqInfo[doc_device_channel].EnableCnt--;
            }
        }
    }
}
