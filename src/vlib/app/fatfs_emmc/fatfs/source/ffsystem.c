/*------------------------------------------------------------------------*/
/* A Sample Code of User Provided OS Dependent Functions for FatFs        */
/*------------------------------------------------------------------------*/

#include "ff.h"


#if FF_USE_LFN == 3	/* Use dynamic memory allocation */

/*------------------------------------------------------------------------*/
/* Allocate/Free a Memory Block                                           */
/*------------------------------------------------------------------------*/

#include <stdlib.h>		/* with POSIX API */
#include "r_osal_api.h"

extern osal_memory_manager_handle_t g_hndl_mmngr;

#define TABLE_SIZE 32

typedef struct {
	uint32_t key;
	uint32_t value;
} hash_node_t;

static hash_node_t hash_table[TABLE_SIZE];
static uint32_t hash_table_num_nodes = 0;

static uint32_t hash(uint32_t key) {
	uint32_t hash_value = 5381;
	uint8_t *key_bytes = (uint8_t *)&key;
	for (size_t i = 0; i < sizeof(key); i++) {
		hash_value = ((hash_value << 5) + hash_value) + key_bytes[i];
	}
	return hash_value;
}

static int hash_table_insert(uint32_t key, uint32_t value) {
	uint32_t hash_value = hash(key) % TABLE_SIZE;
	if (hash_table_num_nodes >= TABLE_SIZE)
		return 0;
	while (hash_table[hash_value].key != 0) {
		hash_value = (hash_value + 1) % TABLE_SIZE;
	}
	hash_table[hash_value].key = key;
	hash_table[hash_value].value = value;
	hash_table_num_nodes++;
	return 1;
}

static uint32_t hash_table_lookup(uint32_t key) {
	uint32_t hash_value = hash(key) % TABLE_SIZE;
	while (hash_table[hash_value].key != key) {
		if (hash_table[hash_value].key == 0) {
			return 0; // Key not found
		}
		hash_value = (hash_value + 1) % TABLE_SIZE;
	}
	return hash_table[hash_value].value;
}

void hash_table_remove(uint32_t key) {
	uint32_t hash_value = hash(key) % TABLE_SIZE;
	while (hash_table[hash_value].key != key) {
		if (hash_table[hash_value].key == 0) {
			return; // Key not found
		}
		hash_value = (hash_value + 1) % TABLE_SIZE;
	}
	hash_table[hash_value].key = 0;
	hash_table_num_nodes--;
}

void* ff_memalloc (	/* Returns pointer to the allocated memory block (null if not enough core) */
	UINT msize		/* Number of bytes to allocate */
)
{
	osal_memory_buffer_handle_t handle;
	void *cpuPTR;
	msize = (msize&0x3f) ? (msize+0x3f) & ~0x3f : msize;
	if (R_OSAL_MmngrAlloc(g_hndl_mmngr, msize, 128, &handle) != OSAL_RETURN_OK)
		return NULL;
	if (R_OSAL_MmngrGetCpuPtr(handle, &cpuPTR) != OSAL_RETURN_OK)
		return NULL;
	if (!hash_table_insert((uint32_t)cpuPTR, (uint32_t)handle)) {
		R_OSAL_MmngrDealloc(g_hndl_mmngr, handle);
		return NULL;
	}
	return cpuPTR;
}


void ff_memfree (
	void* mblock	/* Pointer to the memory block to free (no effect if null) */
)
{
	osal_memory_buffer_handle_t handle;
	handle = (osal_memory_buffer_handle_t) hash_table_lookup((uint32_t)mblock);
	if (handle == NULL)
		return;
	hash_table_remove((uint32_t)mblock);
	R_OSAL_MmngrDealloc(g_hndl_mmngr, handle);
}

#endif




#if FF_FS_REENTRANT	/* Mutal exclusion */
/*------------------------------------------------------------------------*/
/* Definitions of Mutex                                                   */
/*------------------------------------------------------------------------*/

#define OS_TYPE	0	/* 0:Win32, 1:uITRON4.0, 2:uC/OS-II, 3:FreeRTOS, 4:CMSIS-RTOS */


#if   OS_TYPE == 0	/* Win32 */
#include <windows.h>
static HANDLE Mutex[FF_VOLUMES + 1];	/* Table of mutex handle */

#elif OS_TYPE == 1	/* uITRON */
#include "itron.h"
#include "kernel.h"
static mtxid Mutex[FF_VOLUMES + 1];		/* Table of mutex ID */

#elif OS_TYPE == 2	/* uc/OS-II */
#include "includes.h"
static OS_EVENT *Mutex[FF_VOLUMES + 1];	/* Table of mutex pinter */

#elif OS_TYPE == 3	/* FreeRTOS */
#include "FreeRTOS.h"
#include "semphr.h"
static SemaphoreHandle_t Mutex[FF_VOLUMES + 1];	/* Table of mutex handle */

#elif OS_TYPE == 4	/* CMSIS-RTOS */
#include "cmsis_os.h"
static osMutexId Mutex[FF_VOLUMES + 1];	/* Table of mutex ID */

#endif



/*------------------------------------------------------------------------*/
/* Create a Mutex                                                         */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount function to create a new mutex
/  or semaphore for the volume. When a 0 is returned, the f_mount function
/  fails with FR_INT_ERR.
*/

int ff_mutex_create (	/* Returns 1:Function succeeded or 0:Could not create the mutex */
	int vol				/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	Mutex[vol] = CreateMutex(NULL, FALSE, NULL);
	return (int)(Mutex[vol] != INVALID_HANDLE_VALUE);

#elif OS_TYPE == 1	/* uITRON */
	T_CMTX cmtx = {TA_TPRI,1};

	Mutex[vol] = acre_mtx(&cmtx);
	return (int)(Mutex[vol] > 0);

#elif OS_TYPE == 2	/* uC/OS-II */
	OS_ERR err;

	Mutex[vol] = OSMutexCreate(0, &err);
	return (int)(err == OS_NO_ERR);

#elif OS_TYPE == 3	/* FreeRTOS */
	Mutex[vol] = xSemaphoreCreateMutex();
	return (int)(Mutex[vol] != NULL);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	osMutexDef(cmsis_os_mutex);

	Mutex[vol] = osMutexCreate(osMutex(cmsis_os_mutex));
	return (int)(Mutex[vol] != NULL);

#endif
}


/*------------------------------------------------------------------------*/
/* Delete a Mutex                                                         */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount function to delete a mutex or
/  semaphore of the volume created with ff_mutex_create function.
*/

void ff_mutex_delete (	/* Returns 1:Function succeeded or 0:Could not delete due to an error */
	int vol				/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	CloseHandle(Mutex[vol]);

#elif OS_TYPE == 1	/* uITRON */
	del_mtx(Mutex[vol]);

#elif OS_TYPE == 2	/* uC/OS-II */
	OS_ERR err;

	OSMutexDel(Mutex[vol], OS_DEL_ALWAYS, &err);

#elif OS_TYPE == 3	/* FreeRTOS */
	vSemaphoreDelete(Mutex[vol]);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	osMutexDelete(Mutex[vol]);

#endif
}


/*------------------------------------------------------------------------*/
/* Request a Grant to Access the Volume                                   */
/*------------------------------------------------------------------------*/
/* This function is called on enter file functions to lock the volume.
/  When a 0 is returned, the file function fails with FR_TIMEOUT.
*/

int ff_mutex_take (	/* Returns 1:Succeeded or 0:Timeout */
	int vol			/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	return (int)(WaitForSingleObject(Mutex[vol], FF_FS_TIMEOUT) == WAIT_OBJECT_0);

#elif OS_TYPE == 1	/* uITRON */
	return (int)(tloc_mtx(Mutex[vol], FF_FS_TIMEOUT) == E_OK);

#elif OS_TYPE == 2	/* uC/OS-II */
	OS_ERR err;

	OSMutexPend(Mutex[vol], FF_FS_TIMEOUT, &err));
	return (int)(err == OS_NO_ERR);

#elif OS_TYPE == 3	/* FreeRTOS */
	return (int)(xSemaphoreTake(Mutex[vol], FF_FS_TIMEOUT) == pdTRUE);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	return (int)(osMutexWait(Mutex[vol], FF_FS_TIMEOUT) == osOK);

#endif
}



/*------------------------------------------------------------------------*/
/* Release a Grant to Access the Volume                                   */
/*------------------------------------------------------------------------*/
/* This function is called on leave file functions to unlock the volume.
*/

void ff_mutex_give (
	int vol			/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	ReleaseMutex(Mutex[vol]);

#elif OS_TYPE == 1	/* uITRON */
	unl_mtx(Mutex[vol]);

#elif OS_TYPE == 2	/* uC/OS-II */
	OSMutexPost(Mutex[vol]);

#elif OS_TYPE == 3	/* FreeRTOS */
	xSemaphoreGive(Mutex[vol]);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	osMutexRelease(Mutex[vol]);

#endif
}

#endif	/* FF_FS_REENTRANT */

