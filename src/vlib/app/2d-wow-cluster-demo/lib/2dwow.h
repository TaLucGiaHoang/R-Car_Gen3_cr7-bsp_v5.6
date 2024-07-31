/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/
#ifndef X2DWOW_H
#define X2DWOW_H

#include "display.h"

#define LOC_RESOURCE_ID_2DG      0x9000U

/* for OSAL resource */

/* Used in wrapper */
#define LOC_OSAL_THREAD_DRW2D_TASK00                  (LOC_RESOURCE_ID_2DG   +   2U)  /* CIO Server */
#define LOC_OSAL_THREAD_DRW2D_TASK01                  (LOC_RESOURCE_ID_2DG   +   3U)  /* TAURUS Server */
#define LOC_OSAL_THREAD_DRW2D_TASK02                  (LOC_RESOURCE_ID_2DG   +   4U)  /* Cluster Start */
#define LOC_OSAL_THREAD_DRW2D_TASK03                  (LOC_RESOURCE_ID_2DG   +   5U)  /* main_thread */
#define LOC_OSAL_THREAD_DRW2D_TASK04                  (LOC_RESOURCE_ID_2DG   +   6U)  /* MainLoop */
#define LOC_OSAL_THREAD_DRW2D_TASK05                  (LOC_RESOURCE_ID_2DG   +   7U)  /* start_thread */

#define LOC_CLUSTER_START_PRIO                        OSAL_THREAD_PRIORITY_TYPE10
#define LOC_CIO_SERVER_PRIO                           OSAL_THREAD_PRIORITY_TYPE10
#define LOC_TAURUS_PRIO                               OSAL_THREAD_PRIORITY_TYPE10


#define LOC_R_DAVEHD_THREAD_ID_CIO_SERVER             LOC_OSAL_THREAD_DRW2D_TASK00
#define LOC_CIO_SERVER_STACKSIZE    0x4000

#define LOC_R_DAVEHD_THREAD_ID_CLUSTER_START          LOC_OSAL_THREAD_DRW2D_TASK02
#define LOC_CLUSTER_START_STACKSIZE 0x4000

#define LOC_R_DAVEHD_THREAD_ID_TAURUS                 LOC_OSAL_THREAD_DRW2D_TASK01
#define LOC_TAURUS_STACKSIZE        0x4000

#define LOC_THREAD_ID_START                         (LOC_OSAL_THREAD_DRW2D_TASK05)

/* Used in app */

#define LOC_OSAL_THREAD_DRW2D_MAINTHREAD              (LOC_OSAL_THREAD_DRW2D_TASK03)  /* main_thread */

#define LOC_XMAINLOOP_STACKSIZE 0x4000
#define LOC_XMAINLOOP_PRIO      OSAL_THREAD_PRIORITY_TYPE10


#define LOC_OSAL_THREAD_DRW2D_MAINLOOP                (LOC_OSAL_THREAD_DRW2D_TASK04)  /* MainLoop */

#define LOC_COM_THREAD_STACK_SIZE                     0x4000

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef struct {
    r_wm_DisplayPort_t Port;
    uint32_t           PosX;
    uint32_t           PosY;
    uint32_t           PosZ;
    uint32_t           Width;
    uint32_t           Height;
} cluster_Config_t;

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern void Cluster_Main(const cluster_Config_t * ConfigRef);

int x2dwow_main(const cluster_Config_t * ConfigRef);

#endif /* #define X2DWOW_H */
