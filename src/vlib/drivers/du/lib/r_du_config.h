/*************************************************************************************************************
* du_config
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/


#ifndef R_DU_CONFIG_H_
#define R_DU_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * \defgroup DUconsts   Constants
 * @ingroup DUgroup
 * @{
 */

/* Maximum number of Layers the DU support */
#define R_DU_NUM_LAYERS       (2) /* Layer 1 and Layer 3 */

/* Maximum number of display unit supported by a device */
#define R_DU_RCARD3_MAX_UNIT  (2)
#define R_DU_RCARE3_MAX_UNIT  (2)
#define R_DU_RCARM3_MAX_UNIT  (3)
#define R_DU_RCARH3_MAX_UNIT  (4)

/**
 * List of the supported Flags for Vout configuration
 */
#define R_DU_FLAG_NONE            (0 << 0)
#define R_DU_FLAG_CSYNC           (1 << 1)
#define R_DU_FLAG_HSYNC_ACTHI     (1 << 2)
#define R_DU_FLAG_VSYNC_ACTHI     (1 << 3)
#define R_DU_FLAG_DESYNC_ACTHI    (1 << 4)
#define R_DU_FLAG_DVI_MODE        (1 << 5)
#define R_DU_FLAG_HDCP_KEEPOUT    (1 << 6)
#define R_DU_FLAG_VBLANK_IN_OSC   (1 << 7)

/** @} constants */
/** @addtogroup DUtypes */

/**
 * List the existing DU. DU_LAST is used as a delimiter or to mark
 * a DU as unused.
 */
typedef enum {
    R_DU_DU0 = 0,/**< DU_DU0 */
    R_DU_DU1,    /**< DU_DU1 */
    R_DU_DU2,    /**< DU_DU2 */
    R_DU_DU3,    /**< DU_DU3 */
    R_DU_LAST    /**< DU_LAST */
}r_du_Unit_t;

/**
 * List the Video Output types
 */
typedef enum {
    R_DU_VOUT_HDMI,   /**< VOUT is HDMI */
    R_DU_VOUT_VGA,    /**< VOUT is VGA */
    R_DU_VOUT_LVDS,   /**< VOUT is LVDS */
} r_du_VoutType_t;

/**
 * Display configuration
 */
typedef struct {
    uint32_t ScreenWidth;       /**< Active display width in pixels */
    uint32_t ScreenHeight;      /**< Active display height in pixels */
    uint32_t TargetDispClock;   /**< Target pixel clock in Hz */
    uint32_t CurrentDispClock;  /**< Actual display clock calculated by the driver in Hz */
    uint32_t VoutFlags;         /**< Vout configuration flags, see R_DU_FLAG_* */
    uint32_t VoutHc;            /**< Length of the horizontal line in pixels */
    uint32_t VoutHsw;           /**< Length of the horizontal sync in pixels */
    uint32_t VoutXs;            /**< Length of the horizontal back porch in pixels */
    uint32_t VoutVc;            /**< Length of the vertical line in pixels */
    uint32_t VoutVsw;           /**< Length of the vertical sync in pixels */
    uint32_t VoutYs;            /**< Length of the vertical back porch in pixels */
    uint32_t HdmiPllCtrl;       /**< HDMI PLL controller. If 0, all HDMI PLL settings are calculated */
    uint32_t HdmiPllCfg;        /**< HDMI PLL configuration */
    uint32_t HdmiPllDivider;    /**< HDMI PLL divider */
    uint32_t LvdsPllLvdpllcr;   /**< LVDS PLL controller. If 0, all LVDS PLL settings are calculated */
    uint32_t LvdsPllLvddiv;     /**< LVDS PLL divider */
    r_du_VoutType_t VoutType;   /**< VOUT type */
    uint32_t DotClkIn[4];       /**< DU_DOTCLKIN[0..3] pin clock frequencies in Hz */
    uint32_t Extal;             /**< EXTAL pin clock frequency in Hz, only relevant for R-Car E3/D3 LVDS */
} r_du_DispTimings_t;

/**
 * Blending operations
 */
typedef enum {
    R_DU_BLEND_OPAQUE,  /**< Plane is opaque */
    R_DU_BLEND_ALPHA,   /**< Use Plane alpha */
    R_DU_BLEND_EOR      /**< Use Exclusive Or */
}r_du_PlaneBlending_t;

/**
 * List of color formats supported by a DU plane
 */
typedef enum {
    R_DU_FORMAT_ARGB8888,   /**< 32-bit ARGB color format */
    R_DU_FORMAT_RGB888,     /**< 32-bit/pixel 24-bit RGB color format (not support) */
    R_DU_FORMAT_RGB666,     /**< 32-bit/pixel 18-bit RGB color format (not support) */
    R_DU_FORMAT_YCBCR444    /**< 32-bit/pixel YCbCr444 format (not support) */
}r_du_PlaneFormat_t;

/**
 * Configuration of a DU plane.
 */
typedef struct {
    uint32_t vspd_unit;     /**< VSPD unit mapped to the DU plane */
    uint32_t size_w;        /**< Width of the plane in pixels */
    uint32_t size_h;        /**< Height of the plane in pixels */
    uint32_t pos_x;         /**< Horizontal position of the plane in pixels */
    uint32_t pos_y;         /**< Vertical position of the plane in pixels */
    r_du_PlaneBlending_t blending;  /**< Blending type */
    r_du_PlaneFormat_t format;      /**< Color format */
} r_du_Plane_t;


/** @} data types */

#ifdef __cplusplus
}
#endif
#endif /* R_DU_CONFIG_H_ */
