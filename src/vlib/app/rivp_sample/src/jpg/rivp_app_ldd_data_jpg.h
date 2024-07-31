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
* JPG test data Defines
* Copyright (C) 2015 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/

#ifndef RIVP_APP_LDD_DATA_JPG_H
#define RIVP_APP_LDD_DATA_JPG_H

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
/* definition for JPEG sample stram/yuv */

#define RIVP_APP_LDD_JPG_DATA_REF_YUV_SIZE	(38016u)			/*!< yuv size */
#define RIVP_APP_LDD_JPG_DATA_STRM_FRAME	(1u)				/*!< 1Frame */
#define RIVP_APP_LDD_JPG_DATA_STRM_422		(RIVP_LDD_FALSE)	/*!< YCbCr format of sample stream */
#define RIVP_APP_LDD_JPG_DATA_STRM_12BIT	(RIVP_LDD_FALSE)	/*!< bit depth of sample stream */
#define RIVP_APP_LDD_JPG_DATA_STRM_SIZE		(7000u)				/*!< stream size */
#define RIVP_APP_LDD_JPG_DATA_STRM_WIDTH	(176u)				/*!< picture width  of sample stream */
#define RIVP_APP_LDD_JPG_DATA_STRM_HEIGHT	(144u)				/*!< picture height of sample stream */
#define RIVP_APP_LDD_JPG_DATA_BUFFERING		(0u)


#define RIVP_APP_LDD_JPG_DATA_STRM_BPP		((RIVP_APP_LDD_JPG_DATA_STRM_12BIT == RIVP_LDD_TRUE) ? 2u : 1u)
#define RIVP_APP_LDD_JPG_DATA_STRM_PITCH	(((RIVP_APP_LDD_JPG_DATA_STRM_WIDTH * RIVP_APP_LDD_JPG_DATA_STRM_BPP) > 2048u) ? 4096u : \
											 ((RIVP_APP_LDD_JPG_DATA_STRM_WIDTH * RIVP_APP_LDD_JPG_DATA_STRM_BPP) > 1024u) ? 2048u : \
											 ((RIVP_APP_LDD_JPG_DATA_STRM_WIDTH * RIVP_APP_LDD_JPG_DATA_STRM_BPP) >  512u) ? 1024u : \
																												 512)
/******************************************************************************/
/*                    FORWARD DECLARATION                                     */
/******************************************************************************/
extern const RIVP_LDD_U8	 g_rivpAppLddJpgSampleStrm[RIVP_APP_LDD_JPG_DATA_STRM_SIZE];
extern const RIVP_LDD_U8	 g_rivpAppLddJpgSampleYuv[RIVP_APP_LDD_JPG_DATA_REF_YUV_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* RIVP_APP_LDD_DATA_JPG_H */
