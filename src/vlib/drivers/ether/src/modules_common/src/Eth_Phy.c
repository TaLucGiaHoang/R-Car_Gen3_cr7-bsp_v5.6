/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Phy.c                                                   */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2020 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains Internal functions implementation of  Ethernet Physical */
/* Interface  Component.                                                      */
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
* V1.0.0:  08-Dec-2015  : Initial Version
* V1.0.1:  05-Aug-2016  : Remove Renesas specific number 59
*                         Unify version number
* V1.1.0:  16-Sep-2016  : Update Compiler Abstraction and Memory Mapping
*                         follow Specification of AUTOSAR Release 4.2.2
* V1.1.1:  04-Nov-2016  : Implement read/write verify check
*                         Removed unnecessary write 1 after Eth_PhyBusRelease()
*                         in Eth_ReadMii API
*                         Replaced Eth_PhyBusRelease by
*                         Eth_PhyIndependentBusRelease in  Eth_WriteMii API
*                         Added Eth_PhyIndependentBusRelease function
* V1.1.2:  07-Jan-2017  : Revise read verify check
* V1.1.3:  08-Mar-2017  : Updated MISRA-C checking
* V1.1.4:  16-May-2017  : Updated source code based on FuSa review
* V1.1.5:  19-Jun-2017  : Added critical section protection to register
*                         read-modify-write accesses
* V1.1.6:  12-Sep-2017  : Replace Eth_GaaDevice[].BaseAddr global variable by
*                         Eth_GaaBaseAddr constant
* V1.2.0:  11-Oct-2017  : Added Eth_MDCPulseDelay to modify MDC pulse duration
* V1.2.1:  08-Nov-2017  : Modified Eth_PhyRead timing relationship
* V1.2.2:  15-Dec-2017  : Update casting to fix MISRA C Violations
*                         Move declaration of static function from header file
*                         to source file
*                         Replace static by STATIC
* V1.2.3:  25-Dec-2017  : Replace Eth_MDCPulseDelay by Eth_WaitForDelay
* V1.2.4:  07-Feb-2018  : Remove register read-verify check for registers which
*                         can be changed by hardware
* V1.2.5: 19-Aug-2018  : Update MISRA C rule violations
* V1.2.6: 23-Apr-2020  : Add MISRA C rule violation message Msg(4:5087)
*******************************************************************************/

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Included for prototypes for internal functions of Ethernet Physical
Interface Component */
#include "Eth_Phy.h"
#include "Eth_MACRegStruct.h"
#include "Eth_Ram.h"
#include "Eth_Dma.h"
#include "Eth_Common.h"
#include "Os.h"
/* Included for declaration of functional safety APIs or macros */
#include "RCar_FuSa.h"
/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/* AUTOSAR release version information */
#define ETH_PHY_C_AR_RELEASE_MAJOR_VERSION \
  ETH_AR_RELEASE_MAJOR_VERSION_VALUE
#define ETH_PHY_C_AR_RELEASE_MINOR_VERSION \
  ETH_AR_RELEASE_MINOR_VERSION_VALUE
#define ETH_PHY_C_AR_RELEASE_REVISION_VERSION \
  ETH_AR_RELEASE_REVISION_VERSION_VALUE

/* File version information */
#define ETH_PHY_C_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION_VALUE
#define ETH_PHY_C_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION_VALUE

/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/

/* Functionality related to R4.0 */
#if (ETH_PHY_AR_RELEASE_MAJOR_VERSION != \
  ETH_PHY_C_AR_RELEASE_MAJOR_VERSION)
#error "Eth_Phy.c : Mismatch in Release Major Version"
#endif
#if (ETH_PHY_AR_RELEASE_MINOR_VERSION != \
  ETH_PHY_C_AR_RELEASE_MINOR_VERSION)
#error "Eth_Phy.c : Mismatch in Release Minor Version"
#endif
#if (ETH_PHY_AR_RELEASE_REVISION_VERSION != \
  ETH_PHY_C_AR_RELEASE_REVISION_VERSION)
#error "Eth_Phy.c : Mismatch in Release Revision Version"
#endif

#if (ETH_PHY_SW_MAJOR_VERSION != ETH_PHY_C_SW_MAJOR_VERSION)
#error "Eth_Phy.c : Mismatch in Software Major Version"
#endif
#if (ETH_PHY_SW_MINOR_VERSION != ETH_PHY_C_SW_MINOR_VERSION)
#error "Eth_Phy.c : Mismatch in Software Minor Version"
#endif

/*******************************************************************************
**                      Global Data                                           **
*******************************************************************************/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

/*******************************************************************************
**                  MISRA C Rule Violations                                   **
*******************************************************************************/
/* 1. MISRA C RULE VIOLATION:                                                 */
/* Message       : (2:2814) Possible: Dereference of NULL pointer.            */
/* Rule          : None                                                       */
/* Justification : The pointer cannot be NULL because:                        */
/*                 + NULL pointer has been checked before setting the address */
/*                 for pointer.                                               */
/*                 + The address which is used to set pointer is address of   */
/*                 register or global variable. All of them are not NULL      */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(2:2814)-1 and                           */
/*                 END Msg(2:2814)-1 tags in the code.                        */
/******************************************************************************/
/* 2. MISRA C RULE VIOLATION:                                                 */
/* Message       : (2:2913) Suspicious: Wraparound in unsigned arithmetic     */
/*                 operation.                                                 */
/* Rule          : None                                                       */
/* Justification : The violation occurs in addition or multiplication if      */
/*                 the result is too large to be represented. This addition or*/
/*                 multiplication is nescessary to calculate the time stamp   */
/*                 value. The result of calculation has been casted to uint64,*/
/*                 there is no lossing data.                                  */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(2:2913)-2 and                           */
/*                 END Msg(2:2913)-2 tags in the code.                        */
/******************************************************************************/
/* 3. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:2982) This assignment is redundant. The value of this   */
/*                 object is never used before being modified.                */
/* Rule          : MISRA-C:2004 Rule 21.1                                     */
/* Justification : Assign the initialized value for local variables, it is    */
/*                 used to avoid the case that local variable is used before  */
/*                 setting                                                    */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2982)-3 and                           */
/*                 END Msg(4:2982)-3 tags in the code.                        */
/******************************************************************************/
/* 4. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:5087) #include statements in a file should only be      */
/*                  preceded by other preprocessor directives or comments     */
/* Rule          : MISRA-C:2004 Rule 19.1                                     */
/* Justification : include "Fls_MemMap.h" searches for defined symbol to map  */
/*                 memory section in memory mapping. This "include" should be */
/*                 added before and after the definition of memory section    */
/*                 This implementation is correct                             */
/* Verification  : Violation of this message were verified and reported       */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:5087)-4 and                           */
/*                 END Msg(4:5087)-4 tags in the code.                        */
/******************************************************************************/

/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/
/*******************************************************************************
** Function Name         : Eth_PhyWrite1
**
** Service ID            : NA
**
** Description           : This Write 1 into the MII management frame.
**
** Sync/Async            : Asynchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LddStdReturnValue: E_OK/E_NOT_OK
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Function(s) invoked   : Eth_WriteRegister32, Eth_WaitForDelay
**
** Registers Used        : PIR
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/* TRACE[ETH_DDD_ACT_078] */
/* TRACE[ETH_DDD_INF_040] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_PhyWrite1(uint8 LucCtrlIdx)
{
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturnValue;
  /* TRACE[ETH_DDD_ACT_078_REG001] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_PIR, ETH_SIX_32);
  /* TRACE[ETH_DDD_ACT_078_REG002] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
    ETH_PIR, ETH_SEVEN_32);
  /* MDC Pulse Delay */
  LddStdReturnValue = Eth_WaitForDelay(ETH_MDC_DELAY, ETH_OS_COUNTER_ID);
  if (E_OK == LddStdReturnValue)
  {
    /* TRACE[ETH_DDD_ACT_078_REG003] */
    Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_PIR, ETH_SIX_32);
    /* MDC Pulse Delay */
    LddStdReturnValue = Eth_WaitForDelay(ETH_MDC_DELAY, ETH_OS_COUNTER_ID);
  }
  else
  {
    /* No action required */
  }
  return LddStdReturnValue;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */

/*******************************************************************************
** Function Name         : Eth_PhyWrite0
**
** Service ID            : NA
**
** Description           : This Write 0 into the MII management frame.
**
** Sync/Async            : Asynchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LddStdReturnValue: E_OK/E_NOT_OK
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Function(s) invoked   : Eth_WriteRegister32, Eth_WaitForDelay
**
** Registers Used        : PIR
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/* TRACE[ETH_DDD_ACT_077] */
/* TRACE[ETH_DDD_INF_041] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_PhyWrite0(uint8 LucCtrlIdx)
{
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturnValue;
  /* TRACE[ETH_DDD_ACT_077_REG001] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_PIR, ETH_TWO_32);
  /* TRACE[ETH_DDD_ACT_077_REG002] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
    ETH_PIR, ETH_THREE_32);
  /* MDC Pulse Delay */
  LddStdReturnValue = Eth_WaitForDelay(ETH_MDC_DELAY, ETH_OS_COUNTER_ID);
  if (E_OK == LddStdReturnValue)
  {
    /* TRACE[ETH_DDD_ACT_077_REG003] */
    Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_PIR, ETH_TWO_32);
    /* MDC Pulse Delay */
    LddStdReturnValue = Eth_WaitForDelay(ETH_MDC_DELAY, ETH_OS_COUNTER_ID);
  }
  else
  {
    /* No action required */
  }
  return LddStdReturnValue;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/*******************************************************************************
** Function Name         : Eth_PhyRead
**
** Service ID            : NA
**
** Description           : This Read Phy register.
**
** Sync/Async            : Asynchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : LusVal: return data from PHY register
**
** Return parameter      : LddStdReturnValue: E_OK/E_NOT_OK
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Function(s) invoked   : Eth_ReadRegister32, Eth_WriteRegister32,
**                         Eth_WaitForDelay
**
** Registers Used        : PIR
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/* TRACE[ETH_DDD_ACT_075] */
/* TRACE[ETH_DDD_INF_042] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_PhyRead
(
  uint8 LucCtrlIdx,
  CONSTP2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) RegValPtr
)
{
  VAR(uint8, AUTOMATIC) LucInc;
  VAR(uint32, AUTOMATIC) LulTempReg;
  VAR(uint16, AUTOMATIC) LusVal;
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturnValue;

  /* MISRA Violation: START Msg(4:2982)-3 */
  LddStdReturnValue = E_OK;
  /* MISRA Violation: END Msg(4:2982)-3 */
  LusVal = ETH_ZERO_16;
  for (LucInc = ETH_ZERO_8; LucInc < ETH_SIXTEEN_8; LucInc++)
  {
    /* TRACE[ETH_DDD_ACT_075_REG001] */
    Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      ETH_PIR, ETH_ONE_32);
    /* MDC Pulse Delay */
    LddStdReturnValue = Eth_WaitForDelay(ETH_MDC_DELAY, ETH_OS_COUNTER_ID);
    /* TRACE[ETH_DDD_ACT_075_REG003] */
    if (E_OK == LddStdReturnValue)
    {
      Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        ETH_PIR, ETH_ZERO_32);
      LusVal <<= ETH_ONE_16;
      /* TRACE[ETH_DDD_ACT_075_REG002] */
      LulTempReg = Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
        ETH_PIR);
      LusVal |= (uint16)(((LulTempReg) & ETH_EIGHT_32) >> ETH_THREE_8);
      /* MDC Pulse Delay */
      LddStdReturnValue = Eth_WaitForDelay(ETH_MDC_DELAY, ETH_OS_COUNTER_ID);
    }
    else
    {
      /* No action required */
    }
    if (E_NOT_OK == LddStdReturnValue)
    {
      break;
    }
    else
    {
      /* No action required */
    }
  }
  if (E_OK == LddStdReturnValue)
  {
    /* MISRA Violation: START Msg(2:2814)-1 */
    *RegValPtr = LusVal;
    /* MISRA Violation: END Msg(2:2814)-1 */
  }
  else
  {
    /* No action required */
  }
  return LddStdReturnValue;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */

/*******************************************************************************
** Function Name         : Eth_PhyWrite
**
** Service ID            : NA
**
** Description           : This Write Phy register.
**
** Sync/Async            : Asynchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                       : LusValue - Value to be written
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LddStdReturnValue: E_OK/E_NOT_OK
**
** Preconditions         : None
**
** Global Variable(s)    : None
**
** Function(s) invoked   : Eth_PhyWrite1, Eth_PhyWrite0
**
** Registers Used        : None
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/* TRACE[ETH_DDD_ACT_076] */
/* TRACE[ETH_DDD_INF_043] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_PhyWrite
(
  uint8 LucCtrlIdx,
  uint16 LusValue
)
{
  VAR(uint8, AUTOMATIC) LucInc;
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturnValue;

  /* MISRA Violation: START Msg(4:2982)-3 */
  LddStdReturnValue = E_OK;
  /* MISRA Violation: END Msg(4:2982)-3 */

  for (LucInc = ETH_ZERO_8; ETH_SIXTEEN_8 > LucInc; LucInc++)
  {
    if (ETH_ZERO == (LusValue & ETH_BIT15_SET_16))
    {
      LddStdReturnValue = Eth_PhyWrite0(LucCtrlIdx);
    }
    else
    {
      LddStdReturnValue = Eth_PhyWrite1(LucCtrlIdx);
    }
    if (E_OK == LddStdReturnValue)
    {
      /* MISRA Violation: START Msg(2:2913)-2 */
      LusValue <<= ETH_ONE_16;
      /* MISRA Violation: END Msg(2:2913)-2 */
    }
    else
    {
      break;
    }
  }
  return LddStdReturnValue;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */

/*******************************************************************************
** Function Name         : Eth_MIIFrameHeaderSet
**
** Service ID            : NA
**
** Description           : This Set MII management Frame header.
**
** Sync/Async            : Asynchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                       : LucPhyAddr - Phy address
**                       : LucRegAddr - Register address
**                       : LucRW      - Read/Write, 0: read, 1: write
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LddStdReturnValue: E_OK/E_NOT_OK
**
** Preconditions         : None
**
** Global Variable(s)    : None
**
** Function(s) invoked   : Eth_PhyWrite1, Eth_PhyWrite0
**
** Registers Used        : None
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/* TRACE[ETH_DDD_ACT_071] */
/* TRACE[ETH_DDD_INF_044] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_MIIFrameHeaderSet
(
  uint8 LucCtrlIdx,
  uint8 LucPhyAddr,
  uint8 LucRegAddr,
  uint8 LucRW
)
{
  VAR(uint8, AUTOMATIC) LucInc;
  VAR(uint16, AUTOMATIC) LusData;
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturnValue;

  /* MISRA Violation: START Msg(4:2982)-3 */
  LddStdReturnValue = E_OK;
  /* MISRA Violation: END Msg(4:2982)-3 */
  /* MISRA Violation: START Msg(4:2982)-3 */
  LusData = ETH_ZERO_16;
  /* MISRA Violation: END Msg(4:2982)-3 */

  /* ST + OP */
  if (ETH_ONE_8 == LucRW)
  {
    /* Cast to unify type in calculation */
    LusData =  ((uint16)(ETH_ONE_16 << ETH_FOURTEEN_16)) |
      ((uint16)(ETH_ONE_16 << ETH_TWELVE_16));
  }
  else
  {
    /* Cast to unify type in calculation */
    LusData = ((uint16)(ETH_ONE_16 << ETH_FOURTEEN_16)) |
      ((uint16)(ETH_ONE_16 << ETH_THIRTEEN_16));
  }

  /* Phy address */
  /* Cast to unify type in calculation */
  LusData |= (uint16)((uint16)LucPhyAddr << ETH_SEVEN_16);

  /* Reg address */
  /* Cast to unify type in calculation */
  LusData |= (uint16)((uint16)LucRegAddr << ETH_TWO_16);

  /* write preamble */
  for (LucInc = ETH_ZERO_8; LucInc < ETH_THIRTYTWO_8; LucInc++)
  {
    LddStdReturnValue = Eth_PhyWrite1(LucCtrlIdx);
    if (E_NOT_OK == LddStdReturnValue)
    {
      break;
    }
    else
    {
      /* No action required */
    }
  }
  /* write ST, OP, PHYAD, RegAD */
  for (LucInc = ETH_ZERO_8; LucInc < ETH_FOURTEEN_8; LucInc++)
  {
    if (ETH_ZERO == (LusData & ETH_BIT15_SET_16))
    {
      LddStdReturnValue = Eth_PhyWrite0(LucCtrlIdx);
    }
    else
    {
      LddStdReturnValue = Eth_PhyWrite1(LucCtrlIdx);
    }
    LusData <<= ETH_ONE_16;
    if (E_NOT_OK == LddStdReturnValue)
    {
      break;
    }
    else
    {
      /* No action required */
    }
  }
  return LddStdReturnValue;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */

/*******************************************************************************
** Function Name         : Eth_PhyBusRelease
**
** Service ID            : NA
**
** Description           : MII management bus release flow.
**
** Sync/Async            : Asynchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LddStdReturnValue: E_OK/E_NOT_OK
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Function(s) invoked   : Eth_WriteRegister32, Eth_WaitForDelay
**
** Registers Used        : PIR
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/* TRACE[ETH_DDD_ACT_074] */
/* TRACE[ETH_DDD_INF_045] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_PhyBusRelease( uint8 LucCtrlIdx)
{
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturnValue;
  /* TRACE[ETH_DDD_ACT_074_REG001] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
    ETH_PIR, ETH_ZERO_32);
  /* TRACE[ETH_DDD_ACT_074_REG002] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_PIR, ETH_ONE_32);
  /* MDC Pulse Delay */
  LddStdReturnValue = Eth_WaitForDelay(ETH_MDC_DELAY, ETH_OS_COUNTER_ID);
  /* TRACE[ETH_DDD_ACT_074_REG003] */
  if (E_OK == LddStdReturnValue)
  {
    Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
      ETH_PIR, ETH_ZERO_32);
    /* MDC Pulse Delay */
    LddStdReturnValue = Eth_WaitForDelay(ETH_MDC_DELAY, ETH_OS_COUNTER_ID);
  }
  else
  {
    /* No action required */
  }
  return LddStdReturnValue;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */

/*******************************************************************************
** Function Name         : Eth_PhyIndependentBusRelease
**
** Service ID            : NA
**
** Description           : MII management independent bus release flow.
**
** Sync/Async            : Asynchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Function(s) invoked   : Eth_WriteRegister32
**
** Registers Used        : PIR
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/* TRACE[ETH_DDD_ACT_142] */
/* TRACE[ETH_DDD_INF_119] */
FUNC(void, ETH_CODE_SLOW) Eth_PhyIndependentBusRelease(uint8 LucCtrlIdx)
{
  /* TRACE[ETH_DDD_ACT_142_REG001] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
    ETH_PIR, ETH_ZERO_32);
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
