/*************************************************************************************************************
* vsp2_api_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_VSP2_APIH_
#define R_VSP2_APIH_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * \defgroup VSPgroup VSP2 driver
 * @ingroup drivers
 * @brief   Video Signal Processor (VSP2) driver interface
 *
 * @{
 * @}
 */

/**
 * \defgroup VSPconsts   Constants
 * @ingroup VSPgroup
 * @{
 */

#define R_VSP2_ENABLE   (1u)        /**< VSP is enabled macro */
#define R_VSP2_DISABLE  (0u)        /**< VSP is disabled macro */

/** @} constants */

/**
 * \defgroup VSPtypes   Data Types
 * @ingroup VSPgroup
 * @{
 */

/**
 * List of errors VSP2 API can return
 */
typedef enum {
    R_VSP2_ERR_SUCCESS,             /**< success */
    R_VSP2_ERR_INVALID_PARAMETER,   /**< invalid parameter */
    R_VSP2_ERR_INVALID_STATE,       /**< invalid state */
} r_vsp2_Error_t;


/**
 * List the types of VSP2 units available
 */
typedef enum {
    R_VSP2_VSPBC,       /**< VSP BC unit */
    R_VSP2_VSPBD,       /**< VSP BD unit */
    R_VSP2_VSPB,        /**< VSP B  unit */
    R_VSP2_VSPBS,       /**< VSP BS unit */
    R_VSP2_VSPI0,       /**< VSP I0 unit */
    R_VSP2_VSPI1,       /**< VSP I1 unit */
    R_VSP2_VSPD0,       /**< VSP D0 unit */
    R_VSP2_VSPDL,       /**< VSP DL unit */
    R_VSP2_VSPD1,       /**< VSP D1 unit */
    R_VSP2_VSPD2,       /**< VSP D2 unit */
    R_VSP2_LAST         /**< delimiter */
} r_vsp2_Unit_t;

/**
 * List the interrupt types supported by VSP2
 */
typedef enum {
    R_VSP_INT_UND_WPF0 = (1 << 0),      /**< WPF0 underrun */
    R_VSP_INT_DFE_WPF0 = (1 << 1),      /**< WPF0 Frame start */
    R_VSP_INT_FRE_WPF0 = (1 << 2),      /**< WPF0 Frame End */
    R_VSP_INT_UND_WPF1 = (1 << 3),      /**< WPF1 underrun */
    R_VSP_INT_DFE_WPF1 = (1 << 4),      /**< WPF1 Frame start */
    R_VSP_INT_FRE_WPF1 = (1 << 5),      /**< WPF1 Frame End */
} r_vsp2_Int_t;

/**
 * List of DPR Route Config Setting Value
 *
 */
typedef enum
{
    R_VSP_DPR_ROUTE_UIF4        = 12,   /**< UIF4     DPR Route */
    R_VSP_DPR_ROUTE_UIF5        = 13,   /**< UIF5     DPR Route */
    R_VSP_DPR_ROUTE_SRU         = 16,   /**< SRU      DPR Route */
    R_VSP_DPR_ROUTE_UDS         = 17,   /**< UDS      DPR Route */
    R_VSP_DPR_ROUTE_LUT         = 22,   /**< LUT      DPR Route */
    R_VSP_DPR_ROUTE_CLU         = 29,   /**< CLU      DPR Route */
    R_VSP_DPR_ROUTE_HST         = 30,   /**< HST      DPR Route */
    R_VSP_DPR_ROUTE_HSI         = 31,   /**< HSI      DPR Route */
    R_VSP_DPR_ROUTE_SHP         = 46,   /**< SHP      DPR Route */
    R_VSP_DPR_ROUTE_BRU0        = 23,   /**< BRU0     DPR Route */
    R_VSP_DPR_ROUTE_BRU1        = 24,   /**< BRU1     DPR Route */
    R_VSP_DPR_ROUTE_BRU2        = 25,   /**< BRU2     DPR Route */
    R_VSP_DPR_ROUTE_BRU3        = 26,   /**< BRU3     DPR Route */
    R_VSP_DPR_ROUTE_BRU4        = 49,   /**< BRU4     DPR Route */
    R_VSP_DPR_ROUTE_BRS_ILV0    = 38,   /**< BRS/ILV0 DPR Route */
    R_VSP_DPR_ROUTE_BRS_ILV1    = 39,   /**< BRS/ILV0 DPR Route */
    R_VSP_DPR_ROUTE_WPF0        = 56,   /**< WPF0     DPR Route */
    R_VSP_DPR_ROUTE_WPF1        = 57,   /**< WPF1     DPR Route */
    R_VSP_DPR_ROUTE_BRS_TO_WPF0 = (R_VSP_DPR_ROUTE_WPF0 | 0x10000000),   /**< WPF0     DPR Route for VI6_DPR_ILV_BRS_ROUTE */
    R_VSP_DPR_ROUTE_BRS_TO_WPF1 = (R_VSP_DPR_ROUTE_WPF1 | 0x10000000),   /**< WPF1     DPR Route for VI6_DPR_ILV_BRS_ROUTE */
    R_VSP_DPR_ROUTE_DISABLE     = 63,   /**< Disable */
} r_vsp2_Dpr_Route_t;

/**
 * @struct  r_vsp2_Dpr_Conf_t
 * @brief    VSP2 DPR configuration information
 */
typedef struct {
    r_vsp2_Dpr_Route_t Dpr_Rpf0_Route;     /**< Vi6_Dpr_Rpf0_Route regster */
    r_vsp2_Dpr_Route_t Dpr_Rpf1_Route;     /**< Vi6_Dpr_Rpf1_Route regster */
    r_vsp2_Dpr_Route_t Dpr_Rpf2_Route;     /**< Vi6_Dpr_Rpf2_Route regster */
    r_vsp2_Dpr_Route_t Dpr_Rpf3_Route;     /**< Vi6_Dpr_Rpf3_Route regster */
    r_vsp2_Dpr_Route_t Dpr_Rpf4_Route;     /**< Vi6_Dpr_Rpf4_Route regster */
    r_vsp2_Dpr_Route_t Dpr_Sru_Route;      /**< Vi6_Dpr_Sru_Route regster */
    r_vsp2_Dpr_Route_t Dpr_Uds_Route;      /**< Vi6_Dpr_Uds_Route regster */
    r_vsp2_Dpr_Route_t Dpr_Lut_Route;      /**< Vi6_Dpr_Lut_Route regster */
    r_vsp2_Dpr_Route_t Dpr_Clu_Route;      /**< Vi6_Dpr_Clu_Route regster */
    r_vsp2_Dpr_Route_t Dpr_Hst_Route;      /**< Vi6_Dpr_Hst_Route regster */
    r_vsp2_Dpr_Route_t Dpr_Hsi_Route;      /**< Vi6_Dpr_Hsi_Route regster */
    r_vsp2_Dpr_Route_t Dpr_Bru_Route;      /**< Vi6_Dpr_Bru_Route regster */
    r_vsp2_Dpr_Route_t Dpr_Ilv_Brs_Route;  /**< Vi6_Dpr_Ilv_Brs_Route regster */
    r_vsp2_Dpr_Route_t Dpr_Shp_Route;      /**< Vi6_Dpr_Shp_Route regster */
    r_vsp2_Dpr_Route_t Dpr_Uif4_Route;     /**< Vi6_Dpr_Uif4_Route regster */
    r_vsp2_Dpr_Route_t Dpr_Uif5_Route;     /**< Vi6_Dpr_Uif5_Route regster */
} r_vsp2_Dpr_Conf_t;

/**
 * @struct  r_vsp2_Conf_t
 * @brief   VSP2 configuration information
 */
typedef struct {
    r_vsp2_Dpr_Conf_t Dpr_Conf;     /**< Data Path Route config */
} r_vsp2_Conf_t;

/** @} types */
/**
 * \defgroup VSPapi   API
 * @ingroup VSPgroup
 * @{
 */

/**
 * Initialise the VSP2 given Unit
 *
 * VSP2 driver does not have the DeInitialize API, therefore after called this function,
 * target VSP2 Unit can not close.
 *
 * @param[in]   Vsp2Unit        - Vsp2 Unit, range R_VSP2_VSPBC - R_VSP2_VSPD2
 *                                See @ref r_vsp2_Unit_t
 * @param[in]   WpfId           - Wpf number. 0:WPF0, 1:WPF1
 * @param[in]   ScreenWidth     - Width of the screen in pixels, range 1 - 8190
 * @param[in]   ScreenHeight    - Height of the screen in pixels, range 1 - 8190
 * @param[in]   DlAddr          - Display list address, must be 0
 * @param[in]   Dpr_Conf        - Dpr Config @ref r_vsp2_Dpr_Conf_t
 * @param[in]   InitFlag        - Number of times R_VSP2_Init is called
 * @retval      See  @ref r_vsp2_Error_t
 *
 * @note Currently the display list is not supported. This function will return
 * with an error if the DlAddr is not 0
 */
r_vsp2_Error_t R_VSP2_Init(r_vsp2_Unit_t      Vsp2Unit,
                           uint32_t           WpfId,
                           uint32_t           ScreenWidth,
                           uint32_t           ScreenHeight,
                           uintptr_t          DlAddr,
                           r_vsp2_Dpr_Conf_t *Dpr_Conf,
                           uint32_t           InitFlag);

/**
 * Start the VSP2 unit uding the given WPF
 *
 * @param[in]   Vsp2Unit - Vsp2 Unit, range R_VSP2_VSPBC - R_VSP2_VSPD2
 *                         See @ref r_vsp2_Unit_t
 * @param[in]   WpfId    - 0 for Wpf0, 1 for WPf1
 * @retval      See  @ref r_vsp2_Error_t
 */
r_vsp2_Error_t R_VSP2_Start(r_vsp2_Unit_t Vsp2Unit, uint32_t WpfId);


/**
 * Enable/disable the blending between the Rpfs
 *
 * This function enables Blending operation for the given VSPD layer.
 * As the BRU configuration is fixed, this function looks for the Blendingrop unit
 * of the given layer and enables the rendering operation for it.
 * The rendering operation is fixed; the formula is :
 *      color data: src_a * src_c + (1 - src_a) * dst_c
 *      alpha data: const 1
 *
 * @param[in]   Vsp2Unit - Vsp2 Unit @ref r_vsp2_Unit_t
 * @param[in]   Rpf      - Rpf reference, correspond to the PosZ. Range 0 - 5.
 * @param[in]   Enable   - 0 disable, 1 enable
 * @retval      See @ref r_vsp2_Error_t
 */
r_vsp2_Error_t R_VSP2_BlendingEnable(r_vsp2_Unit_t Vsp2Unit, uint32_t Rpf,
                                     uint32_t Enable);

/**
 * Control the color display background
 *
 * This function controls the color of the virtual
 * read layer (lowest level background).
 *
 * @param[in]   Vsp2Unit - Vsp2 Unit, range R_VSP2_VSPBC - R_VSP2_VSPB, R_VSP2_VSPD0 - R_VSP2_VSPD2
 *                         See @ref r_vsp2_Unit_t
 * @param[in]   Color    - ARGB format
 * @retval      See @ref r_vsp2_Error_t
 */
r_vsp2_Error_t R_VSP2_BkgColorSet(r_vsp2_Unit_t Vsp2Unit,
                                  uint32_t Color);

/**
 * Function to be called when a VSP2 ISR occurs
 *
 * @param[in]   Vsp2Unit  - Vsp2 Unit, range R_VSP2_VSPBC - R_VSP2_VSPD2. See @ref r_vsp2_Unit_t
 * @param[in]   Wpf0InUse - 0:wpf0 is not in use. 1:wpf0 is n use.
 * @param[in]   Wpf1InUse - 0:wpf1 is not in use. 1:wpf1 is n use.
 * @retval      contain the flags of the VSP2 IRQ received. @ref r_vsp2_Int_t
 *
 * @note If VSP2 WPF0 flags for Underrun and endframe are received
 *         the return value will be
 *              R_VSP_INT_UND_WPF0 | R_VSP_INT_FRE_WPF0
 *         The application can then take the necessary action depending on the
 *         interrupt signal received.
 */
uint32_t R_VSP2_Isr(r_vsp2_Unit_t Vsp2Unit,
                    uint32_t      Wpf0InUse,
                    uint32_t      Wpf1InUse);

/**
 * the blending between the Rpfs
 *
 * This function enables the BRS blending operation on the specified VSPD layer.
 *
 * @param[in]   Vsp2Unit       - Vsp2 Unit, valid value R_VSP2_VSPBS/R_VSP2_VSPD0/R_VSP2_VSPDL.
 *                               See @ref r_vsp2_Unit_t
 * @param[in]   Dpr_Rpfn_Route - BRSinX(R_VSP_DPR_ROUTE_BRS_ILV0/R_VSP_DPR_ROUTE_BRS_ILV1)
 * @retval      See @ref r_vsp2_Error_t
 */
r_vsp2_Error_t R_VSP2_BlendingEnable_Brs(r_vsp2_Unit_t Vsp2Unit, r_vsp2_Dpr_Route_t Dpr_Rpfn_Route);

/**
 * Control the color display background
 *
 * This function controls the color of the virtual
 * read layer (lowest level background) in BRS.
 *
 * @param[in]   Vsp2Unit - Vsp2 Unit, valid value R_VSP2_VSPBS/R_VSP2_VSPD0/R_VSP2_VSPDL.
 *                         See @ref r_vsp2_Unit_t
 * @param[in]   Color    - ARGB format
 * @retval      See @ref r_vsp2_Error_t
 */
r_vsp2_Error_t R_VSP2_BkgColorSet_Brs(r_vsp2_Unit_t Vsp2Unit,
                                  uint32_t Color);

/** @} api */

#ifdef __cplusplus
}
#endif

#endif /* R_VSP2_API_H_ */
