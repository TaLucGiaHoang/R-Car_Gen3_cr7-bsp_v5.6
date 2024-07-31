/******************************************************************************
 *
 * PURPOSE
 *   RIVP JPG Public Defines
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *
 ******************************************************************************/
/**
 * Copyright(C) 2015 Renesas Electronics Corporation. All Rights Reserved.
 * RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
 * This program must be used solely for the purpose for which
 * it was furnished by Renesas Electronics Corporation.
 * No part of this program may be reproduced or disclosed to
 * others, in any form, without the prior written permission
 * of Renesas Electronics Corporation.
 *
 ******************************************************************************/

#ifndef RIVP_LDD_JPG_H
#define RIVP_LDD_JPG_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
#include "rivp_ldd_api.h"

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
/* RIVP_LDD_CONTEXTPARAM_T */
#define RIVP_LDD_JPG_CODEC_TYPE			(2u)

/**
 * errorInfo for RIVP_LDD_RTN_ERROR_PARAM
 */
/* outputLinearFormat in RIVP_LDD_JPG_CONFIG_T is out-of-range */
#define RIVP_LDD_ERR_PARAM_JPG_OUT_LN_FMT		(0x00008000u)
/* normalDecode in RIVP_LDD_JPG_CONFIG_T is out-of-range */
#define RIVP_LDD_ERR_PARAM_JPG_NORMAL_DEC		(0x00010000u)

/******************************************************************************/
/*                    TYPE DEFINITION                                         */
/******************************************************************************/
typedef struct {
	RIVP_LDD_BOOL				 outputLinearFormat;
	/* \attention RIVP_LDD_FALSE is not support on D1H. */
	RIVP_LDD_BOOL				 normalDecode;
} RIVP_LDD_JPG_CONFIG_T;

typedef struct {
	RIVP_LDD_U32				 major;
	RIVP_LDD_U32				 minor;
	RIVP_LDD_U32				 patch;
} RIVP_LDD_JPG_VERSION_T;

/******************************************************************************/
/*                    FUNCTIONS                                               */
/******************************************************************************/
extern RIVP_LDD_RESULT RIVP_LddJpgGetEntryPoint(
	RIVP_LDD_PTR *entryPoint);

extern const RIVP_LDD_JPG_VERSION_T *RIVP_LddJpgGetVersion(void);

#ifdef __cplusplus
}
#endif

#endif /* RIVP_LDD_JPG_H */
