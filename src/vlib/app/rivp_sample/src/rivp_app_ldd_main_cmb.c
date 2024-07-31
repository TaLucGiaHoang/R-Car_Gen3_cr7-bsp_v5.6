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
#include "rivp_app_ldd_avc.h"

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


/******************************************************************************/
/*                    FORWARD DECLARATION                                     */
/******************************************************************************/

/******************************************************************************/
/*                    EXTERNAL FUNCTIONS                                      */
/******************************************************************************/
/**
 * \brief		 main function of the Application side
 *
 * \return		 int			 : error code
 *
 */
int rivp_ldd_main(void)
{
	RIVP_LDD_BOOL  lBoolRet;
	
	/* update log setting */
	RIVP_DepLddLogSetLevel(RIVP_LDD_LOG_MOD_DISP, RIVP_LDD_LOG_LV_DISP, RIVP_LDD_LOG_OUTPUT);

    RIVP_LDD_LOG_APP_INF("===START RIVP Sample Application===\n");

	lBoolRet = RIVP_AppLddOsalInit();


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

	/* start application main function */
	if (lBoolRet == RIVP_LDD_TRUE) {

#ifdef RIVP_DEP_LDD_ENABLE_JPEG
		/* start JPG sample code main */
		(void)RIVP_AppLddJpgMain();
#else
		/* start AVC sample code main */
		(void)RIVP_AppLddAvcMain();
#endif
	}

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

