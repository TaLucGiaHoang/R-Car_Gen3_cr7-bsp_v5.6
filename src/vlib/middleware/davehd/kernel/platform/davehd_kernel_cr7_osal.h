/****************************************************************************
PROJECT : VLIB (device and OS adaptation)
FILE    : $Id: davehd_kernel_cr7_osal.h 287 2018-03-13 12:23:51Z a5089413 $
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
Purpose: only for testing

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
Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.     

****************************************************************************
*/

/****************************************************************************
  Title: davehd_kernel_cr7_osal.h

 osal based code for DaveHD.

*/

#ifndef DAVEHD_KERNEL_CR7_OSAL_H
#define DAVEHD_KERNEL_CR7_OSAL_H

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
  Section: Includes
*/
#include "r_osal.h"
#include "davehd_kernel_driver.h"
#include "davehd_kernel_hw.h"
#include "r_davehd_server.h"


/***********************************************************
  Section: Global Enum Types
*/
#define R_DAVEHD_IRQ_CHANNEL_STOP   (0u)
#define R_DAVEHD_IRQ_CHANNEL_ERR    (1u)
#define R_DAVEHD_IRQ_CHANNEL_SYNC   (2u)

typedef enum dhd_gpu_interrupt_status_tag {
    R_DHD_INT_STATUS_PROCESS_IRQ,
    R_DHD_INT_STATUS_MBI_OCCURRED, 
    R_DHD_INT_STATUS_LAST
} dhd_gpu_interrupt_status_t;

typedef enum tagdhd_gpu_interrupt_fuctor_y {
    R_DHD_INTERUPT_FACTOR_ERROR,
    R_DHD_INTERUPT_FACTOR_STOP, 
    R_DHD_INTERUPT_FACTOR_SYNC,
    R_DHD_INTERUPT_FACTOR_LAST
} dhd_gpu_interrupt_fuctor_t;


/***********************************************************
  Section: Global Types
*/

typedef struct tagdhd_sys_svr_instance_t {
    osal_device_handle_t          mtx_handle;
    osal_memory_manager_handle_t  cond_handle;
    osal_memory_buffer_handle_t   thread_handle;
} dhd_sys_svr_instance_t;


/*******************************************************************************
  Section: Function
*/


/*******************************************************************************
  Function: dhd_gpu_send_message

  Build a message from the argument information. Send the message.

  Parameters:
  a_function - GPU Call function
       a_idx - Index of Client instance
        a_mq - Message queue handle
    a_buffer - pointer to buffer of  GPU Call data
      a_size - Size of GPU Call data

  Returns:
  See <dhd_uint32_t>
*/
dhd_uint32_t dhd_gpu_send_message(
        dhd_gpu_call_entry_t  a_function,
        int32_t               a_idx,
        osal_mq_handle_t      a_mq,
        uint8_t              *a_buffer,
        uint32_t              a_size);


/*******************************************************************************
  Function: dhd_gpu_recieve_message

  Recieve a message and set data to the client instance.

  Parameters:
    a_client_tbl - Pointer of DHD Client instance table
  num_of_clients - Client number

  Returns:
  Pointer of DHD Client instance
*/
DHDClient_t * dhd_gpu_recieve_message(
        DHDClient_t *a_client_tbl,
        uint32_t     num_of_clients);


/*******************************************************************************
  Function: dhd_gpu_send_response

  Build a response message from the argument information. Send the response message.

  Parameters:
      a_mq - Message queue handle
  a_result - Result code
  a_buffer - Pointer to buffer of GPU Call response data
    a_size - Size of GPU Call response data

  Returns:
  See <dhd_uint32_t>
*/
dhd_uint32_t dhd_gpu_send_response(
        osal_mq_handle_t  a_mq,
        uint32_t          a_result,
        uint8_t          *a_buffer,
        uint32_t          a_size);


/***********************************************************
  Function: dhd_gpu_recieve_response

  Recieve the responce from the dhd_gpu_send_message.


  Parameters:
      a_mq - Message queue handle
  a_buffer - Pointer to buffer of response data
    a_size - Size of response data

  Returns:
  If successful, return E_DHD_OK.
*/
dhd_uint32_t dhd_gpu_recieve_response(
        osal_mq_handle_t  a_mq,
        uint8_t          *a_buffer,
        uint32_t          a_size);


/***********************************************************
  Function: dhd_gpu_send_interrupt

  Send a message from the interrupt.

  Parameters:
      a_mq - Message queue handle
  a_factor - Interrupt factor

  Returns:
  If successful, return E_DHD_OK.
*/
int32_t dhd_gpu_send_interrupt(
        osal_mq_handle_t   a_mq,
        uint32_t           a_factor
        );


/***********************************************************
  Function: dhd_gpu_recieve_interrupt

  Recieve a message from the interrupt.

  Parameters:
      a_mq - Message queue handle
  a_factor - Interrupt factor

  Returns:
  If successful, return E_DHD_OK.
*/
int32_t dhd_gpu_recieve_interrupt(
        osal_mq_handle_t  a_mq,
        uint32_t         *a_factor
        );


/***********************************************************
  Function: dhd_gpu_iodev_read_status

  Read GPU status.

  Parameters:
      inst - DHD instance
  a_Status - Status of GPU interrupt
     p_val - Pointer to store status

  Returns:
  Always return E_DHD_OK.
*/
int32_t dhd_gpu_iodev_read_status(
        dhd_instance_data_t        *inst,
        dhd_gpu_interrupt_status_t  a_Status,
        void                       *p_val
        );

/***********************************************************
  Function: dhd_gpu_iodev_clear_status

  Clear GPU status.

  Parameters:
      inst - DHD instance
  a_Status - Status of GPU interrupt

  Returns:
  If successful, return E_DHD_OK.
*/
int32_t dhd_gpu_iodev_clear_status(
        dhd_instance_data_t        *inst,
        dhd_gpu_interrupt_status_t  a_Status
        );


#ifdef __cplusplus
}
#endif
#endif /* DAVEHD_KERNEL_CR7_OSAL_H*/

