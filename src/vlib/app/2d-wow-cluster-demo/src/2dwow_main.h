/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

#ifndef X2DWOW_MAIN_H
#define X2DWOW_MAIN_H


#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: Main Module Interface
  
  This header contains the declaration of all  specific 
  functions and types of the main test module.
*/

/*******************************************************************************
  Section: Global Defines

  Definition of all global defines.
*/

#undef EXTERN
#ifndef MAIN_GLOBALS
#define EXTERN extern 
#else
#define EXTERN
#endif


/*******************************************************************************
  Section: Global Defines
*/

/*******************************************************************************
  Define: LOC_MAINLOOP_STACKSIZE

  Stack size of the main loop thread.
*/

#define LOC_MAINLOOP_STACKSIZE ((size_t)0x4000)

/*******************************************************************************
  Define: LOC_MAINLOOP_PRIO

  Priorities of the threads
*/

#define LOC_MAINLOOP_PRIO      (OSAL_THREAD_PRIORITY_TYPE10)
#define LOC_CPU_MEAS_PRIO      (LOC_MAINLOOP_PRIO - 1)
#define LOC_DHD_PRIO           (LOC_MAINLOOP_PRIO + 1)
#define LOC_DHD_IRQ_PRIO       (LOC_DHD_PRIO + 1)

#define LOC_MAX_DISP_HEIGHT    (720u)

/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function:ClusterError

  Central error function.

  Parameters:
  Err        - Error code

  Returns:
  void
*/


void ClusterError(int Err);
static int64_t AppLoop(void *Arg);

#ifdef __cplusplus
}
#endif

#endif /* 2DWOW_MAIN_H */

