/*************************************************************************************************************
* i2c_adv7482_api_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2018-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_ADV7482_API_H_
#define R_ADV7482_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup  ADV7482group   ADV7482 driver
 * \ingroup  I2CMaingroup
 * @brief ADV7482 driver interface
 * @{
 * @}
 */

/**
 * \defgroup ADV7482consts   Constants
 * @ingroup ADV7482group
 * @{
 */

/**
 *  ADV7482 I2C slave addresses for internal components
 */
/**
 * @def R_ADV7482_I2CADDR_XX
 * @brief definition of the ADV7482 register address
 */
#define R_ADV7482_I2CADDR_IO              0xE0            /* 8bits: IO        Map(Fixed) */
#define R_ADV7482_I2CADDR_DPLL            0x4C            /* 8bits: DPLL      Map(Programmable) */
#define R_ADV7482_I2CADDR_CP              0x44            /* 8bits: CP        Map(Programmable) */
#define R_ADV7482_I2CADDR_HDMI_RX         0x68            /* 8bits: HDMI Rx   Map(Programmable) */
#define R_ADV7482_I2CADDR_EDID            0x6C            /* 8bits: EDID      Map(Programmable) */
#define R_ADV7482_I2CADDR_REPEATER        0x64            /* 8bits: Repeater  Map(Programmable) */
#define R_ADV7482_I2CADDR_INFO            0x62            /* 8bits: InfoFrame Map(Programmable) */
#define R_ADV7482_I2CADDR_CBUS            0xF0            /* 8bits: CBUS      Map(Programmable) */
#define R_ADV7482_I2CADDR_CEC             0x82            /* 8bits: CEC       Map(Programmable) */
#define R_ADV7482_I2CADDR_SDP             0xF2            /* 8bits: SDP       Map(Programmable) */
#define R_ADV7482_I2CADDR_CSI_TXB         0x90            /* 8bits: CSI-TXB   Map(Programmable) */
#define R_ADV7482_I2CADDR_CSI_TXA         0x94            /* 8bits: CSI-TXA   Map(Programmable) */
#define R_ADV7482_I2CADDR_E_EDID          0xA0            /* 8bits: Internal E-EDID(Fixed) */
#define R_ADV7482_I2CADDR_HDCP            0x74            /* 8bits: HDCP Registers(Fixed) */

/**
 * @def R_ADV7482_COLOR_SPACE_OUT_XX
 * @brief definition of the ADV7482 color space output
 */
#define R_ADV7482_COLOR_SPACE_OUT_RGB      0x01            /* RGB color space output */
#define R_ADV7482_COLOR_SPACE_OUT_YCRCB    0x02            /* YCrCb color space output */

/**
 * @def R_ADV7482_BOUNDARY_XX
 * @brief definition of the ADV7482 boundary 600Mbps
 */
#define R_ADV7482_BOUNDARY_OVER_600MBPS    0x13            /* Over 600MBps */
#define R_ADV7482_BOUNDARY_UPTO_600MBPS    0x10            /* Up to 600MBps */

/** @} constants */
/**
 * \defgroup ADV7482types   Data Types
 * @ingroup ADV7482group
 * @{
 */

/**
 * @enum r_adv7482_I2cChannel_t
 * @brief list of the I2C channels supporting ADV7482
 */
typedef enum {
    R_ADV7482_I2C_CH0,  /**< Channel 0 */
    R_ADV7482_I2C_CH1,  /**< Channel 1 */
    R_ADV7482_I2C_CH2,  /**< Channel 2 */
    R_ADV7482_I2C_CH3,  /**< Channel 3 */
    R_ADV7482_I2C_CH4,  /**< Channel 4 */
    R_ADV7482_I2C_CH5,  /**< Channel 5 */
    R_ADV7482_I2C_CH6,  /**< Channel 6 */
    R_ADV7482_I2C_CH7   /**< Channel 7 */
} r_adv7482_I2cChannel_t;

/**
 * @enum  r_adv7482_Error_t
 * @brief  ADV7482 Errors
 */
typedef enum {
    R_ADV7482_ERR_SUCCESS,           /**< Success */
    R_ADV7482_ERR_FAILED,            /**< failed */
    R_ADV7482_ERR_INVALID_PARAMETER, /**< An input parameter is incorrect */
    R_ADV7482_ERR_TIMEOUT           /**< Timed out */
} r_adv7482_Error_t;

/** @} types */
/**
 * \defgroup ADV7482api   API
 * @ingroup ADV7482group
 * @{
 */

/**
 * Function R_ADV7482_Init
 * @brief Initialise ADV7482
 *
 * @param[in] I2c - I2c channel to be used for ADV7482. See @ref r_adv7482_I2cChannel_t
 * @param[in] ColorSpaceOut - hdmi color space output from ADV7482
 * @param[in] BoundaryMbps - hdmi boundary 600Mbps from ADV7482
 *
 * @retval    See  @ref r_adv7482_Error_t
 *
 * @note ColorSpaceOut(Select R_ADV7482_COLOR_SPACE_OUT_RGB,R_ADV7482_COLOR_SPACE_OUT_YCRCB) see @ref r_vin_Config_t
 * @note BoundaryMbps(Select R_ADV7482_BOUNDARY_OVER_600MBPS,R_ADV7482_BOUNDARY_UPTO_600MBPS) see @ref r_vin_Config_t
 *
 */
r_adv7482_Error_t R_ADV7482_Init(r_adv7482_I2cChannel_t I2c, uint8_t ColorSpaceOut, uint8_t BoundaryMbps);

/**
 * Function R_ADV7482_Start
 * @brief Start the decodeur ADV7482
 *
 * @param[in] I2c  - I2c channel to be used for ADV7482. See @ref r_adv7482_I2cChannel_t
 * @param[in] lane - Number of lanes to use (Settable number 1)
 *
 * @retval    See  @ref r_adv7482_Error_t
 *
 */
r_adv7482_Error_t R_ADV7482_Start(r_adv7482_I2cChannel_t I2c, uint8_t lane);
/**
 * Function R_ADV7482_Stop
 * @brief Stop the decodeur ADV7482
 *
 * @param[in] I2c  - I2c channel to be used for ADV7482. See @ref r_adv7482_I2cChannel_t
 * @param[in] lane - Number of lanes to use (Settable number 1)
 *
 * @retval    See  @ref r_adv7482_Error_t
 *
 */
r_adv7482_Error_t R_ADV7482_Stop(r_adv7482_I2cChannel_t I2c, uint8_t lane);

/**
 * Function R_ADV7482_Start_csi40
 * @brief Start the decodeur ADV7482
 *
 * @param[in] I2c  - I2c channel to be used for ADV7482. See @ref r_adv7482_I2cChannel_t
 * @param[in] lane - Number of lanes to use (Settable number 1,2,4)
 *
 * @retval    See  @ref r_adv7482_Error_t
 *
 */
r_adv7482_Error_t R_ADV7482_Start_csi40(r_adv7482_I2cChannel_t I2c, uint8_t lane);

/**
 * Function R_ADV7482_Stop_csi40
 * @brief Stop the decodeur ADV7482
 *
 * @param[in] I2c  - I2c channel to be used for ADV7482. See @ref r_adv7482_I2cChannel_t
 * @param[in] lane - Number of lanes to use (Settable number 1,2,4)
 *
 * @retval    See  @ref r_adv7482_Error_t
 *
 */
r_adv7482_Error_t R_ADV7482_Stop_csi40(r_adv7482_I2cChannel_t I2c, uint8_t lane);

/**
 * Function R_ADV7482_tmds_stable_wait
 * @brief Wait for TMDS clock frequency to stabilize
 *
 * @param[in] I2c - I2c channel to be used for ADV7482. See @ref r_adv7482_I2cChannel_t
 *
 * @retval    See  @ref r_adv7482_Error_t
 *
 */
r_adv7482_Error_t R_ADV7482_tmds_stable_wait(r_adv7482_I2cChannel_t I2c);

/** @} api */
#ifdef __cplusplus
}
#endif

#endif /* R_ADV7482_API_H_ */
