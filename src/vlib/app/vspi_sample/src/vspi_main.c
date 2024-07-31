/*************************************************************************************************************
* vspi_main
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/*******************************************************************************
  Title: Main Module

  This module contains the main and the main loop functions.
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_cio_api.h"
#include "logo_image.h"

#include "config.h"
#include "display.h"
#include "vspi_main.h"

#include "r_print_api.h"


#ifdef USE_DPAD
#define RGB_IMAGE(member)     (rgb_image_dpad.member)
#elif USE_LVDS
#define RGB_IMAGE(member)     (rgb_image_lvds.member)
#else  /* USE_HDMI0 / USE_HDMI1 */
#define RGB_IMAGE(member)     (rgb_image_hdmi.member)
#endif /* USE_DPAD */

/*------------------------------*/
/* User defined output funciton */
/*------------------------------*/
int vspi_main(void)
{
    R_PRINT_Log("=== VSPI Sample Start ===\n");
    Disp_Init(LOGO_DISP_DU_IDX, LOGO_DISP_TIMING_CFG);
    R_PRINT_Log("%s buff:%lx\r\n", __func__, *(uint32_t *)(RGB_IMAGE(pbuffer)));

    Disp_InitSurface(LOGO_DISP_DU_IDX, LOGO_DISP_SURFACE_CFG, (uintptr_t)RGB_IMAGE(pbuffer));
    R_PRINT_Log("Logo disply Stop \r\n");
    return 0;
}
