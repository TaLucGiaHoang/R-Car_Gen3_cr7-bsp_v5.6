/******************************************************************************
 *
 * PURPOSE
 *   RIVP Reference Application Common Local Defines
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *
 ******************************************************************************/
/******************************************************************************
* Common Local Defines
* Copyright (C) 2015 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/
#ifndef RIVP_APP_LDD_CMN_LOCAL_H
#define RIVP_APP_LDD_CMN_LOCAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/*                    INCLUDE FILES                                          */
/*****************************************************************************/

#include "rivp_ldd_api.h"
#include "rivp_app_ldd_osdep.h"
#include "rivp_dep_ldd_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
#ifdef RIVP_LDD_64BIT_SUPPORT
typedef unsigned long long	RIVP_LDD_ADR ;
#else
typedef unsigned long		RIVP_LDD_ADR ;
#endif

/* values */
#define RIVP_APP_LDD_CODEC_CONFIG_SIZE		(0x20u)
/* picture size */
#define RIVP_APP_LDD_MAX_WIDTH				(1920u)
#define RIVP_APP_LDD_MAX_HEIGHT				(1088u)
#define RIVP_APP_LDD_STRIDE					(2048u)
/* input stream */
#define RIVP_APP_LDD_STRM_MARGIN			(1024u)
#define RIVP_APP_LDD_MAX_INPUT_SIZE			(0x00FFF000u)
#define RIVP_APP_LDD_STRM_ALIGN				(256u)
#define RIVP_APP_LDD_STRM_ALIGN_MASK		(0xFFu)
#define RIVP_APP_LDD_RING_BUFF_SIZE			(0x10000u)	/*!< 64k. This must be less than RIVP_APP_LDD_MAX_INPUT_SIZE byte. */
#define RIVP_APP_LDD_SCRH_OFFSET			(32u)
#define RIVP_APP_LDD_EOS_ID					(0x0E0F0E0Fu)
/* output picture */
#define RIVP_APP_LDD_PIC_IDX_INVALID_STRM	(32u)
#define RIVP_APP_LDD_FMEM_ALIGN				(4096u)
#define RIVP_APP_LDD_YBUF_SIZE				( RIVP_APP_LDD_STRIDE * RIVP_APP_LDD_MAX_HEIGHT)
#define RIVP_APP_LDD_CBUF_SIZE				((RIVP_APP_LDD_STRIDE * RIVP_APP_LDD_MAX_HEIGHT) >> 1u)
#define RIVP_APP_LDD_PIC_FMT_CHROMA_MASK	(0x00F0u)
#define RIVP_APP_LDD_PIC_FMT_420			(0x0000u)
#define RIVP_APP_LDD_PIC_FMT_422			(0x0010u)
#define RIVP_APP_LDD_PIC_FMT_TL_MASK		(0x000Fu)
#define RIVP_APP_LDD_PIC_FMT_TILE			(0x0000u)
#define RIVP_APP_LDD_PIC_FMT_LINEAR			(0x0001u)
#define RIVP_APP_LDD_ALIGN(A, W)			((((A) + ((W) - 1u)) / (W)) * (W))
#define RIVP_APP_LDD_MIN(a, b)				(((a) < (b)) ? (a) : (b))
#define RIVP_APP_LDD_MAX(a, b)				(((a) < (b)) ? (b) : (a))
/* work buffer */
#define RIVP_APP_LDD_WORKBUF_ALIGN			(256u)
#define RIVP_APP_LDD_WORKBUF_MARGIN			(1024u)
/* time */
#define RIVP_APP_LDD_TIME_BASE_NSEC			(1uLL)
#define RIVP_APP_LDD_TIME_BASE_USEC			(1000uLL * RIVP_APP_LDD_TIME_BASE_NSEC)
#define RIVP_APP_LDD_TIME_BASE_MSEC			(1000uLL * RIVP_APP_LDD_TIME_BASE_USEC)
#define RIVP_APP_LDD_TIME_BASE_SEC			(1000uLL * RIVP_APP_LDD_TIME_BASE_MSEC)

#define RIVP_APP_LDD_CMN_PEF_PRT(app, func, kind)	\
do {											\
	RIVP_LDD_LOG_APP_PEF("%-22s : Cnt=%5d Ave=%6lld[us] Max=%6lld[us]\n", (func), (RIVP_LDD_U32)(app)->perf.funcCnt[(kind)], (app)->perf.funcAve[(kind)], (app)->perf.funcMax[(kind)]);	\
} while (0)

/* task */
#define RIVP_APP_LDD_DEC_TASK				(0u)
#define RIVP_APP_LDD_OUT_TASK				(1u)
#define RIVP_APP_LDD_RTP_TASK				(2u)
#define RIVP_APP_LDD_MAX_TASK				(3u)

/* message queue */
#define RIVP_APP_LDD_MAIN_MSG_QUEUE			(0u)
#define RIVP_APP_LDD_DEC_MSG_QUEUE			(1u)
#define RIVP_APP_LDD_STRMFREE_MSG_QUEUE		(2u)
#define RIVP_APP_LDD_PICOUT_MSG_QUEUE		(3u)
#define RIVP_APP_LDD_RTP_MSG_QUEUE			(4u)
#define RIVP_APP_LDD_RTPFREE_MSG_QUEUE		(5u)
#define RIVP_APP_LDD_MAX_MSG_QUEUE			(6u)
#define RIVP_APP_LDD_NUM_MAIN_MSG			(1u)
#define RIVP_APP_LDD_NUM_DEC_MSG			(100u)
#define RIVP_APP_LDD_NUM_OUTPUT_MSG			(100u)
#define RIVP_APP_LDD_SIZE_MAIN_DEC_MSG		(64u)		/* Sizeof (RIVP_U32) or more */
#define RIVP_APP_LDD_SIZE_OUTPUT_MSG		(196u)		/* sizeof(RIVP_PICPARAM_T) or more */
#define RIVP_APP_LDD_NUM_RTP_MSG			(100u)
#define RIVP_APP_LDD_SIZE_RTP_MSG			(4u)
#define RIVP_APP_LDD_SIZE_RTPFREE_MSG		(4u)

#define RIVP_APP_LDD_FMT_TO_ASCII( fmt, charFmt )					\
do {																\
	switch((fmt)) {													\
	case RIVP_LDD_FORMAT_8BIT_420_TILING:							\
		(void)strncat((char *)(charFmt), "08B420TL", 8u);	break;	\
	case RIVP_LDD_FORMAT_8BIT_420_LINEAR:							\
		(void)strncat((char *)(charFmt), "08B420LN", 8u);	break;	\
	case RIVP_LDD_FORMAT_8BIT_422_TILING:							\
		(void)strncat((char *)(charFmt), "08B422TL", 8u);	break;	\
	case RIVP_LDD_FORMAT_8BIT_422_LINEAR:							\
		(void)strncat((char *)(charFmt), "08B422LN", 8u);	break;	\
	case RIVP_LDD_FORMAT_10BIT_420_TILING:							\
		(void)strncat((char *)(charFmt), "10B420TL", 8u);	break;	\
	case RIVP_LDD_FORMAT_10BIT_420_LINEAR:							\
		(void)strncat((char *)(charFmt), "10B420LN", 8u);	break;	\
	case RIVP_LDD_FORMAT_10BIT_422_TILING:							\
		(void)strncat((char *)(charFmt), "10B422TL", 8u);	break;	\
	case RIVP_LDD_FORMAT_10BIT_422_LINEAR:							\
		(void)strncat((char *)(charFmt), "10B422LN", 8u);	break;	\
	case RIVP_LDD_FORMAT_12BIT_420_TILING:							\
		(void)strncat((char *)(charFmt), "12B420TL", 8u);	break;	\
	case RIVP_LDD_FORMAT_12BIT_420_LINEAR:							\
		(void)strncat((char *)(charFmt), "12B420LN", 8u);	break;	\
	case RIVP_LDD_FORMAT_12BIT_422_TILING:							\
		(void)strncat((char *)(charFmt), "12B422TL", 8u);	break;	\
	case RIVP_LDD_FORMAT_12BIT_422_LINEAR:							\
		(void)strncat((char *)(charFmt), "12B422LN", 8u);	break;	\
	default:														\
		(void)strncat((char *)(charFmt), "Unknown", 8u);	break;	\
	}																\
} while(0)

/* tile to linear convert */
#define RIVP_APP_LDD_CMN_XY_LUMA	(0u)
#define RIVP_APP_LDD_CMN_XY_CHROMA	(1u)	/*!< NV12 */
#define RIVP_APP_LDD_CMN_XY_MAX		(2u)	/*!< NV12 */
#define RIVP_APP_LDD_CMN_XY_16X1L	(0u)
#define RIVP_APP_LDD_CMN_XY_16X2L	(1u)
#define RIVP_APP_LDD_CMN_LN_OUT		(10u)

#define RIVP_APP_LDD_CMN_SWAP_FRAME_SIZE			(2048u * 1088u)
#define RIVP_APP_LDD_CMN_TL_CONV_Y( LOG2STRIDE, Y )					\
do {																\
	lTileOffsetV	 = ( ( (Y) & 0xffffffe0u ) << (LOG2STRIDE) );	\
	lTileOffsetV	+= ( ( (Y) & 0x00000010u ) << 7u );				\
	lTileOffsetV	+= ( ( (Y) & 0x0000000Cu ) << 6u );				\
	lTileOffsetV	+= ( ( (Y) & 0x00000003u ) << 4u );				\
} while(0)

#define RIVP_APP_LDD_CMN_TL_CONV_X( X )								\
do {																\
	lTileOffsetH	 = ( ( (X) & 0xffffff80u ) << 5u );				\
	lTileOffsetH	+= ( ( (X) & 0x00000040u ) << 4u );				\
	lTileOffsetH	+= ( ( (X) & 0x00000030u ) << 2u );				\
	lTileOffsetH	+= (   (X) & 0x0000000Fu );						\
} while(0)

/******************************************************************************/
/*                    TYPE DEFINITION                                         */
/******************************************************************************/
typedef enum {
	RIVP_APP_LDD_FUNC_INIT		= 0,
	RIVP_APP_LDD_FUNC_GET_CTX,
	RIVP_APP_LDD_FUNC_INPUT,
	RIVP_APP_LDD_FUNC_REL_PIC,
	RIVP_APP_LDD_FUNC_FREE_CTX,
	RIVP_APP_LDD_FUNC_DEINIT,
	RIVP_APP_LDD_FUNC_GETVER,
	RIVP_APP_LDD_FUNC_GETMEM,
	RIVP_APP_LDD_FUNC_JPG_GETVER,
	RIVP_APP_LDD_FUNC_JPG_ENTRY,
	RIVP_APP_LDD_FUNC_PICOUT,
	RIVP_APP_LDD_FUNC_FIRST_PIC,
	RIVP_APP_LDD_FUNC_MAX
} RIVP_APP_LDD_FUNC_KIND;

typedef struct {
	RIVP_LDD_U32				 xPicSize;
	RIVP_LDD_U32				 yPicSize;
	RIVP_LDD_U32				 cropTop;
	RIVP_LDD_U32				 cropBot;
	RIVP_LDD_U32				 cropLeft;
	RIVP_LDD_U32				 cropRight;

	RIVP_LDD_U32				 stride;
	RIVP_LDD_U32				 _2lineShift;
	RIVP_LDD_U32				 xyLineMode;

	RIVP_LDD_U32				 xyOnField;
	RIVP_LDD_PTR				 xyTopAddr;
	RIVP_LDD_PTR				 xyBotAddr;

	RIVP_LDD_U32				 lumaChroma;
	RIVP_LDD_U8*				 lAddr[2];
	RIVP_LDD_U32				 bytePerPixel;
} RIVP_APP_LDD_CMN_XY_T;

typedef struct {
	RIVP_LDD_U32				 tvSec ;
	RIVP_LDD_U32				 tvNsec ;
} RIVP_APP_LDD_TIME_INFO_T;

typedef struct {
	RIVP_LDD_U64				 funcCnt[RIVP_APP_LDD_FUNC_MAX];
	RIVP_LDD_U64				 funcAve[RIVP_APP_LDD_FUNC_MAX];
	RIVP_LDD_U64				 funcMax[RIVP_APP_LDD_FUNC_MAX];
	RIVP_APP_LDD_TIME_INFO_T	 funcStart;
	RIVP_APP_LDD_TIME_INFO_T	 init;
	RIVP_APP_LDD_TIME_INFO_T	 input;
} RIVP_APP_LDD_PERFORMANCE_T;

typedef struct {
	const RIVP_LDD_U8			*fSrc;
	RIVP_LDD_U32				 fSize;
	RIVP_LDD_U32				 fPos;
} RIVP_APP_LDD_CMN_FILE_T;

typedef struct {
	RIVP_LDD_PTR				 ctx;										/* context created by RIVP_LddGetContext */
	RIVP_LDD_INITPARAM_T		 initParam;									/* argument for RIVP_LddInitialize */
	RIVP_LDD_OSAL_CONFIG_T		 osCfg;										/* argument for RIVP_LddInitialize */
	RIVP_LDD_CONTEXTPARAM_T		 ctxParam;									/* argument for RIVP_LddGetContext */
	const RIVP_LDD_VERSION_T	*rivpVer;									/* argument for RIVP_LddGetVersion */
	RIVP_LDD_INPUTPARAM_T		 strmInfo;									/* stream information */
	RIVP_LDD_INPUTPARAM_T		 input;										/* argument for RIVP_LddInput */
	RIVP_LDD_U32				 maxOneInputSize;							/* limitation size of input */
	RIVP_LDD_BOOL				 firstInput;								/* flag of 1st picture input */
	RIVP_LDD_U32				 searchMode;								/* mode of start code search */
	RIVP_LDD_U32				 picIndex;									/* picInex from CbPicOut */
	RIVP_LDD_U32				 decodedStrmSize[RIVP_LDD_STRM_FIELD_MAX];	/* decodedStrmSize from cbStrmFree */
	RIVP_LDD_U8					*strmEsBuff;								/* stream buffer pointer */
	RIVP_LDD_U8					 strmEos[4];								/* EOS stream */
	RIVP_LDD_RESULT				 errorCode;									/* CB_FATAL_ERROR argument */
	RIVP_APP_LDD_CMN_FILE_T		*file;										/* file information */
	RIVP_LDD_U32				*codecConfig;								/* codec configuration */
	RIVP_LDD_BOOL				 displayMode;								/* flag to display data */
	/* for output */
	RIVP_APP_LDD_CMN_XY_T		 xy[RIVP_APP_LDD_CMN_XY_MAX];
	/* for Ring buffer */
	RIVP_LDD_BOOL				 useRingBuff;								/* flag of ring buffer using */
	RIVP_LDD_U8					*strmRingBuff;								/* ring buffer pointer */
	/* for performance */
	RIVP_APP_LDD_PERFORMANCE_T	 perf;
	/* for task */
	RIVP_LDD_BOOL				 decoding;
	RIVP_APP_LDD_OSAL_TASK_HND_T taskHnd[RIVP_APP_LDD_MAX_TASK];
	/* for message queue */
	RIVP_APP_LDD_OSAL_QUE_INF_T	 msgInf[RIVP_APP_LDD_MAX_MSG_QUEUE];
	/* for YUV data */
	RIVP_LDD_U8					*yuvBuff;									/* YUV buffer */
	RIVP_LDD_U32				 yuvBuffCnt;								/* YUV buffer index */
	RIVP_LDD_U32				 streamFrameId;								/* stream frame ID */
} RIVP_APP_LDD_CMN_PARAM_T;

extern const RIVP_LDD_S8 g_rivpAppLddCmnBool[2][6];
extern const RIVP_LDD_S8 g_rivpAppLddCmnResCode[6][8];
extern const RIVP_LDD_S8 g_rivpAppLddCmnIntCnt[3][5];
extern const RIVP_LDD_S8 g_rivpAppLddCmnCodec[3][8];

/******************************************************************************/
/*                    FUNCTIONS                                               */
/******************************************************************************/
extern RIVP_LDD_BOOL RIVP_AppLddCmnSetDefaultXy( RIVP_APP_LDD_CMN_PARAM_T *app );
extern void			 RIVP_AppLddCmnOutputData( RIVP_APP_LDD_CMN_PARAM_T *app, RIVP_APP_LDD_CMN_FILE_T *file );
extern RIVP_LDD_U8	*RIVP_AppLddCmnSearchStartCode( RIVP_LDD_U8 *buffAddr0, RIVP_LDD_U32 buffSize0, RIVP_LDD_U8 *buffAddr1, RIVP_LDD_U32 buffSize1, RIVP_LDD_U32 startCodeWord, RIVP_LDD_U32 startCodeMask, RIVP_LDD_U32 startCodeBits, RIVP_LDD_U32 checkOverlap );
extern void			 RIVP_AppLddCmnUpdateInputParam( RIVP_APP_LDD_CMN_PARAM_T *app, RIVP_APP_LDD_CMN_FILE_T *file );
extern RIVP_LDD_BOOL RIVP_AppLddCmnMemAlloc( RIVP_APP_LDD_CMN_PARAM_T **app, RIVP_APP_LDD_CMN_FILE_T **file, RIVP_LDD_U32 **codecConfig );
extern void			 RIVP_AppLddCmnMemFree( RIVP_APP_LDD_CMN_PARAM_T *app, RIVP_APP_LDD_CMN_FILE_T *file, RIVP_LDD_U32 *codecConfig );
extern RIVP_LDD_BOOL RIVP_AppLddCmnMemAllocOsal( RIVP_APP_LDD_CMN_PARAM_T *app, RIVP_APP_LDD_CMN_FILE_T *file );
extern void			 RIVP_AppLddCmnMemFreeOsal( RIVP_APP_LDD_CMN_PARAM_T *app );
extern void			 RIVP_AppLddCmnGetCurTime( RIVP_APP_LDD_TIME_INFO_T *timeInfo );
extern void			 RIVP_AppLddCmnGetProcTime( RIVP_APP_LDD_CMN_PARAM_T *app, RIVP_APP_LDD_FUNC_KIND kind );
extern void			 RIVP_AppLddCmnPerformanceResult( RIVP_APP_LDD_CMN_PARAM_T *app );
extern RIVP_LDD_BOOL RIVP_AppLddCmnCreateResource( RIVP_APP_LDD_CMN_PARAM_T *app );
extern RIVP_LDD_BOOL RIVP_AppLddCmnDestroyResource( RIVP_APP_LDD_CMN_PARAM_T *app );
extern RIVP_LDD_BOOL RIVP_AppLddCmnRestartTask( RIVP_APP_LDD_CMN_PARAM_T *app );

#ifdef __cplusplus
}
#endif

#endif /* RIVP_APP_LDD_CMN_LOCAL_H */
