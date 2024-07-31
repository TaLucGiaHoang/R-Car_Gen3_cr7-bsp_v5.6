/***********************************************************************************************************************
* IMR Driver sample program
* Copyright (c) [2018,2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/****************************************************************************
 * FILE          : imrlxsample.h
 * DESCRIPTION   : This module is the sample program to use IMR-LX4 driver.
 * CREATED       : 2018.06.15
 * MODIFIED      : -
 * AUTHOR        : Renesas Electronics Corporation
 * TARGET DEVICE : R-Car Gen3e
 * HISTORY       :
 *                 2018.06.15
 *                 - Created new.
 *                 2020.12.11
 *                 - Adapted to xOS2
 *                 2021.2.10
 *                - Fix compile warning
 *                 2021.6.14
 *                - Add macro definition and function prototype
 *
 ****************************************************************************/

/*******************************************************************************
*   Title: IMR device driver sample main
*
*   This module contains the IMR device driver sample main.
*   It must be included in any C Module using those functions
*/


#ifndef IMRLXSAMPLE_H_
#define IMRLXSAMPLE_H_

#ifdef __cplusplus
extern "C"
{
#endif



/***************************************************************************
*   Section: Defines
*/

/* Width and height of source and destination pictures. */
#define IMRSMP_SRC_WIDTH    (1280)
#define IMRSMP_SRC_HEIGHT   (720)
#define IMRSMP_SRC_STRIDE   (1280)
#define IMRSMP_DST_WIDTH    (1280)
#define IMRSMP_DST_HEIGHT   (720)
#define IMRSMP_DST_STRIDE   (1280)

#ifdef IMRSMP_YUVSEP_YONLY
#define IMRSMP_YUVSEP_SIZE  (1)
#else
#define IMRSMP_YUVSEP_SIZE  (2)
#endif

/* Number of pixels. */
#define IMRSMP_SRC_SIZE  ((IMRSMP_SRC_WIDTH * IMRSMP_SRC_HEIGHT) * IMRSMP_YUVSEP_SIZE)
#define IMRSMP_DST_SIZE  ((IMRSMP_DST_WIDTH * IMRSMP_DST_HEIGHT) * IMRSMP_YUVSEP_SIZE)

/***************************************************************************
*   Section: Struct
*/

/* memory data information for rendering */
typedef struct {
    char*                       device_name;
    uint32_t                    size_exp;
    void*                       vmr_addr_dl;
    void*                       vmr_addr_src;
    void*                       vmr_addr_dst;
    void*                       vmr_addr_exp;
    osal_memory_buffer_handle_t handle_osalbuffer_dl;
    osal_memory_buffer_handle_t handle_osalbuffer_src;
    osal_memory_buffer_handle_t handle_osalbuffer_dst;
    osal_memory_buffer_handle_t handle_osalbuffer_exp;
    st_imrdrv_attr_param_t      attr_param;
} st_mem_info_t;

/* imrdrv data information for rendering */
typedef struct {
    e_imrdrv_channelno_t channel_no;
    osal_mutex_id_t      mutex_id;
    osal_mutex_handle_t  handle_osalmutex;
    osal_cond_handle_t   handle_osalcond;
    st_mem_info_t*       p_mem_info;
} st_execute_imrdrv_arg_t;

/***************************************************************************
*   Section: Global Variables
*/
extern const uint8_t g_p2c_1280_15_yuvsep[IMRSMP_SRC_SIZE];
extern const uint8_t g_p2c_1280_15_dst_yuvsep[IMRSMP_DST_SIZE];

extern osal_mutex_id_t       mutex_id_imrdrv[MAX_MODULE];
extern osal_mutex_id_t       mutex_id_callback[MAX_MODULE];
extern osal_cond_id_t        cond_id[MAX_MODULE];
extern osal_thread_id_t      thread_id[MAX_MODULE];
extern char*                 device_name[MAX_MODULE];
extern char*                 task_name[MAX_MODULE];
extern st_execute_imrdrv_arg_t imrdrv_arg[MAX_MODULE];

/***************************************************************************
*   Section: Global Functions
*/

/***************************************************************************
*   Function: get_display_list_addr
*
*   Description:
*       Get global variable g_dl2_top_dst_yuvsep.
*
*   Parameters:
*       p_arraynum - [o]Length of g_dl2_top_dst_yuvsep[] array.
*
*   Return:
*       Global variable g_dl2_top_dst_yuvsep.
*/
extern const uint32_t * get_display_list_addr(uint32_t * const p_arraynum);

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
extern uint32_t convert_channel_to_index(e_imrdrv_channelno_t channel_no);

#ifdef __cplusplus
}
#endif


#endif

/* End of File */

