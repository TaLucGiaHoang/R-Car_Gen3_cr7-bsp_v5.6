/******************************************************************************
 *
 * PURPOSE
 *   RIVP APP Main
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *
 ******************************************************************************/
/******************************************************************************
* Main
* Copyright (C) 2015 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
/* Osal Header */
#include "rcar-xos/osal/r_osal.h"

#include "rivp_app_ldd_osdep.h"

/* Dep Header */
#include "rivp_dep_ldd_log.h"

/* App Header */
#include "rivp_app_ldd_main_local.h"

/* For BSC */
#include "rivp_app_avc.h"

#include "cio_server.h"
#include "r_cio_api.h"
#include "r_print_api.h"
#include "taurus_app.h"
#include "r_lwip_lib.h"

/* For JPG */
#ifdef RIVP_DEP_LDD_ENABLE_JPEG
#include "rivp_app_ldd_jpg.h"
#endif

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
/* Modules	RIVP_LDD_LOG_MOD_XXX : NON/ALL/(APP|API|VDP|PEF) */
#ifndef RIVP_LDD_LOG_MOD_DISP
#define RIVP_LDD_LOG_MOD_DISP	RIVP_LDD_LOG_MOD_APP
#endif
/* Levels	RIVP_LDD_LOG_LV_XXX  : NON/ALL/(INF|WNG|ERR) */
#ifndef RIVP_LDD_LOG_LV_DISP
#define RIVP_LDD_LOG_LV_DISP	RIVP_LDD_LOG_LV_ALL
#endif
/* Output	RIVP_LDD_LOG_OUT_XXX  : NON/PRT/DMP */
#ifndef RIVP_LDD_LOG_OUTPUT
#define RIVP_LDD_LOG_OUTPUT		RIVP_LDD_LOG_OUT_PRT
#endif

#if (RIVP_LDD_LOG_OUTPUT == RIVP_LDD_LOG_OUT_DMP)
#define RIVP_DEP_LDD_MAIN_APP_DMP_MAX 0x5000u
RIVP_DEP_LDD_STR g_rivpDepLddAppDump[RIVP_DEP_LDD_MAIN_APP_DMP_MAX] = { 0 };
#endif

#define LOC_CIO_SERVER_PRIO 101
#define LOC_TAURUS_PRIO     100
#define LOC_LWIP_COMM_PRIO  102
#define LOC_RTP_PLAYER_PRIO 99

#define RTPPLAYER_RESOURCE_ID 0x8000U

#define RTPPLAYER_THREAD_ID_00   (RTPPLAYER_RESOURCE_ID + 50U) /* CIO Server */
#define RTPPLAYER_THREAD_ID_01   (RTPPLAYER_RESOURCE_ID + 51U) /* Taurus Server */
#define RTPPLAYER_THREAD_ID_02   (RTPPLAYER_RESOURCE_ID + 52U) /* RTP Player */
#define RTPPLAYER_THREAD_ID_03   (RTPPLAYER_RESOURCE_ID + 53U) /* LWIP */

/******************************************************************************/
/*                    FORWARD DECLARATION                                     */
/******************************************************************************/

/******************************************************************************/
/*                    EXTERNAL FUNCTIONS                                      */
/******************************************************************************/
static void *loc_CioServerThread(void * Arg)
{
    (void)Arg;
    cio_main();
    return 0;
}

#if 0
static void *loc_TaurusThread(void * Arg)
{
    taurus_main();
    return 0;
}
#endif

static void *loc_LwipThread(void * Arg)
{
    (void)Arg;
    vLwipStart();
    return 0;
}

static void *loc_RtpPlayerThread(void * Arg)
{
	(void)Arg;
	/* Sleep for 3 seconds to wait for LWIP to link */
	R_OSAL_ThreadSleepForTimePeriod(3000);
	/* start AVC sample code main */
	(void)RIVP_AppLddAvcMain();
}

/**
 * \brief		 main function of the Application side
 *
 * \return		 int			 : error code
 *
 */
int rtp_player_main(void)
{
	RIVP_LDD_BOOL			lBoolRet;
	e_osal_return_t			osal_ret;
	osal_thread_handle_t	cio_thread_handle;
	osal_thread_handle_t	rtp_thread_handle;
	osal_thread_handle_t	lwip_thread_handle;
	st_osal_thread_config_t	thread_config;
	int64_t					thread_ret;
	
	/* update log setting */
	RIVP_DepLddLogSetLevel(RIVP_LDD_LOG_MOD_DISP, RIVP_LDD_LOG_LV_DISP, RIVP_LDD_LOG_OUTPUT);

	RIVP_LDD_LOG_APP_INF("===START RIVP Sample Application===\n");

	lBoolRet = RIVP_AppLddOsalInit();

	thread_config.func = (p_osal_thread_func_t)loc_CioServerThread;
	thread_config.userarg = NULL;
	thread_config.priority = OSAL_THREAD_PRIORITY_TYPE10;
	osal_ret = R_OSAL_ThreadCreate(&thread_config, RTPPLAYER_THREAD_ID_00, &cio_thread_handle);
	if (OSAL_RETURN_OK != osal_ret)
	{
		R_PRINT_Log("[main]: ERROR!!! CioServerThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
		return (-1);
	}

	thread_config.func = (p_osal_thread_func_t)loc_RtpPlayerThread;
	thread_config.userarg = NULL;
	thread_config.priority = OSAL_THREAD_PRIORITY_TYPE9;
	osal_ret = R_OSAL_ThreadCreate(&thread_config, RTPPLAYER_THREAD_ID_02, &rtp_thread_handle);
	if (OSAL_RETURN_OK != osal_ret)
	{
		R_PRINT_Log("[main]: ERROR!!! RtpPlayerThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
		return (-1);
	}

	thread_config.func = (p_osal_thread_func_t)loc_LwipThread;
	thread_config.userarg = NULL;
	thread_config.priority = OSAL_THREAD_PRIORITY_TYPE11;
	osal_ret = R_OSAL_ThreadCreate(&thread_config, RTPPLAYER_THREAD_ID_03, &lwip_thread_handle);
	if (OSAL_RETURN_OK != osal_ret)
	{
		R_PRINT_Log("[main]: ERROR!!! LwipThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
		return (-1);
	}

#if (RIVP_LDD_LOG_OUTPUT == RIVP_LDD_LOG_OUT_DMP)
	/* if use log dump, initialize dump area */
	if (lBoolRet == RIVP_LDD_TRUE) {
		lBoolRet = RIVP_DepLddLogDmpStart(g_rivpDepLddAppDump, RIVP_DEP_LDD_MAIN_APP_DMP_MAX);
	}
#endif

	/* output message */
	if (lBoolRet != RIVP_LDD_TRUE) {
		RIVP_LDD_LOG_APP_ERR_SYS("Initialize Error \n");
	}

	R_OSAL_ThreadJoin(rtp_thread_handle, &thread_ret);
	R_OSAL_ThreadJoin(lwip_thread_handle, &thread_ret);

#if (RIVP_LDD_LOG_OUTPUT == RIVP_LDD_LOG_OUT_DMP)
	/* if use log dump, deinitialize dump area */
	(void)RIVP_DepLddLogDmpStop();
#endif


	if (lBoolRet == RIVP_LDD_TRUE) {
		RIVP_LDD_LOG_APP_INF("main normal end \n");

		RIVP_AppLddOsalDeinit();
	} else {
		RIVP_LDD_LOG_APP_ERR_SYS("main abnormal end \n");
	}

	return 0;
}

/******************************************************************************/
/*                    INTERNAL FUNCTIONS                                      */
/******************************************************************************/

