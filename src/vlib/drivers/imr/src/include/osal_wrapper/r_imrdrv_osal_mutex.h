/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_osal_mutex.h
* Product Name : IMR Driver
* Device(s)    : R-Car H3/M3/M3N/D3/E3
* Description  : IMR Driver's sub-component public header of OSAL Wrapper Mutex
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 02.04.2021 0.01     New create
 *         : 16.04.2021 0.02     Add r_imrdrv_osal_mutex_lock() and r_imrdrv_osal_mutex_unlock()
 *********************************************************************************************************************/

#ifndef R_IMRDRV_OSAL_MUTEX_H
#define R_IMRDRV_OSAL_MUTEX_H
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "rcar-xos/imr/r_imrdrv_type.h"
#include "rcar-xos/osal/r_osal.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define IMRDRV_MUTEX_WAIT_TIME_MIN  (0)     /* The minimum wait time (msec) of locking mutex */

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef struct
{
    osal_milli_sec_t        mutex_wait_period;  /* The wait time (msec) of locking mutex */
    osal_mutex_handle_t     mutex_handle;       /* OSAL mutex handle */
} st_imrdrv_osal_mutex_area_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern e_imrdrv_errorcode_t r_imrdrv_osal_mutex_create(
    const osal_mutex_id_t mutex_id,
    const osal_milli_sec_t mutex_wait_period,
    st_imrdrv_osal_mutex_area_t *const p_osal_mutex_area
);

extern e_imrdrv_errorcode_t r_imrdrv_osal_mutex_destroy (
    st_imrdrv_osal_mutex_area_t *const p_osal_mutex_area
);

extern e_imrdrv_errorcode_t r_imrdrv_osal_mutex_lock (
    const st_imrdrv_osal_mutex_area_t *const p_osal_mutex_area
);

extern e_imrdrv_errorcode_t r_imrdrv_osal_mutex_unlock (
    const st_imrdrv_osal_mutex_area_t *const p_osal_mutex_area
);

#ifdef __cplusplus
}
#endif
#endif /* R_IMRDRV_OSAL_MUTEX_H */

