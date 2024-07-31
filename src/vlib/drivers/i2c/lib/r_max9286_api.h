/*************************************************************************************************************
* i2c_max9286_api_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#ifndef R_MAX9286_API_H_
#define R_MAX9286_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BIT(nr)			(1UL << (nr))
#define MAX9271_ID      0x9
#define MAX96705_ID		0x41
#define MAX9286_ID		0x40

#define	MAX9286_I2C_ADDR		0x90
#define MAX9271_I2C_ADDR		0x80
#define CAMERA_LINKS            0x4


/**
 * @enum r_MAX9286_I2cChannel_t
 * @brief list of the I2C channels supporting MAX9286
 */
typedef enum {
    R_MAX9286_I2C_CH0,  /**< Channel 0 */
    R_MAX9286_I2C_CH1,  /**< Channel 1 */
    R_MAX9286_I2C_CH2,  /**< Channel 2 */
    R_MAX9286_I2C_CH3,  /**< Channel 3 */
    R_MAX9286_I2C_CH4,  /**< Channel 4 */
    R_MAX9286_I2C_CH5,  /**< Channel 5 */
    R_MAX9286_I2C_CH6,  /**< Channel 6 */
    R_MAX9286_I2C_CH7   /**< Channel 7 */
} r_MAX9286_I2cChannel_t;


int max9286_reg8_write(uint8_t reg, uint8_t val);
int max9286_reg8_read(uint8_t reg, uint8_t* val);
int max9271_reg8_write(uint8_t reg, uint8_t val);
int max9271_reg8_read(uint8_t reg, uint8_t* val);
int reg8_write(uint8_t i2c_addr, uint8_t reg, uint8_t val);
int reg8_read(uint8_t i2c_addr, uint8_t reg, uint8_t* val);

uint8_t  MAX9271_MAP_ADDR(int port);
void R_MAX9286_Init();
void R_MAX9286_Start();
void R_MAX9286_Stop(void);
void R_MAX9286_PowerOn(void);
void R_MAX9286_PowerOff(void);
int R_GET_MAX9286_Ch(void);

/** @} */ /* enf of group MAX9286group */
#ifdef __cplusplus
}
#endif

#endif /* R_MAX9286_API_H_ */
