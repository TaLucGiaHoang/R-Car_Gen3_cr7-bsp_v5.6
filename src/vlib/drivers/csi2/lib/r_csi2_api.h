/*************************************************************************************************************
* csi2_api_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#ifndef _CSI2_H_
#define _CSI2_H_

/**
 * \defgroup CSI2group   CSI2 driver
 * @ingroup drivers
 * @brief CSI2(Camera Serial Interface 2) driver interface
 * @{
 * @}
 */

/**
 * \defgroup CSI2consts   Constants
 * @ingroup CSI2group
 * @{
 */

#define RCAR_CSI2_CSI20_BASE                (0xFEA80000)        /**< CSI20 base address */
#define RCAR_CSI2_CSI40_BASE                (0xFEAA0000)        /**< CSI40 base address*/
#define RCAR_CSI2_CSI41_BASE                (0xFEAB0000)        /**< CSI41 base address */

#define RCAR_CSI2_80MBPS              0
#define RCAR_CSI2_90MBPS              1
#define RCAR_CSI2_100MBPS             2
#define RCAR_CSI2_110MBPS             3
#define RCAR_CSI2_120MBPS             4
#define RCAR_CSI2_130MBPS             5
#define RCAR_CSI2_140MBPS             6
#define RCAR_CSI2_150MBPS             7
#define RCAR_CSI2_160MBPS             8
#define RCAR_CSI2_170MBPS             9
#define RCAR_CSI2_180MBPS             10
#define RCAR_CSI2_190MBPS             11
#define RCAR_CSI2_205MBPS             12
#define RCAR_CSI2_220MBPS             13
#define RCAR_CSI2_235MBPS             14
#define RCAR_CSI2_250MBPS             15
#define RCAR_CSI2_275MBPS             16
#define RCAR_CSI2_300MBPS             17
#define RCAR_CSI2_325MBPS             18
#define RCAR_CSI2_350MBPS             19
#define RCAR_CSI2_400MBPS             20
#define RCAR_CSI2_450MBPS             21
#define RCAR_CSI2_500MBPS             22
#define RCAR_CSI2_550MBPS             23
#define RCAR_CSI2_600MBPS             24
#define RCAR_CSI2_650MBPS             25
#define RCAR_CSI2_700MBPS             26
#define RCAR_CSI2_750MBPS             27
#define RCAR_CSI2_800MBPS             28
#define RCAR_CSI2_850MBPS             29
#define RCAR_CSI2_900MBPS             30
#define RCAR_CSI2_950MBPS             31
#define RCAR_CSI2_1000MBPS    32
#define RCAR_CSI2_1050MBPS    33
#define RCAR_CSI2_1100MBPS    34
#define RCAR_CSI2_1150MBPS    35
#define RCAR_CSI2_1200MBPS    36
#define RCAR_CSI2_1250MBPS    37
#define RCAR_CSI2_1300MBPS    38
#define RCAR_CSI2_1350MBPS    39
#define RCAR_CSI2_1400MBPS    40
#define RCAR_CSI2_1450MBPS    41
#define RCAR_CSI2_1500MBPS    42

/** @} constants */

/**
 * \defgroup CSI2types   Data Types
 * @ingroup CSI2group
 * @{
 */

/**
 * @struct  r_Csi2_t
 * @brief   CSI2 configuration information
 */
typedef struct {
    uint32_t    Base;       /**< CSI2 base address */
    uint8_t     Lanes;      /**< Number of lanes to use */
    uint16_t    DataRate;   /**< MIPI input data rate (Mbps) from ADV7482 */
    bool        Interlaced; /**< true:Interlace, false:Progressive */
    uint16_t    CpgModule;  /**< CPG module */
    uint32_t    Vcdt;       /**< VCDT register */
    uint32_t    Vcdt2;      /**< VCDT2 register */
} r_Csi2_t;

/**
 * List of CSI2 channel supported
 */
typedef enum {
    RCAR_CSI20, /**< Only on H3,H3-N,M3-W,M3-W+,M3-N*/
    RCAR_CSI40, /**< Only on H3,H3-N,M3-W,M3-W+,V3M,V3H,M3-N,E3*/
    RCAR_CSI41  /**< Only on H3,V3H */
} r_csi2_Channel_t;

/**
 * List of CSI2 Errors
 */
typedef enum {
    R_CSI2_ERR_SUCCESS,           /**< Success */
    R_CSI2_ERR_FAILED,            /**< failed */
    R_CSI2_ERR_INVALID_PARAMETER, /**< An input parameter is incorrect */
    R_CSI2_ERR_PHY_TIMEOUT        /**< Timeout of reading the PHY */
} r_csi2_Error_t;

/** @} types */
/**
 * \defgroup CSI2api   API
 * @ingroup CSI2group
 * @{
 */

/**
 * Function   R_CSI2_Init
 * @brief     Initial setting of CSI2
 * @param[in] ch         - CSI2 channel, range RCAR_CSI20 - RCAR_CSI41. See @ref r_csi2_Channel_t
 * @param[in] DataRate   - MIPI input data rate (Mbps) from ADV7482
 * @param[in] Interlaced - true:Interlace, false:Progressive
 * @param[in] Lanes      - Number of lanes to use
 *
 * @retval    See  @ref r_csi2_Error_t
 *
 * @note Lanes - 1 if Ch is RCAR_CSI20. 1,2,4 if Ch is RCAR_CSI40
 *
 */
r_csi2_Error_t R_CSI2_Init(r_csi2_Channel_t Ch, uint16_t DataRate, bool Interlaced, uint8_t Lanes);

/**
 * Function   R_CSI2_DeInit
 * @brief     DeInitialize of CSI2
 * @param[in] Ch   - CSI2 channel, range RCAR_CSI20 - RCAR_CSI41. See @ref r_csi2_Channel_t
 *
 * @retval    See  @ref r_csi2_Error_t
 */
r_csi2_Error_t R_CSI2_DeInit(r_csi2_Channel_t Ch);

/** @} */ /* end of group CSI2group */

#endif /* _CSI2_H_ */
