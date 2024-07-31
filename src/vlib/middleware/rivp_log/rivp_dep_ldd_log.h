/******************************************************************************
 *
 * PURPOSE
 *   RIVP Log System Defines
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 ******************************************************************************/
/******************************************************************************
* Log System Defines
* Copyright (C) 2015 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/
#ifndef RIVP_DEP_LDD_LOG_H
#define RIVP_DEP_LDD_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
#include "rivp_ldd_types.h"

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
#define RIVP_LDD_LOG_TMP_BIT		(0x00010000u)

/* modules for RIVP_DepLog */
#define RIVP_LDD_LOG_MOD_APP_ID		(1u)
#define RIVP_LDD_LOG_MOD_API_ID		(2u)
#define RIVP_LDD_LOG_MOD_VDP_ID		(3u)
#define RIVP_LDD_LOG_MOD_PEF_ID		(4u)
#define RIVP_LDD_LOG_MOD_MAX_ID		(5u)

/* modules for RIVP_DepLogSetLevel */
#define RIVP_LDD_LOG_MOD_APP		(RIVP_LDD_LOG_TMP_BIT << RIVP_LDD_LOG_MOD_APP_ID)
#define RIVP_LDD_LOG_MOD_API		(RIVP_LDD_LOG_TMP_BIT << RIVP_LDD_LOG_MOD_API_ID)
#define RIVP_LDD_LOG_MOD_VDP		(RIVP_LDD_LOG_TMP_BIT << RIVP_LDD_LOG_MOD_VDP_ID)
#define RIVP_LDD_LOG_MOD_PEF		(RIVP_LDD_LOG_TMP_BIT << RIVP_LDD_LOG_MOD_PEF_ID)
#define RIVP_LDD_LOG_MOD_NON		(0x00000000u)
#define RIVP_LDD_LOG_MOD_ALL		(0xFFFFFFFFu)

/* levels for RIVP_DepLog */
#define RIVP_LDD_LOG_LV_INF_ID		(1u)
#define RIVP_LDD_LOG_LV_WNG_ID		(2u)
#define RIVP_LDD_LOG_LV_ERR_PRM_ID	(3u)
#define RIVP_LDD_LOG_LV_ERR_CTX_ID	(4u)
#define RIVP_LDD_LOG_LV_ERR_LIB_ID	(5u)
#define RIVP_LDD_LOG_LV_ERR_SYS_ID	(6u)
#define RIVP_LDD_LOG_LV_MAX_ID		(7u)

/* levels for RIVP_DepLogSetLevel */
#define RIVP_LDD_LOG_LV_INF			(RIVP_LDD_LOG_TMP_BIT << RIVP_LDD_LOG_LV_INF_ID)
#define RIVP_LDD_LOG_LV_WNG			(RIVP_LDD_LOG_TMP_BIT << RIVP_LDD_LOG_LV_WNG_ID)
#define RIVP_LDD_LOG_LV_ERR			((RIVP_LDD_LOG_TMP_BIT << RIVP_LDD_LOG_LV_ERR_PRM_ID)  | \
								    (RIVP_LDD_LOG_TMP_BIT << RIVP_LDD_LOG_LV_ERR_CTX_ID)  | \
								    (RIVP_LDD_LOG_TMP_BIT << RIVP_LDD_LOG_LV_ERR_LIB_ID)  | \
								    (RIVP_LDD_LOG_TMP_BIT << RIVP_LDD_LOG_LV_ERR_SYS_ID))
#define RIVP_LDD_LOG_LV_NON			(0x00000000u)
#define RIVP_LDD_LOG_LV_ALL			(0xFFFFFFFFu)

/* outputs for RIVP_DepLogSetLevel */
#define RIVP_LDD_LOG_OUT_NON		(0u)
#define RIVP_LDD_LOG_OUT_PRT		(1u)
#define RIVP_LDD_LOG_OUT_DMP		(2u)


/***********************************************/
/* LOG Target Settings                         */
/***********************************************/
#define RIVP_LDD_LOG_DMP_ROW_MAX 	(0x00000100u)

/* log macros */
/* PRQA S 3453 35 *//* PRQA S 1030 35 *//* use only when outputting logs for debugging purposes. */
#define RIVP_LDD_LOG_APP_INF(...)		RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_APP_ID, RIVP_LDD_LOG_LV_INF_ID, __VA_ARGS__)
#define RIVP_LDD_LOG_APP_WNG(...)		RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_APP_ID, RIVP_LDD_LOG_LV_WNG_ID, __VA_ARGS__)
#define RIVP_LDD_LOG_APP_ERR_PRM(...)	RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_APP_ID, RIVP_LDD_LOG_LV_ERR_PRM_ID, __VA_ARGS__)
#define RIVP_LDD_LOG_APP_ERR_CTX(...)	RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_APP_ID, RIVP_LDD_LOG_LV_ERR_CTX_ID, __VA_ARGS__)
#define RIVP_LDD_LOG_APP_ERR_LIB(...)	RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_APP_ID, RIVP_LDD_LOG_LV_ERR_LIB_ID, __VA_ARGS__)
#define RIVP_LDD_LOG_APP_ERR_SYS(...)	RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_APP_ID, RIVP_LDD_LOG_LV_ERR_SYS_ID, __VA_ARGS__)

#define RIVP_LDD_LOG_API_INF(...)		RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_API_ID, RIVP_LDD_LOG_LV_INF_ID, __VA_ARGS__)
#define RIVP_LDD_LOG_API_WNG(...)		RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_API_ID, RIVP_LDD_LOG_LV_WNG_ID, __VA_ARGS__)
#define RIVP_LDD_LOG_API_ERR_PRM(...)	RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_API_ID, RIVP_LDD_LOG_LV_ERR_PRM_ID, __VA_ARGS__)
#define RIVP_LDD_LOG_API_ERR_CTX(...)	RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_API_ID, RIVP_LDD_LOG_LV_ERR_CTX_ID, __VA_ARGS__)
#define RIVP_LDD_LOG_API_ERR_LIB(...)	RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_API_ID, RIVP_LDD_LOG_LV_ERR_LIB_ID, __VA_ARGS__)
#define RIVP_LDD_LOG_API_ERR_SYS(...)	RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_API_ID, RIVP_LDD_LOG_LV_ERR_SYS_ID, __VA_ARGS__)

#define RIVP_LDD_LOG_VDP_INF(...)		RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_VDP_ID, RIVP_LDD_LOG_LV_INF_ID, __VA_ARGS__)
#define RIVP_LDD_LOG_VDP_WNG(...)		RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_VDP_ID, RIVP_LDD_LOG_LV_WNG_ID, __VA_ARGS__)
#define RIVP_LDD_LOG_VDP_ERR_PRM(...)	RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_VDP_ID, RIVP_LDD_LOG_LV_ERR_PRM_ID, __VA_ARGS__)
#define RIVP_LDD_LOG_VDP_ERR_CTX(...)	RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_VDP_ID, RIVP_LDD_LOG_LV_ERR_CTX_ID, __VA_ARGS__)
#define RIVP_LDD_LOG_VDP_ERR_LIB(...)	RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_VDP_ID, RIVP_LDD_LOG_LV_ERR_LIB_ID, __VA_ARGS__)
#define RIVP_LDD_LOG_VDP_ERR_SYS(...)	RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_VDP_ID, RIVP_LDD_LOG_LV_ERR_SYS_ID, __VA_ARGS__)

#define RIVP_LDD_LOG_APP_PEF(...)		RIVP_LDD_LOG( RIVP_LDD_LOG_MOD_PEF_ID, RIVP_LDD_LOG_LV_INF_ID, __VA_ARGS__)
/* WNG,ERR_XXX are not used in PEF. */

/******************************************************************************/
/*                    TYPE DEFINITION                                         */
/******************************************************************************/
typedef char			RIVP_DEP_LDD_STR;

/******************************************************************************/
/*                    FUNCTIONS                                               */
/******************************************************************************/
void RIVP_DepLddLog(
	RIVP_LDD_U32 mod,
	RIVP_LDD_U32 lv,
	const RIVP_DEP_LDD_STR *fmt, ...);
/* PRQA S 3453 1 *//* PRQA S 1030 1 *//* use only when outputting logs for debugging purposes. */
#define RIVP_LDD_LOG(mod, lv, ...)	RIVP_DepLddLog(mod, lv, __VA_ARGS__)

extern RIVP_LDD_BOOL RIVP_DepLddLogDmpStart(
	RIVP_DEP_LDD_STR *ptr,
	RIVP_LDD_U32 max);

extern void RIVP_DepLddLogDmpStop(void);

extern void RIVP_DepLddLogSetLevel(
	RIVP_LDD_U32 module,
	RIVP_LDD_U32 level,
	RIVP_LDD_U32 output);


#ifdef __cplusplus
}
#endif

#endif /* RIVP_DEP_LDD_LOG_H */
