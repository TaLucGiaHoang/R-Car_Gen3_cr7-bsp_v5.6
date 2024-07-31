/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/


/*
   Title: Cluster Application Module
 */

#include <stdio.h>
#include <stdint.h>

#include "davehd_driver.h"
#include "davehd_util.h"

#include "r_config_drw2d.h"
#include "r_drw2d_types.h"
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_drw2d_ctx_dhd.h"

#include "demo.h"

#include "img_format.h"

#include "font.h"
#include "app_cluster_image.h"
#include "app_cluster.h"
#include "app_cl_carmodel.h"

#include "app_cl_gauge.h"

#include "error.h"
#include "romfs_api.h"

#include "davehd_types.h"
#include "davehd_driver.h"

#define LOC_ORIG_DEMO_FRAMERATE 60

extern const eeRomFs_Data_t RomFileSystemData[];

extern unsigned int avgtime;


/*
   Test application render function.

   The function renders the different components of the demo.

   Parameters:
   handle - device pointer
 */
void RenderAppCluster(davehd_dev_t *Dev)
{
    static uint32_t u32Timer = 0;
    uint32_t u32Update;


    RenderClGaugeScreen(Dev);

#ifdef WIN32
    u32Update = (1000 / avgtime) / LOC_ORIG_DEMO_FRAMERATE;
#else
    u32Update = 0;
#endif
    if ((u32Update == 0) || (u32Timer++ % (u32Update) == 0 )) {
        CarModelUpdate();
        u32Timer = 1;
    }
}

/*
   Test application init function.

   The function creates the required surfaces and contextes.
   It informs the generic D2D framework to use the application's
   drawing function and surface.

   Parameters:
   handle - device pointer
 */
void InitAppCluster(davehd_dev_t *Dev)
{

    /* Init ROMFS */
    eeRomFs_Init(&RomFileSystemData[0]);

    GaugeScreenInit(Dev);

    InitCarModel();
}

/*
   Test application deinit function.

   The function releases all created resources. It informs
   the generic D2D framework not to use the application's
   surface or drawing function anymore.

   Parameters:
   Dev - device pointer
 */
void DeInitAppCluster(davehd_dev_t *Dev)
{
    DeInitCarModel();

    GaugeScreenDeInit(Dev);

    eeRomFs_DeInit();
}
