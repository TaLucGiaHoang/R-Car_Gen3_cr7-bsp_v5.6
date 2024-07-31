/******************************************************************************
 *
 * PURPOSE
 *   RIVP API Public Defines
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

#ifndef RIVP_LDD_API_H
#define RIVP_LDD_API_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
#include "rivp_ldd_types.h"
/* OSAL API includes */
#include "rcar-xos/osal/r_osal.h"

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
/* RIVP_LDD_INITPARAM_T */
#define RIVP_LDD_MAX_NUM_CONTEXT		(4u)

/* RIVP_LDD_CONTEXTPARAM_T */
#define RIVP_LDD_MAX_NUM_BUFFERING_PIC	(3u)
#define RIVP_LDD_AVC_CODEC_TYPE			(1u)

/* RIVP_LDD_INPUTPARAM_T */
#define RIVP_LDD_MAX_STRM_BUFF			(2u)

/* RIVP_LDD_PICPARAM_T */
#define RIVP_LDD_PIC_ERROR_HDR			(0x00000001u)
#define RIVP_LDD_PIC_ERROR_PIC			(0x00000002u)
#define RIVP_LDD_PIC_ERROR_TOPFLD_DROP	(0x00000010u)
#define RIVP_LDD_PIC_ERROR_BOTFLD_DROP	(0x00000020u)
#define RIVP_LDD_PIC_ERROR_MBL_INT		(0x80000000u)

/* RIVP_LDD_AVC_CONFIG_T */
#define RIVP_LDD_AVC_DPB_NUM_STANDARD	(0u)
#define RIVP_LDD_AVC_DPB_NUM_MIN		(1u)
#define RIVP_LDD_AVC_DPB_NUM_MAX		(16u)
#define RIVP_LDD_AVC_DPB_NUM_INTRA_ONLY	(0xFFFFFFFFu)

/**
 * errorInfo for RIVP_LDD_RTN_WARNING
 */
/* Invalid stream */
#define RIVP_LDD_ERR_WNG_INVALID_STRM			(0x00000001u)
/* No picture buffer */
#define RIVP_LDD_ERR_WNG_NO_PIC_BUFF			(0x00000002u)
/* Hardware is still working */
#define RIVP_LDD_ERR_WNG_HW_WORKING				(0x00000004u)
/* Sequence End is detected */
#define RIVP_LDD_ERR_WNG_SEQ_END				(0x00000008u)

/**
 * errorInfo for RIVP_LDD_RTN_ERROR_PARAM
 */
/* Pointer argument is NULL */
#define RIVP_LDD_ERR_PARAM_NULL					(0x00000001u)
/* ch in RIVP_LDD_CONTEXTPARAM_T is out-of-range */
#define RIVP_LDD_ERR_PARAM_CH					(0x00000002u)
/* maxWidth in RIVP_LDD_CONTEXTPARAM_T is out-of-range */
#define RIVP_LDD_ERR_PARAM_MAX_WIDTH			(0x00000004u)
/* maxHeight in RIVP_LDD_CONTEXTPARAM_T is out-of-range */
#define RIVP_LDD_ERR_PARAM_MAX_HEIGHT			(0x00000008u)
/* fmemPhysAddr in RIVP_LDD_CONTEXTPARAM_T is invalid */
#define RIVP_LDD_ERR_PARAM_FMEM_ADDR			(0x00000010u)
/* fmemSize in RIVP_LDD_CONTEXTPARAM_T is out-of-range */
#define RIVP_LDD_ERR_PARAM_FMEM_SIZE			(0x00000020u)
/* numBufferingPic in RIVP_LDD_CONTEXTPARAM_T is out-of-range */
#define RIVP_LDD_ERR_PARAM_NUM_BUF_PIC			(0x00000040u)
/* cbPicOut in RIVP_LDD_CONTEXTPARAM_T is NULL */
#define RIVP_LDD_ERR_PARAM_PICOUT_NULL			(0x00000080u)
/* cbStrmFree in RIVP_LDD_CONTEXTPARAM_T is NULL */
#define RIVP_LDD_ERR_PARAM_STRMFREE_NULL		(0x00000100u)
/* codecEntryPoint in RIVP_LDD_CONFIG_T is NULL */
#define RIVP_LDD_ERR_PARAM_CFG_ENTRY			(0x00000200u)
/* codecConfig in RIVP_LDD_CONFIG_T is NULL */
#define RIVP_LDD_ERR_PARAM_CFG_CONFIG_NULL		(0x00000400u)
/* codecConfigSize in RIVP_LDD_CONFIG_T is 0 */
#define RIVP_LDD_ERR_PARAM_CFG_SIZE				(0x00000800u)
/* support422Format in RIVP_LDD_CONFIG_T is out-of-range or invalid */
#define RIVP_LDD_ERR_PARAM_CFG_422_FMT			(0x00001000u)
/* support12bitDepth in RIVP_LDD_CONFIG_T is out-of-range or invalid */
#define RIVP_LDD_ERR_PARAM_CFG_12BIT_DEP		(0x00002000u)
/* intCnt in RIVP_LDD_CONFIG_T is out-of-range or invalid */
#define RIVP_LDD_ERR_PARAM_CFG_INT_CNT			(0x00004000u)
/* outputLinearFormat in RIVP_LDD_AVC_CONFIG_T is out-of-range or invalid */
#define RIVP_LDD_ERR_PARAM_AVC_OUT_LN_FMT		(0x00008000u)
/* normalDecode in RIVP_LDD_AVC_CONFIG_T is out-of-range */
#define RIVP_LDD_ERR_PARAM_AVC_NORMAL_DEC		(0x00010000u)
/* reorderEnable in RIVP_LDD_AVC_CONFIG_T is out-of-range */
#define RIVP_LDD_ERR_PARAM_AVC_REORDER			(0x00020000u)
/* fixedPicSize in RIVP_LDD_AVC_CONFIG_T is out-of-range */
#define RIVP_LDD_ERR_PARAM_AVC_FIX_PIC			(0x00040000u)
/* restrictedDpbNum in RIVP_LDD_AVC_CONFIG_T is out-of-range or invalid */
#define RIVP_LDD_ERR_PARAM_AVC_DPB_NUM			(0x00080000u)
/* maxLevel in RIVP_LDD_AVC_CONFIG_T is out-of-range */
#define RIVP_LDD_ERR_PARAM_AVC_MAX_LEVEL		(0x00100000u)
/* strmId in RIVP_LDD_INPUTPARAM_T is 0 */
#define RIVP_LDD_ERR_PARAM_STRM_ID				(0x00200000u)
/* strmBuffNum in RIVP_LDD_INPUTPARAM_T is out-of-range */
#define RIVP_LDD_ERR_PARAM_STRM_BUF_NUM			(0x00400000u)
/* strmBuff in RIVP_LDD_INPUTPARAM_T has invalid setting */
#define RIVP_LDD_ERR_PARAM_STRMBUF				(0x00800000u)
/* The number of context exceeds numContext in RIVP_LDD_INITPARAM_T */
#define RIVP_LDD_ERR_PARAM_NUM_CTX				(0x01000000u)
/* Context of the designated ch in RIVP_LDD_CONTEXTPARAM_T already exists */
#define RIVP_LDD_ERR_PARAM_CTX_EXIST			(0x02000000u)
/*!< virtAddr in RIVP_LDD_WORKBUFF_T is NULL */
#define RIVP_LDD_ERR_PARAM_WORKBUF_NULL			(0x04000000u)
/*!< physAddr in RIVP_LDD_WORKBUFF_T is invalid */
#define RIVP_LDD_ERR_PARAM_WORKBUF_ADDR			(0x08000000u)
/*!< size in RIVP_LDD_WORKBUFF_T is out-of-range */
#define RIVP_LDD_ERR_PARAM_WORKBUF_SIZE			(0x10000000u)
/**
 * errorInfo for RIVP_LDD_RTN_FATAL_CONTEXT 
 */
/* Context state is invalid */
#define RIVP_LDD_ERR_CTX_STATE					(0x00000001u)
/* Failed to input process */
#define RIVP_LDD_ERR_CTX_FAIL_INPUT				(0x00000002u)
/* Failed to release process */
#define RIVP_LDD_ERR_CTX_FAIL_RELEASE			(0x00000004u)
/* Abnormal error in decode */
#define RIVP_LDD_ERR_CTX_DECODE_ERROR			(0x00000008u)

/**
 * errorInfo for RIVP_LDD_RTN_FATAL_LIBRARY
 */
/* Library state is uninitialized */
#define RIVP_LDD_ERR_LIB_UNINIT					(0x00000001u)
/* Library state is already library error */
#define RIVP_LDD_ERR_LIB_STATE					(0x00000002u)
/* Failure is detected in this library while RIVP_LddGetContext */
#define RIVP_LDD_ERR_LIB_FAIL_START_DEC			(0x00000004u)
/* Failure is detected in this library while RIVP_LddFreeContext */
#define RIVP_LDD_ERR_LIB_FAIL_STOP_DEC			(0x00000008u)
/* Hardware hung up in decoding */
#define RIVP_LDD_ERR_LIB_HW_HUNG_UP				(0x00000010u)
/* Failed to calculate memory size */
#define RIVP_LDD_ERR_LIB_FAIL_CALC_MEM_SIZE		(0x00000020u)

/**
 * errorInfo for RIVP_LDD_RTN_FATAL_SYSTEM
 */
/* Library state is already system error */
#define RIVP_LDD_ERR_SYS_STATE					(0x00000001u)
/* OSAL function for semaphore returns failure or semaphore is invalid */
#define RIVP_LDD_ERR_SYS_OSAL_SEM				(0x00000002u)
/* Failed to start up Hardware */
#define RIVP_LDD_ERR_SYS_CTRL_HW				(0x00000004u)
/* Unspecified stack corruption occurred */
#define RIVP_LDD_ERR_SYS_STACK_CORRUPTION		(0x00000008u)

/******************************************************************************/
/*                    TYPE DEFINITION                                         */
/******************************************************************************/
/* enumerated type of VDP common */
typedef enum {
	RIVP_LDD_FORMAT_8BIT_420_TILING		= 0x0000,
	RIVP_LDD_FORMAT_8BIT_420_LINEAR		= 0x0001,
	RIVP_LDD_FORMAT_8BIT_422_TILING		= 0x0010,
	RIVP_LDD_FORMAT_8BIT_422_LINEAR		= 0x0011,
	RIVP_LDD_FORMAT_10BIT_420_TILING	= 0x0100,
	RIVP_LDD_FORMAT_10BIT_420_LINEAR	= 0x0101,
	RIVP_LDD_FORMAT_10BIT_422_TILING	= 0x0110,
	RIVP_LDD_FORMAT_10BIT_422_LINEAR	= 0x0111,
	RIVP_LDD_FORMAT_12BIT_420_TILING	= 0x0200,
	RIVP_LDD_FORMAT_12BIT_420_LINEAR	= 0x0201,
	RIVP_LDD_FORMAT_12BIT_422_TILING	= 0x0210,
	RIVP_LDD_FORMAT_12BIT_422_LINEAR	= 0x0211
} RIVP_LDD_FRAME_PLACEMENT_FORMAT;

typedef enum {
	RIVP_LDD_MBLINE_INT_NONE			= 0,
	RIVP_LDD_MBLINE_INT_ONE_HALF,
	RIVP_LDD_MBLINE_INT_ONE_THIRD
} RIVP_LDD_MBLINE_INT_CNT;

typedef enum {
	RIVP_LDD_PIC_COMPONENT_Y			= 0,
	RIVP_LDD_PIC_COMPONENT_C,
	RIVP_LDD_PIC_COMPONENT_MAX
} RIVP_LDD_PIC_COMPONENT;

typedef enum {
	RIVP_LDD_PIC_STRUCT_FRAME			= 0,
	RIVP_LDD_PIC_STRUCT_FIELD
} RIVP_LDD_PIC_STRUCT;

typedef enum {
	RIVP_LDD_PIC_FIELD_TOP				= 0,
	RIVP_LDD_PIC_FIELD_BOT,
	RIVP_LDD_PIC_FIELD_MAX
} RIVP_LDD_PIC_FIELD;

typedef enum {
	RIVP_LDD_STRM_FIELD_FIRST			= 0,
	RIVP_LDD_STRM_FIELD_SECOND,
	RIVP_LDD_STRM_FIELD_MAX
} RIVP_LDD_STRM_FIELD;

/* enumerated type of AVC */
typedef enum {
	RIVP_LDD_AVC_PIC_TYPE_I				= 0,
	RIVP_LDD_AVC_PIC_TYPE_P				= 1,
	RIVP_LDD_AVC_PIC_TYPE_B				= 2
} RIVP_LDD_AVC_PIC_TYPE;

typedef enum {
	RIVP_LDD_AVC_MAX_LEVEL_10			= 10,
	RIVP_LDD_AVC_MAX_LEVEL_11			= 11,
	RIVP_LDD_AVC_MAX_LEVEL_12			= 12,
	RIVP_LDD_AVC_MAX_LEVEL_13			= 13,
	RIVP_LDD_AVC_MAX_LEVEL_20			= 20,
	RIVP_LDD_AVC_MAX_LEVEL_21			= 21,
	RIVP_LDD_AVC_MAX_LEVEL_22			= 22,
	RIVP_LDD_AVC_MAX_LEVEL_30			= 30,
	RIVP_LDD_AVC_MAX_LEVEL_31			= 31,
	RIVP_LDD_AVC_MAX_LEVEL_32			= 32,
	RIVP_LDD_AVC_MAX_LEVEL_40			= 40,
	RIVP_LDD_AVC_MAX_LEVEL_41			= 41,
	RIVP_LDD_AVC_MAX_LEVEL_42			= 42,
	RIVP_LDD_AVC_MAX_LEVEL_50			= 50,
	RIVP_LDD_AVC_MAX_LEVEL_51			= 51
} RIVP_LDD_AVC_MAX_LEVEL;

/*******************************************************************************************************************//**
 * This structure is the configuration for OSAL.
***********************************************************************************************************************/
typedef struct {
	e_osal_pm_policy_t				pmPolicy;			/*!< OSAL power management policy */
	e_osal_interrupt_priority_t		priorityValueIrq0;	/*!< Irq0 priority value */
	e_osal_interrupt_priority_t		priorityValueIrq1;	/*!< Irq1 priority value */
	e_osal_interrupt_priority_t		priorityValueIrq2;	/*!< Irq2 priority value */
	osal_mutex_id_t					mutexId0;			/*!< Mutex id for library */
	osal_mutex_id_t					mutexId1;			/*!< Mutex id for manager */
	osal_milli_sec_t				timePeriod;			/*!< Timeout period for mutex */
	osal_thread_id_t				threadId;			/*!< Thread id for library */
	osal_mq_id_t					mqId0;				/*!< MessageQueue id for library */
	osal_mq_id_t					mqId1;				/*!< MessageQueue id for library */
	osal_milli_sec_t				mqTimePeriod;		/*!< Timeout period for message queue */
} RIVP_LDD_OSAL_CONFIG_T;

/* structure of RIVP-AVC */
typedef struct {
	RIVP_LDD_BOOL					 outputLinearFormat;
	/* \attention RIVP_LDD_FALSE is not support on D1H. */
	RIVP_LDD_BOOL					 normalDecode;
	/**
	 * \attention if intCnt in RIVP_LDD_CONTEXTPARAM_T is
	 * not _INT_NONE, this must set RIVP_LDD_FALSE.
	 */
	RIVP_LDD_BOOL					 reorderEnable;
	RIVP_LDD_BOOL					 fixedPicSize;
	/**
	 * \attention 0 is standard value, 1-16 uses small one between
	 * a standard and specified value.
	 */
	RIVP_LDD_U32					 restrictedDpbNum;
	RIVP_LDD_AVC_MAX_LEVEL			 maxLevel;
} RIVP_LDD_AVC_CONFIG_T;

typedef struct {
	RIVP_LDD_AVC_PIC_TYPE			 picType;
	RIVP_LDD_U32					 cropLeft;
	RIVP_LDD_U32					 cropRight;
	RIVP_LDD_U32					 cropTop;
	RIVP_LDD_U32					 cropBottom;
	RIVP_LDD_U32					 vuiVideoFullRangeFlag;
	RIVP_LDD_U32					 reserved;
} RIVP_LDD_AVC_PICPARAM_T;


/* structure of RIVP common */
typedef struct {
	RIVP_LDD_U32					 major;
	RIVP_LDD_U32					 minor;
	RIVP_LDD_U32					 patch;
} RIVP_LDD_VERSION_T;

typedef struct {
	RIVP_LDD_U32					 numContext;
} RIVP_LDD_INITPARAM_T;

typedef struct {
	RIVP_LDD_PTR					 virtAddr;
	RIVP_LDD_U32					 physAddr;
	RIVP_LDD_U32					 size;
} RIVP_LDD_STREAMBUFF_T;

typedef struct {
	RIVP_LDD_U32					 strmId;
	RIVP_LDD_U32					 strmBuffNum;
	RIVP_LDD_STREAMBUFF_T			 strmBuff[RIVP_LDD_MAX_STRM_BUFF];
} RIVP_LDD_INPUTPARAM_T;

typedef struct {
	/**
	 * \remarks   The memory size is calculated
	 * by stride and height and format.
	 */
	RIVP_LDD_U32					 yPhysAddr;
	/**
	 * \remarks   The memory size is calculated
	 * by stride and height and format.
	 */
	RIVP_LDD_U32					 cPhysAddr;
	/* \attention : not used now */
	RIVP_LDD_U32					 pmr;
	RIVP_LDD_U32					 reserved;
} RIVP_LDD_PICBUFF_T;

typedef struct {
	RIVP_LDD_PTR					 codecEntryPoint;
	RIVP_LDD_PTR					 codecConfig;
	RIVP_LDD_U32					 codecConfigSize;
	RIVP_LDD_U32					 codecType;
	RIVP_LDD_BOOL					 support422Format;
	RIVP_LDD_BOOL					 support12bitDepth;
	RIVP_LDD_MBLINE_INT_CNT			 intCnt;
	RIVP_LDD_U32					 reserved;
} RIVP_LDD_CONFIG_T;

typedef struct {
	/* context info */
	RIVP_LDD_U32					 ch;
	RIVP_LDD_U32					 codecType;
	RIVP_LDD_U32					 reserved;
	/* picture info */
	RIVP_LDD_U32					 width;
	RIVP_LDD_U32					 height;
	RIVP_LDD_U32					 stride;
	RIVP_LDD_U32					 picIndex;
	RIVP_LDD_U32					 picError;
	RIVP_LDD_FRAME_PLACEMENT_FORMAT	 format[RIVP_LDD_PIC_COMPONENT_MAX];
	RIVP_LDD_U32					 bytePerPixel[RIVP_LDD_PIC_COMPONENT_MAX];
	RIVP_LDD_PIC_STRUCT				 picStruct;
	RIVP_LDD_PICBUFF_T				 picBuff[RIVP_LDD_PIC_FIELD_MAX];
	/* stream info */
	RIVP_LDD_U32					 strmId[RIVP_LDD_STRM_FIELD_MAX];
	RIVP_LDD_U32					 decodedStrmSize[RIVP_LDD_STRM_FIELD_MAX];
	/* additional info */
	RIVP_LDD_AVC_PICPARAM_T			 avcPic;
} RIVP_LDD_PICPARAM_T;

/*******************************************************************************************************************//**
 * This structure is the information of memory area.
***********************************************************************************************************************/
typedef struct {
	RIVP_LDD_PTR					 virtAddr;				/*!< Set the virtual address of memory area */
	RIVP_LDD_U32					 physAddr;				/*!< Set the physical address of memory area */
	RIVP_LDD_U32					 size;					/*!< Set the size of memory area in byte unit */
	RIVP_LDD_PTR					 buffHdl;				/*!< Set the handle of work buffer */
} RIVP_LDD_WORKBUFF_T;

/* callback function */
typedef void (*RIVP_LDD_CB_STRMBUF_FREE)(
	RIVP_LDD_PTR context,
	RIVP_LDD_PTR udptr,
	RIVP_LDD_INPUTPARAM_T *inputParam);

typedef void (*RIVP_LDD_CB_PICTUREOUT)(
	RIVP_LDD_PTR context,
	RIVP_LDD_PTR udptr,
	RIVP_LDD_PICPARAM_T *picParam);

typedef void (*RIVP_LDD_CB_FATAL_ERROR)(
	RIVP_LDD_PTR context,
	RIVP_LDD_PTR udptr,
	RIVP_LDD_RESULT errorCode,
	RIVP_LDD_U32 errorInfo );

typedef struct {
	RIVP_LDD_U32					 ch;
	RIVP_LDD_PTR					 udptr;
	/* setting */
	RIVP_LDD_U32					 maxWidth;
	RIVP_LDD_U32					 maxHeight;
	RIVP_LDD_CONFIG_T				 config;
	/* frame memory */
	/* \attention : not used now */
	RIVP_LDD_U32					 fmemPhysAddr;
	RIVP_LDD_U32					 fmemSize;
	RIVP_LDD_U32					 numBufferingPic;
	/* callback function */
	RIVP_LDD_CB_STRMBUF_FREE		 cbStrmFree;
	RIVP_LDD_CB_PICTUREOUT			 cbPicOut;
	RIVP_LDD_CB_FATAL_ERROR			 cbError;
	RIVP_LDD_WORKBUFF_T				 hwWork;				/*!< Set the work buffer information for hardware */
	RIVP_LDD_U32					 reserved;
} RIVP_LDD_CONTEXTPARAM_T;

/******************************************************************************/
/*                    FUNCTIONS                                               */
/******************************************************************************/
extern RIVP_LDD_RESULT RIVP_LddInitialize(
	RIVP_LDD_INITPARAM_T *initParam,
	const RIVP_LDD_OSAL_CONFIG_T *osCfg);

extern RIVP_LDD_RESULT RIVP_LddGetContext(
	RIVP_LDD_CONTEXTPARAM_T *ctxParam,
	RIVP_LDD_PTR *context);

extern RIVP_LDD_RESULT RIVP_LddInput(
	RIVP_LDD_PTR context,
	RIVP_LDD_INPUTPARAM_T *inputParam);

extern RIVP_LDD_RESULT RIVP_LddReleasePicture(
	RIVP_LDD_PTR context,
	RIVP_LDD_U32 picIndex);

extern RIVP_LDD_RESULT RIVP_LddFreeContext(
	RIVP_LDD_PTR context);

extern RIVP_LDD_RESULT RIVP_LddDeinitialize(
	RIVP_LDD_BOOL forced);

extern const RIVP_LDD_VERSION_T *RIVP_LddGetVersion(void);

extern RIVP_LDD_RESULT RIVP_LddGetMemSize(
	RIVP_LDD_CONTEXTPARAM_T *ctxParam,
	RIVP_LDD_U32 *minFmemSize,
	RIVP_LDD_U32 *workSize);

#ifdef __cplusplus
}
#endif

#endif /* RIVP_LDD_API_H */
