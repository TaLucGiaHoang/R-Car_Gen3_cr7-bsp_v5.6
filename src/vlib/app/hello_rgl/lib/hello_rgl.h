/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/
#ifndef HELLO_RGL_H
#define HELLO_RGL_H


#include "rcar-xos/osal/r_osal.h"
#include "r_wm_api.h"
#include "r_print_api.h"


#define LOC_RESOURCE_ID_2DG                         (0x9000U)

/* for OSAL resource */

/* Used in wrapper */
#define LOC_OSAL_THREAD_DRW2D_TASK00                (LOC_RESOURCE_ID_2DG   +   2U)  /* CIO Server */
#define LOC_OSAL_THREAD_DRW2D_TASK01                (LOC_RESOURCE_ID_2DG   +   3U)  /* TAURUS Server */
#define LOC_OSAL_THREAD_DRW2D_TASK02                (LOC_RESOURCE_ID_2DG   +   4U)  /* hello rgl Start */
#define LOC_OSAL_THREAD_DRW2D_TASK03                (LOC_RESOURCE_ID_2DG   +   5U)  /* main_thread */
#define LOC_OSAL_THREAD_DRW2D_TASK04                (LOC_RESOURCE_ID_2DG   +   6U)  /* MainLoop */
#define LOC_OSAL_THREAD_DRW2D_TASK05                (LOC_RESOURCE_ID_2DG   +   7U)  /* DrawThreadFunc */

#define LOC_HELLO_RGL_PRIO                          OSAL_THREAD_PRIORITY_TYPE10
#define LOC_CIO_SERVER_PRIO                         OSAL_THREAD_PRIORITY_TYPE10
#define LOC_TAURUS_PRIO                             OSAL_THREAD_PRIORITY_TYPE10

#define LOC_THREAD_ID_HELLO_RGL                     (LOC_OSAL_THREAD_DRW2D_TASK02)
#define LOC_THREAD_HELLO_RGL_STACK_SIZE             (0x4000)

#define LOC_THREAD_ID_CIO_SERVER                    (LOC_OSAL_THREAD_DRW2D_TASK00)
#define LOC_THREAD_CIO_SERVER_STACK_SIZE            (0x4000)

#define LOC_THREAD_ID_TAURUS                        (LOC_OSAL_THREAD_DRW2D_TASK01)
#define LOC_THREAD_TAURUS_STACK_SIZE                (0x4000)

/* Used in app */

#define LOC_OSAL_THREAD_DRW2D_MAINTHREAD            (LOC_OSAL_THREAD_DRW2D_TASK03)  /* main_thread */

#define LOC_MAINTHREAD_STACKSIZE                    (0x4000)
#define LOC_MAINTHREAD_PRIO                         OSAL_THREAD_PRIORITY_TYPE10

#define LOC_OSAL_THREAD_DRW2D_MAINLOOP              (LOC_OSAL_THREAD_DRW2D_TASK04)  /* MainLoop */

#define LOC_MAINLOOP_STACKSIZE                      (0x4000)
#define LOC_MAINLOOP_PRIO                           OSAL_THREAD_PRIORITY_TYPE10

#define LOC_OSAL_THREAD_DRW2D_DRWTHREAD00           (LOC_OSAL_THREAD_DRW2D_TASK05 +  0U)
#define LOC_OSAL_THREAD_DRW2D_DRWTHREAD01           (LOC_OSAL_THREAD_DRW2D_TASK05 +  1U)
#define LOC_OSAL_THREAD_DRW2D_DRWTHREAD02           (LOC_OSAL_THREAD_DRW2D_TASK05 +  2U)
#define LOC_OSAL_THREAD_DRW2D_DRWTHREAD03           (LOC_OSAL_THREAD_DRW2D_TASK05 +  3U)
#define LOC_OSAL_THREAD_DRW2D_DRWTHREAD04           (LOC_OSAL_THREAD_DRW2D_TASK05 +  4U)
#define LOC_OSAL_THREAD_DRW2D_DRWTHREAD05           (LOC_OSAL_THREAD_DRW2D_TASK05 +  5U)

#define LOC_THREAD_ID_START                         (LOC_OSAL_THREAD_DRW2D_DRWTHREAD05 + 1U) /* start_thread */

/* for VSync signal */
#define LOC_OSAL_MUTEX_SYNC_BASE                    (LOC_RESOURCE_ID_2DG + 40U)   /* OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_32 to 
                                                                                     OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_37 */
#define LOC_OSAL_MQ_SYNC_BASE                       (LOC_RESOURCE_ID_2DG + 65U)   /* OSAL_MQ_DRW2D_VSYNC_00 to 
                                                                                     OSAL_MQ_DRW2D_VSYNC_05 */

#define LOC_DRAW_PRIO                               OSAL_THREAD_PRIORITY_TYPE10
#define LOC_DRAW_STACKSIZE                          (0x4000)

#define LOC_MUTEX_ID_VOUT                           (LOC_RESOURCE_ID_2DG + 39)    /* OSAL_MUTEX_DRW2D_DEVICE_CONTEXT_31 */
#define LOC_MUTEX_TIMEOUT_MS                        (1000u) /* 1000 milisecond */


int hello_main(void);

#endif /* HELLO_RGL_H */
