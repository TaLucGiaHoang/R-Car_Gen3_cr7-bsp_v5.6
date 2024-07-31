/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_type.h
* Product Name : IMR Driver
* Device(s)    : R-Car H3/M3/M3N/D3/E3
* Description  : IMR Driver's public header of macros and types.
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 16.04.2021 0.01     New create
 *         : 14.06.2021 0.02     Add macro definition, enumeration definition and structure definition.
 *                               Remove unnecessary macro definition.
 *                               Change structure member of st_imrdrv_data_t and st_imrdrv_attr_param_t.
 *                               Modify callback funtion definition.
 *         : 24.08.2021 0.03     Add macro definition, enumeration definition and structure definition.
 *         : 24.08.2021 0.04     Add definitions for Add R_IMRDRV_AttrSetMipmap().
 *         : 01.09.2021 0.05     Add structure definitions for R_IMRDRV_AttrSetGaussianFilter().
 *         : 29.10.2021 0.06     Changed "only V3x" to [Not Supported] in the comment
 *********************************************************************************************************************/

#ifndef R_IMRDRV_TYPE_H
#define R_IMRDRV_TYPE_H
#ifdef __cplusplus
extern "C"
{
#endif

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include "rcar-xos/osal/r_osal.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/**
 * \defgroup IMRconsts   Constants
 * @ingroup IMRgroup
 * @{
 */

#define IMRDRV_SIZE_WORKAREA            (2048U) /* Lower limit of work area size */

/* IMR Interrupt status */
#define IMRDRV_SR_WERR                  (0x00000800U) /* WERR interrupt */
#define IMRDRV_SR_WOVF                  (0x00000400U) /* WOVF interrupt */
#define IMRDRV_SR_IER                   (0x00000002U) /* IER interrupt */
#define IMRDRV_SR_TRA                   (0x00000001U) /* TRA interrupt */

/* Index of attribute information for scale down */
#define IMRDRV_ATTR_RS_IDX_ONE_ONE      (0U) /**< Index of attribute information for scale down to 1/1. */
#define IMRDRV_ATTR_RS_IDX_ONE_HALF     (1U) /**< Index of attribute information for scale down to 1/2. */
#define IMRDRV_ATTR_RS_IDX_ONE_FOURTH   (2U) /**< Index of attribute information for scale down to 1/4. */
#define IMRDRV_ATTR_RS_IDX_ONE_EIGHTH   (3U) /**< Index of attribute information for scale down to 1/8. */
#define IMRDRV_ATTR_RS_IDX_MAX_NUM      (4U) /**< Maximum number of attribute information for scale down. */

/* Invalid value of attribute information for scale down */
#define IMRDRV_ATTR_RS_ADDR_INVALID     (0xFFFFFFFFU) /* Invalid value of address for scale down. */
#define IMRDRV_ATTR_DYP_ADDR_INVALID    (0xFFFFFFFFU) /* Invalid value of address for DYP. */
#define IMRDRV_ATTR_DYP_STRIDE_INVALID  (0x00000000U) /* Invalid value of stride for DYP. */

/* Maximum number of sync module */
#define IMRDRV_SYNC_MODULE_MAX_NUM      (16U) /* Maximum number of sync module. */

/* Index of attribute information for mipmap */
#define IMRDRV_MAP_INDEX_LV1            (0U) /**< Index of mipmap unit level 1.  */
#define IMRDRV_MAP_INDEX_LV2            (1U) /**< Index of mipmap unit level 2.  */
#define IMRDRV_MAP_INDEX_LV3            (2U) /**< Index of mipmap unit level 3.  */
#define IMRDRV_MAP_INDEX_LV4            (3U) /**< Index of mipmap unit level 4.  */
#define IMRDRV_MAP_MAX_NUM              (4U) /**< Maximum number of mipmap unit. */
/** @} constants */

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**
 * \defgroup IMRtypes   Data Types
 * @ingroup IMRgroup
 * @{
 */

/* Error code of IMR Driver */
typedef enum
{
    IMRDRV_ERROR_OK         = 0,            /**< Successful completion. */
    IMRDRV_ERROR_PAR        = 1,            /**< Parameter error. */
    IMRDRV_ERROR_DEV        = 2,            /**< Hardware fail. */
    IMRDRV_ERROR_TIME       = 3,            /**< Timeout error. */
    IMRDRV_ERROR_CONF       = 4,            /**< Inconsistent configuration error. */
    IMRDRV_ERROR_FAIL       = 5,            /**< Unexpected error. */
    IMRDRV_ERROR_IO         = 6,            /**< The register operation error. */
    IMRDRV_ERROR_STATE      = 1024,         /**< IMR Driver status error. */
    IMRDRV_ERROR_CONFREG    = 1025,         /**< Configuration Register Check error. */
    IMRDRV_ERROR_MODULESTOP = 1026,         /**< Unintended Module Stop Check error. */
    IMRDRV_ERROR_HANDLE     = 1027          /**< IMR Driver control data error. */
} e_imrdrv_errorcode_t;

typedef enum
{
    IMRDRV_CHANNELNO_INVALID    = -1,   /**< Invalid channel number. */
    IMRDRV_CHANNELNO_0,                 /**< IMR-LX4 channel0. */
    IMRDRV_CHANNELNO_1,                 /**< IMR-LX4 channel1. [Only H3,M3,M3N]*/
    IMRDRV_CHANNELNO_2,                 /**< IMR-LX4 channel2. [Only H3]*/
    IMRDRV_CHANNELNO_3,                 /**< IMR-LX4 channel3. [Only H3] */
    IMRDRV_CHANNELNO_4,                 /**< IMR-LX4 channel4. [Not supported] */
    IMRDRV_CHANNELNO_5                  /**< IMR-LX4 channel5. [Not supported] */
} e_imrdrv_channelno_t;

typedef enum
{
    IMRDRV_STATE_UNINIT  = 0,      /**< Uninitialized State. */
    IMRDRV_STATE_INIT,             /**< Initialized State.   */
    IMRDRV_STATE_READY,            /**< Ready State.         */
    IMRDRV_STATE_ATTR_SET,         /**< Attribute set State. */
    IMRDRV_STATE_ACTIVE            /**< Active State.        */
} e_imrdrv_state_t;

/*******************************************************************************************************************//**
 * @enum  e_imrdrv_color_format_t
 * Color formats supported by IMR-LX4.
***********************************************************************************************************************/
typedef enum {
    IMRDRV_COLOR_FORM_INVALID = -1,         /**< Invalid value of color format. */
    IMRDRV_COLOR_FORM_Y,                    /**< YUV422/YUV420 Y data. */
    IMRDRV_COLOR_FORM_UV,                   /**< YUV422 UV data. */
    IMRDRV_COLOR_FORM_SEP_Y,                /**< YUV422/YUV420 Separated Y data. */
    IMRDRV_COLOR_FORM_SEP_UV,               /**< YUV422 Separated UV data. */
    IMRDRV_COLOR_FORM_DATA,                 /**< Data. */
    IMRDRV_COLOR_FORM_UYVY,                 /**< YUV422 interleaved UYVY. */
    IMRDRV_COLOR_FORM_YVYU,                 /**< YUV422 interleaved YVYU. */
    IMRDRV_COLOR_FORM_VYUY,                 /**< YUV422 interleaved VYUY. */
    IMRDRV_COLOR_FORM_YUYV,                 /**< YUV422 interleaved YUYV. */
    IMRDRV_COLOR_FORM_SEMI422,              /**< YUV422 semi-planer. [Not support] */
    IMRDRV_COLOR_FORM_SEMI420,              /**< YUV420 semi-planer. [Not support] */
    IMRDRV_COLOR_FORM_UV420,                /**< YUV420 UV data. [Not support] */
    IMRDRV_COLOR_FORM_SEP_UV420             /**< YUV420 Separated UV data. [Not support] */
} e_imrdrv_color_format_t;

/*******************************************************************************************************************//**
 * @enum  e_imrdrv_bpp_t
 * Bit per pixel supported by IMR-LX4.
***********************************************************************************************************************/
typedef enum {
    IMRDRV_BPP_INVALID = -1,                /**< Invalid value of bit per pixel. */
    IMRDRV_BPP_8,                           /**< 8-bpp. */
    IMRDRV_BPP_10,                          /**< 10-bpp. */
    IMRDRV_BPP_12,                          /**< 12-bpp. */
    IMRDRV_BPP_14,                          /**< 14-bpp. [Not support] */
    IMRDRV_BPP_16,                          /**< 16-bpp. [Not support] */
    IMRDRV_BPP_32                           /**< 32-bpp. [Not support] */
} e_imrdrv_bpp_t;

/*******************************************************************************************************************//**
 * @enum  e_imrdrv_mode_t
 * Definition of mode ON(enable) / OFF(disable) setting.
***********************************************************************************************************************/
typedef enum {
    IMRDRV_MODE_INVALID = -1,               /**< Invalid value. */
    IMRDRV_MODE_OFF,                        /**< Mode off. */
    IMRDRV_MODE_ON                          /**< Mode on. */
} e_imrdrv_mode_t;

/*******************************************************************************************************************//**
 * @enum  e_imrdrv_scaling_down_filter_t
 * Scaling down filter supported by IMR-LXn.
***********************************************************************************************************************/
typedef enum {
    IMRDRV_SCALE_DOWN_FILT_INVALID = -1,    /**< Invalid value. */
    IMRDRV_SCALE_DOWN_FILT_AVERAGE,         /**< Averaging the value of 2x2 pixels before scaling down. */
    IMRDRV_SCALE_DOWN_FILT_ADD              /**< Adding the value of 2x2 pixels before scaling down. */
} e_imrdrv_scaling_down_filter_t;

/*******************************************************************************************************************//**
 * @enum  e_imrdrv_rounding_mode_t
 * Rounding mode supported by IMR-LXn.
***********************************************************************************************************************/
typedef enum {
    IMRDRV_ROUND_MODE_INVALID = -1,         /**< Invalid value. */
    IMRDRV_ROUND_MODE_ROUND_DOWN,           /**< Rounding down. */
    IMRDRV_ROUND_MODE_ROUND_NEAR            /**< Rounding to the nearest number. */
} e_imrdrv_rounding_mode_t;

/*******************************************************************************************************************//**
 * @enum  e_imrdrv_rotate_mode_t
 * Algorithm to rotate the UV plane in YUV422 format supported by IMR-LXn.
***********************************************************************************************************************/
typedef enum {
    IMRDRV_ROTATE_MODE_INVALID = -1,        /**< Invalid value. */
    IMRDRV_ROTATE_MODE_UNIT_BLOCKS,         /**< Rotated in 2x2 block unit. */
    IMRDRV_ROTATE_MODE_INTERPOLATING        /**< Rotated during interpolation. */
} e_imrdrv_rotate_mode_t;

/*******************************************************************************************************************//**
 * @enum  e_imrdrv_cache_mode_t
 * Cache mode supported by IMR-LXn.
***********************************************************************************************************************/
typedef enum {
    IMRDRV_CACHE_MODE_INVALID = -1,          /**< Invalid value. */
    IMRDRV_CACHE_MODE_NORMAL,                /**< Normal mode. */
    IMRDRV_CACHE_MODE_NON_BLOCKING           /**< Non-blocking mode. */
} e_imrdrv_cache_mode_t;

/*******************************************************************************************************************//**
 * @enum  e_imrdrv_double_cache_mode_t
 * 
***********************************************************************************************************************/
typedef enum {
    IMRDRV_DOUBLE_CACHE_MODE_INVALID = -1,   /**< Invalid value. */
    IMRDRV_DOUBLE_CACHE_MODE_SINGLE,         /**< Single Cache Mode(OFF). */
    IMRDRV_DOUBLE_CACHE_MODE_DOUBLE          /**< Double Cache Mode(ON). */
} e_imrdrv_double_cache_mode_t;

/*******************************************************************************************************************//**
 * @enum  e_imrdrv_sync_module_t
 * Synchronizing module supported by IMR-LXn.
***********************************************************************************************************************/
typedef enum {
    IMRDRV_SYNC_MODULE_INVALID = -1,        /**< Invalid value. */
    IMRDRV_SYNC_MODULE_IMR_CH0,             /**< Synchronizing module is IMR-LXn ch0.[Not supported] */
    IMRDRV_SYNC_MODULE_IMR_CH1,             /**< Synchronizing module is IMR-LXn ch1.[Not supported] */
    IMRDRV_SYNC_MODULE_IMR_CH2,             /**< Synchronizing module is IMR-LXn ch2.[Not supported] */
    IMRDRV_SYNC_MODULE_IMR_CH3,             /**< Synchronizing module is IMR-LXn ch3.[Not supported] */
    IMRDRV_SYNC_MODULE_IMR_CH4,             /**< Synchronizing module is IMR-LXn ch4.[Not supported] */
    IMRDRV_SYNC_MODULE_IMR_CH5,             /**< Synchronizing module is IMR-LXn ch5.[Not supported] */
    IMRDRV_SYNC_MODULE_IMP_DMAC0_0,         /**< Synchronizing module is IMP DMAC0 (thread0).[Not supported] */
    IMRDRV_SYNC_MODULE_IMP_DMAC0_1,         /**< Synchronizing module is IMP DMAC0 (thread1).[Not supported] */
    IMRDRV_SYNC_MODULE_IMP_DMAC1_0,         /**< Synchronizing module is IMP DMAC1 (thread0).[Not supported] */
    IMRDRV_SYNC_MODULE_IMP_DMAC1_1,         /**< Synchronizing module is IMP DMAC1 (thread1).[Not supported] */
    IMRDRV_SYNC_MODULE_SLIM_DMAC0_0,        /**< Synchronizing module is Slim-IMP DMAC0 (thread0).[Not supported] */
    IMRDRV_SYNC_MODULE_SLIM_DMAC0_1,        /**< Synchronizing module is Slim-IMP DMAC0 (thread1).[Not supported] */
    IMRDRV_SYNC_MODULE_SLIM_DMAC1_0,        /**< Synchronizing module is Slim-IMP DMAC1 (thread0).[Not supported] */
    IMRDRV_SYNC_MODULE_SLIM_DMAC1_1,        /**< Synchronizing module is Slim-IMP DMAC1 (thread1).[Not supported] */
    IMRDRV_SYNC_MODULE_ISP0,                /**< Synchronizing module is ISP0.[Not supported] */
    IMRDRV_SYNC_MODULE_ISP1,                /**< Synchronizing module is ISP1.[Not supported] */
    IMRDRV_SYNC_MODULE_ISP2,                /**< Synchronizing module is ISP2.[Not supported] */
    IMRDRV_SYNC_MODULE_ISP3,                /**< Synchronizing module is ISP3.[Not supported] */
    IMRDRV_SYNC_MODULE_VIN0_CH0,            /**< Synchronizing module is VIN[0].ch[0].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN0_CH1,            /**< Synchronizing module is VIN[0].ch[1].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN0_CH2,            /**< Synchronizing module is VIN[0].ch[2].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN0_CH3,            /**< Synchronizing module is VIN[0].ch[3].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN0_CH4,            /**< Synchronizing module is VIN[0].ch[4].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN0_CH5,            /**< Synchronizing module is VIN[0].ch[5].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN0_CH6,            /**< Synchronizing module is VIN[0].ch[6].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN0_CH7,            /**< Synchronizing module is VIN[0].ch[7].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN1_CH0,            /**< Synchronizing module is VIN[1].ch[0].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN1_CH1,            /**< Synchronizing module is VIN[1].ch[1].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN1_CH2,            /**< Synchronizing module is VIN[1].ch[2].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN1_CH3,            /**< Synchronizing module is VIN[1].ch[3].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN1_CH4,            /**< Synchronizing module is VIN[1].ch[4].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN1_CH5,            /**< Synchronizing module is VIN[1].ch[5].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN1_CH6,            /**< Synchronizing module is VIN[1].ch[6].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN1_CH7,            /**< Synchronizing module is VIN[1].ch[7].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN2_CH0,            /**< Synchronizing module is VIN[2].ch[0].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN2_CH1,            /**< Synchronizing module is VIN[2].ch[1].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN2_CH2,            /**< Synchronizing module is VIN[2].ch[2].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN2_CH3,            /**< Synchronizing module is VIN[2].ch[3].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN2_CH4,            /**< Synchronizing module is VIN[2].ch[4].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN2_CH5,            /**< Synchronizing module is VIN[2].ch[5].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN3_CH0,            /**< Synchronizing module is VIN[3].ch[0].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN3_CH1,            /**< Synchronizing module is VIN[3].ch[1].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN3_CH2,            /**< Synchronizing module is VIN[3].ch[2].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN3_CH3,            /**< Synchronizing module is VIN[3].ch[3].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN3_CH4,            /**< Synchronizing module is VIN[3].ch[4].[Not supported] */
    IMRDRV_SYNC_MODULE_VIN3_CH5             /**< Synchronizing module is VIN[3].ch[5].[Not supported] */
} e_imrdrv_sync_module_t;

/*******************************************************************************************************************//**
 * @enum  e_imrdrv_syncc_no_t
 * SYNCC number supported by IMR-LXn.
***********************************************************************************************************************/
typedef enum {
    IMRDRV_SYNCC_NO_INVALID = -1,           /**< Invalid value. */
    IMRDRV_SYNCC_NO_0,                      /**< SYNCC number is SYNCC0.[Not supported] */
    IMRDRV_SYNCC_NO_1,                      /**< SYNCC number is SYNCC1.[Not supported] */
    IMRDRV_SYNCC_NO_2,                      /**< SYNCC number is SYNCC2.[Not supported] */
    IMRDRV_SYNCC_NO_3,                      /**< SYNCC number is SYNCC3.[Not supported] */
    IMRDRV_SYNCC_NO_4,                      /**< SYNCC number is SYNCC4.[Not supported] */
    IMRDRV_SYNCC_NO_5,                      /**< SYNCC number is SYNCC5.[Not supported] */
    IMRDRV_SYNCC_NO_6,                      /**< SYNCC number is SYNCC6.[Not supported] */
    IMRDRV_SYNCC_NO_7,                      /**< SYNCC number is SYNCC7.[Not supported] */
    IMRDRV_SYNCC_NO_8,                      /**< SYNCC number is SYNCC8.[Not supported] */
    IMRDRV_SYNCC_NO_9,                      /**< SYNCC number is SYNCC9.[Not supported] */
    IMRDRV_SYNCC_NO_10,                     /**< SYNCC number is SYNCC10.[Not supported] */
    IMRDRV_SYNCC_NO_11,                     /**< SYNCC number is SYNCC11.[Not supported] */
    IMRDRV_SYNCC_NO_12,                     /**< SYNCC number is SYNCC12.[Not supported] */
    IMRDRV_SYNCC_NO_13,                     /**< SYNCC number is SYNCC13.[Not supported] */
    IMRDRV_SYNCC_NO_14,                     /**< SYNCC number is SYNCC14.[Not supported] */
    IMRDRV_SYNCC_NO_15                      /**< SYNCC number is SYNCC15.[Not supported] */
} e_imrdrv_syncc_no_t;

/*******************************************************************************************************************//**
 * @enum  e_imrdrv_map_max_level_t
 * 
***********************************************************************************************************************/
typedef enum {
    IMRDRV_MAP_MAX_LEVEL_INVALID = -1,       /**< Invalid value. */
    IMRDRV_MAP_MAX_LEVEL_0,                  /**< The maximum level of detail for pyramidal images is 0. */
    IMRDRV_MAP_MAX_LEVEL_1,                  /**< The maximum level of detail for pyramidal images is 1.[Not supported] */
    IMRDRV_MAP_MAX_LEVEL_2,                  /**< The maximum level of detail for pyramidal images is 2.[Not supported] */
    IMRDRV_MAP_MAX_LEVEL_3,                  /**< The maximum level of detail for pyramidal images is 3.[Not supported] */
    IMRDRV_MAP_MAX_LEVEL_4                   /**< The maximum level of detail for pyramidal images is 4.[Not supported] */
} e_imrdrv_map_max_level_t;


typedef struct
{
    void                    *p_work_addr;   /* The pointer of the work area. */
    uint32_t                work_size;      /* Size of the work area. */
    e_imrdrv_channelno_t    channel_no;     /* Number of channels to use. */
} st_imrdrv_initdata_t;

typedef struct
{
    osal_mutex_id_t                 mutex_id;           /* OSAL mutex ID. */
    osal_milli_sec_t                mutex_wait_period;  /* The wait time (msec) of locking mutex. */
    e_osal_interrupt_priority_t     dev_irq_priority;   /* Interrupt priority. */
} st_imrdrv_os_config_t;

/* Control structure handle */
typedef struct st_imrdrv_ctrl_handle_t* imrdrv_ctrl_handle_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_dl_t
 * This structure describes the Display List data.
***********************************************************************************************************************/
typedef struct
{
    uint32_t    phys_addr;                              /**< Physical address of DL storage area. */
    
} st_imrdrv_dl_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_uv_setting_t
 * This structure describes the UV image data.
***********************************************************************************************************************/
typedef struct
{
    uint32_t                 uv_stride;                 /**< Stride of the picture (pixel). */
    uint32_t                 uv_phys_addr;              /**< Start address of the physical memory area. */
    e_imrdrv_bpp_t           uv_bpp;                    /**< Color depth of the picture(bit/pixel). */
} st_imrdrv_uv_setting_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_data_t
 * This structure describes the image data.
***********************************************************************************************************************/
typedef struct
{
    uint32_t                 height;                    /**< Height of the picture(line). */
    uint32_t                 width;                     /**< Width of the picture(pixel). */
    uint32_t                 stride;                    /**< Stride of the picture (pixel). */
    uint32_t                 phys_addr;                 /**< Start address of the physical memory area. */
    e_imrdrv_color_format_t  color_format;              /**< Color format of the picture. */
    e_imrdrv_bpp_t           bpp;                       /**< Color depth of the picture(bit/pixel). */
    st_imrdrv_uv_setting_t   uv_set;                    /**< UV image data. */
} st_imrdrv_data_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_triangle_mode_t
 * This structure describes the triangle mode.
***********************************************************************************************************************/
typedef struct
{
    e_imrdrv_mode_t          uvshfval;                 /**< On/off setting of UVSHFVAL in TRIMR register. [Not supported] */
    e_imrdrv_mode_t          shfval;                   /**< On/off setting of SHFVAL in TRIMR register. [Not supported] */
    e_imrdrv_mode_t          uvshfe;                   /**< On/off setting of UVSHFE in TRIMR register. [Not supported] */
    e_imrdrv_mode_t          shfe;                     /**< On/off setting of SHFE in TRIMR register. [Not supported] */
    e_imrdrv_mode_t          rde;                      /**< On/off setting of RDE in TRIMR register. [Not supported] */
    e_imrdrv_mode_t          tfe;                      /**< On/off setting of TFE in TRIMR register. [Not supported] */
    e_imrdrv_mode_t          tcm;                      /**< On/off setting of TCM in TRIMR register. */
    e_imrdrv_mode_t          autosg;                   /**< On/off setting of AUTOSG in TRIMR register. */
    e_imrdrv_mode_t          autodg;                   /**< On/off setting of AUTODG in TRIMR register. */
    e_imrdrv_mode_t          bfe;                      /**< On/off setting of BFE in TRIMR register. */
    e_imrdrv_mode_t          tme;                      /**< On/off setting of TME in TRIMR register. */
} st_imrdrv_triangle_mode_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_attr_param_t
 * This structure describes the attribute parameter.
***********************************************************************************************************************/
/**************************************************************************************************************
 * MISRA Rule: Dir 4.8
 * Reason: This structure is used by both IMR Driver and User Application.
 * Verification: This structure is assumed in advance that some components use it.
**************************************************************************************************************/
typedef struct
{
    st_imrdrv_dl_t              dl_data;                /**< Display List data for execute. */
    st_imrdrv_data_t            src_data;               /**< The source image data to be rendered. */
    st_imrdrv_data_t            dst_data;               /**< Destination image data to be rendered. */
    st_imrdrv_triangle_mode_t   triangle_mode;          /**< Triangle mode. */
} st_imrdrv_attr_param_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_attr_rs_unit_t
 * This structure describes the attribute of rotator and scaler by size.
***********************************************************************************************************************/
typedef struct
{
    e_imrdrv_scaling_down_filter_t scaling_down_filter; /**< Scaling down filter. */
    e_imrdrv_rounding_mode_t       rounding_mode;       /**< Rounding mode. */
    e_imrdrv_bpp_t                 dst_bpp;             /**< Destination image format. */
    uint32_t                       dst_addr_norotate;   /**< Destination address for not rotate. */
    uint32_t                       dst_addr_rotate;     /**< Destination address for rotate. */
    uint32_t                       dst_stride_norotate; /**< Destination stride for rotate. */
    uint32_t                       dst_stride_rotate;   /**< Destination stride for not rotate. */
} st_imrdrv_attr_rs_unit_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_attr_rs_param_t
 * This structure describes the attribute of rotator and scaler.
***********************************************************************************************************************/
typedef struct
{
    uint32_t                    valid_rs_idx;                         /**< Valid index of attribute of rotator and scaler. */
    st_imrdrv_attr_rs_unit_t    rs_param[IMRDRV_ATTR_RS_IDX_MAX_NUM]; /**< Attribute of rotator and scaler. */
    e_imrdrv_rotate_mode_t      rotate_mode_yuv422;                   /**< Algorithm to rotate the UV plane in YUV422 format. */
} st_imrdrv_attr_rs_param_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_attr_cache_mode_t
 * This structure describes the attribute of cache mode.
***********************************************************************************************************************/
typedef struct
{
    e_imrdrv_cache_mode_t          cache_mode;          /**< The cache mode. */
    e_imrdrv_double_cache_mode_t   double_cache_mode;   /**< The double cache mode. */
} st_imrdrv_attr_cache_mode_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_attr_sync_unit_t
 * This structure describes the attribute of one element of sync module.
***********************************************************************************************************************/
typedef struct
{
    e_imrdrv_sync_module_t  sync_module;    /**< The type of sync module. */
    e_imrdrv_syncc_no_t     syncc_no;       /**< The number which of SYNCC0-15 it corresponds to. */
} st_imrdrv_attr_sync_unit_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_attr_sync_param_t
 * This structure describes the attribute of sync module.
***********************************************************************************************************************/
typedef struct
{
    uint32_t                    valid_sync_idx;                         /**< Valid index of attribute of sync module. */
    st_imrdrv_attr_sync_unit_t  sync_param[IMRDRV_SYNC_MODULE_MAX_NUM]; /**< Attribute of sync module. */
} st_imrdrv_attr_sync_param_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_attr_mipmap_unit_ex_t
 * This structure describes the uv plane for semi-planar format, or DYP attribute of one element of pyramidal image.
***********************************************************************************************************************/
typedef struct
{
    uint32_t                        stride;             /**< Stride of the picture. (byte) */
    uint32_t                        phys_addr;          /**< Start address of the physical memory area. */
} st_imrdrv_attr_mipmap_unit_ex_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_attr_mipmap_unit_t
 * This structure describes the attribute of one element of pyramidal image.
***********************************************************************************************************************/
typedef struct
{
    uint32_t                        stride;             /**< Stride of the picture. (byte) */
    uint32_t                        phys_addr;          /**< Start address of the physical memory area. */
    st_imrdrv_attr_mipmap_unit_ex_t extend_set;         /**< Extended settings. */
} st_imrdrv_attr_mipmap_unit_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_attr_mipmap_param_t
 * This structure describes the attribute of pyramidal image.
***********************************************************************************************************************/
typedef struct
{
    e_imrdrv_map_max_level_t        maxlodu;            /**< The maximum level of detail for pyramidal images to be used in the u direction. */
    e_imrdrv_map_max_level_t        maxlodv;            /**< The maximum level of detail for pyramidal images to be used in the v direction. */
    st_imrdrv_attr_mipmap_unit_t    mipmap_param[IMRDRV_MAP_MAX_NUM]; /**< Attribute of pyramidal image. */
} st_imrdrv_attr_mipmap_param_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_attr_dyp_param_t
 * This structure describes the double luminance plane.
***********************************************************************************************************************/
typedef struct
{
    uint32_t                 stride;                    /**< Stride of the picture (pixel). */
    uint32_t                 phys_addr;                 /**< Start address of the physical memory area. */
} st_imrdrv_attr_dyp_param_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_gauss_padding_t
 * This structure describes the attribute of padding for gaussian filter.
***********************************************************************************************************************/
typedef struct
{
    e_imrdrv_mode_t     top;                            /**< Padding at the top of the image. */
    e_imrdrv_mode_t     down;                           /**< Padding at the bottom of the image. */
    e_imrdrv_mode_t     left;                           /**< Padding at the left edge of the image. */
    e_imrdrv_mode_t     right;                          /**< Padding at the right edge of the image. */
} st_imrdrv_gauss_padding_t;

/*******************************************************************************************************************//**
 * @struct  st_imrdrv_attr_gauss_filter_t
 * This structure describes the attribute of gaussian filter.
***********************************************************************************************************************/
typedef struct
{
    e_imrdrv_rounding_mode_t        half_scale;         /**< Rounding mode for half scale. */
    e_imrdrv_rounding_mode_t        quarter_scale;      /**< Rounding mode for quarter scale. */
    e_imrdrv_rounding_mode_t        one_eighth_scale;   /**< Rounding mode for one eighth scale. */
    st_imrdrv_gauss_padding_t       padding;            /**< Attribute of padding. */
} st_imrdrv_attr_gauss_filter_t;

typedef int32_t (*p_imrdrv_callback_func_t)(
    const e_imrdrv_errorcode_t  ret_code,               /* The result of callback processing. */
    const uint32_t              details_code,           /* The factor of the interrupt. */
    void                        *const p_callback_arg   /* The argument of callback. */
);

/* Handle defined in IMR Driver implementation files */
struct st_imrdrv_ctrl_handle_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/** @} types */

#ifdef __cplusplus
}
#endif
#endif /* R_IMRDRV_TYPE_H */

