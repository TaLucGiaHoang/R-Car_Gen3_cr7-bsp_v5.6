/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

#include <stdlib.h>
#include <string.h>

#include "rcar-xos/osal/r_osal.h"

#include "hello_rgl.h"
#include "hello_rgl_intern.h"

#include "r_config_drw2d.h"
#include "r_drw2d_types.h"
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_drw2d_ctx_dhd.h"

#include "vout.h"

#define ALLOC_SIZE           (1024u * 1024u * 32u)


//static osal_memory_manager_handle_t    MemHandle = 0;

int GpuInit(void)
{
    r_dhd_ServerInfo_t dhdInitInfo;

    /* Start DHD Server. */
    dhdInitInfo.VideoMemoryHandle = (dhd_handle_t)VoutGetMemhandle();
    dhdInitInfo.VideoMemorySize   = ALLOC_SIZE;
    dhdInitInfo.TimeOut           = LOC_MUTEX_TIMEOUT_MS;

    dhd_server_main(&dhdInitInfo);

    return 0;
}

int GpuDeinit(void)
{
    //int x;

    /* TODO: kill DHD threads */
    //x = locDaveHDDeInit(0);
    return 0;
}
