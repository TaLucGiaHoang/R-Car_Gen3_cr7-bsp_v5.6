/******************************************************************************
 *
 * PURPOSE
 *   RIVP OS Wrapper local Defines
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 ******************************************************************************/
/******************************************************************************
* OS Wrapper local Defines
* Copyright (C) 2015 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/
#ifndef RIVP_APP_LDD_OSDEP_H
#define RIVP_APP_LDD_OSDEP_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
#include "rivp_ldd_types.h"
#include "rcar-xos/osal/r_osal.h"

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
#define RIVP_APP_LDD_OSAL_TASK_NAME_LENGTH	(32u)	/* max length of task name */
#define RIVP_APP_LDD_OSAL_TASK_STACK_SIZE	(8000u)	/* stack of each task */
#define RIVP_APP_LDD_OSAL_TASK_NAME_SIZE	(12u)	/* length of task name */
#define RIVP_APP_LDD_OSAL_TASK_NAME_DEC		"rivpTaskDec"	/* name of decode task */
#define RIVP_APP_LDD_OSAL_TASK_NAME_OUT		"rivpTaskOut"	/* name of output task */
#define RIVP_APP_LDD_OSAL_TASK_NAME_RTP		"rivpTaskRtp"
#define RIVP_APP_LDD_OSAL_DEC_TASK_ID		(0u)
#define RIVP_APP_LDD_OSAL_OUT_TASK_ID		(1u)
#define RIVP_APP_LDD_OSAL_RTP_TASK_ID		(2u)
#define RIVP_APP_LDD_OSAL_TIME_RESO			(4295u)
#define RIVP_APP_LDD_OSAL_SEM_NUM			(7u)
#define RIVP_APP_LDD_OSAL_MSG_SIZE_MAX		(256u)
#define RIVP_APP_LDD_OSAL_MSG_NUM_MAX		(128u)
#define RIVP_APP_LDD_OSAL_QUE_NUM_MAX		(5u)

/* OSAL resource ID */
#define RIVP_APP_LDD_OSAL_MUTEX_ID_00 		(0x9800U)		/* OSAL_MUTEX_LDD_00 */
#define RIVP_APP_LDD_OSAL_MUTEX_ID_01 		(0x9801U)		/* OSAL_MUTEX_LDD_01 */
#define RIVP_APP_LDD_OSAL_MUTEX_ID_02 		(0x9802U)		/* OSAL_MUTEX_LDD_02 */
#define RIVP_APP_LDD_OSAL_MUTEX_ID_03 		(0x9803U)		/* OSAL_MUTEX_LDD_03 */
#define RIVP_APP_LDD_OSAL_MUTEX_ID_04 		(0x9804U)		/* OSAL_MUTEX_LDD_04 */
#define RIVP_APP_LDD_OSAL_MUTEX_ID_05 		(0x9805U)		/* OSAL_MUTEX_LDD_05 */
#define RIVP_APP_LDD_OSAL_MUTEX_ID_06		(0x9806U)		/* OSAL_MUTEX_LDD_06 */
#define RIVP_APP_LDD_OSAL_MUTEX_ID_07		(0x9807U)		/* OSAL_MUTEX_LDD_07 */
#define RIVP_APP_LDD_OSAL_LIB_MUTEX_ID_00 	(0x9808U)		/* OSAL_MUTEX_LDD_06 : rivp api */
#define RIVP_APP_LDD_OSAL_LIB_MUTEX_ID_01 	(0x9809U)		/* OSAL_MUTEX_LDD_07 : mng_vdp */

#define RIVP_APP_LDD_OSAL_MQ_MAIN_ID		(0x9800U)	/* {16, 64} */	/* OSAL_MQ_LDD_00 */
#define RIVP_APP_LDD_OSAL_MQ_DEC_ID			(0x9801U)	/* {16, 64} */	/* OSAL_MQ_LDD_01 */
#define RIVP_APP_LDD_OSAL_MQ_SRM_ID			(0x9802U)	/* {16, 64} */	/* OSAL_MQ_LDD_02 */
#define RIVP_APP_LDD_OSAL_MQ_PIC_ID			(0x9803U)	/* {16,196} */	/* OSAL_MQ_LDD_03 */
#define RIVP_APP_LDD_OSAL_LIB_MQ_ID_00		(0x9804U)	/* { 4, 16} */
#define RIVP_APP_LDD_OSAL_LIB_MQ_ID_01		(0x9805U)	/* { 1,  4} */
#define RIVP_APP_LDD_OSAL_MQ_RTP_ID			(0x9806U)	/* {100, 64} */
#define RIVP_APP_LDD_OSAL_MQ_RTPF_ID		(0x9807U)	/* {100, 4} */

#define RIVP_APP_LDD_OSAL_LIB_THREAD_ID		(0x9800U)
#define RIVP_APP_LDD_OSAL_THREAD_DEC_ID		(0x9801U)		/* OSAL_THREAD_LDD_DECTASK */
#define RIVP_APP_LDD_OSAL_THREAD_OUT_ID		(0x9802U)		/* OSAL_THREAD_LDD_OUTTASK */
#define RIVP_APP_LDD_OSAL_THREAD_RTP_ID		(0x9803U)		/* OSAL_THREAD_LDD_RTPTASK */

#define RIVP_APP_LDD_OSAL_MUTEX_TIMEOUT		(10000u)
#define RIVP_APP_LDD_OSAL_MQ_TIMEOUT		(10000u)

/* set alias of RIVP_LDD_PTR type */
typedef void (*RIVP_APP_LDD_OSAL_ENTRY)(RIVP_LDD_PTR);

#define RIVP_APP_LDD_OSAL_INVALID_ADDRESS 	(0xFFFFFFFFu)

/******************************************************************************/
/*                    TYPE DEFINITION                                         */
/******************************************************************************/
typedef struct {
	RIVP_APP_LDD_OSAL_ENTRY		 entry;
	RIVP_LDD_PTR				 param;
	RIVP_LDD_S32				 priority;
	RIVP_LDD_S8					 name[RIVP_APP_LDD_OSAL_TASK_NAME_LENGTH];
	RIVP_LDD_PTR				 id;
	osal_thread_id_t 			 osalThreadId; 	/* in:OSAL Thread ID */
} RIVP_APP_LDD_OSAL_TASK_HND_T;

typedef struct {
	RIVP_LDD_U32				 num;			/* in:number of queue */
	RIVP_LDD_U32				 size;			/* in:size of message */
	RIVP_LDD_PTR				 id;			/* out:handle.use rivp_app_ldd_osdep.c only */
	osal_mq_id_t 				 osalMqId;		/* in:OSAL Message Queue ID */
	/* Queue Ctrl */
	RIVP_LDD_PTR				 sem;			/* semaphore */
	RIVP_LDD_U32				 msgCnt;		/* message counter */
} RIVP_APP_LDD_OSAL_QUE_INF_T;

/******************************************************************************/
/*                    FUNCTIONS                                               */
/******************************************************************************/
/* init/deinit */
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalInit(void);
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalDeinit(void);

/* Timer */
extern void				 RIVP_AppLddOsalGetCurTime(RIVP_LDD_U32 *sec, RIVP_LDD_U32 *nsec);

/* Semaphore */
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalSemInit(void);
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalSemDeinit(void);
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalSemCreate(RIVP_LDD_PTR *ptr);
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalSemDestroy(const RIVP_LDD_PTR ptr);
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalSemLock(const RIVP_LDD_PTR ptr );
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalSemUnlock(const RIVP_LDD_PTR ptr);

/* Task */
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalTaskCreate(RIVP_APP_LDD_OSAL_TASK_HND_T *tskHnd);
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalTaskDestroy(RIVP_APP_LDD_OSAL_TASK_HND_T *tskHnd);
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalTaskRun(RIVP_APP_LDD_OSAL_TASK_HND_T *tskHnd);
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalTaskHalt(RIVP_APP_LDD_OSAL_TASK_HND_T *tskHnd);
extern void				*RIVP_AppLddOsalTaskReady(RIVP_LDD_PTR tskHnd);

/* Message */
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalQueueCreate(RIVP_APP_LDD_OSAL_QUE_INF_T *msgInf);
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalQueueDestroy(RIVP_APP_LDD_OSAL_QUE_INF_T *msgInf);
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalQueueSend(RIVP_APP_LDD_OSAL_QUE_INF_T *msgInf, const RIVP_LDD_PTR msg);
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalQueueReceive(RIVP_APP_LDD_OSAL_QUE_INF_T *msgInf, RIVP_LDD_PTR msg);
extern RIVP_LDD_BOOL	 RIVP_AppLddOsalQueueGetMsgCnt(RIVP_APP_LDD_OSAL_QUE_INF_T *msgInf, RIVP_LDD_U32 *count);

/* Sleep */
extern void				 RIVP_AppLddOsalUSleep(RIVP_LDD_U32 msTime);

/* Other */
extern void				 RIVP_AppLddOsalMemCpy(RIVP_LDD_PTR dst, const RIVP_LDD_PTR src, RIVP_LDD_U32 size);

#ifdef __cplusplus
}
#endif

#endif /* RIVP_APP_LDD_OSDEP_H */
