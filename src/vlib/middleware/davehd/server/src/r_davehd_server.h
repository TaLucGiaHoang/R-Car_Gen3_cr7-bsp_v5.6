/*
****************************************************************************
PROJECT : VLIB (device and OS adaptation)
FILE    : $Id: r_davehd_server.h 510 2019-09-30 02:32:37Z a5110279 $
============================================================================ 
DESCRIPTION
D/AVE HD Server
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
#ifndef R_DAVEHD_SERVER_H_
#define R_DAVEHD_SERVER_H_

/*
 * Data for a single DHD instance.
 */
typedef struct tagdhd_instance_data_t {
    osal_device_handle_t          IODevHandle;
    osal_memory_manager_handle_t  MemHandle;
    osal_memory_buffer_handle_t   VidMemHandle;
    void                         *VidMemPhy;
    void                         *VidMemVirt;
    size_t                        VidMemSize;
    dhd_irq_t                     IRQCallback;
    dhd_gpu_device_data_t        *DHDDevice;
    osal_thread_handle_t          ThreadIRQ;
    osal_mq_handle_t              MqIRQ;
    uint32_t                      IrqState;
    int32_t                       IrqCount;
    bool                          MbiOccurred;
    uint32_t                      TimeOut;
} dhd_instance_data_t;

typedef struct DHDClient_tag {
    uint32_t           Function;
    int32_t            ClientIdx;
    uint8_t           *CallData;
    osal_mq_handle_t   Response;
    uint8_t           *ComBuffer;
    uint32_t           DataSize;
    uint32_t           BufferSize;
    uint8_t            num;
} DHDClient_t;

typedef enum {
    DHDSRV_ERR_OK                   = 0,
    DHDSRV_ERR_NG                   = 1,
    DHDSRV_ERR_MAX_NUM
} dhdsrv_err_t;


/***********************************************************
  Type: r_dhd_ServerInfo_t
  
  Description:
    Initialize information for DHD Server.


  Fields:
      VideoMemoryHandle - Handle for VMem
        VideoMemorySize - Size of VMem
                TimeOut - Time out value[ms].
*/
typedef struct r_dhd_ServerInfo_s
{
    dhd_handle_t    VideoMemoryHandle;
    uint32_t        VideoMemorySize;
    uint32_t        TimeOut;
} r_dhd_ServerInfo_t ;


/*******************************************************************************
  Group: D/AVE HD Server Functions

*/

/*******************************************************************************
  Function: dhd_server_main

  Start the D/AVE HD server.

  Parameter:
    void

  Return value:
    void
*/
dhd_error_t dhd_server_main(r_dhd_ServerInfo_t *InitInfo);


#endif /* R_DAVEHD_SERVER_H_ */
