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
#include "rivp_app_ldd_osdep.h"
#include "rivp_app_ldd_mmgr.h"

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
	RIVP_LDD_BOOL				 lContinueDecode;
	RIVP_LDD_U32				 lOutputCount;
	RIVP_LDD_U32				 lOutputStartCnt;
	RIVP_LDD_U32				 lNumBufferingPic;
	RIVP_APP_LDD_CMN_PARAM_T	*lAppPtr;
	RIVP_LDD_AVC_CONFIG_T		*lAvcConfigPtr;
	RIVP_LDD_U8					 lOutMsg[RIVP_APP_LDD_SIZE_OUTPUT_MSG];
	RIVP_LDD_U8					 lMainMsg[RIVP_APP_LDD_SIZE_MAIN_DEC_MSG];

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
			lOutputStartCnt  = (lAvcConfigPtr->reorderEnable == RIVP_LDD_TRUE) ? lOutputStartCnt : RIVP_APP_LDD_OUT_NO_BUFFERING; /* [T.B.D.] TRUE case is not tested */
		} else { /* ctxParam.config.codecType == RIVP_LDD_JPG_CODEC_TYPE */
			/* always no buffering */
			lOutputStartCnt  = RIVP_APP_LDD_OUT_NO_BUFFERING;
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
					/* update tile(xy) */
					lBoolRet = RIVP_AppLddOutUpdateXy(lAppPtr, &lPic);

					/* output YUV data */
					if (lBoolRet == RIVP_LDD_TRUE) {
						lBoolRet = RIVP_AppLddOutOutputYuv(lAppPtr, &lPic);
					}

					if (lBoolRet == RIVP_LDD_TRUE) {
						/* wait next output */
						RIVP_AppLddOsalUSleep(RIVP_APP_LDD_OUT_SLEEP_TIME);
					} else {
						RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddOutTask. Output YUV data error.\n");
					}
				}

				if (lResult == RIVP_LDD_RTN_OK) {
					/* Invalidate the output buffer cache */
					RIVP_AppLddMmgrInvalidate((RIVP_LDD_PTR)RIVP_AppLddMmgrPtoV(lAppPtr->ctxParam.fmemPhysAddr));
				}

				if (lResult == RIVP_LDD_RTN_OK) {
					/* release picture */
					RIVP_AppLddCmnGetCurTime(&lAppPtr->perf.funcStart);
					lResult = RIVP_LddReleasePicture(lAppPtr->ctx, lPic.picIndex);
					RIVP_AppLddCmnGetProcTime(lAppPtr, RIVP_APP_LDD_FUNC_REL_PIC);
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

/******************************************************************************/
/*                    INTERNAL FUNCTIONS                                      */
/******************************************************************************/
/**
 * \brief			 update xy information
 *
 * \param[in,out]	 app					 : application control information
 * \param[in]		 pic					 : decoded picture parameter
 *
 * \retval			 RIVP_LDD_TRUE			 : success
 * \retval			 RIVP_LDD_FALSE			 : invalid argument
 *
 */
RIVP_LDD_STATIC RIVP_LDD_BOOL RIVP_AppLddOutUpdateXy(
	RIVP_APP_LDD_CMN_PARAM_T *app,
	RIVP_LDD_PICPARAM_T *pic)
{
	RIVP_LDD_BOOL lBoolRet;
	RIVP_LDD_S8	  lCharFmt[RIVP_LDD_PIC_COMPONENT_MAX][10] = {{""},{""}};

	/* set initial value */
	lBoolRet = RIVP_LDD_FALSE;

	/* check arguments */
	if ((app != RIVP_LDD_NULL) && (pic != RIVP_LDD_NULL)) {
		/* debug log */
		/* print result */
		RIVP_APP_LDD_FMT_TO_ASCII(pic->format[RIVP_LDD_PIC_COMPONENT_Y], lCharFmt[RIVP_LDD_PIC_COMPONENT_Y]);
		RIVP_APP_LDD_FMT_TO_ASCII(pic->format[RIVP_LDD_PIC_COMPONENT_C], lCharFmt[RIVP_LDD_PIC_COMPONENT_C]);
		RIVP_LDD_LOG_APP_INF("RIVP_LDD_CB_PICTUREOUT\n");
		RIVP_LDD_LOG_APP_INF(" - error                = 0x%08X\n",	pic->picError);
		RIVP_LDD_LOG_APP_INF(" - picIndex             = %d\n",		pic->picIndex);
		RIVP_LDD_LOG_APP_INF(" - formatY              = %s\n",		lCharFmt[RIVP_LDD_PIC_COMPONENT_Y]);
		RIVP_LDD_LOG_APP_INF(" - formatC              = %s\n",		lCharFmt[RIVP_LDD_PIC_COMPONENT_C]);
		RIVP_LDD_LOG_APP_INF(" - strmId[1st]          = %d\n",		pic->strmId[RIVP_LDD_STRM_FIELD_FIRST]);
		RIVP_LDD_LOG_APP_INF(" - strmId[2nd]          = %d\n",		pic->strmId[RIVP_LDD_STRM_FIELD_SECOND]);
		RIVP_LDD_LOG_APP_INF(" - decodedStrmSize[1st] = %d\n",		pic->decodedStrmSize[RIVP_LDD_STRM_FIELD_FIRST]);
		RIVP_LDD_LOG_APP_INF(" - decodedStrmSize[2nd] = %d\n",		pic->decodedStrmSize[RIVP_LDD_STRM_FIELD_SECOND]);

		/* set default crop */
		if (app->ctxParam.config.codecType == RIVP_LDD_AVC_CODEC_TYPE) {
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropTop		= pic->avcPic.cropTop;
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropBot		= pic->height - (pic->avcPic.cropBottom + 1u);
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropLeft		= pic->avcPic.cropLeft;
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropRight		= pic->width - (pic->avcPic.cropRight + 1u);
		} else { /* ctxParam.config.codecType == RIVP_LDD_JPG_CODEC_TYPE */
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropTop		= 0u;
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropBot		= 0u;
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropLeft		= 0u;
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropRight		= 0u;
		}

		app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropTop			= app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropTop;
		app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropBot			= app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropBot;
		app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropLeft		= app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropLeft;
		app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropRight		= app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropRight;

		/* luma setting */
		app->xy[RIVP_APP_LDD_CMN_XY_LUMA].bytePerPixel		= pic->bytePerPixel[RIVP_LDD_PIC_COMPONENT_Y];
		app->xy[RIVP_APP_LDD_CMN_XY_LUMA].xPicSize			= app->xy[RIVP_APP_LDD_CMN_XY_LUMA].bytePerPixel * RIVP_APP_LDD_ALIGN(pic->width,  2u);
		app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropLeft			= app->xy[RIVP_APP_LDD_CMN_XY_LUMA].bytePerPixel * app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropLeft;
		app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropRight			= app->xy[RIVP_APP_LDD_CMN_XY_LUMA].bytePerPixel * app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropRight;
		if (((RIVP_LDD_U32)pic->format[RIVP_LDD_PIC_COMPONENT_Y] & RIVP_APP_LDD_PIC_FMT_CHROMA_MASK) == RIVP_APP_LDD_PIC_FMT_420) {	/* YCbCr420 */
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA].yPicSize		= RIVP_APP_LDD_ALIGN(pic->height, 2u);
		} else {
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA].yPicSize		= pic->height;
		}
		app->xy[RIVP_APP_LDD_CMN_XY_LUMA].stride			= pic->stride;
		app->xy[RIVP_APP_LDD_CMN_XY_LUMA].xyTopAddr			= (RIVP_LDD_PTR)RIVP_AppLddMmgrPtoV((RIVP_LDD_ADR)pic->picBuff[RIVP_LDD_PIC_FIELD_TOP].yPhysAddr);
		app->xy[RIVP_APP_LDD_CMN_XY_LUMA].xyBotAddr			= (RIVP_LDD_PTR)RIVP_AppLddMmgrPtoV((RIVP_LDD_ADR)pic->picBuff[RIVP_LDD_PIC_FIELD_BOT].yPhysAddr);

		/* chroma setting */
		app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].bytePerPixel	= pic->bytePerPixel[RIVP_LDD_PIC_COMPONENT_C];
		app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].xPicSize		= app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].bytePerPixel * RIVP_APP_LDD_ALIGN(pic->width,  2u);
		app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropLeft		= app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].bytePerPixel * app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropLeft;
		app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropRight		= app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].bytePerPixel * app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropRight;
		if (((RIVP_LDD_U32)pic->format[RIVP_LDD_PIC_COMPONENT_C] & RIVP_APP_LDD_PIC_FMT_CHROMA_MASK) == RIVP_APP_LDD_PIC_FMT_420) {	/* YCbCr420 */
			app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].yPicSize	= RIVP_APP_LDD_ALIGN(pic->height, 2u)     >> 1u;
			app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropTop		= app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropTop >> 1u;
			app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropBot		= app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropBot >> 1u;
		} else {
			app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].yPicSize	= pic->height;
		}
		app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].stride			= pic->stride;
		app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].xyTopAddr		= (RIVP_LDD_PTR)RIVP_AppLddMmgrPtoV((RIVP_LDD_ADR)pic->picBuff[RIVP_LDD_PIC_FIELD_TOP].cPhysAddr);
		app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].xyBotAddr		= (RIVP_LDD_PTR)RIVP_AppLddMmgrPtoV((RIVP_LDD_ADR)pic->picBuff[RIVP_LDD_PIC_FIELD_BOT].cPhysAddr);

		/* frame/field */
		if (pic->picStruct == RIVP_LDD_PIC_STRUCT_FRAME) {
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA].xyOnField		= 0u;
			app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].xyOnField	= 0u;
		} else {
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA].xyOnField		= 1u;
			app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].xyOnField	= 1u;
		}

		/* tile/linear */
		if (((RIVP_LDD_U32)pic->format[RIVP_LDD_PIC_COMPONENT_Y] & RIVP_APP_LDD_PIC_FMT_TL_MASK) == RIVP_APP_LDD_PIC_FMT_TILE) {
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA].xyLineMode	= RIVP_APP_LDD_CMN_XY_16X2L;
		} else {
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA].xyLineMode	= RIVP_APP_LDD_CMN_LN_OUT;
		}
		if (((RIVP_LDD_U32)pic->format[RIVP_LDD_PIC_COMPONENT_C] & RIVP_APP_LDD_PIC_FMT_TL_MASK) == RIVP_APP_LDD_PIC_FMT_TILE) {
			app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].xyLineMode	= RIVP_APP_LDD_CMN_XY_16X2L;
		} else {
			app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].xyLineMode	= RIVP_APP_LDD_CMN_LN_OUT;
		}

		/* for gen3 option */
		if (pic->avcPic.reserved == 1u) {
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA]._2lineShift	= 1u;
			app->xy[RIVP_APP_LDD_CMN_XY_CHROMA]._2lineShift	= 1u;
		} else {
			app->xy[RIVP_APP_LDD_CMN_XY_LUMA]._2lineShift	= 0u;
			app->xy[RIVP_APP_LDD_CMN_XY_CHROMA]._2lineShift	= 0u;
		}

		lBoolRet = RIVP_LDD_TRUE;
	}

	return lBoolRet;
}

/**
 * \brief			 output YUV data
 *
 * \param[in,out]	 app					 : application control information
 * \param[in]		 pic					 : decoded picture parameter
 *
 * \retval			 RIVP_LDD_TRUE			 : success
 * \retval			 RIVP_LDD_FALSE			 : invalid argument
 *
 */
RIVP_LDD_STATIC RIVP_LDD_BOOL RIVP_AppLddOutOutputYuv(
	RIVP_APP_LDD_CMN_PARAM_T *app, 
	RIVP_LDD_PICPARAM_T *pic)
{
	RIVP_LDD_BOOL	 lBoolRet;
	RIVP_LDD_U32	 lYPicSize;
	RIVP_LDD_U32	 lCPicSize;

	/* set initial value */
	lBoolRet = RIVP_LDD_FALSE;

	/* check arguments */
	if ((app != RIVP_LDD_NULL) && (pic != RIVP_LDD_NULL)) {
		if (pic->picIndex < RIVP_APP_LDD_PIC_IDX_INVALID_STRM) {
			/* convert decoded picture to linear format */
			RIVP_AppLddCmnOutputData(app, app->file);

			/* copy YUV result to YUV buffer */
			if (app->strmInfo.strmId <= app->streamFrameId) {
				lYPicSize = (app->xy[RIVP_APP_LDD_CMN_XY_LUMA].xPicSize - (app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropLeft + app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropRight))
							 * (app->xy[RIVP_APP_LDD_CMN_XY_LUMA].yPicSize - (app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropTop + app->xy[RIVP_APP_LDD_CMN_XY_LUMA].cropBot));
				lCPicSize = ((app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].xPicSize - (app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropLeft + app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropRight))
							 * (app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].yPicSize - (app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropTop + app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].cropBot))) >> 1u;

				RIVP_AppLddOsalMemCpy(&app->yuvBuff[app->yuvBuffCnt], app->xy[RIVP_APP_LDD_CMN_XY_LUMA].lAddr[0], lYPicSize);
				app->yuvBuffCnt += lYPicSize;
				RIVP_AppLddOsalMemCpy(&app->yuvBuff[app->yuvBuffCnt], app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].lAddr[0], lCPicSize);
				app->yuvBuffCnt += lCPicSize;
				RIVP_AppLddOsalMemCpy(&app->yuvBuff[app->yuvBuffCnt], app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].lAddr[1], lCPicSize);
				app->yuvBuffCnt += lCPicSize;

				/* update result */
				lBoolRet = RIVP_LDD_TRUE;
			}
		}
	}

	return lBoolRet;
}
