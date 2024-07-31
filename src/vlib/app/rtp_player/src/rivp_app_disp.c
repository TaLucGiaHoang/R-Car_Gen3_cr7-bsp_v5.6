/******************************************************************************
 *
 * PURPOSE
 *   RIVP Reference Application Window Manger Wrapper
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *
 ******************************************************************************/
/**
 * Copyright(C) 2017-2019 Renesas Electronics Corporation. All Rights Reserved.
 * RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
 * This program must be used solely for the purpose for which
 * it was furnished by Renesas Electronics Corporation.
 * No part of this program may be reproduced or disclosed to
 * others, in any form, without the prior written permission
 * of Renesas Electronics Corporation.
 *
 ******************************************************************************/

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
#include "rivp_ldd_api.h"
#include "rivp_app_disp.h"
#include "rivp_app_ldd_osdep.h"
#include "rivp_dep_ldd_log.h"

#include "display.h"


/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
#define RIVP_APP_DISP_BUF_NUM        (2u)
#define RIVP_APP_DISP_BUFF_YADDR    (0u)
#define RIVP_APP_DISP_BUFF_CADDR    (1u)

#define RIVP_APP_DISP_SYNC_WAIT (1)

/******************************************************************************/
/*                    TYPE DEFINITION                                         */
/******************************************************************************/
typedef struct {
    RIVP_LDD_BOOL                    wndInit;
    RIVP_LDD_BOOL                    wndEnable;
} RIVP_APP_DISPCTL_T;

/******************************************************************************/
/*                    FORWARD DECLARATION                                     */
/******************************************************************************/

r_wm_Display_t s_rivpAppDispDisplay;
r_wm_Layer_t s_rivpAppDispLayer;


/******************************************************************************/
/*                    EXTERNAL FUNCTIONS                                      */
/******************************************************************************/

/**
 * \brief         initialization of the WM Device
 *
 * \param[in]     dispParam                     : Parameter for initialization
 *
 * \retval         RIVP_LDD_TRUE                     : Success
 * \retval         RIVP_LDD_FALSE                     : Failure
 *
 */
RIVP_LDD_BOOL RIVP_AppDispInit(const RIVP_APP_DISP_PARAM_T *dispParam)
{
    RIVP_LDD_BOOL lBoolRet = RIVP_LDD_TRUE;

    if(R_WM_ERR_SUCCESS != R_WM_DisplayConfig(R_WM_DISP_PORT_HDMI0, &s_rivpAppDispDisplay)){
        lBoolRet = RIVP_LDD_FALSE;
        RIVP_LDD_LOG_APP_WNG("[%s] Failed to config display.\n", __func__);
    } else {
        s_rivpAppDispDisplay.BackgroundColor = 0xffffffff; //white
        if(R_WM_ERR_SUCCESS != R_WM_DisplayInit(&s_rivpAppDispDisplay)) {
            lBoolRet = RIVP_LDD_FALSE;
            RIVP_LDD_LOG_APP_WNG("[%s] Failed to initialize display.\n", __func__);
        } else {
            if(R_WM_ERR_SUCCESS != R_WM_DisplayEnable(&s_rivpAppDispDisplay, true)){
                lBoolRet = RIVP_LDD_FALSE;
                RIVP_LDD_LOG_APP_WNG("[%s] Failed to enable display.\n", __func__);
            } else {
                s_rivpAppDispLayer.ParentPtr = &s_rivpAppDispDisplay;
                s_rivpAppDispLayer.VoutSurface.PosX = 0;
                s_rivpAppDispLayer.VoutSurface.PosY = 0;
                s_rivpAppDispLayer.VoutSurface.PosZ = 1;  //z-index? check
                s_rivpAppDispLayer.VoutSurface.Width = dispParam->width;
                s_rivpAppDispLayer.VoutSurface.Height = dispParam->height;
                s_rivpAppDispLayer.VoutSurface.Fmt = R_WM_FMT_YCBCR420; //NV12      //select with dispParam.support422Format?
                s_rivpAppDispLayer.VoutSurface.Handle = 0;
                s_rivpAppDispLayer.VoutSurface.FBuf = 0;
                // s_rivpAppDispLayer.VoutSurface.FBufAlpha = 0;
                s_rivpAppDispLayer.VoutSurface.StrideY = dispParam->pitch;
                s_rivpAppDispLayer.VoutSurface.StrideC = dispParam->pitch;

                if(R_WM_ERR_SUCCESS != R_WM_LayerInit(&s_rivpAppDispLayer)) {
                    lBoolRet = RIVP_LDD_FALSE;
                    RIVP_LDD_LOG_APP_WNG("[%s] Failed to initialize layer.\n",__func__);
                }
                if(R_WM_ERR_SUCCESS !=  R_WM_LayerShow(&s_rivpAppDispLayer, true)) {
                    lBoolRet = RIVP_LDD_FALSE;
                    RIVP_LDD_LOG_APP_WNG("[%s] Failed to show layer.\n",__func__);
                }
            }
        }
    }

    return lBoolRet;
}

/**
 * \brief         deinitialize the WM Dev for the display output unit
 *
 * \retval         RIVP_LDD_TRUE                         : Success
 * \retval         RIVP_LDD_FALSE                         : Failure
 *
 */
RIVP_LDD_BOOL RIVP_AppDispDeinit(void)
{
    r_wm_Error_t wmErr;

    wmErr = R_WM_LayerDeInit(&s_rivpAppDispLayer);
    if (R_WM_ERR_SUCCESS != wmErr) {
        RIVP_LDD_LOG_APP_WNG("R_WM_LayerDeInit failed Err= %d\n", wmErr);
    }

    return RIVP_LDD_TRUE;
}

/**
 * \brief         update frame buffers for display output
 *
 * \param[in]     pic                             : Decoded picture parameter
 *
 * \retval         RIVP_LDD_TRUE                         : Success
 * \retval         RIVP_LDD_FALSE                         : Failure
 *
 */
RIVP_LDD_BOOL RIVP_AppDispUpdate(RIVP_LDD_PICPARAM_T *pic)
{
    RIVP_LDD_BOOL lBoolRet = RIVP_LDD_TRUE;
    r_wm_Error_t wm_ret;
    if(pic->format[RIVP_LDD_PIC_COMPONENT_Y] != RIVP_LDD_FORMAT_8BIT_420_LINEAR){
        RIVP_LDD_LOG_APP_ERR_PRM("Invalid picture format.\n");
        lBoolRet = RIVP_LDD_FALSE;
        goto exit;
    }

    s_rivpAppDispLayer.VoutSurface.FBuf = pic->picBuff[RIVP_LDD_PIC_FIELD_TOP].yPhysAddr;
    s_rivpAppDispLayer.VoutSurface.FBufC0 = pic->picBuff[RIVP_LDD_PIC_FIELD_TOP].cPhysAddr;
    s_rivpAppDispLayer.VoutSurface.FBufC1 = 0;
    wm_ret = R_WM_SurfacePropertySet(s_rivpAppDispLayer.ParentPtr->Dev, R_WM_SURF_PROP_BUFFER, (void *)&s_rivpAppDispLayer.VoutSurface);
    if(wm_ret != R_WM_ERR_SUCCESS){
        RIVP_LDD_LOG_APP_ERR_PRM("Display set Frame buffer [%d]\n", wm_ret);
        lBoolRet = RIVP_LDD_FALSE;
        goto exit;
    }
    wm_ret = R_WM_ScreenSurfacesFlush(s_rivpAppDispLayer.ParentPtr->Dev, 0);
    if(wm_ret != R_WM_ERR_SUCCESS){
        RIVP_LDD_LOG_APP_ERR_PRM("Display flush [%d]\n", wm_ret);
        lBoolRet = RIVP_LDD_FALSE;
        goto exit;
    }

exit:
    return lBoolRet;
}


/******************************************************************************/
/*                    INTERNAL FUNCTIONS                                      */
/******************************************************************************/
