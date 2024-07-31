/******************************************************************************
 *
 * PURPOSE
 *   RIVP Reference Application OS Wrapper
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 ******************************************************************************/
/******************************************************************************
* OS Wrapper
* Copyright (C) 2015 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
#include <string.h>
#include <stdlib.h>

#include "rivp_dep_ldd_log.h"
#include "rivp_app_ldd_osdep.h"
#include "rivp_app_ldd_mmgr.h"

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
#define RIVP_LDD_ENABLE_PERFORMANCE (1u)	/* 0 : Disable, 1 : Enable */

/******************************************************************************/
/*                    FORWARD DECLARATION                                     */
/******************************************************************************/
typedef struct {
	osal_mutex_id_t		 id;
	RIVP_LDD_PTR			 handle;
} RIVP_APP_LDD_OSAL_SEM_MNG_T;

/******************************************************************************/
/*                         VARIABLE                                           */
/******************************************************************************/
RIVP_LDD_STATIC RIVP_LDD_BOOL					 s_rivpAppLddOsalSemInit	= RIVP_LDD_FALSE;
RIVP_LDD_STATIC RIVP_LDD_PTR					 s_rivpAppLddOsalSemSem;
RIVP_LDD_STATIC RIVP_APP_LDD_OSAL_SEM_MNG_T		 s_rivpAppLddOsalSemData[RIVP_APP_LDD_OSAL_SEM_NUM] = {
	{ RIVP_APP_LDD_OSAL_MUTEX_ID_01,	 RIVP_LDD_NULL },
	{ RIVP_APP_LDD_OSAL_MUTEX_ID_02,	 RIVP_LDD_NULL },
	{ RIVP_APP_LDD_OSAL_MUTEX_ID_03,	 RIVP_LDD_NULL },
	{ RIVP_APP_LDD_OSAL_MUTEX_ID_04,	 RIVP_LDD_NULL },
	{ RIVP_APP_LDD_OSAL_MUTEX_ID_05,	 RIVP_LDD_NULL }};
RIVP_LDD_STATIC RIVP_LDD_U32					 s_rivpAppLddOsalSemUseNum	= 0u;

/******************************************************************************/
/*                    EXTERNAL FUNCTIONS                                      */
/******************************************************************************/
/**
 * \brief		 Initialize Application for OSAL functions
 *
 * \retval		 RIVP_LDD_TRUE				 : Success
 * \retval		 RIVP_LDD_FALSE	 			 : Failure
 *
 * \note		 This function executes the initialization in order to use the OSAL functions.
 *				 Specifically, initialization of work area and creation of a semaphore are executed.
 * 				 Return value indicates whether the processing of this function succeeded or failed.
 */
RIVP_LDD_BOOL RIVP_AppLddOsalInit(void)
{
	RIVP_LDD_BOOL	 lRet;
	RIVP_LDD_BOOL	 lRetSem;
	
	lRet = RIVP_LDD_TRUE;

	if (lRet == RIVP_LDD_TRUE) {
		lRetSem = RIVP_AppLddOsalSemInit();
		if (lRetSem == RIVP_LDD_FALSE) {
			lRet = RIVP_LDD_FALSE;
			RIVP_LDD_LOG_APP_ERR_SYS(
				"RIVP_AppLddOsalInit : RIVP_AppLddOsalSemInit Failure.\n");
		}
	}
	
	if (lRet == RIVP_LDD_TRUE) {
		/* open MMGR handle */
		lRet = RIVP_AppLddMmgrOpen();
		if (lRet == RIVP_LDD_TRUE) {
			/* No action required */
		} else {
			RIVP_LDD_LOG_APP_ERR_SYS(
				"RIVP_AppLddOsalInit : RIVP_AppLddMmgrOpen Failure.\n");
		}
	}

	/* return result */
	return lRet;
}

/**
 * \brief		 Deinitialize Application for OSAL functions
 *
 * \retval		 RIVP_LDD_TRUE				 : Success
 * \retval		 RIVP_LDD_FALSE	 			 : Failure
 *
 * \note		 This function executes the processing to terminate using OSAL functions.
 *				 Specifically, close work area and destruction the semaphore created by RIVP_AppLddOsalInit.
 *				 Return value indicates whether the processing of this function succeeded or failed.
 */
RIVP_LDD_BOOL RIVP_AppLddOsalDeinit(void)
{
	RIVP_LDD_BOOL lRet;

	/* close MMGR driver */
	lRet = RIVP_AppLddMmgrClose();
	if (lRet == RIVP_LDD_TRUE) {
		/* deinitialize internal resources for work area and set result */
		lRet = RIVP_AppLddOsalSemDeinit();
		if (lRet == RIVP_LDD_TRUE) {
			/* No action required */
		} else {
			RIVP_LDD_LOG_APP_ERR_SYS(
				"RIVP_AppLddOsalDeinit : RIVP_AppLddOsalSemDeinit Failure.\n");
		}
	} else {
		RIVP_LDD_LOG_APP_ERR_SYS(
			"RIVP_AppLddOsalDeinit : RIVP_AppLddMmgrClose Failure.\n");
	}

	/* return result */
	return lRet;
}

/**
 * \brief		 Create task
 *
 * \param[in,out] tskHnd				 : Task information
 *
 * \retval		 RIVP_LDD_TRUE			 : Success
 * \retval		 RIVP_LDD_FALSE			 : Failure
 *
 * \note		 This function creates a task.
 *				 Please set the task information to argument "tskHnd".
 *				 Return value indicates whether the processing of this function
 *				 success or failure.
 */
RIVP_LDD_BOOL RIVP_AppLddOsalTaskCreate(RIVP_APP_LDD_OSAL_TASK_HND_T *tskHnd)
{
	RIVP_LDD_BOOL			lRet;
	osal_thread_handle_t 	lTask_Handle;
	e_osal_return_t	 		lOsalRet;
	st_osal_thread_config_t lp_config;

	/* sets the initial value to result. */
	lRet = RIVP_LDD_FALSE;

	/* check the arguments is not NULL. */
	if (tskHnd != RIVP_LDD_NULL) {
		/* create task by calling OSAL API */
		lp_config.func = (p_osal_thread_func_t)tskHnd->entry;
		lp_config.userarg = (void *)tskHnd->param;
		lOsalRet = R_OSAL_ThreadCreate(&lp_config,tskHnd->osalThreadId,&lTask_Handle);

		/* get resource handle by calling QNX API. */
		tskHnd->id = lTask_Handle;
		/* check the create task is success *
		 *   and task ID is not NULL. */
		if ((lOsalRet		== OSAL_RETURN_OK) &&
			(tskHnd->id != RIVP_LDD_NULL)) {
			/* OSAL does not support TaskSuspend function */
			lRet = RIVP_LDD_TRUE;
		} else {
			/* sets NULL to task ID. */
			tskHnd->id = RIVP_LDD_NULL;
		}
	}

	/* return result. */
	return lRet;
}

/**
 * \brief		 Destroy task
 *
 * \param[in,out] tskHnd				 : Task information
 *
 * \retval		 RIVP_LDD_TRUE			 : Success
 * \retval		 RIVP_LDD_FALSE			 : Failure
 *
 * \note		 This function destroys the task created by RIVP_AppLddOsalTaskCreate.
 *				 Please set the task information to argument "tskHnd".
 *				 Return value indicates whether the processing of this function
 *				 success or failure.
 */
RIVP_LDD_BOOL RIVP_AppLddOsalTaskDestroy(RIVP_APP_LDD_OSAL_TASK_HND_T *tskHnd)
{
	e_osal_return_t lOsalRet;
	int64_t		  	lthret;
	RIVP_LDD_BOOL 	lRet;
	
	lRet = RIVP_LDD_FALSE;

	/* Wait for thread to finish */
	lOsalRet = R_OSAL_ThreadJoin((osal_thread_handle_t)tskHnd->id, &lthret);
	
	if (lOsalRet != OSAL_RETURN_OK) {
		RIVP_LDD_LOG_APP_ERR_SYS("R_OSAL_ThreadJoin fail."\
								"osal_ret=%d \n",lOsalRet);
	} else {
		tskHnd->id = RIVP_LDD_NULL;
		lRet = RIVP_LDD_TRUE;
	}
	
	/* return result. */
	return lRet;
}

/**
 * \brief		 Run task
 *
 * \param[in,out] tskHnd				 : task information
 *
 * \retval		 RIVP_LDD_TRUE			 : Success
 * \retval		 RIVP_LDD_FALSE			 : Failure
 *
 * \note		 This function runs the task created by RIVP_AppLddOsalTaskCreate.
 *				 Please set the task information to argument "tskHnd".
 *				 Return value indicates whether the processing of this function
 *				 success or failure.
 */
RIVP_LDD_BOOL RIVP_AppLddOsalTaskRun(RIVP_APP_LDD_OSAL_TASK_HND_T *tskHnd)
{
	RIVP_LDD_BOOL lRet;
	
	lRet = RIVP_AppLddOsalTaskCreate(tskHnd);
	if (lRet != RIVP_LDD_TRUE) {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddOsalTaskCreate failed!!\n");
	}

	/* return result. */
	return lRet;
}

/**
 * \brief		 Create message queue
 *
 * \param[in,out]	 msgInf					: Message queue information
 *
 * \retval		 RIVP_LDD_TRUE				: Success
 * \retval		 RIVP_LDD_FALSE				: Failure
 *
 * \note		 This function creates a message queue.
 *				 Please set the message queue information to argument "msgInf".
 *				 The created message queue is stored on argument "msgInf".
 *				 Return value indicates whether the processing of this function
 *				 success or failure.
 */
RIVP_LDD_BOOL RIVP_AppLddOsalQueueCreate(RIVP_APP_LDD_OSAL_QUE_INF_T *msgInf)
{
	RIVP_LDD_BOOL		 lRet;
	RIVP_LDD_BOOL		 lRetSem;
	e_osal_return_t		 lOsalRet;
	st_osal_mq_config_t	 lMq_config;
	osal_mq_handle_t	 handle;
	
	/* sets the initial value to result. */
	lRet = RIVP_LDD_FALSE;

	/* check arguments */
	if (msgInf != RIVP_LDD_NULL) {
		
		/* create semaphore */
		lRetSem = RIVP_AppLddOsalSemCreate(&msgInf->sem);
		
		if (lRetSem == RIVP_LDD_TRUE) {
			/* create new queue */
			/* set queue information. */
			lMq_config.max_num_msg = msgInf->num;
			lMq_config.msg_size = msgInf->size;
		
			/* create new queue by calling OSAL API. */
			lOsalRet = R_OSAL_MqCreate(&lMq_config, msgInf->osalMqId, &handle);

			msgInf->id = handle;
			if ((lOsalRet	== OSAL_RETURN_OK) &&
				(msgInf->id != RIVP_LDD_NULL)) {
				/* set true to result. */
				msgInf->msgCnt = 0u;
				lRet = RIVP_LDD_TRUE;
			} else {
				/* set NULL to message queue ID. */
				msgInf->id = RIVP_LDD_NULL;
			}
		}
	}

	if (lRet != RIVP_LDD_TRUE) {
		RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddOsalQueueCreate.\n");
	}

	/* return result */
	return lRet;
}

/**
 * \brief		 Destroy message queue
 *
 * \param[in]	 msgInf					 : Message information
 *
 * \retval		 RIVP_LDD_TRUE			 : Success
 * \retval		 RIVP_LDD_FALSE			 : Failure
 *
 * \note		 This function destroys the task created by RIVP_AppLddOsalQueueCreate.
 *				 Please set the message information to argument "msgInf".
 *				 Return value indicates whether the processing of this function
 *				 success or failure.
 */
RIVP_LDD_BOOL RIVP_AppLddOsalQueueDestroy(RIVP_APP_LDD_OSAL_QUE_INF_T *msgInf)
{
	RIVP_LDD_BOOL	 lRet;
	RIVP_LDD_BOOL	 lRetSem;
	e_osal_return_t	 lOsalRet;
	/* set the initial value to result. */
	lRet = RIVP_LDD_FALSE;

	/* check the arguments is not NULL. */
	if (msgInf != RIVP_LDD_NULL) {
		/* create new queue by calling OSAL API. */
		lOsalRet = R_OSAL_MqDelete(msgInf->id);
		/* check the delete queue is success. */
		if (lOsalRet == OSAL_RETURN_OK) {
			/* set true to result. */
			msgInf->id = RIVP_LDD_NULL;
			msgInf->msgCnt = 0u;
			
			lRetSem = RIVP_AppLddOsalSemDestroy(msgInf->sem);
			if (lRetSem == RIVP_LDD_TRUE) {
				msgInf->sem = RIVP_LDD_NULL;
				lRet = RIVP_LDD_TRUE;
			} else {
				RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddOsalQueueDestroy : Failed to close semaphore.\n");
			}
		}
	}
	
	if (lRet != RIVP_LDD_TRUE) {
		RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddOsalQueueDestroy.\n");
	}

	/* return result. */
	return lRet;
}

/**
 * \brief		 Send message
 *
 * \param[in]	 msgInf					 : Message information
 * \param[in]	 msg					 : Message
 *
 * \retval		 RIVP_LDD_TRUE			 : Success
 * \retval		 RIVP_LDD_FALSE			 : Failure
 *
 * \note		 This function sends the message.
 *				 Please set the message information to argument "msgInf".
 *				 Please set the message to argument "msg".
 *				 Return value indicates whether the processing of this function
 *				 success or failure.
 */
RIVP_LDD_BOOL RIVP_AppLddOsalQueueSend(RIVP_APP_LDD_OSAL_QUE_INF_T *msgInf, const RIVP_LDD_PTR msg)
{
	RIVP_LDD_BOOL 	 lRet;
	RIVP_LDD_BOOL	 lRetSem;
	e_osal_return_t	 lOsalRet;	

	/* set the initial value to result. */
	lRet = RIVP_LDD_FALSE;

	/* checks the arguments is not NULL. */
	if ((msgInf		 != RIVP_LDD_NULL) &&
		(msg		 != RIVP_LDD_NULL)) {
		/* send queue by calling OSAL API. */
		lOsalRet = R_OSAL_MqSendForTimePeriod(msgInf->id, RIVP_APP_LDD_OSAL_MQ_TIMEOUT, msg, msgInf->size);
		/* check the send message to queue is success. */
		if (lOsalRet == OSAL_RETURN_OK) {
			/* lock semaphore */
			lRetSem = RIVP_AppLddOsalSemLock(msgInf->sem);
			if (lRetSem == RIVP_LDD_TRUE) {
				/* set true to result. */
				msgInf->msgCnt++;
				lRet = RIVP_LDD_TRUE;

				/* unlock semaphore */
				lRetSem = RIVP_AppLddOsalSemUnlock(msgInf->sem);
				if (lRetSem != RIVP_LDD_TRUE) {
					lRet = RIVP_LDD_FALSE;
				}
			}
		} else {
			RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddOsalQueueSend. lOsalRet: %x\n", lOsalRet);
		}
	}

	if (lRet != RIVP_LDD_TRUE) {
		RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddOsalQueueSend.\n");
	}

	/* return result. */
	return lRet;
}

/**
 * \brief		 Receive message
 *
 * \param[in]	 msgInf					 : Message information
 * \param[out]	 msg					 : Received message
 *
 * \retval		 RIVP_LDD_TRUE			 : Success
 * \retval		 RIVP_LDD_FALSE			 : Failure
 *
 * \note		 This function receives the message.
 *				 Please set the message information to argument "msgInf".
 *				 The received message is stored on argument "msg".
 *				 Return value indicates whether the processing of this function
 *				 success or failure.
 */
RIVP_LDD_BOOL RIVP_AppLddOsalQueueReceive(RIVP_APP_LDD_OSAL_QUE_INF_T *msgInf, RIVP_LDD_PTR msg)
{
	RIVP_LDD_BOOL 	 lRet;
	RIVP_LDD_BOOL	 lRetSem;
	e_osal_return_t	 lOsalRet;	
 
	/* set the initial value to result. */
	lRet = RIVP_LDD_FALSE;

	/* checks the arguments is not NULL. */
	if ((msgInf		 != RIVP_LDD_NULL) &&
		(msg		 != RIVP_LDD_NULL)) {
		/* receive queue by calling OSAL API. */
		lOsalRet = R_OSAL_MqReceiveForTimePeriod(msgInf->id, RIVP_APP_LDD_OSAL_MQ_TIMEOUT, msg, msgInf->size);
		/* check the receive message to queue is success. */
		if (lOsalRet == OSAL_RETURN_OK) {
			/* lock semaphore */
			lRetSem = RIVP_AppLddOsalSemLock(msgInf->sem);
			if (lRetSem == RIVP_LDD_TRUE) {
				/* set true to result. */
				msgInf->msgCnt--;
				lRet = RIVP_LDD_TRUE;
				
				/* unlock semaphore */
				lRetSem = RIVP_AppLddOsalSemUnlock(msgInf->sem);
				if (lRetSem != RIVP_LDD_TRUE) {
					lRet = RIVP_LDD_FALSE;
				}
			}
		} else {
			RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppOsalQueueReceive. lOsalRet: %x\n", lOsalRet);
		}
	}

	if (lRet != RIVP_LDD_TRUE) {
		RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddOsalQueueReceive.\n");
	}

	/* return result. */
	return lRet;
}

/**
 * \brief		 Get message count for message queue
 *
 * \param[in]	 msgInf					 : Message information
 * \param[out]	 count					 : Message count
 *
 * \retval		 RIVP_LDD_TRUE			 : Success
 * \retval		 RIVP_LDD_FALSE			 : Failure
 *
 * \note		 This function gets message count for message queue.
 *				 Please set the message information to argument "msgInf".
 *				 The message count is stored on argument "count".
 *				 Return value indicates whether the processing of this function
 *				 success or failure.
 */
RIVP_LDD_BOOL RIVP_AppLddOsalQueueGetMsgCnt(RIVP_APP_LDD_OSAL_QUE_INF_T *msgInf, RIVP_LDD_U32 *count)
{
	RIVP_LDD_BOOL		 lRet;
	RIVP_LDD_BOOL		 lRetSem;

	/* set initial value */
	lRet = RIVP_LDD_FALSE;

	/* check arguments */
	if ((msgInf != RIVP_LDD_NULL) && (count != RIVP_LDD_NULL)) {
		/* lock semaphore */
		lRetSem = RIVP_AppLddOsalSemLock(msgInf->sem);
		if (lRetSem == RIVP_LDD_TRUE) {
			/* set count of message */
			*count = msgInf->msgCnt;

			/* unlock semaphore */
			lRetSem = RIVP_AppLddOsalSemUnlock(msgInf->sem);
			if (lRetSem == RIVP_LDD_TRUE) {
				lRet = RIVP_LDD_TRUE;
			}
		}
	}

	if (lRet != RIVP_LDD_TRUE) {
		RIVP_LDD_LOG_APP_ERR_SYS("Failed to RIVP_AppLddOsalQueueGetMsgCnt.\n");
	}

	/* return result */
	return lRet;
}

/**
 * \brief		 Get Current Time
 *
 * \param[out]	 sec					 : Sec of Current Time
 * \param[out]	 nsec					 : Nano sec of Current Time
 *
 * \retval		 RIVP_LDD_TRUE			 : Success
 * \retval		 RIVP_LDD_FALSE			 : Fail
 *
 * \note		 This function get the current time from the OS.
 *				 "sec" of the argument is stored the sec of current time.
 *				 "nsec" of the argument is stored the nano sec of current time.
 *				 Return value indicates objectives success or failure of this function.
 */
void RIVP_AppLddOsalGetCurTime(RIVP_LDD_U32 *sec, RIVP_LDD_U32 *nsec)
{
#if RIVP_LDD_ENABLE_PERFORMANCE
#if 1	//TODO osal time
	e_osal_return_t	 lOsalRet;	
	st_osal_time_t	time_stamp;

	if ((sec != RIVP_LDD_NULL) && (nsec != RIVP_LDD_NULL)) {
		lOsalRet = R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &time_stamp);
		if (lOsalRet == OSAL_RETURN_OK) {
			*sec = time_stamp.tv_sec;
			*nsec = time_stamp.tv_nsec;
		} else {
			*sec = 0u;
			*nsec = 0u;
		}
	}
#else
	Time lTime;

	if ((sec != RIVP_LDD_NULL) && (nsec != RIVP_LDD_NULL)) {
		if (GetClockTime(HighestResStandardClock, &lTime) == Success) {
			*sec = (RIVP_LDD_U32)lTime.Seconds;
			*nsec = (lTime.Fraction / RIVP_APP_LDD_OSAL_TIME_RESO) * 1000u;
		} else {
			*sec  = 0u;
			*nsec = 0u;
		}
	}
#endif
#else
	*sec  = 0u;
	*nsec = 0u;
#endif
}

/**
 * \brief		 Initialize for Semaphore functions
 *
 * \retval		 RIVP_LDD_TRUE				 : Success
 * \retval		 RIVP_LDD_FALSE	 			 : Failure
 *
 * \note		 This function executes the initialization in order to use the Semaphore functions.
 *				 Specifically, creation of a semaphore is executed.
 *				 This function should be called only once in the same address space.
 * 				 Return value indicates whether the processing of this function succeeded or failed.
 */
RIVP_LDD_BOOL RIVP_AppLddOsalSemInit(void)
{
	RIVP_LDD_BOOL	 lRet;
	RIVP_LDD_U32	 lCnt;
	e_osal_return_t	 lOsalRet;

	/* set initial value */
	lRet = RIVP_LDD_FALSE;

	/* check initialized flag */
	if (s_rivpAppLddOsalSemInit == RIVP_LDD_FALSE) {
		/* clear semaphore data */
		for (lCnt = 0u; lCnt < RIVP_APP_LDD_OSAL_SEM_NUM; lCnt++) {
			s_rivpAppLddOsalSemData[lCnt].handle = RIVP_LDD_NULL;
		}
		s_rivpAppLddOsalSemUseNum = 0u;

		/* create new semaphore for Semaphore*/
		lOsalRet = R_OSAL_ThsyncMutexCreate(RIVP_APP_LDD_OSAL_MUTEX_ID_00, (osal_mutex_handle_t *)&s_rivpAppLddOsalSemSem);

		if (lOsalRet == OSAL_RETURN_OK) {
			s_rivpAppLddOsalSemInit = RIVP_LDD_TRUE;
			lRet = RIVP_LDD_TRUE;
		} else {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddOsalSemInit sem_create error\n");
		}
	}

	/* return result */
	return lRet;
}

/**
 * \brief		 Deinitialize for Semaphore functions
 *
 * \retval		 RIVP_LDD_TRUE				 : Success
 * \retval		 RIVP_LDD_FALSE	 			 : Failure
 *
 * \note		 This function executes the processing to terminate using Semaphore functions.
 *				 Specifically, destruction the semaphore created by RIVP_AppLddOsalSemInit is executed.
 *				 This function should be called only once in the same address space.
 *				 Return value indicates whether the processing of this function succeeded or failed.
 */
RIVP_LDD_BOOL RIVP_AppLddOsalSemDeinit(void)
{
	RIVP_LDD_BOOL	 lRet;
	RIVP_LDD_BOOL	 lRetSem;
	e_osal_return_t	 lOsalRet;
	RIVP_LDD_U32	 lCnt;

	/* set initial value */
	lRet		 = RIVP_LDD_FALSE;
	lRetSem		 = RIVP_LDD_TRUE;

	/* check initialized flag */
	if (s_rivpAppLddOsalSemInit == RIVP_LDD_TRUE) {
		
		/* close semaphore data */
		for (lCnt = 0u; (lCnt < RIVP_APP_LDD_OSAL_SEM_NUM) && (lRetSem == RIVP_LDD_TRUE); lCnt++) {
			if (s_rivpAppLddOsalSemData[lCnt].handle != RIVP_LDD_NULL) {
				/* close semaphore for semaphore and set error code */
				lRetSem = RIVP_AppLddOsalSemDestroy(s_rivpAppLddOsalSemData[lCnt].handle);
				if (lRetSem == RIVP_LDD_TRUE) {
					s_rivpAppLddOsalSemData[lCnt].handle = RIVP_LDD_NULL;
				}
			}
		}

		/* close semaphore for semaphore */
		lOsalRet = R_OSAL_ThsyncMutexDestroy(s_rivpAppLddOsalSemSem);
		if (lOsalRet == OSAL_RETURN_OK) {
			s_rivpAppLddOsalSemSem  = RIVP_LDD_NULL;
		} else {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddOsalSemDeinit : R_OSAL_ThsyncMutexDestroy error.\n");
		}

		/* check error code */
		if ((lRetSem == RIVP_LDD_TRUE) && (lOsalRet == OSAL_RETURN_OK)) {
			/* update initialized flag and semaphore and set result */
			s_rivpAppLddOsalSemInit = RIVP_LDD_FALSE;
			lRet = RIVP_LDD_TRUE;
		} else {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddOsalSemDeinit : sem_destroy error \n");
		}
	} else {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddOsalSemDeinit : Not initialized!!! \n");
	}

	if (lRet == RIVP_LDD_FALSE) {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddOsalSemDeinit returns RIVP_LDD_FALSE. \n");
	}

	/* return result */
	return lRet;
}

/**
 * \brief		 Create semaphore
 *
 * \param[out]	 ptr					 : Pointer to the created semaphore
 *
 * \retval		 RIVP_LDD_TRUE			 : Success
 * \retval		 RIVP_LDD_FALSE	 		 : Failure
 *
 * \note		 This function creates a binary semaphore.
 *				 The address of the created semaphore is stored on argument "ptr".
 *				 Return value indicates whether the processing of this function succeeded or failed.
 */
RIVP_LDD_BOOL RIVP_AppLddOsalSemCreate(RIVP_LDD_PTR *ptr)
{
	RIVP_LDD_BOOL	 lRet;
	RIVP_LDD_BOOL	 lRetSem;
	RIVP_LDD_U32	 lCnt;
	e_osal_return_t	 lOsalRet;

	/* set initial value */
	lRet	= RIVP_LDD_FALSE;

	/* check arguments */
	if (ptr != RIVP_LDD_NULL) {
		/* check initialized flag */
		if (s_rivpAppLddOsalSemInit == RIVP_LDD_TRUE) {
			/* lock semaphore for semaphore and check result */
			lRetSem = RIVP_AppLddOsalSemLock(s_rivpAppLddOsalSemSem);
			if (lRetSem == RIVP_LDD_TRUE) {
				if (s_rivpAppLddOsalSemUseNum < RIVP_APP_LDD_OSAL_SEM_NUM) {
					for (lCnt = 0u; lCnt < RIVP_APP_LDD_OSAL_SEM_NUM; lCnt++) {
						if (s_rivpAppLddOsalSemData[lCnt].handle == RIVP_LDD_NULL) {
							/* create new semaphore.*/
							lOsalRet = R_OSAL_ThsyncMutexCreate(s_rivpAppLddOsalSemData[lCnt].id, (osal_mutex_handle_t *)&s_rivpAppLddOsalSemData[lCnt].handle);
							/* checks the return value */
							if (lOsalRet == OSAL_RETURN_OK) {
								*ptr = s_rivpAppLddOsalSemData[lCnt].handle;
								if (s_rivpAppLddOsalSemUseNum < RIVP_APP_LDD_OSAL_SEM_NUM) {
									s_rivpAppLddOsalSemUseNum++;
								}
								/* sets the result. */
								lRet = RIVP_LDD_TRUE;
							}
							lCnt = RIVP_APP_LDD_OSAL_SEM_NUM;
						}
					}
				}
				lRetSem = RIVP_AppLddOsalSemUnlock(s_rivpAppLddOsalSemSem);
				if (lRetSem != RIVP_LDD_TRUE) {
					lRet = RIVP_LDD_FALSE;
				}
			}
		}
	}

	if (lRet == RIVP_LDD_FALSE) {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddOsalSemCreate : Failed to create semaphore. \n");
	}

	/* return result */
	return lRet;
}

/**
 * \brief		 Destroy semaphore
 *
 * \param[in]	 ptr					 : Pointer to the semaphore
 *
 * \retval		 RIVP_LDD_TRUE			: Success
 * \retval		 RIVP_LDD_FALSE			: Failure
 *
 * \note		 This function destroys the semaphore that was created by RIVP_AppLddOsalSemCreate.
 *				 Please set the address of the semaphore to be destroyed to argument "ptr".
 */
RIVP_LDD_BOOL RIVP_AppLddOsalSemDestroy(const RIVP_LDD_PTR ptr)
{
	RIVP_LDD_BOOL	 lRet;
	RIVP_LDD_BOOL	 lRetSem;
	RIVP_LDD_U32	 lCnt;
	e_osal_return_t	 lOsalRet;

	/* set initial value */
	lRet	= RIVP_LDD_FALSE;

	/* check arguments is NULL */
	if (ptr != RIVP_LDD_NULL) {
		/* check initialized flag */
		if (s_rivpAppLddOsalSemInit == RIVP_LDD_TRUE) {
			/* lock semaphore for semaphore and check result */
			lRetSem = RIVP_AppLddOsalSemLock(s_rivpAppLddOsalSemSem);
			if (lRetSem == RIVP_LDD_TRUE) {
				for (lCnt = 0u; lCnt < RIVP_APP_LDD_OSAL_SEM_NUM; lCnt++) {
					if (s_rivpAppLddOsalSemData[lCnt].handle == ptr) {
						/* release semaphore handler.*/
						lOsalRet = R_OSAL_ThsyncMutexDestroy(s_rivpAppLddOsalSemData[lCnt].handle);
						/* checks the return code is OK. */
						if (lOsalRet == OSAL_RETURN_OK) {
							/* sets the result. */
							s_rivpAppLddOsalSemData[lCnt].handle = RIVP_LDD_NULL;
							if (s_rivpAppLddOsalSemUseNum > 0u) {
								s_rivpAppLddOsalSemUseNum--;
							}
							lRet = RIVP_LDD_TRUE;
						}
						lCnt = RIVP_APP_LDD_OSAL_SEM_NUM;
					}
				}
				/* unlock semaphore for semaphore and check result */
				lRetSem = RIVP_AppLddOsalSemUnlock(s_rivpAppLddOsalSemSem);
				if (lRetSem != RIVP_LDD_TRUE) {
					lRet = RIVP_LDD_FALSE;
				}
			}
		}
	}

	if (lRet == RIVP_LDD_FALSE) {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddOsalSemDestroy : Failed to destroy semaphore. \n");
	}

	/* return result */
	return lRet;
}

/**
 * \brief		 Lock semaphore
 *
 * \param[in]	 ptr					 : Pointer to the semaphore
 *
 * \retval		 RIVP_LDD_TRUE			 : Success
 * \retval		 RIVP_LDD_FALSE	 		 : Failure
 *
 * \note		 This function locks the semaphore that was created by RIVP_AppLddOsalSemCreate.
 *				 Please set the address of the semaphore to be locked to argument "ptr".
 *				 Return value indicates whether the processing of this function succeeded or failed.
 */
RIVP_LDD_BOOL RIVP_AppLddOsalSemLock(const RIVP_LDD_PTR ptr)
{
	RIVP_LDD_BOOL	 lRet;
	e_osal_return_t	 lOsalRet;

	/* set initial value */
	lRet = RIVP_LDD_FALSE;

	/* check arguments is not NULL */
	if (ptr != RIVP_LDD_NULL) {

		/* take ownership of the semaphore */
		/* if the semaphore is owned by another task, wait for the semaphore to be freed */
		/* call sem_wait and set error code */
		lOsalRet = R_OSAL_ThsyncMutexLockForTimePeriod(ptr, RIVP_APP_LDD_OSAL_MUTEX_TIMEOUT);
		
		/* check the wait semaphore is success. */
		if (lOsalRet == OSAL_RETURN_OK) {
			/* set true to result. */
			lRet = RIVP_LDD_TRUE;
		}
	}

	/* return result */
	return lRet;
}

/**
 * \brief		 Unlock semaphore
 *
 * \param[in]	 ptr					 : Pointer to the semaphore
 *
 * \retval		 RIVP_LDD_TRUE			 : Success
 * \retval		 RIVP_LDD_FALSE	 		 : Failure
 *
 * \note		 This function unlocks the semaphore that was created by RIVP_AppLddOsalSemCreate.
 *				 Please set the address of the semaphore to be unlocked to argument "ptr".
 */
RIVP_LDD_BOOL RIVP_AppLddOsalSemUnlock(const RIVP_LDD_PTR ptr)
{
	RIVP_LDD_BOOL	 lRet;
	e_osal_return_t	 lOsalRet;

	/* set initial value */
	lRet = RIVP_LDD_FALSE;

	/* check arguments */
	if (ptr != RIVP_LDD_NULL) {

		/* release ownership of the semaphore and set error code */
		lOsalRet = R_OSAL_ThsyncMutexUnlock(ptr);

		/* check error code */
		if (lOsalRet == OSAL_RETURN_OK) {
			/* set result */
			lRet = RIVP_LDD_TRUE;
		}
	}

	/* return result */
	return lRet;
}

/**
 * \brief		 Copy memory
 *
 * \param[out]		 dst			: destination address
 * \param[in]		 src			: source address
 * \param[in]		 size			: size of copy
 *
 * \retval			 none			: None
 *
 * \note		 This function copy memory.
 *				 Please set the destination address to argument "dst".
 *				 Please set the source address to argument "src".
 *				 Please set the size to argument "size".
 */
void RIVP_AppLddOsalMemCpy(
		RIVP_LDD_PTR						 dst,
		const RIVP_LDD_PTR					 src,
		RIVP_LDD_U32						 size
		)
{
	/* check the arguments are not NULL. */
	if ((dst != RIVP_LDD_NULL) &&
		(src != RIVP_LDD_NULL)) {
		/* copy memory */
		(void)memcpy(dst, (const void*)src, (size_t)size);
	} else {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddOsalMemCpy Error\n");
	}
}

/**
 * \brief		 Sleep by designation of micro second.
 *
 * \param[in]	 msTime			 : Sleep interval value (micro second)
 *
 * \retval		 none			 : None
 */
void RIVP_AppLddOsalUSleep(RIVP_LDD_U32 usTime)
{
    /* Task(myself) state to suspend (can NOT ditect error) */
	if (usTime >= 1000u) {
		R_OSAL_ThreadSleepForTimePeriod(usTime / 1000u);
	} else {
		R_OSAL_ThreadSleepForTimePeriod(1u);
	}
}

/******************************************************************************/
/*                    INTERNAL FUNCTIONS                                      */
/******************************************************************************/
