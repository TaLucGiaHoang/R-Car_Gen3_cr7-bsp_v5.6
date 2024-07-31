/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = SchM_Eth.h                                                  */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2020 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* Header file information for application.                                   */
/*                                                                            */
/*============================================================================*/
/*                                                                            */
/* Unless otherwise agreed upon in writing between your company and           */
/* Renesas Electronics Corporation the following shall apply!                 */
/*                                                                            */
/* Warranty Disclaimer                                                        */
/*                                                                            */
/* There is no warranty of any kind whatsoever granted by Renesas. Any        */
/* warranty is expressly disclaimed and excluded by Renesas, either expressed */
/* or implied, including but not limited to those for non-infringement of     */
/* intellectual property, merchantability and/or fitness for the particular   */
/* purpose.                                                                   */
/*                                                                            */
/* Renesas shall not have any obligation to maintain, service or provide bug  */
/* fixes for the supplied Product(s) and/or the Application.                  */
/*                                                                            */
/* Each User is solely responsible for determining the appropriateness of     */
/* using the Product(s) and assumes all risks associated with its exercise    */
/* of rights under this Agreement, including, but not limited to the risks    */
/* and costs of program errors, compliance with applicable laws, damage to    */
/* or loss of data, programs or equipment, and unavailability or              */
/* interruption of operations.                                                */
/*                                                                            */
/* Limitation of Liability                                                    */
/*                                                                            */
/* In no event shall Renesas be liable to the User for any incidental,        */
/* consequential, indirect, or punitive damage (including but not limited     */
/* to lost profits) regardless of whether such liability is based on breach   */
/* of contract, tort, strict liability, breach of warranties, failure of      */
/* essential purpose or otherwise and even if advised of the possibility of   */
/* such damages. Renesas shall not be liable for any services or products     */
/* provided by third party vendors, developers or consultants identified or   */
/* referred to the User by Renesas in connection with the Product(s) and/or   */
/* the Application.                                                           */
/*                                                                            */
/*============================================================================*/
/* Environment:                                                               */
/*              Devices:        R-Car R-Car Series, 3rd Generation            */
/*============================================================================*/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V1.0.0:  25-Aug-2015  : Initial version.
 * V1.1.0:  30-Sep-2016  : Added SchM_Enter_Eth_STATISTIC_DATA_PROTECTION,
 *                         SchM_Exit_Eth_STATISTIC_DATA_PROTECTION
 * V1.2.0:  19-Jun-2017  : Added SchM_Enter_Eth_REGISTER_PROTECTION,
 *                         SchM_Exit_Eth_REGISTER_PROTECTION
 *                         Removed SchM_Enter_Eth_DRIVERSTATE_DATA_PROTECTION,
 *                         SchM_Exit_Eth_DRIVERSTATE_DATA_PROTECTION
 *                         Added SchM_Enter_Eth_GLOBAL_PROTECTION,
 *                         SchM_Enter_Eth_GLOBAL_PROTECTION
 * V1.2.1:  24-Jan-2018  : Add device name M3 about comment.
 * V1.2.2:  13-Apr-2018  : Change ETH SchM Exclusive Area API from macro
 *                         to function definition.
 * V1.2.3:  05-Feb-2020  : Add function prototype for Eth_MainFunction
 *                         Include file Eth_MemMap.h for declarations of memory
 *                         section type
 *
 */
/******************************************************************************/
#ifndef SCHM_ETH_H
#define SCHM_ETH_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

/*******************************************************************************
**                      Version Information                                  **
*******************************************************************************/

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
#include "Eth_MemMap.h"
extern FUNC(void, ETH_CODE_SLOW) Eth_MainFunction(void);
#define ETH_STOP_SEC_CODE_SLOW
#include "Eth_MemMap.h"

extern void SchM_Enter_Eth_REGISTER_PROTECTION(void);
extern void SchM_Exit_Eth_REGISTER_PROTECTION(void);

extern void SchM_Enter_Eth_STATISTIC_DATA_PROTECTION(void);
extern void SchM_Exit_Eth_STATISTIC_DATA_PROTECTION(void);

extern void SchM_Enter_Eth_GLOBAL_PROTECTION(void);
extern void SchM_Exit_Eth_GLOBAL_PROTECTION(void);

#endif /* SCHM_ETH_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
