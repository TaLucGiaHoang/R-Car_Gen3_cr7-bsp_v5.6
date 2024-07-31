/*
============================================================================
DESCRIPTION
OS Application layer for Autosar
============================================================================
                            C O P Y R I G H T
============================================================================
                      Copyright (c) 2019 - 2021
                                  by
                       Renesas Electronics (Europe) GmbH.
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
Purpose: to be used as sample S/W

DISCLAIMER
This software is supplied by Renesas Electronics Corporation and is only
intended for use with Renesas products. No other uses are authorized. This
software is owned by Renesas Electronics Corporation and is protected under
all applicable laws, including copyright laws.
THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
Renesas reserves the right, without notice, to make changes to this software
and to discontinue the availability of this software. By using this software,
you agree to the additional terms and conditions found by accessing the
following link:
http://www.renesas.com/disclaimer *
Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
*/

#ifndef R_AUTOSAR_OSAL_H_
#define R_AUTOSAR_OSAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tpl_os.h"

#define R_MAX_TASK_NAME_LEN 16

typedef struct {
    const TaskType *AutosarTask_id;
    const AlarmType *AutosarTask_SleepAlarm;
    uint8_t Priority;       /* Task priority */
    char Name[R_MAX_TASK_NAME_LEN];       /* Task name */
    void *(*Task)(void *);  /* Function to run for the task */
    void *Arg;              /* Argument passed to the Task function */
    void *Ret;              /* Return value from the Task function */
    int *WaitingItem;       /* The mutex, semaphore or queue that Task is waiting on */
} osal_autosar_thread_t;


osal_autosar_thread_t *R_OSAL_Sys_GetThread(int index);

int R_OSAL_Sys_GetNrThreads(void);

#ifdef __cplusplus
}
#endif

#endif /* R_AUTOSAR_OSAL_H_ */
