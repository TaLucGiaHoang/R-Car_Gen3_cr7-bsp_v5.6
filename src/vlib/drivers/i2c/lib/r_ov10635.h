/*************************************************************************************************************
* i2c_ov10635_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#ifndef R_OV10635_API_H_
#define R_OV10635_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#define OV10635_SENSOR_WIDTH		1312
#define OV10635_SENSOR_HEIGHT		814

#define OV10635_MAX_WIDTH		1280
#define OV10635_MAX_HEIGHT		800

#define	OV10635_I2C_ADDR		0x60

/**
 * Function R_OV10635_Init
 * @brief Initialise OV10635
 *
 * @param[in] I2c - I2c channel to be used for OV10635. See @ref r_MAX9286_I2cChannel_t I2c
 *
 */
void R_OV10635_Init();

/**
 * Function R_OV10635_Start
 * @brief Start the decodeur OV10635
 *
 * @param[in] I2c - I2c channel to be used for OV10635. See @ref r_MAX9286_I2cChannel_t I2c
 *
 */
void R_OV10635_Start();

/**
 * Function R_OV10635_Stop
 * @brief Stop the decodeur OV10635
 *
 * @param[in] I2c - I2c channel to be used for OV10635. See @ref r_MAX9286_I2cChannel_t I2c
 *
 */
void R_OV10635_Stop();
#ifdef __cplusplus
}
#endif

#endif /* R_MAX9286_API_H_ */