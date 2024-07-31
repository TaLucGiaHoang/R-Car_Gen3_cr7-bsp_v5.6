/*************************************************************************************************************
* logo_config
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#ifndef CONFIG_H_
#define CONFIG_H_

#ifdef R_TARGET_BOARD_EBISU
/* TBD: Not verified yet */
#endif

#if defined(R_TARGET_BOARD_SALVATORXS) || defined(R_TARGET_BOARD_ERGUOTOU)

#ifdef USE_DPAD

/* DSIP for VGA (DPAD) */
#define LOGO_DISP_DU_IDX             3 /* M3=2, H3=3 */
#define LOGO_DISP_TIMING_CFG         1
#define LOGO_DISP_SURFACE_CFG        1
#define LOGO_DISP_SURFACE_WIDTH      1024
#define LOGO_DISP_SURFACE_HEIGHT     192

#elif USE_LVDS

/* DISP for LVDS */
#define LOGO_DISP_DU_IDX             0
#define LOGO_DISP_TIMING_CFG         2 /* not used */
#define LOGO_DISP_SURFACE_CFG        2
#define LOGO_DISP_SURFACE_WIDTH      1280
#define LOGO_DISP_SURFACE_HEIGHT     800

#elif USE_HDMI0

/* DISP for HDMI (HDMI0) */
#define LOGO_DISP_DU_IDX             1
#define LOGO_DISP_TIMING_CFG         0
#define LOGO_DISP_SURFACE_CFG        0
#define LOGO_DISP_SURFACE_WIDTH      1920
#define LOGO_DISP_SURFACE_HEIGHT     360

#else

/* DISP for HDMI (HDMI1) */
#define LOGO_DISP_DU_IDX             2
#define LOGO_DISP_TIMING_CFG         0
#define LOGO_DISP_SURFACE_CFG        0
#define LOGO_DISP_SURFACE_WIDTH      1920
#define LOGO_DISP_SURFACE_HEIGHT     360

#endif

#endif

#endif /* CONFIG_H_ */
