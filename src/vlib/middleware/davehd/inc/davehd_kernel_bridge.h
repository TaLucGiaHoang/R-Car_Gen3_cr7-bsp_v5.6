/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Kernel callgate for user mode 
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 289 $
 *  $Date: 2018-03-15 11:17:58 +0900 (木, 15 3 2018) $
 *  $LastChangedBy: a5089413 $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#ifndef DAVEHD_KERNEL_BRIDGE_H_INCLUDED
#define DAVEHD_KERNEL_BRIDGE_H_INCLUDED

#include "davehd_types.h"
#include "davehd_settings.h"

/*----------------------------------------------------------------------------------------------------------*/
typedef void * dhd_bridge_wakeup_token;        /* token representing a bridge client to wakeup*/

/*----------------------------------------------------------------------------------------------------------*/
typedef struct tagdhd_waiting_client {    
  dhd_gpu_device_data_t   *m_client;           /* kernel mode instance handle the client is using*/
  dhd_jobid_t              m_jobid;            /* target jobid the client is waiting for*/
  dhd_uint32_t             m_jobgen;           /* traget job's generation*/
  dhd_bridge_wakeup_token  m_token;            /* passed back to platform layer to signal the waiting client*/
} dhd_waiting_client_t;

/*----------------------------------------------------------------------------------------------------------*/
typedef enum tagdhd_gpu_call_entry {
  E_DHD_CALL_NONE,
  
  E_DHD_CALL_GET_INSTANCE_COUNT,               /* Query number of initialized HW instances. (see dhd_gpu_get_instance_count)*/
  E_DHD_CALL_QUERY_INSTANCE,                   /* Query description of a specific HW instance. (see dhd_gpu_query_instance)*/
  E_DHD_CALL_INIT,                             /* Initialize kernel mode driver. (see dhd_gpu_init)*/
  E_DHD_CALL_SHUTDOWN,                         /* Shutdown the kernel mode driver. (see dhd_gpu_shutdown)*/
  E_DHD_CALL_CONNECT,                          /* Establish a connection from user mode. (see dhd_gpu_connect)*/
  E_DHD_CALL_DISCONNECT,                       /* Disconnect a user mode connection. (see dhd_gpu_disconnect)*/
  E_DHD_CALL_ALLOC_MEM,                        /* Allocate a chunk of video memory. (see dhd_gpu_alloc_mem)*/
  E_DHD_CALL_FREE_MEM,                         /* Release a chunk of video memory. (see dhd_gpu_free_mem)*/
  E_DHD_CALL_CAN_MAP_MEM,                      /* Check if a specific memory mapping is possible. (see dhd_gpu_can_map_mem)*/
  E_DHD_CALL_MAP_MEM,                          /* Create a memory mapping. (see dhd_gpu_map_mem)*/
  E_DHD_CALL_UNMAP_MEM,                        /* Release a memory mapping. (see dhd_gpu_unmap_mem)*/
  E_DHD_CALL_UPLOAD,                           /* Upload data to video memory. (see dhd_gpu_upload)*/
  E_DHD_CALL_DOWNLOAD,                         /* Download data from video memory. (see dhd_gpu_download)*/
  E_DHD_CALL_COMMIT,                           /* Register job for execution. (see dhd_gpu_commit)  */
  E_DHD_CALL_GET_SYNC,                         /* Query latest completed job and clear id. (see dhd_gpu_get_sync)*/
  E_DHD_CALL_WAIT_FOR_JOB,                     /* Wait for a job. (see dhd_gpu_wait_for_job)*/
  E_DHD_CALL_GET_RECLAIMED_JOBS,               /* Retrieve a list of free job buffers. (see dhd_gpu_get_reclaimed_jobs)*/
  E_DHD_CALL_FORCE_YIELD,                      /* Interrupt current job and reschedule. (see dhd_gpu_force_yield)*/
  E_DHD_CALL_DOWNLOAD_STATE,                   /* Retrieve HW state. (see dhd_gpu_download_state)*/
  E_DHD_CALL_TLIST_ENABLE,                     /* Start or stop TLIST recording. (see dhd_gpu_tlist_enable)*/
  E_DHD_CALL_TLIST_WRITE,                      /* Add a custom string to TLIST output. (see dhd_gpu_tlist_write)*/
  E_DHD_CALL_ALLOC_PC,                         /* Allocate a performance counter. (see dhd_gpu_alloc_pc)*/
  E_DHD_CALL_FREE_PC,                          /* Free a performance counter. (see dhd_gpu_free_pc)*/
  E_DHD_CALL_ALLOC_CLUT,                       /* Allocate a part of the CLUT RAM. (see dhd_gpu_alloc_clut)*/
  E_DHD_CALL_FREE_CLUT,                        /* Free a part of the CLUT RAM. (see dhd_gpu_free_clut)*/
  E_DHD_CALL_RESUME,                           /* HW has completed halt. Need to initiate resume. (see dhd_gpu_resume)*/
  E_DHD_CALL_CAN_ACCESS_MEM,                   /* Check if a specific area of memory can be accessed by th HW*/
  E_DHD_CALL_SCHEDULE_JOB_TRIGGER,             /* Asynchronous trigger by ISR to schedule job. */
  E_DHD_CALL_QUANTITY
} dhd_gpu_call_entry_t;

/*----------------------------------------------------------------------------------------------------------*/
dhd_uint32_t dhd_gpu_call(dhd_handle_t a_bridge, dhd_gpu_call_entry_t a_function, dhd_gpu_call_data_t *a_parameters);

/*----------------------------------------------------------------------------------------------------------*/
void dhd_gpu_trigger(dhd_handle_t a_bridge, dhd_gpu_call_entry_t a_function, dhd_gpu_call_data_t *a_parameters);
dhd_uint32_t dhd_gpu_dispatch(dhd_gpu_call_entry_t a_function, dhd_gpu_call_data_t *a_parameters);
dhd_handle_t dhd_gpu_bridge_init_client(void);
dhd_uint32_t dhd_gpu_bridge_shutdown_client(dhd_handle_t a_bridge);
void dhd_gpu_bridge_init_server(void);
void dhd_gpu_bridge_shutdown_server(void);
void dhd_gpu_bridge_init_server_internal(void);
void dhd_gpu_bridge_add_waiting(dhd_handle_t a_client, dhd_jobid_t a_jobid, dhd_bridge_wakeup_token a_token);
void dhd_gpu_bridge_wakeup(dhd_bridge_wakeup_token a_token);
void dhd_gpu_bridge_wakeup_all(void);
void dhd_gpu_bridge_sync(const dhd_gpu_device_data_t *a_device, dhd_uint32_t a_jobsync);
#ifdef RENESAS_D1X
void dhd_gpu_dispatch_loop(void);
void dhd_gpu_preprocess(dhd_gpu_call_data_t *a_parameters);
dhd_uint32_t dhd_gpu_prepare_instance(dhd_uint32_t a_instance);
#endif /* RENESAS_D1X */


#ifdef DHD_KERNEL_SOFT_IRQ
void dhd_enter_irq_lock(void);
void dhd_exit_irq_lock(void);
#endif
/*----------------------------------------------------------------------------------------------------------*/

#endif /*DAVEHD_KERNEL_BRIDGE_H_INCLUDED*/
