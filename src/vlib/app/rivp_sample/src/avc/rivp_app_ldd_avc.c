/******************************************************************************
 *
 * PURPOSE
 *   RIVP Reference Application AVC Decoder
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *
 ******************************************************************************/
/******************************************************************************
* AVC Decoder
* Copyright (C) 2015 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
#include "rivp_app_ldd_avc.h"
#include "rivp_app_ldd_data_avc.h"
#include "rivp_app_ldd_cmn_local.h"

#include "rivp_app_ldd_out.h"

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
/* flag for loop */
#ifndef RIVP_APP_LDD_LOOP
#define RIVP_APP_LDD_LOOP			(RIVP_LDD_FALSE)
#endif

/* total number of application address space. if it is not defined, set the default value(1). */
#ifndef RIVP_APP_LDD_CTX_TOTALNUM
#define RIVP_APP_LDD_CTX_TOTALNUM				(1u)			/*!< this application address space only */
#endif

/******************************************************************************/
/*                    FORWARD DECLARATION                                     */
/******************************************************************************/
RIVP_LDD_STATIC RIVP_LDD_BOOL	 RIVP_AppLddAvcInitAppInfo(RIVP_APP_LDD_CMN_PARAM_T **app, RIVP_LDD_BOOL existResource);
RIVP_LDD_STATIC RIVP_LDD_RESULT	 RIVP_AppLddAvcInitLibProc(RIVP_APP_LDD_CMN_PARAM_T *app);

/******************************************************************************/
/*                         VARIABLE                                           */
/******************************************************************************/
RIVP_LDD_STATIC RIVP_LDD_U8		 s_rivpAppLddAvcYuvBuff[RIVP_APP_LDD_YBUF_SIZE + (RIVP_APP_LDD_CBUF_SIZE * RIVP_APP_LDD_CMN_XY_MAX)] = {0u};

/******************************************************************************/
/*                    EXTERNAL FUNCTIONS                                      */
/******************************************************************************/
/**
 * \brief			 main function of this application
 *
 * \retval			 RIVP_LDD_RTN_OK				 : success
 * \retval			 RIVP_LDD_RTN_FATAL_WARNING		 : decode error
 * \retval			 RIVP_LDD_RTN_FATAL_CONTEXT		 : context error
 * \retval			 RIVP_LDD_RTN_FATAL_LIBRARY		 : library error
 * \retval			 RIVP_LDD_RTN_FATAL_SYSTEM		 : system(like a os dependent) error
 *
 * \attention 		 only a check using rivp_app_ldd_strm_avc_conf025_264_5fr.h. other cases are unconfirmed.
 *
 */
RIVP_LDD_RESULT RIVP_AppLddAvcMain( void )
{
	static RIVP_APP_LDD_CMN_PARAM_T	*s_rivpAppLddPtr			 = RIVP_LDD_NULL;
	static RIVP_LDD_BOOL			 s_rivpAppLddResourceFlg	 = RIVP_LDD_FALSE;
	const RIVP_LDD_VERSION_T		*lVerPtr;
	RIVP_LDD_RESULT					 lResult;
	RIVP_LDD_BOOL					 lBoolRet;
	RIVP_LDD_U32					 lCnt;
	RIVP_LDD_BOOL					 lLoopFlg;
	RIVP_LDD_U8						 lMsg[RIVP_APP_LDD_SIZE_MAIN_DEC_MSG];

	lLoopFlg = RIVP_APP_LDD_LOOP;

	RIVP_LDD_LOG_APP_INF("!!! RIVP Reference Application AVC Decoder start !!!\n");

	/* initialize application control information */
	lBoolRet = RIVP_AppLddAvcInitAppInfo(&s_rivpAppLddPtr, s_rivpAppLddResourceFlg);

	/* initialize this library */
	if (lBoolRet == RIVP_LDD_TRUE) {
		lResult = RIVP_AppLddAvcInitLibProc(s_rivpAppLddPtr);
	} else {
		lResult  = RIVP_LDD_RTN_FATAL_SYSTEM;
	}

	/* get version */
	if (lResult == RIVP_LDD_RTN_OK) {
		
		RIVP_AppLddCmnGetCurTime(&s_rivpAppLddPtr->perf.funcStart);
		lVerPtr = RIVP_LddGetVersion();
		RIVP_AppLddCmnGetProcTime(s_rivpAppLddPtr, RIVP_APP_LDD_FUNC_GETVER);
		s_rivpAppLddPtr->rivpVer = lVerPtr;
		
		RIVP_LDD_LOG_APP_INF("RIVP_LddGetVersion() = %s\n",	  g_rivpAppLddCmnResCode[(RIVP_LDD_U32)RIVP_LDD_RTN_OK]);
		RIVP_LDD_LOG_APP_INF(" - major    = %2d\n", s_rivpAppLddPtr->rivpVer->major);
		RIVP_LDD_LOG_APP_INF(" - minor    = %2d\n", s_rivpAppLddPtr->rivpVer->minor);
		RIVP_LDD_LOG_APP_INF(" - patch    = %2d\n", s_rivpAppLddPtr->rivpVer->patch);
	}

	if (lResult == RIVP_LDD_RTN_OK) {
		if (s_rivpAppLddResourceFlg != RIVP_LDD_TRUE) {
			/* create OS resources and run tasks */
			lBoolRet = RIVP_AppLddCmnCreateResource(s_rivpAppLddPtr);

			/* update flag to avoid duplication */
			s_rivpAppLddResourceFlg = RIVP_LDD_TRUE;
		} else {
			/* restart tasks */
			lBoolRet = RIVP_AppLddCmnRestartTask(s_rivpAppLddPtr);
		}

		/* wait until receiving message */
		if (lBoolRet == RIVP_LDD_TRUE) {
			lBoolRet = RIVP_AppLddOsalQueueReceive(&s_rivpAppLddPtr->msgInf[RIVP_APP_LDD_MAIN_MSG_QUEUE], &lMsg[0]);
			memcpy(&lResult, &lMsg[0], sizeof(lResult));
		}

		if (lBoolRet == RIVP_LDD_FALSE) {
			lResult = RIVP_LDD_RTN_FATAL_SYSTEM;
		}
	}

	/* deinitialization of this library */
	if (lResult == RIVP_LDD_RTN_OK) {
		
		RIVP_AppLddCmnGetCurTime(&s_rivpAppLddPtr->perf.funcStart);
		lResult = RIVP_LddDeinitialize(RIVP_LDD_FALSE);
		RIVP_AppLddCmnGetProcTime(s_rivpAppLddPtr, RIVP_APP_LDD_FUNC_DEINIT);
		
		RIVP_LDD_LOG_APP_INF("RIVP_LddDeinitialize() = %s\n", g_rivpAppLddCmnResCode[(RIVP_LDD_U32)lResult]);
	}

	/* print the result of performance */
	RIVP_AppLddCmnPerformanceResult(s_rivpAppLddPtr);

	/* destroy OS resources */
	if (lBoolRet == RIVP_LDD_TRUE) {
		if (lLoopFlg != RIVP_LDD_TRUE) {
			lBoolRet = RIVP_AppLddCmnDestroyResource(s_rivpAppLddPtr);
		}
	}

	/* deinitialize for display */
	if (lBoolRet == RIVP_LDD_TRUE) {
		/* compare YUV data */
		for (lCnt = 0u; ((lCnt < sizeof(g_rivpAppLddAvcSampleYuv)) && (lBoolRet != RIVP_LDD_FALSE)); lCnt++) {
			if (s_rivpAppLddPtr->yuvBuff[lCnt] != g_rivpAppLddAvcSampleYuv[lCnt]) {
				RIVP_LDD_LOG_APP_WNG("YUV(%d) is invalid!!!!!\n", lCnt);
				lBoolRet = RIVP_LDD_FALSE;
			}
		}

		if (lBoolRet == RIVP_LDD_TRUE) {
			RIVP_LDD_LOG_APP_INF("YUV is all ok!!!!!\n");
		}
	}

	/* deinitialize application control information */
	if (lBoolRet == RIVP_LDD_TRUE) {
		if (lLoopFlg != RIVP_LDD_TRUE) {
			RIVP_AppLddCmnMemFree(s_rivpAppLddPtr, s_rivpAppLddPtr->file, s_rivpAppLddPtr->codecConfig);
		}
	}

	/* output result log */
	RIVP_LDD_LOG_APP_INF("***************************************************\n");
	RIVP_LDD_LOG_APP_INF("    RIVP_AppLddAvcMain result=%s \n", g_rivpAppLddCmnResCode[(RIVP_LDD_U32)lResult]);
	RIVP_LDD_LOG_APP_INF("***************************************************\n\n");


	RIVP_LDD_LOG_APP_INF("!!! RIVP Reference Application AVC Decoder end !!!\n\n");

	return lResult;
}

/******************************************************************************/
/*                    INTERNAL FUNCTIONS                                      */
/******************************************************************************/
/**
 * \brief			 initialize application control information
 *
 * \param[in,out]	 app						 : application control information
 * \param[in]		 existResource				 : existence of application resource
 *
 * \retval			 RIVP_LDD_TRUE					 : success
 * \retval			 RIVP_LDD_FALSE					 : invalid argument
 *
 */
RIVP_LDD_STATIC RIVP_LDD_BOOL RIVP_AppLddAvcInitAppInfo(RIVP_APP_LDD_CMN_PARAM_T **app, RIVP_LDD_BOOL existResource)
{
	RIVP_LDD_BOOL				 lBoolRet;
	RIVP_APP_LDD_CMN_PARAM_T	*lAppPtr;
	RIVP_APP_LDD_CMN_FILE_T		*lFilePtr;
	RIVP_LDD_U32				*lAvcConfigPtr;

	/* set initial value */
	lBoolRet = RIVP_LDD_FALSE;

	/* check arguments */
	if (app != RIVP_LDD_NULL) {
		if (existResource == RIVP_LDD_FALSE) {
			if (sizeof(RIVP_LDD_AVC_CONFIG_T) <= RIVP_APP_LDD_CODEC_CONFIG_SIZE) {
				/* allocate application resource */
				lBoolRet = RIVP_AppLddCmnMemAlloc(&lAppPtr, &lFilePtr, (RIVP_LDD_U32**)&lAvcConfigPtr);
			}

			if (lBoolRet == RIVP_LDD_TRUE) {
				/* update pointer */
				*app                 = lAppPtr;
				lAppPtr->file        = lFilePtr;
				lAppPtr->codecConfig = lAvcConfigPtr;

				/* allocate application resource for tile(xy) to linear convert */
				lBoolRet = RIVP_AppLddCmnSetDefaultXy(lAppPtr);
				if (lBoolRet != RIVP_LDD_TRUE) {
					RIVP_AppLddCmnMemFree(lAppPtr, lAppPtr->file, (RIVP_LDD_U32*)lAppPtr->codecConfig);
				}
			}
		} else {
			/* reuse application control information */
			lAppPtr = *app;
			if (lAppPtr != RIVP_LDD_NULL) {
				lBoolRet = RIVP_LDD_TRUE;
			}
		}

		if (lBoolRet == RIVP_LDD_TRUE) {
			/* initialize application control information */
			lAppPtr->firstInput			= RIVP_LDD_FALSE;
			lAppPtr->useRingBuff		= RIVP_LDD_FALSE;	/* use ring buffer, or not */ /* [T.B.D.] TRUE case is not tested in this application */
			lAppPtr->picIndex			= 0u;
			lAppPtr->strmInfo.strmId	= 0u;
			lAppPtr->errorCode			= RIVP_LDD_RTN_OK;
			lAppPtr->yuvBuff			= s_rivpAppLddAvcYuvBuff;
			lAppPtr->yuvBuffCnt			= 0u;
			lAppPtr->streamFrameId		= RIVP_APP_LDD_AVC_DATA_STRM_FRAME;

			/* set end of sequence code for RIVP_LddInput */
			lAppPtr->strmEos[0] 		= (RIVP_LDD_U8)0x00u;
			lAppPtr->strmEos[1] 		= (RIVP_LDD_U8)0x00u;
			lAppPtr->strmEos[2] 		= (RIVP_LDD_U8)0x01u;
			lAppPtr->strmEos[3] 		= (RIVP_LDD_U8)0x0Au;

			/* set input stream information */
			lAppPtr->file->fSrc			= g_rivpAppLddAvcSampleStrm;
			lAppPtr->file->fSize		= sizeof(g_rivpAppLddAvcSampleStrm);
		}
	}

	return lBoolRet;
}

/**
 * \brief		 initialize RIVP library
 *
 * \param[in]	 app					 : application control information
 *
 * \retval		 RIVP_LDD_RTN_OK			 : success
 * \retval		 RIVP_LDD_RTN_FATAL_SYSTEM	 : os dependent error
 *
 */
RIVP_LDD_STATIC RIVP_LDD_RESULT RIVP_AppLddAvcInitLibProc(RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_RESULT				 lResult;
	RIVP_LDD_OSAL_CONFIG_T		 losCfg;

	/* set initial value */
	lResult = RIVP_LDD_RTN_FATAL_SYSTEM;

	/* check arguments */
	if (app != RIVP_LDD_NULL) {
		/* set initParam */
		app->initParam.numContext	 = RIVP_APP_LDD_CTX_TOTALNUM;
		
		/* set Osal Configuration */
		losCfg.pmPolicy				= OSAL_PM_POLICY_HP;
		losCfg.priorityValueIrq0 	= OSAL_INTERRUPT_PRIORITY_HIGHEST;
		losCfg.priorityValueIrq1 	= OSAL_INTERRUPT_PRIORITY_HIGHEST;
		losCfg.priorityValueIrq2 	= OSAL_INTERRUPT_PRIORITY_HIGHEST;
		losCfg.mutexId0 			= RIVP_APP_LDD_OSAL_LIB_MUTEX_ID_00;
		losCfg.mutexId1 			= RIVP_APP_LDD_OSAL_LIB_MUTEX_ID_01;
		losCfg.timePeriod 			= 10000u;
		losCfg.threadId			 	= RIVP_APP_LDD_OSAL_LIB_THREAD_ID;
		losCfg.mqId0	 			= RIVP_APP_LDD_OSAL_LIB_MQ_ID_00;
		losCfg.mqId1	 			= RIVP_APP_LDD_OSAL_LIB_MQ_ID_01;
		losCfg.mqTimePeriod 		= 10000u;

		/* initialize */
		RIVP_AppLddCmnGetCurTime(&app->perf.funcStart);
		lResult = RIVP_LddInitialize(&app->initParam, &losCfg);
		RIVP_AppLddCmnGetProcTime(app, RIVP_APP_LDD_FUNC_INIT);
		
		RIVP_LDD_LOG_APP_INF("RIVP_LddInitialize() = %s\n", g_rivpAppLddCmnResCode[(RIVP_LDD_U32)lResult]);
	}

	return lResult;
}
