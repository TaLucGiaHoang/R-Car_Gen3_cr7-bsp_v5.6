/*************************************************************************************************************
* cio_vin_ioctl_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/*******************************************************************************
  Section: Includes
*/
#include "rcar-xos/osal/r_osal.h"
#include "r_cio_bridge.h"
#include "r_cio_vin_main.h"
#include "r_cio_vin_ioctl.h"
#include "r_cio_vin_public.h"

#include "r_vin_api.h"
#include "r_rtdmac_api.h"
#include "r_csi2_api.h"
#include "r_adv7482_api.h"
#include "r_print_api.h"

#define VIE_VALUE_FIE   (1 << 4)  /* FIE */
#define VIE_VALUE_EFE   (1 << 1)  /* EFE */

/*******************************************************************************
  Section: Local functions
*/


/*******************************************************************************
  Section: Global functions
*/

/*******************************************************************************
  Function name: VIN_PRV_IoCtl
*/
ssize_t R_CIO_VIN_PRV_IoCtl(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    R_CIO_VIN_PRV_Instance_t *pinst;
    ssize_t ret = R_CIO_VIN_ERR_OK;
    r_cio_vin_Ioctl_t ctrl = (r_cio_vin_Ioctl_t)Par1;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    osal_thread_handle_t self_thread_handle = OSAL_THREAD_HANDLE_INVALID;
    static uint8_t adv_ini_cnt = 0;
    r_vin_Error_t vin_rt = R_VIN_ERR_SUCCESS;
    r_csi2_Error_t csi2_rt = R_CSI2_ERR_SUCCESS;
    r_adv7482_Error_t adv_rt = R_ADV7482_ERR_SUCCESS;
    uint32_t dmac_rt = 0;

    pinst = R_CIO_VIN_PRV_FindInstance(Addr);
    if (0 == pinst)
    {
        R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThsyncMutexLockForTimePeriod failed(%d)\r\n", osal_ret);
        ret = R_CIO_VIN_ERR_NO_INST;
    }
    else {
        switch (ctrl)
        {
            case R_CIO_VIN_IOC_CONFIG:
            {
                r_cio_vin_Config_t *pcfg = (r_cio_vin_Config_t *)Par2;

                osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(pinst->mtx_handle, (osal_milli_sec_t)TIMEOUT_MS);
                if(OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThsyncMutexLockForTimePeriod failed(%d)\r\n", osal_ret);
                    ret = R_CIO_VIN_ERR_FAILED;
                    break;
                }
                /* TODO: check config parameters. */
                if (pinst->State == R_CIO_VIN_STATE_UNINIT)
                {
                    pinst->Config = *pcfg;
                    pinst->State = R_CIO_VIN_STATE_CONFIGURED;

                    /* adapt settings to lower driver */
                    pinst->Setting.Cam_Res_X = (int16_t)pinst->Config.ImageInWidth;
                    pinst->Setting.Cam_Res_Y = (int16_t)pinst->Config.ImageInHeight;
                    pinst->Setting.pos_x = pinst->Config.pos_x;
                    pinst->Setting.pos_y = pinst->Config.pos_y;
                    pinst->Setting.Out_Res_X = (int16_t)pinst->Config.ImageOutWidth;
                    pinst->Setting.Out_Res_Y = (int16_t)pinst->Config.ImageOutHeight;
                    pinst->Setting.Scaled_X = (int16_t)pinst->Config.ImageScaledWidth;
                    pinst->Setting.Scaled_Y = (int16_t)pinst->Config.ImageScaledHeight;
                    pinst->Setting.Offset = pinst->Config.ImageUvaof;
                    pinst->Setting.Stride = pinst->Config.ImageStride;
                    pinst->Setting.Interlaced = pinst->Config.Interlaced;
                    pinst->Setting.VmcInf = pinst->Config.VmcInf;
                    pinst->Setting.VmcIm = pinst->Config.VmcIm;
                    pinst->Setting.OutputFmt = pinst->Config.OutputFmt;

                    if(adv_ini_cnt == 0)
                    {
                        /* For Salvator XS: CVBS-ADV7482 */
                        /* CSI40-VIN0, CSI20-VIN5 */
                        adv_rt = R_ADV7482_Init(R_ADV7482_I2C_CH4,
                                                pinst->Config.ColorSpaceOut,
                                                pinst->Config.BoundaryMbps);
                        if (R_ADV7482_ERR_SUCCESS != adv_rt)
                        {
                            R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_ADV7482_Init failed(%d)\r\n", adv_rt);
                            ret = R_CIO_VIN_ERR_FAILED;
                        }
                        adv_ini_cnt++;
                    }

                    if (R_CIO_VIN_ERR_OK == ret)
                    {
                        if (pinst->Config.Csi2Type == R_CIO_VIN_CSI2_TYPE_CSI40)
                        {
                            /* Wait for TMDS clock frequency to stabilize */
                            adv_rt = R_ADV7482_tmds_stable_wait(R_ADV7482_I2C_CH4);
                            if (R_ADV7482_ERR_SUCCESS != adv_rt)
                            {
                                R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_ADV7482_tmds_stable_wait failed(%d)\r\n", adv_rt);
                                ret = R_CIO_VIN_ERR_FAILED;
                            }

                            if (R_CIO_VIN_ERR_OK == ret)
                            {
                                adv_rt = R_ADV7482_Stop_csi40(R_ADV7482_I2C_CH4,
                                                           pinst->Config.Lanes);
                                if (R_ADV7482_ERR_SUCCESS != adv_rt)
                                {
                                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_ADV7482_Stop_csi40 failed(%d)\r\n", adv_rt);
                                    ret = R_CIO_VIN_ERR_FAILED;
                                }
                            }

                            if (R_CIO_VIN_ERR_OK == ret)
                            {
                                vin_rt = R_VIN_Prepare(pinst->VinIdx,
                                                       0,
                                                       &pinst->Setting);
                                if (R_VIN_ERR_SUCCESS != vin_rt)
                                {
                                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_VIN_Prepare failed(%d)\r\n", vin_rt);
                                    ret = R_CIO_VIN_ERR_FAILED;
                                }

                            }

                            if (R_CIO_VIN_ERR_OK == ret)
                            {
                                csi2_rt = R_CSI2_Init(RCAR_CSI40,
                                                      pinst->Config.DataRate,
                                                      pinst->Config.Interlaced,
                                                      pinst->Config.Lanes);

                                if (R_CSI2_ERR_SUCCESS != csi2_rt)
                                {
                                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_CSI2_Init failed(%d)\r\n", csi2_rt);
                                    ret = R_CIO_VIN_ERR_FAILED;
                                }
                             }

                            if (R_CIO_VIN_ERR_OK == ret)
                            {
                                /* start adv7482 */
                                adv_rt = R_ADV7482_Start_csi40(R_ADV7482_I2C_CH4,
                                                           pinst->Config.Lanes);
                                if (R_ADV7482_ERR_SUCCESS != adv_rt)
                                {
                                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_ADV7482_Start_csi40 failed(%d)\r\n", adv_rt);
                                    ret = R_CIO_VIN_ERR_FAILED;
                                }
                            }
                        }
                        else if (pinst->Config.Csi2Type == R_CIO_VIN_CSI2_TYPE_CSI20)
                        {
                            /* CSI20-VIN5 */
                            adv_rt = R_ADV7482_Stop(R_ADV7482_I2C_CH4,
                                                    pinst->Config.Lanes);
                            if (R_ADV7482_ERR_SUCCESS != adv_rt)
                            {
                                R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_ADV7482_Stop failed(%d)\r\n", adv_rt);
                                ret = R_CIO_VIN_ERR_FAILED;
                            }

                            if (R_CIO_VIN_ERR_OK == ret)
                            {
                                vin_rt = R_VIN_Prepare(pinst->VinIdx,
                                                       0,
                                                       &pinst->Setting);
                                if (R_VIN_ERR_SUCCESS != vin_rt)
                                {
                                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_VIN_Prepare failed(%d)\r\n", vin_rt);
                                    ret = R_CIO_VIN_ERR_FAILED;
                                }
                            }

                            if (R_CIO_VIN_ERR_OK == ret)
                            {
                                csi2_rt = R_CSI2_Init(RCAR_CSI20,
                                                      pinst->Config.DataRate,
                                                      pinst->Config.Interlaced,
                                                      pinst->Config.Lanes);
                                if (R_CSI2_ERR_SUCCESS != csi2_rt)
                                {
                                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_CSI2_Init failed(%d)\r\n", csi2_rt);
                                    ret = R_CIO_VIN_ERR_FAILED;
                                }
                            }

                            if (R_CIO_VIN_ERR_OK == ret)
                            {
                                /* start adv7482 */
                                adv_rt = R_ADV7482_Start(R_ADV7482_I2C_CH4,
                                                         pinst->Config.Lanes);
                                if (R_ADV7482_ERR_SUCCESS != adv_rt)
                                {
                                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_ADV7482_Start failed(%d)\r\n", adv_rt);
                                    ret = R_CIO_VIN_ERR_FAILED;
                                }
                            }
                        }
                    }
                }
                else
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl Status Error. ctrl(%d), pinst->State(%d)\r\n", ctrl, pinst->State);
                    ret = R_CIO_VIN_ERR_IOC_CHN_CONFIGED;
                }
                osal_ret = R_OSAL_ThsyncMutexUnlock(pinst->mtx_handle);
                if (OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThsyncMutexUnlock failed(%d)\r\n", osal_ret);
                    ret = R_CIO_VIN_ERR_FAILED;
                }
                break;
            }

            case R_CIO_VIN_IOC_GET_CONFIG:
            {
                if (pinst->State != R_CIO_VIN_STATE_UNINIT)
                {
                    r_cio_vin_Config_t *pcfg = (r_cio_vin_Config_t *)Par2;
                    *pcfg = pinst->Config;
                }
                else
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl Status Error. ctrl(%d), pinst->State(%d)\r\n", ctrl, pinst->State);
                    ret = R_CIO_VIN_ERR_IOC_CHN_UNINIT;
                }
                break;
            }

            case R_CIO_VIN_IOC_START_CAPTURE:
            {
                osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(pinst->mtx_handle, (osal_milli_sec_t)TIMEOUT_MS);
                if(OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThsyncMutexLockForTimePeriod failed(%d)\r\n", osal_ret);
                    ret = R_CIO_VIN_ERR_FAILED;
                    break;
                }
                if ((pinst->State == R_CIO_VIN_STATE_CONFIGURED) ||
                    (pinst->State == R_CIO_VIN_STATE_STOPPED))
                {
                    if ((pinst->Setting.Cam_Res_X != pinst->Setting.Scaled_X) ||
                        (pinst->Setting.Cam_Res_Y != pinst->Setting.Scaled_Y))
                    {
                        /* Set Scale Image */
                        vin_rt = R_VIN_ScaleImage(pinst->VinIdx,
                                                  &pinst->Setting);
                        if (R_VIN_ERR_SUCCESS != vin_rt)
                        {
                            R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_VIN_ScaleImage failed(%d)\r\n", vin_rt);
                            ret = R_CIO_VIN_ERR_PARA;
                        }
                    }

                    if (R_CIO_VIN_ERR_OK == ret)
                    {
                        vin_rt = R_VIN_InitEx(pinst->VinIdx,
                                     &pinst->Setting,
                                     pinst->Config.FrameBuffer[0],
                                     pinst->Config.FrameBuffer[1],
                                     pinst->Config.FrameBuffer[2]);
                        if (R_VIN_ERR_SUCCESS != vin_rt)
                        {
                            R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_VIN_InitEx failed(%d)\r\n", vin_rt);
                            ret = R_CIO_VIN_ERR_FAILED;
                        }
                    }

                    if (R_CIO_VIN_ERR_OK == ret)
                    {
                        pinst->State = R_CIO_VIN_STATE_STARTED;
                    }
                }
                if (R_CIO_VIN_ERR_OK == ret)
                {
                    if (pinst->State == R_CIO_VIN_STATE_STARTED)
                    {
                        int32_t idx;
                        osal_ret = R_OSAL_ThreadSelf(&self_thread_handle);
                        if (OSAL_RETURN_OK != osal_ret)
                        {
                            R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThreadSelf failed(%d)\r\n", osal_ret);
                            ret = R_CIO_VIN_ERR_FAILED;
                        }

                        if (R_CIO_VIN_ERR_OK == ret)
                        {
                            idx = R_CIO_VIN_RPV_FindConsumer(pinst, self_thread_handle);
                            if (idx >= 0)
                            {
                                pinst->Consumer[idx].IsCapturing = 1;
                                if (pinst->Config.Interlaced == false)
                                {
                                    R_VIN_EnableInterrupt(pinst->VinIdx, VIE_VALUE_FIE);
                                }
                                else
                                {
                                    R_VIN_EnableInterrupt(pinst->VinIdx, VIE_VALUE_EFE);
                                }
                                R_VIN_Start(pinst->VinIdx, &pinst->Setting);
                            }
                            else
                            {
                                R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl ctrl(%d).R_CIO_VIN_RPV_FindConsumer failed(%d)\r\n", ctrl, idx);
                                ret = R_CIO_VIN_ERR_FAILED;
                            }
                        }
                    }
                    else
                    {
                        R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl Status Error. ctrl(%d), pinst->State(%d)\r\n", ctrl, pinst->State);
                        ret = R_CIO_VIN_ERR_IOC_CHN_UNINIT;
                    }
                }
                osal_ret = R_OSAL_ThsyncMutexUnlock(pinst->mtx_handle);
                if (OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThsyncMutexUnlock failed(%d)\r\n", osal_ret);
                    ret = R_CIO_VIN_ERR_FAILED;
                }
                break;
            }
            case R_CIO_VIN_IOC_RESTART_CAPTURE:
            {
                osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(pinst->mtx_handle, (osal_milli_sec_t)TIMEOUT_MS);
                if(OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThsyncMutexLockForTimePeriod failed(%d)\r\n", osal_ret);
                    ret = R_CIO_VIN_ERR_FAILED;
                    break;
                }
                if ((pinst->State == R_CIO_VIN_STATE_CONFIGURED) ||
                    (pinst->State == R_CIO_VIN_STATE_STOPPED))
                {
                    if(pinst->Config.Csi2Type == R_CIO_VIN_CSI2_TYPE_CSI40)
                    {
                        vin_rt = R_VIN_Prepare(pinst->VinIdx, 0, &pinst->Setting);
                        if (R_VIN_ERR_SUCCESS != vin_rt)
                        {
                            R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_VIN_Prepare failed(%d)\r\n", vin_rt);
                            ret = R_CIO_VIN_ERR_FAILED;
                        }

                        if (R_CIO_VIN_ERR_OK == ret)
                        {
                            csi2_rt = R_CSI2_Init(RCAR_CSI40, pinst->Config.DataRate,
                                     pinst->Config.Interlaced, pinst->Config.Lanes);
                            if (R_CSI2_ERR_SUCCESS != csi2_rt)
                            {
                                R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_CSI2_Init failed(%d)\r\n", csi2_rt);
                                ret = R_CIO_VIN_ERR_FAILED;
                            }
                        }

                        if (R_CIO_VIN_ERR_OK == ret)
                        {
                            adv_rt = R_ADV7482_Start_csi40(R_ADV7482_I2C_CH4,
                                                           pinst->Config.Lanes);
                            if (R_ADV7482_ERR_SUCCESS != adv_rt)
                            {
                                R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_ADV7482_Start_csi40 failed(%d)\r\n", adv_rt);
                                ret = R_CIO_VIN_ERR_FAILED;
                            }
                        }
                    }
                    else
                    {
                        vin_rt = R_VIN_Prepare(pinst->VinIdx, 0, &pinst->Setting);
                        if (R_VIN_ERR_SUCCESS != vin_rt)
                        {
                            R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_VIN_Prepare failed(%d)\r\n", vin_rt);
                            ret = R_CIO_VIN_ERR_FAILED;
                        }

                        if (R_CIO_VIN_ERR_OK == ret)
                        {
                            csi2_rt = R_CSI2_Init(RCAR_CSI20, pinst->Config.DataRate,
                                     pinst->Config.Interlaced, pinst->Config.Lanes);
                            if (R_CSI2_ERR_SUCCESS != csi2_rt)
                            {
                                R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_CSI2_Init failed(%d)\r\n", csi2_rt);
                                ret = R_CIO_VIN_ERR_FAILED;
                            }
                        }

                        if (R_CIO_VIN_ERR_OK == ret)
                        {
                            adv_rt = R_ADV7482_Start(R_ADV7482_I2C_CH4,
                                                     pinst->Config.Lanes);
                            if (R_ADV7482_ERR_SUCCESS != adv_rt)
                            {
                                R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_ADV7482_Start failed(%d)\r\n", adv_rt);
                                ret = R_CIO_VIN_ERR_FAILED;
                            }
                        }
                    }

                    if (R_CIO_VIN_ERR_OK == ret)
                    {
                        if ((pinst->Setting.Cam_Res_X != pinst->Setting.Scaled_X) ||
                            (pinst->Setting.Cam_Res_Y != pinst->Setting.Scaled_Y))
                        {
                            /* Set Scale Image */
                            vin_rt = R_VIN_ScaleImage(pinst->VinIdx,
                                                      &pinst->Setting);
                            if (R_VIN_ERR_SUCCESS != vin_rt)
                            {
                                R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_VIN_ScaleImage failed(%d)\r\n", vin_rt);
                                ret = R_CIO_VIN_ERR_PARA;
                            }
                        }
                    }

                    if (R_CIO_VIN_ERR_OK == ret)
                    {
                        vin_rt = R_VIN_InitEx(pinst->VinIdx,
                                     &pinst->Setting,
                                     pinst->Config.FrameBuffer[0],
                                     pinst->Config.FrameBuffer[1],
                                     pinst->Config.FrameBuffer[2]);
                        if (R_VIN_ERR_SUCCESS != vin_rt)
                        {
                            R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_VIN_InitEx failed(%d)\r\n", vin_rt);
                            ret = R_CIO_VIN_ERR_FAILED;
                        }
                    }

                    if (R_CIO_VIN_ERR_OK == ret)
                    {
                        if(pinst->Config.Interlaced == false)
                        {
                            R_VIN_EnableInterrupt(pinst->VinIdx, VIE_VALUE_FIE);
                        }
                        else
                        {
                            R_VIN_EnableInterrupt(pinst->VinIdx, VIE_VALUE_EFE);
                        }
                        R_VIN_Start(pinst->VinIdx, &pinst->Setting);
                        pinst->State = R_CIO_VIN_STATE_STARTED;
                    }
                }

                if (R_CIO_VIN_ERR_OK == ret)
                {
                    if (pinst->State == R_CIO_VIN_STATE_STARTED)
                    {
                        int32_t idx;
                        osal_ret = R_OSAL_ThreadSelf(&self_thread_handle);
                        if (OSAL_RETURN_OK != osal_ret)
                        {
                            R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThreadSelf failed(%d)\r\n", osal_ret);
                            ret = R_CIO_VIN_ERR_FAILED;
                        }
                        if (R_CIO_VIN_ERR_OK == ret)
                        {
                            idx = R_CIO_VIN_RPV_FindConsumer(pinst, self_thread_handle);
                            if (idx >= 0)
                            {
                                pinst->Consumer[idx].IsCapturing = 1;
                            }
                            else
                            {
                                R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl ctrl(%d).R_CIO_VIN_RPV_FindConsumer failed(%d)\r\n", ctrl, idx);
                                ret = R_CIO_VIN_ERR_FAILED;
                            }
                        }
                    }
                    else
                    {
                        R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl Status Error. ctrl(%d), pinst->State(%d)\r\n", ctrl, pinst->State);
                        ret = R_CIO_VIN_ERR_IOC_CHN_UNINIT;
                    }
                }

                osal_ret = R_OSAL_ThsyncMutexUnlock(pinst->mtx_handle);
                if (OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThsyncMutexUnlock failed(%d)\r\n", osal_ret);
                    ret = R_CIO_VIN_ERR_FAILED;
                }
                break;
            }
            case R_CIO_VIN_IOC_STOP_CAPTURE:
            {
                osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(pinst->mtx_handle, (osal_milli_sec_t)TIMEOUT_MS);
                if(OSAL_RETURN_OK != osal_ret)
                {
                    ret = R_CIO_VIN_ERR_FAILED;
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThsyncMutexLockForTimePeriod failed(%d)\r\n", osal_ret);
                    break;
                }
                if (pinst->State == R_CIO_VIN_STATE_STARTED)
                {
                    int32_t idx;

                    /* stop current consumer */
                    osal_ret = R_OSAL_ThreadSelf(&self_thread_handle);
                    if (OSAL_RETURN_OK != osal_ret)
                    {
                        R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThreadSelf failed(%d)\r\n", osal_ret);
                        ret = R_CIO_VIN_ERR_FAILED;
                    }
                    if (R_CIO_VIN_ERR_OK == ret)
                    {
                        idx = R_CIO_VIN_RPV_FindConsumer(pinst, self_thread_handle);
                        if (idx >= 0) {
                            pinst->Consumer[idx].IsCapturing = 0;
                        }
                        else
                        {
                            R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl ctrl(%d).R_CIO_VIN_RPV_FindConsumer failed(%d)\r\n", ctrl, idx);
                            ret = R_CIO_VIN_ERR_FAILED;
                        }

                        if (R_CIO_VIN_ERR_OK == ret)
                        {
                            /* check other consumers */
                            for (idx = 0; idx < R_CIO_VIN_MAX_CONSUMER_PER_INSTANCE; idx++)
                            {
                                if (pinst->Consumer[idx].IsCapturing)
                                    break;
                            }

                            if (idx >= R_CIO_VIN_MAX_CONSUMER_PER_INSTANCE)
                            {
                                vin_rt = R_VIN_DeInit(pinst->VinIdx);
                                if (R_VIN_ERR_SUCCESS != vin_rt)
                                {
                                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_VIN_DeInit failed(%d)\r\n", vin_rt);
                                    ret = R_CIO_VIN_ERR_FAILED;
                                }
                                else
                                {
                                    if(pinst->Config.Csi2Type == R_CIO_VIN_CSI2_TYPE_CSI40)
                                    {
                                        csi2_rt = R_CSI2_DeInit(RCAR_CSI40);
                                        if (R_CSI2_ERR_SUCCESS != csi2_rt)
                                        {
                                            R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_CSI2_DeInit failed(%d)\r\n", csi2_rt);
                                            ret = R_CIO_VIN_ERR_FAILED;
                                        }
                                        else
                                        {
                                            adv_rt = R_ADV7482_Stop_csi40(R_ADV7482_I2C_CH4,
                                                           pinst->Config.Lanes);
                                            if (R_ADV7482_ERR_SUCCESS != adv_rt)
                                            {
                                                R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_ADV7482_Stop_csi40 failed(%d)\r\n", adv_rt);
                                                ret = R_CIO_VIN_ERR_FAILED;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        csi2_rt = R_CSI2_DeInit(RCAR_CSI20);
                                        if (R_CSI2_ERR_SUCCESS != csi2_rt)
                                        {
                                            R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_CSI2_DeInit failed(%d)\r\n", csi2_rt);
                                            ret = R_CIO_VIN_ERR_FAILED;
                                        }
                                        else
                                        {
                                            adv_rt = R_ADV7482_Stop(R_ADV7482_I2C_CH4,
                                                           pinst->Config.Lanes);
                                            if (R_ADV7482_ERR_SUCCESS != adv_rt)
                                            {
                                                R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_ADV7482_Stop failed(%d)\r\n", adv_rt);
                                                ret = R_CIO_VIN_ERR_FAILED;
                                            }
                                        }
                                    }
                                }

                                if (R_CIO_VIN_ERR_OK == ret)
                                {
                                    pinst->State = R_CIO_VIN_STATE_STOPPED;
                                }
                            }
                        }
                    }
                }
                else
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl Status Error. ctrl(%d), pinst->State(%d)\r\n", ctrl, pinst->State);
                    ret = R_CIO_VIN_ERR_IOC_CHN_NOT_STARTED;
                }
                osal_ret = R_OSAL_ThsyncMutexUnlock(pinst->mtx_handle);
                if (OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThsyncMutexUnlock failed(%d)\r\n", osal_ret);
                }
                break;
            }

            case R_CIO_VIN_IOC_GET_VALID_FRAME:
            {
                osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(pinst->mtx_handle, (osal_milli_sec_t)TIMEOUT_MS);
                if(OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThsyncMutexLockForTimePeriod failed(%d)\r\n", osal_ret);
                    ret = R_CIO_VIN_ERR_FAILED;
                    break;
                }
                if (pinst->State == R_CIO_VIN_STATE_STARTED)
                {
                    uintptr_t *ppframe = (uintptr_t *)Par2;
                    uintptr_t pvinframe = R_VIN_GetLatestFrameBuffer(pinst->VinIdx);
                    if (0 == *ppframe)
                    {
                        *ppframe = pvinframe;
                    }
                    else
                    {
                        if (R_VIN_OUTF_ARGB8888 == pinst->Setting.OutputFmt)
                        {
                            /* copy vin frame to specified buffer (RGB) */
                            dmac_rt = R_RTDMAC_CopyMemory((void*)*ppframe,
                                                (void*)pvinframe,
                                                pinst->Config.ImageOutHeight * pinst->Config.ImageStride * 4);
                            if (drv_OK != dmac_rt)
                            {
                                R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_RTDMAC_CopyMemory error 1. ReturnValue(%d)\r\n", dmac_rt);
                                ret = R_CIO_VIN_ERR_FAILED;
                            }
                        }
                        else /* R_WM_FMT_YCBCR420 */
                        {
                            /* copy vin frame to specified buffer (Y) */
                            dmac_rt = R_RTDMAC_CopyMemory((void*)*ppframe,
                                                (void*)pvinframe,
                                                pinst->Config.ImageOutHeight * pinst->Config.ImageStride);
                            if (drv_OK != dmac_rt)
                            {
                                R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_RTDMAC_CopyMemory error 2. ReturnValue(%d)\r\n", dmac_rt);
                                ret = R_CIO_VIN_ERR_FAILED;
                            }

                            if (R_CIO_VIN_ERR_OK == ret)
                            {
                                /* copy vin frame to specified buffer (CrCb) */
                                dmac_rt = R_RTDMAC_CopyMemory((void*)(((uint32_t)*ppframe) + pinst->Config.ImageUvaof),
                                                    (void*)(pvinframe + pinst->Config.ImageUvaof),
                                                    pinst->Config.ImageOutHeight * pinst->Config.ImageStride);
                                if (drv_OK != dmac_rt)
                                {
                                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_RTDMAC_CopyMemory error 3. ReturnValue(%d)\r\n", dmac_rt);
                                    ret = R_CIO_VIN_ERR_FAILED;
                                }
                            }
                        }
                    }
                }
                else
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl Status Error. ctrl(%d), pinst->State(%d)\r\n", ctrl, pinst->State);
                    ret = R_CIO_VIN_ERR_IOC_CHN_NOT_STARTED;
                }
                osal_ret = R_OSAL_ThsyncMutexUnlock(pinst->mtx_handle);
                if (OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThsyncMutexUnlock failed(%d)\r\n", osal_ret);
                }
                break;
            }

            case R_CIO_VIN_IOC_REGISTER_FRAME_READY_EVT:
            {
                osal_mq_handle_t * mq_handle = (osal_mq_handle_t *)Par2;
                int32_t idx;

                osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(pinst->mtx_handle, (osal_milli_sec_t)TIMEOUT_MS);
                if(OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThsyncMutexLockForTimePeriod failed(%d)\r\n", osal_ret);
                    ret = R_CIO_VIN_ERR_FAILED;
                    break;
                }
                osal_ret = R_OSAL_ThreadSelf(&self_thread_handle);
                if (OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThreadSelf failed(%d)\r\n", osal_ret);
                    ret = R_CIO_VIN_ERR_FAILED;
                }

                if (R_CIO_VIN_ERR_OK == ret)
                {
                    idx = R_CIO_VIN_RPV_FindConsumer(pinst, self_thread_handle);

                    if (idx >= 0)
                    {
                        if (NULL == mq_handle)
                        {
                            R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl mq_handle is NULL. ctrl(%d)\r\n", ctrl);
                            ret = R_CIO_VIN_ERR_PARA;
                        }
                        else if (pinst->Consumer[idx].IsCapturing)
                        {
                            R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl Status Error. ctrl(%d), IsCapturing(%d)\r\n", ctrl, pinst->Consumer[idx].IsCapturing);
                            ret = R_CIO_VIN_ERR_WRONG_STATE;
                        }
                        else
                        {
                            pinst->Consumer[idx].mq_handle = mq_handle;
                        }
                    }
                    else
                    {
                        R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl ctrl(%d).R_CIO_VIN_RPV_FindConsumer failed(%d)\r\n", ctrl, idx);
                        ret = R_CIO_VIN_ERR_FAILED;
                    }
                }

                osal_ret = R_OSAL_ThsyncMutexUnlock(pinst->mtx_handle);
                if(OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThsyncMutexUnlock failed(%d)\r\n", osal_ret);
                }
                break;
            }

            case R_CIO_VIN_IOC_DISABLE_INTERRUPT:
            {
                osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(pinst->mtx_handle, (osal_milli_sec_t)TIMEOUT_MS);
                if(OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThsyncMutexLockForTimePeriod failed(%d)\r\n", osal_ret);
                    ret = R_CIO_VIN_ERR_FAILED;
                    break;
                }
                R_VIN_DisableInterrupt(pinst->VinIdx, 0x2);
                osal_ret = R_OSAL_ThsyncMutexUnlock(pinst->mtx_handle);
                if(OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl R_OSAL_ThsyncMutexUnlock failed(%d)\r\n", osal_ret);
                    ret = R_CIO_VIN_ERR_FAILED;
                    break;
                }
                break;
            }

            default:
            R_PRINT_Log("[CioVinIoctl]: R_CIO_VIN_PRV_IoCtl Unknown Event(%d)\r\n", ctrl);
            ret = R_CIO_VIN_ERR_IOC_UNKNOWN;
            break;
        }
    }

    return ret;
}
