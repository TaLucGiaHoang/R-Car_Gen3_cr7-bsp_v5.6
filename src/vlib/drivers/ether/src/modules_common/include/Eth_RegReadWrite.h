/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_RegReadWrite.h                                          */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2016-2017 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains the type definitions of Register read and write macros  */
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
/*              Devices: R-Car Series, 3rd Generation                         */
/*============================================================================*/

/*******************************************************************************
**                      Revision Control History                              **
********************************************************************************
* V1.0.0:  08-Dec-2016  : Initial Version
* V1.0.1:  08-Mar-2017  : Updated MISRA-C checking
* V1.1.0:  16-May-2017  : Removed register access macros
*                         Added register access functions prototype
* V1.1.1:  18-Jun-2017  : Update memory mapping of read and write register
*                         function
* V1.1.2:  12-Sep-2017  : Added extern declaration of Eth_GaaBaseAddr
*                         Added the definition of ETH_BASE macro
*******************************************************************************/
#ifndef ETH_REG_WRITE_H
#define ETH_REG_WRITE_H

/*******************************************************************************
**                     Include Section                                        **
*******************************************************************************/
#include "Eth_Types.h"
/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
#define ETH_START_SEC_CONST_32
#include "Eth_MemMap.h"
/* TRACE [ETH_DDD_GBL_028] */
extern CONST(uint32, ETH_CONST) Eth_GaaBaseAddr[ETH_MAX_CTRLS_SUPPORTED];
#define ETH_STOP_SEC_CONST_32
#include "Eth_MemMap.h"

/*******************************************************************************
**                     Macro Definitions                                      **
*******************************************************************************/
/* Base Address for the Whole AVB Macro in R-Car Series, 3rd Generation */
#define ETH_BASE       0xE6800000UL
/* Macro for Register read-back disable */
#define WV_DISABLE                     0U
/* Macro for Register read-back INIT_ONLY */
#define WV_INIT_ONLY                   1U
/* Macro for Register read-back INIT_RUNTIME */
#define WV_INIT_RUNTIME                2U
/* Macro for Register read-verify RUNTIME */
#define RV_RUNTIME                     3U
/* Macro for Register read-verify disable */
#define RV_DISABLE                     4U

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
#include "Eth_MemMap.h"
extern FUNC(void, ETH_CODE_FAST) Eth_WriteRegister32
(
  VAR(uint32, AUTOMATIC) ulRegAddr,
  VAR(uint32, AUTOMATIC) ulRegValue
);

extern FUNC(uint32, ETH_CODE_FAST) Eth_ReadRegister32
(
  VAR(uint32, AUTOMATIC) ulRegAddr
);

extern FUNC(void, ETH_CODE_FAST) Eth_ClearBitRegister32
(
  VAR(uint32, AUTOMATIC) ulRegAddr,
  VAR(uint8, AUTOMATIC) ucBit
);

extern FUNC(uint32, ETH_CODE_FAST) Eth_ReadBitRegister32
(
  VAR(uint32, AUTOMATIC) ulRegAddr,
  VAR(uint8, AUTOMATIC) ucBit
);

extern FUNC(void, ETH_CODE_FAST) Eth_VerifyReadRegister32
(
  VAR(uint32, AUTOMATIC) ulRegAddr,
  VAR(uint32, AUTOMATIC) ulCheckValue
);
#define ETH_STOP_SEC_CODE_FAST
#include "Eth_MemMap.h"

#define ETH_START_SEC_CODE_SLOW
#include "Eth_MemMap.h"
extern FUNC(void, ETH_CODE_SLOW) Eth_SetBitRegister32
(
  VAR(uint32, AUTOMATIC) ulRegAddr,
  VAR(uint8, AUTOMATIC) ucBit
);

extern FUNC(void, ETH_CODE_SLOW) Eth_VerifyWriteRegister32
(
  VAR(uint32, AUTOMATIC) ulRegAddr,
  VAR(uint32, AUTOMATIC) ulCheckValue,
  VAR(uint32, AUTOMATIC) ulWCMaskValue
);
#define ETH_STOP_SEC_CODE_SLOW
#include "Eth_MemMap.h"

#endif /*  ETH_REG_WRITE_H */
/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
