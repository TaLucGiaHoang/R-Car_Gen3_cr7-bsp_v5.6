/******************************************************************************
 *
 * PURPOSE
 *   RIVP Reference Application AVC Data
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *
 ******************************************************************************/
/******************************************************************************
* AVC Data
* Copyright (C) 2015 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
#include "rivp_app_ldd_data_avc.h"
#include "rivp_app_ldd_strm_avc_conf025_264_5fr.h"

/******************************************************************************/
/*                         VARIABLE                                           */
/******************************************************************************/
/* H.264/AVC stream data */
const RIVP_LDD_U8 g_rivpAppLddAvcSampleStrm[RIVP_APP_LDD_AVC_DATA_STRM_SIZE] = {
	RIVP_APP_LDD_AVC_DATA_STRM_BINARY
} ;

/* yuv data */
const RIVP_LDD_U8 g_rivpAppLddAvcSampleYuv[RIVP_APP_LDD_AVC_DATA_REF_YUV_SIZE] = {
	RIVP_APP_LDD_AVC_DATA_YUV_BINARY
} ;

