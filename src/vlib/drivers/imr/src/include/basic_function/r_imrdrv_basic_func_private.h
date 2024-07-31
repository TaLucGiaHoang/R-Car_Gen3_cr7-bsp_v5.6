/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_basic_func_private.h
* Product Name : IMR Driver
* Device(s)    : R-Car H3/M3/M3N/D3/E3
* Description  : IMR Driver Basic Function APIs Private header
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.04.2021 0.01     New create
 *********************************************************************************************************************/

#ifndef R_IMRDRV_BASIC_FUNC_PRIVATE_H
#define R_IMRDRV_BASIC_FUNC_PRIVATE_H
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "rcar-xos/imr/r_imrdrv_type.h"
#include "r_imrdrv_basic_mainctl.h"
#include "r_imrdrv_basic_dev.h"
#include "r_imrdrv_state_manage.h"
#include "r_imrdrv_osal_io.h"
#include "r_imrdrv_osal_mutex.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef struct
{
    p_imrdrv_callback_func_t    callback_func_app;      /* Callback function to User Application. */
    void                        *p_callback_arg;        /* Argument of the callback function to User Application. */
} st_imrdrv_basic_api_area_t;

typedef struct st_imrdrv_ctrl_handle_t
{
    st_imrdrv_basic_api_area_t      basic_api_area;     /* Basic Function APIs area. */
    st_imrdrv_basic_mainctl_area_t  basic_mainctl_area; /* Basic Function Main Control area. */
    st_imrdrv_basic_device_area_t   basic_device_area;  /* Basic Function Device Dependent Control area */
    st_imrdrv_state_manage_area_t   state_manage_area;  /* State Management area. */
    st_imrdrv_osal_io_area_t        osal_io_area;       /* OSAL Wrapper IO area. */
    st_imrdrv_osal_mutex_area_t     osal_mutex_area;    /* OSAL Wrapper Mutex area. */
} st_imrdrv_work_area_data_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* R_IMRDRV_BASIC_FUNC_PRIVATE_H */

