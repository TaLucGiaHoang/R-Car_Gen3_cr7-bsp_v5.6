/*************************************************************************************************************
* cio_ether_irq_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/


/**@defgroup group5_ether Ether Driver interrupt intialization functions
 * @ingroup cio_eth_driver
 *
 * @brief Init and deinit interrupt handling for the CIO server
 *
 * @{
 */



#ifndef R_CIO_ETHER_IRQ_H
#define  R_CIO_ETHER_IRQ_H

#ifdef __cplusplus
extern "C" {
#endif

#define ETH_AVBCH00ISR_ID 0
#define ETH_AVBCH01ISR_ID 1
#define ETH_AVBCH02ISR_ID 2
#define ETH_AVBCH03ISR_ID 3
#define ETH_AVBCH04ISR_ID 4
#define ETH_AVBCH05ISR_ID 5
#define ETH_AVBCH06ISR_ID 6
#define ETH_AVBCH07ISR_ID 7
#define ETH_AVBCH08ISR_ID 8
#define ETH_AVBCH09ISR_ID 9
#define ETH_AVBCH10ISR_ID 10
#define ETH_AVBCH11ISR_ID 11
#define ETH_AVBCH12ISR_ID 12
#define ETH_AVBCH13ISR_ID 13
#define ETH_AVBCH14ISR_ID 14
#define ETH_AVBCH15ISR_ID 15
#define ETH_AVBCH16ISR_ID 16
#define ETH_AVBCH17ISR_ID 17
#define ETH_AVBCH18ISR_ID 18
#define ETH_AVBCH19ISR_ID 19
#define ETH_AVBCH20ISR_ID 20
#define ETH_AVBCH21ISR_ID 21
#define ETH_AVBCH22ISR_ID 22
#define ETH_AVBCH23ISR_ID 23
#define ETH_AVBCH24ISR_ID 24

/*******************************************************************************
 *  Function Name: R_CIO_ETHER_PRV_IrqInit
 */
/**
 * @brief  Initialise ETHER Interrupt handling
 *
 * @return == 0 - OK
 * @return != 0 - NG
 */
int32_t R_CIO_ETHER_PRV_IrqInit();

/*******************************************************************************
 * Function Name: R_CIO_ETHER_PRV_IrqDeInit
 */
/**
 * @brief  Deinitialise ETHER Interrupt handling
 *
 * @return == 0 - OK
 * @return != 0 - NG
 */
int32_t R_CIO_ETHER_PRV_IrqDeInit();


#ifdef __cplusplus
}
#endif

#endif

/** @} */
