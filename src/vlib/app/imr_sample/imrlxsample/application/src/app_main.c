/***********************************************************************************************************************
* IMR Driver sample program
* Copyright (c) [2018,2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/****************************************************************************
 * FILE          : main.c
 * DESCRIPTION   : This module is the sample program to use IMR-LX4 driver.
 * CREATED       : 2018.06.15
 * MODIFIED      :
 * AUTHOR        : Renesas Electronics Corporation
 * TARGET DEVICE : R-Car Gen3e
 * HISTORY       :
 *                 2018.06.15
 *                 - Created new.
 *                 2020.05.18
 *                 -Adapted to OSAL API Rev.0.73.
 *                 2020.12.11
 *                 -Adapted to xOS2
 *                 2021.2.10
 *                - Fix compile warning
 *                 2021.4.14
 *                - Change parameter for R_OSAL_MmngrOpen
 *                - Handling return value
 *                 2021.5.6
 *                - IMR Channel execute parallel
 *                 2021.5.12
 *                - Sample app support multi target device
 *                 2021.6.14
 *                - Adapted to IMR Driver Ver.2.2.0
 *                - Remove unnecessary macro definition
 *                - Modify macro definition value
 *                - Modify check item for checking destination data in execute_imrdrv()
 *                - Modify callback_func() to check details_code.
 *
 ****************************************************************************/


/*******************************************************************************
*   Title: IMR device driver sample main
*
*   This module contains the IMR device driver sample main.
*/


/***************************************************************************
*   Section: Includes
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "r_prr_api.h"
#include "rcar-xos/imr/r_imrdrv_api.h"
#include "rcar-xos/osal/r_osal.h"
#include "imr_sample.h"
#include "imrlxsample.h"

/***************************************************************************
*   Section: Defines
*/

/** Unchangeable definitions of sample application **/
#define DATA_SIZE_ALIGN (256U) /* Alignment of data size */
#define DL_SIZE_ALIGN   (64U)  /* Alignment of DL size */
#define TIMEOUT         (3000U) /* msec */

#define OSAL_MUTEX_WAIT (10000U) /* msec */

#define IMR_SIZEOF_DL_OPERAND (4U) /* Size fo IMR DL Operand */
#define IMR_DL_SIZE(d)  ((d) * IMR_SIZEOF_DL_OPERAND)



/** Changeable definitions of sample application **/
#define RENDERING_LOOP_NUM   (3) /* The number of IMR module for execute rendering */

#define IMRSMP_MAINLOOP_STACKSIZE (0x4000U)
#define IMRSMP_MAINLOOP_PRIO      (OSAL_THREAD_PRIORITY_TYPE8)


#define IMRSMP_PUT(str)         do {\
                                    (void)R_OSAL_ThsyncMutexLockForTimePeriod(mutex_print, 0xFFFFFFFFu); \
                                    puts(str); fflush(stdout);                              \
                                    (void)R_OSAL_ThsyncMutexUnlock(mutex_print);            \
                                } while(0)
#define IMRSMP_PRINTF(fmt, ...) do {\
                                    (void)R_OSAL_ThsyncMutexLockForTimePeriod(mutex_print, 0xFFFFFFFFu); \
                                    printf(fmt, __VA_ARGS__); fflush(stdout);               \
                                    (void)R_OSAL_ThsyncMutexUnlock(mutex_print);            \
                                } while(0)

#if 0 /* Enable when debugging the rendering process. */
#define IMRSMP_DEBUG_PRINTF(fmt, ...)   do {\
                                            (void)R_OSAL_ThsyncMutexLockForTimePeriod(mutex_print, 0xFFFFFFFFu); \
                                            printf(fmt, __VA_ARGS__); fflush(stdout);               \
                                            (void)R_OSAL_ThsyncMutexUnlock(mutex_print);            \
                                        } while(0)
#else
#define IMRSMP_DEBUG_PRINTF(fmt, ...)
#endif

static osal_mutex_handle_t mutex_print;

/***************************************************************************
*   Section: Local Functions Prototypes
*/
static int64_t execute_imrdrv (
    void* args
);
static int get_rendering_module_num (
    void
);
static bool execute_rendering (
    e_imrdrv_channelno_t channel_no,
    osal_cond_handle_t handle_osalcond,
    osal_mutex_handle_t handle_osalmutex,
    imrdrv_ctrl_handle_t* p_ctrhandle,
    st_mem_info_t *p_mem_info
);
static bool allocate_set_data_memory (
    osal_memory_manager_handle_t handle_osalmmngr,
    int rendering_module_num,
    st_mem_info_t mem_info[]
);
static int deallocate_data_memory (
    osal_memory_manager_handle_t handle_osalmmngr,
    int rendering_module_num,
    st_mem_info_t mem_info[]
);
static int32_t callback_func (
    const e_imrdrv_errorcode_t ret_code,
    const uint32_t details_code,
    void *const p_callback_args
);

/***************************************************************************
*   Section: Global Functions
*/

/***************************************************************************
*   Function: main
*/
int imr_main(void)
{
    bool                         ret_set_memory = false;
    int                          i;
    int                          ret = 0;
    int                          ret2 = 0;
    int64_t                      ret3 = 0;
    e_osal_return_t              ret_osal = OSAL_RETURN_OK;
    e_osal_return_t              ret_mmngr = OSAL_RETURN_OK;
    e_osal_return_t              ret_osal_init = OSAL_RETURN_OK;
    st_mem_info_t                mem_info[MAX_MODULE];
    st_osal_thread_config_t      thread_config[MAX_MODULE];
    st_osal_mmngr_config_t       osal_mmngr_config;
    osal_memory_manager_handle_t handle_osalmmngr = OSAL_MEMORY_MANAGER_HANDLE_INVALID;
    osal_mutex_handle_t          handle_osalmutex[MAX_MODULE];
    osal_cond_handle_t           handle_osalcond[MAX_MODULE];
    osal_thread_handle_t         handle_osalthread[MAX_MODULE];
    int                          rendering_module_num = 0;

    printf("===START imr_main===\n");
    fflush(stdout);
    ret_osal = R_OSAL_ThsyncMutexCreate(0x1009U, &mutex_print);
    if (ret_osal != OSAL_RETURN_OK)
    {
        printf("[RESULT]<FAIL> Failed R_OSAL_ThsyncMutexCreate mutex_id=%d, ret=%d\n", 0x1009U, ret_osal);
        ret = 1;
    }

#if 0 /* For CR */
    /* For CR, R_OSAL_Initialize is called in main function. */
    /* initialize OSAL */
    ret_osal_init = R_OSAL_Initialize();
    if (ret_osal_init != OSAL_RETURN_OK)
    {
        ret = 1;
        printf("[RESULT]<FAIL> Failed R_OSAL_Initialize ret=%d\n", ret_osal_init);
        fflush(stdout);
    }
#endif /* For CR */

    rendering_module_num = get_rendering_module_num();

    if (ret_osal == OSAL_RETURN_OK)
    {
        for (i = 0; i < rendering_module_num; i++)
        {
            /* create Cond */
            ret_osal = R_OSAL_ThsyncCondCreate(cond_id[i], &handle_osalcond[i]);
            if (ret_osal != OSAL_RETURN_OK)
            {
                ret = 1;
                IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_ThsyncCondCreate cond_id=%ld, ret=%d\n", (uint32_t)cond_id[i], ret_osal);
            }
        }
    }

    if (ret_osal == OSAL_RETURN_OK)
    {
        for (i = 0; i < rendering_module_num; i++)
        {
            ret_osal = R_OSAL_ThsyncMutexCreate(mutex_id_callback[i], &handle_osalmutex[i]);
            if (ret_osal != OSAL_RETURN_OK)
            {
                ret = 1;
                IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_ThsyncMutexCreate mutex_id=%ld, ret=%d\n", (uint32_t)mutex_id_callback[i], ret_osal);
            }
        }
    }

    if (ret_osal == OSAL_RETURN_OK)
    {
        /* get OSAL MMGR config */
        ret_mmngr = R_OSAL_MmngrGetOsalMaxConfig(&osal_mmngr_config);
        if (ret_mmngr != OSAL_RETURN_OK)
        {
            ret = 1;
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrGetOsalMaxConfig ret=%d\n", ret_mmngr);
        }
        else
        {
            /* open OSAL MMGR */
            ret_mmngr = R_OSAL_MmngrOpen(&osal_mmngr_config, &handle_osalmmngr);
            if (ret_mmngr != OSAL_RETURN_OK)
            {
                ret = 1;
                ret_osal = ret_mmngr;
                IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrOpen ret=%d\n", ret_mmngr);
            }
            else
            {
                for (i = 0; i < rendering_module_num; i++)
                {
                    mem_info[i].device_name = device_name[i];
                }
                /* allocate and set memory */
                ret_set_memory = allocate_set_data_memory(handle_osalmmngr, rendering_module_num, mem_info);
                if (ret_set_memory == false)
                {
                    ret = 1;
                }
            }
        }
    }

    if ((ret_osal == OSAL_RETURN_OK) && (ret_set_memory == true))
    {
        /* render start */
        for (i = 0; i < rendering_module_num; i++)
        {
            imrdrv_arg[i].handle_osalcond = handle_osalcond[i];
            imrdrv_arg[i].mutex_id         = mutex_id_imrdrv[i];
            imrdrv_arg[i].handle_osalmutex = handle_osalmutex[i];
            imrdrv_arg[i].p_mem_info = &mem_info[i];

            ret_osal = R_OSAL_ThreadInitializeThreadConfigSt(&thread_config[i]);
            if (ret_osal != OSAL_RETURN_OK)
            {
                ret = 1;
                IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_ThreadInitializeThreadConfigSt ret=%d\n", ret_osal);
            }
            else
            {
                thread_config[i].func       = &execute_imrdrv;
                thread_config[i].userarg    = (void*)&imrdrv_arg[i];
                thread_config[i].stack_size = IMRSMP_MAINLOOP_STACKSIZE;
                thread_config[i].task_name  = task_name[i];
                thread_config[i].priority   = IMRSMP_MAINLOOP_PRIO;
                ret_osal = R_OSAL_ThreadCreate(&thread_config[i], thread_id[i], &handle_osalthread[i]);
                if (ret_osal != OSAL_RETURN_OK)
                {
                    ret = 1;
                    IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_ThreadCreate ret=%d\n", ret_osal);
                }
            }
        }
        for (i = 0; i < rendering_module_num; i++)
        {
            if (handle_osalthread[i] != NULL)
            {
                ret_osal = R_OSAL_ThreadJoin(handle_osalthread[i], &ret3);
                if ((ret_osal != OSAL_RETURN_OK) || (ret3 != 0))
                {
                    ret = 1;
                    IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_ThreadJoin ret=%d\n", ret_osal);
                }
            }
        }
    }

    /* deallocate memory */
    ret2 = deallocate_data_memory(handle_osalmmngr, rendering_module_num, mem_info);
    if (ret2 != 0)
    {
        ret = 1;
    }

    if (ret_mmngr != OSAL_RETURN_OK)
    {
        /* close OSAL MMGR */
        ret_osal = R_OSAL_MmngrClose(handle_osalmmngr);
        if (ret_osal != OSAL_RETURN_OK)
        {
            ret = 1;
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrClose ret=%d\n", ret_osal);
        }
    }

    for (i = 0; i < rendering_module_num; i++)
    {
        /* destroy Mutex */
        ret_osal = R_OSAL_ThsyncMutexDestroy(handle_osalmutex[i]);
        if (ret_osal != OSAL_RETURN_OK)
        {
            ret = 1;
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_ThsyncMutexDestroy ret=%d\n", ret_osal);
        }
        /* destroy Cond */
        ret_osal = R_OSAL_ThsyncCondDestroy(handle_osalcond[i]);
        if (ret_osal != OSAL_RETURN_OK)
        {
            ret = 1;
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_ThsyncCondDestroy ret=%d\n", ret_osal);
        }
    }

    (void)R_OSAL_ThsyncMutexDestroy(mutex_print);

#if 0 /* For CR */
    /* For CR, R_OSAL_Deinitialize is called in main function. */
    if (OSAL_RETURN_OK == ret_osal_init)
    {
        /* deinitialize OSAL */
        ret_osal = R_OSAL_Deinitialize();
        if (ret_osal != OSAL_RETURN_OK)
        {
            ret = 1;
            printf("[RESULT]<FAIL> Failed R_OSAL_Deinitialize ret=%d\n", ret_osal);
        }
    }
#endif

    printf("imrlxsample result = %d (OK:0, NG:else)\n", ret);
    return ret;
}


/***************************************************************************
*   Section: Local Functions
*/

/***************************************************************************
*   Function: get_rendering_module_num
*
*   Description:
*       Adaptive detection of the actual number of IMR rendering modules
*       based on the PRR register value. For now supports H3/M3/M3N/E3.
*
*   Parameters:
*       none
*
*   Return:
*       n  - number of IMR rendering modules, in case of error the
*            MAX_MODULE is returned.
*/
static int get_rendering_module_num (void)
{
    int rendering_module_num = MAX_MODULE;

    r_prr_DevName_t dev = R_PRR_GetDevice();
    switch (dev)
    {
    case R_PRR_RCARE3:
        rendering_module_num = 1;
        printf("E3 is detected. Number of IMR modules is: %d.\n", rendering_module_num);
        break;
    case R_PRR_RCARH3:
        rendering_module_num = 4;
        printf("H3 is detected. Number of IMR modules is: %d.\n", rendering_module_num);
        break;
    case R_PRR_RCARM3W:
        rendering_module_num = 2;
        printf("M3-W is detected. Number of IMR modules is: %d.\n", rendering_module_num);
        break;
    case R_PRR_RCARM3N:
        rendering_module_num = 2;
        printf("M3-N is detected. Number of IMR modules is: %d.\n", rendering_module_num);
        break;
    default:
        printf("Unknown SiP! Use default number or IMR modules: %d.\n", rendering_module_num);
        break;
    }

    return rendering_module_num;
}

/***************************************************************************
*   Function: execute_imrdrv
*
*   Description:
*       Call IMR Driver API for rendering.
*
*   Parameters:
*       args             - [i,o] Address of imrdrv data information structure
*
*   Return:
*       0  - Success
*       1  - Failed
*/
static int64_t execute_imrdrv (
    void *args
)
{
    int                   ret = 0;
    e_osal_return_t       ret_osal;
    e_imrdrv_errorcode_t  ret_imr;
    bool                  ret_rendering;
    int32_t               compare;
    uint32_t              ctrldata[IMRDRV_SIZE_WORKAREA];
    st_imrdrv_initdata_t  initdata;
    imrdrv_ctrl_handle_t  ctrhandle;
    st_imrdrv_os_config_t os_config;
    uint32_t              src_y_phys_addr;
    uint32_t              dst_y_phys_addr;
#ifndef IMRSMP_YUVSEP_YONLY
    uint32_t              src_uv_phys_addr;
    uint32_t              dst_uv_phys_addr;
#endif /* IMRSMP_YUVSEP_YONLY */
    int32_t               loop;

    e_imrdrv_channelno_t channel_no       = ((st_execute_imrdrv_arg_t *)args)->channel_no;
    st_mem_info_t        mem_info         = *((st_execute_imrdrv_arg_t *)args)->p_mem_info;
    osal_mutex_handle_t  handle_osalmutex = ((st_execute_imrdrv_arg_t *)args)->handle_osalmutex;
    osal_cond_handle_t   handle_osalcond  = ((st_execute_imrdrv_arg_t *)args)->handle_osalcond;
    osal_mutex_id_t      mutex_id_imrdrv  = ((st_execute_imrdrv_arg_t *)args)->mutex_id;

    IMRSMP_PRINTF("[%d]%s start.\n", channel_no, __func__);

    /* === Initialize device drivers. === */
    initdata.p_work_addr        = ctrldata;
    initdata.work_size          = IMRDRV_SIZE_WORKAREA;
    initdata.channel_no         = channel_no;
    os_config.mutex_id          = mutex_id_imrdrv;
    os_config.mutex_wait_period = OSAL_MUTEX_WAIT;
    os_config.dev_irq_priority  = OSAL_INTERRUPT_PRIORITY_TYPE10;

    ret_imr = R_IMRDRV_Init(&initdata, &os_config, callback_func, (void *)&handle_osalcond, &ctrhandle);
    if (ret_imr != IMRDRV_ERROR_OK)
    {
        IMRSMP_PRINTF("[RESULT]<FAIL> [%d]Failed R_IMRDRV_Init, ret=%d\n", channel_no, ret_imr);
        return 1;
    }
    IMRSMP_PRINTF("[%d]Succeed R_IMRDRV_Init\n", channel_no);

    /* === Setup IMR device driver. === */
    ret_imr = R_IMRDRV_Start(ctrhandle);
    if (ret_imr != IMRDRV_ERROR_OK)
    {
        IMRSMP_PRINTF("[RESULT]<FAIL> [%d]Failed R_IMRDRV_Start, ret=%d\n", channel_no, ret_imr);
        return 1;
    }
    IMRSMP_PRINTF("[%d]Succeed R_IMRDRV_Start\n", channel_no);

    src_y_phys_addr  = mem_info.attr_param.src_data.phys_addr;
    dst_y_phys_addr  = mem_info.attr_param.dst_data.phys_addr;
#ifndef IMRSMP_YUVSEP_YONLY
    src_uv_phys_addr = mem_info.attr_param.src_data.phys_addr +
                       (mem_info.attr_param.src_data.stride * mem_info.attr_param.src_data.height);
    dst_uv_phys_addr = mem_info.attr_param.dst_data.phys_addr +
                       (mem_info.attr_param.dst_data.stride * mem_info.attr_param.dst_data.height);
#endif
    IMRSMP_PRINTF("[%d]Rendering start. loop count:%2d\n", channel_no, RENDERING_LOOP_NUM);
    for (loop = 0; loop < RENDERING_LOOP_NUM; loop++)
    {
        /* === Execute Y data rendering === */
        IMRSMP_DEBUG_PRINTF("[%d]Execute Y data rendering\n", channel_no);

        mem_info.attr_param.triangle_mode.uvshfval = IMRDRV_MODE_OFF;
        mem_info.attr_param.triangle_mode.shfval = IMRDRV_MODE_OFF;
        mem_info.attr_param.triangle_mode.uvshfe = IMRDRV_MODE_OFF;
        mem_info.attr_param.triangle_mode.shfe = IMRDRV_MODE_OFF;
        mem_info.attr_param.triangle_mode.rde = IMRDRV_MODE_OFF;
        mem_info.attr_param.triangle_mode.tfe = IMRDRV_MODE_OFF;
        mem_info.attr_param.triangle_mode.tcm = IMRDRV_MODE_OFF;
        mem_info.attr_param.triangle_mode.autosg = IMRDRV_MODE_OFF;
        mem_info.attr_param.triangle_mode.autodg = IMRDRV_MODE_OFF;
        mem_info.attr_param.triangle_mode.bfe = IMRDRV_MODE_ON;
        mem_info.attr_param.triangle_mode.tme = IMRDRV_MODE_ON;

        mem_info.attr_param.src_data.color_format = IMRDRV_COLOR_FORM_SEP_Y;
        mem_info.attr_param.src_data.phys_addr    = src_y_phys_addr;
        mem_info.attr_param.dst_data.color_format = IMRDRV_COLOR_FORM_SEP_Y;
        mem_info.attr_param.dst_data.phys_addr    = dst_y_phys_addr;

        ret_rendering = execute_rendering(channel_no, handle_osalcond, handle_osalmutex, &ctrhandle, &mem_info);
        if (ret_rendering != true)
        {
            return 1;
        }
#ifndef IMRSMP_YUVSEP_YONLY
        /* === Execute UV data rendering === */
        IMRSMP_DEBUG_PRINTF("[%d]Execute UV data rendering\n", channel_no);

        mem_info.attr_param.src_data.color_format = IMRDRV_COLOR_FORM_SEP_UV;
        mem_info.attr_param.src_data.phys_addr    = src_uv_phys_addr;
        mem_info.attr_param.dst_data.color_format = IMRDRV_COLOR_FORM_SEP_UV;
        mem_info.attr_param.dst_data.phys_addr    = dst_uv_phys_addr;

        ret_rendering = execute_rendering(channel_no, handle_osalcond, handle_osalmutex, &ctrhandle, &mem_info);
        if (ret_rendering != true)
        {
            return 1;
        }
#endif /* IMRSMP_YUVSEP_YONLY */
    }

    ret_osal = R_OSAL_MmngrInvalidate(mem_info.handle_osalbuffer_dst, 0, mem_info.size_exp);
    if (ret_osal != OSAL_RETURN_OK)
    {
        IMRSMP_PRINTF("[RESULT]<FAIL> [%d]Failed R_OSAL_MmngrInvalidate\n", ret_osal);
        return 1;
    }

    /* === Check destination data. === */
    /* The cast is no problem because VmrStartAddr member is memory address. */
    compare = memcmp((void *)(mem_info.vmr_addr_dst), (void *)(mem_info.vmr_addr_exp), mem_info.size_exp);
    /* Output result. */
    if ((compare == 0) && (mem_info.size_exp != 0U))
    {
        IMRSMP_PRINTF("[%d]Rendering successful completion.\n", channel_no);
    }
    else
    {
        IMRSMP_PRINTF("[%d]Rendering error.\n", channel_no);
        ret = 1;
    }

    /* === Close IMR device driver. === */
    ret_imr = R_IMRDRV_Stop(ctrhandle);
    if (ret_imr != IMRDRV_ERROR_OK)
    {
        IMRSMP_PRINTF("[RESULT]<FAIL> [%d]Failed R_IMRDRV_Stop, ret=%d\n", channel_no, ret_imr);
        return 1;
    }
    IMRSMP_PRINTF("[%d]Succeed R_IMRDRV_Stop\n", channel_no);

    /* === Deinitialize. === */
    ret_imr = R_IMRDRV_Quit(ctrhandle);
    if (ret_imr != IMRDRV_ERROR_OK)
    {
        IMRSMP_PRINTF("[RESULT]<FAIL> [%d]Failed R_IMRDRV_Quit, ret=%d\n", channel_no, ret_imr);
        ret = 1;
    }
    else
    {
        IMRSMP_PRINTF("[%d]Succeed R_IMRDRV_Quit\n", channel_no);
    }

    return ret;
}

/***************************************************************************
*   Function: execute_rendering
*
*   Description:
*       Execute rendering.
*
*   Parameters:
*       channel_no       - [i]  Channel number
*       handle_osalcond  - [i]  OSAL condition variable handle.
*       handle_osalmutex - [i]  OSAL mutex handle.
*       p_ctrhandle      - [i]  IMR Driver handle.
*       p_mem_info       - [i]  Address of memory data information structure.
*
*   Return:
*       true  - Success.
*       false - Failure.
*/
static bool execute_rendering(
    e_imrdrv_channelno_t channel_no,
    osal_cond_handle_t handle_osalcond,
    osal_mutex_handle_t handle_osalmutex,
    imrdrv_ctrl_handle_t *p_ctrhandle,
    st_mem_info_t *p_mem_info
)
{
    e_osal_return_t      ret_osal;
    e_imrdrv_errorcode_t ret_imr;

    ret_imr = R_IMRDRV_AttrSetParam(*p_ctrhandle, &(p_mem_info->attr_param));
    if (ret_imr != IMRDRV_ERROR_OK)
    {
        IMRSMP_PRINTF("[RESULT]<FAIL> [%d]Failed R_IMRDRV_AttrSetParam, ret=%d\n", channel_no, ret_imr);
        return false;
    }
    IMRSMP_DEBUG_PRINTF("[%d]Succeed R_IMRDRV_AttrSetParam\n", channel_no);

    ret_imr = R_IMRDRV_Execute(*p_ctrhandle);
    if (ret_imr != IMRDRV_ERROR_OK)
    {
        IMRSMP_PRINTF("[RESULT]<FAIL> [%d]Failed R_IMRDRV_Execute ret=%d\n", channel_no, ret_imr);
        return false;
    }
    IMRSMP_DEBUG_PRINTF("[%d]Succeed R_IMRDRV_Execute\n", channel_no);

    ret_osal = R_OSAL_ThsyncMutexTryLock(handle_osalmutex);
    if (ret_osal != OSAL_RETURN_OK)
    {
        IMRSMP_PRINTF("[RESULT]<FAIL> [%d]Failed R_OSAL_ThsyncMutexTryLock ret=%d\n", channel_no, ret_osal);
        return false;
    }

    ret_osal = R_OSAL_ThsyncCondWaitForTimePeriod(handle_osalcond, handle_osalmutex, (osal_milli_sec_t)TIMEOUT);
    if (ret_osal != OSAL_RETURN_OK)
    {
        if (ret_osal == OSAL_RETURN_TIME)
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> [%d]Failed(TIMEOUT) R_OSAL_ThsyncCondWaitForTimePeriod ret=%d\n", channel_no, ret_osal);
            exit(1);
        }
        else
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> [%d]Failed R_OSAL_ThsyncCondWaitForTimePeriod ret=%d\n", channel_no, ret_osal);
            return false;
        }
    }
    else
    {
        IMRSMP_DEBUG_PRINTF("[%d]Rendering completed\n", channel_no);
    }

    ret_osal = R_OSAL_ThsyncMutexUnlock(handle_osalmutex);
    if (ret_osal != OSAL_RETURN_OK)
    {
        IMRSMP_PRINTF("[RESULT]<FAIL> [%d]Failed R_OSAL_ThsyncMutexUnlock ret=%d\n", channel_no, ret_osal);
        return false;
    }

    return true;
}

/***************************************************************************
*   Function: allocate_set_data_memory
*
*   Description:
*       Allocate memory for the following data by using OSAL API and set data.
*          Display List
*          Source Data
*          Destination Data
*          Expected Destination Data
*
*   Parameters:
*       handle_osalmmngr     - [i]OSAL MMNGR handle.
*       rendering_module_num - [i]Number of IMR modules for execute rendering.
*       mem_info             - [o]Address of memory data information structure array.
*
*   Return:
*       true  - Success.
*       false - Failure.
*/
static bool allocate_set_data_memory (
    osal_memory_manager_handle_t handle_osalmmngr,
    int rendering_module_num,
    st_mem_info_t mem_info[]
)
{
    e_osal_return_t   ret_osal;
    int32_t           i;
    uint32_t          dl_num;
    size_t            size;
    uint32_t          size_src;
    uint32_t          size_dst;
    const uint32_t *  p_dl_top;
    osal_axi_bus_id_t axi_id[rendering_module_num];

    if (handle_osalmmngr == OSAL_MEMORY_MANAGER_HANDLE_INVALID)
    {
        IMRSMP_PUT("[RESULT]<FAIL> Failed allocate_set_data_memory() handle_osalmmngr == NULL\n");
        return false;
    }

    /* === Get axi id === */
    for (i = 0; i < rendering_module_num; i++)
    {
        ret_osal = R_OSAL_IoGetAxiBusIdFromDeviceName(mem_info[i].device_name, &axi_id[i]);
        if (ret_osal != OSAL_RETURN_OK)
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_IoGetAxiBusIdFromDeviceName ret=%d\n", ret_osal);
            return false;
        }
    }

    /* === Set display list. === */
    p_dl_top = get_display_list_addr(&dl_num);
    for (i = 0; i < rendering_module_num; i++)
    {
        uint32_t data_size = (IMR_DL_SIZE(dl_num) + DL_SIZE_ALIGN - 1) & (~(DL_SIZE_ALIGN - 1));
        ret_osal = R_OSAL_MmngrAlloc(handle_osalmmngr, data_size, DL_SIZE_ALIGN, &(mem_info[i].handle_osalbuffer_dl));
        if (ret_osal == OSAL_RETURN_OK)
        {
            ret_osal = R_OSAL_MmngrGetCpuPtr(mem_info[i].handle_osalbuffer_dl, &(mem_info[i].vmr_addr_dl));
            if (ret_osal != OSAL_RETURN_OK)
            {
                IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrGetCpuPtr(dl) ret=%d\n", ret_osal);
                return false;
            }
        }
        else
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrAlloc(dl) ret=%d\n", ret_osal);
            return false;
        }
        ret_osal = R_OSAL_MmngrGetHwAddr(mem_info[i].handle_osalbuffer_dl, axi_id[i],
                                         (uintptr_t *)&(mem_info[i].attr_param.dl_data.phys_addr));
        if (ret_osal != OSAL_RETURN_OK)
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrGetHwAddr(dl) ret=%d\n", ret_osal);
            return false;
        }

        /* The cast is no problem because VmrStartAddr member is memory address. */
        (void)memcpy((void *)mem_info[i].vmr_addr_dl, p_dl_top, data_size);
        ret_osal = R_OSAL_MmngrGetSize(mem_info[i].handle_osalbuffer_dl, &size);
        if (ret_osal != OSAL_RETURN_OK)
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrGetSize(dl) ret=%d\n", ret_osal);
            return false;
        }
        ret_osal = R_OSAL_MmngrFlush(mem_info[i].handle_osalbuffer_dl, 0, size);
        if (ret_osal != OSAL_RETURN_OK)
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrFlush(dl) ret=%d\n", ret_osal);
            return false;
        }
    }

    /* === Set source picture. === */
    for (i = 0; i < rendering_module_num; i++)
    {
        size_src = IMRSMP_SRC_SIZE;
        ret_osal = R_OSAL_MmngrAlloc(handle_osalmmngr, size_src, DATA_SIZE_ALIGN, &(mem_info[i].handle_osalbuffer_src));
        if (ret_osal == OSAL_RETURN_OK)
        {
            ret_osal = R_OSAL_MmngrGetCpuPtr(mem_info[i].handle_osalbuffer_src, &(mem_info[i].vmr_addr_src));
            if (ret_osal != OSAL_RETURN_OK)
            {
                IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrGetCpuPtr(src) ret=%d\n", ret_osal);
                return false;
            }
        }
        else
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrAlloc(src) ret=%d\n", ret_osal);
            return false;
        }
        ret_osal = R_OSAL_MmngrGetHwAddr(mem_info[i].handle_osalbuffer_src, axi_id[i],
                                         (uintptr_t *)&(mem_info[i].attr_param.src_data.phys_addr));
        if (ret_osal != OSAL_RETURN_OK)
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrGetHwAddr(src) ret=%d\n", ret_osal);
            return false;
        }
        mem_info[i].attr_param.src_data.height       = IMRSMP_SRC_HEIGHT;
        mem_info[i].attr_param.src_data.width        = IMRSMP_SRC_WIDTH;
        mem_info[i].attr_param.src_data.bpp          = IMRDRV_BPP_8;
        mem_info[i].attr_param.src_data.stride       = IMRSMP_SRC_STRIDE;
        mem_info[i].attr_param.src_data.color_format = IMRDRV_COLOR_FORM_SEP_Y;

        /* The cast is no problem because VmrStartAddr member is memory address. */
        (void)memcpy((void *)mem_info[i].vmr_addr_src, g_p2c_1280_15_yuvsep, size_src);
        ret_osal = R_OSAL_MmngrGetSize(mem_info[i].handle_osalbuffer_src, &size);
        if (ret_osal != OSAL_RETURN_OK)
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrGetSize(src) ret=%d\n", ret_osal);
            return false;
        }
        ret_osal = R_OSAL_MmngrFlush(mem_info[i].handle_osalbuffer_src, 0, size);
        if (ret_osal != OSAL_RETURN_OK)
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrFlush(src) ret=%d\n", ret_osal);
            return false;
        }
    }

    /* === Zero-clear destination area. === */
    for (i = 0; i < rendering_module_num; i++)
    {
        size_dst = IMRSMP_DST_SIZE;
        ret_osal = R_OSAL_MmngrAlloc(handle_osalmmngr, size_dst, DATA_SIZE_ALIGN, &(mem_info[i].handle_osalbuffer_dst));
        if (ret_osal == OSAL_RETURN_OK)
        {
            ret_osal = R_OSAL_MmngrGetCpuPtr(mem_info[i].handle_osalbuffer_dst, &(mem_info[i].vmr_addr_dst));
            if (ret_osal != OSAL_RETURN_OK)
            {
                IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrGetCpuPtr(dst) ret=%d\n", ret_osal);
                return false;
            }
        }
        else
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrAlloc(dst) ret=%d\n", ret_osal);
            return false;
        }
        ret_osal = R_OSAL_MmngrGetHwAddr(mem_info[i].handle_osalbuffer_dst, axi_id[i],
                                         (uintptr_t *)&(mem_info[i].attr_param.dst_data.phys_addr));
        if (ret_osal != OSAL_RETURN_OK)
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrGetHwAddr(dst) ret=%d\n", ret_osal);
            return false;
        }
        mem_info[i].attr_param.dst_data.height       = IMRSMP_DST_HEIGHT;
        mem_info[i].attr_param.dst_data.width        = IMRSMP_DST_WIDTH;
        mem_info[i].attr_param.dst_data.bpp          = IMRDRV_BPP_8;
        mem_info[i].attr_param.dst_data.stride       = IMRSMP_DST_STRIDE;
        mem_info[i].attr_param.dst_data.color_format = IMRDRV_COLOR_FORM_SEP_Y;

        /* The cast is no problem because VmrStartAddr member is memory address. */
        (void)memset((void *)mem_info[i].vmr_addr_dst, 0, size_dst);
        ret_osal = R_OSAL_MmngrGetSize(mem_info[i].handle_osalbuffer_dst, &size);
        if (ret_osal != OSAL_RETURN_OK)
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrGetSize(dst) ret=%d\n", ret_osal);
            return false;
        }
        ret_osal = R_OSAL_MmngrFlush(mem_info[i].handle_osalbuffer_dst, 0, size);
        if (ret_osal != OSAL_RETURN_OK)
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrFlush(dst) ret=%d\n", ret_osal);
            return false;
        }
        ret_osal = R_OSAL_MmngrInvalidate(mem_info[i].handle_osalbuffer_dst, 0, size);
        if (ret_osal != OSAL_RETURN_OK)
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrInvalidate(dst) ret=%d\n", ret_osal);
            return false;
        }
    }

    /* === Set expected destination data. === */
    for (i = 0; i < rendering_module_num; i++)
    {
        mem_info[i].size_exp = size_dst;
        ret_osal = R_OSAL_MmngrAlloc(handle_osalmmngr, size_dst, DATA_SIZE_ALIGN, &(mem_info[i].handle_osalbuffer_exp));
        if (ret_osal == OSAL_RETURN_OK)
        {
            ret_osal = R_OSAL_MmngrGetCpuPtr(mem_info[i].handle_osalbuffer_exp, &(mem_info[i].vmr_addr_exp));
            if (ret_osal != OSAL_RETURN_OK)
            {
                IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrGetCpuPtr(exp) ret=%d\n", ret_osal);
                return false;
            }
        }
        else
        {
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrAlloc(exp) ret=%d\n", ret_osal);
            return false;
        }
        /* The cast is no problem because VmrStartAddr member is memory address. */
        (void)memcpy((void *)mem_info[i].vmr_addr_exp, g_p2c_1280_15_dst_yuvsep, mem_info[i].size_exp);
    }

    return true;
}

/***************************************************************************
*   Function: deallocate_data_memory
*
*   Description:
*       Deallocate memory for the following data by using OSAL API.
*          Display List
*          Source Data
*          Destination Data
*          Expected Destination Data
*
*   Parameters:
*       handle_osalmmngr     - [i]OSAL MMNGR handle.
*       rendering_module_num - [i]Number of IMR modules for execute rendering.
*       mem_info             - [i]Address of memory data information structure array.
*
*   Return:
*       0  - Success
*       1  - Failed
*/
static int deallocate_data_memory(osal_memory_manager_handle_t handle_osalmmngr,
                                   int rendering_module_num,
                                   st_mem_info_t mem_info[])
{
    e_osal_return_t         ret_osal;
    int32_t                 i;
    int                     ret = 0;

    if ( handle_osalmmngr == OSAL_MEMORY_MANAGER_HANDLE_INVALID )
    {
        IMRSMP_PUT("[RESULT]<FAIL> Failed deallocate_data_memory() handle_osalmmngr == NULL\n");
        ret = 1;
    }

    for (i = rendering_module_num - 1; i >= 0; i--)
    {
        if ( mem_info[i].handle_osalbuffer_exp != OSAL_MEMORY_BUFFER_HANDLE_INVALID  )
        {
            ret_osal = R_OSAL_MmngrDealloc(handle_osalmmngr, mem_info[i].handle_osalbuffer_exp);
            if (ret_osal != OSAL_RETURN_OK)
            {
                IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrDealloc(exp) no=%ld ret=%d\n", i, ret_osal);
                ret = 1;
            }
        }
    }

    for (i = rendering_module_num - 1; i >= 0; i--)
    {
        if ( mem_info[i].handle_osalbuffer_dst != OSAL_MEMORY_BUFFER_HANDLE_INVALID  )
        {
            ret_osal = R_OSAL_MmngrDealloc(handle_osalmmngr, mem_info[i].handle_osalbuffer_dst);
            if (ret_osal != OSAL_RETURN_OK)
            {
                IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrDealloc(dst) no=%ld ret=%d\n", i, ret_osal);
                ret = 1;
            }
        }
    }

    for (i = rendering_module_num - 1; i >= 0; i--)
    {
        if ( mem_info[i].handle_osalbuffer_src != OSAL_MEMORY_BUFFER_HANDLE_INVALID  )
        {
            ret_osal = R_OSAL_MmngrDealloc(handle_osalmmngr, mem_info[i].handle_osalbuffer_src);
            if (ret_osal != OSAL_RETURN_OK)
            {
                IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrDealloc(src) no=%ld ret=%d\n", i, ret_osal);
                ret = 1;
            }
        }
    }

    for (i = rendering_module_num - 1; i >= 0; i--)
    {
        if ( mem_info[i].handle_osalbuffer_dl != OSAL_MEMORY_BUFFER_HANDLE_INVALID  )
        {
            ret_osal = R_OSAL_MmngrDealloc(handle_osalmmngr, mem_info[i].handle_osalbuffer_dl);
            if (ret_osal != OSAL_RETURN_OK)
            {
                IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_MmngrDealloc(dl) no=%ld ret=%d\n", i, ret_osal);
                ret = 1;
            }
        }
    }

    return ret;
}


/***************************************************************************
*   Function: callback_func
*
*   Description:
*       Callback function.
*
*   Parameters:
*       ret_code         - [i]The result of callback processing.
*       details_code     - [i]The factor of the interrupt.
*       p_callback_args  - [i]The pointer of argument of callback function.
*
*   Return:
*       0  - Success
*       1  - Failed
*
*/
static int32_t callback_func (
    const e_imrdrv_errorcode_t ret_code,
    const uint32_t details_code,
    void *const p_callback_args
)
{
    e_osal_return_t   ret_osal;
    int32_t           ret = 0;
    osal_cond_handle_t *p_handl_osalcond;
    p_handl_osalcond = (osal_cond_handle_t *)p_callback_args;

    if ( ret_code == IMRDRV_ERROR_OK )
    {
        /* Check the factor of the interrupt */
        if (IMRDRV_SR_TRA == details_code)
        {
            /* TRA interrupt occurs only. Rendering finished successfully in this case */
            /* Do nothing */
        }
        else
        {
            /* Error occurs in redering, so please execute system reset */
        }

        ret_osal = R_OSAL_ThsyncCondSignal(*p_handl_osalcond);
        if (ret_osal != OSAL_RETURN_OK)
        {
            ret = 1;
            IMRSMP_PRINTF("[RESULT]<FAIL> Failed R_OSAL_ThsyncCondSignal ret=%d\n", ret_osal);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        IMRSMP_PRINTF("callback_func received error code =%d\n", ret_code);
    }

    return ret;
}

/* End of File */
