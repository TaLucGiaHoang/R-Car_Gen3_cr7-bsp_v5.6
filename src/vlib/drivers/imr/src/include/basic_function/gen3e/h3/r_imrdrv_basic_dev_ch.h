/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_basic_dev_ch.h
* Product Name : IMR Driver
* Device(s)    : R-Car H3
* Description  : IMR Driver Basic Function Device Dependent Control Public header
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 29.10.2021 0.01     New create
 *********************************************************************************************************************/

#ifndef R_IMRDRV_BASIC_DEV_CH_H
#define R_IMRDRV_BASIC_DEV_CH_H
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define IMRDRV_IMR_CHANNEL_NUM      (4U)    /* The number of IMR channel. */

/* Validity of Channel number for function r_imrdrv_devctl_check_ch() */
#define IMRDRV_DEV_CHANNEL_0_ISVALID  IMRDRV_ERROR_OK
#define IMRDRV_DEV_CHANNEL_1_ISVALID  IMRDRV_ERROR_OK
#define IMRDRV_DEV_CHANNEL_2_ISVALID  IMRDRV_ERROR_OK
#define IMRDRV_DEV_CHANNEL_3_ISVALID  IMRDRV_ERROR_OK

/* IO device type */
#define IMRDRV_IO_DEV_TYPE_CH0  "imr" /* IO device type for CH0 */
#define IMRDRV_IO_DEV_TYPE_CH1  "imr" /* IO device type for CH1 */
#define IMRDRV_IO_DEV_TYPE_CH2  "imr" /* IO device type for CH2 */
#define IMRDRV_IO_DEV_TYPE_CH3  "imr" /* IO device type for CH3 */

/* IO device name */
#define IMRDRV_IO_DEV_NAME_CH0  "imr_00" /* IO device name for CH0 */
#define IMRDRV_IO_DEV_NAME_CH1  "imr_01" /* IO device name for CH1 */
#define IMRDRV_IO_DEV_NAME_CH2  "imr_02" /* IO device name for CH2 */
#define IMRDRV_IO_DEV_NAME_CH3  "imr_03" /* IO device name for CH3 */

/* Device type Name */
#define IMRDRV_DEVICE_TYPE_NAME \
    {   /* channel number,    IO device type,         IO device name         */ \
        { IMRDRV_CHANNELNO_0, IMRDRV_IO_DEV_TYPE_CH0, IMRDRV_IO_DEV_NAME_CH0 }, \
        { IMRDRV_CHANNELNO_1, IMRDRV_IO_DEV_TYPE_CH1, IMRDRV_IO_DEV_NAME_CH1 }, \
        { IMRDRV_CHANNELNO_2, IMRDRV_IO_DEV_TYPE_CH2, IMRDRV_IO_DEV_NAME_CH2 }, \
        { IMRDRV_CHANNELNO_3, IMRDRV_IO_DEV_TYPE_CH3, IMRDRV_IO_DEV_NAME_CH3 }, \
    }

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* R_IMRDRV_BASIC_DEV_CH_H */
