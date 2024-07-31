/*******************************************************************************
 *
 * PURPOSE
 *   RIVP Reference Application Common
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *
 ******************************************************************************/
/******************************************************************************
* Common
* Copyright (C) 2015 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
#include "rivp_app_ldd_cmn_local.h"
#include "rivp_app_ldd_osdep.h"
#include "rivp_app_ldd_mmgr.h"
#include "rivp_app_ldd_dec.h"
#include "rivp_app_ldd_out.h"
#include "rivp_app_rtp.h"

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
#define RIVP_APP_LDD_CMN_STRM_PAGE0			(0u)
#define RIVP_APP_LDD_CMN_STRM_PAGE1			(1u)
#define RIVP_APP_LDD_CMN_BIT_TO_BYTE(BIT)	(((BIT) + 0x07u) >> 3u)
#define RIVP_APP_LDD_CMN_BYTE_TO_BIT(BYTE)	((BYTE) << 3u)
#define	RIVP_APP_LDD_CMN_AU_BYTE			(0x04u)
#define	RIVP_APP_LDD_CMN_GET_AU_ADDR(X)		(((X) >> 2u) << 2u)
#define RIVP_APP_LDD_CMN_SWAP_CACHE(X)		((((X)) >> 24u) | (((X) & 0x00ff0000u) >> 8u) | (((X) & 0x0000ff00u) << 8u) | (((X)) << 24u))

#define RIVP_APP_LDD_CMN_SLEEP_TIME			(1u)

/******************************************************************************/
/*                         VARIABLE                                           */
/******************************************************************************/
const RIVP_LDD_S8 g_rivpAppLddCmnBool[2][6]	 = {"FALSE", "TRUE"};
const RIVP_LDD_S8 g_rivpAppLddCmnResCode[6][8]	 = {"OK", "WARN", "E_PARAM", "E_CTX", "E_LIB", "E_SYS"};
const RIVP_LDD_S8 g_rivpAppLddCmnIntCnt[3][5]	 = {"NONE", "1/2", "1/3"};
const RIVP_LDD_S8 g_rivpAppLddCmnCodec[3][8]	 = {"Unknown", "AVC", "JPG"};

/******************************************************************************/
/*                    EXTERNAL FUNCTIONS                                      */
/******************************************************************************/
/*** RIVP optional Application function ***/

/**
 * \brief			set xy structure(tile to linear convert)
 *
 * \param[in,out]	app				: application control information
 *
 * \retval			RIVP_LDD_TRUE	: success
 * \retval			RIVP_LDD_FALSE	: invalid argument
 *
 * \attention 		only a check using rivp_app_strm_avc_conf025_264_5fr/jpg_cjpeg_000.h.
 *                  other cases are unconfirmed.
 *
 */
RIVP_LDD_BOOL RIVP_AppLddCmnSetDefaultXy(
	RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_BOOL lBoolRet;

	static RIVP_LDD_S8 lYBuff[RIVP_APP_LDD_YBUF_SIZE];
	static RIVP_LDD_S8 lCBuff[RIVP_APP_LDD_CMN_XY_MAX][RIVP_APP_LDD_CBUF_SIZE];

	if (app != RIVP_LDD_NULL) {
		app->xy[RIVP_APP_LDD_CMN_XY_LUMA].lumaChroma	= RIVP_APP_LDD_CMN_XY_LUMA;
		app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].lumaChroma	= RIVP_APP_LDD_CMN_XY_CHROMA;

		app->xy[RIVP_APP_LDD_CMN_XY_LUMA].lAddr[0]		= (RIVP_LDD_U8 *)lYBuff;	/* dst buff of Y */
		app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].lAddr[0]	= (RIVP_LDD_U8 *)lCBuff[0];	/* dst buff of U */
		app->xy[RIVP_APP_LDD_CMN_XY_CHROMA].lAddr[1]	= (RIVP_LDD_U8 *)lCBuff[1];	/* dst buff of V */

		lBoolRet = RIVP_LDD_TRUE;
	} else {
		lBoolRet = RIVP_LDD_FALSE;
	}
	
	return lBoolRet;
}

/**
 * \brief			search startCodeWord in bitstream
 *
 * \param[in]		buffAddr0		  : virtual address of strmBuff0
 * \param[in]		buffSize0		  : size of valid data of strmBuff0
 * \param[in]		buffAddr1		  : virtual address of strmBuff1
 * \param[in]		buffSize1		  : size of valid data of strmBuff1
 * \param[in]		startCodeWord	  : value of startcode
 * \param[in]		startCodeMask	  : mask of startcode
 * \param[in]		startCodeBits	  : number of the bits of startcode
 * \param[in]		checkOverlap	  : check the overlap of startcode
 *
 * \retval			addr			  : detected
 * \retval			RIVP_LDD_NULL	  : not detected
 *
 * \attention 		only a check using rivp_app_strm_avc_conf025_264_5fr/jpg_cjpeg_000.h.
 *                  other cases are unconfirmed.
 */
RIVP_LDD_U8 *RIVP_AppLddCmnSearchStartCode(
	RIVP_LDD_U8		*buffAddr0,
	RIVP_LDD_U32	 buffSize0,
	RIVP_LDD_U8		*buffAddr1,
	RIVP_LDD_U32	 buffSize1,
	RIVP_LDD_U32	 startCodeWord,
	RIVP_LDD_U32	 startCodeMask,
	RIVP_LDD_U32	 startCodeBits,
	RIVP_LDD_U32	 checkOverlap)
{
	RIVP_LDD_U8		*lStartCodePtr;
	RIVP_LDD_U8		*lCurPtr;
	RIVP_LDD_U8		*lEndPtr;
	RIVP_LDD_U8		*lBufEndPtr;
	RIVP_LDD_U32	 lPage;
	RIVP_LDD_U32	 lStartCodeBytes;
	RIVP_LDD_U32	 lStartCode32bit;
	RIVP_LDD_U32	 lAlign;
	RIVP_LDD_U32	 lCache32bit0;
	RIVP_LDD_U32	 lCache32bit1;
	RIVP_LDD_U32	 lCache32bit2;
	RIVP_LDD_U32	 lShifter;
	RIVP_LDD_U32	*lNextAdr;
	RIVP_LDD_U32	 lCurCnt;
	RIVP_LDD_U32	 lBuffSize;
	RIVP_LDD_U32	 lNextOfs;
	RIVP_LDD_U32	 lRaw32bit;
	RIVP_LDD_U32	 lRawBits;
	RIVP_LDD_U32	 lUtmp;
	RIVP_LDD_BOOL	 lBuffRemainFlg;

	/* set initial value */
	lStartCodePtr	= RIVP_LDD_NULL;
	lPage			= RIVP_APP_LDD_CMN_STRM_PAGE0;
	lStartCodeBytes	= RIVP_APP_LDD_CMN_BIT_TO_BYTE( startCodeBits );
	lStartCode32bit	= startCodeWord & startCodeMask;
	lCurPtr			= buffAddr0;
	lEndPtr			= ( buffAddr1 == RIVP_LDD_NULL ) ? ( &lCurPtr[ buffSize0 - lStartCodeBytes ] ) : ( &lCurPtr[ buffSize0 - 1u ] );
	lBufEndPtr		= &lCurPtr[ buffSize0 - 1u ];
	lCurCnt			= 0u;
	lBuffSize		= buffSize0 + buffSize1;
	lBuffRemainFlg	= RIVP_LDD_TRUE;

	/* init cache */
	lAlign = (RIVP_LDD_ADR)lCurPtr % RIVP_APP_LDD_CMN_AU_BYTE;
	if (lAlign == 0u) {
		lCache32bit0 = 0u;
		lCache32bit1 = *(RIVP_LDD_U32 *)RIVP_APP_LDD_CMN_GET_AU_ADDR( (RIVP_LDD_ADR)lCurPtr );
	} else {
		lCache32bit0 = *(RIVP_LDD_U32 *)RIVP_APP_LDD_CMN_GET_AU_ADDR( (RIVP_LDD_ADR)lCurPtr );
		if (RIVP_APP_LDD_CMN_GET_AU_ADDR( (RIVP_LDD_ADR)lCurPtr ) != RIVP_APP_LDD_CMN_GET_AU_ADDR( (RIVP_LDD_ADR)lBufEndPtr )) {
			lCache32bit1 = *(RIVP_LDD_U32 *)RIVP_APP_LDD_CMN_GET_AU_ADDR( (RIVP_LDD_ADR)lCurPtr + RIVP_APP_LDD_CMN_AU_BYTE );
		} else if (buffAddr1 != RIVP_LDD_NULL) {
			lCache32bit1 = *(RIVP_LDD_U32 *)RIVP_APP_LDD_CMN_GET_AU_ADDR( (RIVP_LDD_ADR)buffAddr1 );
		} else {
			lCache32bit1 = 0u;
		}
	}
	lCache32bit0 = RIVP_APP_LDD_CMN_SWAP_CACHE( lCache32bit0 );
	lCache32bit1 = RIVP_APP_LDD_CMN_SWAP_CACHE( lCache32bit1 );

	while (lBuffRemainFlg == RIVP_LDD_TRUE) {

		/* check buffer end */
		if (lCurPtr > lEndPtr) {
			if ((buffAddr1 == RIVP_LDD_NULL) || (lPage == RIVP_APP_LDD_CMN_STRM_PAGE1)) {
				/* no more buffer, quit search */
				lBuffRemainFlg = RIVP_LDD_FALSE;
			} else {
				/* more buffer exist */
				/* switch new page */
				/* |-------|        |-------| */
				/* |       |        |       | */
				/* | page0 |   ==>  | page1 | */
				/* |       |        |       | */
				/* |-------|        |-------| */
				lCurPtr			= buffAddr1;
				lEndPtr			= &lCurPtr[ buffSize1 - lStartCodeBytes ];
				lBufEndPtr		= &lCurPtr[ buffSize1 - 1u ];
				lPage		= RIVP_APP_LDD_CMN_STRM_PAGE1;

				/* init cache */
				lCache32bit1 = *(RIVP_LDD_U32 *)RIVP_APP_LDD_CMN_GET_AU_ADDR( (RIVP_LDD_ADR)lCurPtr );
				lCache32bit1 = RIVP_APP_LDD_CMN_SWAP_CACHE( lCache32bit1 );
			}
		}

		if (lBuffRemainFlg == RIVP_LDD_TRUE) {
			/* update cache */
			lAlign = (RIVP_LDD_ADR)lCurPtr % RIVP_APP_LDD_CMN_AU_BYTE;
			if (lAlign == 0u) {
				lCache32bit0 = lCache32bit1;
				if (RIVP_APP_LDD_CMN_GET_AU_ADDR( (RIVP_LDD_ADR)lCurPtr ) != RIVP_APP_LDD_CMN_GET_AU_ADDR( (RIVP_LDD_ADR)lBufEndPtr )) {
					lCache32bit1 = *(RIVP_LDD_U32 *)RIVP_APP_LDD_CMN_GET_AU_ADDR( (RIVP_LDD_ADR)lCurPtr + RIVP_APP_LDD_CMN_AU_BYTE );
				} else if ((lPage == RIVP_APP_LDD_CMN_STRM_PAGE0) && (buffAddr1 != RIVP_LDD_NULL)) {
					lCache32bit1 = *(RIVP_LDD_U32 *)RIVP_APP_LDD_CMN_GET_AU_ADDR( (RIVP_LDD_ADR)buffAddr1 );
				} else {
					lCache32bit1 = 0u;
				}
				lCache32bit1 = RIVP_APP_LDD_CMN_SWAP_CACHE( lCache32bit1 );
			}

			/* compare start code */
			lShifter = RIVP_APP_LDD_CMN_BYTE_TO_BIT( lAlign );

			lRaw32bit = lCache32bit0 << lShifter;
			if (lShifter > 0u) {
				lRaw32bit |= lCache32bit1 >> ( RIVP_APP_LDD_CMN_BYTE_TO_BIT( RIVP_APP_LDD_CMN_AU_BYTE ) - lShifter );
			}
			if ((lRaw32bit & startCodeMask) == lStartCode32bit) {

				if (checkOverlap != 0u) {
					/* check if the start codes are overlap each other */
					/* stream         : detect start code         */
					/* 000001xxyy     : 000001xx                  */
					/* 000001000001zz : 000001zz ( not 00000100 ) */

					/* cache32bit1 */
					if (startCodeBits == RIVP_APP_LDD_CMN_BYTE_TO_BIT( RIVP_APP_LDD_CMN_AU_BYTE )) {
						lRaw32bit	     = lCache32bit1 << ( RIVP_APP_LDD_CMN_BYTE_TO_BIT( RIVP_APP_LDD_CMN_AU_BYTE ) - lShifter );
						lRawBits	     = lShifter;
					} else {
						lRaw32bit	   <<= startCodeBits;
						lRawBits	     = ( RIVP_APP_LDD_CMN_BYTE_TO_BIT( RIVP_APP_LDD_CMN_AU_BYTE ) - startCodeBits );
						lRaw32bit	    |= ( lCache32bit1 << lShifter ) >> lRawBits;
						lUtmp		     = RIVP_APP_LDD_CMN_BYTE_TO_BIT( RIVP_APP_LDD_CMN_AU_BYTE ) - lShifter;
						lRawBits	    += ( lUtmp <= startCodeBits ) ? lUtmp : startCodeBits;
					}
					/* cache32bit2 */
					if (lRawBits < startCodeBits) {
						lNextOfs = lCurCnt + ( RIVP_APP_LDD_CMN_AU_BYTE * 2u );

						if (lNextOfs < buffSize0) {
							lNextAdr     = (RIVP_LDD_U32*)RIVP_APP_LDD_CMN_GET_AU_ADDR( (RIVP_LDD_ADR)buffAddr0 + lNextOfs );
							lCache32bit2 = *lNextAdr;
							lCache32bit2 = RIVP_APP_LDD_CMN_SWAP_CACHE( lCache32bit2 );

							lRaw32bit   |= lCache32bit2 >> lRawBits;
							lRawBits     = RIVP_APP_LDD_CMN_BYTE_TO_BIT( RIVP_APP_LDD_CMN_AU_BYTE );
						} else if ((buffAddr1 != RIVP_LDD_NULL) &&
									(lNextOfs < lBuffSize)) {
							lNextAdr     = (RIVP_LDD_U32*)RIVP_APP_LDD_CMN_GET_AU_ADDR( ((RIVP_LDD_ADR)buffAddr1 + lNextOfs) - buffSize0 );
							lCache32bit2 = *lNextAdr;
							lCache32bit2 = RIVP_APP_LDD_CMN_SWAP_CACHE( lCache32bit2 );

							lRaw32bit   |= lCache32bit2 >> lRawBits;
							lRawBits     = RIVP_APP_LDD_CMN_BYTE_TO_BIT( RIVP_APP_LDD_CMN_AU_BYTE );
						} else {
							/* Nothing to do */
						}
					}
					/* check startCode */
					if ((lRawBits < startCodeBits) ||
						((lRaw32bit & startCodeMask) != lStartCode32bit)) {
						/* found startCode */
						lStartCodePtr = lCurPtr;
						lBuffRemainFlg = RIVP_LDD_FALSE;
					}
				} else {
					lStartCodePtr  = lCurPtr;
					lBuffRemainFlg = RIVP_LDD_FALSE;
				}
			}
		}

		/* increment address */
		lCurPtr++;
		lCurCnt++;
	}

	return lStartCodePtr;
}

/**
 * \brief			update RIVP_LDD_INPUTPARAM_T(stream information)
 *
 * \param[in,out]	app		  : application control information
 * \param[in,out]	file	  : input/output information
 *
 * \retval			none
 *
 * \attention 		only a check using rivp_app_strm_avc_conf025_264_5fr/jpg_cjpeg_000.h.
 *                  other cases are unconfirmed.
 *
 */
void RIVP_AppLddCmnUpdateInputParam(
	RIVP_APP_LDD_CMN_PARAM_T *app,
	RIVP_APP_LDD_CMN_FILE_T *file)
{
	RIVP_LDD_U32 lNextBuffOffset;

	if ((app != RIVP_LDD_NULL) && (file != RIVP_LDD_NULL)) {
		if (app->firstInput == RIVP_LDD_FALSE) {
			/* set default input parameter */
			app->strmInfo.strmBuffNum	 = 1u;

			if (app->useRingBuff == RIVP_LDD_TRUE) {
				/* in the ring buffer using, read a stream for ring buffer size, and the remaining stream reads after decode. */
				RIVP_LDD_U32 ringBuffOffset = RIVP_APP_LDD_RING_BUFF_SIZE - ((file->fSize * 3u) / 5u);
				/* sample stream size is smaller than RIVP_APP_LDD_RING_BUFF_SIZE, offset the start point as below. */
				/* |<--- RIVP_APP_LDD_RING_BUFF_SIZE --->| */
				/* |*Buff1*|-----------------|*Buff0*| */
				/* |ringBuffOffset --------->|         */

				app->strmInfo.strmBuff[0].virtAddr	 = (RIVP_LDD_PTR)((RIVP_LDD_ADR)app->strmRingBuff + ringBuffOffset);
				app->strmInfo.strmBuff[0].size		 = RIVP_APP_LDD_RING_BUFF_SIZE - ringBuffOffset;
				(void)memcpy(app->strmInfo.strmBuff[0].virtAddr, app->strmEsBuff, app->strmInfo.strmBuff[0].size);

				app->strmInfo.strmBuff[1].virtAddr	 = app->strmRingBuff;
				app->strmInfo.strmBuff[1].size		 = file->fSize - app->strmInfo.strmBuff[0].size;
				(void)memcpy(app->strmInfo.strmBuff[1].virtAddr, (RIVP_LDD_PTR)((RIVP_LDD_ADR)app->strmEsBuff + app->strmInfo.strmBuff[0].size), app->strmInfo.strmBuff[1].size);

				file->fPos = app->strmInfo.strmBuff[0].size + app->strmInfo.strmBuff[1].size;
				app->strmInfo.strmBuffNum			 = 2u;

			} else {
				app->strmInfo.strmBuff[0].virtAddr	 = app->strmEsBuff;
				app->strmInfo.strmBuff[0].size		 = file->fSize;
			}

		} else {
			/* set next input parameter */

			/* The input parameter makes addition and subtraction by the decodedStrmSize[]. */
			lNextBuffOffset = app->decodedStrmSize[RIVP_LDD_STRM_FIELD_FIRST] + app->decodedStrmSize[RIVP_LDD_STRM_FIELD_SECOND];
			if (lNextBuffOffset > RIVP_APP_LDD_SCRH_OFFSET) {
				/* Error measures, perform the next start cord search from the address that subtracted 128 bytes from from decodedStrmSize. */
				lNextBuffOffset -= RIVP_APP_LDD_SCRH_OFFSET;
			}

			if (app->useRingBuff == RIVP_LDD_TRUE) {
				/* when there is a next start point in buff1, copy buff1 to buff0 */
				/* and subtract size of buff0 from lNextBuffOffset */
				if (app->strmInfo.strmBuff[0].size <= lNextBuffOffset) {
					lNextBuffOffset -= app->strmInfo.strmBuff[0].size;
					if ((lNextBuffOffset > 0u) || (app->strmInfo.strmBuff[1].size > 0u)) {
						app->strmInfo.strmBuff[0].virtAddr	 = app->strmInfo.strmBuff[1].virtAddr;
						app->strmInfo.strmBuff[0].size		 = app->strmInfo.strmBuff[1].size;
						app->strmInfo.strmBuffNum			 = 1u;
					} else {
						/* size of buff1 is less than lNextBuffOffset */
						app->strmInfo.strmBuff[0].virtAddr	 = RIVP_LDD_NULL;
						app->strmInfo.strmBuff[0].size		 = 0u;
						app->strmInfo.strmBuffNum			 = 0u;
					}
					app->strmInfo.strmBuff[1].virtAddr		 = RIVP_LDD_NULL;
					app->strmInfo.strmBuff[1].size		 	= 0u;
				}
			}

			/* update strmBuff0 information */
			if (app->strmInfo.strmBuff[0].size > lNextBuffOffset) {
				app->strmInfo.strmBuff[0].virtAddr	 = (RIVP_LDD_PTR)((RIVP_LDD_ADR)app->strmInfo.strmBuff[0].virtAddr + lNextBuffOffset);
				app->strmInfo.strmBuff[0].size		 = app->strmInfo.strmBuff[0].size - lNextBuffOffset;

			} else {
				/* End of Stream */
				app->strmInfo.strmBuff[0].virtAddr	 = RIVP_LDD_NULL;
				app->strmInfo.strmBuff[0].size		 = 0u;
				app->strmInfo.strmBuffNum			 = 0u;
			}

			/* read elementary stream to ring buffer */
			if ((app->useRingBuff == RIVP_LDD_TRUE) && (file->fSize != file->fPos)) {
				RIVP_LDD_ADR readAddr  = 0uL;
				RIVP_LDD_U32 copySize  = 0u;
				RIVP_LDD_U32 remainStream = file->fSize - file->fPos;

				if (app->strmInfo.strmBuff[0].virtAddr == RIVP_LDD_NULL) {
					/* fail safe */
					readAddr = (RIVP_LDD_ADR)app->strmRingBuff;
					copySize = RIVP_APP_LDD_MIN(RIVP_APP_LDD_RING_BUFF_SIZE, remainStream);
					app->strmInfo.strmBuff[0].virtAddr	 = (RIVP_LDD_PTR)readAddr;
					app->strmInfo.strmBuff[0].size		 = copySize;
					app->strmInfo.strmBuffNum			 = 1u;

				} else {
					/* |<---------------------------- ringBuffSize --------------------------->| */
					/* |<-- strmBuff[1].size -->|***** read area *****|<-- strmBuff[0].size -->| */
					/* |<-- strmBuff[1].virtAddr|                     |<-- strmBuff[0].virtAddr| */
					if (app->strmInfo.strmBuff[1].virtAddr == RIVP_LDD_NULL) {
						app->strmInfo.strmBuff[1].virtAddr	 = app->strmRingBuff;
						app->strmInfo.strmBuff[1].size		 = 0u;
					}
					readAddr = (RIVP_LDD_ADR)app->strmInfo.strmBuff[1].virtAddr + app->strmInfo.strmBuff[1].size;
					copySize = RIVP_APP_LDD_MIN(((RIVP_LDD_ADR)app->strmInfo.strmBuff[0].virtAddr - readAddr), remainStream);
					app->strmInfo.strmBuff[1].size		+= copySize;
					app->strmInfo.strmBuffNum			 = 2u;
				}
				/* read stream and update filePos */
				(void)memcpy((RIVP_LDD_PTR)readAddr, &app->strmEsBuff[file->fPos], copySize);
				file->fPos += copySize;
			}
		}
	}
}

/**
 * \brief			allocate app/file/config area by RIVP_AppLddMemAlloc()
 *
 * \param[out]		app		    	: application control information pointer
 * \param[out]		file	    	: input/output information pointer
 * \param[out]		codecConfig 	: configuration for avc pointer
 *
 * \retval			RIVP_LDD_TRUE	: success
 * \retval			RIVP_LDD_FALSE	: invalid argument/fail allocate
 *
 * \attention 		only a check using rivp_app_strm_avc_conf025_264_5fr/jpg_cjpeg_000.h.
 *                  other cases are unconfirmed.
 *
 */
RIVP_LDD_BOOL RIVP_AppLddCmnMemAlloc(
	RIVP_APP_LDD_CMN_PARAM_T **app,
	RIVP_APP_LDD_CMN_FILE_T **file,
	RIVP_LDD_U32 **codecConfig)
{
	RIVP_LDD_BOOL lBoolRet;

	if ((app != RIVP_LDD_NULL) && (file != RIVP_LDD_NULL) && (codecConfig != RIVP_LDD_NULL)) {
		/* allocate APP info */
		lBoolRet = RIVP_AppLddMemAlloc((RIVP_LDD_PTR*)app, sizeof(RIVP_APP_LDD_CMN_PARAM_T));
		if (lBoolRet == RIVP_LDD_TRUE) {
			lBoolRet = RIVP_AppLddMemAlloc((RIVP_LDD_PTR*)file, sizeof(RIVP_APP_LDD_CMN_FILE_T));
		}
		if (lBoolRet == RIVP_LDD_TRUE) {
			lBoolRet = RIVP_AppLddMemAlloc((RIVP_LDD_PTR*)codecConfig, RIVP_APP_LDD_CODEC_CONFIG_SIZE);
		}
		if (lBoolRet != RIVP_LDD_TRUE) {
			RIVP_LDD_LOG_APP_ERR_SYS("Resource of application is not allocated.\n");
			RIVP_AppLddCmnMemFree(*app, *file, *codecConfig);

		} else {
			/* clear local memory */
			(void)memset(*app,		0, sizeof(RIVP_APP_LDD_CMN_PARAM_T));
			(void)memset(*file,		0, sizeof(RIVP_APP_LDD_CMN_FILE_T));
			(void)memset(*codecConfig,0, RIVP_APP_LDD_CODEC_CONFIG_SIZE);
		}
	} else {
		lBoolRet = RIVP_LDD_FALSE;
	}

	return lBoolRet;
}
/**
 * \brief			free app/file/config area by RIVP_AppLddMemFree()
 *
 * \param[in]		app		    : application control information pointer
 * \param[in]		file	    : input/output information pointer
 * \param[in]		codecConfig : configuration for avc pointer
 *
 * \retval			none
 *
 * \attention 		only a check using rivp_app_strm_avc_conf025_264_5fr/jpg_cjpeg_000.h.
 *                  other cases are unconfirmed.
 *
 */
void RIVP_AppLddCmnMemFree(
	RIVP_APP_LDD_CMN_PARAM_T *app,
	RIVP_APP_LDD_CMN_FILE_T *file,
	RIVP_LDD_U32 *codecConfig)
{
	if (app != RIVP_LDD_NULL) {
		(void)RIVP_AppLddMemFree((RIVP_LDD_PTR)app);
	}
	if (file != RIVP_LDD_NULL) {
		(void)RIVP_AppLddMemFree((RIVP_LDD_PTR)file);
	}
	if (codecConfig != RIVP_LDD_NULL) {
		(void)RIVP_AppLddMemFree((RIVP_LDD_PTR)codecConfig);
	}
}
/**
 * \brief			allocate stream/frame area by RIVP_AppLddMmgrAlloc()
 *
 * \param[in,out]	app		    : application control information
 * \param[in]		file	    : input/output information
 *
 * \retval			RIVP_LDD_TRUE	: success
 * \retval			RIVP_LDD_FALSE	: invalid argument
 *
 * \attention 		only a check using rivp_app_strm_avc_conf025_264_5fr/jpg_cjpeg_000.h.
 *                  other cases are unconfirmed.
 *
 */
RIVP_LDD_BOOL RIVP_AppLddCmnMemAllocOsal(
	RIVP_APP_LDD_CMN_PARAM_T *app,
	RIVP_APP_LDD_CMN_FILE_T *file)
{
	RIVP_LDD_BOOL	 lBoolRet;
	RIVP_LDD_PTR	 lFmemVirtAddr;

	/* set initial value */
	lBoolRet = RIVP_LDD_FALSE;
	
	if (app != RIVP_LDD_NULL) {
		/* allocate stream buffer. add a margin(1024byte) to buffer size. */
		app->strmEsBuff = (RIVP_LDD_U8*)RIVP_AppLddMmgrAlloc(file->fSize + RIVP_APP_LDD_STRM_MARGIN, RIVP_APP_LDD_STRM_ALIGN);

		/* allocate frame memory */
		lFmemVirtAddr = RIVP_AppLddMmgrAlloc(app->ctxParam.fmemSize, RIVP_APP_LDD_FMEM_ALIGN);
		/* convert to physical memory address for RIVP_LDD_CONTEXTPARAM_T */
		app->ctxParam.fmemPhysAddr = (RIVP_LDD_U32)RIVP_AppLddMmgrVtoP(lFmemVirtAddr);
		/* Invalidate the output buffer cache */
		RIVP_AppLddMmgrInvalidate(lFmemVirtAddr);

		/* allocate work buffer */
		app->ctxParam.hwWork.virtAddr = RIVP_AppLddMmgrAlloc((app->ctxParam.hwWork.size + RIVP_APP_LDD_WORKBUF_MARGIN), RIVP_APP_LDD_WORKBUF_ALIGN);
		app->ctxParam.hwWork.physAddr = (RIVP_LDD_U32)RIVP_AppLddMmgrVtoP(app->ctxParam.hwWork.virtAddr);
		app->ctxParam.hwWork.buffHdl = (RIVP_LDD_PTR)RIVP_AppLddMmgrGetBufHdl(app->ctxParam.hwWork.virtAddr);
		/* invalidate cache */
		RIVP_AppLddMmgrInvalidate(app->ctxParam.hwWork.virtAddr);

		/* allocate ring buffer(if use) */
		if ((app->strmEsBuff != RIVP_LDD_NULL) && (lFmemVirtAddr != RIVP_LDD_NULL) && (app->ctxParam.hwWork.virtAddr != RIVP_LDD_NULL)) {
			lBoolRet = RIVP_LDD_TRUE;
			if (app->useRingBuff == RIVP_LDD_TRUE) {
				RIVP_LDD_LOG_APP_INF("Ring buffer test.\n");
				app->strmRingBuff = (RIVP_LDD_U8*)RIVP_AppLddMmgrAlloc(RIVP_APP_LDD_RING_BUFF_SIZE + RIVP_APP_LDD_STRM_MARGIN, RIVP_APP_LDD_STRM_ALIGN);
				if (app->strmRingBuff == RIVP_LDD_NULL) {
					lBoolRet = RIVP_LDD_FALSE;
				}
			}
		}

		if (lBoolRet == RIVP_LDD_TRUE) {
			/* copy stream to memory */
			(void)memcpy(app->strmEsBuff, file->fSrc, file->fSize);
			/* flash memory */
			//TODO flush RIVP_AppLddMmgrFlush((RIVP_LDD_PTR)app->strmEsBuff);

		} else {
			RIVP_LDD_LOG_APP_ERR_SYS("Resource of application is not allocated.\n");
		}
	}

	return lBoolRet;
}
/**
 * \brief			free stream/frame area by RIVP_AppLddMmgrFree()
 *
 * \param[in]		app		    : application control information
 *
 * \retval			none
 *
 * \attention 		only a check using rivp_app_strm_avc_conf025_264_5fr/jpg_cjpeg_000.h.
 *                  other cases are unconfirmed.
 *
 */
void RIVP_AppLddCmnMemFreeOsal(
	RIVP_APP_LDD_CMN_PARAM_T *app)
{
	if (app != RIVP_LDD_NULL) {
		if (app->strmRingBuff != RIVP_LDD_NULL) {
			RIVP_AppLddMmgrFree(app->strmRingBuff);
		}
		if (app->ctxParam.hwWork.virtAddr != RIVP_LDD_NULL) {
			RIVP_AppLddMmgrFree(app->ctxParam.hwWork.virtAddr);
		}
		if (app->ctxParam.fmemPhysAddr != 0u) {
			RIVP_AppLddMmgrFree(RIVP_AppLddMmgrPtoV(app->ctxParam.fmemPhysAddr));
		}
		if (app->strmEsBuff != RIVP_LDD_NULL) {
			RIVP_AppLddMmgrFree(app->strmEsBuff);
		}
	}
}

/**
 * \brief			get current time for performance test
 *
 * \param[out]		timeInfo	: current time information
 *
 * \retval			none
 *
 * \attention 		only a check using rivp_app_strm_avc_conf025_264_5fr/jpg_cjpeg_000.h.
 *                  other cases are unconfirmed.
 *
 */
void RIVP_AppLddCmnGetCurTime( RIVP_APP_LDD_TIME_INFO_T *timeInfo )
{
	if (timeInfo != RIVP_LDD_NULL) {
		RIVP_AppLddOsalGetCurTime(&timeInfo->tvSec, &timeInfo->tvNsec);
	}
}

/**
 * \brief			get process time of kind
 *
 * \param[in,out]	app		    : application control information
 * \param[in]		kind	    : target point
 *
 * \retval			none
 *
 * \attention 		only a check using rivp_app_strm_avc_conf025_264_5fr/jpg_cjpeg_000.h.
 *                  other cases are unconfirmed.
 *
 */
void RIVP_AppLddCmnGetProcTime(
	RIVP_APP_LDD_CMN_PARAM_T *app, RIVP_APP_LDD_FUNC_KIND kind)
{
	RIVP_APP_LDD_TIME_INFO_T 	lTimePrev;
	RIVP_APP_LDD_TIME_INFO_T 	lTimeCur;
	
	RIVP_LDD_U64 				lProcTime;

	if (app != RIVP_LDD_NULL) {
		RIVP_AppLddCmnGetCurTime(&lTimeCur);
		if ((lTimeCur.tvSec != 0u) || (lTimeCur.tvNsec != 0u)) {
			/* set stat time */
			switch (kind) {
			case RIVP_APP_LDD_FUNC_INIT:
				lTimePrev = app->perf.funcStart;
				app->perf.init = lTimePrev;	/* for initialize to 1st CbPicOut */
				break;
			case RIVP_APP_LDD_FUNC_INPUT:
				lTimePrev = app->perf.funcStart;
				app->perf.input = lTimePrev;	/* for input to CbPicOut */
				break;
			case RIVP_APP_LDD_FUNC_PICOUT:
				lTimePrev = app->perf.input;
				break;
			case RIVP_APP_LDD_FUNC_FIRST_PIC:
				lTimePrev = app->perf.init;
				break;
			default:
				lTimePrev = app->perf.funcStart;
				break;
			}

			/* get proc time */
			lProcTime = ((lTimeCur.tvSec * RIVP_APP_LDD_TIME_BASE_SEC) + lTimeCur.tvNsec)			/* Current Time */
						- ((lTimePrev.tvSec * RIVP_APP_LDD_TIME_BASE_SEC) + lTimePrev.tvNsec);	/* Previous Time */

			/* update average */
			app->perf.funcAve[kind] += lProcTime;

			/* update max */
			if (app->perf.funcMax[kind] < lProcTime) {
				app->perf.funcMax[kind] = lProcTime;
			}

			/* update count */
			app->perf.funcCnt[kind] += 1u;
		}
	}
}

/**
 * \brief			print result of performance
 *
 * \param[in,out]	app		  : application control information
 *
 * \retval			none
 *
 * \attention 		only a check using rivp_app_strm_avc_conf025_264_5fr/jpg_cjpeg_000.h.
 *                  other cases are unconfirmed.
 *
 */
void RIVP_AppLddCmnPerformanceResult(
	RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_U32 lCnt;

	if (app != RIVP_LDD_NULL) {
		/* culculate averate time, and modify time scale(ns -> us) */
		for (lCnt = 0u; lCnt < (RIVP_LDD_U32)RIVP_APP_LDD_FUNC_MAX; lCnt++) {
			if (app->perf.funcCnt[lCnt] != 0u) {
				app->perf.funcAve[lCnt] = app->perf.funcAve[lCnt] / app->perf.funcCnt[lCnt];
			}
			app->perf.funcAve[lCnt] = app->perf.funcAve[lCnt] / RIVP_APP_LDD_TIME_BASE_USEC;
			app->perf.funcMax[lCnt] = app->perf.funcMax[lCnt] / RIVP_APP_LDD_TIME_BASE_USEC;
		}
		RIVP_LDD_LOG_APP_PEF("----- performance of RIVP func -----\n");
		RIVP_LDD_LOG_APP_PEF("----- size:width=%d, height=%d -----\n", app->ctxParam.maxWidth, app->ctxParam.maxHeight);
		RIVP_APP_LDD_CMN_PEF_PRT(app, "RIVP_LddInitialize",		 RIVP_APP_LDD_FUNC_INIT		);
		RIVP_APP_LDD_CMN_PEF_PRT(app, "RIVP_LddGetContext",		 RIVP_APP_LDD_FUNC_GET_CTX	);
		RIVP_APP_LDD_CMN_PEF_PRT(app, "RIVP_LddInput",			 RIVP_APP_LDD_FUNC_INPUT	);
		RIVP_APP_LDD_CMN_PEF_PRT(app, "RIVP_LddReleasePicture",	 RIVP_APP_LDD_FUNC_REL_PIC	);
		RIVP_APP_LDD_CMN_PEF_PRT(app, "RIVP_LddFreeContext",	 RIVP_APP_LDD_FUNC_FREE_CTX	);
		RIVP_APP_LDD_CMN_PEF_PRT(app, "RIVP_LddDeinitialize",	 RIVP_APP_LDD_FUNC_DEINIT	);
		RIVP_APP_LDD_CMN_PEF_PRT(app, "RIVP_LddGetVersion",		 RIVP_APP_LDD_FUNC_GETVER	);
		RIVP_APP_LDD_CMN_PEF_PRT(app, "RIVP_LddGetMemSize",		 RIVP_APP_LDD_FUNC_GETMEM	);
		RIVP_LDD_LOG_APP_PEF("----- performance of RIVP decode(w/o fopen,fread) -----\n");
		RIVP_APP_LDD_CMN_PEF_PRT(app, "Init  to 1st CB_PICOUT",	 RIVP_APP_LDD_FUNC_FIRST_PIC);
		RIVP_APP_LDD_CMN_PEF_PRT(app, "Input to CB_PICOUT",		 RIVP_APP_LDD_FUNC_PICOUT	);
	}
}

/**
 * \brief			create OS resources
 *
 * \param[in,out]	app				: application control information
 *
 * \retval			RIVP_LDD_TRUE	: success
 * \retval			RIVP_LDD_FALSE	: invalid argument/OSAL function returns failure
 *
 */
RIVP_LDD_BOOL RIVP_AppLddCmnCreateResource(
	RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_BOOL					 lBoolRet;

	/* set initial value */
	lBoolRet = RIVP_LDD_FALSE;

	/* check arguments */
	if (app != RIVP_LDD_NULL) {
		/* create message queue of main */
		app->msgInf[RIVP_APP_LDD_MAIN_MSG_QUEUE].num = RIVP_APP_LDD_NUM_MAIN_MSG;
		app->msgInf[RIVP_APP_LDD_MAIN_MSG_QUEUE].size = RIVP_APP_LDD_SIZE_MAIN_DEC_MSG;
		app->msgInf[RIVP_APP_LDD_MAIN_MSG_QUEUE].osalMqId = RIVP_APP_LDD_OSAL_MQ_MAIN_ID;
		lBoolRet = RIVP_AppLddOsalQueueCreate(&app->msgInf[RIVP_APP_LDD_MAIN_MSG_QUEUE]);

		/* create message queue of decode task */
		if (lBoolRet == RIVP_LDD_TRUE) {
			app->msgInf[RIVP_APP_LDD_DEC_MSG_QUEUE].num = RIVP_APP_LDD_NUM_DEC_MSG;
			app->msgInf[RIVP_APP_LDD_DEC_MSG_QUEUE].size = RIVP_APP_LDD_SIZE_MAIN_DEC_MSG;
			app->msgInf[RIVP_APP_LDD_DEC_MSG_QUEUE].osalMqId = RIVP_APP_LDD_OSAL_MQ_DEC_ID;
			lBoolRet = RIVP_AppLddOsalQueueCreate(&app->msgInf[RIVP_APP_LDD_DEC_MSG_QUEUE]);
		}

		/* create message queue of callback of stream free */
		if (lBoolRet == RIVP_LDD_TRUE) {
			app->msgInf[RIVP_APP_LDD_STRMFREE_MSG_QUEUE].num = RIVP_APP_LDD_NUM_DEC_MSG;
			app->msgInf[RIVP_APP_LDD_STRMFREE_MSG_QUEUE].size = RIVP_APP_LDD_SIZE_MAIN_DEC_MSG;
			app->msgInf[RIVP_APP_LDD_STRMFREE_MSG_QUEUE].osalMqId = RIVP_APP_LDD_OSAL_MQ_SRM_ID;
			lBoolRet = RIVP_AppLddOsalQueueCreate(&app->msgInf[RIVP_APP_LDD_STRMFREE_MSG_QUEUE]);
		}

		/* create message queue of callback of picture output */
		if (lBoolRet == RIVP_LDD_TRUE) {
			app->msgInf[RIVP_APP_LDD_PICOUT_MSG_QUEUE].num = RIVP_APP_LDD_NUM_OUTPUT_MSG;
			app->msgInf[RIVP_APP_LDD_PICOUT_MSG_QUEUE].size = RIVP_APP_LDD_SIZE_OUTPUT_MSG;
			app->msgInf[RIVP_APP_LDD_PICOUT_MSG_QUEUE].osalMqId = RIVP_APP_LDD_OSAL_MQ_PIC_ID;
			lBoolRet = RIVP_AppLddOsalQueueCreate(&app->msgInf[RIVP_APP_LDD_PICOUT_MSG_QUEUE]);
		}

		/* create message queue of callback of rtp output */
		if (lBoolRet == RIVP_LDD_TRUE) {
			app->msgInf[RIVP_APP_LDD_RTP_MSG_QUEUE].num = RIVP_APP_LDD_NUM_RTP_MSG;
			app->msgInf[RIVP_APP_LDD_RTP_MSG_QUEUE].size = RIVP_APP_LDD_SIZE_RTP_MSG;
			app->msgInf[RIVP_APP_LDD_RTP_MSG_QUEUE].osalMqId = RIVP_APP_LDD_OSAL_MQ_RTP_ID;
			lBoolRet = RIVP_AppLddOsalQueueCreate(&app->msgInf[RIVP_APP_LDD_RTP_MSG_QUEUE]);
		}

		/* create message queue of callback of rtp free */
		if (lBoolRet == RIVP_LDD_TRUE) {
			app->msgInf[RIVP_APP_LDD_RTPFREE_MSG_QUEUE].num = RIVP_APP_LDD_NUM_RTP_MSG;
			app->msgInf[RIVP_APP_LDD_RTPFREE_MSG_QUEUE].size = RIVP_APP_LDD_SIZE_RTPFREE_MSG;
			app->msgInf[RIVP_APP_LDD_RTPFREE_MSG_QUEUE].osalMqId = RIVP_APP_LDD_OSAL_MQ_RTPF_ID;
			lBoolRet = RIVP_AppLddOsalQueueCreate(&app->msgInf[RIVP_APP_LDD_RTPFREE_MSG_QUEUE]);
		}

		/* set parameter for decode task create */
		if (lBoolRet == RIVP_LDD_TRUE) {
			/* sets the task name */
			(void)memcpy(app->taskHnd[RIVP_APP_LDD_OSAL_DEC_TASK_ID].name,
					RIVP_APP_LDD_OSAL_TASK_NAME_DEC,
					RIVP_APP_LDD_OSAL_TASK_NAME_SIZE);	/* ToDo: void cast */

			app->taskHnd[RIVP_APP_LDD_OSAL_DEC_TASK_ID].entry = (RIVP_APP_LDD_OSAL_ENTRY)&RIVP_AppLddDecTask;
			app->taskHnd[RIVP_APP_LDD_OSAL_DEC_TASK_ID].priority = OSAL_THREAD_PRIORITY_TYPE9;
			app->taskHnd[RIVP_APP_LDD_OSAL_DEC_TASK_ID].param = (RIVP_LDD_PTR)app;
			app->taskHnd[RIVP_APP_LDD_OSAL_DEC_TASK_ID].osalThreadId = RIVP_APP_LDD_OSAL_THREAD_DEC_ID;

			/* run task of decode */
			lBoolRet = RIVP_AppLddOsalTaskRun(&app->taskHnd[RIVP_APP_LDD_OSAL_DEC_TASK_ID]);
			RIVP_AppLddOsalUSleep(RIVP_APP_LDD_CMN_SLEEP_TIME);
		}
		/* set parameter for create task of output create */
		if (lBoolRet == RIVP_LDD_TRUE) {
			/* sets the task name */
			(void)memcpy(app->taskHnd[RIVP_APP_LDD_OSAL_OUT_TASK_ID].name,
					RIVP_APP_LDD_OSAL_TASK_NAME_OUT,
					RIVP_APP_LDD_OSAL_TASK_NAME_SIZE);	/* ToDo: void cast */

			app->taskHnd[RIVP_APP_LDD_OSAL_OUT_TASK_ID].entry = (RIVP_APP_LDD_OSAL_ENTRY)&RIVP_AppLddOutTask;
			app->taskHnd[RIVP_APP_LDD_OSAL_OUT_TASK_ID].priority = OSAL_THREAD_PRIORITY_TYPE10;
			app->taskHnd[RIVP_APP_LDD_OSAL_OUT_TASK_ID].param = (RIVP_LDD_PTR)app;
			app->taskHnd[RIVP_APP_LDD_OSAL_OUT_TASK_ID].osalThreadId = RIVP_APP_LDD_OSAL_THREAD_OUT_ID;

			lBoolRet = RIVP_AppLddOsalTaskRun(&app->taskHnd[RIVP_APP_LDD_OUT_TASK]);
			RIVP_AppLddOsalUSleep(RIVP_APP_LDD_CMN_SLEEP_TIME);
		}

		/* set parameter for create task of output create */
		if (lBoolRet == RIVP_LDD_TRUE) {
			/* sets the task name */
			(void)memcpy(app->taskHnd[RIVP_APP_LDD_OSAL_RTP_TASK_ID].name,
						 RIVP_APP_LDD_OSAL_TASK_NAME_RTP,
						 RIVP_APP_LDD_OSAL_TASK_NAME_SIZE); /* ToDo: void cast */

			app->taskHnd[RIVP_APP_LDD_OSAL_RTP_TASK_ID].entry = (RIVP_APP_LDD_OSAL_ENTRY)&RIVP_AppRtpTask;
			app->taskHnd[RIVP_APP_LDD_OSAL_RTP_TASK_ID].priority = OSAL_THREAD_PRIORITY_TYPE11;
			app->taskHnd[RIVP_APP_LDD_OSAL_RTP_TASK_ID].param = (RIVP_LDD_PTR)app;
			app->taskHnd[RIVP_APP_LDD_OSAL_RTP_TASK_ID].osalThreadId = RIVP_APP_LDD_OSAL_THREAD_RTP_ID;

			lBoolRet = RIVP_AppLddOsalTaskRun(&app->taskHnd[RIVP_APP_LDD_RTP_TASK]);
			RIVP_AppLddOsalUSleep(RIVP_APP_LDD_CMN_SLEEP_TIME);
		}
	}

	if (lBoolRet == RIVP_LDD_FALSE) {
		RIVP_LDD_LOG_APP_ERR_SYS("Failed to create OS Resources.\n");
	}

	/* return result */
	return lBoolRet;
}

/**
 * \brief			destroy OS resources
 *
 * \param[in]		app				: application control information
 *
 * \retval			RIVP_LDD_TRUE	: success
 * \retval			RIVP_LDD_FALSE	: invalid argument/OSAL function returns failure
 *
 */
RIVP_LDD_BOOL RIVP_AppLddCmnDestroyResource(
	RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_BOOL	 lBoolRet;
	RIVP_LDD_U8		 lCnt;

	/* set initial value */
	lBoolRet = RIVP_LDD_FALSE;

	/* check arguments */
	if (app != RIVP_LDD_NULL) {
		lBoolRet = RIVP_LDD_TRUE;

		/* destroy message queues */
		for (lCnt = 0u; (lBoolRet == RIVP_LDD_TRUE) && (lCnt < RIVP_APP_LDD_MAX_MSG_QUEUE); lCnt++) {
			if (app->msgInf[lCnt].id != RIVP_LDD_NULL) {
				lBoolRet = RIVP_AppLddOsalQueueDestroy(&app->msgInf[lCnt]);
			}
		}

		/* halt and destroy tasks */
		if (lBoolRet == RIVP_LDD_TRUE) {
			for (lCnt = 0u; (lBoolRet == RIVP_LDD_TRUE) && (lCnt < RIVP_APP_LDD_MAX_TASK); lCnt++) {
				if (app->taskHnd[lCnt].id != RIVP_LDD_NULL) {
					lBoolRet = RIVP_AppLddOsalTaskDestroy(&app->taskHnd[lCnt]);
				}
			}
		}
	}

	if (lBoolRet == RIVP_LDD_FALSE) {
		RIVP_LDD_LOG_APP_ERR_SYS("Failed to destroy OS Resources.\n");
	}

	/* return result */
	return lBoolRet;
}

/**
 * \brief			restart tasks
 *
 * \param[in]		app				: application control information
 *
 * \retval			RIVP_LDD_TRUE	: success
 * \retval			RIVP_LDD_FALSE	: invalid argument/OSAL function returns failure
 *
 */
RIVP_LDD_BOOL RIVP_AppLddCmnRestartTask(
	RIVP_APP_LDD_CMN_PARAM_T *app)
{
	RIVP_LDD_BOOL					 lBoolRet;

	/* set initial value */
	lBoolRet = RIVP_LDD_FALSE;

	/* check arguments */
	if (app != RIVP_LDD_NULL) {
		/* run task of decode */
		lBoolRet = RIVP_AppLddOsalTaskRun(&app->taskHnd[RIVP_APP_LDD_DEC_TASK]);

		/* run task of output */
		if (lBoolRet == RIVP_LDD_TRUE) {
			lBoolRet = RIVP_AppLddOsalTaskRun(&app->taskHnd[RIVP_APP_LDD_OUT_TASK]);
		}
	}

	return lBoolRet;
}
