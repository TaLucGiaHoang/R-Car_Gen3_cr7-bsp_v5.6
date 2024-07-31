/*******************************************************************************
 *
 * PURPOSE
 *   RIVP Reference Application Memory Manager Defines
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *
 ******************************************************************************/
/******************************************************************************
* Memory Manager Defines
* Copyright (C) 2017 Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
*******************************************************************************/
#ifndef RIVP_APP_LDD_MMGR_H
#define RIVP_APP_LDD_MMGR_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                    INCLUDE FILES                                           */
/******************************************************************************/
#include "rcar-xos/osal/r_osal.h"
#include "rivp_ldd_types.h"

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
#define RIVP_APP_LDD_MMGR_VA			(0u)
#define RIVP_APP_LDD_MMGR_PA			(1u)
#define RIVP_APP_LDD_MMGR_ALIGN_SIZE	(64u)

/******************************************************************************/
/*                    TYPE DEFINITION                                         */
/******************************************************************************/

/******************************************************************************/
/*                    FUNCTIONS                                               */
/******************************************************************************/
/* open/close */
extern RIVP_LDD_BOOL RIVP_AppLddMmgrOpen(void);
extern RIVP_LDD_BOOL RIVP_AppLddMmgrClose(void);

/* memory manager */
extern RIVP_LDD_PTR  RIVP_AppLddMmgrAlloc(RIVP_LDD_U32 size, RIVP_LDD_U32 align);
extern RIVP_LDD_BOOL RIVP_AppLddMmgrFree(RIVP_LDD_PTR ptr);
extern RIVP_LDD_U32  RIVP_AppLddMmgrVtoP(RIVP_LDD_PTR ptr);
extern RIVP_LDD_PTR  RIVP_AppLddMmgrPtoV(RIVP_LDD_U32 ptr);
extern void          RIVP_AppLddMmgrInvalidate(const RIVP_LDD_PTR ptr);
extern void          RIVP_AppLddMmgrFlush(const RIVP_LDD_PTR ptr);
extern osal_memory_buffer_handle_t  RIVP_AppLddMmgrGetBufHdl(const RIVP_LDD_PTR ptr);

/* memory heap */
extern RIVP_LDD_BOOL RIVP_AppLddMemAlloc(RIVP_LDD_PTR *addr, RIVP_LDD_U32 size);
extern RIVP_LDD_BOOL RIVP_AppLddMemFree(RIVP_LDD_PTR addr);

#ifdef __cplusplus
}
#endif

#endif /* RIVP_APP_LDD_MMGR_H */
