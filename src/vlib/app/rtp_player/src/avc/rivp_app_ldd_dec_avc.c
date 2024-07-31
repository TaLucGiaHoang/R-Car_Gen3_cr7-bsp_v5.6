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
* AVC decode part
* Copyright (C) 2017 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
#include "rivp_app_ldd_dec.h"
#include "rivp_app_ldd_cmn_local.h"
#include "rivp_app_rtp.h"

#include "rivp_ldd_api.h"

#include "rivp_app_ldd_osdep.h"
#include "rivp_app_ldd_mmgr.h"
#include "rivp_app_disp.h"
#include "rivp_dep_ldd_log.h"
#include "rivp_app_config.h"
#include "rivp_app_frame.h"
#include "rivp_app_ldd_data_avc.h"

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
/* macro for search of startcode. */
#define RIVP_APP_LDD_AVC_START_CODE				(0x00000100u)	/*!< the value of startcode */
#define RIVP_APP_LDD_AVC_MASK_START_CODE		(0xFFFFFF00u)	/*!< the mask value of startcode */
#define RIVP_APP_LDD_AVC_MASK_NAL_UNIT			(0x0000001Fu)	/*!< the mask value of nal_unit_type in nal_unit header */
#define RIVP_APP_LDD_AVC_MASK_1ST_MB_IN_SLH		(0x00000080u)	/*!< the mask value of first_mb_in_slice in slice header */
#define RIVP_APP_LDD_AVC_BITS_SC				(24u)			/*!< the number of the bits of startcode. */
#define RIVP_APP_LDD_AVC_OFST_NAL_UNIT			(3u)			/*!< the offset value from top of startcode to nal_unit_type */
#define RIVP_APP_LDD_AVC_OFST_1ST_MB_IN_SLH		(4u)			/*!< the offset value from top of startcode to first_mb_in_slice */
#define RIVP_APP_LDD_AVC_SRCH_NAL_NON_IDR		(0x00000002u)	/*!< nal_unit_type =  1: Coded slice of a non-IDR picture */
#define RIVP_APP_LDD_AVC_SRCH_NAL_IDR			(0x00000020u)	/*!< nal_unit_type =  5: Coded slice of an IDR picture */
#define RIVP_APP_LDD_AVC_SRCH_NAL_SEI			(0x00000040u)	/*!< nal_unit_type =  6: Supplemental enhancement information */
#define RIVP_APP_LDD_AVC_SRCH_NAL_SPS			(0x00000080u)	/*!< nal_unit_type =  7: Sequence parameter set */
#define RIVP_APP_LDD_AVC_SRCH_NAL_PPS			(0x00000100u)	/*!< nal_unit_type =  8: Picture parameter set */
#define RIVP_APP_LDD_AVC_SRCH_NAL_AUD			(0x00000200u)	/*!< nal_unit_type =  9: Access unit delimiter */
#define RIVP_APP_LDD_AVC_SRCH_NAL_EOST			(0x00000400u)	/*!< nal_unit_type = 10: End of sequence */
#define RIVP_APP_LDD_AVC_SRCH_NAL_EOSQ			(0x00000800u)	/*!< nal_unit_type = 11: End of stream */
/* the value of target list of nal_unit_type(sequence/slice/picture/eos) */
#define RIVP_APP_LDD_AVC_SRCH_NAL_SEQ			(RIVP_APP_LDD_AVC_SRCH_NAL_SPS)
#define RIVP_APP_LDD_AVC_SRCH_NAL_SLH			(RIVP_APP_LDD_AVC_SRCH_NAL_NON_IDR	\
												|RIVP_APP_LDD_AVC_SRCH_NAL_IDR)
#define RIVP_APP_LDD_AVC_SRCH_NAL_PIC			(RIVP_APP_LDD_AVC_SRCH_NAL_NON_IDR	\
												|RIVP_APP_LDD_AVC_SRCH_NAL_IDR 		\
												|RIVP_APP_LDD_AVC_SRCH_NAL_SEI		\
												|RIVP_APP_LDD_AVC_SRCH_NAL_SPS		\
												|RIVP_APP_LDD_AVC_SRCH_NAL_PPS		\
												|RIVP_APP_LDD_AVC_SRCH_NAL_AUD		\
												|RIVP_APP_LDD_AVC_SRCH_NAL_EOST		\
												|RIVP_APP_LDD_AVC_SRCH_NAL_EOSQ)
#define RIVP_APP_LDD_AVC_SRCH_NAL_EOS			(RIVP_APP_LDD_AVC_SRCH_NAL_EOST		\
												|RIVP_APP_LDD_AVC_SRCH_NAL_EOSQ)

#define RIVP_APP_LDD_DEC_AVC_MAXFS_SIZE			(5u)			/*!< array size of maxfs */
#define RIVP_APP_LDD_DEC_AVC_MAXFF_DEV			(10u)

#define RIVP_APP_AVC_DATA_REF_YUV_SIZE			(1u)					/*!< yuv size */
#define RIVP_APP_AVC_DATA_STRM_FRAME			(5u)					/*!< 5Frame */
#define RIVP_APP_AVC_DATA_STRM_422				(RIVP_LDD_FALSE)			/*!< YCbCr format of sample stream */
#define RIVP_APP_AVC_DATA_STRM_12BIT			(RIVP_LDD_FALSE)			/*!< bit depth of sample stream */
#define RIVP_APP_AVC_DATA_STRM_WIDTH			(1920)					/*!< picture width  of sample stream */
#define RIVP_APP_AVC_DATA_STRM_HEIGHT			(1088)					/*!< picture height of sample stream */
#define RIVP_APP_AVC_DATA_STRM_LEVEL			(RIVP_AVC_MAX_LEVEL_10)	/*!< level of sample stream */
#define RIVP_APP_AVC_DATA_BUFFERING				CONFIG_OUTPUT_BUFFERRING
#define CONFIG_STREAM_WITH_B_FRAME				(1)
#define RIVP_APP_AVC_DATA_STRM_FIXED			(RIVP_LDD_TRUE)				/*!< reference application is single stream and sample stream is single sequence. */

/******************************************************************************/
/*                    FORWARD DECLARATION                                     */
/******************************************************************************/
RIVP_LDD_STATIC RIVP_LDD_RESULT	 RIVP_AppLddDecAvcGetCtxProc		(RIVP_APP_LDD_CMN_PARAM_T *app);
RIVP_LDD_STATIC RIVP_LDD_RESULT	 RIVP_AppLddDecAvcFreeCtxProc		(RIVP_APP_LDD_CMN_PARAM_T *app);
RIVP_LDD_STATIC RIVP_LDD_BOOL	 RIVP_AppLddDecAvcInitCtxParam		(RIVP_APP_LDD_CMN_PARAM_T *app);
RIVP_LDD_STATIC RIVP_LDD_RESULT	 RIVP_AppLddDecAvcInput				(RIVP_APP_LDD_CMN_PARAM_T *app);
RIVP_LDD_STATIC RIVP_LDD_RESULT	 RIVP_AppLddDecAvcRecovery			(RIVP_APP_LDD_CMN_PARAM_T *app);
RIVP_LDD_STATIC RIVP_LDD_BOOL	 RIVP_AppLddDecAvcSearchStartCode	(RIVP_APP_LDD_CMN_PARAM_T *app, RIVP_LDD_U32 tgtNalLst);

/* callback from RIVP */
RIVP_LDD_STATIC void			 RIVP_AppLddDecAvcCbPicOut			(RIVP_LDD_PTR context, RIVP_LDD_PTR udptr, RIVP_LDD_PICPARAM_T *pic);
RIVP_LDD_STATIC void			 RIVP_AppLddDecAvcCbError			(RIVP_LDD_PTR context, RIVP_LDD_PTR udptr, RIVP_LDD_RESULT errorCode, RIVP_LDD_U32 errorInfo);
RIVP_LDD_STATIC void			 RIVP_AppLddDecAvcCbStrmBufFree		(RIVP_LDD_PTR context, RIVP_LDD_PTR udptr, RIVP_LDD_INPUTPARAM_T *inParam);

/* table of maxPicSize for AVC(Table A-1 in standard). value of index is maxLevel/10 in this application. */
/* when specified size larger than 1 picture to RIVP_LddInput, this is necessary. */
RIVP_LDD_STATIC const RIVP_LDD_U32 s_rivpAppLddDecAvcMaxFs[RIVP_APP_LDD_DEC_AVC_MAXFS_SIZE] = {
	396u * (384u / 2u), 1620u * (384u / 2u), 5120u * (384u / 2u), 8704u * (384u / 2u), 36864u * (384u / 2u)
};

RIVP_LDD_STATIC RIVP_VIDEO_FRAME s_videoFrame[RIVP_APP_VIDEO_FRAME_NUM] = {
    {0, 0, 0, NULL},
    {0, 0, 0, NULL},
    {0, 0, 0, NULL},
};

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
	RIVP_LDD_RESULT				 lResult = RIVP_LDD_RTN_OK;
	RIVP_LDD_BOOL				 lBoolRet;
	RIVP_APP_LDD_CMN_PARAM_T	*lAppPtr;
	RIVP_LDD_BOOL				 lContinueWait;
	RIVP_LDD_U8					 lMsg[RIVP_APP_LDD_SIZE_MAIN_DEC_MSG];

	RIVP_LDD_LOG_APP_INF("avc decode task start.\n");

	/* check argumetns */
	if (arg != RIVP_LDD_NULL) {

		for (int i = 0; i < RIVP_APP_VIDEO_FRAME_NUM; i++) {
			s_videoFrame[i].data = RIVP_AppLddMmgrAlloc(RIVP_APP_VIDEO_FRAME_SIZE, 256);
			if (RIVP_LDD_NULL == s_videoFrame[i].data) {
				RIVP_LDD_LOG_APP_ERR_SYS("Failed to allocate video frame buffer.\n");
				lResult = RIVP_LDD_RTN_FATAL_SYSTEM;
			}
		}

		if (lResult == RIVP_LDD_RTN_OK) {
			lAppPtr = (RIVP_APP_LDD_CMN_PARAM_T*)arg;

			/* set initial value */
			lBoolRet      = RIVP_LDD_TRUE;
			lContinueWait = RIVP_LDD_TRUE;

			/* get context */
			lResult = RIVP_AppLddDecAvcGetCtxProc(lAppPtr);
		}

		/* input stream */
		if (lResult == RIVP_LDD_RTN_OK) {
			lResult = RIVP_AppLddDecAvcInput(lAppPtr);
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
			lResult = RIVP_AppLddDecAvcFreeCtxProc(lAppPtr);
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
RIVP_LDD_STATIC RIVP_LDD_RESULT RIVP_AppLddDecAvcGetCtxProc(RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_RESULT	 lResult;
	RIVP_LDD_BOOL	 lBoolRet;

	/* set initial value */
	lResult = RIVP_LDD_RTN_FATAL_SYSTEM;

	/* check arguments */
	if (app != RIVP_LDD_NULL) {
		/* initialize for AVC */
		lBoolRet = RIVP_AppLddDecAvcInitCtxParam(app);

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
					RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecAvcGetCtxProc(). Memory allocation error.\n");
					lResult = RIVP_LDD_RTN_FATAL_SYSTEM;
				}
			}

			/* create context */
			if (lResult == RIVP_LDD_RTN_OK) {
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
RIVP_LDD_STATIC RIVP_LDD_RESULT RIVP_AppLddDecAvcFreeCtxProc(RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_RESULT	 lResult;

	/* set initial value */
	lResult = RIVP_LDD_RTN_FATAL_SYSTEM;

	/* check arguments */
	if (app != RIVP_LDD_NULL) {
		/* free context */
		do {
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
RIVP_LDD_STATIC RIVP_LDD_BOOL RIVP_AppLddDecAvcInitCtxParam(RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_BOOL			 lBoolRet;
	RIVP_LDD_AVC_CONFIG_T	*lAvcConfigPtr;
	RIVP_LDD_U32			 lIndexMaxFs;

	/* set initial value */
	lBoolRet    = RIVP_LDD_FALSE;
	lIndexMaxFs = (RIVP_LDD_AVC_MAX_LEVEL_42 / RIVP_APP_LDD_DEC_AVC_MAXFF_DEV) - 1u;	/* default */

	/* check arguments */
	if (app != RIVP_LDD_NULL) {
		/* set result */
		lBoolRet = RIVP_LDD_TRUE;

		lAvcConfigPtr = (RIVP_LDD_AVC_CONFIG_T *)app->codecConfig;

		/* set ctxParam */
		/* set codecConfig */
		lAvcConfigPtr->maxLevel					= RIVP_APP_LDD_AVC_DATA_STRM_LEVEL;		/* 10-42 */
		lAvcConfigPtr->normalDecode				= RIVP_LDD_TRUE;						/* not support, only RIVP_LDD_TRUE is setable. */
		lAvcConfigPtr->outputLinearFormat		= RIVP_LDD_TRUE;						/* TRUE,FALSE */ /* [T.B.D.] TRUE case is not tested in this application */
		lAvcConfigPtr->reorderEnable			= CONFIG_STREAM_WITH_B_FRAME==1 ?
													RIVP_LDD_TRUE : RIVP_LDD_FALSE;		/* TRUE,FALSE */
		lAvcConfigPtr->fixedPicSize				= RIVP_APP_LDD_AVC_DATA_STRM_FIXED;		/* TRUE,FALSE */
		lAvcConfigPtr->restrictedDpbNum			= RIVP_LDD_AVC_DPB_NUM_STANDARD;		/* _STANDARD,1-16,_INTRA_ONLY */

		/* set config */
		app->ctxParam.config.codecType			= RIVP_LDD_AVC_CODEC_TYPE;
		app->ctxParam.config.codecConfig		= lAvcConfigPtr;
		app->ctxParam.config.codecConfigSize	= sizeof(RIVP_LDD_AVC_CONFIG_T);
		app->ctxParam.config.support12bitDepth	= RIVP_APP_LDD_AVC_DATA_STRM_12BIT;		/* this is for Gen3 and must set RIVP_LDD_FALSE. 	*/
		app->ctxParam.config.support422Format	= RIVP_APP_LDD_AVC_DATA_STRM_422;		/* this is for Gen3 and must set RIVP_LDD_FALSE. 	*/
		app->ctxParam.config.intCnt				= RIVP_LDD_MBLINE_INT_NONE;				/* not support, must set _INT_NONE. 			*/
		app->ctxParam.ch						= 0u;									/* 0 											*/
		app->ctxParam.maxWidth					= RIVP_APP_LDD_AVC_DATA_STRM_WIDTH;		/* 80-1920 										*/
		app->ctxParam.maxHeight					= RIVP_APP_LDD_AVC_DATA_STRM_HEIGHT;	/* 80-1088 										*/
		app->ctxParam.numBufferingPic			= RIVP_APP_LDD_AVC_DATA_BUFFERING;		/* 0-3 											*/
		app->ctxParam.cbStrmFree				= &RIVP_AppLddDecAvcCbStrmBufFree;
		app->ctxParam.cbPicOut					= &RIVP_AppLddDecAvcCbPicOut;
		app->ctxParam.cbError					= &RIVP_AppLddDecAvcCbError;
		app->ctxParam.udptr						= app;

		RIVP_LDD_LOG_APP_INF("ctxParam setting:\n");
		RIVP_LDD_LOG_APP_INF(" - codecType          = %s\n", g_rivpAppLddCmnCodec[app->ctxParam.config.codecType]);
		RIVP_LDD_LOG_APP_INF(" - intCnt             = %s\n", g_rivpAppLddCmnIntCnt[(RIVP_LDD_U32)app->ctxParam.config.intCnt]);
		RIVP_LDD_LOG_APP_INF(" - support12bitDepth  = %s\n", g_rivpAppLddCmnBool[(RIVP_LDD_U32)app->ctxParam.config.support12bitDepth]);
		RIVP_LDD_LOG_APP_INF(" - support422Format   = %s\n", g_rivpAppLddCmnBool[(RIVP_LDD_U32)app->ctxParam.config.support422Format]);
		RIVP_LDD_LOG_APP_INF(" - BufferingPic       = %d\n", app->ctxParam.numBufferingPic);
		RIVP_LDD_LOG_APP_INF("avcConfig setting:\n");
		RIVP_LDD_LOG_APP_INF(" - maxLevel           = %d\n", lAvcConfigPtr->maxLevel);
		RIVP_LDD_LOG_APP_INF(" - normalDecode       = %s\n", g_rivpAppLddCmnBool[(RIVP_LDD_U32)lAvcConfigPtr->normalDecode]);
		RIVP_LDD_LOG_APP_INF(" - outputLinearFormat = %s\n", g_rivpAppLddCmnBool[(RIVP_LDD_U32)lAvcConfigPtr->outputLinearFormat]);
		RIVP_LDD_LOG_APP_INF(" - reorderEnable      = %s\n", g_rivpAppLddCmnBool[(RIVP_LDD_U32)lAvcConfigPtr->reorderEnable]);
		RIVP_LDD_LOG_APP_INF(" - fixedPicSize       = %s\n", g_rivpAppLddCmnBool[(RIVP_LDD_U32)lAvcConfigPtr->fixedPicSize]);
		RIVP_LDD_LOG_APP_INF(" - restrictedDpbNum   = %d\n", (RIVP_LDD_U32)lAvcConfigPtr->restrictedDpbNum);

		/* set maxOneInputSize for this application */
		/* This must be less than RIVP_APP_LDD_MAX_INPUT_SIZE byte. */
		if ((RIVP_LDD_AVC_MAX_LEVEL_10 <= lAvcConfigPtr->maxLevel) && (lAvcConfigPtr->maxLevel <= RIVP_LDD_AVC_MAX_LEVEL_51)) {
			lIndexMaxFs = (lAvcConfigPtr->maxLevel/RIVP_APP_LDD_DEC_AVC_MAXFF_DEV) - 1u;
		}
		app->maxOneInputSize = s_rivpAppLddDecAvcMaxFs[lIndexMaxFs];
	}

	return lBoolRet;
}

/**
 * \brief			 get an available video frame buffer
 *
 * \param[in,out]	 unpacker					: rtp/h.264 unpacker data
 *
 * \retval			 none
 *
 */
void rivp_dec_get_video_frame(st_rivp_rtp_unpacker *unpacker)
{
	static RIVP_LDD_U8 lVFrameIndex = 0;
	while (1) {
		if (RIVP_APP_VIDEO_FRAME_NUM == lVFrameIndex) {
			lVFrameIndex = 0;
			RIVP_AppLddOsalUSleep(RIVP_APP_LDD_DEC_AVC_SLEEP_TIME);
		}
		if (0 == s_videoFrame[lVFrameIndex].frameUse) {
			s_videoFrame[lVFrameIndex].frameUse = 1;
			unpacker->video_frame = &s_videoFrame[lVFrameIndex];
			break;
		}
		else {
			lVFrameIndex++;
		}
	}
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
RIVP_LDD_STATIC RIVP_LDD_RESULT RIVP_AppLddDecAvcInput(RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_RESULT		 lResult;
	RIVP_LDD_BOOL		 lBoolRet;
	st_rivp_rtp_unpacker unpacker;

	/* set initial value */
	lResult = RIVP_LDD_RTN_FATAL_SYSTEM;
	unpacker.app = app;

	/* check arguments */
	if (app != RIVP_LDD_NULL) {
		app->decoding   = RIVP_LDD_TRUE;
		app->searchMode = RIVP_APP_LDD_AVC_SRCH_NAL_SEQ;	/* in the first time, search sequence header only */

		rivp_dec_get_video_frame(&unpacker);

		while (app->decoding == RIVP_LDD_TRUE) {
			/* check error code */
			if (app->errorCode != RIVP_LDD_RTN_OK) {
				/* do recovery process */
				lResult = RIVP_AppLddDecAvcRecovery(app);
				if (lResult != RIVP_LDD_RTN_OK) {
					RIVP_LDD_LOG_APP_ERR_SYS("!! Error recovery process. !!\n");
					app->decoding = RIVP_LDD_FALSE;
				}
			} else {
				RIVP_AppLddOsalQueueReceive(&app->msgInf[RIVP_APP_LDD_RTP_MSG_QUEUE], (RIVP_LDD_PTR)&unpacker.rtp_frame);

				rivp_frame_read_packets(&unpacker);

				/*send message to RTP task*/
				lBoolRet = RIVP_AppLddOsalQueueSend(&app->msgInf[RIVP_APP_LDD_RTPFREE_MSG_QUEUE], &unpacker.rtp_frame);
				if (lBoolRet != RIVP_LDD_TRUE) {
					RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppDecAvcCbStrmBufFree. Send message error.\n");
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
RIVP_LDD_STATIC RIVP_LDD_RESULT RIVP_AppLddDecAvcRecovery(RIVP_APP_LDD_CMN_PARAM_T *app)
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
				app->searchMode			= RIVP_APP_LDD_AVC_SRCH_NAL_SEQ;		/* seach new seaquence header */
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
				// app->searchMode			= RIVP_APP_LDD_AVC_SRCH_NAL_SEQ;
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
 * \attention 		only a check using rivp_app_ldd_strm_avc_conf025_264_5fr.h. other cases are unconfirmed.
 *
 */
RIVP_LDD_STATIC void RIVP_AppLddDecAvcCbPicOut(RIVP_LDD_PTR ctx, RIVP_LDD_PTR udptr, RIVP_LDD_PICPARAM_T *pic)
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
				RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecAvcCbPicOut. Send message error.\n");
			}
		}
	} else {
		RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecAvcCbPicOut. Argument is NULL.\n");
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
 * \attention 		only a check using rivp_app_ldd_strm_avc_conf025_264_5fr.h. other cases are unconfirmed.
 *
 */
RIVP_LDD_STATIC void RIVP_AppLddDecAvcCbError(RIVP_LDD_PTR ctx, RIVP_LDD_PTR udptr, RIVP_LDD_RESULT errorCode, RIVP_LDD_U32 errorInfo)
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
		RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecAvcCbError(). Argument is NULL.\n");
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
 * \attention 		only a check using rivp_app_ldd_strm_avc_conf025_264_5fr.h. other cases are unconfirmed.
 *
 */
RIVP_LDD_STATIC void RIVP_AppLddDecAvcCbStrmBufFree(RIVP_LDD_PTR ctx, RIVP_LDD_PTR udptr, RIVP_LDD_INPUTPARAM_T *inParam)
{
	RIVP_APP_LDD_CMN_PARAM_T	*lAppPtr;
	RIVP_LDD_BOOL				 lBoolRet;
	RIVP_LDD_BOOL				 lContinueDecode;
	RIVP_LDD_U8					 lMsg[RIVP_APP_LDD_SIZE_MAIN_DEC_MSG];
	RIVP_LDD_U32				 lVFrameDataAddr;
	RIVP_LDD_U8					 lVFrameIndex;

	/* check arguments */
	if ((udptr != RIVP_LDD_NULL) && (inParam != RIVP_LDD_NULL)) {
		lAppPtr = (RIVP_APP_LDD_CMN_PARAM_T*)udptr;
		lVFrameDataAddr = (RIVP_LDD_U32)(inParam->strmBuff[0].virtAddr);

		/* set decoded stream size for next startcode search */
		if (inParam->strmBuff[RIVP_LDD_STRM_FIELD_FIRST].size <= 4u) {	/* EOS or invalid stream */
			/* in this case because callback CbPicOut is not notified, update the parameter for output picture. */
			lAppPtr->picIndex = RIVP_APP_LDD_EOS_ID;
		}
		/* set picture information */
		lAppPtr->decodedStrmSize[RIVP_LDD_STRM_FIELD_FIRST]  = inParam->strmBuff[RIVP_LDD_STRM_FIELD_FIRST].size;
		lAppPtr->decodedStrmSize[RIVP_LDD_STRM_FIELD_SECOND] = inParam->strmBuff[RIVP_LDD_STRM_FIELD_SECOND].size;

		/* update flag of application status */
		lAppPtr->searchMode = RIVP_APP_LDD_AVC_SRCH_NAL_PIC;	/* 1st picture decoded, set search all picture edge */

		/* send message */
		lContinueDecode = RIVP_LDD_TRUE;
		memcpy(&lMsg[0], &lContinueDecode, sizeof(lContinueDecode));
		lBoolRet = RIVP_AppLddOsalQueueSend(&lAppPtr->msgInf[RIVP_APP_LDD_STRMFREE_MSG_QUEUE], (RIVP_LDD_PTR)&lMsg[0]);
		if (lBoolRet != RIVP_LDD_TRUE) {
			RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecAvcCbStrmBufFree. Send message error.\n");
		}
		for (lVFrameIndex = 0; lVFrameIndex < RIVP_APP_VIDEO_FRAME_NUM; lVFrameIndex++) {
			if (lVFrameDataAddr == (RIVP_LDD_U32)s_videoFrame[lVFrameIndex].data) {
				s_videoFrame[lVFrameIndex].framePos = 0;
				s_videoFrame[lVFrameIndex].frameSize = 0;
				s_videoFrame[lVFrameIndex].frameUse = 0;
				break;
			}
		}
		if (RIVP_APP_VIDEO_FRAME_NUM == lVFrameIndex) {
			RIVP_LDD_LOG_APP_ERR_SYS("Failed to find video frame.\n");
		}
	} else {
		RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddDecAvcCbStrmBufFree. Arguments are NULL.\n");
	}
}

/**
 * \brief			search start code of target nal_unit_type of AVC, and create INPUTPARAM_T for RIVP_LddInput
 *
 * \param[in,out]	app		   : application control information
 * \param[in]		tgtNalLst  : target nal_unit_type list
 *
 * \retval			RIVP_LDD_TRUE  : find the target nal_unit_type and create INPUTPARAM_T
 * \retval			RIVP_LDD_FALSE : not find the target nal_unit_type or end of stream
 *
 * \attention 		only a check using rivp_app_ldd_strm_avc_conf025_264_5fr.h. other cases are unconfirmed.
 *
 */
RIVP_LDD_STATIC RIVP_LDD_BOOL RIVP_AppLddDecAvcSearchStartCode(RIVP_APP_LDD_CMN_PARAM_T *app, RIVP_LDD_U32 tgtNalLst)
{
	RIVP_LDD_BOOL	 		 lBoolRet;
	RIVP_LDD_BOOL	 		 lSearchFlg;	/* TRUE: search, FALSE:search end */
	RIVP_LDD_U8*	 		 lBuffAddr;
	RIVP_LDD_U32	 		 lNalType;
	RIVP_LDD_U8		 		 lSlh1st;
	RIVP_LDD_INPUTPARAM_T	*lCurInputPtr;	/* all stream information */
	RIVP_LDD_INPUTPARAM_T	*lNextInputPtr;	/* next input information for RIVP_LddInput */

	lBoolRet			= RIVP_LDD_FALSE;
	lSearchFlg			= RIVP_LDD_TRUE;	/* TRUE: search, FALSE:search end */	

	if (app != RIVP_LDD_NULL) {
		/* initial setting */
		lCurInputPtr		= &app->strmInfo;
		lNextInputPtr	= &app->input;
		lNextInputPtr->strmBuffNum = 0u;

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
			if ((((RIVP_LDD_ADR)lCurInputPtr->strmBuff[0].virtAddr & RIVP_APP_LDD_STRM_ALIGN_MASK) == 0u) &&
				(((RIVP_LDD_ADR)lCurInputPtr->strmBuff[0].virtAddr + lCurInputPtr->strmBuff[0].size) < (RIVP_LDD_ADR)lCurInputPtr->strmBuff[1].virtAddr)) {
				lSearchFlg = RIVP_LDD_FALSE;
			}
		}

		/* search target nal_unit_type */
		/* strmBuff   |---------**S*****St*****S*****S*****E-----| S:start code, t:target nal, E:end of stream */
		/* strmInf    |         |<------------------------>|     |                                             */
		while (lSearchFlg == RIVP_LDD_TRUE) {
			/* search startcode by common function */
			lBuffAddr = RIVP_AppLddCmnSearchStartCode((RIVP_LDD_U8*)lCurInputPtr->strmBuff[0].virtAddr, lCurInputPtr->strmBuff[0].size,
													(RIVP_LDD_U8*)lCurInputPtr->strmBuff[1].virtAddr, lCurInputPtr->strmBuff[1].size,
													RIVP_APP_LDD_AVC_START_CODE, RIVP_APP_LDD_AVC_MASK_START_CODE,
													RIVP_APP_LDD_AVC_BITS_SC, 1u);
			if (lBuffAddr != RIVP_LDD_NULL) {
				/* detect startcode. update stream information */
				if ((RIVP_LDD_ADR)lBuffAddr >= (RIVP_LDD_ADR)lCurInputPtr->strmBuff[0].virtAddr) {
					if ((RIVP_LDD_ADR)lBuffAddr < ((RIVP_LDD_ADR)lCurInputPtr->strmBuff[0].virtAddr + lCurInputPtr->strmBuff[0].size)) {
						/* lBuffAddr is detected in strmBuff[0] */
						lCurInputPtr->strmBuff[0].size		= (RIVP_LDD_U32)(((RIVP_LDD_ADR)lCurInputPtr->strmBuff[0].virtAddr + lCurInputPtr->strmBuff[0].size) - (RIVP_LDD_ADR)lBuffAddr);
						lCurInputPtr->strmBuff[0].virtAddr	= (RIVP_LDD_PTR)lBuffAddr;
					} else {
						/* invalid address */
						lSearchFlg = RIVP_LDD_FALSE;
					}

				} else if ((RIVP_LDD_ADR)lBuffAddr >= (RIVP_LDD_ADR)lCurInputPtr->strmBuff[1].virtAddr) {
					if ((RIVP_LDD_ADR)lBuffAddr < ((RIVP_LDD_ADR)lCurInputPtr->strmBuff[1].virtAddr + lCurInputPtr->strmBuff[1].size)) {
						/* lBuffAddr is detected in strmBuff[1] */
						/* in this case, strmBuffNum switches from 2 to 1. so, copy strmBuff[1] to strmBuff[0] and set strmBuff[1] to NULL. */
						lCurInputPtr->strmBuff[0].size		= (RIVP_LDD_U32)(((RIVP_LDD_ADR)lCurInputPtr->strmBuff[1].virtAddr + lCurInputPtr->strmBuff[1].size) - (RIVP_LDD_ADR)lBuffAddr);
						lCurInputPtr->strmBuff[0].virtAddr	= (RIVP_LDD_PTR)lBuffAddr;
						lCurInputPtr->strmBuff[1].size		= 0u;
						lCurInputPtr->strmBuff[1].virtAddr	= RIVP_LDD_NULL;
						lCurInputPtr->strmBuffNum			= 1u;
					} else {
						/* invalid address */
						lSearchFlg = RIVP_LDD_FALSE;
					}

				} else {
					/* invalid address */
					lSearchFlg = RIVP_LDD_FALSE;
				}

			} else {
				/* not detected */
				lSearchFlg = RIVP_LDD_FALSE;
			}

			/* check whether nal_unit_type is the target */
			if (lSearchFlg == RIVP_LDD_TRUE) {
				/* get nal_unit_type */
				if (lCurInputPtr->strmBuff[0].size > RIVP_APP_LDD_AVC_OFST_NAL_UNIT) {
					/* nal_unit_type is detected in strmBuff[0]. set bit lNalType. */
					lNalType  = (RIVP_LDD_U32)1u << (lBuffAddr[RIVP_APP_LDD_AVC_OFST_NAL_UNIT] & RIVP_APP_LDD_AVC_MASK_NAL_UNIT);

				} else if (lCurInputPtr->strmBuff[1].size > 0u) {
					/* nal_unit_type is detected in strmBuff[1]. set bit lNalType. */
					lBuffAddr = (RIVP_LDD_U8*)lCurInputPtr->strmBuff[1].virtAddr;
					lNalType  = (RIVP_LDD_U32)1u << (lBuffAddr[RIVP_APP_LDD_AVC_OFST_NAL_UNIT - lCurInputPtr->strmBuff[0].size] & RIVP_APP_LDD_AVC_MASK_NAL_UNIT);

				} else {
					/* nal_unit_type does not exist. */
					lNalType  = 0u;
					lSearchFlg    = RIVP_LDD_FALSE;
				}
				/* check nal_unit_type */
				if ((lNalType & tgtNalLst) != 0u) {
					if ((lNalType & RIVP_APP_LDD_AVC_SRCH_NAL_SLH) != 0u) {
						/* detect slice header. check the value of first_mb_in_slice. */
						if (lCurInputPtr->strmBuff[0].size > RIVP_APP_LDD_AVC_OFST_1ST_MB_IN_SLH) {
							/* first_mb_in_slice is detected in strmBuff[0]. */
							lSlh1st   = lBuffAddr[RIVP_APP_LDD_AVC_OFST_1ST_MB_IN_SLH] & RIVP_APP_LDD_AVC_MASK_1ST_MB_IN_SLH;

						} else if (lCurInputPtr->strmBuff[1].size > 0u) {
							/* first_mb_in_slice is detected in strmBuff[1]. */
							lBuffAddr = (RIVP_LDD_U8*)lCurInputPtr->strmBuff[1].virtAddr;
							lSlh1st   = lBuffAddr[RIVP_APP_LDD_AVC_OFST_1ST_MB_IN_SLH - lCurInputPtr->strmBuff[0].size] & RIVP_APP_LDD_AVC_MASK_1ST_MB_IN_SLH;

						} else {
							/* first_mb_in_slice does not exist. */
							lSlh1st   = 0u;
							lSearchFlg    = RIVP_LDD_FALSE;
						}
						if (lSlh1st != 0u) {
							/* syntax(golomb code) of first_mb_in_slice is 0. RIVP supports only input from 1st slice(first_mb_in_slice == 0). */
							lBoolRet = RIVP_LDD_TRUE;
							lSearchFlg  = RIVP_LDD_FALSE;
						}

					} else {
						/* detect target nal_unit_type except slice header */
						lBoolRet = RIVP_LDD_TRUE;
						lSearchFlg  = RIVP_LDD_FALSE;
					}
				}
				if (lBoolRet != RIVP_LDD_TRUE) {
					/* not detect target nal_unit_type, search next startcode. */
					/* update stream information(cancel data to nal_unit_type which has been detected). */
					if (lCurInputPtr->strmBuff[0].size > RIVP_APP_LDD_AVC_OFST_NAL_UNIT) {
						lCurInputPtr->strmBuff[0].virtAddr	= (RIVP_LDD_PTR)((RIVP_LDD_ADR)lCurInputPtr->strmBuff[0].virtAddr + RIVP_APP_LDD_AVC_OFST_NAL_UNIT);
						lCurInputPtr->strmBuff[0].size		= lCurInputPtr->strmBuff[0].size - RIVP_APP_LDD_AVC_OFST_NAL_UNIT;

					} else if ((lCurInputPtr->strmBuffNum == 2u) && (lCurInputPtr->strmBuff[1].size > RIVP_APP_LDD_AVC_OFST_NAL_UNIT)) {
						RIVP_LDD_U32 offsetVal = RIVP_APP_LDD_AVC_OFST_NAL_UNIT - lCurInputPtr->strmBuff[0].size;
						lCurInputPtr->strmBuff[0].size		= lCurInputPtr->strmBuff[1].size - offsetVal;
						lCurInputPtr->strmBuff[0].virtAddr	= (RIVP_LDD_PTR)((RIVP_LDD_ADR)lCurInputPtr->strmBuff[1].virtAddr + offsetVal);
						lCurInputPtr->strmBuff[1].size		= 0;
						lCurInputPtr->strmBuff[1].virtAddr	= RIVP_LDD_NULL;
						lCurInputPtr->strmBuffNum			= 1;

					} else {
						/* end of stream. search end. */
						lSearchFlg  = RIVP_LDD_FALSE;
					}
				}
			}
		}
		if (lBoolRet == RIVP_LDD_TRUE) {
			/* detect target nal_unit_type. create RIVP_LDD_INPUTPARAM_T for RIVP_LddInput. */

			/* copy current stream information to next input information */
			/* strmBuff   |----------*S*****St*****S*****S*****E-----| S:start code, St:target nal, E:end of stream */
			/* strmInf    |                 |<---------------->|     |                                              */
			/* nextInput  |                 |<---------------->|     |                                              */
			lCurInputPtr->strmId++;
			*lNextInputPtr = *lCurInputPtr;

			/* update minimum size of input. this should make 1 picture size, but this application uses the case of maxOneInputSize. */
			/* strmBuff        |----------*S*****St*****S*****S*****E-----| S:start code, St:target nal, E:end of stream */
			/* strmInf.size    |                 |<---------------->|     |                                              */
			/* nextInput.size  |                 |<---------------->|     |                                              */
			/* maxOneInputSize |                 |<------>|               |                                              */
			/* cut off stream size of input information by maxOneInputSize. */
			lNextInputPtr->strmBuff[0].size = RIVP_APP_LDD_MIN(lNextInputPtr->strmBuff[0].size, app->maxOneInputSize);
			if ((lNextInputPtr->strmBuff[1].size > 0u) && (lNextInputPtr->strmBuff[0].size < app->maxOneInputSize)) {
				lNextInputPtr->strmBuff[1].size = RIVP_APP_LDD_MIN(lNextInputPtr->strmBuff[1].size, (app->maxOneInputSize - lNextInputPtr->strmBuff[0].size));
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

	return lBoolRet;
}
