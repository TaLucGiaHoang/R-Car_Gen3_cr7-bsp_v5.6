/******************************************************************************
 *
 * PURPOSE
 *   RIVP Reference Application test data Defines
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *
 ******************************************************************************/
/******************************************************************************
* AVC test data Defines
* Copyright (C) 2015 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/

#ifndef RIVP_APP_LDD_DATA_AVC_H
#define RIVP_APP_LDD_DATA_AVC_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/*                    INCLUDE FILES                                          */
/*****************************************************************************/
#include "rivp_ldd_types.h"

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
/* definition for H.264/AVC sample stram/yuv */

#define RIVP_APP_LDD_AVC_DATA_REF_YUV_SIZE		(190080u)					/*!< yuv size */
#define RIVP_APP_LDD_AVC_DATA_STRM_FRAME		(5u)						/*!< 5Frame */
#define RIVP_APP_LDD_AVC_DATA_STRM_422			(RIVP_LDD_FALSE)			/*!< YCbCr format of sample stream */
#define RIVP_APP_LDD_AVC_DATA_STRM_12BIT		(RIVP_LDD_FALSE)			/*!< bit depth of sample stream */
#define RIVP_APP_LDD_AVC_DATA_STRM_SIZE			(4000u)						/*!< stream size */
#define RIVP_APP_LDD_AVC_DATA_STRM_WIDTH		(1920)						/*!< picture width  of sample stream */
#define RIVP_APP_LDD_AVC_DATA_STRM_HEIGHT		(1088)						/*!< picture height of sample stream */
#define RIVP_APP_LDD_AVC_DATA_STRM_LEVEL		(RIVP_LDD_AVC_MAX_LEVEL_42)	/*!< level of sample stream */
#define RIVP_APP_LDD_AVC_DATA_BUFFERING			(3u)

#define RIVP_APP_LDD_AVC_DATA_STRM_FIXED		(RIVP_LDD_TRUE)				/*!< reference application is single stream and sample stream is single sequence. */
#define RIVP_APP_LDD_AVC_DATA_STRM_BPP			((RIVP_APP_LDD_AVC_DATA_STRM_12BIT == RIVP_LDD_TRUE) ? 2u : 1u)
#define RIVP_APP_LDD_AVC_DATA_STRM_PITCH		(((RIVP_APP_LDD_AVC_DATA_STRM_WIDTH * RIVP_APP_LDD_AVC_DATA_STRM_BPP) > 2048u) ? 4096u : \
												 ((RIVP_APP_LDD_AVC_DATA_STRM_WIDTH * RIVP_APP_LDD_AVC_DATA_STRM_BPP) > 1024u) ? 2048u : \
												 ((RIVP_APP_LDD_AVC_DATA_STRM_WIDTH * RIVP_APP_LDD_AVC_DATA_STRM_BPP) >  512u) ? 1024u : \
																													 512)

/******************************************************************************/
/*                    FORWARD DECLARATION                                     */
/******************************************************************************/
extern const RIVP_LDD_U8	 g_rivpAppLddAvcSampleStrm[RIVP_APP_LDD_AVC_DATA_STRM_SIZE];
extern const RIVP_LDD_U8	 g_rivpAppLddAvcSampleYuv[RIVP_APP_LDD_AVC_DATA_REF_YUV_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* RIVP_APP_LDD_DATA_AVC_H */
