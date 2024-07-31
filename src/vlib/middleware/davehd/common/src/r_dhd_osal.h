/****************************************************************************
PROJECT : VLIB (device and OS adaptation)
FILE    : $Id: r_dhd_osal.h 287 2018-03-13 12:23:51Z a5089413 $
============================================================================ 
DESCRIPTION
Malloc wrapper 
============================================================================
                            C O P Y R I G H T                                    
============================================================================
                           Copyright (c) 2022
                                  by 
                       Renesas Electronics (Europe) GmbH. 
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
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
Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.     

****************************************************************************
*/

/****************************************************************************
  Title: r_dhd_osal.h

 Wrapper for malloc, free an realloc 

*/

#ifndef R_DHD_OSAL_H
#define R_DHD_OSAL_H

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
  Section: Includes
*/

#include "r_dhd_typedefs.h"
#include "davehd_types.h"
#include "r_osal.h"

/***********************************************************
  Section: Global Enum Types
*/

typedef enum r_dhd_OS_Error_tag {
    R_DHD_OS_ERR_OK               = 0,
    R_DHD_OS_ERR_FAIL             = 1,
    R_DHD_OS_ERR_MAX_NUM
} r_dhd_OS_Error_t;

typedef enum r_dhd_Thread_Usage_tag {
    R_DHD_THREAD_USAGE_KERNEL     = 0,
    R_DHD_THREAD_USAGE_INTERRUPT  = 1,
    R_DHD_THREAD_USAGE_MAX_NUM
} r_dhd_Thread_Usage_t;


typedef enum r_dhd_Mq_Usage_tag {
    R_DHD_MQ_USAGE_KERNEL           = 0,
    R_DHD_MQ_USAGE_INTERRUPT        = 1,
    R_DHD_MQ_USAGE_CLIENT_RESPONSE  = 2,
    R_DHD_MQ_USAGE_MAX_NUM
} r_dhd_Mq_Usage_t;


/***********************************************************
  Section: Global Types
*/

typedef struct r_dhd_videomemory_tag {
    osal_memory_manager_handle_t   MemHandle;
    osal_memory_buffer_handle_t    VidMemHandle;
    uintptr_t                      VidMemPhy;
    void                          *VidMemVirt;
    size_t                         VidMemSize;
} r_dhd_videomemory_t;


typedef struct r_dhd_OS_Signal_tag {
    osal_cond_handle_t   cond_handle;
    osal_mutex_handle_t  mtx_handle;
    uint32_t             wait_continue;
} r_dhd_OS_Signal_t;

/*******************************************************************************
  Function: dhd_sys_create_mutex
  
  Creates a mutex and store the created mutex handle into a_Mutex.

  Parameters:
  a_Mutex          - Address to store mutex handle

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_create_mutex ( osal_mutex_handle_t *a_Mutex );


/*******************************************************************************
  Function: dhd_sys_destroy_mutex
  
  Destroys a mutex.

  Parameters:
  a_Mutex          - mutex handle

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_destroy_mutex ( osal_mutex_handle_t a_Mutex );


/*******************************************************************************
  Function: dhd_sys_lock_mutex
  
  Locks a mutex.

  Parameters:
  a_Mutex          - mutex handle

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_lock_mutex ( osal_mutex_handle_t a_Mutex );


/*******************************************************************************
  Function: dhd_sys_unlock_mutex
  
  Unlocks a mutex.

  Parameters:
  a_Mutex          - mutex handle

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_unlock_mutex ( osal_mutex_handle_t a_Mutex );


/*******************************************************************************
  Function: dhd_sys_init_video_mem
  
  Initialize the management data used internally.

  Parameters:
  a_MemManager    - Memory Manager handle
  a_size          - Size of Video Memory
  a_vmem          - Address to store videomemory infomation

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_init_video_mem ( 
        osal_memory_manager_handle_t  a_MemManager, 
        size_t                        a_size, 
        r_dhd_videomemory_t          *a_vmem 
        );


/*******************************************************************************
  Function: dhd_sys_deinit_video_mem
  
  Clears the managed video memory physical address, CPU address, and size data.

  Parameters:
  a_vmem          - Pointer to videomemory infomation

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_deinit_video_mem ( 
        r_dhd_videomemory_t          *a_vmem 
        );


/*******************************************************************************
  Function: dhd_sys_create_thread
  
  Creates and starts a new thread.

  Parameters:
  a_Usage          - Usage of Thread
  a_Func           - Pointer to Function
  a_Arg            - Pointer to argument data
  a_Thread         - Address to store thread handle

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_create_thread ( 
        r_dhd_Thread_Usage_t  a_Usage, 
        p_osal_thread_func_t  a_Func, 
        void                 *a_Arg, 
        osal_thread_handle_t *a_Thread 
        );


/*******************************************************************************
  Function: dhd_sys_wait_thread
  
  Waits for the thread specified by a_Thread to terminate.
  Then, set the return value of the thread specified in a_RetVal.
  If a_RetVal is NULL, the return value of the specified thread is not set
  and waits for the thread specified by a_Thread to terminate.

  Parameters:
  a_Thread          - thread handle
  a_RetVal          - Address to store returned value

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_wait_thread (  
        osal_thread_handle_t  a_Thread, 
        int64_t              *a_RetVal 
        );


/*******************************************************************************
  Function: dhd_sys_create_condition
  
  Creates a condition and store the created condition handle into the
  cond_handle member of a_Signal structure.
  Creates a mutex and store the created mutex handle into the
  mutex_handle member of a_Signal structure.

  Parameters:
  a_Signal          - Address to store conditoin handle and mutex handle

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_create_condition ( r_dhd_OS_Signal_t *a_Signal );


/*******************************************************************************
  Function: dhd_sys_destroy_condition
  
  Destroys a condition and a mutex.

  Parameters:
  a_Signal          - Pointer to r_dhd_OS_Signal_t structure

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_destroy_condition ( r_dhd_OS_Signal_t *a_Signal );


/*******************************************************************************
  Function: dhd_sys_send_condition_signal
  
  Broadcast a signal through the condition handle.
  Notify all threads waiting for notification through the condition handle
  specified by a_Signal.

  Parameters:
  a_Signal          - Pointer to r_dhd_OS_Signal_t structure

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_send_condition_signal ( r_dhd_OS_Signal_t *a_Signal );


/*******************************************************************************
  Function: dhd_sys_receive_condition_signal
  
  Wait for a signal to be received through the condition handle.

  Parameters:
  a_Signal          - Pointer to r_dhd_OS_Signal_t structure

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_receive_condition_signal ( r_dhd_OS_Signal_t *a_Signal );


/*******************************************************************************
  Function: dhd_sys_create_message_queue
  
  Creates a message queue that suits the usage and store the
  created message queue handle into a_Mq.

  Parameters:
  a_Usage          - Usage of message queues
  a_Mq             - Address to store massage Queue handle

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_create_message_queue ( 
        r_dhd_Mq_Usage_t  a_Usage, 
        osal_mq_handle_t *a_Mq
        );


/*******************************************************************************
  Function: dhd_sys_destroy_message_queue
  
  Destroys a message queue.

  Parameters:
  a_Mq          - Massage Queue handle

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_destroy_message_queue ( osal_mq_handle_t a_Mq );


/*******************************************************************************
  Function: dhd_sys_send_message_queue
  
  Send a message through the message queue handle.

  Parameters:
  a_Mq            - Massage Queue handle
  a_Msg           - Pointer to message data
  a_Size          - Size of message data

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_send_message_queue ( 
        osal_mq_handle_t  a_Mq,
        uint8_t          *a_Msg,
        size_t            a_Size
        );


/*******************************************************************************
  Function: dhd_sys_receive_message_queue

  Wait for a message to be received through the message queue handle.

  Parameters:
  a_Mq           - Massage Queue handle
  a_Msg          - Address to store message data
  a_Size         - Size of area to store message data

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_receive_message_queue(osal_mq_handle_t  a_Mq,
                                               uint8_t          *a_Msg,
                                               size_t            a_Size
                                               );


/*******************************************************************************
  Function: dhd_sys_open_io_device

  Open a device with device_type "dhd" and store the created 
  device handle into a_Device.
  Then set the power status of the device to Ready.

  Parameters:
  a_Device       - Address to store device handle

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_open_io_device(osal_device_handle_t *a_Device);


/*******************************************************************************
  Function: dhd_sys_close_io_device

  Close a device.

  Parameters:
  a_Device       - Device handle

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_close_io_device(osal_device_handle_t a_Device);


/*******************************************************************************
  Function: dhd_sys_write_io32

  Write a value (a_Data) to register by 32bit alignment.

  Parameters:
  a_Device       - device handle
  a_Offset       - Offset from the base address of device handle by byte
  a_Data         - Data of uint32_t for write

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_write_io32(osal_device_handle_t  a_Device,
                                    uintptr_t             a_Offset,
                                    uint32_t              a_Data);


/*******************************************************************************
  Function: dhd_sys_read_io32

  Read a value from register by 32bit alignment and set 
  the read value to a_Data.

  Parameters:
  a_Device       - device handle
  a_Offset       - Offset from the base address of device handle by byte
  a_Data         - Address to store data of uint32_t

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_read_io32(osal_device_handle_t  a_Device,
                                   uintptr_t             a_Offset,
                                   uint32_t             *a_Data);


/*******************************************************************************
  Function: dhd_sys_register_isr

  Registers the interrupt handler service routine called 
  when an interrupt occurs from device.

  Parameters:
  a_Device       - device handle
  a_Channel      - Irq channel number
  a_Function     - Pointer to CallbackFunction
  a_Arg          - Pointer to argument data

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_register_isr(osal_device_handle_t  a_Device,
                                      uint64_t              a_Channel,
                                      p_osal_isr_func_t     a_Function,
                                      void                 *a_Arg
                                      );


/*******************************************************************************
  Function: dhd_sys_unregister_isr

  Unregisters the interrupt handler service routine called 
  when an interrupt occurs from device.

  Parameters:
  a_Device       - device handle
  a_Channel      - Irq channel number
  a_Function     - Pointer to CallbackFunction

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_unregister_isr(osal_device_handle_t a_Device,
                                        uint64_t             a_Channel,
                                        p_osal_isr_func_t    a_Function
                                        );


/*******************************************************************************
  Function: dhd_sys_enable_isr

  Enables the interrupt handler already registered to 
  the specified a_Channel.

  Parameters:
  a_Device       - device handle
  a_Channel      - Irq channel number

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_enable_isr(osal_device_handle_t a_Device,
                                    uint64_t             a_Channel
                                    );


/*******************************************************************************
  Function: dhd_sys_disable_isr

  Disables the interrupt handler already registered to 
  the specified a_Channel.

  Parameters:
  a_Device       - device handle
  a_Channel      - Irq channel number

  Returns:
  see: <r_dhd_OS_Error_t>

*/
r_dhd_OS_Error_t dhd_sys_disable_isr(osal_device_handle_t a_Device,
                                     uint64_t             a_Channel
                                     );


#ifdef __cplusplus
}
#endif
#endif /* R_DHD_OSAL_H*/

