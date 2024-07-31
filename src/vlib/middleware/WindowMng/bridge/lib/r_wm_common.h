/*************************************************************************************************************
* WindowMng_wm_common
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_WM_COMMON_H_
#define R_WM_COMMON_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Title: Window Manager Common declaration
 *        Contain the type declarations that are needed by both the application
 *        and the WM Server
 */

/*
 * Section: Type declarations
 */

/* MAX monitors per DOC unit */
#define DOC_MAX_MONITORS 16

typedef struct {
    bool         MonitorStatus;
    bool         ParameterError;
    bool         VocMonitorStatus;
    uint32_t     VocSelMon;
    bool         ActMonError1;
    bool         ActMonError0;
    bool         VomError[16];
} r_wm_DOC_Status_t;

/**
 * DOC error callback function pointer
 *
 * @param[in] pCbArg - User-defined argument
 *
 * @retval See @ref r_wm_Error_t
 */
typedef void(*r_wm_DocCb_t)(void *pCbArg, r_wm_DOC_Status_t status);

/**
 * DISCOM mismatch callback function pointer
 *
 * @param[in] pCbArg - User-defined argument
 * @param[in] crc    - Calculated CRC
 *
 * @retval See @ref r_wm_Error_t
 */
typedef void(*r_wm_DiscomCb_t)(void *pCbArg, uint32_t crc);

/**
 * \ingroup PublicTypes WM User mode types
 * @brief WM User Types description
 * @{
 */

/**
 * Enumeration listing the different error types a WM function can return
 */
typedef enum {
    R_WM_ERR_SUCCESS,                   /**< success */
    R_WM_ERR_FAILED,                    /**< Failed - no precision*/
    R_WM_ERR_NOT_INITIALIZED,           /**< the WM device is not initialised */
    R_WM_ERR_UNIT_ALREADY_INIT,         /**< The WM device is already initialised*/
    R_WM_ERR_WRONG_PARAMETER,           /**< Invalid value as input parameter */
    R_WM_ERR_NULL_POINTER,              /**< A Null pointer was passed as parameter */
    R_WM_ERR_INVALID_PROP,              /**< Invalid surface or screen property */
    R_WM_ERR_RINGBUF_NOT_INITIALISED,   /**< The internal ring buffer is not initialised */
    R_WM_ERR_RINGBUFFER_OVERFLOW,       /**< Ring buffer overflow occured */
    R_WM_ERR_OPENCOM_FAILED,            /**< the opening of the communication channel with WMDriver failed */
    R_WM_ERR_CLOSECOM_FAILED,           /**< Failed to close the communication channel */
    R_WM_ERR_MSGIOCTL_FAILED,           /**< Failed to transmit the MSG through IOCTL */
    R_WM_ERR_INVALID_CMD,               /**< And invalid command was passed to the IOCTL */
    R_WM_ERR_OS_MUTEX,                  /**< OS failed to get, release, create or delete a mutex */
    R_WM_ERR_OS_SEMA,                   /**< OS failed to get, release, create or delete a semaphore */
    R_WM_ERR_DEINIT_FAILED,             /**< Failed to de-initialise WM */
    R_WM_ERR_UNIT_USED,                 /**< the WM device is already in used by another process */
    R_WM_ERR_WRONG_PID,                 /**< the application PID does not correspond to the one using the WMDev*/
    R_WM_ERR_NO_SURF,                   /**< No surface available */
    R_WM_ERR_TOO_MANY_THREADS,          /**< The maximum thread dor on WMDev has been reached */
    R_WM_ERR_DUINIT_FAILED,             /**< Failed to initialise the DU of the WM Dev */
    R_WM_ERR_VSPDINIT_FAILED,           /**< Failed to initialised the VSPD corresponding to the WM Dev */
    R_WM_ERR_VSPDBLEND_FAILED,          /**< Failed to set the blending operation */
    R_WM_ERR_NA,                        /**< unknown error */
    R_WM_ERR_LAST                       /**< delimiter */
} r_wm_Error_t;

/**
 * List the WM Device supported.
 */
typedef enum {
    R_WM_DEV_VOUT0,     /**< WM device for VOUT0 - for RCar: DU0 */
    R_WM_DEV_VOUT1,     /**< WM device for VOUT1 - for RCar: DU1 */
    R_WM_DEV_VOUT2,     /**< WM device for VOUT2 - for RCar: DU2 or DU3 (device dependent) */
    R_WM_DEV_VOUT3,     /**< WM device for VOUT3 - for RCar: DU3 */
    R_WM_DEV_LAST       /**< Delimiter */
} r_wm_Dev_t;

/**
 * List the screen properties that the WM can set. See @ref R_WM_ScreenPropertySet
 */
typedef enum {
    R_WM_SCREEN_PROP_FMT,       /**< Screen out format */
    R_WM_SCREEN_PROP_BKGCOLOR,  /**< Screen background color */
    R_WM_SCREEN_PROP_TIMING,    /**< Screen timings */
    R_WM_SCREEN_PROP_LAST       /**< Delimiter */
} r_wm_ScreenProp_t;

/**
 * List the surface properties that the WM can set. See @ref R_WM_SurfacePropertySet
*/
typedef enum {
    R_WM_SURF_PROP_POS,         /**< Surface position (x, y, z) */
    R_WM_SURF_PROP_SIZE,        /**< Surface size (width and height) */
    R_WM_SURF_PROP_COLORFMT,    /**< Set color format */
    R_WM_SURF_PROP_BUFFER,      /**< FrameBuffer address to be set */
    R_WM_SURF_PROP_DOC,         /**< Configure DOC */
    R_WM_SURF_PROP_DISCOM,      /**< Configure DISCOM */
    R_WM_SURF_ALPHA_SET,        /**< Set alpha ratio to the surface */
    R_WM_SURF_PROP_LAST         /**< Delimiter */
} r_wm_SurfaceProp_t;

/**
 * List of events the WM Device supports.
 */
typedef enum {
    R_WM_EVENT_VBLANK,  /**< VBLANK. (is VSPD FRE) */
    R_WM_EVENT_FLUSHSYNC, /**< Requested Flush occured */
    R_WM_EVENT_LAST     /**< Delimiter */
} r_wm_EventId_t;

/**
 * List of color format the WM Device supports.
 * Four C codes defining supported raster formats, use 4c coding for DRM compatibility
 */
#define R_FMT_TO_4C(a,b,c,d) ((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))

typedef enum {
    R_WM_FMT_ARGB8888 =R_FMT_TO_4C('A','R','2','4'),    /**< ARGB 32-bit color format */
    R_WM_FMT_RGB332   =R_FMT_TO_4C('R','G','B','1'),    /**< RGB 8-bit color format   (not support) */
    R_WM_FMT_ARGB4444 =R_FMT_TO_4C('A','R','1','2'),    /**< ARGB 16-bit color format (not support) */
    R_WM_FMT_XRGB4444 =R_FMT_TO_4C('X','R','1','2'),    /**< XRGB 16-bit color format (not support) */
    R_WM_FMT_ARGB1555 =R_FMT_TO_4C('A','R','1','5'),    /**< ARGB 16-bit color format (not support) */
    R_WM_FMT_XRGB1555 =R_FMT_TO_4C('X','R','1','5'),    /**< XRGB 16-bit color format (not support) */
    R_WM_FMT_RGB565   =R_FMT_TO_4C('R','G','B','P'),    /**< RGB 16-bit color format */
    R_WM_FMT_BGR888   =R_FMT_TO_4C('B','G','R','3'),    /**< BGR 24-bit color format (not support) */
    R_WM_FMT_RGB888   =R_FMT_TO_4C('R','G','B','3'),    /**< RGB 24-bit color format (not support) */
    R_WM_FMT_RGB666,       /**< RGB 18-bit color format (not support) */
    R_WM_FMT_YCBCR444,     /**< YUV444 semi-planar color format */
    R_WM_FMT_YCBCR422,     /**< YUV422 semi-planar(NV16) color format */
    R_WM_FMT_YCBCR420,     /**< YUV420 semi-planar(NV12) color format */
    R_WM_FMT_YCBCR444ITL,  /**< YUV444 interleaved color format */
    R_WM_FMT_YCBCR422ITL,  /**< YUV422 interleaved color format */
    R_WM_FMT_YCBCR420ITL,  /**< YUV420 interleaved color format */
    R_WM_FMT_YCBCR420P,    /**< YUV420 planar color format */
    R_WM_FMT_YCBCR422P,    /**< YUV422 planar color format */
    R_WM_FMT_YCBCR444P,    /**< YUV444 planar color format */
} r_wm_ColorFmt_t;

/**
 * @enum DU_CH
 * @brief list of DU video channels
 */
typedef enum {
    DU_CH_0,    /**< DU ch = 0 */
    DU_CH_1,    /**< DU ch = 1 */
    DU_CH_LAST  /**< Delimiter */
} DU_CH;

/**
 * @enum DOC_UNIT
 * @brief list of DOC units
 */
typedef enum {
    DOC_DOC0,   /**< DOC unit = 0 */
    DOC_DOC1,   /**< DOC unit = 1 */
    DOC_LAST    /**< Delimiter */
} r_wm_Doc_DocUnit_t;

typedef struct {
    uint8_t upper;
    uint8_t lower;
} r_wm_Doc_Threshold;

/**
 * @struct r_wm_Doc_Color
 * @brief Data Structure containing the upper/lower limits of a color for DOC
 */
typedef struct {
    r_wm_Doc_Threshold red;
    r_wm_Doc_Threshold blue;
    r_wm_Doc_Threshold green;
} r_wm_DOC_Color;

/**
 * @struct r_wm_DOC_pos
 * @brief structure describing the DOC Position configuration
 */
typedef struct {
    uint32_t pos_x;         /**< The horizontal axis position of the check start */
    uint32_t pos_y;         /**< The vertical axis position of the check start */
} r_wm_DOC_pos;

/**
 * @struct r_wm_DOC_rect
 * @brief structure describing the DOC Rectangle configuration
 */
typedef struct {
    uint32_t width;         /**< The width of the checked area */
    uint32_t height;        /**< The height of the checked area */
} r_wm_DOC_rect;

/**
 * @struct r_wm_Doc_MonitorParam_t
 * @brief Data Structure containing the DOC configuration
 */
typedef struct {
    r_wm_DOC_pos    pos;        /**< */
    r_wm_DOC_rect   rect;       /**< */
    r_wm_DOC_Color  color[4];   /**< */
    uint32_t        threshold;  /**< */
    uint8_t         *CLUT;
    uint32_t        vom_num;
    _Bool           in_use;
    _Bool           Enable;
} r_wm_Doc_MonitorParam_t;

/**
 * @enum    r_doc_Enable_t
 */
typedef enum {
    DOC_DISABLE,      /**< DOC DisEnable */
    DOC_ENABLE,       /**< DOC Enable  */
    DOC_ENABLE_LAST   /**< Delimiter  */
} r_wm_doc_Enable_t;

/**
 * @struct r_wm_Doc_DUParam_t
 * @brief  Data Structure containing the used DU parameters by DOC
 */
typedef struct {
    uint32_t    ScreenWidth;        /**< the width of the screen registered in DU */
    uint32_t    ScreenHeight;       /**< the height of the screen registered in DU */
    uint32_t    HbackPorchOffset;   /**< the horizontal porch offset value registered in DU */
    uint32_t    VbackPorchOffset;   /**< the vertical porch offset value registered in DU */
} r_wm_Doc_DUParam_t;

/**
 * @struct color_threshold
 * @brief structure describing the DOC Color configuration
 */
typedef struct {
    uint8_t upper;          /**< Color Code Upper Limit */
    uint8_t lower;          /**< Color Code Lower Limit */
} color_threshold;

/**
 * @struct r_wm_DOC_color
 * @brief structure describing the DOC RGB configuration
 */
typedef struct {
    color_threshold Red;    /**< Red Color Code Threshold */
    color_threshold Green;  /**< Green Color Code Threshold */
    color_threshold Blue;   /**< Blue Color Code Threshold */
} r_wm_DOC_color;

/**
 * @struct r_wm_Doc_ExpParam_t
 * @brief structure describing the DOC configuration
 */
typedef struct {
    uint32_t            DU_ch;      /**< DU Channel to Check */
    r_wm_DOC_pos        pos;        /**< Potision to Check */
    r_wm_DOC_rect       rect;       /**< Rectangle to Check */
    r_wm_DOC_color      color[4];   /**< Color Code to Check */
    uint32_t            threshold;  /**< Color Code Threshold to Check */
    uint8_t             *CLUT;      /**< Color LookUp Table Address for Check */
} r_wm_Doc_ExpParam_t;

/** @} */ /* End Ingroup Types */

#ifdef __cplusplus
}
#endif

#endif /* R_WM_COMMON_H_ */
