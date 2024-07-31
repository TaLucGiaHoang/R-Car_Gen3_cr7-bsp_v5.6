/*************************************************************************************************************
* vin_api_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2018-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_VIN_API_H_
#define R_VIN_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * \defgroup VINgroup   VIN driver
 * @ingroup drivers
 * @brief Video Input driver interface
 * @{
 * @}
 */

/**
 * \defgroup VINconsts   Constants
 * @ingroup VINgroup
 * @{
 */

/**
 * @def    R_VIN_VMC_INF_XX
 * @brief   List of VIn VMC register INF bit supported
 */
#define    R_VIN_VMC_INF_RGB888      0x01     /**< VIN VMC INF RGB888 */
#define    R_VIN_VMC_INF_YUV8_BT601  0x02     /**< VIN VMC INF YUV8_BT601 */

/**
 * @def    R_VIN_VMC_IM_XX
 * @brief   List of VIn VMC register IM bit supported
 */
#define R_VMC_IM_ODD          (0 << 3)
#define R_VMC_IM_ODD_EVEN     (1 << 3)
#define R_VMC_IM_EVEN         (2 << 3)
#define R_VMC_IM_FULL         (3 << 3)

/** @} consts */
/**
 * \defgroup VINtypes   Data Types
 * @ingroup VINgroup
 * @{
 */

/**
 * List of VIn channel supported
 */
typedef enum {
    R_VIN0,     /**< VIN Channel 0 */
    R_VIN1,     /**< VIN Channel 1 */
    R_VIN2,     /**< VIN Channel 2 */
    R_VIN3,     /**< VIN Channel 3 */
    R_VIN4,     /**< VIN Channel 4 */
    R_VIN5,     /**< VIN Channel 5 */
    R_VIN6,     /**< VIN Channel 6 */
    R_VIN7,     /**< VIN Channel 7 */
    R_VIN_LAST  /**< delimiter */
} r_vin_Channel_t;

/**
 * VIN Onput Interface Format
 */
typedef enum {
    R_VIN_OUTF_YCBCR420,  /**< VIN Output Format YcBcr420 */
    R_VIN_OUTF_ARGB8888   /**< VIN Output Format ARGB8888 */
} r_vin_OUTF_t;

/**
 * VIN Errors
 */
typedef enum {
    R_VIN_ERR_SUCCESS,               /**< Success */
    R_VIN_ERR_FAILED,                /**< failed */
    R_VIN_ERR_INVALID_PARAMETER,     /**< An input parameter is incorrect */
    R_VIN_ERR_OUT_OF_RANGE,          /**< out of range */
    R_VIN_ERR_TIMEOUT                /**< timeout */
} r_vin_Error_t;

/**
 * @struct  r_vin_Config_t
 * @brief  Video input configuration information
 */
typedef struct {
    int16_t Cam_Res_X;  /**< length of the camera horizontal line*/
    int16_t Cam_Res_Y;  /**< length of the camera vertical line*/
    int16_t Scaled_X;   /**< length of the scaled image horizontal line */
    int16_t Scaled_Y;   /**< length of the scaled image vertical line */
    int16_t Out_Res_X;  /**< CL_HSIZE[11:0] of VnUDS_CLIP_SIZE */
    int16_t Out_Res_Y;  /**< CL_VSIZE[11:0] of VnUDS_CLIP_SIZE */
    bool  Interlaced;   /**< true:Interlace, false:Progressive */
    int32_t Offset;     /**< UV Data Address Offset (unit:byte) */
    int32_t Stride;     /**< Data size per one horizontal line (pixel)*/
    uint8_t VmcInf;     /**< VIN VMC register INF bit */
    uint8_t VmcIm;      /**< VIN VMC register IM bit */
    r_vin_OUTF_t OutputFmt; /**< vin output format */
    uint32_t pos_x;    /**< Start Pixel Pre-Clip (SPPrC[11:0] of VnSPPrC) */
    uint32_t pos_y;    /**< Start Pixel Pre-Clip (SLPrC[11:0] of VnSLPrC) */
} r_vin_Config_t;
/** @} data types */

/**
 * \defgroup VINapi   API
 * @ingroup VINgroup
 * @{
 */

/**
 * Function R_VIN_Start
 * @brief   Start the video capture
 *
 * @param[in] Ch    - Vin channel. See @ref r_vin_Channel_t
 * @param[in] Conf  - pointer to the configuration information(must not be NULL).
 *                    See @ref r_vin_Config_t
 *
 * @note Conf->OutputFmt see @ref r_vin_Config_t
 */
void    R_VIN_Start(r_vin_Channel_t Ch, r_vin_Config_t *Conf);

/**
 * Function R_VIN_Stop
 * @brief   Stop the video capture
 *
 * @param[in] Ch - Vin channel. See @ref r_vin_Channel_t
 *
 * @retval    See  @ref r_vin_Error_t
 */
r_vin_Error_t R_VIN_Stop(r_vin_Channel_t Ch);

/**
 * Function R_VIN_Init
 * @brief   Configure the video input channel, Screen specifications, and buffer address(Single Frame Capture Mode).
 *
 * @param[in] Ch            - Vin channel. See @ref r_vin_Channel_t
 * @param[in] Conf          - pointer to the configuration information(must not be NULL).
 *                            See @ref r_vin_Config_t
 * @param[in] BufferAddr    - Address of the capture buffer
 *
 * @retval    See  @ref r_vin_Error_t
 *
 * @note Conf->VmcInf(Select R_VIN_VMC_INF_RGB888,R_VIN_VMC_INF_YUV8_BT601) see @ref r_vin_Config_t
 * @note Conf->OutputFmt see @ref r_vin_Config_t
 * @note Conf->VmcIm(Select R_VMC_IM_ODD,R_VMC_IM_ODD_EVEN,R_VMC_IM_EVEN,R_VMC_IM_FULL) see @ref r_vin_Config_t
 * @note Conf->Cam_Res_X, Conf->Cam_Res_Y, Conf->Scaled_X, Conf->Scaled_Y see @ref r_vin_Config_t
 * @note Conf->pos_x , Conf->pos_y , Conf->Interlaced, Conf->Stride and Conf->Offset see @ref r_vin_Config_t
 */
r_vin_Error_t R_VIN_Init(r_vin_Channel_t Ch, r_vin_Config_t *Conf,
                         uintptr_t BufferAddr);

/**
 * Function R_VIN_InitEx
 * @brief   Configure the video input channel, Screen specifications, and buffer address(Continuous Frame Capture Mode).
 *
 * @param[in] Ch            - Vin channel. See @ref r_vin_Channel_t
 * @param[in] Conf          - pointer to the configuration information(must not be NULL).
 *                            See @ref r_vin_Config_t
 * @param[in] BufferAddr1   - Address of the capture buffer 1
 * @param[in] BufferAddr2   - Address of the capture buffer 2
 * @param[in] BufferAddr3   - Address of the capture buffer 3
 *
 * @retval    See  @ref r_vin_Error_t
 *
 * @note Conf->VmcInf(Select R_VIN_VMC_INF_RGB888,R_VIN_VMC_INF_YUV8_BT601) see @ref r_vin_Config_t
 * @note Conf->OutputFmt see @ref r_vin_Config_t
 * @note Conf->VmcIm(Select R_VMC_IM_ODD,R_VMC_IM_ODD_EVEN,R_VMC_IM_EVEN,R_VMC_IM_FULL) see @ref r_vin_Config_t
 * @note Conf->Cam_Res_X, Conf->Cam_Res_Y, Conf->Scaled_X, Conf->Scaled_Y see @ref r_vin_Config_t
 * @note Conf->pos_x , Conf->pos_y , Conf->Stride and Conf->Offset see @ref r_vin_Config_t
 */
r_vin_Error_t R_VIN_InitEx(r_vin_Channel_t Ch, r_vin_Config_t *Conf,
           uintptr_t BufferAddr1, uintptr_t BufferAddr2, uintptr_t BufferAddr3);

/**
 * Function R_VIN_GetLatestFrameBufIndex
 * @brief   Get latest valid frame buffer slot.
 *
 * @param[in] Ch    - Vin channel. See @ref r_vin_Channel_t
 *
 * @retval Index of frame buffer specified by the R_VIN_InitEx ()
 * @retval   0: BufferAddr1
 * @retval   1: BufferAddr2
 * @retval   2: BufferAddr3
 * @retval   3: No valid buffer.
 */
int32_t R_VIN_GetLatestFrameBufIndex(r_vin_Channel_t Ch);

/**
 * Function R_VIN_GetLatestFrameBuffer
 * @brief   Get latest valid frame buffer address.
 *
 * @param[in] Ch    - Vin channel. See @ref r_vin_Channel_t
 *
 * @retval Address of frame buffer.
 */
uintptr_t R_VIN_GetLatestFrameBuffer(r_vin_Channel_t Ch);

/**
 * Function R_VIN_Release
 * @brief   Release the video input channel and its parent channel
 *
 * @param[in] Ch - Vin channel. See @ref r_vin_Channel_t
 *
 * @retval    See  @ref r_vin_Error_t
 */
r_vin_Error_t R_VIN_Release(r_vin_Channel_t Ch);

/**
 * Function R_VIN_ScaleImage
 * @brief   Enable scaling
 *
 * @param[in] Ch    - Vin channel, valid value R_VIN0/R_VIN1/R_VIN4/R_VIN5.
 *                    See @ref r_vin_Channel_t
 * @param[in] Conf  - pointer to the configuration information(must not be NULL).
 *                    See @ref r_vin_Config_t
 * @retval    see @ref r_vin_Error_t
 *
 * @note Conf->Scaled_X - Must be an even value in the range of 4 to 2048.
 * @note Conf->Scaled_Y - Must be a value in the range of 4 to 2048.
 * @note Conf->Cam_Res_X, Conf->Cam_Res_Y, Conf->Out_Res_X, Conf->Out_Res_Y see @ref r_vin_Config_t
 * @note Conf->pos_x, Conf->pos_y, Conf->Interlaced see @ref r_vin_Config_t
 */
r_vin_Error_t R_VIN_ScaleImage(r_vin_Channel_t Ch, r_vin_Config_t *Conf);

/**
 * Function R_VIN_EnableInterrupt
 * @brief   Enable interrupt.
 *
 * @param[in] Ch   - Vin channel. See @ref r_vin_Channel_t
 * @param[in] VnIE - Value of VnIE register.
 */
void R_VIN_EnableInterrupt(r_vin_Channel_t Ch, uint32_t VnIE);

/**
 * Function R_VIN_DisableInterrupt
 * @brief   Disable interrupt.
 *
 * @param[in] Ch   - Vin channel. See @ref r_vin_Channel_t
 * @param[in] VnIE - Value of VnIE register.
 */
void R_VIN_DisableInterrupt(r_vin_Channel_t Ch, uint32_t VnIE);

/**
 * Function R_VIN_ClearInterrupt
 * @brief   Clear all interrupt status.
 *
 * @param[in] Ch   - Vin channel. See @ref r_vin_Channel_t
 */
void R_VIN_ClearInterrupt(r_vin_Channel_t Ch);

/**
 * Function R_VIN_Prepare
 * @brief   Preparation for VIN initialization processing.
 *
 * @param[in] Ch              - Vin channel. See @ref r_vin_Channel_t
 * @param[in] Chsel           - VnCSI_IFMD CSI_CHSEL bit please refer rcar H/W specification 26.2.8
 * @param[in] Conf(not used)  - pointer to the configuration information. See @ref r_vin_Config_t
 *
 * @retval    See  @ref r_vin_Error_t
 *
 */
r_vin_Error_t R_VIN_Prepare(r_vin_Channel_t Ch, uint8_t Chsel, r_vin_Config_t *Conf);

/**
 * Function R_VIN_DeInit
 * @brief   DeInitialize.
 *
 * @param[in] Ch    - Vin channel. See @ref r_vin_Channel_t
 *
 * @retval    See  @ref r_vin_Error_t
 */
r_vin_Error_t R_VIN_DeInit(r_vin_Channel_t Ch);

/** @} */ /* end of group VINgroup */

#ifdef __cplusplus
}
#endif

#endif /* R_API_VIN_H_ */
