/*
 * Copyright (c) 2017 Simon Goldschmidt
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Simon Goldschmidt <goldsimon@gmx.de>
 *
 */

/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"

#include "rcar-xos/osal/r_osal.h"
#include "r_print_api.h"

/** Set this to 1 if you want the stack size passed to sys_thread_new() to be
 * interpreted as number of stack words (FreeRTOS-like).
 * Default is that they are interpreted as byte count (lwIP-like).
 */
#ifndef LWIP_FREERTOS_THREAD_STACKSIZE_IS_STACKWORDS
#define LWIP_FREERTOS_THREAD_STACKSIZE_IS_STACKWORDS  0
#endif

/** Set this to 1 to use a mutex for SYS_ARCH_PROTECT() critical regions.
 * Default is 0 and locks interrupts/scheduler for SYS_ARCH_PROTECT().
 */
#ifndef LWIP_FREERTOS_SYS_ARCH_PROTECT_USES_MUTEX
#define LWIP_FREERTOS_SYS_ARCH_PROTECT_USES_MUTEX     0
#endif

/** Set this to 1 to include a sanity check that SYS_ARCH_PROTECT() and
 * SYS_ARCH_UNPROTECT() are called matching.
 */
#ifndef LWIP_FREERTOS_SYS_ARCH_PROTECT_SANITY_CHECK
#define LWIP_FREERTOS_SYS_ARCH_PROTECT_SANITY_CHECK   0
#endif

/** Set this to 1 to let sys_mbox_free check that queues are empty when freed */
#ifndef LWIP_FREERTOS_CHECK_QUEUE_EMPTY_ON_FREE
#define LWIP_FREERTOS_CHECK_QUEUE_EMPTY_ON_FREE       0
#endif

/** Set this to 1 to enable core locking check functions in this port.
 * For this to work, you'll have to define LWIP_ASSERT_CORE_LOCKED()
 * and LWIP_MARK_TCPIP_THREAD() correctly in your lwipopts.h! */
#ifndef LWIP_FREERTOS_CHECK_CORE_LOCKING
#define LWIP_FREERTOS_CHECK_CORE_LOCKING              0
#endif

/** Set this to 0 to implement sys_now() yourself, e.g. using a hw timer.
 * Default is 1, where FreeRTOS ticks are used to calculate back to ms.
 */
#ifndef LWIP_FREERTOS_SYS_NOW_FROM_FREERTOS
#define LWIP_FREERTOS_SYS_NOW_FROM_FREERTOS           1
#endif

#if SYS_LIGHTWEIGHT_PROT && LWIP_FREERTOS_SYS_ARCH_PROTECT_USES_MUTEX
static SemaphoreHandle_t sys_arch_protect_mutex;
#endif
#if SYS_LIGHTWEIGHT_PROT && LWIP_FREERTOS_SYS_ARCH_PROTECT_SANITY_CHECK
static sys_prot_t sys_arch_protect_nesting;
#endif

#define ENABLE_IRQ() asm("cpsie i\n\t")
#define DISABLE_IRQ() asm("cpsid i\n\t")

/* Initialize this module (see description in sys.h) */
void
sys_init(void)
{
#if SYS_LIGHTWEIGHT_PROT && LWIP_FREERTOS_SYS_ARCH_PROTECT_USES_MUTEX
  /* initialize sys_arch_protect global mutex */
  sys_arch_protect_mutex = xSemaphoreCreateRecursiveMutex();
  LWIP_ASSERT("failed to create sys_arch_protect mutex",
    sys_arch_protect_mutex != NULL);
#endif /* SYS_LIGHTWEIGHT_PROT && LWIP_FREERTOS_SYS_ARCH_PROTECT_USES_MUTEX */
}

#if configUSE_16_BIT_TICKS == 1
#error This port requires 32 bit ticks or timer overflow will fail
#endif

#if LWIP_FREERTOS_SYS_NOW_FROM_FREERTOS
u32_t
sys_now(void)
{
  st_osal_time_t time;
  R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_PRECISION, &time);
  return (time.tv_sec * 1000U) + (time.tv_nsec/1000000U);
}
#endif

u32_t
sys_jiffies(void)
{
  st_osal_time_t time;
  R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_PRECISION, &time);
  return (time.tv_sec * 1000000U) + (time.tv_nsec / 1000U);
}

#if SYS_LIGHTWEIGHT_PROT

sys_prot_t
sys_arch_protect(void)
{
#if LWIP_FREERTOS_SYS_ARCH_PROTECT_USES_MUTEX
  BaseType_t ret;
  LWIP_ASSERT("sys_arch_protect_mutex != NULL", sys_arch_protect_mutex != NULL);

  // ret = xSemaphoreTakeRecursive(sys_arch_protect_mutex, portMAX_DELAY);
  LWIP_ASSERT("sys_arch_protect failed to take the mutex", ret == pdTRUE);
#else /* LWIP_FREERTOS_SYS_ARCH_PROTECT_USES_MUTEX */
  DISABLE_IRQ();
#endif /* LWIP_FREERTOS_SYS_ARCH_PROTECT_USES_MUTEX */
#if LWIP_FREERTOS_SYS_ARCH_PROTECT_SANITY_CHECK
  {
    /* every nested call to sys_arch_protect() returns an increased number */
    sys_prot_t ret = sys_arch_protect_nesting;
    sys_arch_protect_nesting++;
    LWIP_ASSERT("sys_arch_protect overflow", sys_arch_protect_nesting > ret);
    return ret;
  }
#else
  return 1;
#endif
}

void
sys_arch_unprotect(sys_prot_t pval)
{
#if LWIP_FREERTOS_SYS_ARCH_PROTECT_USES_MUTEX
  BaseType_t ret;
#endif
#if LWIP_FREERTOS_SYS_ARCH_PROTECT_SANITY_CHECK
  LWIP_ASSERT("unexpected sys_arch_protect_nesting", sys_arch_protect_nesting > 0);
  sys_arch_protect_nesting--;
  LWIP_ASSERT("unexpected sys_arch_protect_nesting", sys_arch_protect_nesting == pval);
#endif

#if LWIP_FREERTOS_SYS_ARCH_PROTECT_USES_MUTEX
  LWIP_ASSERT("sys_arch_protect_mutex != NULL", sys_arch_protect_mutex != NULL);

  // ret = xSemaphoreGiveRecursive(sys_arch_protect_mutex);
  LWIP_ASSERT("sys_arch_unprotect failed to give the mutex", ret == pdTRUE);
#else /* LWIP_FREERTOS_SYS_ARCH_PROTECT_USES_MUTEX */
  ENABLE_IRQ();
#endif /* LWIP_FREERTOS_SYS_ARCH_PROTECT_USES_MUTEX */
  LWIP_UNUSED_ARG(pval);
}

#endif /* SYS_LIGHTWEIGHT_PROT */

void
sys_arch_msleep(u32_t delay_ms)
{
  (void)R_OSAL_ThreadSleepForTimePeriod((osal_milli_sec_t)delay_ms);
}

#if !LWIP_COMPAT_MUTEX
#define LWIP_OSAL_BASE_ID           (0x8200)

#define LWIP_OSAL_MUTEX_NUM         3
#define LWIP_OSAL_MUTEX_ID_0        (LWIP_OSAL_BASE_ID + 0U)
#define LWIP_OSAL_MUTEX_ID_1        (LWIP_OSAL_BASE_ID + 1U)
#define LWIP_OSAL_MUTEX_ID_2        (LWIP_OSAL_BASE_ID + 2U)
#define LWIP_OSAL_MUTEX_TIMEOUT_MS  (1000U)

#define LWIP_OSAL_SEM_NUM           3
#define LWIP_OSAL_COND_ID_0         (LWIP_OSAL_BASE_ID + 0U)
#define LWIP_OSAL_COND_ID_1         (LWIP_OSAL_BASE_ID + 1U)
#define LWIP_OSAL_COND_ID_2         (LWIP_OSAL_BASE_ID + 2U)

typedef struct {
  osal_mutex_id_t mutex_id;
  osal_mutex_handle_t mutex_handle;
} st_lwip_osal_mutex;

static st_lwip_osal_mutex lwip_osal_mutex[LWIP_OSAL_MUTEX_NUM] = {
  {LWIP_OSAL_MUTEX_ID_0, NULL},
  {LWIP_OSAL_MUTEX_ID_1, NULL},
  {LWIP_OSAL_MUTEX_ID_2, NULL},
};

typedef struct {
  osal_cond_id_t cond_id;
  osal_cond_handle_t cond_handle;
  st_lwip_osal_mutex *mutex;
  uint32_t value;
} st_lwip_osal_sem;

static st_lwip_osal_sem lwip_osal_sem[LWIP_OSAL_SEM_NUM] = {
  {LWIP_OSAL_COND_ID_0, NULL, NULL, 0},
  {LWIP_OSAL_COND_ID_1, NULL, NULL, 0},
  {LWIP_OSAL_COND_ID_2, NULL, NULL, 0},
};

/* Create a new mutex*/
err_t
sys_mutex_new(sys_mutex_t *mutex)
{
  e_osal_return_t osal_ret;
  st_lwip_osal_mutex *p_lwip_osal_mutex = NULL;
  LWIP_ASSERT("mutex != NULL", mutex != NULL);

  /* Find available mutex id/handle */
  for (int i = 0; i < LWIP_OSAL_MUTEX_NUM; i++) {
    if (lwip_osal_mutex[i].mutex_handle == NULL) {
      p_lwip_osal_mutex = &lwip_osal_mutex[i];
      break;
    }
  }

  if (NULL == p_lwip_osal_mutex) {
    R_PRINT_Log("[LWIP] %s: Failed to allocate OSAL Mutex\r\n", __func__);
    return ERR_MEM;
  }

  osal_ret = R_OSAL_ThsyncMutexCreate(p_lwip_osal_mutex->mutex_id, &p_lwip_osal_mutex->mutex_handle);
  if (OSAL_RETURN_OK != osal_ret) {
    R_PRINT_Log("[LWIP] %s: R_OSAL_ThsyncMutexCreate Failed. osal_ret=%d\r\n", __func__, osal_ret);
    return ERR_MEM;
  }
  
  mutex->mut = p_lwip_osal_mutex;
  if(mutex->mut == NULL) {
    SYS_STATS_INC(mutex.err);
    return ERR_MEM;
  }

  SYS_STATS_INC_USED(mutex);
  return ERR_OK;
}

void
sys_mutex_lock(sys_mutex_t *mutex)
{
  e_osal_return_t osal_ret;
  st_lwip_osal_mutex *p_lwip_osal_mutex = (st_lwip_osal_mutex*)mutex->mut;
  osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(p_lwip_osal_mutex->mutex_handle, LWIP_OSAL_MUTEX_TIMEOUT_MS);
  LWIP_ASSERT("failed to take the mutex", OSAL_RETURN_OK == osal_ret);
}

void
sys_mutex_unlock(sys_mutex_t *mutex)
{
  e_osal_return_t osal_ret;
  st_lwip_osal_mutex *p_lwip_osal_mutex = (st_lwip_osal_mutex*)mutex->mut;
  osal_ret = R_OSAL_ThsyncMutexUnlock(p_lwip_osal_mutex->mutex_handle);
  LWIP_ASSERT("failed to give the mutex", OSAL_RETURN_OK == osal_ret);
}

void
sys_mutex_free(sys_mutex_t *mutex)
{
  e_osal_return_t osal_ret;
  st_lwip_osal_mutex *p_lwip_osal_mutex = (st_lwip_osal_mutex*)mutex->mut;

  LWIP_ASSERT("mutex != NULL", mutex != NULL);
  LWIP_ASSERT("mutex->mut != NULL", mutex->mut != NULL);

  SYS_STATS_DEC(mutex.used);

  osal_ret = R_OSAL_ThsyncMutexDestroy(p_lwip_osal_mutex->mutex_handle);
  LWIP_ASSERT("failed to destory mutex", OSAL_RETURN_OK == osal_ret);

  p_lwip_osal_mutex->mutex_handle = NULL;
  mutex->mut = NULL;
}

#endif /* !LWIP_COMPAT_MUTEX */

err_t
sys_sem_new(sys_sem_t *sem, u8_t initial_count)
{
  st_lwip_osal_sem *p_lwip_osal_sem = NULL;
  e_osal_return_t osal_ret;
  err_t lwip_err;
  sys_mutex_t init_mut;
  LWIP_ASSERT("sem != NULL", sem != NULL);
  LWIP_ASSERT("initial_count invalid (not 0 or 1)",
    (initial_count == 0) || (initial_count == 1));

  /* Find available semaphore id/handle */
  for (int i = 0; i < LWIP_OSAL_SEM_NUM; i++) {
    if (lwip_osal_sem[i].cond_handle == NULL) {
      p_lwip_osal_sem = &lwip_osal_sem[i];
      break;
    }
  }

  if (p_lwip_osal_sem == NULL) {
    R_PRINT_Log("[LWIP] Failed to allocate OSAL Mutex\r\n");
    return ERR_MEM;
  }

  lwip_err = sys_mutex_new(&init_mut);
  if (ERR_OK != lwip_err) {
    R_PRINT_Log("[LWIP] %s: sys_mutex_new failed. ret=%d\r\n", __func__, lwip_err);
    return ERR_MEM;
  }
  p_lwip_osal_sem->mutex = init_mut.mut;

  osal_ret = R_OSAL_ThsyncCondCreate(p_lwip_osal_sem->cond_id, &p_lwip_osal_sem->cond_handle);
  if (OSAL_RETURN_OK != osal_ret) {
    R_PRINT_Log("[LWIP] %s: R_OSAL_ThsyncCondCreate failed. ret=%d\r\n", __func__, osal_ret);
    SYS_STATS_INC(sem.err);
    sys_mutex_free(&init_mut);
    return ERR_MEM;
  }

  p_lwip_osal_sem->value = initial_count;
  sem->sem = p_lwip_osal_sem;

  SYS_STATS_INC_USED(sem);

  return ERR_OK;
}

void
sys_sem_signal(sys_sem_t *sem)
{
  st_lwip_osal_sem *p_lwip_osal_sem = (st_lwip_osal_sem*)sem->sem;
  e_osal_return_t osal_ret;
  LWIP_ASSERT("sem != NULL", sem != NULL);
  LWIP_ASSERT("sem->sem != NULL", sem->sem != NULL);

  osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(p_lwip_osal_sem->mutex->mutex_handle, LWIP_OSAL_MUTEX_TIMEOUT_MS);
  if (OSAL_RETURN_OK != osal_ret) {
    R_PRINT_Log("[LWIP] %s: R_OSAL_ThsyncMutexLockForTimePeriod failed. ret=%d\r\n", __func__, osal_ret);
    return;
  }

  p_lwip_osal_sem->value++;

  osal_ret = R_OSAL_ThsyncCondSignal(p_lwip_osal_sem->cond_handle);
  if (OSAL_RETURN_OK != osal_ret) {
    R_PRINT_Log("[LWIP] %s: R_OSAL_ThsyncCondSignal failed. ret=%d\r\n", __func__, osal_ret);
  }

  R_OSAL_ThsyncMutexUnlock(p_lwip_osal_sem->mutex->mutex_handle);
}

u32_t
sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout_ms)
{
  st_lwip_osal_sem *p_lwip_osal_sem = (st_lwip_osal_sem*)sem->sem;
  e_osal_return_t osal_ret;
  LWIP_ASSERT("sem != NULL", sem != NULL);
  LWIP_ASSERT("sem->sem != NULL", sem->sem != NULL);

  osal_ret = R_OSAL_ThsyncMutexLockForTimePeriod(p_lwip_osal_sem->mutex->mutex_handle, LWIP_OSAL_MUTEX_TIMEOUT_MS);
  if (OSAL_RETURN_OK != osal_ret) {
    R_PRINT_Log("[LWIP] %s: R_OSAL_ThsyncMutexLockForTimePeriod failed. ret=%d\r\n", __func__, osal_ret);
    return 1;
  }

  if (timeout_ms == 0) {
    /* wait infinite for signal */
    while ((p_lwip_osal_sem->value <= 0) && (osal_ret == OSAL_RETURN_OK || osal_ret == OSAL_RETURN_TIME)) {
      osal_ret = R_OSAL_ThsyncCondWaitForTimePeriod(p_lwip_osal_sem->cond_handle,
                                                    p_lwip_osal_sem->mutex->mutex_handle,
                                                    LWIP_OSAL_MUTEX_TIMEOUT_MS);
    }
    LWIP_ASSERT("R_OSAL_ThsyncCondWaitForTimePeriod failed", (osal_ret == OSAL_RETURN_OK) || (osal_ret == OSAL_RETURN_TIME));
  }
  else {
    /* wait timeout for signal */
    while ((p_lwip_osal_sem->value <= 0) && (osal_ret == OSAL_RETURN_OK)) {
      osal_ret = R_OSAL_ThsyncCondWaitForTimePeriod(p_lwip_osal_sem->cond_handle,
                                                    p_lwip_osal_sem->mutex->mutex_handle,
                                                    timeout_ms);
    }
    if (osal_ret == OSAL_RETURN_TIME) {
      /* timed out */
      return SYS_ARCH_TIMEOUT;
    }
    LWIP_ASSERT("R_OSAL_ThsyncCondWaitForTimePeriod failed", osal_ret == OSAL_RETURN_OK);
  }

  p_lwip_osal_sem->value--;

  R_OSAL_ThsyncMutexUnlock(p_lwip_osal_sem->mutex->mutex_handle);

  /* Old versions of lwIP required us to return the time waited.
     This is not the case any more. Just returning != SYS_ARCH_TIMEOUT
     here is enough. */
  return 1;
}

void
sys_sem_free(sys_sem_t *sem)
{
  st_lwip_osal_sem *p_lwip_osal_sem = (st_lwip_osal_sem*)sem->sem;
  e_osal_return_t osal_ret;
  LWIP_ASSERT("sem != NULL", sem != NULL);
  LWIP_ASSERT("sem->sem != NULL", sem->sem != NULL);

  osal_ret = R_OSAL_ThsyncMutexDestroy(p_lwip_osal_sem->mutex->mutex_handle);
  LWIP_ASSERT("R_OSAL_ThsyncMutexDestroy failed", osal_ret == OSAL_RETURN_OK);
  osal_ret = R_OSAL_ThsyncCondDestroy(p_lwip_osal_sem->cond_handle);
  LWIP_ASSERT("R_OSAL_ThsyncCondDestroy failed", osal_ret == OSAL_RETURN_OK);

  p_lwip_osal_sem->mutex->mutex_handle = NULL;
  p_lwip_osal_sem->cond_handle = NULL;

  SYS_STATS_DEC(sem.used);
  sem->sem = NULL;
}

#define LWIP_OSAL_MQ_NUM            3
#define LWIP_OSAL_MQ_ID_0           (LWIP_OSAL_BASE_ID + 0U)
#define LWIP_OSAL_MQ_ID_1           (LWIP_OSAL_BASE_ID + 1U)
#define LWIP_OSAL_MQ_ID_2           (LWIP_OSAL_BASE_ID + 2U)
#define LWIP_OSAL_MQ_TIMEOUT_MS     (1000U)

typedef struct {
  osal_mq_id_t mq_id;
  osal_mq_handle_t mq_handle;
} st_lwip_osal_mbox;

static st_lwip_osal_mbox lwip_osal_mbox[LWIP_OSAL_MQ_NUM] = {
  {LWIP_OSAL_MQ_ID_0, NULL},
  {LWIP_OSAL_MQ_ID_1, NULL},
  {LWIP_OSAL_MQ_ID_2, NULL},
};

err_t
sys_mbox_new(sys_mbox_t *mbox, int size)
{
  e_osal_return_t osal_ret;
  st_lwip_osal_mbox *p_lwip_osal_mbox = NULL;
  st_osal_mq_config_t mq_config = {
    .max_num_msg = size,
    .msg_size = sizeof(void*)
  };
  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  LWIP_ASSERT("size > 0", size > 0);

  /* Find available Mq id/handle */
  for (int i = 0; i < LWIP_OSAL_MQ_NUM; i++) {
    if (lwip_osal_mbox[i].mq_handle == NULL) {
      p_lwip_osal_mbox = &lwip_osal_mbox[i];
      break;
    }
  }

  if (NULL == p_lwip_osal_mbox) {
    R_PRINT_Log("[LWIP] Failed to allocate OSAL Mq\r\n");
    return ERR_MEM;
  }

  osal_ret = R_OSAL_MqCreate(&mq_config, p_lwip_osal_mbox->mq_id, &p_lwip_osal_mbox->mq_handle);
  if (OSAL_RETURN_OK != osal_ret) {
    R_PRINT_Log("[LWIP] %s: R_OSAL_MqCreate Failed. osal_ret=%d\r\n", __func__, osal_ret);
    SYS_STATS_INC(mbox.err);
    return ERR_MEM;
  }

  mbox->mbx = p_lwip_osal_mbox;

  SYS_STATS_INC_USED(mbox);
  return ERR_OK;
}

void
sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  st_lwip_osal_mbox *p_lwip_osal_mbox = (st_lwip_osal_mbox*)mbox->mbx;
  e_osal_return_t osal_ret;
  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  LWIP_ASSERT("mbox->mbx != NULL", mbox->mbx != NULL);

  do {
    osal_ret = R_OSAL_MqSendForTimePeriod(p_lwip_osal_mbox->mq_handle, LWIP_OSAL_MQ_TIMEOUT_MS, &msg, sizeof(msg));
  } while (osal_ret == OSAL_RETURN_TIME);

  LWIP_ASSERT("mbox post failed", osal_ret == OSAL_RETURN_OK);
}

err_t
sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  st_lwip_osal_mbox *p_lwip_osal_mbox = (st_lwip_osal_mbox*)mbox->mbx;
  e_osal_return_t osal_ret;
  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  LWIP_ASSERT("mbox->mbx != NULL", mbox->mbx != NULL);

  osal_ret = R_OSAL_MqSendForTimePeriod(p_lwip_osal_mbox->mq_handle, 0, &msg, sizeof(msg));
  if (osal_ret == OSAL_RETURN_OK) {
    return ERR_OK;
  }
  else {
    LWIP_ASSERT("mbox trypost failed", osal_ret == OSAL_RETURN_TIME);
    SYS_STATS_INC(mbox.err);
    return ERR_MEM;
  }
}

err_t
sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void *msg)
{
  return sys_mbox_trypost(mbox, msg);
}

u32_t
sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout_ms)
{
  st_lwip_osal_mbox *p_lwip_osal_mbox = (st_lwip_osal_mbox*)mbox->mbx;
  e_osal_return_t osal_ret;
  void *msg_dummy;
  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  LWIP_ASSERT("mbox->mbx != NULL", mbox->mbx != NULL);

  if (!msg) {
    msg = &msg_dummy;
  }

  if (timeout_ms == 0) {
    /* wait infinite for message */
    do {
      osal_ret = R_OSAL_MqReceiveForTimePeriod(p_lwip_osal_mbox->mq_handle, LWIP_OSAL_MQ_TIMEOUT_MS, &(*msg), sizeof(*msg));
    } while (osal_ret == OSAL_RETURN_TIME);
    LWIP_ASSERT("mbox fetch failed", osal_ret == OSAL_RETURN_OK);
  }
  else {
    osal_ret = R_OSAL_MqReceiveForTimePeriod(p_lwip_osal_mbox->mq_handle, timeout_ms, &(*msg), sizeof(*msg));
    if (osal_ret == OSAL_RETURN_TIME) {
      /* timed out */
      *msg = NULL;
      return SYS_ARCH_TIMEOUT;
    }
    LWIP_ASSERT("mbox fetch failed", osal_ret == OSAL_RETURN_OK);
  }

  /* Old versions of lwIP required us to return the time waited.
     This is not the case any more. Just returning != SYS_ARCH_TIMEOUT
     here is enough. */
  return 1;
}

u32_t
sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  st_lwip_osal_mbox *p_lwip_osal_mbox = (st_lwip_osal_mbox*)mbox->mbx;
  void *msg_dummy;
  e_osal_return_t osal_ret;
  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  LWIP_ASSERT("mbox->mbx != NULL", mbox->mbx != NULL);

  if (!msg) {
    msg = &msg_dummy;
  }

  osal_ret = R_OSAL_MqReceiveForTimePeriod(p_lwip_osal_mbox->mq_handle, 0, &(*msg), sizeof(*msg));
  if (osal_ret == OSAL_RETURN_TIME) {
    *msg = NULL;
    return SYS_MBOX_EMPTY;
  }
  LWIP_ASSERT("mbox fetch failed", osal_ret == OSAL_RETURN_OK);

  /* Old versions of lwIP required us to return the time waited.
     This is not the case any more. Just returning != SYS_ARCH_TIMEOUT
     here is enough. */
  return 1;
}

void
sys_mbox_free(sys_mbox_t *mbox)
{
  st_lwip_osal_mbox *p_lwip_osal_mbox = (st_lwip_osal_mbox*)mbox->mbx;
  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  LWIP_ASSERT("mbox->mbx != NULL", mbox->mbx != NULL);

#if LWIP_FREERTOS_CHECK_QUEUE_EMPTY_ON_FREE
  {
    UBaseType_t msgs_waiting = uxQueueMessagesWaiting(mbox->mbx);
    LWIP_ASSERT("mbox quence not empty", msgs_waiting == 0);

    if (msgs_waiting != 0) {
      SYS_STATS_INC(mbox.err);
    }
  }
#endif

  R_OSAL_MqDelete(p_lwip_osal_mbox->mq_handle);
  p_lwip_osal_mbox->mq_handle = NULL;

  SYS_STATS_DEC(mbox.used);
}

#define LWIP_OSAL_THREAD_NUM          3
#define LWIP_OSAL_THREAD_ID_0         (LWIP_OSAL_BASE_ID + 0U)
#define LWIP_OSAL_THREAD_ID_1         (LWIP_OSAL_BASE_ID + 1U)
#define LWIP_OSAL_THREAD_ID_2         (LWIP_OSAL_BASE_ID + 2U)
#define LWIP_OSAL_THREAD_TIMEOUT_MS   (1000U)

typedef struct {
  osal_thread_id_t thread_id;
  osal_thread_handle_t thread_handle;
} st_lwip_osal_thread;

static st_lwip_osal_thread lwip_osal_thread[LWIP_OSAL_THREAD_NUM] = {
  {LWIP_OSAL_THREAD_ID_0, NULL},
  {LWIP_OSAL_THREAD_ID_1, NULL},
  {LWIP_OSAL_THREAD_ID_2, NULL},
};

sys_thread_t
sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
  e_osal_return_t osal_ret;
  sys_thread_t lwip_thread;
  st_lwip_osal_thread *p_lwip_osal_thread = NULL;
  st_osal_thread_config_t thread_config = {
    .task_name = name,
    .func = (p_osal_thread_func_t)thread,
    .userarg = arg,
    .stack_size = stacksize,
    .priority = prio
  };

  lwip_thread.thread_handle = NULL;

  /* Find available thread id/handle */
  for (int i = 0; i < LWIP_OSAL_THREAD_NUM; i++) {
    if (lwip_osal_thread[i].thread_handle == NULL) {
      p_lwip_osal_thread = &lwip_osal_thread[i];
      break;
    }
  }
  
  if (p_lwip_osal_thread == NULL) {
    R_PRINT_Log("[LWIP] %s: Failed to allocate OSAL Thread\r\n", __func__);
    return lwip_thread;
  }
  
  osal_ret = R_OSAL_ThreadCreate(&thread_config, p_lwip_osal_thread->thread_id, &p_lwip_osal_thread->thread_handle);
  if (OSAL_RETURN_OK != osal_ret) {
    R_PRINT_Log("[LWIP] %s: R_OSAL_ThreadCreate Failed. osal_ret=%d\r\n", __func__, osal_ret);
    return lwip_thread;
  }
  lwip_thread.thread_handle = p_lwip_osal_thread->thread_handle;

  return lwip_thread;
}

#if LWIP_NETCONN_SEM_PER_THREAD
#if configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0

sys_sem_t *
sys_arch_netconn_sem_get(void)
{
  void* ret;
  TaskHandle_t task = xTaskGetCurrentTaskHandle();
  LWIP_ASSERT("task != NULL", task != NULL);

  ret = pvTaskGetThreadLocalStoragePointer(task, 0);
  return ret;
}

void
sys_arch_netconn_sem_alloc(void)
{
  void *ret;
  TaskHandle_t task = xTaskGetCurrentTaskHandle();
  LWIP_ASSERT("task != NULL", task != NULL);

  ret = pvTaskGetThreadLocalStoragePointer(task, 0);
  if(ret == NULL) {
    sys_sem_t *sem;
    err_t err;
    /* need to allocate the memory for this semaphore */
    sem = mem_malloc(sizeof(sys_sem_t));
    LWIP_ASSERT("sem != NULL", sem != NULL);
    err = sys_sem_new(sem, 0);
    LWIP_ASSERT("err == ERR_OK", err == ERR_OK);
    LWIP_ASSERT("sem invalid", sys_sem_valid(sem));
    vTaskSetThreadLocalStoragePointer(task, 0, sem);
  }
}

void sys_arch_netconn_sem_free(void)
{
  void* ret;
  TaskHandle_t task = xTaskGetCurrentTaskHandle();
  LWIP_ASSERT("task != NULL", task != NULL);

  ret = pvTaskGetThreadLocalStoragePointer(task, 0);
  if(ret != NULL) {
    sys_sem_t *sem = ret;
    sys_sem_free(sem);
    mem_free(sem);
    vTaskSetThreadLocalStoragePointer(task, 0, NULL);
  }
}

#else /* configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 */
#error LWIP_NETCONN_SEM_PER_THREAD needs configNUM_THREAD_LOCAL_STORAGE_POINTERS
#endif /* configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 */

#endif /* LWIP_NETCONN_SEM_PER_THREAD */

#if LWIP_FREERTOS_CHECK_CORE_LOCKING
#if LWIP_TCPIP_CORE_LOCKING

/** Flag the core lock held. A counter for recursive locks. */
static u8_t lwip_core_lock_count;
static TaskHandle_t lwip_core_lock_holder_thread;

void
sys_lock_tcpip_core(void)
{
   sys_mutex_lock(&lock_tcpip_core);
   if (lwip_core_lock_count == 0) {
     lwip_core_lock_holder_thread = xTaskGetCurrentTaskHandle();
   }
   lwip_core_lock_count++;
}

void
sys_unlock_tcpip_core(void)
{
   lwip_core_lock_count--;
   if (lwip_core_lock_count == 0) {
       lwip_core_lock_holder_thread = 0;
   }
   sys_mutex_unlock(&lock_tcpip_core);
}

#endif /* LWIP_TCPIP_CORE_LOCKING */

#if !NO_SYS
static TaskHandle_t lwip_tcpip_thread;
#endif

void
sys_mark_tcpip_thread(void)
{
#if !NO_SYS
  lwip_tcpip_thread = xTaskGetCurrentTaskHandle();
#endif
}

void
sys_check_core_locking(void)
{
  /* Embedded systems should check we are NOT in an interrupt context here */
  /* E.g. core Cortex-M3/M4 ports:
         configASSERT( ( portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK ) == 0 );

     Instead, we use more generic FreeRTOS functions here, which should fail from ISR: */
  taskENTER_CRITICAL();
  taskEXIT_CRITICAL();

#if !NO_SYS
  if (lwip_tcpip_thread != 0) {
    TaskHandle_t current_thread = xTaskGetCurrentTaskHandle();

#if LWIP_TCPIP_CORE_LOCKING
    LWIP_ASSERT("Function called without core lock",
                current_thread == lwip_core_lock_holder_thread && lwip_core_lock_count > 0);
#else /* LWIP_TCPIP_CORE_LOCKING */
    LWIP_ASSERT("Function called from wrong thread", current_thread == lwip_tcpip_thread);
#endif /* LWIP_TCPIP_CORE_LOCKING */
  }
#endif /* !NO_SYS */
}

#endif /* LWIP_FREERTOS_CHECK_CORE_LOCKING*/
