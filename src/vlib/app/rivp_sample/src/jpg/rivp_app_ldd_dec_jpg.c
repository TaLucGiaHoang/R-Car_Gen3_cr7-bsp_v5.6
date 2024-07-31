/******************************************************************************
 *
 * PURPOSE
 *   RIVP Reference Application for decode part
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *
 ******************************************************************************/
/******************************************************************************
* JPG decode part
* Copyright (C) 2017 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
#include "rivp_ldd_jpg.h"
#include "rivp_app_ldd_dec.h"
#include "rivp_app_ldd_cmn_local.h"

#include "rivp_ldd_api.h"
#include "rivp_app_ldd_data_jpg.h"

#include "rivp_app_ldd_osdep.h"
#include "rivp_app_ldd_mmgr.h"
#include "rivp_dep_ldd_log.h"

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
/* macro for search of startcode. */
#define RIVP_APP_LDD_JPG_START_CODE				(0xFFD80000u)	/* the value of startcode */
#define RIVP_APP_LDD_JPG_MASK_START_CODE		(0xFFFF0000u)	/* the mask value of startcode */
#define RIVP_APP_LDD_JPG_SC_BITS				(16u)			/* the number of the bits of startcode. */
#define RIVP_APP_LDD_JPG_SLEEP_TIME				(500u)

/******************************************************************************/
/*                    FORWARD DECLARATION                                     */
/******************************************************************************/
RIVP_LDD_STATIC RIVP_LDD_RESULT	 RIVP_AppLddDecJpgGetCtxProc		(RIVP_APP_LDD_CMN_PARAM_T *app);
RIVP_LDD_STATIC RIVP_LDD_RESULT	 RIVP_AppLddDecJpgFreeCtxProc		(RIVP_APP_LDD_CMN_PARAM_T *app);
RIVP_LDD_STATIC RIVP_LDD_BOOL	 RIVP_AppLddDecJpgInitCtxParam		(RIVP_APP_LDD_CMN_PARAM_T *app);
RIVP_LDD_STATIC RIVP_LDD_RESULT	 RIVP_AppLddDecJpgInput				(RIVP_APP_LDD_CMN_PARAM_T *app);
RIVP_LDD_STATIC RIVP_LDD_RESULT	 RIVP_AppLddDecJpgRecovery			(RIVP_APP_LDD_CMN_PARAM_T *app);
RIVP_LDD_STATIC RIVP_LDD_BOOL	 RIVP_AppLddDecJpgSearchStartCode	(RIVP_APP_LDD_CMN_PARAM_T *app);

/* callback from RIVP */
RIVP_LDD_STATIC void		 RIVP_AppLddDecJpgCbPicOut				(RIVP_LDD_PTR context, RIVP_LDD_PTR udptr, RIVP_LDD_PICPARAM_T *pic);
RIVP_LDD_STATIC void		 RIVP_AppLddDecJpgCbError				(RIVP_LDD_PTR context, RIVP_LDD_PTR udptr, RIVP_LDD_RESULT errorCode, RIVP_LDD_U32 errorInfo);
RIVP_LDD_STATIC void		 RIVP_AppLddDecJpgCbStrmBufFree			(RIVP_LDD_PTR context, RIVP_LDD_PTR udptr, RIVP_LDD_INPUTPARAM_T *inParam);

/******************************************************************************/
/*                    EXTERNAL FUNCTIONS                                      */
/******************************************************************************/
/**
 * \brief			 task of decode process
 *
 * \param[in,out]	 arg			: application control information
 *
 * \retval			 void			: none
 *
 */
void *RIVP_AppLddDecTask(RIVP_LDD_PTR arg)
{
	RIVP_LDD_RESULT				 lResult;
	RIVP_LDD_BOOL				 lBoolRet;
	RIVP_APP_LDD_CMN_PARAM_T	*lAppPtr;
	RIVP_LDD_BOOL				 lContinueWait;
	RIVP_LDD_U8					 lMsg[RIVP_APP_LDD_SIZE_MAIN_DEC_MSG];

	RIVP_LDD_LOG_APP_INF("jpg decode task start.\n");

	/* check argumetns */
	if (arg != RIVP_LDD_NULL) {
		lAppPtr = (RIVP_APP_LDD_CMN_PARAM_T*)arg;

		/* set initial value */
		lBoolRet      = RIVP_LDD_TRUE;
		lContinueWait = RIVP_LDD_TRUE;

		/* get context */
		lResult = RIVP_AppLddDecJpgGetCtxProc(lAppPtr);

		/* input stream */
		if (lResult == RIVP_LDD_RTN_OK) {
			lResult = RIVP_AppLddDecJpgInput(lAppPtr);
		}

		/* wait end of output */
		if (lResult == RIVP_LDD_RTN_OK) {
			while ((lBoolRet == RIVP_LDD_TRUE) && (lContinueWait == RIVP_LDD_TRUE)) {
				lBoolRet = RIVP_AppLddOsalQueueReceive(&lAppPtr->msgInf[RIVP_APP_LDD_DEC_MSG_QUEUE], (RIVP_LDD_PTR)&lMsg[0]);
				memcpy(&lContinueWait, &lMsg[0], sizeof(lContinueWait));
			}

			if (lBoolRet != RIVP_LDD_TRUE) {
				lResult = RIVP_LDD_RTN_FATAL_SYSTEM;
				RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecTask. Receive message error.\n");
			}
		}

		/* free context */
		if (lResult == RIVP_LDD_RTN_OK) {
			lResult = RIVP_AppLddDecJpgFreeCtxProc(lAppPtr);
		}

		/* send message main message queue */
		memcpy(&lMsg[0], &lResult, sizeof(lResult));
		lBoolRet = RIVP_AppLddOsalQueueSend(&lAppPtr->msgInf[RIVP_APP_LDD_MAIN_MSG_QUEUE], (RIVP_LDD_PTR)&lMsg[0]);
		if (lBoolRet != RIVP_LDD_TRUE) {
			RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecTask. Send message error.\n");
		}
	} else {
		RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecTask. Argument is NULL.\n");
	}

	RIVP_LDD_LOG_APP_INF("decode task end.\n");

	return RIVP_LDD_RTN_OK;
}


/******************************************************************************/
/*                    INTERNAL FUNCTIONS                                      */
/******************************************************************************/
/**
 * \brief			 get a context
 *
 * \param[in,out]	 app						: application control information
 *
 * \retval			 RIVP_LDD_RTN_OK			: success
 * \retval			 RIVP_LDD_RTN_WARNING		: warning error
 * \retval			 RIVP_LDD_RTN_FATAL_LIBRARY	: library error
 * \retval			 RIVP_LDD_RTN_FATAL_SYSTEM	: system(like a os dependent) error
 *
 */
RIVP_LDD_STATIC RIVP_LDD_RESULT RIVP_AppLddDecJpgGetCtxProc(RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_RESULT	 lResult;
	RIVP_LDD_BOOL	 lBoolRet;

	/* set initial value */
	lResult = RIVP_LDD_RTN_FATAL_SYSTEM;

	/* check arguments */
	if (app != RIVP_LDD_NULL) {
		/* initialize for JPG */
		lBoolRet = RIVP_AppLddDecJpgInitCtxParam(app);

		if (lBoolRet == RIVP_LDD_TRUE) {
			
			/* get memory size */
			RIVP_AppLddCmnGetCurTime(&app->perf.funcStart);
			lResult = RIVP_LddGetMemSize(&app->ctxParam, &app->ctxParam.fmemSize, &app->ctxParam.hwWork.size);
			RIVP_AppLddCmnGetProcTime(app, RIVP_APP_LDD_FUNC_GETMEM);
			
			RIVP_LDD_LOG_APP_INF("RIVP_LddGetMemSize() = %s\n", g_rivpAppLddCmnResCode[(RIVP_LDD_U32)lResult]);

			/* allocate RIVP resource(stream/frame memory) from mmngr. */
			if (lResult == RIVP_LDD_RTN_OK) {
				RIVP_LDD_LOG_APP_INF(" - fmemSize = 0x%08X\n", app->ctxParam.fmemSize);
				RIVP_LDD_LOG_APP_INF(" - workSize = 0x%08X\n", app->ctxParam.hwWork.size);

				lBoolRet = RIVP_AppLddCmnMemAllocOsal(app, app->file);
				if (lBoolRet != RIVP_LDD_TRUE) {
					RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecJpgGetCtxProc(). Memory allocation error.\n");
					lResult = RIVP_LDD_RTN_FATAL_SYSTEM;
				}
			}

			if (lResult == RIVP_LDD_RTN_OK) {
				
				/* create context */
				RIVP_AppLddCmnGetCurTime(&app->perf.funcStart);
				lResult = RIVP_LddGetContext(&app->ctxParam, &app->ctx);
				RIVP_AppLddCmnGetProcTime(app, RIVP_APP_LDD_FUNC_GET_CTX);
				
				RIVP_LDD_LOG_APP_INF("RIVP_LddGetContext() = %s\n", g_rivpAppLddCmnResCode[(RIVP_LDD_U32)lResult]);
			}
		}
	}

	/* return result */
	return lResult;
}

/**
 * \brief			 free a context
 *
 * \param[in]		 app						: application control information
 *
 * \retval			 RIVP_LDD_RTN_OK			: success
 * \retval			 RIVP_LDD_RTN_FATAL_LIBRARY	: library error
 * \retval			 RIVP_LDD_RTN_FATAL_SYSTEM	: system(like a os dependent) error
 *
 */
RIVP_LDD_STATIC RIVP_LDD_RESULT RIVP_AppLddDecJpgFreeCtxProc(RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_RESULT	 lResult;

	/* set initial value */
	lResult = RIVP_LDD_RTN_FATAL_SYSTEM;

	/* check arguments */
	if (app != RIVP_LDD_NULL) {
		do {
			/* free context */
			RIVP_AppLddCmnGetCurTime(&app->perf.funcStart);
			lResult = RIVP_LddFreeContext(app->ctx);
			RIVP_AppLddCmnGetProcTime(app, RIVP_APP_LDD_FUNC_FREE_CTX);
			
		} while (lResult == RIVP_LDD_RTN_WARNING);

		RIVP_LDD_LOG_APP_INF("RIVP_LddFreeContext() = %s\n", g_rivpAppLddCmnResCode[(RIVP_LDD_U32)lResult]);

		/* free RIVP resource(stream/frame memory) from mmngr. */
		RIVP_AppLddCmnMemFreeOsal(app);
	}

	return lResult;
}

/**
 * \brief			 initialize parameter for context
 *
 * \param[in,out]	 app						: application control information
 *
 * \retval			 RIVP_LDD_TRUE				: success
 * \retval			 RIVP_LDD_FALSE				: invalid argument
 *
 */
RIVP_LDD_STATIC RIVP_LDD_BOOL RIVP_AppLddDecJpgInitCtxParam(RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_BOOL			 lBoolRet;
	RIVP_LDD_JPG_CONFIG_T	*lJpgConfigPtr;

	/* set initial value */
	lBoolRet = RIVP_LDD_FALSE;

	/* check arguments */
	if (app != RIVP_LDD_NULL) {
		/* set result */
		lBoolRet = RIVP_LDD_TRUE;

		lJpgConfigPtr = (RIVP_LDD_JPG_CONFIG_T*)app->codecConfig;

		/* set ctxParam */
		/* set codecConfig */
		lJpgConfigPtr->normalDecode				= RIVP_LDD_TRUE;					/* not support, only RIVP_LDD_TRUE is setable. */
		lJpgConfigPtr->outputLinearFormat		= RIVP_LDD_TRUE;					/* TRUE,FALSE */

		/* set config */
		app->ctxParam.config.codecType			= RIVP_LDD_JPG_CODEC_TYPE;
		app->ctxParam.config.codecConfig		= lJpgConfigPtr;
		app->ctxParam.config.codecConfigSize	= sizeof(RIVP_LDD_JPG_CONFIG_T);
		app->ctxParam.config.support12bitDepth	= RIVP_APP_LDD_JPG_DATA_STRM_12BIT;		/* TRUE,FALSE */
		app->ctxParam.config.support422Format	= RIVP_APP_LDD_JPG_DATA_STRM_422;		/* TRUE,FALSE */
		app->ctxParam.config.intCnt				= RIVP_LDD_MBLINE_INT_NONE;				/* not support, only INT_NONE is setable. */
		app->ctxParam.ch 						= 0u;									/* 0 */
		app->ctxParam.maxWidth					= RIVP_APP_LDD_JPG_DATA_STRM_WIDTH;		/* 80-1920 */
		app->ctxParam.maxHeight					= RIVP_APP_LDD_JPG_DATA_STRM_HEIGHT;	/* 80-1088 */
		app->ctxParam.numBufferingPic			= RIVP_APP_LDD_JPG_DATA_BUFFERING;		/* 0-3 */
		app->ctxParam.cbStrmFree				= &RIVP_AppLddDecJpgCbStrmBufFree;
		app->ctxParam.cbPicOut					= &RIVP_AppLddDecJpgCbPicOut;
		app->ctxParam.cbError					= &RIVP_AppLddDecJpgCbError;
		app->ctxParam.udptr						= app;

		RIVP_LDD_LOG_APP_INF("ctxParam setting:\n");
		RIVP_LDD_LOG_APP_INF(" - codecType          = %s\n", g_rivpAppLddCmnCodec[app->ctxParam.config.codecType]);
		RIVP_LDD_LOG_APP_INF(" - intCnt             = %s\n", g_rivpAppLddCmnIntCnt[(RIVP_LDD_U32)app->ctxParam.config.intCnt]);
		RIVP_LDD_LOG_APP_INF(" - support12bitDepth  = %s\n", g_rivpAppLddCmnBool[(RIVP_LDD_U32)app->ctxParam.config.support12bitDepth]);
		RIVP_LDD_LOG_APP_INF(" - support422Format   = %s\n", g_rivpAppLddCmnBool[(RIVP_LDD_U32)app->ctxParam.config.support422Format]);
		RIVP_LDD_LOG_APP_INF(" - BufferingPic       = %d\n", app->ctxParam.numBufferingPic);
		RIVP_LDD_LOG_APP_INF("jpgConfig setting:\n");
		RIVP_LDD_LOG_APP_INF(" - normalDecode       = %s\n", g_rivpAppLddCmnBool[(RIVP_LDD_U32)lJpgConfigPtr->normalDecode]);
		RIVP_LDD_LOG_APP_INF(" - outputLinearFormat = %s\n", g_rivpAppLddCmnBool[(RIVP_LDD_U32)lJpgConfigPtr->outputLinearFormat]);

		/* set maxOneInputSize for this application(in the case of JPEG, this is fail safe) */
		app->maxOneInputSize = app->ctxParam.maxWidth * app->ctxParam.maxHeight;
		app->maxOneInputSize = (app->ctxParam.config.support12bitDepth == RIVP_LDD_TRUE) ? (app->maxOneInputSize * 2u) : app->maxOneInputSize;				/* 10/12bit depth */
		app->maxOneInputSize = (app->ctxParam.config.support422Format  == RIVP_LDD_TRUE) ? (app->maxOneInputSize * 2u) : ((app->maxOneInputSize * 3u)/2u);	/* YCbCr422 */
	}

	return lBoolRet;
}

/**
 * \brief			 capture a picture / input a stream
 *
 * \param[in,out]	 app						: application control information
 *
 * \retval			 RIVP_LDD_RTN_OK			: success
 * \retval			 RIVP_LDD_RTN_FATAL_LIBRARY	: library error
 * \retval			 RIVP_LDD_RTN_FATAL_SYSTEM	: system(like a os dependent) error
 *
 */
RIVP_LDD_STATIC RIVP_LDD_RESULT RIVP_AppLddDecJpgInput(RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_RESULT	 lResult;
	RIVP_LDD_BOOL	 lBoolRet;
	RIVP_LDD_BOOL	 lDecoding;
	RIVP_LDD_U8		 lMsg[RIVP_APP_LDD_SIZE_MAIN_DEC_MSG];

	/* set initial value */
	lResult = RIVP_LDD_RTN_FATAL_SYSTEM;

	/* check arguments */
	if (app != RIVP_LDD_NULL) {
		app->decoding = RIVP_LDD_TRUE;
		lDecoding = RIVP_LDD_TRUE;

		while (lDecoding == RIVP_LDD_TRUE) {
			/* check error code */
			if (app->errorCode != RIVP_LDD_RTN_OK) {
				/* do recovery process */
				lResult = RIVP_AppLddDecJpgRecovery(app);
				if (lResult != RIVP_LDD_RTN_OK) {
					RIVP_LDD_LOG_APP_ERR_SYS("!! Error recovery process. !!\n");
					lDecoding = RIVP_LDD_FALSE;
				}
			} else {
				if (app->firstInput == RIVP_LDD_TRUE) {
					/* wait until RIVP_AppLddDecJpgCbStrmBufFree is called */
					lBoolRet = RIVP_AppLddOsalQueueReceive(&app->msgInf[RIVP_APP_LDD_STRMFREE_MSG_QUEUE], &lMsg[0]);
					memcpy(&lDecoding, &lMsg[0], sizeof(lDecoding));
					/* Wait for the callback function to exit completely. */
					RIVP_AppLddOsalUSleep(RIVP_APP_LDD_JPG_SLEEP_TIME);
				} else {
					/* skip receive process on first decode */
					lBoolRet = RIVP_LDD_TRUE;
				}

				if (lBoolRet == RIVP_LDD_TRUE) {
					/* create RIVP_LDD_INPUTPARAM_T for RIVP_LddInput. */
					/* update stream information */
					RIVP_AppLddCmnUpdateInputParam(app, app->file);
					/* search picture boundary, and create app->input from app->strmInfo */
					lDecoding = RIVP_AppLddDecJpgSearchStartCode(app);
				} else {
					RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecJpgInput. Receive message error.\n");
					lDecoding = RIVP_LDD_FALSE;
				}

				if (lDecoding == RIVP_LDD_TRUE) {
					/* input */
					app->firstInput = RIVP_LDD_TRUE;
					RIVP_AppLddCmnGetCurTime(&app->perf.funcStart);
					lResult = RIVP_LddInput(app->ctx, &app->input);
					RIVP_AppLddCmnGetProcTime(app, RIVP_APP_LDD_FUNC_INPUT);

					RIVP_LDD_LOG_APP_INF("RIVP_LddInput(strmId=%d) = %s\n", app->input.strmId, g_rivpAppLddCmnResCode[(RIVP_LDD_U32)lResult]);

					if ((lResult != RIVP_LDD_RTN_OK) && (lResult != RIVP_LDD_RTN_WARNING)) {
						/* in this case because callback CbError is not notified(such as context of argument is NULL), check the return value for error process. */
						app->errorCode = lResult;
					}
				} else {
					/* end of decode */
					app->decoding = RIVP_LDD_FALSE;
				}
			}
		}
	}

	return lResult;
}

/**
 * \brief			 recovery process
 *
 * \param[in,out]	 app						: application control information
 *
 * \retval			 RIVP_LDD_RTN_OK			: success
 * \retval			 RIVP_LDD_RTN_FATAL_LIBRARY	: library error
 * \retval			 RIVP_LDD_RTN_FATAL_SYSTEM	: system(like a os dependent) error
 *
 */
RIVP_LDD_STATIC RIVP_LDD_RESULT RIVP_AppLddDecJpgRecovery(RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_RESULT		 lResult;

	/* set initial value */
	lResult = RIVP_LDD_RTN_FATAL_SYSTEM;

	/* check arguments */
	if (app != RIVP_LDD_NULL) {
		/* error process */
		switch (app->errorCode) {
		case RIVP_LDD_RTN_FATAL_CONTEXT:
			/* remake context and start decoding with new SPS. */
			/* free the context that an error occurred */
			do {
				lResult = RIVP_LddFreeContext(app->ctx);
			} while (lResult == RIVP_LDD_RTN_WARNING);

			if (lResult == RIVP_LDD_RTN_OK) {
				/* initialize app ctrl parameters. */
				app->firstInput			= RIVP_LDD_FALSE;
				app->picIndex			= RIVP_APP_LDD_PIC_IDX_INVALID_STRM;	/* skip output yuv process */
				app->strmInfo.strmId	= 0u;
				app->yuvBuffCnt			= 0u;

				/* remake context with same setting */
				/* if user makes ctxParam different setting, please set ctxParam of new setting. */
				lResult = RIVP_LddGetContext(&app->ctxParam, &app->ctx);
				/* When the user wants to protect displaying picture, the user will be necessary like a following process. */
				/* - copy an displaying picture in another memory area */
				/* - allocate the fmemPhysAddr newly. And free the previous fmemPhysAddr after new picture displaying. */
			}

			/* update error code */
			app->errorCode = lResult;
			RIVP_LDD_LOG_APP_INF("!! Error recovery process: remake context = %s !!\n", g_rivpAppLddCmnResCode[(RIVP_LDD_U32)lResult]);
			break;

		case RIVP_LDD_RTN_FATAL_LIBRARY:
			/* restart rivp library */
			/* deinitialize rivp library(include freeing of all context) */
			lResult = RIVP_LddDeinitialize(RIVP_LDD_FALSE);
			if (lResult == RIVP_LDD_RTN_OK) {
				/* initialize rivp library */
				lResult = RIVP_LddInitialize(&app->initParam, &app->osCfg);
			}

			if (lResult == RIVP_LDD_RTN_OK) {
				/* initialize app ctrl parameters. */
				app->firstInput			= RIVP_LDD_FALSE;
				app->picIndex			= RIVP_APP_LDD_PIC_IDX_INVALID_STRM;
				app->strmInfo.strmId	= 0u;
				app->yuvBuffCnt			= 0u;

				/* remake context with same setting */
				/* if user makes ctxParam different setting, please set ctxParam of new setting. */
				lResult = RIVP_LddGetContext(&app->ctxParam, &app->ctx);
				/* When the user wants to protect displaying picture, the user will be necessary like a following process. */
				/* - copy an displaying picture in another memory area */
				/* - allocate the fmemPhysAddr newly. And free the previous fmemPhysAddr after new picture displaying. */
			}

			/* update error code */
			app->errorCode = lResult;
			RIVP_LDD_LOG_APP_INF("!! Error recovery process: restart library = %s !!\n", g_rivpAppLddCmnResCode[(RIVP_LDD_U32)lResult]);
			break;

		case RIVP_LDD_RTN_FATAL_SYSTEM:
		default:
			/* forced termination of decode process */
			/* this state will be caused by OS dependent error, so it is necessary to reboot the system. */
			app->decoding = RIVP_LDD_FALSE;
			lResult = app->errorCode;
			RIVP_LDD_LOG_APP_INF("!! Error recovery process: decode process terminate by FATAL_SYSTEM !!\n");
			break;
		}
	}

	return lResult;
}

/*** RIVP callback function ***/
/**
 * \brief			callback function of picture output
 *
 * \param[in]		ctx		 : context (unused)
 * \param[in]		udptr	 : user defined pointer(RIVP_APP_LDD_CMN_PARAM_T in this application)
 * \param[in]		pic		 : decoded picture parameter
 *
 * \retval			none
 *
 * \attention 		only a check using rivp_app_strm_jpg_cjpeg_000.h. other cases are unconfirmed.
 *
 */
RIVP_LDD_STATIC void RIVP_AppLddDecJpgCbPicOut(RIVP_LDD_PTR ctx, RIVP_LDD_PTR udptr, RIVP_LDD_PICPARAM_T *pic)
{
	RIVP_APP_LDD_CMN_PARAM_T	*lAppPtr;
	RIVP_LDD_BOOL				 lBoolRet;
	RIVP_LDD_U8					 lMsg[RIVP_APP_LDD_SIZE_OUTPUT_MSG];

	if ((udptr != RIVP_LDD_NULL) && (pic != RIVP_LDD_NULL)) {
		lAppPtr = (RIVP_APP_LDD_CMN_PARAM_T*)udptr;

		/* set performance information(from RIVP_LddInput to CbPicOut) */
		RIVP_AppLddCmnGetProcTime(lAppPtr, RIVP_APP_LDD_FUNC_PICOUT);
		if (lAppPtr->perf.funcCnt[RIVP_APP_LDD_FUNC_FIRST_PIC] == 0u) {
			RIVP_AppLddCmnGetProcTime(lAppPtr, RIVP_APP_LDD_FUNC_FIRST_PIC);
		}

		if ((pic->picError & RIVP_LDD_PIC_ERROR_MBL_INT) != 0u) {
			/* not support */
		} else {

			/* send message */
			memcpy(&lMsg[0], (RIVP_LDD_PTR)pic, sizeof(RIVP_LDD_PICPARAM_T));
			lBoolRet = RIVP_AppLddOsalQueueSend(&lAppPtr->msgInf[RIVP_APP_LDD_PICOUT_MSG_QUEUE], (RIVP_LDD_PTR)&lMsg[0]);
			if (lBoolRet != RIVP_LDD_TRUE) {
				RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecJpgCbPicOut. Send message error.\n");
			}
		}
	} else {
		RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecJpgCbPicOut. Argument is NULL.\n");
	}
}

/**
 * \brief			callback function of error
 *
 * \param[in]		ctx			 : context (unused)
 * \param[in,out]	udptr		 : user defined pointer(RIVP_APP_LDD_CMN_PARAM_T in this application)
 * \param[in]		errorCode	 : detected error code
 * \param[in]		errorInfo	 : error information
 *
 * \retval			none
 *
 * \attention 		only a check using rivp_app_strm_jpg_cjpeg_000.h. other cases are unconfirmed.
 *
 */
RIVP_LDD_STATIC void RIVP_AppLddDecJpgCbError(RIVP_LDD_PTR ctx, RIVP_LDD_PTR udptr, RIVP_LDD_RESULT errorCode, RIVP_LDD_U32 errorInfo)
{
	RIVP_APP_LDD_CMN_PARAM_T	*lAppPtr;

	/* check arguments */
	if (udptr != RIVP_LDD_NULL) {
		lAppPtr = (RIVP_APP_LDD_CMN_PARAM_T*)udptr;

		/* stores the errorCode for error process */
		if (errorCode == RIVP_LDD_RTN_WARNING) {
			RIVP_LDD_LOG_APP_INF("RIVP_LDD_CB_FATAL_ERROR - invalid stream\n");

		} else if (errorCode == RIVP_LDD_RTN_FATAL_CONTEXT) {
			lAppPtr->errorCode = errorCode;
			RIVP_LDD_LOG_APP_ERR_CTX("RIVP_LDD_CB_FATAL_ERROR - code = %s\n", g_rivpAppLddCmnResCode[(RIVP_LDD_U32)errorCode]);

		} else if (errorCode == RIVP_LDD_RTN_FATAL_LIBRARY) {
			lAppPtr->errorCode = errorCode;
			RIVP_LDD_LOG_APP_ERR_LIB("RIVP_LDD_CB_FATAL_ERROR - code = %s\n", g_rivpAppLddCmnResCode[(RIVP_LDD_U32)errorCode]);

		} else if (errorCode == RIVP_LDD_RTN_FATAL_SYSTEM) {
			lAppPtr->errorCode = errorCode;
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_LDD_CB_FATAL_ERROR - code = %s\n", g_rivpAppLddCmnResCode[(RIVP_LDD_U32)errorCode]);

		} else {
			/* unknown error */
		}
	} else {
		RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecJpgCbError(). Argument is NULL.\n");
	}
}

/**
 * \brief			callback function of stream buffer free
 *
 * \param[in]		ctx		 : context (unused)
 * \param[in]		udptr	 : user defined pointer(RIVP_APP_LDD_CMN_PARAM_T in this application)
 * \param[in]		inParam	 : input parameter(the argument of RIVP_LddInput. only size is updated.)
 *
 * \retval			none
 *
 * \attention 		only a check using rivp_app_strm_jpg_cjpeg_000.h. other cases are unconfirmed.
 *
 */
RIVP_LDD_STATIC void RIVP_AppLddDecJpgCbStrmBufFree(RIVP_LDD_PTR ctx, RIVP_LDD_PTR udptr, RIVP_LDD_INPUTPARAM_T *inParam)
{
	RIVP_APP_LDD_CMN_PARAM_T	*lAppPtr;
	RIVP_LDD_BOOL				 lBoolRet;
	RIVP_LDD_BOOL				 lContinueDecode;
	RIVP_LDD_U8					 lMsg[RIVP_APP_LDD_SIZE_MAIN_DEC_MSG];

	/* check arguments */
	if ((udptr != RIVP_LDD_NULL) && (inParam != RIVP_LDD_NULL)) {
		lAppPtr = (RIVP_APP_LDD_CMN_PARAM_T*)udptr;

		/* set picture information */
		lAppPtr->decodedStrmSize[RIVP_LDD_STRM_FIELD_FIRST]  = inParam->strmBuff[RIVP_LDD_STRM_FIELD_FIRST].size;
		lAppPtr->decodedStrmSize[RIVP_LDD_STRM_FIELD_SECOND] = inParam->strmBuff[RIVP_LDD_STRM_FIELD_SECOND].size;

		/* send message */
		lContinueDecode = RIVP_LDD_TRUE;
		memcpy(&lMsg[0], &lContinueDecode, sizeof(lContinueDecode));
		lBoolRet = RIVP_AppLddOsalQueueSend(&lAppPtr->msgInf[RIVP_APP_LDD_STRMFREE_MSG_QUEUE], (RIVP_LDD_PTR)&lMsg[0]);
		if (lBoolRet != RIVP_LDD_TRUE) {
			RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecJpgCbStrmBufFree. Send message error.\n");
		}
	} else {
		RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecJpgCbStrmBufFree. Arguments are NULL.\n");
	}
}

/**
 * \brief			search start code of JPG, and create INPUTPARAM_T for RIVP_LddInput
 *
 * \param[in,out]	app		   : application control information
 *
 * \retval			RIVP_LDD_TRUE  : find the SOI and create INPUTPARAM_T
 * \retval			RIVP_LDD_FALSE : not find the SOI or end of stream
 *
 * \attention 		only a check using rivp_app_strm_jpg_cjpeg_000.h. other cases are unconfirmed.
 *
 */
RIVP_LDD_STATIC RIVP_LDD_BOOL RIVP_AppLddDecJpgSearchStartCode(RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_BOOL			 lResult;
	RIVP_LDD_BOOL			 lSearchFlg;	/* TRUE: search, FALSE:search end        */
	RIVP_LDD_U8*			 lBuffAddr;
	RIVP_LDD_INPUTPARAM_T	*lCurInputPtr;	/* all stream information                */
	RIVP_LDD_INPUTPARAM_T	*lNextInputPtr;	/* next input information for RIVP_LddInput */
	RIVP_LDD_INPUTPARAM_T	 lTmpInput;		/* next input size for RIVP_LddInput        */

	lResult				 = RIVP_LDD_FALSE;
	lSearchFlg			 = RIVP_LDD_TRUE;	/* TRUE: search, FALSE:search end */

	if (app != RIVP_LDD_NULL) {
		/* initial setting */
		lCurInputPtr				= &app->strmInfo;
		lNextInputPtr				= &app->input;
		lNextInputPtr->strmBuffNum 	= 0u;

		/*** parameter check ***/
		/* phyAddr and strmId are not check. */
		/* strmBuffNum must be 1 or 2. */
		if ((lCurInputPtr->strmBuffNum != 1u) &&
			(lCurInputPtr->strmBuffNum != 2u)) {
			lSearchFlg = RIVP_LDD_FALSE;
		}
		/* in strmBuff[0], virtAddr must be valid address and size is necessary over the 1byte. */
		if ((lCurInputPtr->strmBuff[0].virtAddr == RIVP_LDD_NULL) ||
			(lCurInputPtr->strmBuff[0].size == 0u)) {
			lSearchFlg = RIVP_LDD_FALSE;
		}
		if (lCurInputPtr->strmBuffNum == 2u) {
			/* in strmBuff[1], virtAddr must be valid address and size is necessary over the 1byte. */
			if ((lCurInputPtr->strmBuff[1].virtAddr == RIVP_LDD_NULL) ||
				(lCurInputPtr->strmBuff[1].size == 0u)) {
				lSearchFlg = RIVP_LDD_FALSE;
			}
			/* strmBuff[0] must be the address that is smaller than strmBuff[1]. */
			/* in addition, it is necessary for strmBuff[0] to be 256 bytes align. */
			if ((((RIVP_LDD_ADR)lCurInputPtr->strmBuff[0].virtAddr & 0xFFu) == 0u) &&
				(((RIVP_LDD_ADR)lCurInputPtr->strmBuff[0].virtAddr + lCurInputPtr->strmBuff[0].size) 
				< (RIVP_LDD_ADR)lCurInputPtr->strmBuff[1].virtAddr)) {
				lSearchFlg = RIVP_LDD_FALSE;
			}
		}

		/* search SOI marker */
		/* strmBuff |---------**S*****S*****S*****S*****E-----| S:SOI, E:end of stream  */
		/* strmInf  |         |<----------------------->|     |                         */
		if (lSearchFlg == RIVP_LDD_TRUE) {
			/* search startcode by common function */
			lBuffAddr = RIVP_AppLddCmnSearchStartCode((RIVP_LDD_U8*)lCurInputPtr->strmBuff[0].virtAddr, lCurInputPtr->strmBuff[0].size,
													(RIVP_LDD_U8*)lCurInputPtr->strmBuff[1].virtAddr, lCurInputPtr->strmBuff[1].size,
													RIVP_APP_LDD_JPG_START_CODE, RIVP_APP_LDD_JPG_MASK_START_CODE,
													RIVP_APP_LDD_JPG_SC_BITS, 1u);
			if (lBuffAddr != RIVP_LDD_NULL) {
				/* detect startcode. update stream information */
				if ((RIVP_LDD_ADR)lBuffAddr >= (RIVP_LDD_ADR)lCurInputPtr->strmBuff[0].virtAddr) {
					if ((RIVP_LDD_ADR)lBuffAddr < ((RIVP_LDD_ADR)lCurInputPtr->strmBuff[0].virtAddr + lCurInputPtr->strmBuff[0].size)) {
						/* lBuffAddr is detected in strmBuff[0] */
						lCurInputPtr->strmBuff[0].size		= (RIVP_LDD_U32)(((RIVP_LDD_ADR)lCurInputPtr->strmBuff[0].virtAddr 
															+ lCurInputPtr->strmBuff[0].size) 
															- (RIVP_LDD_ADR)lBuffAddr);
						lCurInputPtr->strmBuff[0].virtAddr	= (RIVP_LDD_PTR)lBuffAddr;
						lResult = RIVP_LDD_TRUE;
					}

				} else if ((RIVP_LDD_ADR)lBuffAddr >= (RIVP_LDD_ADR)lCurInputPtr->strmBuff[1].virtAddr) {
					if ((RIVP_LDD_ADR)lBuffAddr < ((RIVP_LDD_ADR)lCurInputPtr->strmBuff[1].virtAddr + lCurInputPtr->strmBuff[1].size)) {
						/* lBuffAddr is detected in strmBuff[1] */
						/* in this case, strmBuffNum switches from 2 to 1. so, copy strmBuff[1] to strmBuff[0] and set strmBuff[1] to NULL. */
						lCurInputPtr->strmBuff[0].size		= (RIVP_LDD_U32)(((RIVP_LDD_ADR)lCurInputPtr->strmBuff[1].virtAddr 
															+ lCurInputPtr->strmBuff[1].size) 
															- (RIVP_LDD_ADR)lBuffAddr);
						lCurInputPtr->strmBuff[0].virtAddr	= (RIVP_LDD_PTR)lBuffAddr;
						lCurInputPtr->strmBuff[1].size		= 0u;
						lCurInputPtr->strmBuff[1].virtAddr	= RIVP_LDD_NULL;
						lCurInputPtr->strmBuffNum			= 1u;
						lResult = RIVP_LDD_TRUE;
					}
				} else {
					/* invalid address */
				}

			} else {
				/* not detected */
			}

			if (lResult == RIVP_LDD_TRUE) {
				/* detect SOI. create RIVP_LDD_INPUTPARAM_T for RIVP_LddInput. */
				RIVP_LDD_U32 offsetVal = RIVP_APP_LDD_JPG_SC_BITS / 4u;

				/* copy current stream information to next input information, */
				/*                  and set tmpInput for next next SOI maker. */
				/* strmBuff  |---------**S*****S*****S*****S*****E-----| S:SOI, E:end of stream  */
				/* strmInf   |           |<--------------------->|     |                         */
				/* nextInput |           |<--------------------->|     |                         */
				/* tmpInput  |                 |<--------------->|     |                         */
				lCurInputPtr->strmId++;
				*lNextInputPtr	= *lCurInputPtr;
				(void)memset(&lTmpInput, 0, sizeof(RIVP_LDD_INPUTPARAM_T));
				if (lNextInputPtr->strmBuff[0].size > offsetVal) {
					lTmpInput						= *lCurInputPtr;
					lTmpInput.strmBuff[0].virtAddr	= (RIVP_LDD_PTR)((RIVP_LDD_ADR)lTmpInput.strmBuff[0].virtAddr + offsetVal);
					lTmpInput.strmBuff[0].size		= lTmpInput.strmBuff[0].size - offsetVal;

				} else if ((lNextInputPtr->strmBuffNum == 2u) && (lNextInputPtr->strmBuff[1].size > offsetVal)) {
					offsetVal -= lNextInputPtr->strmBuff[0].size;
					lTmpInput						= *lCurInputPtr;
					lTmpInput.strmBuff[0].size		= lTmpInput.strmBuff[1].size - offsetVal;
					lTmpInput.strmBuff[0].virtAddr	= (RIVP_LDD_PTR)((RIVP_LDD_ADR)lTmpInput.strmBuff[1].virtAddr + offsetVal);
					lTmpInput.strmBuff[1].size		= 0;
					lTmpInput.strmBuff[1].virtAddr	= RIVP_LDD_NULL;
					lTmpInput.strmBuffNum			= 1;

				} else {
					/* remain size is same as RIVP_APP_LDD_JPG_SC_BITS? */
				}

				/* search next next startcode.  */
				lBuffAddr = RIVP_AppLddCmnSearchStartCode((RIVP_LDD_U8*)lTmpInput.strmBuff[0].virtAddr, lTmpInput.strmBuff[0].size,
														(RIVP_LDD_U8*)lTmpInput.strmBuff[1].virtAddr, lTmpInput.strmBuff[1].size,
														RIVP_APP_LDD_JPG_START_CODE, RIVP_APP_LDD_JPG_MASK_START_CODE,
														RIVP_APP_LDD_JPG_SC_BITS, 1u);

				/* update minimum size of input. this should make 1 picture size. */
				/* strmBuff        |----------*S*****S*****S*****S*****E-----| S:SOI, E:end of stream  */
				/* strmInf.size    |           |<--------------------->|     |                         */
				/* nextInput.size  |           |<--->|                       |                         */
				/* tmpInput.size   |                 |<--------------->|     |                         */
				/* maxOneInputSize |           |<------>|                    |                         */
				if (lBuffAddr == RIVP_LDD_NULL) {
					/* end of frame */

				} else {
					if ((RIVP_LDD_ADR)lBuffAddr >= (RIVP_LDD_ADR)lNextInputPtr->strmBuff[0].virtAddr) {
						lNextInputPtr->strmBuff[0].size = RIVP_APP_LDD_MIN( ((RIVP_LDD_ADR)lBuffAddr - (RIVP_LDD_ADR)lNextInputPtr->strmBuff[0].virtAddr), 
																			lNextInputPtr->strmBuff[0].size);

					} else {
						lNextInputPtr->strmBuff[1].size = RIVP_APP_LDD_MIN( ((RIVP_LDD_ADR)lBuffAddr - (RIVP_LDD_ADR)lNextInputPtr->strmBuff[1].virtAddr),
																			lNextInputPtr->strmBuff[1].size);
					}
				}

				/* fale safe: cut off stream size by maxOneInputSize. */
				lNextInputPtr->strmBuff[0].size 		= RIVP_APP_LDD_MIN( lNextInputPtr->strmBuff[0].size, app->maxOneInputSize);

				if ((lNextInputPtr->strmBuff[1].size > 0u) && (lNextInputPtr->strmBuff[0].size < app->maxOneInputSize)) {

					lNextInputPtr->strmBuff[1].size		= RIVP_APP_LDD_MIN( lNextInputPtr->strmBuff[1].size, 
																			(app->maxOneInputSize - lNextInputPtr->strmBuff[0].size));

				} else {
					/* not use 2nd buffer */
					lNextInputPtr->strmBuff[1].size		= 0u;
					lNextInputPtr->strmBuff[1].virtAddr	= RIVP_LDD_NULL;
					lNextInputPtr->strmBuffNum			= 1u;
				}

				/* convert virtual address to physical address */
				if (lNextInputPtr->strmBuff[0].virtAddr != RIVP_LDD_NULL) {
					lNextInputPtr->strmBuff[0].physAddr	 = (RIVP_LDD_U32)RIVP_AppLddMmgrVtoP(lNextInputPtr->strmBuff[0].virtAddr);
					/* Flush the input buffer cache */
					RIVP_AppLddMmgrFlush((RIVP_LDD_PTR)lNextInputPtr->strmBuff[0].virtAddr);
				}
				if (lNextInputPtr->strmBuff[1].virtAddr != RIVP_LDD_NULL) {
					lNextInputPtr->strmBuff[1].physAddr	 = (RIVP_LDD_U32)RIVP_AppLddMmgrVtoP(lNextInputPtr->strmBuff[1].virtAddr);
					/* Flush the input buffer cache */
					RIVP_AppLddMmgrFlush((RIVP_LDD_PTR)lNextInputPtr->strmBuff[1].virtAddr);
				}
			}
		}
	}

	return lResult;
}
