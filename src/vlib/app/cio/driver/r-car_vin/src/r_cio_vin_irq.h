/*************************************************************************************************************
* cio_vin_irq_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/**@defgroup group5_vin VIN Driver interrupt intialization functions
 * @ingroup cio_vin_driver
 *
 * @brief Init and deinit interrupt handling for the CIO server
 *
 * @{
 */
#ifndef R_CIO_VIN_IRQ_H
#define R_CIO_VIN_IRQ_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Function Name: R_CIO_VIN_PRV_IrqInit
 */
/**
 * @brief  Initialise VIN Interrupt handling
 *
 *
 * @param[in] Inst       - Pointer to VIN Device instance
 *
 * @return R_CIO_VIN_ERR_OK
 * @return R_CIO_VIN_ERR_FAILED
 */
int32_t R_CIO_VIN_PRV_IrqInit(R_CIO_VIN_PRV_Instance_t *Inst);

/*******************************************************************************
 * Function Name: R_CIO_VIN_PRV_IrqDeInit
 */
/**
 * @brief
 * Deinitialise VIN Interrupt handling
 *
 * @param[in]  Inst       - Pointer to VIN Device instance
 *
 * @return R_CIO_VIN_ERR_OK
 * @return R_CIO_VIN_ERR_FAILED
 */
int32_t R_CIO_VIN_PRV_IrqDeInit(R_CIO_VIN_PRV_Instance_t *Inst);


#ifdef __cplusplus
}
#endif

#endif /* R_CIO_VIN_IRQ_H */

/** @} */
