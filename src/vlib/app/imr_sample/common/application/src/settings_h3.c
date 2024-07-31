/***********************************************************************************************************************
* IMR Driver sample program
* Copyright (c) [2018,2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/****************************************************************************
 * FILE          : settings_h3.c
 * DESCRIPTION   : This module is the sample program to use IMR-LX4 driver.
 * CREATED       : 2021.06.14
 * MODIFIED      : -
 * AUTHOR        : Renesas Electronics Corporation
 * TARGET DEVICE : R-Car Gen3e
 * HISTORY       :
 *                 2021.06.14
 *                 - Created new.
 ****************************************************************************/

/*******************************************************************************
*   Title: Device Dependent setting
*
*   This module contains the device dependent setting.
*/

#include "rcar-xos/imr/r_imrdrv_api.h"
#include "rcar-xos/osal/r_osal.h"

#include "imrlxsample.h"

osal_mutex_id_t         mutex_id_imrdrv[MAX_MODULE]   = {0x1000, 0x1001, 0x1002, 0x1003};
osal_mutex_id_t         mutex_id_callback[MAX_MODULE] = {0x1004, 0x1005, 0x1006, 0x1007};
osal_cond_id_t          cond_id[MAX_MODULE]   = {0x1000, 0x1001, 0x1002, 0x1003};
osal_thread_id_t        thread_id[MAX_MODULE] = {0x1000, 0x1001, 0x1002, 0x1003};
char*                   device_name[MAX_MODULE] = {"imr_00", "imr_01", "imr_02", "imr_03"};
char*                   task_name[MAX_MODULE] = {"task_imr_00", "task_imr_01", "task_imr_02", "task_imr_03"};
st_execute_imrdrv_arg_t imrdrv_arg[MAX_MODULE] = {
    {IMRDRV_CHANNELNO_0, 1, NULL, NULL, NULL},
    {IMRDRV_CHANNELNO_1, 2, NULL, NULL, NULL},
    {IMRDRV_CHANNELNO_2, 3, NULL, NULL, NULL},
    {IMRDRV_CHANNELNO_3, 4, NULL, NULL, NULL},
};

/***************************************************************************
*   Function: convert_channel_to_index
*
*   Description:
*       Convert a channel to an index.
*
*   Parameters:
*       channel_no - [i]Channel number.
*
*   Return:
*       Index
*/
uint32_t convert_channel_to_index (
    e_imrdrv_channelno_t channel_no
)
{
    uint32_t ret;

    switch (channel_no)
    {
        case IMRDRV_CHANNELNO_0:
            ret = 0U;
            break;
        case IMRDRV_CHANNELNO_1:
            ret = 1U;
            break;
        case IMRDRV_CHANNELNO_2:
            ret = 2U;
            break;
        case IMRDRV_CHANNELNO_3:
            ret = 3U;
            break;
        default:
            ret = MAX_MODULE;
            break;
    }

    return ret;
}
