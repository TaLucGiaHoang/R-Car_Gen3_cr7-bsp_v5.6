/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Mac.c                                                   */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2020 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains Mac functions implementation of Ethernet                */
/* Driver  Component.                                                         */
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
* V1.1.0:  30-Sep-2016  : Update Compiler Abstraction and Memory Mapping
*                         follow Specification of AUTOSAR Release 4.2.2
*                         Added new functions: Eth_RetrieveMacCounter,
*                         Eth_RetrieveStatistics, Eth_ReportExtDem,
*                         Eth_GetFrameLostStatus, Eth_GetControllerErrorSts
* V1.1.1:  08-Dec-2016  : Implement read/write verify check
* V1.1.2:  10-Jan-2017  : Remove functions: Eth_GetFrameLostStatus
*                         Eth_GetControllerErrorSts, Eth_GetCounterValue
*                         Revise read verify check
* V1.1.3:  24-Jan-2017  : Remove ETH_E_MULTIPLECOLLISION, ETH_E_RX_FRAMES_LOST
*                         ETH_E_ALIGNMENT, since no hardware support
*                         Add ETH_E_TRANSMIT_RETRY_OVER, ETH_E_FRAME_RECEIVE,
*                         ETH_E_TRANSMIT_RETRY_OVER hardware dependent errors
* V1.1.4:  08-Mar-2017  : Updated MISRA-C checking
* V1.2.0:  22-Mar-2017  : Implemented Rx Frame lost detection (#73275)
* V1.2.1:  04-Apr-2017  : Removed PromMode from Eth_GstMacConfig
* V1.2.2:  17-Apr-2017  : Update STag_Eth_MacIntType and Eth_EcsiprBitsType
*                         follow ECSIPR register description in
*                         latest HW user manual Rev.0.53
* V1.2.3:  16-May-2017  : Updated source code based on FuSa review
* V1.2.4:  16-May-2017  : Support Magic Packet detection interrupt enable
*                         via interrupt
* V1.2.5:  17-Apr-2017  : Updated index of drop counters in Eth_GetDropCount and
*                         Eth_GetEtherStats
* V1.2.6:  18-Jun-2017  : Remove Payload CRC Module from R-Car V3M device
*                         Update memory mapping for global variable
*                         Eth_GucPayloadCrc
* V1.2.7:  19-Jun-2017  : Added critical section protection to register
*                         read-modify-write accesses
*                         Added critical section protection to global variable
*                         read-modify accesses
* V1.2.8:  08-Sep-2017  : Replace 'STATIC' keyword by 'static' keyword
* V1.2.9:  12-Sep-2017  : Remove Eth_MacStructConfig function
*                         Replace Eth_GaaDevice[].BaseAddr global variable by
*                         Eth_GaaBaseAddr constant
*                         Modify Eth_MacInit function
* V1.2.10: 05-Oct-2017  : Rename Eth_CalcMacAddr to Eth_GetMacAddr
* V1.2.11: 04-Dec-2017  : Update memory mapping for function Eth_GetMacAddr
*                         Move function Eth_CompareMac from Eth.c to Eth_Mac.c
*                         Update type of LenOption from uint8 to Eth_OptionType
* V1.2.12: 15-Dec-2017  : Update Compiler Abstraction
*                         Update casting to fix MISRA C Violations
*                         Replace static by STATIC
* V1.2.13: 26-Dec-2017  : Add more statistic Packet counters for
*                         Eth_GetEtherStats and drop packet counters for
*                         Eth_GetDropCount
* V1.2.14: 05-Feb-2018  : Update the calculating of counter value in
*                         Eth_GetDropCount and Eth_GetEtherStats APIs
*                         Update functions Eth_RetrieveMacCounter and
*                         Eth_RetrieveStatistics to keep the counter values
*                         after reading them
* V1.2.15: 14-Mar-2018  : Remove function Eth_SetRxFilterMode
*                         Add functions Eth_EnablePromMode, Eth_DisablePromMode
* V1.2.16: 05-Jun-2018  : Change "STATIC" keyword to "static".
* V1.2.17: 19-Aug-2018  : Update MISRA C rule violations
* V1.2.18: 23-Apr-2020  : Add MISRA C rule violation message Msg(4:5087)
*******************************************************************************/

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Included for prototypes for internal functions of Ethernet Component */
#include "Eth_Mac.h"

/* Included for Ethernet Component specific types used within the module */
#include "Eth_MACRegStruct.h"

/* Included DMAC Structure - only for the Base Address */
#include "Eth_DMACRegStruct.h"

#include "Eth_Ram.h"

/* Included for declaration of the function Dem_ReportErrorStatus() */
#include "Dem.h"

/* Included for declaration of functional safety APIs or macros */
#include "RCar_FuSa.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/* AUTOSAR release version information */
#define ETH_MAC_C_AR_RELEASE_MAJOR_VERSION \
  ETH_AR_RELEASE_MAJOR_VERSION_VALUE
#define ETH_MAC_C_AR_RELEASE_MINOR_VERSION \
  ETH_AR_RELEASE_MINOR_VERSION_VALUE
#define ETH_MAC_C_AR_RELEASE_REVISION_VERSION \
  ETH_AR_RELEASE_REVISION_VERSION_VALUE

/* File version information */
#define ETH_MAC_C_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION_VALUE
#define ETH_MAC_C_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION_VALUE

/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/

/* Functionality related to R4.0 */
#if (ETH_MAC_AR_RELEASE_MAJOR_VERSION != \
  ETH_MAC_C_AR_RELEASE_MAJOR_VERSION)
#error "Eth_Mac.c : Mismatch in Release Major Version"
#endif
#if (ETH_MAC_AR_RELEASE_MINOR_VERSION != \
  ETH_MAC_C_AR_RELEASE_MINOR_VERSION)
#error "Eth_Mac.c : Mismatch in Release Minor Version"
#endif
#if (ETH_MAC_AR_RELEASE_REVISION_VERSION != \
  ETH_MAC_C_AR_RELEASE_REVISION_VERSION)
#error "Eth_Mac.c : Mismatch in Release Revision Version"
#endif

#if (ETH_MAC_SW_MAJOR_VERSION != ETH_MAC_C_SW_MAJOR_VERSION)
#error "Eth_Mac.c : Mismatch in Software Major Version"
#endif
#if (ETH_MAC_SW_MINOR_VERSION != ETH_MAC_C_SW_MINOR_VERSION)
#error "Eth_Mac.c : Mismatch in Software Minor Version"
#endif

/*******************************************************************************
**                      Global Data                                           **
*******************************************************************************/
#define ETH_START_SEC_CONST_32
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */

#if (ETH_GET_DROP_COUNT_API == STD_ON)
/* Array Used in GetDropCount Api to return MAC HW Counters */
/* TRACE [ETH_DDD_GBL_034] */
/* MISRA Violation: START Msg(4:3218)-6 */
static CONST(uint32, ETH_CONST) Eth_GaaMacCounterAddress
/* MISRA Violation: END Msg(4:3218)-6 */
  [ETH_MAX_CTRLS_SUPPORTED][ETH_MAC_MAXCOUNTERS] =
{
  {
    /* 1 - Buffer Overrun */
    ETH_NOT_AVAILABLE,
    /* 2 - CRC Error */
    ETH_BASE + ETH_CEFCR,
    /* 3 - UNDERSIZE Packet */
    ETH_BASE + ETH_TSFRCR,
    /* 4 - OVERSIZE Packet */
    ETH_BASE + ETH_TLFRCR,
    /* 5 - ALIGNMENT Error */
    ETH_NOT_AVAILABLE,
    /* 6 - SQE Test Error */
    ETH_NOT_AVAILABLE,
    /* 7 - InBound Discarded*/
    ETH_NOT_AVAILABLE,
    /* 8 - Total Erroneous InBound */
    ETH_NOT_AVAILABLE,
    /* 9 - OutBound Discarded */
    ETH_NOT_AVAILABLE,
    /* 10 - Total Erroneous OutBound */
    ETH_NOT_AVAILABLE,
    /* 11 - Single Collision Frames */
    ETH_NOT_AVAILABLE,
    /* 12 - Multiple Collision Frames*/
    ETH_NOT_AVAILABLE,
    /* 13 - Deferred Transmission */
    ETH_NOT_AVAILABLE,
    /* 14 - Late Collision Frames */
    ETH_NOT_AVAILABLE,
    /* 15 - Frame Receive Error Counter */
    ETH_BASE + ETH_FRECR,
    /* 16 - Residual-Bit Frame Receive Counter */
    ETH_BASE + ETH_RFCR,
    /* 17 - Transmit retry over Counter */
    ETH_BASE + ETH_TROCR
  }
};
#endif /* (ETH_GET_DROP_COUNT_API == STD_ON) */
#define ETH_STOP_SEC_CONST_32
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */
/*******************************************************************************
**                  MISRA C Rule Violations                                   **
*******************************************************************************/
/* 1. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0303) [I] Cast between a pointer to volatile object     */
/*                 and an integral type.                                      */
/* Rule          : MISRA-C:2004 Rule 3.1, 11.3                                */
/*                 REFERENCE - ISO:C90-6.3.4 Cast Operators - Semantics       */
/* Justification : The casting is necessary to access descriptor and register */
/*                                                                            */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0303)-1 and                           */
/*                 END Msg(4:0303)-1 tags in the code.                        */
/******************************************************************************/
/* 2. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0492) Array subscripting applied to a function          */
/*                 parameter declared as a pointer.                           */
/* Rule          : MISRA-C:2004 Rule 17.4                                     */
/*                 REFERENCE - ISO:C90-6.3.2.1 Array Subscripting             */
/*                             ISO:C90 6.2.2.1 Lvalues and Function           */
/*                                     Designators                            */
/*                             ISO:C90 6.7.1 Function Definitions - Semantics */
/* Justification : Array is used to easy store data in memory, pointer is     */
/*                 used to easy access to memory. This is perfectly legitimate*/
/*                 in C as Array and pointer are substantially the same thing.*/
/*                 Array subscript implementation is correct.                 */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0492)-2 and                           */
/*                 END Msg(4:0492)-2 tags in the code.                        */
/******************************************************************************/
/* 3. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0759) An object of union type has been defined.         */
/* Rule          : MISRA-C:2004 Rule 18.4                                     */
/* Justification : To access only the lower byte of the converted values      */
/*                 from the conversion register.                              */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0759)-3 and                           */
/*                 END Msg(4:0759)-3 tags in the code.                        */
/******************************************************************************/
/* 4. MISRA C RULE VIOLATION:                                                 */
/* Message       : (2:2814) Possible: Dereference of NULL pointer.            */
/* Rule          : None                                                       */
/* Justification : The pointer cannot be NULL because:                        */
/*                 + NULL pointer has been checked before setting the address */
/*                 for pointer.                                               */
/*                 + The address which is used to set pointer is address of   */
/*                 register or global variable. All of them are not NULL      */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(2:2814)-4 and                           */
/*                 END Msg(2:2814)-4 tags in the code.                        */
/******************************************************************************/
/* 5. MISRA C RULE VIOLATION:                                                 */
/* Message       : (2:2824) Possible: Arithmetic operation on NULL pointer.   */
/* Rule          : None                                                       */
/* Justification : The pointer cannot be NULL because:                        */
/*                 + NULL pointer has been checked before setting the address */
/*                 for pointer.                                               */
/*                 + The address which is used to set pointer is address of   */
/*                 register or global variable. All of them are not NULL      */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(2:2824)-5 and                           */
/*                 END Msg(2:2824)-5 tags in the code.                        */
/******************************************************************************/
/* 6. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:3218) File scope static, '%s', is only accessed in one  */
/*                 function.                                                  */
/* Rule          : MISRA-C:2004 Rule 8.7                                      */
/* Justification : This static is constant variable and it is used to         */
/*                 prevent the cases that the compiler output errors by       */
/*                 constant variable declaration inside a function            */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:3218)-6 and                           */
/*                 END Msg(4:3218)-6 tags in the code.                        */
/******************************************************************************/
/* 7. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:4424) An expression of 'essentially enum' type (%1s)    */
/*                 is being converted to unsigned type, '%2s' on assignment.  */
/* Rule          : MISRA-C:2004 Rule 10.1                                     */
/* Justification : Assign enum to uint32 type. This assigning is              */
/*                 necessary to access register                               */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:4424)-7 and                           */
/*                 END Msg(4:4424)-7 tags in the code.                        */
/******************************************************************************/
/* 8. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:5087) #include statements in a file should only be      */
/*                  preceded by other preprocessor directives or comments     */
/* Rule          : MISRA-C:2004 Rule 19.1                                     */
/* Justification : include "Fls_MemMap.h" searches for defined symbol to map  */
/*                 memory section in memory mapping. This "include" should be */
/*                 added before and after the definition of memory section    */
/*                 This implementation is correct                             */
/* Verification  : Violation of this message were verified and reported       */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:5087)-8 and                           */
/*                 END Msg(4:5087)-8 tags in the code.                        */
/******************************************************************************/

/******************************************************************************/

/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/

/*******************************************************************************
** Function Name         : Eth_MacInit
**
** Service ID            : NA
**
** Description           : This Initialize the MAC registers
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : LucCtrlIdx  - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - driver error code
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GpConfigPtr
**
** Function(s) invoked   : Eth_WriteRegister32, Eth_StoreMacIntoReg
**                         Eth_VerifyWriteRegister32
**
** Registers Used        : GECMR, RFLR, ECSIPR, ECMR, MAHR, MALR, FSO_CTL
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */
/* TRACE[ETH_DDD_ACT_072] */
/* TRACE[ETH_DDD_INF_034] */
FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_MacInit
(
  uint8 LucCtrlIdx
)
{
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnValue;

  /* MISRA Violation: START Msg(4:0759)-3 */
  VAR(Eth_EcmrType, AUTOMATIC) LunEcmr;
  /* MISRA Violation: END Msg(4:0759)-3 */
  /* MISRA Violation: START Msg(4:0759)-3 */
  VAR(Eth_EcsiprType, AUTOMATIC) LunEcsIpr;
  /* MISRA Violation: END Msg(4:0759)-3 */
  /* MISRA Violation: START Msg(4:0759)-3 */
  VAR(Eth_GecmrType, AUTOMATIC) LunGecmr;
  /* MISRA Violation: END Msg(4:0759)-3 */

  if (ETH_MACRO_NUM <= LucCtrlIdx)
  {
    LenReturnValue = ETH_ERR_RANGE;
  }
  else
  {
    /* Set MAC address */
    /* MISRA Violation: START Msg(2:2814)-4 */
    Eth_StoreMacIntoReg(LucCtrlIdx, Eth_GpConfigPtr->ucEthMACAddr);
    /* MISRA Violation: END Msg(2:2814)-4 */

    /* Set transmission speed */
    LunGecmr.Word = ETH_ZERO_32;
    LunGecmr.Bit.Speed = Eth_GpConfigPtr->ucEthSpeed;

    /* TRACE[ETH_DDD_ACT_072_REG001] */
    Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_GECMR,
      LunGecmr.Word);

    /* Register write-verify check */
    /* TRACE[ETH_DDD_ACT_072_REG002] */
    Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_GECMR,
      LunGecmr.Word, ETH_REG_MASK);

    /* set Rx frame length */
    /* TRACE[ETH_DDD_ACT_072_REG003] */
    Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_RFLR,
      (uint32)ETH_CTRL_RX_BUF_LEN_BYTE);

    /* Register write-verify check */
    /* TRACE[ETH_DDD_ACT_072_REG004] */
    Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_RFLR,
      (uint32)ETH_CTRL_RX_BUF_LEN_BYTE, ETH_REG_MASK);

    /* set MAC interrupt */
    LunEcsIpr.Word = ETH_ZERO_32;
    /* MISRA Violation: START Msg(4:4424)-7 */
    LunEcsIpr.Bit.Icdip  = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-7 */
    LunEcsIpr.Bit.Mpdip  = Eth_GpConfigPtr->ucEthMagicDection;
    /* MISRA Violation: START Msg(4:4424)-7 */
    LunEcsIpr.Bit.Linkim = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-7 */
    /* MISRA Violation: START Msg(4:4424)-7 */
    LunEcsIpr.Bit.Phyim  = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-7 */
    /* MISRA Violation: START Msg(4:4424)-7 */
    LunEcsIpr.Bit.Pfrim  = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-7 */

    /* TRACE[ETH_DDD_ACT_072_REG005] */
    Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_ECSIPR,
      LunEcsIpr.Word);

    /* Register write-verify check */
    /* TRACE[ETH_DDD_ACT_072_REG006] */
    Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_ECSIPR,
      LunEcsIpr.Word, ETH_REG_MASK);

    /* Set MAC mode register */
    LunEcmr.Word = ETH_ZERO_32;

    LunEcmr.Bit.Dm   = Eth_GpConfigPtr->ucEthDuplex;
    LunEcmr.Bit.Mpde = Eth_GpConfigPtr->ucEthMagicDection;
    /* MISRA Violation: START Msg(4:4424)-7 */
    LunEcmr.Bit.Txf  = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-7 */
    /* MISRA Violation: START Msg(4:4424)-7 */
    LunEcmr.Bit.Rxf  = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-7 */
    /* MISRA Violation: START Msg(4:4424)-7 */
    LunEcmr.Bit.Pfr  = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-7 */
    /* MISRA Violation: START Msg(4:4424)-7 */
    LunEcmr.Bit.Tzpf = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-7 */
    /* MISRA Violation: START Msg(4:4424)-7 */
    LunEcmr.Bit.Rzpf = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-7 */
    /* MISRA Violation: START Msg(4:4424)-7 */
    LunEcmr.Bit.Dpad = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-7 */
    /* MISRA Violation: START Msg(4:4424)-7 */
    LunEcmr.Bit.Rcsc = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-7 */
    /* MISRA Violation: START Msg(4:4424)-7 */
    LunEcmr.Bit.Te   = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-7 */
    /* MISRA Violation: START Msg(4:4424)-7 */
    LunEcmr.Bit.Re   = ETH_DISABLE;
    /* MISRA Violation: END Msg(4:4424)-7 */

    /* Set Counter Clear Mode to clear when reading */
    LunEcmr.Bit.Trccm = ETH_ONE;

    /* TRACE[ETH_DDD_ACT_072_REG007] */
    Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_ECMR,
      LunEcmr.Word);

    /* Register write-verify check */
    /* TRACE[ETH_DDD_ACT_072_REG008] */
    Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_ECMR,
      LunEcmr.Word, ETH_REG_MASK);

    LenReturnValue = ETH_ERR_OK;
  } /* (ETH_MACRO_NUM <= LucCtrlIdx) */
  return LenReturnValue;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */

/*******************************************************************************
** Function Name         : Eth_MacCommunicationSet
**
** Service ID            : NA
**
** Description           : This performs the enabling and disabling of the
**                         communication Tx or Rx based on the direction
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                       : LenDirection - TX or RX direction
**                       : LenOption - Enable/Disable
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
** Function(s) invoked   : Eth_ReadRegister32, Eth_WriteRegister32
**                         Eth_VerifyWriteRegister32, Eth_VerifyReadRegister32
**
** Registers Used        : ECMR, FSO_CTL
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */
/* TRACE[ETH_DDD_ACT_069] */
/* TRACE[ETH_DDD_INF_036] */
FUNC(void, ETH_CODE_SLOW) Eth_MacCommunicationSet
(
  uint8 LucCtrlIdx,
  Eth_DirectionType LenDirection,
  Eth_OptionType LenOption
)
{
  /* MISRA Violation: START Msg(4:0759)-3 */
  VAR(Eth_EcmrType, AUTOMATIC) LunEcmr;
  /* MISRA Violation: END Msg(4:0759)-3 */
  /* TRACE[ETH_DDD_ACT_069_REG001] */
  LunEcmr.Word = Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
    ETH_ECMR);
  /* Register read-verify check */
  /* TRACE[ETH_DDD_ACT_069_REG004] */
  Eth_VerifyReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_ECMR,
    LunEcmr.Word);

  if (ETH_RX == LenDirection)
  {
    /* MISRA Violation: START Msg(4:4424)-7 */
    LunEcmr.Bit.Re = LenOption;
    /* MISRA Violation: END Msg(4:4424)-7 */
  }
  else if (ETH_TX == LenDirection)
  {
    /* MISRA Violation: START Msg(4:4424)-7 */
    LunEcmr.Bit.Te = LenOption;
    /* MISRA Violation: END Msg(4:4424)-7 */
  }
  else
  {
    /*Do Nothing*/
  }

  /* TRACE[ETH_DDD_ACT_069_REG002] */
  Eth_WriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
    ETH_ECMR, LunEcmr.Word);

  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_069_REG003] */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_ECMR,
    LunEcmr.Word, ETH_REG_MASK);
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */

/*******************************************************************************
** Function Name         : Eth_GetMacAddr
**
** Service ID            : NA
**
** Description           : This performs MAC Address Retrieval from the
**                         MAC address registers.
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : LpMacAddrPtr - MAC address is retrieved for
**                         the channel
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Function(s) invoked   : Eth_ReadRegister32, Eth_VerifyReadRegister32
**
** Registers Used        : MAHR, MALR
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */
/* TRACE[ETH_DDD_ACT_056] */
/* TRACE[ETH_DDD_INF_037] */
FUNC(void, ETH_CODE_FAST) Eth_GetMacAddr
(
  uint8 LucCtrlIdx,
  CONSTP2VAR(uint8, AUTOMATIC, ETH_APPL_DATA)LpMacAddrPtr
)
{
  VAR(uint32, AUTOMATIC) LulMACAddrHigh;
  VAR(uint32, AUTOMATIC) LulMACAddrLow;
  VAR(uint32, AUTOMATIC) LulMACAddrTemp;
  VAR(uint8, AUTOMATIC)  LucByteCount;

  /* Read the MAC Address from the MAC Address register */
  /* TRACE[ETH_DDD_ACT_056_REG001] */
  LulMACAddrHigh = Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
    ETH_MAHR);
  /* Register read-verify check */
  /* TRACE[ETH_DDD_ACT_056_REG003] */
  Eth_VerifyReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_MAHR,
    LulMACAddrHigh);

  /* TRACE[ETH_DDD_ACT_056_REG002] */
  LulMACAddrLow = Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
    ETH_MALR);
  /* Register read-verify check */
  /* TRACE[ETH_DDD_ACT_056_REG004] */
  Eth_VerifyReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_MALR,
    LulMACAddrLow);

  /* Assign the read MAC Address to the the  MacAddrPtr */
  for (LucByteCount = ETH_ZERO_8; LucByteCount < ETH_FOUR_8; LucByteCount++)
  {
    LulMACAddrTemp = LulMACAddrHigh >> (LucByteCount * ETH_EIGHT_8);
    /* MISRA Violation: START Msg(2:2824)-5 */
    /* MISRA Violation: START Msg(4:0492)-2 */
    LpMacAddrPtr[ETH_THREE_8 - LucByteCount] = (uint8)LulMACAddrTemp;
    /* MISRA Violation: END Msg(4:0492)-2 */
    /* MISRA Violation: END Msg(2:2824)-5 */
  }

  for (LucByteCount = ETH_ZERO_8; LucByteCount < ETH_TWO_8; LucByteCount++)
  {
    LulMACAddrTemp = LulMACAddrLow >> (LucByteCount * ETH_EIGHT_8);
    /* MISRA Violation: START Msg(4:0492)-2 */
    LpMacAddrPtr[ETH_FIVE_8 - LucByteCount] = (uint8)LulMACAddrTemp;
    /* MISRA Violation: END Msg(4:0492)-2 */
  }
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */

/*******************************************************************************
** Function Name         : Eth_StoreMacIntoReg
**
** Service ID            : NA
**
** Description           : Store the passed MAC Address into the MAC address
**                         registers.
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant for the same Controller
**                         Reentrant for different Controller
**
** Input Parameters      : LucCtrlIdx   - Index of the ETH Controller
**                       : LpMacAddrPtr - MAC address to be stored
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
** Function(s) invoked   : Eth_WriteRegister32, Eth_VerifyWriteRegister32
**
** Registers Used        : MAHR, MALR, FSO_CTL
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */
/* TRACE[ETH_DDD_ACT_115] */
/* TRACE[ETH_DDD_INF_038] */
FUNC(void, ETH_CODE_SLOW) Eth_StoreMacIntoReg
(
  VAR(uint8, AUTOMATIC) LucCtrlIdx,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) LpMacAddrPtr
)
{
  P2VAR(volatile uint32, AUTOMATIC, REGSPACE) LpMAHR;
  P2VAR(volatile uint32, AUTOMATIC, REGSPACE) LpMALR;
  VAR(uint32, AUTOMATIC) LulTempRegVal;

  /* MISRA Violation: START Msg(4:0303)-1 */
  LpMAHR = (volatile uint32 *)(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_MAHR);
  /* MISRA Violation: END Msg(4:0303)-1 */
  /* MISRA Violation: START Msg(4:0303)-1 */
  LpMALR = (volatile uint32 *)(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_MALR);
  /* MISRA Violation: END Msg(4:0303)-1 */

  /* Little Endian Core */
  /* MISRA Violation: START Msg(2:2824)-5 */
  /* MISRA Violation: START Msg(4:0492)-2 */
  LulTempRegVal = ((uint32)LpMacAddrPtr[ETH_FOUR] << ETH_EIGHT) |
  /* MISRA Violation: END Msg(4:0492)-2 */
  /* MISRA Violation: END Msg(2:2824)-5 */
    /* MISRA Violation: START Msg(4:0492)-2 */
    ((uint32)LpMacAddrPtr[ETH_FIVE]);
    /* MISRA Violation: END Msg(4:0492)-2 */

  /* Write into MALR */
  /* TRACE[ETH_DDD_ACT_115_REG001] */
  /* MISRA Violation: START Msg(4:0303)-1 */
  Eth_WriteRegister32((uint32)LpMALR, LulTempRegVal);
  /* MISRA Violation: END Msg(4:0303)-1 */
  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_115_REG002] */
  /* MISRA Violation: START Msg(4:0303)-1 */
  Eth_VerifyWriteRegister32((uint32)LpMALR, LulTempRegVal, ETH_REG_MASK);
  /* MISRA Violation: END Msg(4:0303)-1 */

  /* Write into MAHR */
  /* MISRA Violation: START Msg(4:0492)-2 */
  LulTempRegVal = ((uint32)LpMacAddrPtr[ETH_ZERO] << ETH_TWENTYFOUR) |
  /* MISRA Violation: END Msg(4:0492)-2 */
    /* MISRA Violation: START Msg(4:0492)-2 */
    ((uint32)LpMacAddrPtr[ETH_ONE] << ETH_SIXTEEN) |
    /* MISRA Violation: END Msg(4:0492)-2 */
    /* MISRA Violation: START Msg(4:0492)-2 */
    ((uint32)LpMacAddrPtr[ETH_TWO] << ETH_EIGHT) |
    /* MISRA Violation: END Msg(4:0492)-2 */
    /* MISRA Violation: START Msg(4:0492)-2 */
    ((uint32)LpMacAddrPtr[ETH_THREE]);
    /* MISRA Violation: END Msg(4:0492)-2 */

  /* TRACE[ETH_DDD_ACT_115_REG003] */
  /* MISRA Violation: START Msg(4:0303)-1 */
  Eth_WriteRegister32((uint32)LpMAHR, LulTempRegVal);
  /* MISRA Violation: END Msg(4:0303)-1 */
  /* Register write-verify check */
  /* TRACE[ETH_DDD_ACT_115_REG004] */
  /* MISRA Violation: START Msg(4:0303)-1 */
  Eth_VerifyWriteRegister32((uint32)LpMAHR, LulTempRegVal, ETH_REG_MASK);
  /* MISRA Violation: END Msg(4:0303)-1 */
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */

/*******************************************************************************
** Function Name         : Eth_CopyMacAddress
**
** Service ID            : NA
**
** Description           : This Api copy 6 bytes MAC DA or SA from address pSrc
**                          to address pDst
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant for the same Controller
**                         Reentrant for different Controller
**
** Input Parameters      : pSrc - Address where the MAC Address is stored in RAM
**
** InOut Parameters      : pDst - Address where the MAC need to be copied to.
**
** Output Parameters     : None
**
** Return parameter      : E_OK - if Data has been copied successfully
**                         E_NOT_OK - otherwise
**
** Preconditions         : None
**
** Global Variable(s)    : None
**
** Function(s) invoked   : None
**
** Registers Used        : None
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */
/* TRACE[ETH_DDD_ACT_053] */
/* TRACE[ETH_DDD_INF_039] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_CopyMacAddress
(
  CONSTP2VAR(volatile uint8, AUTOMATIC, ETH_APPL_DATA) pDst,
  CONSTP2CONST(volatile uint8, AUTOMATIC, ETH_APPL_CONST) pSrc
)
{
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  if ((NULL_PTR != pSrc) && (NULL_PTR != pDst))
  {
    for (LucCount = ETH_ZERO_8; ETH_MACADDR_SIZE > LucCount; LucCount++)
    {
      /* MISRA Violation: START Msg(4:0492)-2 */
      /* MISRA Violation: START Msg(4:0492)-2 */
      pDst[LucCount] = pSrc[LucCount];
      /* MISRA Violation: END Msg(4:0492)-2 */
      /* MISRA Violation: END Msg(4:0492)-2 */
    }

    LddReturnValue = E_OK;
  }
  else
  {
    LddReturnValue = E_NOT_OK;
  }

  return LddReturnValue;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */

/*******************************************************************************
** Function Name         : Eth_RetrieveMacCounter
**
** Service ID            : None
**
** Description           : This performs MAC Counter Retrieval from the
**                         counter registers.
**
** Sync/Async            : Synchronous
**
** Reentrancy            : None
**
** Input Parameters      : LucCtrlIdx  - Index of the ETH Controller
**                       : CountValues - Number of Values to be read
**
** InOut Parameters      : None
**
** Output Parameters     : LpDropCount - Pointer to list with drop counter value
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaMacCounterAddress, Eth_GaaDevice
**
** Function(s) invoked   : Eth_ReadRegister32
**
** Registers Used        : None
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */
#if (ETH_GET_DROP_COUNT_API == STD_ON)
/* TRACE[ETH_DDD_ACT_137] */
/* TRACE[ETH_DDD_INF_112] */
FUNC(void, ETH_CODE_SLOW) Eth_RetrieveMacCounter
(
  uint8 LucCtrlIdx,
  uint8 CountValues,
  CONSTP2VAR(uint32, AUTOMATIC, ETH_APPL_DATA) LpDropCount
)
{
  VAR(uint8, AUTOMATIC) LucIndex;

  for (LucIndex = ETH_ZERO_8; LucIndex < CountValues; LucIndex++)
  {
    /* MISRA Violation: START Msg(2:2824)-5 */
    /* MISRA Violation: START Msg(4:0492)-2 */
    LpDropCount[LucIndex] = ETH_NOT_AVAILABLE;
    /* MISRA Violation: END Msg(4:0492)-2 */
    /* MISRA Violation: END Msg(2:2824)-5 */
    if (ETH_ZERO_8 == LucIndex)
    {
      /* Dropped packets due to buffer overrun */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpDropCount[LucIndex] = Eth_GaaDevice[LucCtrlIdx].DropPacketNo;
      /* MISRA Violation: END Msg(4:0492)-2 */
    }
    else
    {
      /* No action required */
    }
    if (ETH_ONE_8 == LucIndex)
    {
      /* Crc Error packets */
      /* Get current Packet Counter */
      Eth_GaaDevice[LucCtrlIdx].LastCRCPacketNo += Eth_ReadRegister32(
        Eth_GaaMacCounterAddress[LucCtrlIdx][LucIndex]);
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpDropCount[LucIndex] = Eth_GaaDevice[LucCtrlIdx].LastCRCPacketNo;
      /* MISRA Violation: END Msg(4:0492)-2 */
    }
    else
    {
      /* No action required */
    }
    if (ETH_TWO_8 == LucIndex)
    {
      /* Undersize packets */
      /* Get current Packet Counter */
      Eth_GaaDevice[LucCtrlIdx].LastUndersizePacketNo += Eth_ReadRegister32(
        Eth_GaaMacCounterAddress[LucCtrlIdx][LucIndex]);
        /* MISRA Violation: START Msg(4:0492)-2 */
        LpDropCount[LucIndex] = Eth_GaaDevice[LucCtrlIdx].LastUndersizePacketNo;
        /* MISRA Violation: END Msg(4:0492)-2 */
    }
    else
    {
      /* No action required */
    }
    if (ETH_THREE_8 == LucIndex)
    {
      /* Oversize packets */
      /* Get current Packet Counter */
      Eth_GaaDevice[LucCtrlIdx].LastOversizePacketNo += Eth_ReadRegister32(
        Eth_GaaMacCounterAddress[LucCtrlIdx][LucIndex]);
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpDropCount[LucIndex] = Eth_GaaDevice[LucCtrlIdx].LastOversizePacketNo;
      /* MISRA Violation: END Msg(4:0492)-2 */
    }
    else
    {
      /* No action required */
    }
    if (ETH_SIX_8 == LucIndex)
    {
      /* Discarded Packets */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpDropCount[LucIndex] =
      /* MISRA Violation: END Msg(4:0492)-2 */
        Eth_GaaDevice[LucCtrlIdx].DropCount.DiscardedPacketNo;
    }
    else
    {
      /* No action required */
    }
    if (ETH_SEVEN_8 == LucIndex)
    {
      /* Erroneous inbound packets */
      /* Get Current Receive Error packets */
      Eth_GaaDevice[LucCtrlIdx].LastRxErrPacketNo +=
        Eth_ReadRegister32(
          Eth_GaaMacCounterAddress[LucCtrlIdx][ETH_FOURTEEN_8]);
      /* Get Current Residual-Bit packets */
      Eth_GaaDevice[LucCtrlIdx].LastResBitPacketNo +=
        Eth_ReadRegister32(
          Eth_GaaMacCounterAddress[LucCtrlIdx][ETH_FIFTEEN_8]);
      /* Get current erroneous inbound packets Packet Counter */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpDropCount[LucIndex] = Eth_GaaDevice[LucCtrlIdx].LastCRCPacketNo +
      /* MISRA Violation: END Msg(4:0492)-2 */
        Eth_GaaDevice[LucCtrlIdx].LastRxErrPacketNo +
          Eth_GaaDevice[LucCtrlIdx].LastResBitPacketNo;
      #if (ETH_PAYLOAD_CRC_ERROR_DETECT == STD_ON)
      LpDropCount[LucIndex] +=
          Eth_GaaDevice[LucCtrlIdx].DropCount.PayloadCRCErrPacketNo;
      #endif
    }
    else
    {
      /* No action required */
    }
    if (ETH_FOURTEEN_8 == LucIndex)
    {
      /* Receive Error packets */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpDropCount[LucIndex] = Eth_GaaDevice[LucCtrlIdx].LastRxErrPacketNo;
      /* MISRA Violation: END Msg(4:0492)-2 */
    }
    else
    {
      /* No action required */
    }
    if (ETH_FIFTEEN_8 == LucIndex)
    {
      /* Residual-Bit packets */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpDropCount[LucIndex] = Eth_GaaDevice[LucCtrlIdx].LastResBitPacketNo;
      /* MISRA Violation: END Msg(4:0492)-2 */
    }
    else
    {
      /* No action required */
    }
    if (ETH_SIXTEEN_8 == LucIndex)
    {
      /*  Transmit retry over counter */
      /* Get current Packet Counter */
      Eth_GaaDevice[LucCtrlIdx].LastTxRetryOverPacketNo +=
        Eth_ReadRegister32(Eth_GaaMacCounterAddress[LucCtrlIdx][LucIndex]);
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpDropCount[LucIndex] =
      /* MISRA Violation: END Msg(4:0492)-2 */
        Eth_GaaDevice[LucCtrlIdx].LastTxRetryOverPacketNo;
    }
    else
    {
      /* No action required */
    }
  }
}
#endif

/*******************************************************************************
** Function Name         : Eth_RetrieveStatistics
**
** Service ID            : None
**
** Description           : Retrieve Ethernet Counter from the counter registers
**                         according to IETF RFC2819
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : LpEtherStats - Pointer to drop counter values
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaDevice, Eth_GaaBaseAddr
**
** Function(s) invoked   : Eth_ReadRegister32
**
** Registers Used        : MAFCR, CEFCR, TSFRCR, TLFRCR
*******************************************************************************/
#if (ETH_GET_ETHER_STATS_API == STD_ON)
/* TRACE[ETH_DDD_ACT_138] */
/* TRACE[ETH_DDD_INF_113] */
FUNC(void, ETH_CODE_SLOW) Eth_RetrieveStatistics
(
  uint8 LucCtrlIdx,
  CONSTP2VAR(uint32, AUTOMATIC, ETH_APPL_DATA) LpEtherStats
)
{
  VAR(uint8, AUTOMATIC) LucIndex;

  for (LucIndex = ETH_ZERO_8; LucIndex < ETH_MAXSTATISTICS; LucIndex++)
  {
    switch (LucIndex)
    {
    case ETH_ZERO_8:
      /* Get current drop packet counter */
      /* MISRA Violation: START Msg(2:2824)-5 */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpEtherStats[LucIndex] = Eth_GaaDevice[LucCtrlIdx].DropPacketNo;
      /* MISRA Violation: END Msg(4:0492)-2 */
      /* MISRA Violation: END Msg(2:2824)-5 */
      break;
    case ETH_ONE_8:
      /* Get current Octets counter */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpEtherStats[LucIndex] = Eth_GaaDevice[LucCtrlIdx].Stats.StatsOctetsNo;
      /* MISRA Violation: END Msg(4:0492)-2 */
      break;
    case ETH_TWO_8:
      /* Rx Packet Counter */
      /* Get current Rx Packet Counter */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpEtherStats[LucIndex] = Eth_GaaDevice[LucCtrlIdx].Stats.RxPacketNo;
      /* MISRA Violation: END Msg(4:0492)-2 */
      break;
    case ETH_THREE_8:
      /* Rx Broadcast Counter */
      /* Get current Rx Broadcast Counter */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpEtherStats[LucIndex] = Eth_GaaDevice[LucCtrlIdx].Stats.BroadcastPktNo;
      /* MISRA Violation: END Msg(4:0492)-2 */
      break;
    case ETH_FOUR_8:
      /* Multicast Frame Counter */
      /* TRACE[ETH_DDD_ACT_138_REG001] */
      Eth_GaaDevice[LucCtrlIdx].Stats.LastMulticastPktNo +=
        Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_MAFCR);
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpEtherStats[LucIndex] =
      /* MISRA Violation: END Msg(4:0492)-2 */
        Eth_GaaDevice[LucCtrlIdx].Stats.LastMulticastPktNo;
      break;
    case ETH_FIVE_8:
      /* Crc Error Counter  */
      /* Get current Crc Error Counter */
      Eth_GaaDevice[LucCtrlIdx].LastCRCPacketNo +=
        Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_CEFCR);
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpEtherStats[LucIndex] = Eth_GaaDevice[LucCtrlIdx].LastCRCPacketNo;
      /* MISRA Violation: END Msg(4:0492)-2 */
      break;
    case ETH_SIX_8:
      /* Too Short Frames Counter */
      /* TRACE[ETH_DDD_ACT_138_REG003] */
      Eth_GaaDevice[LucCtrlIdx].LastUndersizePacketNo +=
        Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_TSFRCR);
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpEtherStats[LucIndex] =
      /* MISRA Violation: END Msg(4:0492)-2 */
        Eth_GaaDevice[LucCtrlIdx].LastUndersizePacketNo;
      break;
    case ETH_SEVEN_8:
      /* Too Long Frames Counter */
      /* TRACE[ETH_DDD_ACT_138_REG004] */
      Eth_GaaDevice[LucCtrlIdx].LastOversizePacketNo +=
        Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_TLFRCR);
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpEtherStats[LucIndex] = Eth_GaaDevice[LucCtrlIdx].LastOversizePacketNo;
      /* MISRA Violation: END Msg(4:0492)-2 */
      break;
    case ETH_ELEVEN_8:
      /* Ethernet statistic packet 64 Octets counter */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpEtherStats[LucIndex] = Eth_GaaDevice[LucCtrlIdx].Stats.Pkts64OctetsNo;
      /* MISRA Violation: END Msg(4:0492)-2 */
      break;
    case ETH_TWELVE_8:
      /* Ethernet statistic packet 65 to 127 Octets counter */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpEtherStats[LucIndex] =
      /* MISRA Violation: END Msg(4:0492)-2 */
        Eth_GaaDevice[LucCtrlIdx].Stats.Pkts65to127OctetsNo;
      break;
    case ETH_THIRTEEN_8:
      /* Ethernet statistic packet 128 to 255 Octets counter */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpEtherStats[LucIndex] =
      /* MISRA Violation: END Msg(4:0492)-2 */
        Eth_GaaDevice[LucCtrlIdx].Stats.Pkts128to255OctetsNo;
      break;
    case ETH_FOURTEEN_8:
      /* Ethernet statistic packet 256 to 511 Octets counter */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpEtherStats[LucIndex] =
      /* MISRA Violation: END Msg(4:0492)-2 */
        Eth_GaaDevice[LucCtrlIdx].Stats.Pkts256to511OctetsNo;
      break;
    case ETH_FIFTEEN_8:
      /* Ethernet statistic packet 512 to 1023 Octets counter */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpEtherStats[LucIndex] =
      /* MISRA Violation: END Msg(4:0492)-2 */
        Eth_GaaDevice[LucCtrlIdx].Stats.Pkts512to1023OctetsNo;
      break;
    case ETH_SIXTEEN_8:
      /* Ethernet statistic packet 1024 to 1518 Octets counter */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpEtherStats[LucIndex] =
      /* MISRA Violation: END Msg(4:0492)-2 */
        Eth_GaaDevice[LucCtrlIdx].Stats.Pkts1024to1518OctetsNo;
      break;
    default:
      /* Statistic Not Available in HW */
      /* MISRA Violation: START Msg(4:0492)-2 */
      LpEtherStats[LucIndex] = ETH_NOT_AVAILABLE;
      /* MISRA Violation: END Msg(4:0492)-2 */
      break;
    } /* (LucIndex) */
  }
}
#endif

/*******************************************************************************
** Function Name         : Eth_ReportExtDem
**
** Service ID            : None
**
** Description           : This performs checking for controller errors
**                         (e.g. CRC errors) and lost frames
**
** Sync/Async            : Synchronous
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
** Global Variable(s)    : Eth_GaaDevice, Eth_GaaBaseAddr
**
** Function(s) invoked   : Eth_ReadRegister32, Dem_ReportErrorStatus
**
** Registers Used        : CEFCR, TSFRCR, TLFRCR, RFCR, TROCR, FRECR
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_139] */
/* TRACE[ETH_DDD_INF_114] */
FUNC(void, ETH_CODE_SLOW) Eth_ReportExtDem(uint8 LucCtrlIdx)
{
  VAR(uint32, AUTOMATIC) LulCounterVal;
  VAR(uint32, AUTOMATIC) LulCurrentDropPacketNo;

  /* Read CEFCR - CRC Error */
  /* TRACE[ETH_DDD_ACT_139_REG001] */
  LulCounterVal = Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] +
    ETH_CEFCR);
  #if ((ETH_GET_DROP_COUNT_API == STD_ON) || \
    (ETH_GET_ETHER_STATS_API == STD_ON))
  LulCounterVal += Eth_GaaDevice[LucCtrlIdx].LastCRCPacketNo;
  if (LulCounterVal != Eth_GaaDevice[LucCtrlIdx].LastCRCPacketMainApi)
  {
    Eth_GaaDevice[LucCtrlIdx].LastCRCPacketNo = LulCounterVal;
    Eth_GaaDevice[LucCtrlIdx].LastCRCPacketMainApi = LulCounterVal;
  #else
  if (ETH_ZERO_32 < LulCounterVal)
  {
  #endif
    /* CRC Failure - DEM is required */
    Dem_ReportErrorStatus(ETH_E_CRC, DEM_EVENT_STATUS_PREFAILED);
  }
  else
  {
    /* No Error */
  }

  /* Read TSFRCR - Frame Size Underflow */
  /* TRACE[ETH_DDD_ACT_139_REG002] */
  LulCounterVal =
    Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_TSFRCR);

  #if ((ETH_GET_DROP_COUNT_API == STD_ON) || \
    (ETH_GET_ETHER_STATS_API == STD_ON))
  LulCounterVal += Eth_GaaDevice[LucCtrlIdx].LastUndersizePacketNo;
  if (LulCounterVal != Eth_GaaDevice[LucCtrlIdx].LastUndersizePacketMainApi)
  {
    Eth_GaaDevice[LucCtrlIdx].LastUndersizePacketNo = LulCounterVal;
    Eth_GaaDevice[LucCtrlIdx].LastUndersizePacketMainApi = LulCounterVal;
  #else
  if (ETH_ZERO_32 < LulCounterVal)
  {
  #endif
    /* Frame Size Underflow - DEM is required */
    Dem_ReportErrorStatus(ETH_E_UNDERSIZEFRAME, DEM_EVENT_STATUS_PREFAILED);
  }
  else
  {
    /* No Error */
  }

  /* Read TLFRCR - Frame Size Overflow */
  /* TRACE[ETH_DDD_ACT_139_REG003] */
  LulCounterVal =
    Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_TLFRCR);
  #if ((ETH_GET_DROP_COUNT_API == STD_ON) || \
    (ETH_GET_ETHER_STATS_API == STD_ON))
  LulCounterVal += Eth_GaaDevice[LucCtrlIdx].LastOversizePacketNo;
  if (LulCounterVal != Eth_GaaDevice[LucCtrlIdx].LastOversizePacketMainApi)
  {
    Eth_GaaDevice[LucCtrlIdx].LastOversizePacketNo = LulCounterVal;
    Eth_GaaDevice[LucCtrlIdx].LastOversizePacketMainApi = LulCounterVal;
  #else
  if (ETH_ZERO_32 < LulCounterVal)
  {
  #endif
    /* Frame Size Overflow - DEM is required */
    Dem_ReportErrorStatus(ETH_E_OVERSIZEFRAME, DEM_EVENT_STATUS_PREFAILED);
  }
  else
  {
    /* No Error */
  }

  /* Read RFCR - Residual-Bit Frame Receive */
  /* TRACE[ETH_DDD_ACT_139_REG004] */
  LulCounterVal =
    Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_RFCR);
  #if (ETH_GET_DROP_COUNT_API == STD_ON)
  LulCounterVal += Eth_GaaDevice[LucCtrlIdx].LastResBitPacketNo;
  if (LulCounterVal != Eth_GaaDevice[LucCtrlIdx].LastResBitPacketMainApi)
  {
    Eth_GaaDevice[LucCtrlIdx].LastResBitPacketNo = LulCounterVal;
    Eth_GaaDevice[LucCtrlIdx].LastResBitPacketMainApi = LulCounterVal;
  #else
  if (ETH_ZERO_32 < LulCounterVal)
  {
  #endif
    /* Residual-Bit Frame Error - DEM is required */
    Dem_ReportErrorStatus(ETH_E_RESIDUAL_BIT_FRAME, DEM_EVENT_STATUS_PREFAILED);
  }
  else
  {
    /* No Error */
  }

  /* Read TROCR - Transmit retry over counter */
  /* TRACE[ETH_DDD_ACT_139_REG005] */
  LulCounterVal =
    Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_TROCR);

  #if (ETH_GET_DROP_COUNT_API == STD_ON)
  LulCounterVal += Eth_GaaDevice[LucCtrlIdx].LastTxRetryOverPacketNo;
  if (LulCounterVal != Eth_GaaDevice[LucCtrlIdx].LastTxRetryOverPacketMainApi)
  {
    Eth_GaaDevice[LucCtrlIdx].LastTxRetryOverPacketNo = LulCounterVal;
    Eth_GaaDevice[LucCtrlIdx].LastTxRetryOverPacketMainApi = LulCounterVal;
  #else
  if (ETH_ZERO_32 < LulCounterVal)
  {
  #endif
    /* Transmit retry over error - DEM is required */
    Dem_ReportErrorStatus(ETH_E_TRANSMIT_RETRY_OVER,
      DEM_EVENT_STATUS_PREFAILED);
  }
  else
  {
    /* No Error */
  }

  /* Read FRECR - Frame Receive Error Counter */
  /* TRACE[ETH_DDD_ACT_139_REG006] */
  LulCounterVal =
    Eth_ReadRegister32(Eth_GaaBaseAddr[LucCtrlIdx] + ETH_FRECR);
  #if (ETH_GET_DROP_COUNT_API == STD_ON)
  LulCounterVal += Eth_GaaDevice[LucCtrlIdx].LastRxErrPacketNo;
  if (LulCounterVal != Eth_GaaDevice[LucCtrlIdx].LastRxErrPacketMainApi)
  {
    Eth_GaaDevice[LucCtrlIdx].LastRxErrPacketNo = LulCounterVal;
    Eth_GaaDevice[LucCtrlIdx].LastRxErrPacketMainApi = LulCounterVal;
  #else
  if (ETH_ZERO_32 < LulCounterVal)
  {
  #endif
    /* Frame Receive Error are detected - DEM is required */
    Dem_ReportErrorStatus(ETH_E_FRAME_RECEIVE, DEM_EVENT_STATUS_PREFAILED);
  }
  else
  {
    /* No Error */
  }
  /* Get current Drop packet Counter */
  LulCurrentDropPacketNo = Eth_GaaDevice[LucCtrlIdx].DropPacketNo;
  /* Compare the last stored number with current number */
  if (Eth_GaaDevice[LucCtrlIdx].LastDropPacketNoMainApi !=
    LulCurrentDropPacketNo)
  {
    /* Frame Lost are detected - DEM is required */
    Dem_ReportErrorStatus(ETH_E_RX_FRAMES_LOST, DEM_EVENT_STATUS_PREFAILED);
    /* Clear the counter */
    Eth_GaaDevice[LucCtrlIdx].LastDropPacketNoMainApi = LulCurrentDropPacketNo;
  }
  else
  {
    /* No Error */
  }
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */

/*******************************************************************************
** Function Name         : Eth_EnablePromMode
**
** Service ID            : NA
**
** Description           : Enable Promiscuous mode
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_SetBitRegister32, Eth_VerifyWriteRegister32
**
** Registers Used        : ECMR
**
*******************************************************************************/
#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */
/* TRACE[ETH_DDD_ACT_158] */
/* TRACE[ETH_DDD_INF_142] */
FUNC(void, ETH_CODE_SLOW) Eth_EnablePromMode
(
  uint8 CtrlIdx
)
{
  Eth_SetBitRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_ECMR, ETH_ECMR_PRM_BIT);
  /* Register write-verify check */
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_ECMR,
    (ETH_BIT0_SET_32 << ETH_ECMR_PRM_BIT),
    (ETH_BIT0_SET_32 << ETH_ECMR_PRM_BIT));
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */
#endif

/*******************************************************************************
** Function Name         : Eth_DisablePromMode
**
** Service ID            : NA
**
** Description           : Disable Promiscuous mode
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Functions invoked     : Eth_ClearBitRegister32, Eth_VerifyWriteRegister32
**
** Registers Used        : ECMR
**
*******************************************************************************/
#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */
/* TRACE[ETH_DDD_ACT_170] */
/* TRACE[ETH_DDD_INF_143] */
FUNC(void, ETH_CODE_SLOW) Eth_DisablePromMode
(
  uint8 CtrlIdx
)
{
  Eth_ClearBitRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_ECMR, ETH_ECMR_PRM_BIT);
  Eth_VerifyWriteRegister32(Eth_GaaBaseAddr[CtrlIdx] + ETH_ECMR,
    ETH_ZERO_32, (ETH_BIT0_SET_32 << ETH_ECMR_PRM_BIT));

}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */
#endif

/*******************************************************************************
** Function Name         : Eth_CompareMac
**
** Service ID            : NA
**
** Description           : This API compare 2 MAC Address
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant for the same CtrlIdx,
**                         reentrant for different
**
** Input Parameters      : pSrcMac, pDstMac - Pointer to the 2 MACs to be
**                         compared
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
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
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */
/* TRACE[ETH_DDD_ACT_099] */
/* TRACE[ETH_DDD_INF_030] */
FUNC(Std_ReturnType, ETH_CODE_FAST) Eth_CompareMac
(
  CONSTP2CONST(volatile uint8, AUTOMATIC, ETH_APPL_CONST) pSrcMac,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) pDstMac
)
{
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  if ((NULL_PTR != pSrcMac) && (NULL_PTR != pDstMac))
  {
    LucCount = ETH_ZERO_8;

    while ((ETH_MACADDR_SIZE > LucCount) &&
      /* MISRA Violation: START Msg(4:0492)-2 */
      /* MISRA Violation: START Msg(4:0492)-2 */
      (pSrcMac[LucCount] == pDstMac[LucCount]))
      /* MISRA Violation: END Msg(4:0492)-2 */
      /* MISRA Violation: END Msg(4:0492)-2 */
    {
      LucCount++;
    }

    if (ETH_MACADDR_SIZE == LucCount)
    {
      /* Address matches */
      LddReturnValue = E_OK;
    }
    else
    {
      /*Address are different */
      LddReturnValue = E_NOT_OK;
    }
  }
  else
  {
    LddReturnValue = E_NOT_OK;
  }

  return LddReturnValue;
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-8 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-8 */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
