/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_RegReadWrite.c                                          */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2017-2020 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains the definitions of Register read and write functions    */
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
* V1.0.0:  16-May-2017  : Initial Version
* V1.0.1:  18-Jun-2017  : Update memory mapping of read and write register
*                         function
* V1.0.2:  28-Jul-2017  : Change 'inline' keyword to the macro INLINE
* V1.0.3:  12-Sep-2017  : Add Eth_GaaBaseAddr constant
* V1.0.4:  17-Apr-2020  : Suppress unused parameter warnings in function
*                         Eth_VerifyReadRegister32, Eth_VerifyWriteRegister32
*                         Add MISRA C rule violation message Msg(4:5087) and
*                         (4:3112)
*******************************************************************************/

/*******************************************************************************
**                     Include Section                                        **
*******************************************************************************/

/* Dem.h inclusion to get Dem_ReportErrorStatus declaration */
#include "Dem.h"
#include "Std_Types.h"
#include "Eth_PBTypes.h"
#include "Eth_Cfg.h"
#include "Eth_RegReadWrite.h"
#include "RCar_FuSa.h"
#include "Eth_Ram.h"
/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
#define ETH_START_SEC_CONST_32
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/* Array stores the base addresses of AVB part */
CONST(uint32, ETH_CONST) Eth_GaaBaseAddr[ETH_MAX_CTRLS_SUPPORTED] =
{
  ETH_BASE
};
#define ETH_STOP_SEC_CONST_32
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/*******************************************************************************
**                  MISRA C Rule Violations                                   **
*******************************************************************************/
/* 1. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0303) [I] Cast between a pointer to volatile object     */
/*                 and an integral type.                                      */
/* Rule          : MISRA-C:2004 Rule 3.1, 11.3                                */
/*                 REFERENCE - ISO:C90-6.3.4 Cast Operators - Semantics       */
/* Justification : The casting is necessary to access register                */
/*                                                                            */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0303)-1 and                           */
/*                 END Msg(4:0303)-1 tags in the code.                        */
/******************************************************************************/
/* 2. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:1055) [C99] The keyword 'inline' has been used.         */
/* Rule          : MISRA-C:2004 Rule 1.1                                      */
/*                 REFERENCE - ISO:C99-6.7.4 Function specifiers              */
/* Justification : It is used to achieve better throughput instead of         */
/*                 invoking a function call.                                  */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:1055)-2 and                           */
/*                 END Msg(4:1055)-2 tags in the code.                        */
/******************************************************************************/
/* 3. MISRA C RULE VIOLATION:                                                 */
/* Message       : (2:2814) Possible: Dereference of NULL pointer.            */
/* Rule          : None                                                       */
/* Justification : The pointer cannot be NULL because:                        */
/*                 + NULL pointer has been checked before setting the address */
/*                 for pointer.                                               */
/*                 + The address which is used to set pointer is address of   */
/*                 register or global variable. All of them are not NULL      */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(2:2814)-3 and                           */
/*                 END Msg(2:2814)-3 tags in the code.                        */
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
/* 5. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:3112) This statement has no side-effect                 */
/*                  - it can be removed.                                      */
/* Rule          : MISRA-C:2004 Rule 14.2                                     */
/* Justification : The statement is added intendedly to avoid unused parameter*/
/*                 error message from compiler, it does not impact to         */
/*                 the functionality of ETH driver                            */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:3112)-5 and                           */
/*                 END Msg(4:3112)-5 tags in the code.                        */
/******************************************************************************/

/*******************************************************************************
**                     Macro Definitions                                      **
*******************************************************************************/

/*******************************************************************************
** Function Name         : Eth_WriteRegister32
**
** Service ID            : None
**
** Description           : This performs write access to register
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : ulRegAddr - Register address
**                         ulRegValue - Register value
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
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/* TRACE[ETH_DDD_ACT_181] */
/* TRACE[ETH_DDD_INF_152] */
/* MISRA Violation: START Msg(4:1055)-2 */
INLINE FUNC(void, ETH_CODE_FAST) Eth_WriteRegister32
/* MISRA Violation: END Msg(4:1055)-2 */
(
  VAR(uint32, AUTOMATIC) ulRegAddr,
  VAR(uint32, AUTOMATIC) ulRegValue
)
{
  /* MISRA Violation: START Msg(4:0303)-1 */
  /* MISRA Violation: START Msg(2:2814)-3 */
  *((volatile uint32*)(ulRegAddr)) = ulRegValue;
  /* MISRA Violation: END Msg(2:2814)-3 */
  /* MISRA Violation: END Msg(4:0303)-1 */
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */

/*******************************************************************************
** Function Name         : Eth_ReadRegister32
**
** Service ID            : None
**
** Description           : This performs read access to register
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : ulRegAddr - Register address
**
** InOut Parameters      : None
**
** Output Parameters     : LulRegValue - Register value
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variables      : None
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/* TRACE[ETH_DDD_ACT_182] */
/* TRACE[ETH_DDD_INF_153] */
/* MISRA Violation: START Msg(4:1055)-2 */
INLINE FUNC(uint32, ETH_CODE_FAST) Eth_ReadRegister32
/* MISRA Violation: END Msg(4:1055)-2 */
(
  VAR(uint32, AUTOMATIC) ulRegAddr
)
{
  VAR(uint32, AUTOMATIC) LulRegValue;

  /* MISRA Violation: START Msg(4:0303)-1 */
  /* MISRA Violation: START Msg(2:2814)-3 */
  LulRegValue = *((volatile uint32*)(ulRegAddr));
  /* MISRA Violation: END Msg(2:2814)-3 */
  /* MISRA Violation: END Msg(4:0303)-1 */

  return LulRegValue;
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */

/*******************************************************************************
** Function Name         : Eth_SetBitRegister32
**
** Service ID            : None
**
** Description           : This sets a specific BIT in a 32 bit access register
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : ulRegAddr - Register address
**                         ucBit - Bit position
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
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/* TRACE[ETH_DDD_ACT_183] */
/* TRACE[ETH_DDD_INF_154] */
/* MISRA Violation: START Msg(4:1055)-2 */
INLINE FUNC(void, ETH_CODE_SLOW) Eth_SetBitRegister32
/* MISRA Violation: END Msg(4:1055)-2 */
(
  VAR(uint32, AUTOMATIC) ulRegAddr,
  VAR(uint8, AUTOMATIC) ucBit
)
{
  /* Cast to unify type in calculation */
  /* MISRA Violation: START Msg(4:0303)-1 */
  /* MISRA Violation: START Msg(2:2814)-3 */
  *((volatile uint32*)(ulRegAddr)) |= ((uint32)(ETH_BIT0_SET_32<<(ucBit)));
  /* MISRA Violation: END Msg(2:2814)-3 */
  /* MISRA Violation: END Msg(4:0303)-1 */
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */

/*******************************************************************************
** Function Name         : Eth_ClearBitRegister32
**
** Service ID            : None
**
** Description           : This clears a specific BIT in a 32 bit access
**                         register
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : ulRegAddr - Register address
**                         ucBit - Bit position
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
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/* TRACE[ETH_DDD_ACT_184] */
/* TRACE[ETH_DDD_INF_155] */
/* MISRA Violation: START Msg(4:1055)-2 */
INLINE FUNC(void, ETH_CODE_FAST) Eth_ClearBitRegister32
/* MISRA Violation: END Msg(4:1055)-2 */
(
  VAR(uint32, AUTOMATIC) ulRegAddr,
  VAR(uint8, AUTOMATIC) ucBit
)
{
  /* Cast to unify type in calculation */
  /* MISRA Violation: START Msg(4:0303)-1 */
  /* MISRA Violation: START Msg(2:2814)-3 */
  *((volatile uint32*)(ulRegAddr)) &= ((uint32)~(ETH_BIT0_SET_32<<(ucBit)));
  /* MISRA Violation: END Msg(2:2814)-3 */
  /* MISRA Violation: END Msg(4:0303)-1 */
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */

/*******************************************************************************
** Function Name         : Eth_ReadBitRegister32
**
** Service ID            : None
**
** Description           : This reads specific BIT in a 32 bit access register
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : ulRegAddr - Register address
**                         ucBit - Bit position
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LulRegValue - Register value
**
** Preconditions         : None
**
** Global Variables      : None
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/* TRACE[ETH_DDD_ACT_185] */
/* TRACE[ETH_DDD_INF_156] */
/* MISRA Violation: START Msg(4:1055)-2 */
INLINE FUNC(uint32, ETH_CODE_FAST) Eth_ReadBitRegister32
/* MISRA Violation: END Msg(4:1055)-2 */
(
  VAR(uint32, AUTOMATIC) ulRegAddr,
  VAR(uint8, AUTOMATIC) ucBit
)
{
  VAR(uint32, AUTOMATIC) LulRegValue;

  /* Cast to unify type in calculation */
  /* MISRA Violation: START Msg(4:0303)-1 */
  /* MISRA Violation: START Msg(2:2814)-3 */
  LulRegValue = *((volatile uint32*)(ulRegAddr)) &
  /* MISRA Violation: END Msg(2:2814)-3 */
  /* MISRA Violation: END Msg(4:0303)-1 */
    (uint32)(ETH_BIT0_SET_32 << (ucBit));

  return LulRegValue;
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */

/*******************************************************************************
** Function Name         : Eth_VerifyReadRegister32
**
** Service ID            : None
**
** Description           : This does the comparison check when read data from
**                         register and do the error reporting if the
**                         comparison fails
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : ulRegAddr - Register address
**                         uLCheckValue - The value use to compare
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
** Functions invoked     : Dem_ReportErrorStatus, RCar_RfsoFaultControl,
**                         Eth_ReadRegister32
**
** Registers Used        : FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/* TRACE[ETH_DDD_ACT_186] */
/* TRACE[ETH_DDD_INF_157] */
/* MISRA Violation: START Msg(4:1055)-2 */
INLINE FUNC(void, ETH_CODE_FAST) Eth_VerifyReadRegister32
/* MISRA Violation: END Msg(4:1055)-2 */
(
  VAR(uint32, AUTOMATIC) ulRegAddr,
  VAR(uint32, AUTOMATIC) ulCheckValue
)
{
  #if (ETH_READ_VERIFY_CHECK != RV_RUNTIME)
  /* Suppress unused parameter warnings for the parameters below */
  /* MISRA Violation: START Msg(4:3112)-5 */
  (void) ulRegAddr;
  /* MISRA Violation: END Msg(4:3112)-5 */
  /* MISRA Violation: START Msg(4:3112)-5 */
  (void) ulCheckValue;
  /* MISRA Violation: END Msg(4:3112)-5 */
  #endif

  #if (ETH_READ_VERIFY_CHECK == RV_RUNTIME)
  VAR(uint32, AUTOMATIC) LulRegValue;

  LulRegValue = Eth_ReadRegister32(ulRegAddr);
  if (ulCheckValue != LulRegValue)
  {
    Dem_ReportErrorStatus(ETH_E_REG_READ_VERIFY, DEM_EVENT_STATUS_FAILED);
    #if (ETH_READ_VERIFY_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    RCar_RfsoFaultControl(ETH_READ_VERIFY_ERR_NOTIFICATION_TGT,
      RFSO_FAULT_TRANSIENT);
    #endif
  }
  else
  {
    /* No action required */
  }
  #endif
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/*******************************************************************************
** Function Name         : Eth_VerifyWriteRegister32
**
** Service ID            : None
**
** Description           : This does the comparison check when write data to
**                         register and do the error reporting if the
**                         comparison fails
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : ulRegAddr - Register address
**                         uLCheckValue - The value use to compare
**                         ulWCMaskValue - Mask of register
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
** Functions invoked     : Dem_ReportErrorStatus, RCar_RfsoFaultControl,
**                         Eth_ReadRegister32
**
** Registers Used        : FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/* TRACE[ETH_DDD_ACT_187] */
/* TRACE[ETH_DDD_INF_158] */
/* MISRA Violation: START Msg(4:1055)-2 */
INLINE FUNC(void, ETH_CODE_SLOW) Eth_VerifyWriteRegister32
/* MISRA Violation: END Msg(4:1055)-2 */
(
  VAR(uint32, AUTOMATIC) ulRegAddr,
  VAR(uint32, AUTOMATIC) ulCheckValue,
  VAR(uint32, AUTOMATIC) ulWCMaskValue
)
{
  #if (ETH_WRITE_VERIFY_CHECK == WV_DISABLE)
  /* Suppress unused parameter warnings for the parameters below */
  /* MISRA Violation: START Msg(4:3112)-5 */
  (void) ulRegAddr;
  /* MISRA Violation: END Msg(4:3112)-5 */
  /* MISRA Violation: START Msg(4:3112)-5 */
  (void) ulCheckValue;
  /* MISRA Violation: END Msg(4:3112)-5 */
  /* MISRA Violation: START Msg(4:3112)-5 */
  (void) ulWCMaskValue;
  /* MISRA Violation: END Msg(4:3112)-5 */
  #endif

  #if (ETH_WRITE_VERIFY_CHECK != WV_DISABLE)
  VAR(uint32, AUTOMATIC) LulRegValue;

  LulRegValue = Eth_ReadRegister32(ulRegAddr);
  #if (ETH_WRITE_VERIFY_CHECK == WV_INIT_ONLY)
  if ((ETH_STATE_UNINIT == Eth_GenDriverState) &&
    (ulCheckValue != (LulRegValue & ulWCMaskValue)))
  {
    Dem_ReportErrorStatus(ETH_E_REG_WRITE_VERIFY, DEM_EVENT_STATUS_FAILED);
    #if (ETH_WRITE_VERIFY_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    RCar_RfsoFaultControl(ETH_WRITE_VERIFY_ERR_NOTIFICATION_TGT,
      RFSO_FAULT_TRANSIENT);
    #endif
  }
  else
  {
    /* No action required */
  }
  #else
  if (ulCheckValue != (LulRegValue & ulWCMaskValue))
  {
    Dem_ReportErrorStatus(ETH_E_REG_WRITE_VERIFY, DEM_EVENT_STATUS_FAILED);
    #if (ETH_WRITE_VERIFY_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    RCar_RfsoFaultControl(ETH_WRITE_VERIFY_ERR_NOTIFICATION_TGT,
      RFSO_FAULT_TRANSIENT);
    #endif
  }
  else
  {
    /* No action required */
  }
  #endif
  #endif
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-4 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-4 */
/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
