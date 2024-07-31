/*******************************************************************************
 *
 * PURPOSE
 *   RIVP Reference Application Memory Manager function
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *
 ******************************************************************************/
/******************************************************************************
* Memory Manager function
* Copyright (C) 2017 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "rivp_ldd_api.h"
#include "rivp_app_ldd_osdep.h"
#ifdef RIVP_DEP_LDD_ENABLE_JPEG
 #include "rivp_app_ldd_data_jpg.h"
#else
 #include "rivp_app_ldd_data_avc.h"
#endif
#include "rivp_app_ldd_cmn_local.h"
#include "rivp_app_ldd_mmgr.h"
#include "rivp_dep_ldd_log.h"

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
#define RIVP_APP_LDD_WORK_MMGR_NUM_MAX		0x20u
#define RIVP_APP_LDD_WORK_MEM_NUM_MAX		0x20u
#define RIVP_APP_LDD_WORKBUF_SIZE			(2097152u)
#define RIVP_APP_LDD_FRAME_MEM_SIZE			(2097152u)
/******************************************************************************/
/*                    TYPE DEFINITION                                         */
/******************************************************************************/
typedef struct RIVP_APP_LDD_MMGR_CTL {
	RIVP_LDD_U32					size;
	RIVP_LDD_PTR					virtAdr;
	RIVP_LDD_U32					physAdr;
	osal_memory_buffer_handle_t		mmgrBufHandle;
} RIVP_APP_LDD_MMGR_CTL_T;

typedef struct RIVP_APP_LDD_MEM_CTL {
	RIVP_LDD_U32	 size;
	RIVP_LDD_PTR	 addr;
} RIVP_APP_LDD_MEM_CTL_T;

/******************************************************************************/
/*                    FORWARD DECLARATION                                     */
/******************************************************************************/
/* callback function for memory allocation */
RIVP_LDD_STATIC void	  RIVP_AppLddMemCbAlloc(
						 RIVP_LDD_PTR					 user_arg,
						 e_osal_mmngr_event_t		  	 event,
						 e_osal_return_t				 error,
						 osal_memory_buffer_handle_t	 buffer_hndl);
RIVP_LDD_STATIC void 	  RIVP_AppLddMemCbDeAlloc(
						RIVP_LDD_PTR					 user_arg,
						e_osal_mmngr_event_t		  	 event,
						e_osal_return_t				  	 error,
						osal_memory_buffer_handle_t 	 buffer_hndl);
/* Internal function for memory allocation  */
RIVP_LDD_U32 RIVP_AppLddMmgrAlignSize(
						RIVP_LDD_U32						 size);

/******************************************************************************/
/*                         VARIABLE                                           */
/******************************************************************************/
RIVP_LDD_STATIC RIVP_LDD_PTR					s_rivpAppLddMmgrSem	= RIVP_LDD_NULL;
RIVP_LDD_STATIC osal_memory_manager_handle_t	s_rivpAppLddMmgrHandle = RIVP_LDD_NULL;
RIVP_LDD_STATIC RIVP_APP_LDD_MMGR_CTL_T			s_rivpAppLddMmgrMemBlock[RIVP_APP_LDD_WORK_MMGR_NUM_MAX];
RIVP_LDD_STATIC RIVP_APP_LDD_MEM_CTL_T			s_rivpAppLddMemAddr[RIVP_APP_LDD_WORK_MEM_NUM_MAX];

extern osal_memory_manager_handle_t g_hndl_mmngr;

/******************************************************************************/
/*                    EXTERNAL FUNCTIONS                                      */
/******************************************************************************/
/**
 * \brief		 Open MMGR handle
 *
 * \retval		 RIVP_LDD_TRUE				 : Success
 * \retval		 RIVP_LDD_FALSE				 : Failure
 *
 * \note		 This function opens MMGR handle.
 *				 Return value indicates whether the processing of this function
 *				 success or failure.
 */
RIVP_LDD_BOOL RIVP_AppLddMmgrOpen(void)
{
	st_osal_mmngr_config_t			 lMmgrCfg;
	e_osal_return_t					 lOsalRet;
	RIVP_LDD_BOOL					 lRet;
	RIVP_LDD_BOOL					 lRetSemLock;

	/* set initial value */
	lRet			= RIVP_LDD_TRUE;
	lRetSemLock		= RIVP_LDD_FALSE;

	/* check MMGR handle */
	if (s_rivpAppLddMmgrHandle == RIVP_LDD_NULL) {

		/* create semaphore for MMGR and set result */
		lRet = RIVP_AppLddOsalSemCreate(&s_rivpAppLddMmgrSem);

		/* check result */
		if (lRet == RIVP_LDD_TRUE) {
			/* lock semaphore for MMGR by call UDF function */
			lRetSemLock = RIVP_AppLddOsalSemLock(s_rivpAppLddMmgrSem);
			/* check locked result */
			if (lRetSemLock == RIVP_LDD_TRUE) {
				/* No action required */
			} else {
				RIVP_LDD_LOG_APP_ERR_SYS(
					"RIVP_AppLddMmgrOpen : RIVP_AppLddOsalSemLock error.\n");
				/* set result */
				lRet = RIVP_LDD_FALSE;
			}
		} else {
			RIVP_LDD_LOG_APP_ERR_SYS(
				"RIVP_AppLddMmgrOpen : RIVP_AppLddOsalSemCreate error.\n");
		}

		/* check result */
		if (lRet == RIVP_LDD_TRUE) {
			/* initialize memory manager parameter */
			(void)memset(&lMmgrCfg, 0, sizeof(lMmgrCfg));	/* ToDo: void cast */

			/* get memory manager configuration */
			lOsalRet = R_OSAL_MmngrGetOsalMaxConfig(&lMmgrCfg);

			/* check osal result */
			if (lOsalRet == OSAL_RETURN_OK) {
// 				/* set allocate times */
// 				lMmgrCfg.max_allowed_allocations = 4u;
// 				lMmgrCfg.max_registered_monitors_cbs = 3u;
// #ifdef RIVP_DEP_LDD_ENABLE_JPEG
// 				lMmgrCfg.memory_limit = RIVP_AppLddMmgrAlignSize(RIVP_APP_LDD_JPG_DATA_STRM_SIZE + RIVP_APP_LDD_STRM_MARGIN);
// #else
// 				lMmgrCfg.memory_limit = RIVP_AppLddMmgrAlignSize(RIVP_APP_LDD_AVC_DATA_STRM_SIZE + RIVP_APP_LDD_STRM_MARGIN);
// #endif
// 				lMmgrCfg.memory_limit += RIVP_AppLddMmgrAlignSize(RIVP_APP_LDD_RING_BUFF_SIZE + RIVP_APP_LDD_STRM_MARGIN);
// 				lMmgrCfg.memory_limit += RIVP_AppLddMmgrAlignSize(RIVP_APP_LDD_WORKBUF_SIZE);
// 				lMmgrCfg.memory_limit += RIVP_AppLddMmgrAlignSize(RIVP_APP_LDD_FRAME_MEM_SIZE);

				/* Open memory manager handler */
				lOsalRet = R_OSAL_MmngrOpen(&lMmgrCfg, &s_rivpAppLddMmgrHandle);
				g_hndl_mmngr = s_rivpAppLddMmgrHandle;
				if (lOsalRet != OSAL_RETURN_OK) {
					RIVP_LDD_LOG_APP_ERR_SYS(
						"RIVP_AppLddMmgrOpen : R_OSAL_MmngrOpen error(%d).\n",
						lOsalRet);
				}
			}

			/* check osal result */
			if (lOsalRet == OSAL_RETURN_OK) {
				/* Regist monitor function for memory allocate */
				lOsalRet = R_OSAL_MmngrRegisterMonitor(s_rivpAppLddMmgrHandle,
														OSAL_MMNGR_EVENT_ALLOCATE,
														&RIVP_AppLddMemCbAlloc,
														"RIVP_AppLddMemCbAlloc");
				if (lOsalRet != OSAL_RETURN_OK) {
					RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrOpen : R_OSAL_MmngrRegisterMonitor fail."\
											" event=%d, osal_ret=%d \n", OSAL_MMNGR_EVENT_ALLOCATE, lOsalRet);
				}
			}

			/* check osal result */
			if (lOsalRet == OSAL_RETURN_OK) {
				/* Regist monitor function for memory deallocate */
				lOsalRet = R_OSAL_MmngrRegisterMonitor(s_rivpAppLddMmgrHandle,
														OSAL_MMNGR_EVENT_DEALLOCATE,
														&RIVP_AppLddMemCbDeAlloc,
														"RIVP_AppLddMemCbDeAlloc");
				if (lOsalRet != OSAL_RETURN_OK) {
					RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrOpen : R_OSAL_MmngrRegisterMonitor fail."\
											" event=%d, osal_ret=%d \n", OSAL_MMNGR_EVENT_DEALLOCATE, lOsalRet);
				}
			}

			/* check osal result */
			if (lOsalRet == OSAL_RETURN_OK) {
				/* clear static memory block information */
				(void)memset(s_rivpAppLddMmgrMemBlock,
								0,
								sizeof(s_rivpAppLddMmgrMemBlock));	/* ToDo: void cast */
				(void)memset(s_rivpAppLddMemAddr, 0, sizeof(s_rivpAppLddMemAddr));	/* ToDo: void cast */
			} else {
				/* set result */
				lRet = RIVP_LDD_FALSE;
			}
		}

	} else {
		/* No action required */
	}

	/* check semaphore locked result */
	if (lRetSemLock == RIVP_LDD_TRUE) {
		/* unlock semaphore for memory driver */
		lRet = RIVP_AppLddOsalSemUnlock(s_rivpAppLddMmgrSem);
		/* check unlocked result */
		if (lRet == RIVP_LDD_TRUE) {
			/* No action required */
		} else {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrOpen : RIVP_AppLddOsalSemUnlock error.\n");
		}
	} else {
		/* No action required */
	}

	if (lRet == RIVP_LDD_TRUE) {
		/* No action required */
	} else {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrOpen : RIVP_AppLddMmgrOpen error.\n");
	}

	/* return result */
	return lRet;
}

/**
 * \brief		 Close MMGR handle
 *
 * \retval		 RIVP_LDD_TRUE				 : Success
 * \retval		 RIVP_LDD_FALSE				 : Failure
 *
 * \note		 This function closes MMGR handle.
 *				 Return value indicates whether the processing of this function
 *				 success or failure.
 */
RIVP_LDD_BOOL RIVP_AppLddMmgrClose(void)
{
	RIVP_LDD_BOOL	 lRet;
	RIVP_LDD_BOOL	 lRetSemLock;
	e_osal_return_t	 lOsalRet;

	/* set initial value */
	lRet			= RIVP_LDD_TRUE;
	lRetSemLock		= RIVP_LDD_FALSE;

	/* check MMGR handle */
	if (s_rivpAppLddMmgrHandle != RIVP_LDD_NULL) {
		/* No action required */
	} else {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrClose : Not opened. \n");
		/* set result */
		lRet = RIVP_LDD_FALSE;
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* lock semaphore for memory driver */
		lRetSemLock = RIVP_AppLddOsalSemLock(s_rivpAppLddMmgrSem);

		/* check semaphore locked result and set result */
		if (lRetSemLock == RIVP_LDD_TRUE) {
			lOsalRet = R_OSAL_MmngrClose(s_rivpAppLddMmgrHandle);

			/* check osal result */
			if (lOsalRet == OSAL_RETURN_OK) {
				/* clear MMGR handle */
				s_rivpAppLddMmgrHandle = RIVP_LDD_NULL;

			} else {
				RIVP_LDD_LOG_APP_ERR_SYS(
					"RIVP_AppLddMmgrClose : R_OSAL_MmngrClose error(%d).\n",
					lOsalRet);
			}
		} else {
			RIVP_LDD_LOG_APP_ERR_SYS(
				"RIVP_AppLddMmgrClose : RIVP_AppLddOsalSemLock error.\n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}



	/* check semaphore locked result */
	if (lRetSemLock == RIVP_LDD_TRUE) {
		/* unlock semaphore for memory driver */
		lRet = RIVP_AppLddOsalSemUnlock(s_rivpAppLddMmgrSem);
		if (lRet == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrClose : RIVP_AppLddOsalSemUnlock error.\n");
		} else {
			lRet = RIVP_AppLddOsalSemDestroy(s_rivpAppLddMmgrSem);
			if (lRet == RIVP_LDD_FALSE) {
				RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrClose : RIVP_AppLddOsalSemDestroy error.\n");
			}
		}
	}

	if (lRet == RIVP_LDD_FALSE) {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrClose : RIVP_AppLddMmgrClose error.\n");
	}

	/* return result */
	return lRet;
}

/**
 * \brief		 Allocation memory by memory manager
 *
 * \param[in]	 size					 : Size(byte) to allocate
 * \param[in]	 align					 : Align(byte) to allocate
 *
 * \retval		 Allocated address		 : Address of the allocated area by memory manager
 *
 * \note		 This function allocate memory by memory manager.
 *				 "size" of the argument to store the size of the memory to allocate.
 *				 "align" of the argument to store the align of the memory to allocate.
 *				 Return value indicates address of the allocated memory.
 */
RIVP_LDD_PTR RIVP_AppLddMmgrAlloc(
	RIVP_LDD_U32	 size,
	RIVP_LDD_U32	 align
)
{
	RIVP_LDD_U32					 lCnt;
	RIVP_LDD_U32					 lSize;
	RIVP_LDD_PTR					 lAddr;
	RIVP_LDD_BOOL					 lRet;
	RIVP_APP_LDD_MMGR_CTL_T			*lMmgrDataPtr;
	RIVP_LDD_BOOL					 lRetSemLock;
	e_osal_return_t				 lOsalRet;
	osal_memory_buffer_handle_t	 lMmgrBufHandle;
	osal_axi_bus_id_t			 lAxi_bus_id;

	/* set initial value */
	lAddr			= RIVP_LDD_NULL;
	lRet			= RIVP_LDD_TRUE;
	lMmgrDataPtr	= RIVP_LDD_NULL;
	lRetSemLock		= RIVP_LDD_FALSE;
	
	/* set lAxi_bus_id to get hardware addresss. */
	/* Use R_OSAL_IoGetDeviceAxiBusId(device_handle). */
	/* but not support now. */
	lAxi_bus_id = 1u;

	/* check arguments */
	if ((size == 0u) || (align == 0u)) {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrAlloc : Invalid arguments. \n");
		/* set result */
		lRet = RIVP_LDD_FALSE;
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* check MMGR handle */
		if (s_rivpAppLddMmgrHandle == RIVP_LDD_NULL) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrAlloc : Not opened. \n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* lock semaphore for MMGR by call UDF function and check lock result */
		lRetSemLock = RIVP_AppLddOsalSemLock(s_rivpAppLddMmgrSem);

		if (lRetSemLock == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrAlloc : RIVP_AppLddOsalSemLock error.\n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* search unused static memory block information */
		for (lCnt = 0u; lCnt < RIVP_APP_LDD_WORK_MMGR_NUM_MAX; lCnt++) {
			if (s_rivpAppLddMmgrMemBlock[lCnt].size == 0u) {
				lMmgrDataPtr = &s_rivpAppLddMmgrMemBlock[lCnt];
				lCnt = RIVP_APP_LDD_WORK_MMGR_NUM_MAX;
			}
		}

		/* check search result */
		if (lMmgrDataPtr == RIVP_LDD_NULL) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrAlloc : s_rivpAppLddMmgrMemBlock buffer full.\n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		} else {
			/* align size for allocate memory */
			lSize = RIVP_AppLddMmgrAlignSize(size);
			/* set static MMGR memory block information */
			/* allocate memory by call MMGR function and check allocate result */
			(void)memset(&lMmgrBufHandle, 0, sizeof(lMmgrBufHandle));	/* ToDo: void cast */
			lOsalRet = R_OSAL_MmngrAlloc(s_rivpAppLddMmgrHandle,
										 lSize,
										 align,
										 &lMmgrBufHandle);

			if (lOsalRet != OSAL_RETURN_OK) {
				/* clear size of static memory block information */
				lMmgrDataPtr->size		= 0u;
				RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrAlloc : R_OSAL_MmngrAlloc returns (%d).\n", lOsalRet);
			} else {
				/* get virtual address */
				lMmgrDataPtr->virtAdr = (RIVP_LDD_PTR)RIVP_APP_LDD_OSAL_INVALID_ADDRESS;
				lOsalRet = R_OSAL_MmngrGetCpuPtr(lMmgrBufHandle, &(lMmgrDataPtr->virtAdr));
				if (lOsalRet == OSAL_RETURN_OK) {
					/* get physical address */
					lOsalRet = R_OSAL_MmngrGetHwAddr(lMmgrBufHandle, lAxi_bus_id, (uintptr_t *)&lMmgrDataPtr->physAdr);
				}

				if (lOsalRet == OSAL_RETURN_OK) {
					/* set static MMGR memory block control information */
					lMmgrDataPtr->size = (lSize + align - 1) & ~(align - 1);
					lMmgrDataPtr->mmgrBufHandle = lMmgrBufHandle;
				}

				/* set result */
				lAddr = lMmgrDataPtr->virtAdr;
			}
		}
	}

	/* check semaphore */
	if (lRetSemLock == RIVP_LDD_TRUE) {
		/* unlock semaphore for MMGR by call UDF function */
		lRet = RIVP_AppLddOsalSemUnlock(s_rivpAppLddMmgrSem);
		if (lRet == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrAlloc : RIVP_AppLddOsalSemUnlock error.\n");
		}
	}

	/* return result */
	return lAddr;
}

/**
 * \brief		 Free memory of memory manager
 *
 * \param[in]	 ptr					 : Address of the allocated area by memory manager
 *
 * \retval		 None					 : None
 *
 * \note		 This function releases the memory allocated by RIVP_AppLddMmgrAlloc.
 *				 "ptr" of the argument to store the address of the allocated memory.
 */
RIVP_LDD_BOOL RIVP_AppLddMmgrFree(
	RIVP_LDD_PTR	 ptr
)
{
	RIVP_LDD_U32				 lCnt;
	RIVP_LDD_BOOL				 lRet;
	e_osal_return_t				 lOsalRet;
	RIVP_APP_LDD_MMGR_CTL_T		*lMmgrDataPtr;
	RIVP_LDD_BOOL				 lRetSemLock;

	/* set initial value */
	lRet			= RIVP_LDD_TRUE;
	lMmgrDataPtr	= RIVP_LDD_NULL;
	lRetSemLock		= RIVP_LDD_FALSE;

	/* check arguments */
	if (ptr == RIVP_LDD_NULL) {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrFree : Invalid arguments. \n");
		/* set result */
		lRet = RIVP_LDD_FALSE;
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* check MMGR handle */
		if (s_rivpAppLddMmgrHandle == RIVP_LDD_NULL) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrFree : Not opened. \n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* lock semaphore for MMGR by call UDF function */
		lRetSemLock = RIVP_AppLddOsalSemLock(s_rivpAppLddMmgrSem);

		/* check locked result */
		if (lRetSemLock == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrFree : RIVP_AppLddOsalSemLock error. \n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* search used static memory block information */
		for (lCnt = 0u; lCnt < RIVP_APP_LDD_WORK_MMGR_NUM_MAX; lCnt++) {
			if ((s_rivpAppLddMmgrMemBlock[lCnt].size != 0u) &&
				((RIVP_LDD_PTR)s_rivpAppLddMmgrMemBlock[lCnt].virtAdr == ptr)) {
				lMmgrDataPtr = &s_rivpAppLddMmgrMemBlock[lCnt];
				lCnt = RIVP_APP_LDD_WORK_MMGR_NUM_MAX;
			}
		}

		/* check memory block information */
		if (lMmgrDataPtr != RIVP_LDD_NULL) {
			/* free memory by call MMGR function and check free result */
			lOsalRet = R_OSAL_MmngrDealloc(s_rivpAppLddMmgrHandle, lMmgrDataPtr->mmgrBufHandle);
			if (lOsalRet != OSAL_RETURN_OK) {
				RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrFree : R_OSAL_MmngrDealloc error(%d).\n", lOsalRet);
				/* set result */
				lRet = RIVP_LDD_FALSE;
			} else {
				/* clear size of static MMGR memory block control information */
				lMmgrDataPtr->size	= 0u;
			}
		}
	}

	/* check semaphore */
	if (lRetSemLock == RIVP_LDD_TRUE) {
		/* unlock semaphore for MMGR by call UDF function */
		lRet = RIVP_AppLddOsalSemUnlock(s_rivpAppLddMmgrSem);
		if (lRet == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrFree : RIVP_AppLddOsalSemUnlock error.\n");
		}
	}

	return lRet;
}

/**
 * \brief		 Conversion from virtual address to physical address in memory of memory manager
 *
 * \param[in]	 ptr					 : Address of the allocated area by memory manager
 *
 * \retval		 Physical address		 : Converted physical address
 *
 * \note		 This function converts a physical address from a virtual address of the memory allocated by the RIVP_AppLddMmgrAlloc.
 *				 "ptr" of the argument to store the virtual address of the allocated memory.
 *				 Return value indicates the physical address that has been converted from a virtual address.
 */
RIVP_LDD_U32 RIVP_AppLddMmgrVtoP(RIVP_LDD_PTR ptr)
{
	RIVP_LDD_U32					 lCnt;
	RIVP_LDD_BOOL					 lRet;
	RIVP_APP_LDD_MMGR_CTL_T			*lMmgrDataPtr;
	RIVP_LDD_BOOL					 lRetSemLock;
	RIVP_LDD_U32					 lPhysAddr;

	/* set initial value */
	lRet			= RIVP_LDD_TRUE;
	lMmgrDataPtr	= RIVP_LDD_NULL;
	lRetSemLock		= RIVP_LDD_FALSE;
	lPhysAddr		= (RIVP_LDD_U32)((uintptr_t)ptr);
	
	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* check MMGR handle */
		if (s_rivpAppLddMmgrHandle == RIVP_LDD_NULL) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrVtoP : Not opened.\n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* lock semaphore for MMGR by call UDF function */
		lRetSemLock = RIVP_AppLddOsalSemLock(s_rivpAppLddMmgrSem);

		/* check locked result */
		if (lRetSemLock == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrVtoP : RIVP_AppLddOsalSemLock error.\n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		lRet = RIVP_LDD_FALSE;
		/* search used static memory block information */
		for (lCnt = 0u; lCnt < RIVP_APP_LDD_WORK_MMGR_NUM_MAX; lCnt++) {

			lMmgrDataPtr = &s_rivpAppLddMmgrMemBlock[lCnt];

			if ((lMmgrDataPtr->size != 0u) &&
				(((uintptr_t)ptr - (uintptr_t)lMmgrDataPtr->virtAdr) < lMmgrDataPtr->size)) {

				/* calc physical address from a virtual address */
				lPhysAddr = lMmgrDataPtr->physAdr + (RIVP_LDD_U32)((uintptr_t)ptr - (uintptr_t)lMmgrDataPtr->virtAdr);
				lCnt      = RIVP_APP_LDD_WORK_MMGR_NUM_MAX;
				lRet      = RIVP_LDD_TRUE;
			}
		}
	}

	/* check semaphore */
	if (lRetSemLock == RIVP_LDD_TRUE) {
		/* unlock semaphore for MMGR by call UDF function */
		lRet = RIVP_AppLddOsalSemUnlock(s_rivpAppLddMmgrSem);
		if (lRet == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrVtoP : RIVP_AppLddOsalSemUnlock error.\n");
		}
	}

	return lPhysAddr;
}

/**
 * \brief		 Conversion from physical address to virtual address in memory of memory manager
 *
 * \param[in]	 ptr					 : Address of the allocated area by memory manager
 *
 * \retval		 Virtual address		 : Converted virtual address
 *
 * \note		 This function converts a virtual address from a physical address of the memory allocated by the RIVP_AppLddMmgrAlloc.
 *				 "ptr" of the argument to store the physical address of the allocated memory.
 *				 Return value indicates the virtual address that has been converted from a physical address.
 */
RIVP_LDD_PTR RIVP_AppLddMmgrPtoV(RIVP_LDD_U32 ptr)
{
	RIVP_LDD_U32				 lCnt;
	RIVP_LDD_BOOL				 lRet;
	RIVP_APP_LDD_MMGR_CTL_T		*lMmgrDataPtr;
	RIVP_LDD_BOOL				 lRetSemLock;
	uintptr_t				 lVirtAddr;

	/* set initial value */
	lRet			= RIVP_LDD_TRUE;
	lMmgrDataPtr	= RIVP_LDD_NULL;
	lRetSemLock		= RIVP_LDD_FALSE;
	lVirtAddr		= (uintptr_t)ptr;
	
	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* check MMGR handle */
		if (s_rivpAppLddMmgrHandle == RIVP_LDD_NULL) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrPtoV : Not opened. \n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* lock semaphore for MMGR by call UDF function */
		lRetSemLock = RIVP_AppLddOsalSemLock(s_rivpAppLddMmgrSem);

		/* check locked result */
		if (lRetSemLock == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrPtoV : RIVP_AppLddOsalSemLock error.\n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		lRet = RIVP_LDD_FALSE;
		/* search used static memory block information */
		for (lCnt = 0u; lCnt < RIVP_APP_LDD_WORK_MMGR_NUM_MAX; lCnt++) {

			lMmgrDataPtr = &s_rivpAppLddMmgrMemBlock[lCnt];

			if ((lMmgrDataPtr->size != 0u) &&
				((ptr - lMmgrDataPtr->physAdr) < lMmgrDataPtr->size)) {

				/* calc virtual address from physical address */
				lVirtAddr = (uintptr_t)lMmgrDataPtr->virtAdr + (ptr - lMmgrDataPtr->physAdr);
				lCnt      = RIVP_APP_LDD_WORK_MMGR_NUM_MAX;
				lRet      = RIVP_LDD_TRUE;
			}
		}
	}

	/* check semaphore */
	if (lRetSemLock == RIVP_LDD_TRUE) {
		/* unlock semaphore for MMGR by call UDF function */
		lRet = RIVP_AppLddOsalSemUnlock(s_rivpAppLddMmgrSem);
		if (lRet == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrPtoV : RIVP_AppLddOsalSemUnlock error.\n");
		}
	}

	return (RIVP_LDD_PTR)lVirtAddr;
}

/**
 * \brief			 flush memory
 *
 * \param[in]		 ptr				: virtual address of the memory allocated
 *
 * \retval			 void				: none
 *
 */
void RIVP_AppLddMmgrFlush(
		const RIVP_LDD_PTR		ptr
		)
{
	RIVP_LDD_U32					 lCnt;
	RIVP_LDD_BOOL					 lRet;
	RIVP_APP_LDD_MMGR_CTL_T			*lMmgrDataPtr;
	RIVP_LDD_BOOL					 lRetSemLock;
	osal_memory_buffer_handle_t	 lBuffHandle;
	e_osal_return_t				 lOsalRet;
	size_t						 lSize;
	uintptr_t					 lBaseAdr;
	uintptr_t					 lEndAdr;

	/* set initial value */
	lRet			= RIVP_LDD_TRUE;
	lBuffHandle		= RIVP_LDD_NULL;
	lMmgrDataPtr	= RIVP_LDD_NULL;
	lRetSemLock		= RIVP_LDD_FALSE;

	/* check arguments */
	if (ptr == RIVP_LDD_NULL) {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrFlush : Invalid arguments. \n");
		/* set result */
		lRet = RIVP_LDD_FALSE;
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* check MMGR handle */
		if (s_rivpAppLddMmgrHandle == RIVP_LDD_NULL) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrFlush : Not opened. \n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* lock semaphore for MMGR by call UDF function */
		lRetSemLock = RIVP_AppLddOsalSemLock(s_rivpAppLddMmgrSem);

		/* check locked result */
		if (lRetSemLock == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrFlush : RIVP_AppLddOsalSemLock error. \n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* search used static memory block information */
		for (lCnt = 0u; lCnt < RIVP_APP_LDD_WORK_MMGR_NUM_MAX; lCnt++) {
			lBaseAdr = (uintptr_t)s_rivpAppLddMmgrMemBlock[lCnt].virtAdr;
			lEndAdr = lBaseAdr + s_rivpAppLddMmgrMemBlock[lCnt].size;
//			if ((s_rivpAppLddMmgrMemBlock[lCnt].size != 0u) &&
//				((RIVP_LDD_PTR)s_rivpAppLddMmgrMemBlock[lCnt].virtAdr == ptr)) {
			if ((s_rivpAppLddMmgrMemBlock[lCnt].size != 0u) &&
				(lBaseAdr <= (uintptr_t)ptr) && ((uintptr_t)ptr < lEndAdr)) {
				lMmgrDataPtr = &s_rivpAppLddMmgrMemBlock[lCnt];
				lCnt = RIVP_APP_LDD_WORK_MMGR_NUM_MAX;
			}
		}

		/* check memory block information */
		if (lMmgrDataPtr != RIVP_LDD_NULL) {
			lBuffHandle = lMmgrDataPtr->mmgrBufHandle;
		}
		if (lBuffHandle != RIVP_LDD_NULL) {
			/* get the size of buffer object */
			lOsalRet = R_OSAL_MmngrGetSize(lBuffHandle, &lSize);
			if (lOsalRet == OSAL_RETURN_OK) {
				/* flash memory */
				lOsalRet = R_OSAL_MmngrFlush(lBuffHandle, 0u, lSize);
				if (lOsalRet != OSAL_RETURN_OK) {
					RIVP_LDD_LOG_APP_ERR_SYS("R_OSAL_MmngrFlush fail."\
								" osal_ret=%d \n",lOsalRet);
				}
			} else {
				RIVP_LDD_LOG_APP_ERR_SYS("R_OSAL_MmngrGetSize fail."\
								" osal_ret=%d \n",lOsalRet);
			}
		}
	}

	/* check semaphore */
	if (lRetSemLock == RIVP_LDD_TRUE) {
		/* unlock semaphore for MMGR by call UDF function */
		lRet = RIVP_AppLddOsalSemUnlock(s_rivpAppLddMmgrSem);
		if (lRet == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrFlush : RIVP_AppLddOsalSemUnlock error.\n");
		}
	}
}
	
/**
 * \brief			 Invalidate cache
 *
 * \param[in]		 ptr				: virtual address of the memory allocated
 *
 * \retval			 void				: none
 *
 */
void RIVP_AppLddMmgrInvalidate(
		const RIVP_LDD_PTR		ptr
		)
{
	RIVP_LDD_U32					 lCnt;
	RIVP_LDD_BOOL					 lRet;
	RIVP_APP_LDD_MMGR_CTL_T			*lMmgrDataPtr;
	RIVP_LDD_BOOL					 lRetSemLock;
	osal_memory_buffer_handle_t	 lBuffHandle;
	e_osal_return_t				 lOsalRet;
	size_t						 lSize;

	/* set initial value */
	lRet			= RIVP_LDD_TRUE;
	lBuffHandle		= RIVP_LDD_NULL;
	lMmgrDataPtr	= RIVP_LDD_NULL;
	lRetSemLock		= RIVP_LDD_FALSE;

	/* check arguments */
	if (ptr == RIVP_LDD_NULL) {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrInvalidate : Invalid arguments. \n");
		/* set result */
		lRet = RIVP_LDD_FALSE;
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* check MMGR handle */
		if (s_rivpAppLddMmgrHandle == RIVP_LDD_NULL) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrInvalidate : Not opened. \n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* lock semaphore for MMGR by call UDF function */
		lRetSemLock = RIVP_AppLddOsalSemLock(s_rivpAppLddMmgrSem);

		/* check locked result */
		if (lRetSemLock == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrInvalidate : RIVP_AppLddOsalSemLock error. \n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* search used static memory block information */
		for (lCnt = 0u; lCnt < RIVP_APP_LDD_WORK_MMGR_NUM_MAX; lCnt++) {
			if ((s_rivpAppLddMmgrMemBlock[lCnt].size != 0u) &&
				((RIVP_LDD_PTR)s_rivpAppLddMmgrMemBlock[lCnt].virtAdr == ptr)) {
				lMmgrDataPtr = &s_rivpAppLddMmgrMemBlock[lCnt];
				lCnt = RIVP_APP_LDD_WORK_MMGR_NUM_MAX;
			}
		}

		/* check memory block information */
		if (lMmgrDataPtr != RIVP_LDD_NULL) {
			lBuffHandle = lMmgrDataPtr->mmgrBufHandle;
		}
		if (lBuffHandle != RIVP_LDD_NULL) {
			/* get the size of buffer object */
			lOsalRet = R_OSAL_MmngrGetSize(lBuffHandle,	&lSize);
			if (lOsalRet == OSAL_RETURN_OK) {
				/* invalidate cache */
				lOsalRet = R_OSAL_MmngrInvalidate(lBuffHandle, 0u, lSize);
				if (lOsalRet != OSAL_RETURN_OK) {
					RIVP_LDD_LOG_APP_ERR_SYS("R_OSAL_MmngrInvalidate fail."\
								" osal_ret=%d \n",lOsalRet);
				}
			} else {
				RIVP_LDD_LOG_APP_ERR_SYS("R_OSAL_MmngrGetSize fail."\
								" osal_ret=%d \n",lOsalRet);
			}
		}
	}

	/* check semaphore */
	if (lRetSemLock == RIVP_LDD_TRUE) {
		/* unlock semaphore for MMGR by call UDF function */
		lRet = RIVP_AppLddOsalSemUnlock(s_rivpAppLddMmgrSem);
		if (lRet == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrInvalidate : RIVP_AppLddOsalSemUnlock error.\n");
		}
	}
}

/**
 * \brief		 Get the buffer handle of memory manager
 *
 * \param[in]	 ptr					: virtual address of the memory allocated
 *
 * \retval		 Buffer handle			: buffer handle of the memory allocated
 *
 * \note		 This function get buffer handle of memory.
 *				 Please set the virtual address of the memory allocated by the RIVP_AppLddMmgrAlloc to argument "ptr".
 */
osal_memory_buffer_handle_t RIVP_AppLddMmgrGetBufHdl(
		const RIVP_LDD_PTR					ptr
		)
{
	RIVP_LDD_U32					 lCnt;
	RIVP_LDD_BOOL					 lRet;
	RIVP_APP_LDD_MMGR_CTL_T			*lMmgrDataPtr;
	RIVP_LDD_BOOL					 lRetSemLock;
	osal_memory_buffer_handle_t	 lRetHandle;

	/* set initial value */
	lRet			= RIVP_LDD_TRUE;
	lRetHandle		= RIVP_LDD_NULL;
	lMmgrDataPtr	= RIVP_LDD_NULL;
	lRetSemLock		= RIVP_LDD_FALSE;

	/* check arguments */
	if (ptr == RIVP_LDD_NULL) {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrGetBufHdl : Invalid arguments. \n");
		/* set result */
		lRet = RIVP_LDD_FALSE;
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* check MMGR handle */
		if (s_rivpAppLddMmgrHandle == RIVP_LDD_NULL) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrGetBufHdl : Not opened. \n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* lock semaphore for MMGR by call UDF function */
		lRetSemLock = RIVP_AppLddOsalSemLock(s_rivpAppLddMmgrSem);

		/* check locked result */
		if (lRetSemLock == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrGetBufHdl : RIVP_AppLddOsalSemLock error. \n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* search used static memory block information */
		for (lCnt = 0u; lCnt < RIVP_APP_LDD_WORK_MMGR_NUM_MAX; lCnt++) {
			if ((s_rivpAppLddMmgrMemBlock[lCnt].size != 0u) &&
				((RIVP_LDD_PTR)s_rivpAppLddMmgrMemBlock[lCnt].virtAdr == ptr)) {
				lMmgrDataPtr = &s_rivpAppLddMmgrMemBlock[lCnt];
				lCnt = RIVP_APP_LDD_WORK_MMGR_NUM_MAX;
			}
		}

		/* check memory block information */
		if (lMmgrDataPtr != RIVP_LDD_NULL) {
			lRetHandle = lMmgrDataPtr->mmgrBufHandle;
		}
	}

	/* check semaphore */
	if (lRetSemLock == RIVP_LDD_TRUE) {
		/* unlock semaphore for MMGR by call UDF function */
		lRet = RIVP_AppLddOsalSemUnlock(s_rivpAppLddMmgrSem);
		if (lRet == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMmgrGetBufHdl : RIVP_AppLddOsalSemUnlock error.\n");
		}
	}

	return lRetHandle;
}

/**
 * \brief		 Allocation memory from heap
 *
 * \param[out]	 addr					 : Address of the allocated area
 * \param[in]	 size					 : Size(byte) to allocate
 *
 * \retval		 RIVP_LDD_TRUE				 : Success
 * \retval		 RIVP_LDD_FALSE				 : Failure
 *
 * \note		 This function allocate memory from heap.
 *				 "addr" of the argument is stored the address of the allocated memory.
 *				 "size" of the argument to store the size of the memory to allocate.
 *				 Return value indicates objectives success or failure of this function.
 */
RIVP_LDD_BOOL RIVP_AppLddMemAlloc(
	RIVP_LDD_PTR	*addr,
	RIVP_LDD_U32	size
)
{
	RIVP_LDD_U32					 lCnt;
	RIVP_LDD_BOOL					 lRet;
	RIVP_LDD_BOOL					 lRetSemLock;
	RIVP_APP_LDD_MEM_CTL_T			*lHeapDataPtr;

	/* set initial value */
	lRet			= RIVP_LDD_TRUE;
	lRetSemLock		= RIVP_LDD_FALSE;
	lHeapDataPtr	= RIVP_LDD_NULL;
	
	/* check arguments */
	if ((addr == RIVP_LDD_NULL) || (size == 0u)) {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMemAlloc : Invalid arguments. \n");
		/* set result */
		lRet = RIVP_LDD_FALSE;
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* lock semaphore for MMGR by call UDF function and check lock result */
		lRetSemLock = RIVP_AppLddOsalSemLock(s_rivpAppLddMmgrSem);

		if (lRetSemLock == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMemAlloc : RIVP_AppLddOsalSemLock error.\n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* search unused static heap memory address control information */
		for (lCnt = 0u; lCnt < RIVP_APP_LDD_WORK_MEM_NUM_MAX; lCnt++) {
			if (s_rivpAppLddMemAddr[lCnt].size == 0u) {
				lHeapDataPtr = &s_rivpAppLddMemAddr[lCnt];
				lCnt = RIVP_APP_LDD_WORK_MEM_NUM_MAX;
			}
		}

		/* check search result */
		if (lHeapDataPtr != RIVP_LDD_NULL) {
			/* alloc the requested size from heap */
			lHeapDataPtr->addr = (RIVP_LDD_PTR)malloc((size_t)size);
		}

		/* check malloc result */
		if (lHeapDataPtr->addr == RIVP_LDD_NULL) {
			/* set result */
			lRet = RIVP_LDD_FALSE;
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMemAlloc : malloc error. addr=[0x%08X], size=[%d] \n", lHeapDataPtr->addr, size);
		} else {
			/* set static Heap memory address control information */
			lHeapDataPtr->size = size;

			/* set address */
			*addr = lHeapDataPtr->addr;
		}
	}

	/* check semaphore */
	if (lRetSemLock == RIVP_LDD_TRUE) {
		/* unlock semaphore for MMGR by call UDF function */
		lRet = RIVP_AppLddOsalSemUnlock(s_rivpAppLddMmgrSem);
		if (lRet == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMemAlloc : RIVP_AppLddOsalSemUnlock error.\n");
		}
	}

	if (RIVP_LDD_TRUE != lRet) {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMemAlloc : RIVP_AppLddMemAlloc err \n");
	}

	/* return result */
	return lRet;
}

/**
 * \brief		 Free memory of heap
 *
 * \param[in]	 addr					 : Address of the allocated area
 *
 * \retval		 RIVP_LDD_TRUE			 : Success
 * \retval		 RIVP_LDD_FALSE			 : Failure
 *
 * \note		 This function releases the memory allocated by RIVP_AppLddMemAlloc.
 *				 "addr" of the argument to store the address of the allocated memory.
 *				 Return value indicates objectives success or failure of this function.
 */
RIVP_LDD_BOOL RIVP_AppLddMemFree(RIVP_LDD_PTR addr)
{
	RIVP_LDD_U32				 lCnt;
	RIVP_LDD_BOOL				 lRet;
	RIVP_LDD_BOOL				 lRetSemLock;
	RIVP_APP_LDD_MEM_CTL_T		*lHeapDataPtr;

	/* set initial value */
	lRet			= RIVP_LDD_TRUE;
	lRetSemLock		= RIVP_LDD_FALSE;
	lHeapDataPtr	= RIVP_LDD_NULL;
	
	/* check arguments */
	if (addr == RIVP_LDD_NULL) {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMemFree : Invalid arguments. \n");
		/* set result */
		lRet = RIVP_LDD_FALSE;
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* lock semaphore for MMGR by call UDF function and check lock result */
		lRetSemLock = RIVP_AppLddOsalSemLock(s_rivpAppLddMmgrSem);

		if (lRetSemLock == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMemFree : RIVP_AppLddOsalSemLock error.\n");
			/* set result */
			lRet = RIVP_LDD_FALSE;
		}
	}

	/* check result */
	if (lRet == RIVP_LDD_TRUE) {
		/* search used static memory address information */
		for (lCnt = 0u; lCnt < RIVP_APP_LDD_WORK_MEM_NUM_MAX; lCnt++) {
			if ((s_rivpAppLddMemAddr[lCnt].size != 0u) &&
				(addr == s_rivpAppLddMemAddr[lCnt].addr)) {
				lHeapDataPtr = &s_rivpAppLddMemAddr[lCnt];
				lCnt = RIVP_APP_LDD_WORK_MEM_NUM_MAX;
			}
		}

		/* check search result */
		if (lHeapDataPtr != RIVP_LDD_NULL) {
			free(addr);

			/* clear size of static Heap memory address control information */
			lHeapDataPtr->size = 0u;
			lHeapDataPtr->addr = RIVP_LDD_NULL;
			lRet               = RIVP_LDD_TRUE;
		}
	}

	/* check semaphore */
	if (lRetSemLock == RIVP_LDD_TRUE) {
		/* unlock semaphore for MMGR by call UDF function */
		lRet = RIVP_AppLddOsalSemUnlock(s_rivpAppLddMmgrSem);
		if (lRet == RIVP_LDD_FALSE) {
			RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMemFree : RIVP_AppLddOsalSemUnlock error.\n");
		}
	}

	if (RIVP_LDD_TRUE != lRet) {
		RIVP_LDD_LOG_APP_ERR_SYS("RIVP_AppLddMemFree : RIVP_AppLddMemFree err \n");
	}

	/* return result */
	return lRet;
}
/******************************************************************************/
/*                    CALLBACK FUNCTIONS                                      */
/******************************************************************************/
/**
 * \brief			 callback function for memory allocation
 *
 * \param[in]		 user_arg		: user defined argument
 * \param[in]		 event			: event type of event manager
 * \param[in]		 error			: error code with memory allocation
 * \param[in]		 buffer_hndl	: memory allocation buffer handler
 *
 *\retval			 void			: none
 *
 */
RIVP_LDD_STATIC void RIVP_AppLddMemCbAlloc(
		RIVP_LDD_PTR				  user_arg,
		e_osal_mmngr_event_t		  event,
		e_osal_return_t				  error,
		osal_memory_buffer_handle_t	  buffer_hndl
		) 
{
	(void)user_arg;
	(void)event;
	(void)error;
	/* Now, this callback function do log output only. */
	if (buffer_hndl == RIVP_LDD_NULL) {
		RIVP_LDD_LOG_APP_INF("RIVP_AppLddMemCbAlloc : buffer_hndl is NULL!\n");
	}
}

/**
 * \brief			 callback function for memory deallocation
 *
 * \param[in]		 user_arg		: user defined argument
 * \param[in]		 event			: event type of event manager
 * \param[in]		 error			: error code with memory allocation
 * \param[in]		 buffer_hndl	: memory allocation buffer handler
 *
 *\retval			 void			: none
 *
 */
RIVP_LDD_STATIC void RIVP_AppLddMemCbDeAlloc(
		RIVP_LDD_PTR				  user_arg,
		e_osal_mmngr_event_t		  event,
		e_osal_return_t				  error,
		osal_memory_buffer_handle_t	  buffer_hndl
		) 
{
	(void)user_arg;
	(void)event;
	(void)error;
	/* Now, this callback function do log output only. */
	if (buffer_hndl == RIVP_LDD_NULL) {
		RIVP_LDD_LOG_APP_INF("RIVP_AppLddMemCbDeAlloc : buffer_hndl is NULL!\n");
	}
}

/******************************************************************************/
/*                    INTERNAL FUNCTIONS                                      */
/******************************************************************************/
/**
 * \brief		 Align the size for allocation memory
 *
 * \param[in]	 size					 : Size(byte) to align
 *
 * \retval		 Aligned size			 : Alined size(byte)
 *
 */
RIVP_LDD_U32 RIVP_AppLddMmgrAlignSize(
	RIVP_LDD_U32	 size
)
{
	RIVP_LDD_U32 lRetSize;
	
	if (size % RIVP_APP_LDD_MMGR_ALIGN_SIZE == 0u) {
		lRetSize = size;
	} else {
		lRetSize = (size + RIVP_APP_LDD_MMGR_ALIGN_SIZE) - (size % RIVP_APP_LDD_MMGR_ALIGN_SIZE);
	}
	
	return lRetSize;
}
