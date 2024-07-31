/******************************************************************************
 *
 * PURPOSE
 *   RIVP Reference Application JPG Data
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *
 ******************************************************************************/
/******************************************************************************
* JPG Data
* Copyright (C) 2015 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
#include "rivp_app_ldd_data_jpg.h"
#include "rivp_app_ldd_strm_jpg_cjpeg_000.h"

/******************************************************************************/
/*                         VARIABLE                                           */
/******************************************************************************/
/* JPEG stream data */
const RIVP_LDD_U8 g_rivpAppLddJpgSampleStrm[RIVP_APP_LDD_JPG_DATA_STRM_SIZE] = {
	RIVP_APP_LDD_JPG_DATA_STRM_BINARY
} ;

/* yuv data */
const RIVP_LDD_U8 g_rivpAppLddJpgSampleYuv[RIVP_APP_LDD_JPG_DATA_REF_YUV_SIZE] = {
	RIVP_APP_LDD_JPG_DATA_YUV_BINARY
} ;

