/******************************************************************************
 *
 * PURPOSE
 *   RIVP Log System
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 ******************************************************************************/
/******************************************************************************
* Log System
* Copyright (C) 2015 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
/* RIVP LDD UDF header for LOG */
#include "rivp_dep_ldd_log.h"

/* standard library */
#include <stdio.h>		/* PRQA S 5124 *//* unavoidable in the debug log. */
#include <stdarg.h>		/* PRQA S 5130 *//* unavoidable in the debug log. */
/* OSAL API header */
#include "rcar-xos/osal/r_osal.h"

/******************************************************************************/
/*                    UDF LOG Enable		                                  */
/******************************************************************************/

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
#define RIVP_DEP_LDD_LOG_DMP_END_SIZE			(6u)
#define RIVP_DEP_LDD_MUTEX_LOG_OSAL_ID			(0x980AU)		/* OSAL_MUTEX_LDD_LOG_00 */
#define RIVP_DEP_LDD_MUTEX_DMP_OSAL_ID			(0x980BU)		/* OSAL_MUTEX_LDD_LOG_01 */
#define RIVP_DEP_LDD_MUTEX_TIMEOUT_LOG 			(10000u)
#define RIVP_DEP_LDD_MUTEX_TIMEOUT_DMP 			(10000u)
/******************************************************************************/
/*                    TYPE DEFINITION                                         */
/******************************************************************************/

typedef struct {
	RIVP_LDD_U32	module;
	RIVP_LDD_U32	level;
	RIVP_LDD_U32	output;
} RIVP_DEP_LDD_LOG_SETTING_T;

typedef struct {
	RIVP_LDD_U32		 idx;
	RIVP_LDD_U32		 max;
	RIVP_DEP_LDD_STR	*ptr;
	RIVP_LDD_PTR		 sem;
	RIVP_LDD_BOOL		 setflg;
	RIVP_DEP_LDD_STR	 str[RIVP_LDD_LOG_DMP_ROW_MAX];
} RIVP_DEP_LDD_LOG_DMP_INF_T;

/******************************************************************************/
/*                    VARIABLES                                               */
/******************************************************************************/
RIVP_LDD_STATIC RIVP_DEP_LDD_LOG_SETTING_T s_rivpDepLddLogSetting = { 
	RIVP_LDD_LOG_MOD_NON, 
	RIVP_LDD_LOG_LV_NON, 
	RIVP_LDD_LOG_OUT_NON
};
RIVP_LDD_STATIC  RIVP_DEP_LDD_LOG_DMP_INF_T s_rivpDepLddLogDmpInf  = { 
	0u, 
	0u, 
	RIVP_LDD_NULL, 
	RIVP_LDD_NULL, 
	RIVP_LDD_FALSE, 
	{""}
};

/******************************************************************************/
/*                    FORWARD DECLARATION                                     */
/******************************************************************************/
RIVP_LDD_STATIC void RIVP_DepLddLogDmpPut(
				RIVP_DEP_LDD_STR		 dump[], 
				const RIVP_DEP_LDD_STR	 logData[]);

static osal_mutex_handle_t s_rivpDepLddLogSem = RIVP_LDD_NULL;

/******************************************************************************/
/*                    EXTERNAL FUNCTIONS                                      */
/******************************************************************************/
/**
 * \brief			 Output log data
 *
 * \param[in]		 mod			 : Module flag
 * \param[in]		 lv				 : Log level flag
 * \param[in]		 fmt			 : Log output content
 *
 * \retval			 None			 : None
 *
 * \note			 This function outputs a string of log.
 */
void RIVP_DepLddLog(
		RIVP_LDD_U32			 mod,
		RIVP_LDD_U32			 lv,
		const RIVP_DEP_LDD_STR	*fmt,
		...						/* PRQA S 1337 *//* unavoidable in the debug log. */ 
		)
{
	static const RIVP_DEP_LDD_STR *s_rivpDepLddLogModName[] = {
		 "[TMP]"
		,"[APP]"
		,"[API]"
		,"[VDP]"
		,"[PFM]"
	};

	static const RIVP_DEP_LDD_STR *s_rivpDepLddLogLvName[] = {
		 "[TMP]     :"
		,"[INF]     :"
		,"[WNG]     :"
		,"[ERR][PRM]:"
		,"[ERR][CTX]:"
		,"[ERR][LIB]:"
		,"[ERR][SYS]:"
	};

	static const RIVP_DEP_LDD_STR
		s_rivpDepLddLogDmpEnd[RIVP_DEP_LDD_LOG_DMP_END_SIZE] = {"[END]"};


	RIVP_LDD_U32		 lFlgMod;
	RIVP_LDD_U32		 lFlgLv;
	e_osal_return_t		 lSemRet;
	va_list 			 va;
	RIVP_LDD_S32		 lRetStd;

	/* [RQT a000-1000] sets the initial value. */
	lFlgMod = RIVP_LDD_LOG_TMP_BIT;
	lFlgLv  = RIVP_LDD_LOG_TMP_BIT;
	
	/* [RQT a000-1001] checks the output log setting is none. */
	if (s_rivpDepLddLogSetting.output != RIVP_LDD_LOG_OUT_NON) {
		
		/* [RQT a000-1002] checks the mod is invalid. */
		if (mod >= RIVP_LDD_LOG_MOD_MAX_ID) {
			/* [RQT a000-1003] sets mod = 0(flag of log module for none) */
			mod = 0;
				
		} else {
			/* No action required */
		}
				
		/* [RQT a000-1004] checks the lv is invalid. */
		if (lv >= RIVP_LDD_LOG_LV_MAX_ID) {
			/* [RQT a000-1005] sets lv = 0(flag of log level for none) */
			lv = 0;
			
		} else {
			/* No action required */
		} 
		/* [RQT a000-1006] sets the flag of module and level. */
		lFlgMod <<= mod;
		lFlgLv  <<= lv;
		
		/* [RQT a000-1007] checks the flag of LOG module and level. */
		if (((lFlgMod & s_rivpDepLddLogSetting.module) != RIVP_LDD_LOG_MOD_NON) &&
			((lFlgLv  & s_rivpDepLddLogSetting.level)  != RIVP_LDD_LOG_LV_NON)) {
				
			/* [RQT a000-1008] checks the output log setting. */
			if (s_rivpDepLddLogSetting.output == RIVP_LDD_LOG_OUT_PRT) {
				/* process for printf log */
				/* [RQT a000-1009] check this is the first run. */
				if (s_rivpDepLddLogSem == RIVP_LDD_NULL) {

					/* [RQT a000-1010] create new semaphore */
					/* PRQA S 0310 1 *//* unavoidable in the casts of the structure. */ 
					lSemRet = R_OSAL_ThsyncMutexCreate(RIVP_DEP_LDD_MUTEX_LOG_OSAL_ID,
																&s_rivpDepLddLogSem);
					
					/* [RQT a000-1011] checks the return code is OK.*/
					if (lSemRet != OSAL_RETURN_OK) {
						/* [RQT a000-1012] set NULL ptr. */
						s_rivpDepLddLogSem = RIVP_LDD_NULL;
						
					} else {
						/* No action required */
					}

				} else {
					/* No action required */
				}

				/* [RQT a000-1013] check the creation of semaphore. */
				if (s_rivpDepLddLogSem != RIVP_LDD_NULL) {

					/* [RQT a000-1014] obtain semaphore within the specified time.*/
					/* PRQA S 0316 1 *//* unavoidable in the casts of the structure. */ 
					lSemRet = R_OSAL_ThsyncMutexLockForTimePeriod(s_rivpDepLddLogSem,
																	RIVP_DEP_LDD_MUTEX_TIMEOUT_LOG);
				
					/* [RQT a000-1015] check the return code is OK */
					if (lSemRet == OSAL_RETURN_OK) {
						/* [RQT a000-1016] print header info. */
						(void)printf(
								"%s%s",
								s_rivpDepLddLogModName[mod],
								s_rivpDepLddLogLvName[lv]);	/* ToDo: void cast */
						/* [RQT a000-1017] print contents. */
						va_start(va, fmt);
						(void)vprintf(fmt, va);	/* ToDo: void cast */
						/* PRQA S 2983 1 *//* unavoidable in the debug log. */
						va_end(va);
						lRetStd = fflush(stdout);
						if (lRetStd != 0u) {
							(void)printf("[LOG][ERROR][SYS]:RIVP_DepLddLog fflush failed.\n");	/* ToDo: void cast */
						}

						/* [RQT a000-1018] release semaphore for printf. */
						/* PRQA S 0316 1 *//* unavoidable in the casts of the structure. */ 
						lSemRet = R_OSAL_ThsyncMutexUnlock(s_rivpDepLddLogSem);
						if (lSemRet != OSAL_RETURN_OK) {
							(void)printf("[LOG][ERROR][SYS]:RIVP_DepLddLog mutex unlock failed[1].\n");	/* ToDo: void cast */
						}
					} else {
						/* No action required */
					}
					
				} else {
					/* No action required */
				}
			
			} else if (s_rivpDepLddLogSetting.output == RIVP_LDD_LOG_OUT_DMP) {
				/* process for dump log */
				/* [RQT a000-1019] checks the dump flag is TRUE. */
				if (s_rivpDepLddLogDmpInf.setflg == RIVP_LDD_TRUE) {

					/* [RQT a000-1020] obtain semaphore within the specified time. */
					/* PRQA S 0316 2 *//* unavoidable in the casts of the structure. */ 
					lSemRet = R_OSAL_ThsyncMutexLockForTimePeriod(
						(osal_mutex_handle_t)s_rivpDepLddLogDmpInf.sem, RIVP_DEP_LDD_MUTEX_TIMEOUT_DMP);
					
					/* [RQT a000-1021] checks the return code is OK.*/
					if (lSemRet == OSAL_RETURN_OK) {
						/* [RQT a000-1022] create header info. */
						(void)sprintf(
								s_rivpDepLddLogDmpInf.str,
								"%s%s", s_rivpDepLddLogModName[mod],
								s_rivpDepLddLogLvName[lv]);	/* ToDo: void cast */
						/* [RQT a000-1023] output header info. */
						RIVP_DepLddLogDmpPut(
										s_rivpDepLddLogDmpInf.ptr, 
										s_rivpDepLddLogDmpInf.str);

						/* [RQT a000-1024] create contents. */
						va_start(va, fmt);
						(void)vsprintf(s_rivpDepLddLogDmpInf.str, fmt, va);	/* ToDo: void cast */
						/* PRQA S 2983 1 *//* unavoidable in the debug log. */
						va_end(va);
						/* [RQT a000-1025] output contents. */
						RIVP_DepLddLogDmpPut(
										s_rivpDepLddLogDmpInf.ptr, 
										s_rivpDepLddLogDmpInf.str);

						/* [RQT a000-1026] output terminal code. */
						RIVP_DepLddLogDmpPut(
										s_rivpDepLddLogDmpInf.ptr,
										s_rivpDepLddLogDmpEnd);

						/* [RQT a000-1027] checks the offset of terminal. */
						if (s_rivpDepLddLogDmpInf.idx >= RIVP_DEP_LDD_LOG_DMP_END_SIZE) {
							/* [RQT a000-1028] turning back size of terminal code. */
							s_rivpDepLddLogDmpInf.idx -= RIVP_DEP_LDD_LOG_DMP_END_SIZE;
							
						} else {
							/* [RQT a000-1029] turning back size of terminal code. */
							s_rivpDepLddLogDmpInf.idx =  s_rivpDepLddLogDmpInf.max - 
							(RIVP_DEP_LDD_LOG_DMP_END_SIZE - s_rivpDepLddLogDmpInf.idx);
						}
						/* [RQT a000-1030] release semaphore for dump. */
						/* PRQA S 0316 1 *//* unavoidable in the casts of the structure. */ 
						lSemRet = R_OSAL_ThsyncMutexUnlock((osal_mutex_handle_t)s_rivpDepLddLogDmpInf.sem);
						if (lSemRet != OSAL_RETURN_OK) {
							(void)printf("[LOG][ERROR][SYS]:RIVP_DepLddLog mutex unlock failed[2].\n");	/* ToDo: void cast */
						}
					} else {
						/* No action required */
					}
					
				} else {
					/* No action required */
				}
				
			} else {
				/* No action required */
			}
				
		} else {
			/* No action required */
		}
		
	} else {
		/* No action required */
	}
}


/**
 * \brief			 Ready to dump
 *
 * \param[in]		 ptr			 : Address of the area use to dump
 * \param[in]		 max			 : Size of the area use to dump
 *
 * \retval			 RIVP_LDD_TRUE		 : Success
 * \retval			 RIVP_LDD_FALSE	 	 : failure
 *
 * \note			 This function sets the memory area
 *					 if the log is output to the memory.
 *					 After executing this function,
 *					 RIVP_DepLddLog can output a log to the memory.
 */
RIVP_LDD_BOOL RIVP_DepLddLogDmpStart(
		RIVP_DEP_LDD_STR		*ptr,
		RIVP_LDD_U32			 max
		)
{
	RIVP_LDD_BOOL		 lRet;
	e_osal_return_t		 lSemRet;
	
	/* [RQT a001-1000] sets the initial value to result. */
	lRet = RIVP_LDD_FALSE;

	/* [RQT a001-1001] checks the output log setting is output log to memory. */
	if (s_rivpDepLddLogSetting.output == RIVP_LDD_LOG_OUT_DMP) {
		
		/* [RQT a001-1002] checks the ptr is not NULL. */
		if (ptr != RIVP_LDD_NULL) {
			
			/* [RQT a001-1003] checks the max exceeds the upper limit. */
			if (max >= RIVP_LDD_LOG_DMP_ROW_MAX) {

				/* [RQT a001-1004] create new semaphore. */
				/* PRQA S 0310 2 *//* unavoidable in the casts of the structure. */ 
				lSemRet = R_OSAL_ThsyncMutexCreate(RIVP_DEP_LDD_MUTEX_DMP_OSAL_ID,
			 						(osal_mutex_handle_t *)&s_rivpDepLddLogDmpInf.sem);
				/* [RQT a001-1005] checks return code is OK.*/
				if (lSemRet == OSAL_RETURN_OK) {

					/*[RQT a001-1007] checks the result.*/
					if (s_rivpDepLddLogDmpInf.sem != RIVP_LDD_NULL) {
						/*[RQT a001-1008] sets the result.*/
						lRet = RIVP_LDD_TRUE;
						/* [RQT a001-1009] sets internal data for dump */
						s_rivpDepLddLogDmpInf.idx	 = 0u;			/* offset of terminal   */
						s_rivpDepLddLogDmpInf.max	 = max;			/* size of dump area    */
						s_rivpDepLddLogDmpInf.ptr	 = ptr;			/* pointer of dump area */
						s_rivpDepLddLogDmpInf.setflg = RIVP_LDD_TRUE;	/* setting flag */

					} else {
						/* [RQT a001-1010] clear internal data for dump */
						s_rivpDepLddLogDmpInf.idx	 = 0u;
						s_rivpDepLddLogDmpInf.max	 = 0u;
						s_rivpDepLddLogDmpInf.ptr	 = RIVP_LDD_NULL;
						s_rivpDepLddLogDmpInf.sem	 = RIVP_LDD_NULL;
						s_rivpDepLddLogDmpInf.setflg = RIVP_LDD_FALSE;
					}
					
				} else{
					/* No action required */
				}
				
			} else {
				/* No action required */
			}
			
		} else {
			/* No action required */
		}
		
	} else {
		/* No action required */
	}
	/* [RQT a001-1011] return result. */
	return lRet;
}

/**
 * \brief			 End processing of dump
 *
 * \param			 None				 : None
 *
 * \retval			 None				 : None
 *
 * \note			 This function clear the internal information
 *					 that has been set by RIVP_DepLddLogDmpStart.
 *					 After executing this function,
 *					 RIVP_DepLddLog can NOT output a log to the memory.
 */
void RIVP_DepLddLogDmpStop(void)
{
	e_osal_return_t		 lSemRet;

	/* Release mutex log */
	if (s_rivpDepLddLogSem != RIVP_LDD_NULL) {
		lSemRet = R_OSAL_ThsyncMutexDestroy(s_rivpDepLddLogSem);
		if (lSemRet != OSAL_RETURN_OK) {
			(void)printf("[LOG][ERROR][SYS]:RIVP_DepLddLogDmpStop mutex destroy failed[1].\n");	/* ToDo: void cast */
		}
	}

	/* [RQT a002-1000] checks the output log setting is to memory. */
	if (s_rivpDepLddLogSetting.output == RIVP_LDD_LOG_OUT_DMP) {
		/* [RQT a002-1001] sets false to dump flag. */
		s_rivpDepLddLogDmpInf.setflg = RIVP_LDD_FALSE;

		/* [RQT a002-1002] delete semaphore. */
		/* PRQA S 0316 1 *//* unavoidable in the casts of the structure. */ 
		lSemRet = R_OSAL_ThsyncMutexDestroy((osal_mutex_handle_t)s_rivpDepLddLogDmpInf.sem);
		if (lSemRet != OSAL_RETURN_OK) {
			(void)printf("[LOG][ERROR][SYS]:RIVP_DepLddLogDmpStop mutex destroy failed[2].\n");	/* ToDo: void cast */
		}
		/* [RQT a002-1003] clear internal data for dump. */
		s_rivpDepLddLogDmpInf.idx	= 0u;
		s_rivpDepLddLogDmpInf.max	= 0u;
		s_rivpDepLddLogDmpInf.ptr	= RIVP_LDD_NULL;
		s_rivpDepLddLogDmpInf.sem	= RIVP_LDD_NULL;
		
	} else {
		/* No action required */
	}
}

/**
 * \brief		 Set log setting
 *
 * \param[in]	 module				 : Module flag
 * \param[in]	 level				 : Log level flag
 * \param[in]	 output				 : Log output content
 *
 * \retval		 None				 : None
 *
 * \note		 This function updates log setting.
 */
void RIVP_DepLddLogSetLevel(
		RIVP_LDD_U32			 module,
		RIVP_LDD_U32			 level,
		RIVP_LDD_U32			 output
		)
{
	/* [RQT a003-1000] updates members of RIVP_DEP_LDD_LOG_SETTING_T. */
	s_rivpDepLddLogSetting.module = module;
	s_rivpDepLddLogSetting.level  = level;
	s_rivpDepLddLogSetting.output = output;
}

/******************************************************************************/
/*                    INTERNAL FUNCTIONS                                      */
/******************************************************************************/

/**
 * \brief			 Dump log data
 *
 * \param[in,out]	 dump			 : dump memory address
 * \param[in]		 logData		 : Log data
 *
 * \retval			 None			 : None
 *
 */
RIVP_LDD_STATIC void RIVP_DepLddLogDmpPut(
		RIVP_DEP_LDD_STR		 dump[],
		const RIVP_DEP_LDD_STR	 logData[]
		)
{
	RIVP_LDD_U32 lCnt;
	
	lCnt = 0u;
	/* [RQT a004-1000] checks the size of dump area is not cleared. */
	if ((s_rivpDepLddLogDmpInf.max != 0u) &&
		(dump != RIVP_LDD_NULL) &&
		(logData != RIVP_LDD_NULL)) {
		/* [RQT a004-1001] checks the loop parameter. */
		while ( (lCnt < RIVP_LDD_LOG_DMP_ROW_MAX) &&
				('\0' != (RIVP_DEP_LDD_STR)logData[lCnt])) {
			/* [RQT a004-1002] output log to dump area. */
			dump[s_rivpDepLddLogDmpInf.idx] = logData[lCnt];
			s_rivpDepLddLogDmpInf.idx = (s_rivpDepLddLogDmpInf.idx + 1u)
												% s_rivpDepLddLogDmpInf.max;
			lCnt++;
		}
		
	} else {
		/* No action required */
	}
}

