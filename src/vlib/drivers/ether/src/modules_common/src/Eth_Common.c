/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Common.c                                                */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2017-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains implementation of common APIs for the module            */
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
/*              Devices:        R-Car Series, 3rd Generation                  */
/*============================================================================*/

/*******************************************************************************
**                      Revision Control History                              **
********************************************************************************
* V1.0.0:  13-Dec-2017  : Initial Version
* V1.1.0:  25-Dec-2017  : Add function Eth_WaitForDelay
* V1.1.1:  05-Mar-2018  : Update usage of GetElapsedValue in Eth_WaitForTimeout
*                         and Eth_WaitForDelay
* V1.1.2:  26-Apr-2019  : Update type of OsCounterID from uint8 to CounterType
*                         in functions Eth_WaitForTimeout and Eth_WaitForDelay
* V1.1.3:  23-Apr-2020  : Add MISRA C rule violation message Msg(4:5087) and
*                         (4:0857)
* V1.2.0:  23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Eth Common header file */
#include "Eth_Common.h"
#include "ether_api.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define ETH_COMMON_C_AR_RELEASE_MAJOR_VERSION ETH_AR_RELEASE_MAJOR_VERSION_VALUE
#define ETH_COMMON_C_AR_RELEASE_MINOR_VERSION ETH_AR_RELEASE_MINOR_VERSION_VALUE
#define ETH_COMMON_C_AR_RELEASE_REVISION_VERSION \
  ETH_AR_RELEASE_REVISION_VERSION_VALUE

/* File version information */
#define ETH_COMMON_C_SW_MAJOR_VERSION        ETH_SW_MAJOR_VERSION_VALUE
/* MISRA Violation: START Msg(4:0857)-3 */
#define ETH_COMMON_C_SW_MINOR_VERSION        ETH_SW_MINOR_VERSION_VALUE
/* MISRA Violation: START Msg(4:0857)-3 */
/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/
#if (ETH_COMMON_AR_RELEASE_MAJOR_VERSION != \
  ETH_COMMON_C_AR_RELEASE_MAJOR_VERSION)
#error "ETH_Common.c : Mismatch in Release Major Version"
#endif
#if (ETH_COMMON_AR_RELEASE_MINOR_VERSION != \
  ETH_COMMON_C_AR_RELEASE_MINOR_VERSION)
#error "ETH_Common.c : Mismatch in Release Minor Version"
#endif
#if (ETH_COMMON_AR_RELEASE_REVISION_VERSION != \
  ETH_COMMON_C_AR_RELEASE_REVISION_VERSION)
#error "ETH_Common.c : Mismatch in Release Revision Version"
#endif

#if (ETH_COMMON_SW_MAJOR_VERSION != ETH_COMMON_C_SW_MAJOR_VERSION)
#error "ETH_Common.c : Mismatch in Software Major Version"
#endif
#if (ETH_COMMON_SW_MINOR_VERSION != ETH_COMMON_C_SW_MINOR_VERSION)
#error "ETH_Common.c : Mismatch in Software Minor Version"
#endif
/*******************************************************************************
**                      Macro                                                 **
*******************************************************************************/

/*******************************************************************************
**                      Global Data                                           **
*******************************************************************************/

/*******************************************************************************
**                      MISRA C Rule Violations                               **
*******************************************************************************/
/* 1. MISRA C RULE VIOLATION:                                                 */
/* Message       : (2:2814) Possible: Dereference of NULL pointer.            */
/* Rule          : None                                                       */
/* Justification : The pointer cannot be NULL because the address which is    */
/*                 used to set pointer is address of registers                */
/*                 All of them are not NULL pointer                           */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(2:2814)-1 and                           */
/*                 END Msg(2:2814)-1 tags in the code.                        */
/******************************************************************************/
/* 2. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:2982) This assignment is redundant. The value of this   */
/*                 object is never used before being modified.                */
/* Rule          : MISRA-C:2004 Rule 21.1                                     */
/* Justification : Assign the initialized value for local variables, it is    */
/*                 used to avoid the case that local variable is used before  */
/*                 setting                                                    */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2982)-2 and                           */
/*                 END Msg(4:2982)-2 tags in the code.                        */
/******************************************************************************/
/* 3. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0857) [L] Number of macro definitions exceeds 1024      */
/*                 - program does not conform strictly to ISO:C90.            */
/* Rule          : MISRA-C:2004 Rule 1.1                                      */
/* Justification : Macros are used to easy verify and maintain the source     */
/*                 code, many macros are defined because ETH has many specific*/
/*                 keywords need to define                                    */
/* Verification  : Number of macro definitions exceeds 1024 does not affect to*/
/*                 ETH operation. All macro definitions have been reviewed    */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0857)-3 and                           */
/*                 END Msg(4:0857)-3 tags in the code.                        */
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
** Function Name         : Eth_WaitForTimeout
**
** Service ID            : NA
**
** Description           : This service waits timeout to check a condition.
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Reentrant
**
** Input Parameters      : TimeoutID: DEM error ID for timeout
**                         TimeoutValue: waiting time (timeout)
**                         OsCounterID: ID of Os Counter
**                         RegAddr: pointer to register address
**                         Mask: register mask
**                         ExpectedValue: register expected value
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LddStdReturnType: E_OK/E_NOT_OK
**
** Preconditions         : Os must be initialized.
**
** Global Variables Used : None
**
** Functions Invoked     : GetCounterValue, GetElapsedValue,
**                         Dem_ReportErrorStatus
**
** Registers Used        : None
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_194] */
/* TRACE[ETH_DDD_INF_166] */
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
FUNC(Std_ReturnType, ETH_CODE_SLOW)Eth_WaitForTimeout
(
  const Dem_EventIdType TimeoutID,
  const TickType TimeoutValue,
  const CounterType OsCounterID,
  CONSTP2CONST(volatile uint32, AUTOMATIC, ETH_APPL_CONST)RegAddr,
  const uint32 Mask,
  const uint32 ExpectedValue
)
{
  VAR(TickType, AUTOMATIC) LulInitTime;
  VAR(TickType, AUTOMATIC) LulCurTime;
  VAR(TickType, AUTOMATIC) LulElapsedTime;
  VAR(StatusType, AUTOMATIC) LddStatusReturn;
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturnType;

  LddStdReturnType = E_OK;
  LulInitTime = (TickType)ETH_COMMON_ZERO;
  /* MISRA Violation: START Msg(4:2982)-2 */
  LulCurTime = (TickType)ETH_COMMON_ZERO;
  /* MISRA Violation: END Msg(4:2982)-2 */
  LulElapsedTime = (TickType)ETH_COMMON_ZERO;

  /* Get start time */
  LddStatusReturn = GetCounterValue(OsCounterID, &LulInitTime);
  if(E_OK != LddStatusReturn)
  {
    LddStdReturnType = E_NOT_OK;
  }
  else
  {
    do
    {
      LulCurTime = LulInitTime;
      /* Get elapsed time */
      LddStatusReturn = GetElapsedValue(OsCounterID,
        &LulCurTime, &LulElapsedTime);
      if(E_OK != LddStatusReturn)
      {
        LddStdReturnType = E_NOT_OK;
      }
      else
      {
        /* Do Nothing */
      }
    /* MISRA Violation: START Msg(2:2814)-1 */
    }while(((*(RegAddr) & (Mask)) != (ExpectedValue)) &&
    /* MISRA Violation: END Msg(2:2814)-1 */
      (TimeoutValue >= LulElapsedTime) && (E_NOT_OK != LddStdReturnType));

    if (LulElapsedTime > TimeoutValue)
    {
      LddStdReturnType = E_NOT_OK;
    }
    else
    {
      /* Do Nothing */
    }
  }

  /* Report DEM */
  if (E_NOT_OK == LddStdReturnType)
  {
    /* DEM error */
    Dem_ReportErrorStatus(TimeoutID, DEM_EVENT_STATUS_FAILED);
  }
  else
  {
    /* Do Nothing */
  }

  /* Return LddStdReturnType */
  return LddStdReturnType;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */

/*******************************************************************************
** Function Name         : Eth_WaitForDelay
**
** Service ID            : NA
**
** Description           : This service is used to wait for delay time
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Reentrant
**
** Input Parameters      : DelayValue: delay time
**                         OsCounterID: ID of Os Counter
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LddStdReturnType: E_OK/E_NOT_OK
**
** Preconditions         : Os must be initialized.
**
** Global Variables Used : None
**
** Functions Invoked     : GetCounterValue, GetElapsedValue,
**
** Registers Used        : None
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_191] */
/* TRACE[ETH_DDD_INF_165] */
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
FUNC(Std_ReturnType, ETH_CODE_SLOW)Eth_WaitForDelay
(
  const TickType DelayValue,
  const CounterType OsCounterID
)
{
  VAR(TickType, AUTOMATIC) LulInitTime;
  VAR(TickType, AUTOMATIC) LulCurTime;
  VAR(TickType, AUTOMATIC) LulElapsedTime;
  VAR(StatusType, AUTOMATIC) LddStatusReturn;
  VAR(Std_ReturnType, AUTOMATIC) LddStdReturnValue;

  LddStdReturnValue = E_OK;
  LulInitTime = (TickType)ETH_COMMON_ZERO;
  /* MISRA Violation: START Msg(4:2982)-2 */
  LulCurTime = (TickType)ETH_COMMON_ZERO;
  /* MISRA Violation: END Msg(4:2982)-2 */
  LulElapsedTime = (TickType)ETH_COMMON_ZERO;

  /* Get start time */
  LddStatusReturn = GetCounterValue(OsCounterID, &LulInitTime);
  if(E_OK != LddStatusReturn)
  {
    LddStdReturnValue = E_NOT_OK;
  }
  else
  {
    do
    {
      LulCurTime = LulInitTime;
      /* Get elapsed time */
      LddStatusReturn = GetElapsedValue(OsCounterID,
        &LulCurTime, &LulElapsedTime);
      if(E_OK != LddStatusReturn)
      {
        LddStdReturnValue = E_NOT_OK;
      }
      else
      {
        /* Do Nothing */
      }
    }while((DelayValue > LulElapsedTime) && (E_NOT_OK != LddStdReturnValue));
  }
  return LddStdReturnValue;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
