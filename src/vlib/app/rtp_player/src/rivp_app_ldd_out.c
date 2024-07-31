/******************************************************************************
 *
 * PURPOSE
 *   RIVP Reference Application for output part
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *
 ******************************************************************************/
/******************************************************************************
* output part
* Copyright (C) 2017 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
#include "rivp_app_ldd_out.h"
#include "rivp_app_ldd_cmn_local.h"
#include "rivp_app_disp.h"
#include "rivp_app_ldd_osdep.h"
#include "rivp_app_ldd_mmgr.h"
#include "rivp_app_config.h"

#include "rivp_ldd_api.h"

#include "rivp_dep_ldd_log.h"

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
#define RIVP_APP_LDD_OUT_SLEEP_TIME		(10000u)
#define RIVP_APP_LDD_OUT_NO_BUFFERING	(1u)

/******************************************************************************/
/*                    FORWARD DECLARATION                                     */
/******************************************************************************/
RIVP_LDD_STATIC RIVP_LDD_BOOL	 RIVP_AppLddOutUpdateXy(RIVP_APP_LDD_CMN_PARAM_T *app, RIVP_LDD_PICPARAM_T *pic);
RIVP_LDD_STATIC RIVP_LDD_BOOL	 RIVP_AppLddOutOutputYuv(RIVP_APP_LDD_CMN_PARAM_T *app, RIVP_LDD_PICPARAM_T *pic);

/******************************************************************************/
/*                    EXTERNAL FUNCTIONS                                      */
/******************************************************************************/
/**
 * \brief         initialize for display
 *
 * \param[in]     dispParam                 : Parameter for initialization
 *
 * \retval         RIVP_LDD_TRUE                 : success
 * \retval         RIVP_LDD_FALSE                 : display function returns failure
 *
 */
RIVP_LDD_BOOL RIVP_AppOutInit(
    const RIVP_APP_DISP_PARAM_T *dispParam)
{
    RIVP_LDD_BOOL lBoolRet;

    /* initialize WM */
    lBoolRet = RIVP_AppDispInit(dispParam);

    /* return result */
    return lBoolRet;
}

/**
 * \brief         deinitialize for display
 *
 * \retval         RIVP_LDD_TRUE                 : success
 * \retval         RIVP_LDD_FALSE                 : display function returns failure
 *
 */
RIVP_LDD_BOOL RIVP_AppOutDeinit(void)
{
    RIVP_LDD_BOOL lBoolRet;

    /* deinitialize WM */
    lBoolRet = RIVP_AppDispDeinit();

    /* return result */
    return lBoolRet;
}

/**
 * \brief		 task of output process
 *
 * \param[in,out]	 arg			: application control information
 *
 * \retval		 void			: none
 *
 */
void *RIVP_AppLddOutTask(
	RIVP_LDD_PTR arg)
{
	RIVP_LDD_BOOL				 lBoolRet;
	RIVP_LDD_RESULT				 lResult;
	RIVP_LDD_PICPARAM_T			 lPic;
	RIVP_LDD_U32				 lPicIndexToRelease = 0xEFEFEFEF;
	RIVP_LDD_BOOL				 lContinueDecode;
	RIVP_LDD_U32				 lOutputCount;
	RIVP_LDD_U32				 lOutputStartCnt;
	RIVP_LDD_U32				 lNumBufferingPic;
	RIVP_APP_LDD_CMN_PARAM_T	*lAppPtr;
	RIVP_LDD_AVC_CONFIG_T		*lAvcConfigPtr;
	RIVP_LDD_U8					 lOutMsg[RIVP_APP_LDD_SIZE_OUTPUT_MSG];
	RIVP_LDD_U8					 lMainMsg[RIVP_APP_LDD_SIZE_MAIN_DEC_MSG];
	RIVP_APP_DISP_PARAM_T		 lDispParam;

	RIVP_LDD_LOG_APP_INF("output task start.\n");

	/* check arguments */
	if (arg != RIVP_LDD_NULL) {
		lAppPtr  = (RIVP_APP_LDD_CMN_PARAM_T*)arg;
		lBoolRet             = RIVP_LDD_TRUE;
		lOutputCount         = 0u;

		/* determine output count to start output process */
		if (lAppPtr->ctxParam.config.codecType == RIVP_LDD_AVC_CODEC_TYPE) {
			lAvcConfigPtr    = (RIVP_LDD_AVC_CONFIG_T*)lAppPtr->codecConfig;
			lNumBufferingPic = lAppPtr->ctxParam.numBufferingPic;

			lOutputStartCnt  = (lNumBufferingPic == 0u) ? RIVP_APP_LDD_OUT_NO_BUFFERING : lNumBufferingPic;
			lOutputStartCnt  = (lAppPtr->displayMode == RIVP_LDD_TRUE) ? lOutputStartCnt : RIVP_APP_LDD_OUT_NO_BUFFERING;
			lOutputStartCnt  = (lAvcConfigPtr->reorderEnable == RIVP_LDD_TRUE) ? lOutputStartCnt : RIVP_APP_LDD_OUT_NO_BUFFERING; /* [T.B.D.] TRUE case is not tested */
		} else { /* ctxParam.config.codecType == RIVP_LDD_JPG_CODEC_TYPE */
			/* always no buffering */
			lOutputStartCnt  = RIVP_APP_LDD_OUT_NO_BUFFERING;
		}

		/* init display */
		/* set display parameters */
		/* relationship between unit and HW */
		/*  H3 & RIVP_APP_DISP_DST_PTN=0 [ APP1 : HDMI0, APP2 : HDMI1      ] */
		/*  H3 & RIVP_APP_DISP_DST_PTN=1 [ APP1 : HDMI0, APP2 : Analog RGB ] */
		/*  M3                           [ APP1 : HDMI0, APP2 : Analog RGB ] */
		/*  E3                           [ APP1 : Analog RGB               ] (APP2 not supported) */
		lDispParam.unit				= RIVP_APP_DISP_UNIT_SELECT;
		lDispParam.width			= CONFIG_STREAM_WIDTH;
		lDispParam.height			= CONFIG_STREAM_HEIGHT;
		lDispParam.pitch			= 2048;
		lDispParam.support422Format	= RIVP_LDD_FALSE;
		lBoolRet = RIVP_AppOutInit(&lDispParam);
		if (lBoolRet != RIVP_LDD_TRUE) {
			lResult = RIVP_LDD_RTN_FATAL_SYSTEM;
			/* RIVP_AppOutInit will output the error log. */
		}

		/* wait callback of picture out before output process */
		while ((lBoolRet == RIVP_LDD_TRUE) && (lOutputCount < lOutputStartCnt)) {
			lBoolRet = RIVP_AppLddOsalQueueGetMsgCnt(&lAppPtr->msgInf[RIVP_APP_LDD_PICOUT_MSG_QUEUE], &lOutputCount);
			RIVP_AppLddOsalUSleep(RIVP_APP_LDD_OUT_SLEEP_TIME);
		}

		if (lBoolRet == RIVP_LDD_TRUE) {
			lResult = RIVP_LDD_RTN_OK;
		} else {
			lResult = RIVP_LDD_RTN_FATAL_SYSTEM;
			RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddOutTask. Get message count error.\n");
		}

		/* continue output process while decoding or existing output data */
		while (((lAppPtr->decoding == RIVP_LDD_TRUE) || (lOutputCount >= lOutputStartCnt)) && (lResult == RIVP_LDD_RTN_OK)) {
			if (lOutputCount >= lOutputStartCnt) {
				lBoolRet = RIVP_AppLddOsalQueueReceive(&lAppPtr->msgInf[RIVP_APP_LDD_PICOUT_MSG_QUEUE], (RIVP_LDD_PTR)&lOutMsg[0]);
				memcpy(&lPic, &lOutMsg[0], sizeof(RIVP_LDD_PICPARAM_T));
				if (lBoolRet != RIVP_LDD_TRUE) {
					lResult = RIVP_LDD_RTN_FATAL_SYSTEM;
					RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddOutTask. Receive message error.\n");
				}

				if (lResult == RIVP_LDD_RTN_OK) {
					/* check output mode */
					if (lAppPtr->displayMode == RIVP_LDD_TRUE) {
						/* update display */
						lBoolRet = RIVP_AppDispUpdate(&lPic);
						if (lBoolRet != RIVP_LDD_TRUE)
						{
							lResult = RIVP_LDD_RTN_FATAL_SYSTEM;
							RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppOutTask. Update display error.\n");
						}
					}
				}

				if (lResult == RIVP_LDD_RTN_OK) {
					/* In case of non-display mode, release picture directly.
					 * In case of display mode, release picture until it released from display module */
					if (lAppPtr->displayMode == RIVP_LDD_FALSE) {
						/* release picture(RIVPD_BSC_User_Manual 4.2.4) */
						RIVP_AppLddCmnGetCurTime(&lAppPtr->perf.funcStart);
						lResult = RIVP_LddReleasePicture(lAppPtr->ctx, lPicIndexToRelease /*lPic.picIndex*/);
						RIVP_AppLddCmnGetProcTime(lAppPtr, RIVP_APP_LDD_FUNC_REL_PIC);
					}
					else {
						if (lPicIndexToRelease == 0xEFEFEFEF) {
							lPicIndexToRelease = lPic.picIndex;
							lResult = RIVP_LDD_RTN_OK;
						}
						else {
							/* release picture(RIVPD_BSC_User_Manual 4.2.4) */
							RIVP_AppLddCmnGetCurTime(&lAppPtr->perf.funcStart);
							lResult = RIVP_LddReleasePicture(lAppPtr->ctx, lPicIndexToRelease);
							RIVP_AppLddCmnGetProcTime(lAppPtr, RIVP_APP_LDD_FUNC_REL_PIC);
							lPicIndexToRelease = lPic.picIndex;
						}
					}
				}
			} else {
				/* there is no output data, but decode process has not finished yet */
				RIVP_AppLddOsalUSleep(RIVP_APP_LDD_OUT_SLEEP_TIME);
			}

			/* get output data count */
			if (lResult == RIVP_LDD_RTN_OK) {
				lBoolRet = RIVP_AppLddOsalQueueGetMsgCnt(&lAppPtr->msgInf[RIVP_APP_LDD_PICOUT_MSG_QUEUE], &lOutputCount);
				if (lBoolRet != RIVP_LDD_TRUE) {
					lResult = RIVP_LDD_RTN_FATAL_SYSTEM;
				}
			}
		}

		lBoolRet = RIVP_AppOutDeinit();

		if (lResult != RIVP_LDD_RTN_OK) {
			/* send message to notify error */
			memcpy(&lMainMsg[0], &lResult, sizeof(lResult));
			lBoolRet = RIVP_AppLddOsalQueueSend(&lAppPtr->msgInf[RIVP_APP_LDD_MAIN_MSG_QUEUE], &lMainMsg[0]);
		} else {
			/* send message to notify end of output */
			lContinueDecode = RIVP_LDD_FALSE;
			memcpy(&lMainMsg[0], &lContinueDecode, sizeof(lContinueDecode));
			lBoolRet = RIVP_AppLddOsalQueueSend(&lAppPtr->msgInf[RIVP_APP_LDD_DEC_MSG_QUEUE], &lMainMsg[0]);
		}

		if (lBoolRet != RIVP_LDD_TRUE) {
			RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddOutTask. Send message error.\n");
		}
	} else {
		RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddOutTask. Argument is NULL.\n");
	}

	RIVP_LDD_LOG_APP_INF("output task end.\n");

	return RIVP_LDD_RTN_OK;
}
