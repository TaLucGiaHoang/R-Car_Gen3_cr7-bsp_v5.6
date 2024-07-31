/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Stbe.c                                                  */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2016-2020 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains STBE functions implementation of Ethernet               */
/* Driver Component.                                                          */
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
/*              Devices:        R-Car H3                                      */
/*============================================================================*/

/*******************************************************************************
**                      Revision Control History                              **
********************************************************************************
* V1.0.0:  08-Dec-2016  : Initial Version
* V1.1.0:  06-Apr-2017  : Implement Rfso Fault Control
*                         Wrap Eth_EnableEDCProtection into
*                         ETH_DEVICE_VERSION macro
*                         Change ETH_RAM_SIZE to ETH_STBE_RAM_SIZE
*                         Update MISRA C Rule Violations
* V1.1.1:  16-May-2017  : Changed register access macros to functions
* V1.1.2:  19-Aug-2017  : Update initial setting for STBE
* V1.1.3:  18-May-2020  : Add MISRA C Violation message (4:5087)
*                         Remove MISRA C Violation message (2:3204)
*                         Change type of Violation message:
*                         - From (2:0303) to (4:0303)
*                         - From (2:0489) to (4:0489)
*******************************************************************************/

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Included for prototypes for internal functions of Ethernet Component */
#include "Eth_Stbe.h"
/* Included for declaration of functional safety APIs or macros */
#include "RCar_FuSa.h"
/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define ETH_STBE_C_AR_RELEASE_MAJOR_VERSION ETH_AR_RELEASE_MAJOR_VERSION_VALUE
#define ETH_STBE_C_AR_RELEASE_MINOR_VERSION ETH_AR_RELEASE_MINOR_VERSION_VALUE
#define ETH_STBE_C_AR_RELEASE_REVISION_VERSION \
  ETH_AR_RELEASE_REVISION_VERSION_VALUE

/* File version information */
#define ETH_STBE_C_SW_MAJOR_VERSION        ETH_SW_MAJOR_VERSION_VALUE
#define ETH_STBE_C_SW_MINOR_VERSION        ETH_SW_MINOR_VERSION_VALUE

/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/
#if (ETH_STBE_AR_RELEASE_MAJOR_VERSION != ETH_STBE_C_AR_RELEASE_MAJOR_VERSION)
#error "Eth_Stbe.c : Mismatch in Release Major Version"
#endif
#if (ETH_STBE_AR_RELEASE_MINOR_VERSION != ETH_STBE_C_AR_RELEASE_MINOR_VERSION)
#error "Eth_Stbe.c : Mismatch in Release Minor Version"
#endif
#if (ETH_STBE_AR_RELEASE_REVISION_VERSION != \
  ETH_STBE_C_AR_RELEASE_REVISION_VERSION)
#error "Eth_Stbe.c : Mismatch in Release Revision Version"
#endif

#if (ETH_STBE_SW_MAJOR_VERSION != ETH_STBE_C_SW_MAJOR_VERSION)
#error "Eth_Stbe.c : Mismatch in Software Major Version"
#endif
#if (ETH_STBE_SW_MINOR_VERSION != ETH_STBE_C_SW_MINOR_VERSION)
#error "Eth_Stbe.c : Mismatch in Software Minor Version"
#endif

/*******************************************************************************
**                      Global Data                                           **
*******************************************************************************/
/*******************************************************************************
**                      MISRA C Rule Violations                               **
*******************************************************************************/
/* 1. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0303) [I] Cast between a pointer to volatile object     */
/*                 and an integral type.                                      */
/* Rule          : MISRA-C:2004 Rule 3.1, 11.3                                */
/*                 REFERENCE - ISO:C90-6.3.4 Cast Operators - Semantics       */
/* Justification : The casting for register access                            */
/*                                                                            */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0303)-1 and                           */
/*                 END Msg(4:0303)-1 tags in the code.                        */
/******************************************************************************/
/* 2. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0489) The integer value 1 is being added or subtracted  */
/*                 from a pointer.                                            */
/* Rule          : MISRA-C:2004 Rule 17.4                                     */
/*                 REFERENCE - ISO:C90-6.3.6 Additive Operators - Constraints */
/* Justification : This is used to get the next address of STBE RAM           */
/*                                                                            */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0489)-2 and                           */
/*                 END Msg(4:0489)-2 tags in the code.                        */
/******************************************************************************/
/* 3. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:5087) #include statements in a file should only be      */
/*                  preceded by other preprocessor directives or comments     */
/* Rule          : MISRA-C:2004 Rule 19.1                                     */
/* Justification : include "Fls_MemMap.h" searches for defined symbol to map  */
/*                 memory section in memory mapping. This "include" should be */
/*                 added before and after the definition of memory section    */
/*                 This implementation is correct                             */
/* Verification  : Violation of this message were verified and reported       */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:5087)-3 and                          */
/*                 END Msg(4:5087)-3 tags in the code.                       */
/******************************************************************************/

/******************************************************************************/
/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/
/*******************************************************************************
** Function Name         : Eth_StbeEnableEDCProtection
**
** Service ID            : NA
**
** Description           : This enable Error Detection Circuit Protection
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variables      : None
**
** Functions invoked     : Eth_SetBitRegister32, Eth_ClearBitRegister32
**                         Eth_VerifyWriteRegister32
**
** Registers Used        : MFIERRTGTR8, MFIERRCTLR8, CTLR
**
*******************************************************************************/
#if (ETH_STBE_EDC_ERROR_DETECT == STD_ON)
#define ETH_START_SEC_CODE_SLOW
#include "Eth_MemMap.h"
/* TRACE[ETH_DDD_ACT_141] */
/* TRACE[ETH_DDD_INF_118] */
FUNC(void, ETH_CODE_SLOW) Eth_StbeEnableEDCProtection(void)
{
  P2VAR(volatile uint32, AUTOMATIC, ETH_APPL_DATA) LpTempReg;
  VAR(uint32, AUTOMATIC) LulInc;
  VAR(uint32, AUTOMATIC) LulRamSize32;

  #if (ETH_STBE_EDC_ERR_NOTIFICATION == STD_ON)
  /* Notify to MFIS module */
  /* and report to the external systems through FSCLKST# pin*/
  /* Clear bit 0 of MFIERRTGTR8 */
  Eth_ClearBitRegister32(MFIERRTGTR8_ADDR, ETH_ZERO_8);
  /* Register write-verify check */
  Eth_VerifyWriteRegister32(MFIERRTGTR8_ADDR, ETH_ZERO_32, ETH_BIT0_SET_32);
  /* Set bit 0 of MFIERRCTLR8 to 1 */
  Eth_SetBitRegister32(MFIERRCTLR8_ADDR, ETH_ZERO_8);
  /* Register write-verify check */
  Eth_VerifyWriteRegister32(MFIERRCTLR8_ADDR, ETH_BIT0_SET_32, ETH_BIT0_SET_32);
  #endif /* (ETH_STBE_EDC_ERR_NOTIFICATION == STD_ON) */
  /* Read the base address of STBE RAM */
  /* MISRA Violation: START Msg(4:0303)-1 */
  LpTempReg = (volatile uint32 *)ETH_STBE_RAM_BASE;
  /* MISRA Violation: END Msg(4:0303)-1 */
  LulRamSize32 = ETH_STBE_RAM_SIZE / ETH_FOUR_32;
  for (LulInc = ETH_ZERO_32; LulInc < LulRamSize32; LulInc++)
  {
    /* Write initial value in STBE RAM */
    *LpTempReg  = ETH_ZERO_32;
    /* Next address */
    /* MISRA Violation: START Msg(4:0489)-2 */
    LpTempReg++;
    /* MISRA Violation: END Msg(4:0489)-2 */
  }

  /* Set EDCEN bit of STBE Control Register (CTLR) to enable EDC protection */
  Eth_SetBitRegister32(ETH_STBE_REG_BASE + ETH_CTLR, ETH_CTLR_EDCEN_BIT);

  /* Register write-verify check */
  Eth_VerifyWriteRegister32(ETH_STBE_REG_BASE + ETH_CTLR,
    ETH_BIT0_SET_32, ETH_BIT0_SET_32);
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-3 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-3 */
#endif /* (ETH_STBE_EDC_ERROR_DETECT == STD_ON) */
/*******************************************************************************
**                               End of File                                  **
*******************************************************************************/
