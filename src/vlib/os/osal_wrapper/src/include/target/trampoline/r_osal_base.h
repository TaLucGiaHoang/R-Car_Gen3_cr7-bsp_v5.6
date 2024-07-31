/***********************************************************************************************************************
* Copyright [2021] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
* display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
* purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
* SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
* NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
* INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
* OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
* be subject to different terms.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Description :  OSAL basic mutex functions for Trampoline (AUTOSAR)
***********************************************************************************************************************/

#ifndef R_OSAL_BASE
#define R_OSAL_BASE

#include <stdint.h>

#include "tpl_os.h"

#include "r_osal_types.h"

#include "autosar_osal.h" /* app defines osal_autosar_thread_t structure */

/* All alarms must use the same event number */
#define R_AUTORSAR_ALARM_EVENT    1

const char *R_OSAL_Sys_GetTplErrorStr(int errno);

osal_autosar_thread_t *R_OSAL_Sys_GetCurrentThreadData(void);

int R_OSAL_Sys_KickTask(int *);

int R_OSAL_Sys_KickTaskById(osal_autosar_thread_t *task_to_kick);

/* The following functions implement a mutex where the storage is allocated by
 * the caller, so we don't need an array of mutexes to hand out.
 *
 * You can initialise global mutexes to avoid race situations. When doing so,
 * you do not need to call base_mutex_create().
 * e.g. base_mutex_t mtx = R_OSAL_MUTEX_STATIC_INIT;
 */
#define R_OSAL_MUTEX_STATIC_INIT 1

typedef int base_mutex_t;

int base_mutex_create(base_mutex_t *Mutex);
int base_mutex_lock(base_mutex_t *Mutex);
int base_mutex_lock_timeout(base_mutex_t *Mutex, unsigned int Time_ms);
int base_mutex_unlock(base_mutex_t *Mutex);
int base_mutex_destroy(base_mutex_t *Mutex);
int base_mutex_is_unlocked(base_mutex_t *Mutex);


typedef int base_sema_t;

int base_sema_create(base_sema_t *sema);
int base_sema_take(base_sema_t *sema);
int base_sema_take_timeout(base_sema_t *sema, unsigned int Timeout_ms);
int base_sema_give(base_sema_t *sema);
int base_sema_get_count(base_sema_t *sema);

#endif
