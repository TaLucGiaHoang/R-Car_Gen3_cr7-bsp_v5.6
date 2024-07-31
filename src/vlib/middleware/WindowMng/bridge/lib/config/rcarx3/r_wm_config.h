/*************************************************************************************************************
* WindowMng_wm_config
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/


#ifndef R_WM_CONFIG_H_
#define R_WM_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * Title: Declaration of the structure describing the device configuration.
 *
 */

/**
 * \defgroup WMCfg WM configuration
 * \ingroup PublicTypes
 * @brief Types and defines that are device specific.
 *
 * @{
 */

/**
 * Maximum number of layer/surfaces supported by one WM device.
 *
 * @note this value is device dependent.
 */
#define R_WM_NUM_LAYER_PER_VOUT       (5)


/*
 * Section: Type definitions
 */

typedef enum {
    R_WM_VOUT_HDMI,
    R_WM_VOUT_VGA,
    R_WM_VOUT_LVDS,
} r_wm_VoutType_t;


/**
 * WM Timing Flags.
 * List of all available Timing Flags of the Display DataBase
 */
typedef enum
{
    R_WM_VOUT_FLAG_NONE         = (uint8_t) (0),        /**< empty flag */
    R_WM_VOUT_FLAG_CSYNC        = (uint8_t) (1 << 1),   /**< display requires CSYNC signal instead of H/VSYN */
    R_WM_VOUT_FLAG_HSYNC_ACTHI  = (uint8_t) (1 << 2),   /**< use active high level for HSYNC */
    R_WM_VOUT_FLAG_VSYNC_ACTHI  = (uint8_t) (1 << 3),   /**< use active high level for VSYNC */
    R_WM_VOUT_FLAG_DESYNC_ACTHI = (uint8_t) (1 << 4),   /**< use active high level for DATA ENABLE SYNC*/
    R_WM_VOUT_FLAG_USE_RSDS_OUT = (uint8_t) (1 << 5),   /**< DVI mode enable */
    R_WM_VOUT_FLAG_DVI_MODE     = (uint8_t) (1 << 6),   /**<  */
    R_WM_VOUT_FLAG_HDCP_KEEPOUT = (uint8_t) (1 << 7),   /**<  */
    R_WM_VOUT_FLAG_VBLANK_IN_OSC = (uint8_t) (1 << 8),  /**<  */
    R_WM_VOUT_FLAG_LAST                                 /**< Delimiter */
} r_wm_VoutFlags_t;

/**
 * Sub-Timing information of horizontal/vertical display line.
 *
 *          +-----------+
 * SYNC     |           |
 * ---------+           +------------------------------------
 *
 *                               +----------------------+
 * DATA EN                       |                      |
 * ------------------------------+                      +----
 *
 *   Front      Sync      Back
 * <-Porch-><---Width--><-Porch->
 * <---- BlankWidth ------------>
 * <------------------------- Total ------------------------>
 *
 * BlankWidth = Front Porch + Sync Width + Back Porch
 *
 */
typedef struct {
    uint32_t TotalPixel;    /**< length of total line */
    uint32_t SyncWidth;     /**< length of back porch */
    uint32_t BackPorch;     /**< length of active sync pulse */
} r_wm_SubTiming_t;

/**
 * Display timing configuration
 */
typedef struct {
  uint32_t ScreenWidth;         /**< Display width in pixel */
  uint32_t ScreenHeight;        /**< Display height in pixel */

  uint32_t PixelClock;          /**< pixel clock (Hz) */
  uint32_t CurrentDispClock;    /**< current displa clock (hz) */
  uint32_t VoutFlags;           /**< Video output flags @ref r_wm_VoutType_t */
  r_wm_SubTiming_t VoutH;       /**< Horizontal subtiming see @ref r_wm_SubTiming_t */
  r_wm_SubTiming_t VoutV;       /**< Vertical subtiming see @ref r_wm_SubTiming_t */
  uint32_t HdmiPllCfg;          /**< HDMI pll configuration */
  uint32_t HdmiPllCtrl;         /**< HDMI pll control regiter */
  uint32_t HdmiPllDivider;      /**< HDMI pll divider */
  uint32_t LvdsPllLvdpllcr;     /**< LVDS Pll control */
  uint32_t LvdsPllLvddiv;       /**< LVDS pll divider */
  r_wm_VoutType_t VoutType;     /**< Voutput type see @ref r_wm_VoutType_t */
  uint32_t DotClkIn[4];         /**< DU_DOTCLKIN[0..3], must be set by BSP WM */
  uint32_t Extal;               /**< EXTAL clock frequency, must be set by BSP WM (only relevant for R-Car E3/D3 LVDS) */

} r_wm_DispTiming_t;

/** @} */ /* End of group WMCfg */
#ifdef __cplusplus
}
#endif

#endif /* R_WM_CONFIG_H_ */
