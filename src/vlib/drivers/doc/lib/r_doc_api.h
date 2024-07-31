/*************************************************************************************************************
* doc_api_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#ifndef R_DOC_API_H_
#define R_DOC_API_H_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * \defgroup DOCgroup   DOC driver
 * @ingroup drivers
 * @brief DOC(Display Output Checker) driver interface
 * @{
 * @}
 */

/**
 * \defgroup DOCtypes   Data Types
 * @ingroup DOCgroup
 * @{
 */

/**
 * @enum    r_doc_Error_t
 * @brief   list of errors DOC API can return
 */
typedef enum {
    R_DOC_ERR_SUCCESS,             /**< success */
    R_DOC_ERR_INVALID_PARAMETER,   /**< invalid parameter */
} r_doc_Error_t;

/**
 * @enum    r_doc_Unit_t
 * @brief   list the types of DOC units available
 */
typedef enum {
    R_DOC_DOC0,      /*< DOC0 unit */
    R_DOC_DOC1,      /*< DOC1 unit */
} r_doc_Unit_t;

/**
 * @enum    r_doc_VideoChannel_t
 * @brief   list the types of Video Channel available
 */
typedef enum {
    R_DOC_VCH0,      /*< Video Channel 0 */
    R_DOC_VCH1,      /*< Video Channel 1 */
} r_doc_VideoChannel_t;

/**
 * @enum    r_doc_DocInterruptPhase_t
 * @brief   list the types of DOC interrupt phases
 */
typedef enum {
    R_DOC_INT_PHASE_INIT,
    R_DOC_INT_PHASE_POSTPONE,
    R_DOC_INT_PHASE_CLEAR,
} r_doc_DocInterruptPhase_t;


/**
 * @struct r_doc_Du_Params_t
 * @brief  DU parameters used by DOC
 */
typedef struct {
    uint32_t    ScreenWidth;        /*< the width of the screen registered in DU (0..4095) */
    uint32_t    ScreenHeight;       /*< the height of the screen registered in DU (0..2047) */
    uint32_t    HbackPorchOffset;   /*< the horizontal porch offset value registered in DU */
    uint32_t    VbackPorchOffset;   /*< the vertical porch offset value registered in DU */
} r_doc_Du_Params_t;

/**
 * @struct r_doc_Pos_t
 * @brief  start position of area to monitor
 */
typedef struct {
    uint32_t pos_x;                 /*< x position of area to monitor (0..0xFFF) */
    uint32_t pos_y;                 /*< y position of area to monitor (0..0x7FF) */
} r_doc_Pos_t;

/**
 * @struct r_doc_Rect_t
 * @brief  Range of area to monitor
 */
typedef struct {
    uint32_t width;                 /*< width of area to monitor (0..255) */
    uint32_t height;                /*< height of area to monitor (0..255) */
} r_doc_Rect_t;

/**
 * @struct r_doc_Region_t
 * @brief  Structure containing region location and size used by Video Output Monitor
 */
typedef struct {
    r_doc_Pos_t pos;                /*< start position of area to monitor @ref r_doc_Pos_t */
    r_doc_Rect_t rect;              /*< Range of area to monitor  @ref r_doc_Rect_t */
} r_doc_Region_t;

/**
 * @struct r_doc_Threshold_t
 * @brief  threshold of DOC
 */
typedef struct {
    uint8_t upper;                  /*< limit upper */
    uint8_t lower;                  /*< limit lower */
} r_doc_Threshold_t;

/**
 * @struct r_doc_Ref_Colors_t
 * @brief  Structure containing upper and lower thresholds for colors
 */
typedef struct {
    r_doc_Threshold_t red;          /*< Red color threshold @ref r_doc_Threshold_t */
    r_doc_Threshold_t blue;         /*< Blue color threshold @ref r_doc_Threshold_t */
    r_doc_Threshold_t green;        /*< Green color threshold @ref r_doc_Threshold_t */
} r_doc_Ref_Colors_t;

/**
 * @struct r_doc_Status_t
 * @brief  Structure containing status information of DOC
 */
typedef struct {
    bool activity_monitor;    /*< is Activity Monitor active */
    bool parameter_error;     /*< 0=no error, 1=error, see vom_error for specific vom number with error */
    bool vom_status;          /*< 0=vom not active in current video channel frame,
                                    1=vom active in current video channel frame */
    uint32_t current_monitor; /*< monitor number of current video channel frame */
    bool ch1_error;           /*< 0=no error, 1=error */
    bool ch0_error;           /*< 0=no error, 1=error */
    bool vom_error[16];       /*< 0=no error, 1=error*/
} r_doc_Status_t;

/** @} data types */
/**
 * \defgroup DOCapi   API
 * @ingroup DOCgroup
 * @{
 */

/**
 * Function R_DOC_EnableMonitors
 * @brief Enable monitors for specific DOC
 *
 * @param[in]   doc_unit     - DOC Unit @ref r_doc_Unit_t
 * @param[in]   monitors     - video monitors to enable (set bits for 0..15)
 *
 * @retval See @ref r_doc_Error_t
 */
r_doc_Error_t R_DOC_EnableMonitors(r_doc_Unit_t doc_unit, uint32_t monitors);

/**
 * Function R_DOC_DisableMonitors
 * @brief Disable monitors for specific DOC
 *
 * @param[in]   doc_unit     - DOC Unit @ref r_doc_Unit_t
 * @param[in]   monitors     - video monitors to disable (set bits for 0..15)
 *
 * @retval See @ref r_doc_Error_t
 */
r_doc_Error_t R_DOC_DisableMonitors(r_doc_Unit_t doc_unit, uint32_t monitors);

/**
 * Function R_DOC_EnableActMonitor
 * @brief Set Activity Monitor timing
 *
 * @param[in]   doc_unit    - DOC Unit @ref r_doc_Unit_t
 * @param[in]   Enable      - 1 to enable, 0 to disable
 *
 * @retval See @ref r_doc_Error_t
 */
r_doc_Error_t R_DOC_EnableActMonitor(r_doc_Unit_t doc_unit, bool Enable);

/**
 * Function R_DOC_SetActMonitorTiming
 * @brief Set Activity Monitor timing
 *
 * @param[in]   doc_unit    - DOC Unit. See @ref r_doc_Unit_t
 * @param[in]   doc_ch      - video channel. valid value R_DOC_VCH0, R_DOC_VCH1. See @ref r_doc_VideoChannel_t
 * @param[in]   upper_time  - upper detection time for Activity Monitor (0..4095)
 * @param[in]   lower_time  - lower detection time for Activity Monitor (0..4095)
 *
 * @retval See @ref r_doc_Error_t
 */
r_doc_Error_t R_DOC_SetActMonitorTiming(r_doc_Unit_t doc_unit, r_doc_VideoChannel_t doc_ch,
                                        uint32_t upper_time, uint32_t lower_time);

/**
 * Function R_DOC_Init
 * @brief Initialize video channel for DOC
 *
 * @param[in]   doc_unit     - DOC Unit @ref r_doc_Unit_t
 * @param[in]   doc_ch       - video channel. Valid value R_DOC_VCH0, R_DOC_VCH1. See @ref r_doc_VideoChannel_t
 * @param[in]   du_params    - DU Parameters @ref r_doc_Du_Params_t
 *
 * @retval See @ref r_doc_Error_t
 */
r_doc_Error_t R_DOC_Init(r_doc_Unit_t doc_unit, r_doc_VideoChannel_t doc_ch, r_doc_Du_Params_t du_params);

/**
 * Function R_DOC_SetMonitorColorInfo
 * @brief Configure Monitor Color Information
 * This api should be called after calling R_DOC_DisableMonitors()
 * and R_DOC_EnableActMonitor() with parameter of disable.
 *
 * @param[in]   doc_unit    - DOC Unit @ref r_doc_Unit_t
 * @param[in]   monitor_num - video monitor number (m=0..15)
 * @param[in]   region      - Region @ref r_doc_Region_t
 * @param[in]   threshold   - threshold for discriminator (0..0x3FFFF)
 * @param[in]   colors[4]   - reference colors @ref r_doc_Ref_Colors_t
 * @param[in]   clut        - 2bpp image using reference index colors
 *
 * @retval See @ref r_doc_Error_t
 */
r_doc_Error_t R_DOC_SetMonitorColorInfo(r_doc_Unit_t doc_unit, uint32_t monitor_num,
                r_doc_Region_t region, uint32_t threshold, r_doc_Ref_Colors_t colors[4], uint8_t *clut);

/**
 * Function R_DOC_SetMonitorRegion
 * @brief Configure Monitor Region
 * This api should be called after calling R_DOC_DisableMonitors()
 * and R_DOC_EnableActMonitor() with parameter of disable.
 *
 * @param[in]   doc_unit    - DOC Unit @ref r_doc_Unit_t
 * @param[in]   monitor_num - video monitor number (m=0..15)
 * @param[in]   region      - Region @ref r_doc_Region_t
 *
 * @retval See @ref r_doc_Error_t
 */
r_doc_Error_t R_DOC_SetMonitorRegion(r_doc_Unit_t doc_unit, uint32_t monitor_num, r_doc_Region_t region);

/**
 * Function R_DOC_SetMonitorChannel
 * @brief Configure Monitor Video Channel
 * This api should be called after calling R_DOC_DisableMonitors()
 * and R_DOC_EnableActMonitor() with parameter of disable.
 *
 * @param[in]   doc_unit    - DOC Unit @ref r_doc_Unit_t
 * @param[in]   doc_ch      - video channel, valid value R_DOC_VCH0, R_DOC_VCH1. See @ref r_doc_VideoChannel_t
 * @param[in]   monitor_num - video monitor number (m=0..15)
 *
 * @retval See @ref r_doc_Error_t
 */
r_doc_Error_t R_DOC_SetMonitorChannel(r_doc_Unit_t doc_unit, r_doc_VideoChannel_t doc_ch, uint32_t monitor_num);

/**
 * Function R_DOC_GetStatus
 * @brief Get Status information
 *
 * @param[in]   doc_unit    - DOC Unit @ref r_doc_Unit_t
 *
 * @retval See @ref r_doc_Status_t
 */
r_doc_Status_t R_DOC_GetStatus(r_doc_Unit_t doc_unit);

/**
 * Function R_DOC_ClearErrors
 * @brief Clear DOC errors
 * This api should be called after calling R_DOC_DisableMonitors()
 * and R_DOC_EnableActMonitor() with parameter of disable
 *
 * @param[in]   doc_unit        - DOC Unit @ref r_doc_Unit_t
 * @param[in]   parameter_error - 1 to clear
 * @param[in]   monitors        - bits for monitor errors to clear (0..15)
 *
 * @retval See @ref r_doc_Error_t
 */
r_doc_Error_t R_DOC_ClearErrors(r_doc_Unit_t doc_unit, bool parameter_error, uint32_t monitors);

/**
 * Function R_DOC_EnableIrq
 * @brief Enable the DOC IRQ
 *
 * @param[in]   doc_unit    - DOC Unit @ref r_doc_Unit_t
 * @param[in]   enable      - 1 to enable, 0 to disable
 *
 * @retval See @ref r_doc_Error_t
 */
r_doc_Error_t R_DOC_EnableIrq(r_doc_Unit_t doc_unit, bool enable);

/**
 * Function R_DOC_ClearDocIntStatus
 * @brief Clear Doc Interrupt status
 *
 * @param[in]   doc_unit    - DOC Unit @ref r_doc_Unit_t
 * @param[out]  vc0_status  - video channel 0 status (NULL is prohibited.)
 * @param[out]  vc1_status  - video channel 1 status (NULL is prohibited.)
 *
 * @retval See @ref r_doc_Error_t
 */
r_doc_Error_t R_DOC_ClearDocIntStatus(r_doc_Unit_t doc_unit, uint32_t *vc0_status, uint32_t *vc1_status);

/**
 * Function R_DOC_ClearDocIntStatusPost
 * @brief Clear Doc Interrupt status
 *
 * @param[in]   doc_unit    - DOC Unit @ref r_doc_Unit_t
 *
 * @retval See @ref r_doc_Error_t
 */
r_doc_Error_t R_DOC_ClearDocIntStatusPost(r_doc_Unit_t doc_unit);

/**
 * Function R_DOC_ClearActIntStatus
 * @brief Clear Act Interrupt status
 *
 * @param[in]   doc_unit    - DOC Unit @ref r_doc_Unit_t
 * @param[out]  status      - Active monitor status (NULL is prohibited.)
 *
 * @retval See @ref r_doc_Error_t
 */
r_doc_Error_t R_DOC_ClearActIntStatus(r_doc_Unit_t doc_unit, uint32_t *status);

/**
 * Function R_DOC_DeInit
 * @brief De-initialize and disable DOC unit
 *
 * @param[in]   doc_unit    - DOC Unit. See @ref r_doc_Unit_t
 * @param[in]   doc_ch      - video channel, valid value R_DOC_VCH0, R_DOC_VCH1. See @ref r_doc_VideoChannel_t
 *
 * @retval See @ref r_doc_Error_t
 */
r_doc_Error_t R_DOC_DeInit(r_doc_Unit_t doc_unit, r_doc_VideoChannel_t doc_ch);

/** @} api */

#endif /* R_DOC_API_H */
