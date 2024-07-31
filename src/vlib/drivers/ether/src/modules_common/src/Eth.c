/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth.c                                                       */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains API function implementations of ETH Driver              */
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
*                         Implement Error Detection Circuit code
*                         Implement Eth_GetVersionInfo API as function
*                         Unify version number
* V1.1.0:  30-Sep-2016  : Update Compiler Abstraction and Memory Mapping
*                         follow Specification of AUTOSAR Release 4.2.2
*                         Added new 4.2.2 APIs: Eth_GetDropCount,
*                         Eth_GetEtherStats, Eth_MainFunction
*                         Merged Eth_ControllerInit into Eth_Init
* V1.1.1:  08-Dec-2016  : Added code to reported interrupt controller check
*                         and Payload CRC error to the external systems through
*                         RFSO external pins
*                         Added code to target the EDC error notification
*                         to internal interrupt signal (INTC).
* V1.1.2:  10-Jan-2017  : Remove Eth_GetCounterState API
* V1.1.3:  24-Jan-2017  : Wrap Eth_Stbe.h and Eth_StbeEnableEDCProtection()
*                         inside ETH_DEVICE_NAME macro because STBE does not
*                         supported in V3M
* V1.2.0:  07-Mar-2017  : Updated source code to support static memory
*                         allocation for descriptors and buffers
*                         via linker script (#68800)
* V1.2.1:  08-Mar-2017  : Updated MISRA-C checking
*                         Change H3_ES2_0 to H3_WS2_0
* V1.2.2:  22-Mar-2017  : Corrected the flow of unlocking buffer (#77583)
*                         Corrected the size of frame handling in
*                         Eth_ProvideTxBuffer, Eth_Transmit
*                         and Eth_Receive (#77452)
*                         Added checking controller access and modified
*                         Eth_ModuleStopCheck (#73275)
*                         Added DropPacketNo to collect
*                         No. of lost frames (#73275)
* V1.3.0:  05-Apr-2017  : Fixed flow of Physical Address Filtering Mechanism
*                         Fixed flow of Eth_Receive API
*                         Support reception of single frame multi buffers
* V1.3.1:  17-Apr-2017  : Change name of Eth_PayloadCRCAreaStartset to
*                         Eth_PayloadCRCAreaStartSet
*                         Update the valid RegIdx checking in Eth_WriteMii,
*                         Eth_ReadMii
*                         Update Eth_IsRxFrameValid function to remove
*                         function call in "If" statement
* V1.3.2:  16-May-2017  : Update DET checking for functions:
*                         Eth_SetControllerMode, Eth_GetControllerMode,
*                         Eth_GetDropCount, Eth_GetEtherStats.
*                         Updated source code based on FuSa review
*                         Support adjustment the ETH local clock
* V1.3.3:  17-May-2017  : Corrected the flow of TxConfirmation
*                         Changed the name of Eth_TxBufferRelease to
*                         Eth_TxBufferCheck
* V1.3.4:  17-May-2017  : Revised the flow of Eth_Transmit
* V1.3.5:  20-May-2017  : Revised the flow of Eth_SetCorrectionTime
*                         Updated DET checking for Eth_SetGlobalTime
* V1.3.6:  17-Jun-2017  : Added volatile to local pointers which are used to
*                         access shared data
* V1.3.7:  18-Jun-2017  : Move Eth_SetRxFilterMode from Eth.c to
*                         Eth_Mac.c
*                         Remove Payload CRC Module and Error Detection Circuit
*                         (EDC)for Stream Buffer from R-Car V3M device
*                         Change type of BufPtr in Eth_ProvideTxBuffer from
*                         Eth_DataType** to uint8**
*                         Update memory mapping for functions which are related
*                         to interrupt
*                         Move function Eth_DMACReset from Eth_Dma.c to Eth.c
* V1.3.8:  19-Jun-2017  : Added critical section protection to global variable
*                         read-modify accesses
* V1.3.9:  22-Jun-2017  : Modified Eth_SetControllerMode to reset Tx/Rx Queues
*                         when setting mode DOWN
*                         Removed redundant steps in Eth_Init and Eth_Transit
* V1.3.10: 25-Aug-2017  : Update Eth_GetDropCount to only return values in the
*                         list of counters
* V1.3.11: 08-Sep-2017  : Replace 'STATIC' keyword by 'static' keyword
*                         Removed redundant steps in Eth_Init and Eth_Transmit
* V1.4.0:  12-Sep-2017  : Changed Eth_GucBroadcastAddr and Eth_GucNullAddr
*                         global variables to Eth_GaaBroadcastAddr,
*                         Eth_GaaNullAddr constants
*                         Removed Eth_GstPadding global variable and
*                         Eth_AvbIfStructConfig internal functions
*                         Modify Eth_Init, Eth_Receive, Eth_RxIrqHdlr_01,
*                         Eth_ModeTransitionProcess, Eth_DeviceStructInit
*                         Eth_RxConfig to remove some global variables usage
* V1.5.0:  28-Sep-2017  : Replace Eth_GstBufferLock[].bufAddr by
*                         Eth_GstBufferLock[].descAddr
*                         Revise Eth_TxBufferCheck to check the descriptor
*                         of each buffer instead of the Queue
*                         Add Eth_WaitDescProcessComplete function and use it
*                         inside Eth_Transmit to wait for data transferring
*                         completion if TxConfirmation is false
*                         Release buffer when transmission failed
* V1.5.1:  05-Oct-2017  : Modify Eth_IsRxFrameValid function to compare with
*                         HW MAC address
* V1.5.2:  04-Dec-2017  : Move:
*                         - Functions: Eth_TxBufferCheck, Eth_RxFilterProcess,
*                           Eth_RxCallEthIf, Eth_IsRxFrameValid,
*                           Eth_GetRxFilterMode from Eth.c to Eth_Dma.c
*                         - Functions: Eth_RxIrqHdlr_01, Eth_TxIrqHdlr_01
*                           from Eth.c to Eth_Irq.c
*                         - Function Eth_CompareMac from Eth.c to Eth_Mac.c
*                         - Global arrays Eth_GaaBroadcastAddr, Eth_GaaNullAddr
*                           definition from Eth.c to Eth_Ram.c
*                         Update type of operating status variable from uint8
*                         to Eth_OpStatusType
* V1.5.3:  15-Dec-2017  : Update Compiler Abstraction
*                         Remove redundant input parameters
*                         Update casting to fix MISRA C Violations
*                         Replace static by STATIC
* V1.5.4:  15-Dec-2017  : Unify timeout handling
*                         Remove Eth_WaitDescProcessComplete function
* V1.5.5:  19-Dec-2017  : Add report DET error for Eth_SetPhysAddr when
*                         mode is not ETH_MODE_DOWN
*                         Update check value for the valid TrcvIdx in
*                         Eth_WriteMii and Eth_ReadMii
*                         Remove redundant void casting
* V1.5.6:  23-Dec-2017  : Update Eth_Transmit to use global MAC address
*                         Flush cache data after creating frame header
*                         Modify Eth_Receive to read UFCV register and move
*                         Eth_RxDescChainUpdate to after Eth_RxCallEthIf
*                         Update Eth_SetPhysAddr to change global MAC address
*                         Initialize MacAddr and UFCOverflow global variables
* V1.5.7:  25-Dec-2017  : Add error handling during Operating mode transition
*                         Report DET error if Eth_Init is called when Eth driver
*                         has already been initialized
* V1.5.8:  25-Dec-2017  : Add checking for return value of functions:
*                         Eth_PhyWrite0, Eth_PhyWrite1
*                         Eth_PhyWrite, Eth_PhyRead, Eth_MIIFrameHeaderSet,
*                         Eth_PhyBusRelease
* V1.5.9:  26-Dec-2017  : Add more statistic Packet counters for
*                         Eth_GetEtherStats and drop packet counters for
*                         Eth_GetDropCount
* V1.5.10: 18-Jan-2018  : Update function Eth_DeviceStructInit to skip MAC
*                         address initialization if driver initialized
* V1.5.11: 05-Feb-2018  : Add branch process without
*                         ETH_AVB_EDC_ERR_NOTIFICATION for M3.
*                         Update function Eth_DeviceStructInit to remove last
*                         statistic and drop packet counter value initialization
*                         Update minimum buffer size checking in API
*                         Eth_ProvideTxBuffer
* V1.5.12: 14-Mar-2018  : Getting next Rx descriptor is moved out of
*                         Eth_RxQueueProcess
*                         Add initial value for component ucChannel of global
*                         variable Eth_GstBufferLock in API Eth_Init
*                         Support updating filter addresses in ACTIVE mode
*                         Update unread frame checking
* V1.5.13: 26-Apr-2018  : Remove input parameter Action in function
*                         Eth_WriteIntoSFPReg
* V1.5.14: 10-May-2018  : Add pre-processor for M3N
* V1.5.15: 18-May-2018  : Move calling of Eth_DeviceStructInit from
*                         Eth_SetControllerMode to Eth_MainFunction
*                         Change "STATIC" keyword to "static".
* V1.5.16: 11-Jun-2018  : Invalidate "EDC for STBE" for M3 and M3N
*                         Invalidate "Payload CRC" for M3
*                         Invalidate "EDC for EthernetAVB-IF" for M3N
* V1.5.17: 26-Jun-2018  : Update Eth_Transmit to move step trigger
*                         buffer confirmation right before step trigger frame
*                         transmission
* V1.5.18: 09-Aug-2018  : Moved buffer release from Eth_Transmit to
*                         Eth_TxConfirmation when TxConfirmation is false
*                         Remove safety mechanism EDC for EthernetAVB-IF
*                         Update MISRA C rule violations
* V1.5.19: 22-Aug-2018  : Invalidate "Payload CRC" for M3N
* V1.5.20: 08-Oct-2018  : Update Registers Used information of function
*                         Eth_GetDropCount
* V1.5.21: 02-May-2019  : Remove the DET checking ETH_E_INV_MODE in functions
*                         Eth_WriteMii and Eth_ReadMii
* V1.5.22: 04-Feb-2020  : Add step to reset field benableTS of transmit buffers
*                         in API Eth_Transmit after a transmission failure
*                         Suppress unused parameter warnings in functions
*                         Eth_GetIngressTimeStamp, Eth_ProvideTxBuffer
*                         Add MISRA C rule violation message Msg(4:5087) and
*                         (4:3112)
* V1.6.0:  23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

/*******************************************************************************
**                              Include Section                               **
*******************************************************************************/
/* Included for module version information and other types declarations */
#include "ether_api.h"
#include "Eth_Ram.h"
#include "Eth_Dma.h"
#include "Eth_Mac.h"
#include "EthIf_Cbk.h"
#include "EthTrcv_Cbk.h"
#include "Eth_Common.h"
#include "rcar-xos/osal/r_osal.h"
#include "r_print_api.h"
/* Included for declaration of RCar specific */
#include "RCar_Types.h"
#if (ETH_DEVICE_NAME == RCAR_H3)
#include "Eth_Stbe.h"
#endif
#include "RCar_FuSa.h"
#if (ETH_CTRL_ENABLE_MII == STD_ON)
#include "Eth_Phy.h"
#endif
#if (ETH_DEV_ERROR_DETECT == STD_ON)
/* Included for the declaration of Det_ReportError() */
#include "Det.h"
#endif
/* Included for declaration of the function Dem_ReportErrorStatus() */
#include "Dem.h"
#if (ETH_CRITICAL_SECTION_PROTECTION == STD_ON)
/* Included for the declaration of the critical section protection functions */
#include "SchM_Eth.h"
#endif
/*******************************************************************************
**                           Version Information                              **
*******************************************************************************/
/* AUTOSAR release version information */

/* File version information */
#define ETH_C_AR_RELEASE_MAJOR_VERSION    ETH_AR_RELEASE_MAJOR_VERSION_VALUE
#define ETH_C_AR_RELEASE_MINOR_VERSION    ETH_AR_RELEASE_MINOR_VERSION_VALUE
#define ETH_C_AR_RELEASE_REVISION_VERSION ETH_AR_RELEASE_REVISION_VERSION_VALUE

/* File version information */
#define ETH_C_SW_MAJOR_VERSION            1
#define ETH_C_SW_MINOR_VERSION            5

/*******************************************************************************
**                               Version Check                                **
*******************************************************************************/
#if (ETH_AR_RELEASE_MAJOR_VERSION != ETH_C_AR_RELEASE_MAJOR_VERSION)
#error "Eth.c : Mismatch in Release Major Version"
#endif

#if (ETH_AR_RELEASE_MINOR_VERSION != ETH_C_AR_RELEASE_MINOR_VERSION)
#error "Eth.c : Mismatch in Release Minor Version"
#endif

#if (ETH_AR_RELEASE_REVISION_VERSION != ETH_C_AR_RELEASE_REVISION_VERSION)
#error "Eth.c : Mismatch in Release Revision Version"
#endif


#if (ETH_SW_MAJOR_VERSION != ETH_C_SW_MAJOR_VERSION)
#error "Eth.c : Mismatch in Software Major Version"
#endif

#if (ETH_SW_MINOR_VERSION != ETH_C_SW_MINOR_VERSION)
#error "Eth.c : Mismatch in Software Minor Version"
#endif

/*******************************************************************************
**                                Global Data                                 **
*******************************************************************************/
extern osal_memory_manager_handle_t g_hndl_mmngr;
/* Handles for dynamic securing */
osal_memory_buffer_handle_t Eth_GucRxBuff_handle;
osal_memory_buffer_handle_t Eth_GucTxBuff_handle;

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
static FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_ControllerInitialization
(
  uint8 CtrlIdx
);

static FUNC(void, ETH_CODE_SLOW) Eth_DeviceStructInit(uint8 CtrlIdx);

static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_IntMaskSet(uint8 CtrlIdx);

static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_RxConfig(uint8 CtrlIdx);

static FUNC(void, ETH_CODE_SLOW) Eth_ModeTransitionProcess(uint8 CtrlIdx);

#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
static FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_AddToFilter
(
  uint8 LucCtrlIdx,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) pFilterAddr
);

static FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_RemoveFromFilter
(
  uint8 LucCtrlIdx,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) pFilterAddr
);
#endif

#if (ETH_QOS_SUPPORT == STD_ON)
static FUNC(Eth_TxChannelType, ETH_CODE_SLOW) Eth_FindTxQueue
(
  uint8 Priority
);
#endif

#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */

/*******************************************************************************
**                         MISRA C Rule Violations                            **
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
/* Message       : (4:0306) [I] Cast between a pointer to object and an       */
/*                 integral type.                                             */
/* Rule          : MISRA-C:2004 Rule 3.1, 11.3                                */
/*                 REFERENCE - ISO:C90-6.3.4 Cast Operators - Semantics       */
/* Justification : The casting is necessary to access descriptor and buffer   */
/*                 of ETH driver                                              */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0306)-2 and                           */
/*                 END Msg(4:0306)-2 tags in the code.                        */
/******************************************************************************/
/* 3. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0488) Performing pointer arithmetic.                    */
/* Rule          : MISRA-C:2004 Rule 17.4                                     */
/*                 REFERENCE - ISO:C90-6.3.6 Additive Operators - Constraints */
/* Justification : Increment is done on a successfully allocated  portion of  */
/*                 memory. These increments and decrements are used to easy   */
/*                 access to memory.                                          */
/*                 All these pointer operation is done/covered in condition   */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0488)-3 and                           */
/*                 END Msg(4:0488)-3 tags in the code.                        */
/******************************************************************************/
/* 4. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0489) The integer value 1 is being added or subtracted  */
/*                 from a pointer.                                            */
/* Rule          : MISRA-C:2004 Rule 17.4                                     */
/*                 REFERENCE - ISO:C90-6.3.6 Additive Operators - Constraints */
/* Justification : Increment is done on a successfully allocated portion of   */
/*                 memory. These increments and decrements are used to easy   */
/*                 access to memory.                                          */
/*                 All these pointer operation is done/covered in condition   */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0489)-4 and                           */
/*                 END Msg(4:0489)-4 tags in the code.                        */
/******************************************************************************/
/* 5. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0491) Array subscripting applied to an object of        */
/*                 pointer type.                                              */
/* Rule          : MISRA-C:2004 Rule 17.4                                     */
/*                 REFERENCE - ISO:C90-6.3.2.1 Array Subscripting             */
/*                             ISO:C90 6.2.2.1 Lvalues and Function           */
/*                                     Designators                            */
/*                             ISO:C90 6.7.1 Function Definitions - Semantics */
/* Justification : This is perfectly legitimate in C as Array and pointer are */
/*                 substantially the same thing                               */
/*                 Array subscript implementation is correct.                 */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0491)-5 and                           */
/*                 END Msg(4:0491)-5 tags in the code.                        */
/******************************************************************************/
/* 6. MISRA C RULE VIOLATION:                                                 */
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
/* Reference     : Look for START Msg(4:0492)-6 and                           */
/*                 END Msg(4:0492)-6 tags in the code.                        */
/******************************************************************************/
/* 7. MISRA C RULE VIOLATION:                                                 */
/* Message       : (2:2814) Possible: Dereference of NULL pointer.            */
/* Rule          : None                                                       */
/* Justification : The pointer cannot be NULL because:                        */
/*                 + NULL pointer has been checked before setting the address */
/*                 for pointer.                                               */
/*                 + The address which is used to set pointer is address of   */
/*                 register or global variable. All of them are not NULL      */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(2:2814)-7 and                           */
/*                 END Msg(2:2814)-7 tags in the code.                        */
/******************************************************************************/
/* 8. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:2877) This loop will never be executed more than once.  */
/* Rule          : MISRA-C:2004 Rule 21.1                                     */
/* Justification : Follow ATOSAR spec, ETH driver need to configure/check for */
/*                 all ETH controllers, however there is only 1 ETH controller*/
/*                 so it makes "for" statement is only executed 1 times.      */
/*                 This "for" statement is necessary to improve reusability   */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2877)-8 and                           */
/*                 END Msg(4:2877)-8 tags in the code.                        */
/******************************************************************************/
/* 9. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:2982) This assignment is redundant. The value of this   */
/*                 object is never used before being modified.                */
/* Rule          : MISRA-C:2004 Rule 21.1                                     */
/* Justification : Assign the initialized value for local variables, it is    */
/*                 used to avoid the case that local variable is used before  */
/*                 setting                                                    */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2982)-9 and                           */
/*                 END Msg(4:2982)-9 tags in the code.                        */
/******************************************************************************/
/* 10. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:2983) This assignment is redundant. The value of this   */
/*                 object is never subsequently used.                         */
/* Rule          : MISRA-C:2004 Rule 21.1                                     */
/* Justification : After calling Eth_OpModeChange, Eth_OpModeGet will be      */
/*                 called to check operation mode changes success or not,     */
/*                 there is no need to check the return value from            */
/*                 Eth_OpModeChange                                           */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2983)-10 and                          */
/*                 END Msg(4:2983)-10 tags in the code.                       */
/******************************************************************************/
/* 11. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:3415) Right hand operand of '&&' or '||' is an          */
/*                 expression with possible side effects.                     */
/* Rule          : MISRA-C:2004 Rule 12.4                                     */
/*                 REFERENCE - ISO:C90-5,1,2,3 Program Execution              */
/* Justification : To access a volatile object. The opertion of '&&' and '||' */
/*                 still correct when volatile object is changed              */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:3415)-11 and                          */
/*                 END Msg(4:3415)-11 tags in the code.                       */
/******************************************************************************/
/* 12. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:5087) #include statements in a file should only be      */
/*                  preceded by other preprocessor directives or comments     */
/* Rule          : MISRA-C:2004 Rule 19.1                                     */
/* Justification : include "Fls_MemMap.h" searches for defined symbol to map  */
/*                 memory section in memory mapping. This "include" should be */
/*                 added before and after the definition of memory section    */
/*                 This implementation is correct                             */
/* Verification  : Violation of this message were verified and reported       */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:5087)-12 and                          */
/*                 END Msg(4:5087)-12 tags in the code.                       */
/******************************************************************************/
/* 13. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:3112) This statement has no side-effect                 */
/*                  - it can be removed.                                      */
/* Rule          : MISRA-C:2004 Rule 14.2                                     */
/* Justification : The statement is added intendedly to avoid unused parameter*/
/*                 error message from compiler, it does not impact to         */
/*                 the functionality of ETH driver                            */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:3112)-13 and                          */
/*                 END Msg(4:3112)-13 tags in the code.                       */
/******************************************************************************/

/*******************************************************************************
**                           Function Definitions                             **
*******************************************************************************/

/*******************************************************************************
** Function Name         : Eth_GetVersionInfo
**
** Service ID            : 0x0D
**
** Description           : This API returns the version information of ETH
**                         driver component.
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Reentrant
**
** Input Parameters      : None
**
** InOut Parameters      : None
**
** Output Parameters     : VersionInfoPtr
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variables      : None
**
** Functions invoked     : Det_ReportError
**
** Registers Used        : None
**
*******************************************************************************/
#if (ETH_VERSION_INFO_API == STD_ON)
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_003] */
FUNC(void, ETH_CODE_SLOW) Eth_GetVersionInfo
(
  CONSTP2VAR(Std_VersionInfoType, AUTOMATIC, ETH_APPL_DATA)VersionInfoPtr
)
{
  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  /* Check if parameter passed is equal to Null pointer */
  if (NULL_PTR == VersionInfoPtr)
  {
    /* Report to DET */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_GET_VERSION_INFO_SID, ETH_E_PARAM_POINTER);
    /* END Msg(4:3200)-16 */
  }
  else
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
  {
    /* Cast type of macros to larger type to unify type in assignment */
    /* Copy the vendor Id */
    VersionInfoPtr->vendorID = (uint16)ETH_VENDOR_ID;
    /* Copy the module Id */
    VersionInfoPtr->moduleID = (uint16)ETH_MODULE_ID;
    /* Copy Software Major Version */
    VersionInfoPtr->sw_major_version = (uint8)ETH_SW_MAJOR_VERSION;
    /* Copy Software Minor Version */
    VersionInfoPtr->sw_minor_version = (uint8)ETH_SW_MINOR_VERSION;
    /* Copy Software Patch Version */
    VersionInfoPtr->sw_patch_version = (uint8)ETH_SW_PATCH_VERSION;
  } /* (NULL_PTR == VersionInfoPtr) */
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
#endif /* (ETH_VERSION_INFO_API == STD_ON) */


/*******************************************************************************
** Function Name         : Eth_Init
**
** Service ID            : 0x01
**
** Description           : This API performs the initialization of the Ethernet
**                         Driver by initializing the configuration structure
**                         for subsequent API calls.
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CfgPtr: Points to
**                         the implementation specific structure
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variables      : Eth_GpConfigPtr, Eth_GenControllerMode,
**                         Eth_GenMainFunctionState, Eth_GstBufferLock,
**                         Eth_GaaTempTimeStamp, Eth_GenDriverState,
**                         Eth_GaaPayloadCrc, Eth_GucTsQueueIdx, Eth_GucRxBuff,
**                         Eth_GaaTempTsDescr, Eth_GaaBaseAddr, Eth_GaaDevice,
**                         Eth_GstDescTable, Eth_GstTxQueue, Eth_GstRxBeQueue
**                         Eth_GstRxStreamQueue, Eth_GstRxNcQueue,
**                         Eth_GenAddrFilterMode, Eth_GstControllerFilterAddr
**
** Functions invoked     : Det_ReportError, Eth_ControllerAccessCheck
**                         Eth_DeviceStructInit, Eth_OpModeChange,
**                         Eth_SetRxFilter, Eth_ControllerInitialization,
**                         Dem_ReportErrorStatus, Eth_OpModeGet,
**                         Eth_DMACResetHW
**
** Registers Used        : GECMR, RFLR, ECSIPR, ECMR, CSR, CCC, APSR, EIL,
**                         CTLR, RCR, DBAT, RQCi(i = 0 to 4), RPC, TGC, EIC,
**                         RIC0, RIC1, RIC2, TIC, SFO, SFM0, SFM1, VRR, CIVR0,
**                         CIVR1, CDVR0, CDVR1, CUL0, CUL1, CLL0, CLL1,
**                         MFIERRTGTR7, MFIERRCTLR7, GIC, MAHR, MALR, CIE,
**                         MFIERRTGTR8, MFIERRCTLR8, SFPi(i = 0 to 31), FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_004] */
FUNC(void, ETH_CODE_SLOW) Eth_Init
(
  CONSTP2CONST(Eth_ConfigType, AUTOMATIC, ETH_APPL_CONST) CfgPtr
)
{
  VAR(uint8, AUTOMATIC) LucInc;
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;
  VAR(Eth_ErrorType, AUTOMATIC) LenOpModeReturnError;
  VAR(Eth_OpStatusType, AUTOMATIC) LenMode;

  LddReturnValue = E_OK;
  e_osal_return_t osal_ret = OSAL_RETURN_OK;

  osal_ret |= R_OSAL_MmngrAlloc(g_hndl_mmngr, alignment64(sizeof(uint8) * ETH_RX_BUF_SIZE_CACHE_ALIGN), OSAL_ALIGNMENT, &Eth_GucRxBuff_handle);
  osal_ret |= R_OSAL_MmngrAlloc(g_hndl_mmngr, alignment64(sizeof(uint8) * ETH_TX_BUF_SIZE_CACHE_ALIGN), OSAL_ALIGNMENT, &Eth_GucTxBuff_handle);

  osal_ret |= R_OSAL_MmngrGetHwAddr(Eth_GucRxBuff_handle, OSAL_AXI_BUS_ID_IPA_MAIN_MEMORY, (uintptr_t *)&Eth_GucRxBuff);
  osal_ret |= R_OSAL_MmngrGetHwAddr(Eth_GucTxBuff_handle, OSAL_AXI_BUS_ID_IPA_MAIN_MEMORY, (uintptr_t *)&Eth_GucTxBuff);

  if(osal_ret != OSAL_RETURN_OK)
  {
    R_PRINT_Log("Error in MmngrAlloc, MmngrGetHwAddr ret=%d", osal_ret);
    #if (ETH_DEV_ERROR_DETECT == STD_ON)
    /* Report Error to DET */
    (void)Det_ReportError(ETH_MODULE_ID,  ETH_INSTANCE_ID,ETH_INIT_SID, ETH_E_GET_MEMORY);
    #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
  }
  /* MISRA Violation: START Msg(4:2982)-9 */
  LenOpModeReturnError = ETH_ERR_NG;
  /* MISRA Violation: END Msg(4:2982)-9 */
  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  /* Report Error to DET, if the configuration pointer value is NULL */
  if (NULL_PTR == CfgPtr)
  {
    /* Report Error to DET */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_INIT_SID, ETH_E_PARAM_POINTER);
    LddReturnValue = E_NOT_OK;
  }
  /* Cast to larger type to unify type in comparison */
  else if ((uint32)ETH_DBTOC_VALUE != CfgPtr->ulStartOfDbToc)
  {
    /* Report Error to DET */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_INIT_SID, ETH_E_INVALID_DATABASE);
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_STATE_INIT == Eth_GenDriverState)
  {
    /* Report Error to DET */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_INIT_SID, ETH_E_INITIALIZED);
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No Action Required */
  }

  if (E_OK != LddReturnValue)
  {
    /* No action required */
  }
  else
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
  {
    /* Assign the configuration pointer value to global pointer */
    /* TRACE[ETH_DDD_ACT_004_GLB001] */
    Eth_GpConfigPtr = CfgPtr;

    /* Initialize MainFunction State */
    Eth_GenMainFunctionState = AVB_IDLE;
    /* Initialize payload CRC checking flag */
    #if (ETH_DEVICE_NAME == RCAR_H3)
    #if (ETH_PAYLOAD_CRC_ERROR_DETECT == STD_ON)
    /* TRACE[ETH_DDD_ACT_004_GLB004] */
    for (LucInc = ETH_ZERO_8; LucInc < ETH_RX_QUEUE_COUNT; LucInc++)
    {
      Eth_GaaPayloadCrc[LucInc] = ETH_ZERO_8;
    }
    #endif
    #endif
    for (LucInc = ETH_ZERO_8; ETH_TX_BUF_TOTAL > LucInc; LucInc++)
    {
      /* TRACE[ETH_DDD_ACT_004_GLB005] */
      Eth_GstBufferLock[LucInc].bufLock = ETH_BUF_UNLOCKED;
      /* TRACE[ETH_DDD_ACT_004_GLB006] */
      Eth_GstBufferLock[LucInc].descAddr = ETH_ZERO_32;
      /* TRACE[ETH_DDD_ACT_004_GLB007] */
      Eth_GstBufferLock[LucInc].benableTS = ETH_FALSE;
      /* TRACE[ETH_DDD_ACT_004_GLB008] */
      Eth_GstBufferLock[LucInc].bufTxConfirm = ETH_FALSE;
      Eth_GstBufferLock[LucInc].bufTxFlag = ETH_FALSE;
      /* TRACE[ETH_DDD_ACT_004_GLB017] */
      Eth_GstBufferLock[LucInc].ucChannel = ETH_TX_BE;
    }
    #if (ETH_GLOBAL_TIME_SUPPORT == STD_ON)
    /* Initialize the Queue index which the Timestamp belongs to */
    Eth_GucTsQueueIdx = ETH_BECHANNEL;
    for (LucInc = ETH_ZERO_8; LucInc < ETH_RX_QUEUE_COUNT; LucInc++)
    {
      /* Initialize the address of descriptor of a TimeStamp received frame */
      Eth_GaaTempTsDescr[LucInc] = NULL_PTR;
      /* Initialize the Timestamp just read and stored */
      Eth_GaaTempTimeStamp[LucInc].Timestamp0 = ETH_ZERO_32;
      Eth_GaaTempTimeStamp[LucInc].Timestamp1 = ETH_ZERO_32;
      Eth_GaaTempTimeStamp[LucInc].Timestamp2 = ETH_ZERO_16;
    }
    #endif

    /* MISRA Violation: START Msg(4:2877)-8 */
    for (LucInc = ETH_ZERO_8; ETH_MAX_CTRLS_SUPPORTED > LucInc; LucInc++)
    /* MISRA Violation: END Msg(4:2877)-8 */
    {
      /*Initialize the Device - BaseAddress and Status */
      Eth_DeviceStructInit(LucInc);
      #if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
      /* Initialize Physical Address Filter Mode */
      Eth_GenAddrFilterMode[LucInc] = ETH_NORMAL_MODE;
      Eth_GstControllerFilterAddr[LucInc].RxFilterLength = ETH_ZERO_8;
      #endif
      /* Check the access to Ethernet Controller */
      LddReturnValue = Eth_ControllerAccessCheck(LucInc);
      if (E_OK == LddReturnValue)
      {
        /* Ethernet Controller Access Passed */
        Dem_ReportErrorStatus(ETH_E_ACCESS, DEM_EVENT_STATUS_PREPASSED);
        /* Reset AVB-DMAC */
        LddReturnValue = Eth_DMACResetHW(LucInc);
        if (E_OK == LddReturnValue)
        {
          /* Set DMAC to Configuration Mode As per State Transition */
          LenOpModeReturnError = Eth_OpModeChange(LucInc, ETH_CONFIG);
          if (ETH_ERR_OK == LenOpModeReturnError)
          {
            /* Check the mode has been changed to CONFIG mode or not */
            LenMode = Eth_OpModeGet(LucInc);
            if (ETH_CONFIG == LenMode)
            {
              LddReturnValue = E_OK;
            }
            else
            {
              /* Error on mode transition - DEM is required */
              Dem_ReportErrorStatus(ETH_E_OPMODE, DEM_EVENT_STATUS_FAILED);
              /* Return Error - as Mode not changed */
              LddReturnValue = E_NOT_OK;
            }
          }
          else
          {
            LddReturnValue = E_NOT_OK;
          }
          if (E_OK == LddReturnValue)
          {
            /* Mode Changed Successfully */
            /*Call Eth_ControllerInitialization as private API */
            LddReturnValue = Eth_ControllerInitialization(LucInc);
            /*Set Common RX Filter */
            /*Set SFO , SFM0, SFM1 */
            Eth_SetRxFilter(LucInc);
          }
          else
          {
            /* No action required */
          }
        }
        else
        {
          /* No action required */
        }
      } /* (E_OK == LddReturnValue) */
      else
      {
        /* Ethernet Controller Access Failed */
        Dem_ReportErrorStatus(ETH_E_ACCESS, DEM_EVENT_STATUS_PREFAILED);
      }
    }
    if (E_OK == LddReturnValue)
    {
      /* Initialization Successfully */
      /* Set DriverState to Initialized */
      /* TRACE[ETH_DDD_ACT_004_GLB010] */
      Eth_GenDriverState = ETH_STATE_INIT;
      /* Set Controller State to Down */
      /* TRACE[ETH_DDD_ACT_004_GLB011] */
      Eth_GenControllerMode = ETH_MODE_DOWN;
      /* Report the extended production errors */
      /* When Ethernet Controller is successfully initialized */
      Dem_ReportErrorStatus(ETH_E_CRC, DEM_EVENT_STATUS_PREPASSED);
      Dem_ReportErrorStatus(ETH_E_UNDERSIZEFRAME, DEM_EVENT_STATUS_PREPASSED);
      Dem_ReportErrorStatus(ETH_E_OVERSIZEFRAME, DEM_EVENT_STATUS_PREPASSED);
      Dem_ReportErrorStatus(ETH_E_ALIGNMENT, DEM_EVENT_STATUS_PREPASSED);
      Dem_ReportErrorStatus(ETH_E_MULTIPLECOLLISION,DEM_EVENT_STATUS_PREPASSED);
      Dem_ReportErrorStatus(ETH_E_RX_FRAMES_LOST, DEM_EVENT_STATUS_PREPASSED);
      Dem_ReportErrorStatus(ETH_E_SINGLECOLLISION, DEM_EVENT_STATUS_PREPASSED);
      Dem_ReportErrorStatus(ETH_E_LATECOLLISION, DEM_EVENT_STATUS_PREPASSED);
    } /* (E_OK == LddReturnValue) */
    else
    {
      /* Something Wrong */
      /* State UNINIT */
      /* TRACE[ETH_DDD_ACT_004_GLB009] */
      Eth_GenDriverState = ETH_STATE_UNINIT;
      /* TRACE[ETH_DDD_ACT_004_GLB012] */
      Eth_GenControllerMode = ETH_MODE_DOWN;
    }
  }
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */

/*******************************************************************************
** Function Name         : Eth_ControllerInitialization
**
** Service ID            : NA
**
** Description           : Initializes the indexed controller using the indexed
**                         configuration
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : None
**
** Global Variables      : Eth_GstDescTable, Eth_GaaDevice, Eth_GpConfigPtr,
**                         Eth_GucRxBuff, Eth_GaaBaseAddr, Eth_GstRxStreamQueue,
**                         Eth_GstTxQueue, Eth_GstRxBeQueue, Eth_GstRxNcQueue
**
** Functions invoked     : Eth_StbeEnableEDCProtection,
**                         Eth_ConfigureDMAC, Eth_DescConfig
**                         Eth_MacInit, Eth_IntMaskSet, Eth_SetCbsParameter,
**                         Eth_RxConfig
**
** Registers Used        : MFIERRTGTR8, MFIERRCTLR8, CTLR, ECMR,
**                         MFIERRTGTR7, MFIERRCTLR7, GECMR, RFLR, ECSIPR,
**                         MAHR, MALR, RIC0, RIC1, RIC2, CIE, TIC, GIC,
**                         CIVR0, CIVR1, CDVR0, CDVR1, CUL0, CUL1, CLL0, CLL1,
**                         RQCi i = 0,1,...,4, CSR, RPC, SFPi i = 0, 1..31,
**                         CCC, APSR, EIC, RCR, TGC, DBAT, FSO_CTL, EIL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_140] */
/* TRACE[ETH_DDD_INF_115] */
static FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_ControllerInitialization
(
  uint8 CtrlIdx
)
{
  VAR(Eth_ErrorType, AUTOMATIC) LenErrorDMAC;
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  LddReturnValue = E_OK;
  #if (ETH_DEVICE_NAME == RCAR_H3)
  #if (ETH_STBE_EDC_ERROR_DETECT == STD_ON)
  Eth_StbeEnableEDCProtection();
  #endif
  #endif /* (ETH_DEVICE_NAME == RCAR_H3) */
  /* Allocate also memory for DMA */
  LenErrorDMAC = Eth_ConfigureDMAC(CtrlIdx);

  if (ETH_ERR_OK == LenErrorDMAC)
  {
    /* Configure descriptor chain Tx and Rx */
    Eth_DescConfig(CtrlIdx);
    /* Configure MAC */
    LenErrorDMAC = Eth_MacInit(CtrlIdx);
    if (ETH_ERR_OK == LenErrorDMAC)
    {
      /* Configure Interrupt */
      LenErrorDMAC = Eth_IntMaskSet(CtrlIdx);
    }
    else
    {
      /* No action required */
    }
    #if (ETH_QOS_SUPPORT == STD_ON)
    /* Set CBS Counter parameters (CIV and CDV) */
    Eth_SetCbsParameter(CtrlIdx);
    #endif
    if (ETH_ERR_OK == LenErrorDMAC)
    {
      LenErrorDMAC = Eth_RxConfig(CtrlIdx);
    }
    else
    {
      /* No action required */
    }
    if (ETH_ERR_OK != LenErrorDMAC)
    {
      LddReturnValue = E_NOT_OK;
    }
    else
    {
      /* No action required */
    }
  } /* (ETH_ERR_OK == LenErrorDMAC) */
  else
  {
    /*Failed to go in OPERATION Mode */
    LddReturnValue = E_NOT_OK;
  }

  return LddReturnValue;
}/* End of API Eth_ControllerInitialization */
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */

/*******************************************************************************
** Function Name         : Eth_SetControllerMode
**
** Service ID            : 0x03
**
** Description           : This API performs Enabling / disabling of the indexed
**                         controller
**
** Sync/Async            : Asynchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the controller within
**                                   the context of the Ethernet Driver
**                         CtrlMode - Mode to be set
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GenControllerMode,
**                         Eth_GstBufferLock, Eth_GenMainFunctionState,
**                         Eth_GpConfigPtr, Eth_GaaBaseAddr, Eth_GaaDevice
**
** Functions invoked     : Det_ReportError, Eth_DisableController,
**                         Eth_OpModeChange,
**                         Eth_EnableController, Dem_ReportErrorStatus,
**                         EthIf_CtrlModeIndication, Eth_OpModeGet
**                         Eth_PayloadCRCAreaStartSet,
**                         Eth_ControllerAccessCheck
**
** Registers Used        : GECMR, ECMR, CSR, CCC, VRR, PCRC, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_008] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_SetControllerMode
(
  uint8 CtrlIdx,
  Eth_ModeType CtrlMode
)
{
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;
  VAR(Eth_ErrorType, AUTOMATIC) LenOpModeReturnError;
  VAR(Eth_OpStatusType, AUTOMATIC) LenMode;

  LddReturnValue = E_OK;
  /* MISRA Violation: START Msg(4:2982)-9 */
  LenOpModeReturnError = ETH_ERR_NG;
  /* MISRA Violation: END Msg(4:2982)-9 */

  /* Check if the module is initialized */
  if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    #if (ETH_DEV_ERROR_DETECT == STD_ON)
    /* Report error to DET that module is not initialized */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_SETCONTROLLERMODE_SID, ETH_E_NOT_INITIALIZED);
    #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    #if (ETH_DEV_ERROR_DETECT == STD_ON)
    /* Report error to DET if the CtrlIdx is Invalid*/
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_SETCONTROLLERMODE_SID, ETH_E_INV_CTRL_IDX);
    #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No action required */
  }

  if (AVB_IDLE != Eth_GenMainFunctionState)
  {
    /* Mode transition is running */
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* There is no mode transition running */
  }
  if (E_OK == LddReturnValue)
  {
    LddReturnValue = Eth_ControllerAccessCheck(CtrlIdx);
    if (E_OK == LddReturnValue)
    {
      /* Ethernet Controller Access Passed */
      Dem_ReportErrorStatus(ETH_E_ACCESS, DEM_EVENT_STATUS_PREPASSED);
    }
    else
    {
      /* Ethernet Controller Access Failed */
      Dem_ReportErrorStatus(ETH_E_ACCESS, DEM_EVENT_STATUS_PREFAILED);
    }
  } /* (E_OK == LddReturnValue) */
  else
  {
    /* No action required */
  }
  if (E_OK == LddReturnValue)
  {
    if (Eth_GenControllerMode != CtrlMode)
    {
      if (ETH_MODE_DOWN == CtrlMode)
      {
        /* Disable the DMAC and MAC of Controller for pending Tx or Rx */
        Eth_DisableController(CtrlIdx);

        /* Change mode to Configuration */
        LenOpModeReturnError = Eth_OpModeChange(CtrlIdx, ETH_CONFIG);

        if (ETH_ERR_OK == LenOpModeReturnError)
        {
          /*Reset all Buffer Locks or pending Transmit*/
          for (LucCount = ETH_ZERO_8; ETH_TX_BUF_TOTAL > LucCount; LucCount++)
          {
            /* TRACE[ETH_DDD_ACT_008_GLB003] */
            Eth_GstBufferLock[LucCount].bufLock   = ETH_BUF_UNLOCKED;
            /* Clear Tx confirmation request */
            Eth_GstBufferLock[LucCount].bufTxConfirm = ETH_FALSE;
            /* Clear transmitting flag */
            Eth_GstBufferLock[LucCount].bufTxFlag = ETH_FALSE;
            /* TRACE[ETH_DDD_ACT_008_GLB004] */
            Eth_GstBufferLock[LucCount].descAddr   = ETH_ZERO_32;
            /* TRACE[ETH_DDD_ACT_008_GLB005] */
            Eth_GstBufferLock[LucCount].benableTS = ETH_FALSE;
            /* TRACE[ETH_DDD_ACT_008_GLB006] */
            Eth_GstBufferLock[LucCount].ucChannel = ETH_TX_BE;
          }
          /* Trigger MainFunction */
          Eth_GenMainFunctionState = AVB_CONFIG;
        }
        else
        {
          LddReturnValue = E_NOT_OK;
        }
      } /* (ETH_MODE_DOWN == CtrlMode) */
      /* Controller -> Active */
      else
      {
        /* Enable the DMAC and MAC of Controller */
        Eth_EnableController(CtrlIdx);

        /* set AVB to operation mode */
        /* MISRA Violation: START Msg(4:2983)-10 */
        LenOpModeReturnError = Eth_OpModeChange(CtrlIdx, ETH_OPERATION);
        /* MISRA Violation: END Msg(4:2983)-10 */
        LenMode = Eth_OpModeGet(CtrlIdx);
        if (ETH_OPERATION == LenMode)
        {
          #if (ETH_DEVICE_NAME == RCAR_H3)
          #if (ETH_PAYLOAD_CRC_ERROR_DETECT == STD_ON)
          /*
          * Set the number of the first data byte in received frame to check
          *  a potential payload CRC
          */
          Eth_PayloadCRCAreaStartSet(CtrlIdx,
            Eth_GpConfigPtr->ucStartOfPayloadCrcArea);
          #endif
          #endif
          /* Notify EthIf with Mode ACTIVE */
          EthIf_CtrlModeIndication(CtrlIdx, ETH_MODE_ACTIVE);
          Eth_GenControllerMode = ETH_MODE_ACTIVE;
          LddReturnValue = E_OK;
        }
        else
        {
          /* Error on mode transition - DEM is required */
          Dem_ReportErrorStatus(ETH_E_OPMODE, DEM_EVENT_STATUS_FAILED);
          /* Return Error - as Mode not changed */
          LddReturnValue = E_NOT_OK;
        }
      } /* (ETH_MODE_DOWN == CtrlMode) */
    } /* (Eth_GenControllerMode != CtrlMode) */
    /* Current_mode = new_mode */
    else
    {
      /* Directly call EthIf_CtrlModeIndication */
      if (ETH_MODE_DOWN == CtrlMode)
      {
        /* Notify DOWN Mode   */
        EthIf_CtrlModeIndication(CtrlIdx, ETH_MODE_DOWN);
      }
      else
      {
        /* Notify ACTIVE Mode */
        EthIf_CtrlModeIndication(CtrlIdx, ETH_MODE_ACTIVE);
      }
    } /* (Eth_GenControllerMode != CtrlMode) */
  }
  else
  {
    /* No action required */
  }

  return LddReturnValue;
} /* End of API Eth_SetControllerMode */
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */

/*******************************************************************************
** Function Name         : Eth_GetControllerMode
**
** Service ID            : 0x04
**
** Description           : This API obtains the state of the indexed controller
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : CtrlModePtr - Pointer to store the Mode being get
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GenControllerMode
**
** Functions invoked     : Det_ReportError
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_001] */
FUNC(Std_ReturnType, ETH_CODE_SLOW)Eth_GetControllerMode
(
  uint8 CtrlIdx,
  CONSTP2VAR(Eth_ModeType, AUTOMATIC, ETH_APPL_DATA) CtrlModePtr
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  LddReturnValue = E_OK;

  /* Check if the module is initialized */
  if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    #if (ETH_DEV_ERROR_DETECT == STD_ON)
    /* Report error to DET that module is not initialized */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_GETCONTROLLERMODE_SID, ETH_E_NOT_INITIALIZED);
    #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
    LddReturnValue = E_NOT_OK;
  }
  /* Check if the CtrlIdx is Valid */
  else if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    #if (ETH_DEV_ERROR_DETECT == STD_ON)
    /* Report error to DET if the CtrlIdx is Invalid */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_GETCONTROLLERMODE_SID, ETH_E_INV_CTRL_IDX);
    #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
    LddReturnValue = E_NOT_OK;
  }
  /* Report Error to DET, if the CtrlModePtr pointer value is NULL */
  else if (NULL_PTR == CtrlModePtr)
  {
    #if (ETH_DEV_ERROR_DETECT == STD_ON)
    /* Report Error to DET */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_GETCONTROLLERMODE_SID, ETH_E_PARAM_POINTER);
    #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No action required */
  }

  if (E_OK != LddReturnValue)
  {
    /* No action required */
  }
  else
  {
    /* Get Operating Mode Status */
    *CtrlModePtr = (Eth_ModeType)Eth_GenControllerMode;
  }

  return LddReturnValue;
} /* End of API Eth_GetControllerMode */
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */

/*******************************************************************************
** Function Name         : Eth_GetPhysAddr
**
** Service ID            : 0x08
**
** Description           : This API obtains the physical source address (MAC
**                         Address) configured for the indexed controller.
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : PhysAddrPtr - Pointer to store the physical source
**                                       address (MAC Address)
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GaaBaseAddr
**
** Functions invoked     : Det_ReportError, Eth_GetMacAddr
**
** Registers Used        : MAHR, MALR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_002] */
FUNC(void, ETH_CODE_SLOW) Eth_GetPhysAddr
(
  uint8 CtrlIdx,
  CONSTP2VAR(uint8, AUTOMATIC, ETH_APPL_DATA) PhysAddrPtr
)
{
  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  LddReturnValue = E_OK;

  /* Check if the module is initialized */
  if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    /* Report error to DET that module is not initialized */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_GETPHYSADDR_SID, ETH_E_NOT_INITIALIZED);
    LddReturnValue = E_NOT_OK;
  }
  /* Check for the valid CtrlIdx */
  else if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    /* Report error to DET if the TrcvIdx is Invalid */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_GETPHYSADDR_SID, ETH_E_INV_CTRL_IDX);
    LddReturnValue = E_NOT_OK;
  }
  /* Report Error to DET, if the PhysAddrPtr pointer value is NULL */
  else if (NULL_PTR == PhysAddrPtr)
  {
    /* Report Error to DET */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_GETPHYSADDR_SID, ETH_E_PARAM_POINTER);
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No action required */
  }

  if (E_OK != LddReturnValue)
  {
    /* No action required */
  }
  else
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
  {
    Eth_GetMacAddr(CtrlIdx, PhysAddrPtr);
  }
} /* End of API Eth_GetPhysAddr */
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */

/*******************************************************************************
** Function Name         : Eth_WriteMii
**
** Service ID            : 0x05
**
** Description           : This API configures a transceiver register or
**                         triggers a function offered by the receiver by
**                         writing the specified transceiver register through
**                         the MII of the indexed controller
**
** Sync/Async            : Asynchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**                         TrcvIdx - Index of the transceiver on the MII
**                         RegIdx - Index of the transceiver register on the MII
**                         RegVal - Value to be written into the register
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LddReturnValue - E_OK: Service accepted
**                                        - E_NOT_OK: Service denied
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GenControllerMode,
**                         Eth_GaaBaseAddr
**
** Functions invoked     : Det_ReportError, Eth_MIIFrameHeaderSet,
**                         Eth_PhyWrite1, Eth_PhyWrite0, Eth_PhyWrite,
**                         Eth_PhyIndependentBusRelease,
**                         EthTrcv_WriteMiiIndication
**
** Registers Used        : PIR
**
*******************************************************************************/
#if (ETH_CTRL_ENABLE_MII == STD_ON)
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_011] */
FUNC(Std_ReturnType, ETH_CODE_SLOW)Eth_WriteMii
(
  uint8 CtrlIdx,
  uint8 TrcvIdx,
  uint8 RegIdx,
  uint16 RegVal
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  LddReturnValue = E_OK;

  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  /* Check if the module is initialized */
  if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    /* Report error to DET if the component is not initialized */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_WRITEMII_SID, ETH_E_NOT_INITIALIZED);
    LddReturnValue = E_NOT_OK;
  }
  /* Check for the valid CtrlIdx */
  else if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    /* Report error to DET if the CtrlIdx is Invalid */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_WRITEMII_SID, ETH_E_INV_CTRL_IDX);
    LddReturnValue = E_NOT_OK;
  }
  /* Check for the valid TrcvIdx */
  else if (ETH_TRCV_MAX_INDEX < TrcvIdx)
  {
    /* Report error to DET if the TrcvIdx is Invalid */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_WRITEMII_SID, ETH_E_INV_PARAM );
    LddReturnValue = E_NOT_OK;
  }
  /* Check for the valid RegIdx */
  else if (ETH_MII_REG_MAX_INDEX < RegIdx)
  {
    /* Report error to DET if the RegIdx is Invalid*/
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_WRITEMII_SID, ETH_E_INV_PARAM );
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No action required */
  } /* (ETH_MII_REG_MAX_INDEX < RegIdx) */

  if (E_OK != LddReturnValue)
  {
    /* No action required */
  }
  else
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
  {
    /* frame header */
    LddReturnValue = Eth_MIIFrameHeaderSet(CtrlIdx, TrcvIdx, RegIdx, ETH_ONE_8);
    if (E_OK == LddReturnValue)
    {
      /* TA */
      LddReturnValue = Eth_PhyWrite1(CtrlIdx);
    }
    else
    {
      /* No action required */
    }
    if (E_OK == LddReturnValue)
    {
      LddReturnValue = Eth_PhyWrite0(CtrlIdx);
    }
    else
    {
      /* No action required */
    }
    if (E_OK == LddReturnValue)
    {
      /* Write data to PHY chip */
      LddReturnValue = Eth_PhyWrite(CtrlIdx, RegVal);
    }
    else
    {
      /* No action required */
    }
    if (E_OK == LddReturnValue)
    {
      /* Independent bus release */
      Eth_PhyIndependentBusRelease(CtrlIdx);
      /* Notify Ethernet Transceiver */
      EthTrcv_WriteMiiIndication(CtrlIdx, TrcvIdx, RegIdx);
    }
    else
    {
      /* No action required */
    }
  } /* (E_OK != LddReturnValue) */

  return LddReturnValue;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */

/*******************************************************************************
** Function Name         : Eth_ReadMii
**
** Service ID            : 0x06
**
** Description           : This API reads the specified transceiver register
**                         through the MII of the indexed controller.
**
** Sync/Async            : Asynchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**                         TrcvIdx - Index of the transceiver on the MII
**                         RegIdx - Index of the transceiver register on the MII
**
** InOut Parameters      : None
**
** Output Parameters     : RegValPtr - Pointer to store register content
**                                     of the indexed register
**
** Return parameter      : LddReturnValue - E_OK: Service accepted
**                                        - E_NOT_OK: Service denied
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GenControllerMode,
**                         Eth_GaaBaseAddr
**
** Functions invoked     : Det_ReportError, Eth_MIIFrameHeaderSet,
**                         Eth_PhyBusRelease, Eth_PhyRead,
**                         EthTrcv_ReadMiiIndication
**
** Registers Used        : PIR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_006] */
FUNC(Std_ReturnType, ETH_CODE_SLOW)Eth_ReadMii
(
  uint8 CtrlIdx,
  uint8 TrcvIdx,
  uint8 RegIdx,
  CONSTP2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) RegValPtr
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  LddReturnValue = E_OK;

  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  /* Check if the module is initialized */
  if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    /* Report error to DET if the component is not initialized */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_READMII_SID, ETH_E_NOT_INITIALIZED);
    LddReturnValue = E_NOT_OK;
  }
  /* Check for the valid CtrlIdx */
  else if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    /* Report error to DET if the CtrlIdx is Invalid */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_READMII_SID, ETH_E_INV_CTRL_IDX);
    LddReturnValue = E_NOT_OK;
  }
  /* Check for the valid TrcvIdx */
  else if (ETH_TRCV_MAX_INDEX < TrcvIdx)
  {
    /* Report error to DET if the TrcvIdx is Invalid */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_READMII_SID, ETH_E_INV_PARAM );
    LddReturnValue = E_NOT_OK;
  }
  /* Check for the valid RegIdx */
  else if (ETH_MII_REG_MAX_INDEX < RegIdx)
  {
    /* Report error to DET if the RegIdx is Invalid*/
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_READMII_SID, ETH_E_INV_PARAM);
    LddReturnValue = E_NOT_OK;
  }
  else if (NULL_PTR == RegValPtr)
  {
    /* Report error to DET if the RegValPtr is Invalid */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_READMII_SID, ETH_E_PARAM_POINTER);
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No action required */
  }/* (NULL_PTR == RegValPtr) */

  if (E_OK != LddReturnValue)
  {
    /* No action required */
  }
  else
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
  {
    /* frame header */
    LddReturnValue = Eth_MIIFrameHeaderSet(CtrlIdx, TrcvIdx, RegIdx,
      ETH_ZERO_8);
    if (E_OK == LddReturnValue)
    {
      /* TA */
      LddReturnValue = Eth_PhyBusRelease(CtrlIdx);
    }
    else
    {
      /* No action required */
    }
    /* Read data from PHY chip */
    if (E_OK == LddReturnValue)
    {
      LddReturnValue = Eth_PhyRead(CtrlIdx, RegValPtr);
    }
    else
    {
      /* No action required */
    }
    if (E_OK == LddReturnValue )
    {
      /* Notify Ethernet Transceiver */
      EthTrcv_ReadMiiIndication(CtrlIdx, TrcvIdx, RegIdx, *RegValPtr);
    }
    else
    {
      /* No action required */
    }
  } /* (E_OK != LddReturnValue) */
  return LddReturnValue;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
#endif /* (ETH_CTRL_ENABLE_MII == STD_ON) */

/*******************************************************************************
** Function Name         : Eth_ProvideTxBuffer
**
** Service ID            : 0x09
**
** Description           : This API provides access to a transmit buffer
**                         of the specified controller
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : LenBytePtr - In:  desired length in bytes
**                                      Out: granted length in bytes
**
** Output Parameters     : BufIdxPtr - Pointer to store the Index to the granted
**                                     buffer resource used for subsequent calls
**                         BufPtr    - Pointer to the granted buffer
**
** Return parameter      : BufReq_ReturnType
**                          - BUFREQ_OK: success
**                          - BUFREQ_E_NOT_OK: development error detected
**                          - BUFREQ_E_BUSY: all buffers in use
**                          - BUFREQ_E_OVFL: requested buffer too large
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GstBufferLock,
**                         Eth_GpConfigPtr
**
** Functions invoked     : Det_ReportError, Eth_FindTxQueue, Eth_GetTxBuffAddr
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_154] */
#if (ETH_QOS_SUPPORT == STD_ON)
FUNC(BufReq_ReturnType, ETH_CODE_SLOW) Eth_ProvideTxBuffer(
  uint8 CtrlIdx,
  uint8 Priority,
  CONSTP2VAR(Eth_BufIdxType, AUTOMATIC, ETH_APPL_DATA) BufIdxPtr,
  CONSTP2VAR(P2VAR(uint8, AUTOMATIC, ETH_APPL_DATA),
    AUTOMATIC, ETH_APPL_DATA) BufPtr,
  CONSTP2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) LenBytePtr)
#else
/* TRACE[ETH_DDD_ACT_005] */
FUNC(BufReq_ReturnType, ETH_CODE_SLOW) Eth_ProvideTxBuffer(
  uint8 CtrlIdx,
  CONSTP2VAR(Eth_BufIdxType, AUTOMATIC, ETH_APPL_DATA) BufIdxPtr,
  CONSTP2VAR(P2VAR(uint8, AUTOMATIC, ETH_APPL_DATA),
    AUTOMATIC, ETH_APPL_DATA) BufPtr,
  CONSTP2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) LenBytePtr)
#endif
{
  /* Local variable to hold the DET return value */
  VAR(BufReq_ReturnType, AUTOMATIC) LddReturnValue;
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(Eth_TxChannelType, AUTOMATIC) LenQIdx;
  VAR(uint32, AUTOMATIC) LulbufAddr;

  #if (ETH_DEV_ERROR_DETECT != STD_ON)
  /* Suppress unused parameter warnings for the parameters below */
  /* MISRA Violation: START Msg(4:3112)-13 */
  (void) CtrlIdx;
  /* MISRA Violation: END Msg(4:3112)-13 */
  #endif /* (ETH_DEV_ERROR_DETECT != STD_ON) */

  LddReturnValue = BUFREQ_OK;

  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  /* Check if the module is initialized */
  if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    /* Report error to DET that module is not initialized */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_PROVIDETXBUFFER_SID, ETH_E_NOT_INITIALIZED);
    /* Set Return Value as BUFREQ_E_NOT_OK */
    LddReturnValue = BUFREQ_E_NOT_OK;
  }
  /* Check if the CtrlIdx is Valid */
  else if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    /* Report error to DET if the CtrlIdx is Invalid*/
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_PROVIDETXBUFFER_SID, ETH_E_INV_CTRL_IDX);
    /* Set Return Value as BUFREQ_E_NOT_OK */
    LddReturnValue = BUFREQ_E_NOT_OK;
  }
  /* Report Error to DET, if the BufIdxPtr pointer value is NULL */
  else if (NULL_PTR == BufIdxPtr)
  {
    /* Report Error to DET */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_PROVIDETXBUFFER_SID, ETH_E_PARAM_POINTER);
    /* Set Return Value as BUFREQ_E_NOT_OK */
    LddReturnValue = BUFREQ_E_NOT_OK;
  }
  /* Report Error to DET, if the BufPtr pointer value is NULL */
  else if (NULL_PTR == BufPtr)
  {
    /* Report Error to DET */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_PROVIDETXBUFFER_SID, ETH_E_PARAM_POINTER);
    /* Set Return Value as BUFREQ_E_NOT_OK */
    LddReturnValue = BUFREQ_E_NOT_OK;
  }
  /* Report Error to DET, if the LenBytePtr pointer value is NULL */
  else if (NULL_PTR == LenBytePtr)
  {
    /* Report Error to DET */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_PROVIDETXBUFFER_SID, ETH_E_PARAM_POINTER);
    /* Set Return Value as BUFREQ_E_NOT_OK */
    LddReturnValue = BUFREQ_E_NOT_OK;
  }
  else
  {
    /* No action required */
  }
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
  if (BUFREQ_E_NOT_OK != LddReturnValue)
  {
    /* Cast to larger type to unify type in comparison */
    if (((uint16)ETH_CTRL_TX_BUF_LEN_BYTE - ETH_HEADER_SIZE) < *LenBytePtr)
    {
      /* Cast to larger type to unify type in calculation */
      *LenBytePtr = (uint16)ETH_CTRL_TX_BUF_LEN_BYTE - ETH_HEADER_SIZE;
      /* Set Return Value as BUFREQ_E_OVFL */
      LddReturnValue = BUFREQ_E_OVFL;
    }
    else
    {
      /* No action required */
    }
  }
  else
  {
    /* No action required */
  }

  if ((BUFREQ_E_NOT_OK != LddReturnValue) && (BUFREQ_E_OVFL != LddReturnValue))
  {
    if ((ETH_MIN_BUFFER_SIZE - ETH_HEADER_SIZE - ETH_FCS_LENGTH) >= *LenBytePtr)
    {
      *LenBytePtr = ETH_MIN_BUFFER_SIZE - ETH_HEADER_SIZE - ETH_FCS_LENGTH;
    }
    else
    {
      /*Do Nothing*/
    }

    #if (ETH_QOS_SUPPORT == STD_ON)
    LenQIdx = Eth_FindTxQueue(Priority);
    #else
    /* Use BE Queue for any Traffic */
    LenQIdx = ETH_TX_BE;
    #endif

    for (LucCount = ETH_ZERO_8; ETH_TX_BUF_TOTAL > LucCount; LucCount++)
    {
      if (ETH_BUF_LOCKED != Eth_GstBufferLock[LucCount].bufLock)
      {
        /* Cast to larger type to unify type in assignment */
        *BufIdxPtr = (Eth_BufIdxType)LucCount;

        /* Assign the buffer to a specific Queue */
        /* TRACE[ETH_DDD_ACT_005_GLB001] */
        /* TRACE[ETH_DDD_ACT_154_GLB001] */
        Eth_GstBufferLock[LucCount].ucChannel = LenQIdx;
        /* TRACE[ETH_DDD_ACT_005_GLB002] */
        /* TRACE[ETH_DDD_ACT_154_GLB002] */
        Eth_GstBufferLock[LucCount].bufLock = ETH_BUF_LOCKED;
        /*
         *  Cast from uint8 to uint32 to be compatible with type of
         *  input parameter
         */
        LulbufAddr = Eth_GetTxBuffAddr((uint32)LucCount);
        /* Cast to larger type to unify type in calculation */
        /* MISRA Violation: START Msg(4:0306)-2 */
        *BufPtr = (uint8 *)(LulbufAddr + (uint32)ETH_HEADER_SIZE);
        /* MISRA Violation: END Msg(4:0306)-2 */
        LddReturnValue = BUFREQ_OK;
        break;
      } /* (ETH_BUF_LOCKED != Eth_GstBufferLock[LucCount].bufLock) */
      else if ((ETH_TX_BUF_TOTAL - ETH_ONE_8) == LucCount)
      {
        LddReturnValue = BUFREQ_E_BUSY;
      }
      else
      {
        /* Set Return Value as BUFREQ_E_NOT_OK */
        LddReturnValue = BUFREQ_E_NOT_OK;
      }
    }
  } /*
    * ((BUFREQ_E_NOT_OK != LddReturnValue) &&
    * (BUFREQ_E_OVFL != LddReturnValue))
    */
  else
  {
    /* Do Nothing */
  } /* (BUFREQ_E_NOT_OK != LddReturnValue)&&(BUFREQ_E_OVFL != LddReturnValue) */

  return LddReturnValue;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */

/*******************************************************************************
** Function Name         : Eth_Transmit
**
** Service ID            : 0xA
**
** Description           : This API triggers transmission of a previously
**                         filled transmit buffer
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx     - Index of the ETH Controller
**                         BufIdx      - Buffer index
**                         FrameType   - Ethernet frame type
**                         TxConfirmation - Activates transmission confirmation
**                         LenByte     - Data length in byte
**                         PhysAddrPtr - Physical target address (MAC address)
**                                       in network byte order
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : Controller initialized and Buffer is provided using
**                         Eth_ProvideTxBuffer
**
** Global Variables      : Eth_GenDriverState, Eth_GstBufferLock
**                         Eth_GenControllerMode, Eth_GaaDevice,
**                         Eth_GucTxBuff, Eth_GaaBaseAddr
**
** Functions invoked     : Det_ReportError, Eth_GetTxBuffAddr,
**                         Eth_SingleDescFrameSend,
**                         CR7_Flush_DCache_By_Addr
**
** Registers Used        : CDARq q = 0,1...,21, TCCR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_009] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_Transmit
(
  uint8 CtrlIdx,
  Eth_BufIdxType BufIdx,
  VAR(Eth_FrameType, AUTOMATIC) FrameType,
  boolean TxConfirmation,
  uint16 LenByte,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) PhysAddrPtr
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;
  P2VAR(volatile Eth_DataType, AUTOMATIC, ETH_APPL_DATA) LpBufPtr;
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(Eth_TxSingleFrameType, AUTOMATIC) LstFrame;
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;
  VAR(uint32, AUTOMATIC) LulbufAddr;

  LddReturnValue = E_OK;
  /* MISRA Violation: START Msg(4:2982)-9 */
  LenReturnError = ETH_ERR_OK;
  /* MISRA Violation: END Msg(4:2982)-9 */
  e_osal_return_t osal_ret = OSAL_RETURN_OK;

  if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    #if (ETH_DEV_ERROR_DETECT == STD_ON)
    /* Report error to DET that module is not initialized */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_TRANSMIT_SID, ETH_E_NOT_INITIALIZED);
    #endif
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No action */
  }
  if (E_OK == LddReturnValue)
  {
    /* Check if the CtrlIdx is Valid */
    if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
    {
      #if (ETH_DEV_ERROR_DETECT == STD_ON)
      /* Report error to DET if the CtrlIdx is Invalid*/
      (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
        ETH_TRANSMIT_SID, ETH_E_INV_CTRL_IDX);
      #endif
      LddReturnValue = E_NOT_OK;
    }
    else
    {
      /* No action */
    }
  }
  else
  {
    /* No action */
  }
  if (E_OK == LddReturnValue)
  {
    /* Check if the BufIdx is Valid */
    /* Cast type of macro to larger type to unify type in assignment */
    if ((Eth_BufIdxType)ETH_TX_BUF_TOTAL <= BufIdx)
    {
      #if (ETH_DEV_ERROR_DETECT == STD_ON)
      /* Report error to DET if the BufIdx is Invalid*/
      (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
        ETH_TRANSMIT_SID, ETH_E_INV_PARAM);
      #endif
      LddReturnValue = E_NOT_OK;
    }
    else
    {
      /* No action */
    }
  }
  else
  {
    /* No action */
  }
  if (E_OK == LddReturnValue)
  {
    /* Report Error to DET, if the PhysAddrPtr pointer value is NULL */
    if (NULL_PTR == PhysAddrPtr)
    {
      #if (ETH_DEV_ERROR_DETECT == STD_ON)
      /* Report Error to DET */
      (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
        ETH_TRANSMIT_SID, ETH_E_PARAM_POINTER);
      #endif
      LddReturnValue = E_NOT_OK;
    }
    else
    {
      /* No action */
    }
  }
  else
  {
    /* No action */
  }
  if (E_OK == LddReturnValue)
  {
    /* Report Error to DET, if the controller mode is not Active */
    if (ETH_MODE_ACTIVE != Eth_GenControllerMode)
    {
      #if (ETH_DEV_ERROR_DETECT == STD_ON)
      /* Report Error to DET */
      (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
        ETH_TRANSMIT_SID, ETH_E_INV_MODE);
      #endif
      LddReturnValue = E_NOT_OK;
    }
    else
    {
      /* No action */
    }
  }
  else
  {
    /* No action */
  }
  if ((E_OK == LddReturnValue) &&
    (ETH_BUF_LOCKED == Eth_GstBufferLock[BufIdx].bufLock))
  {
    LulbufAddr = Eth_GetTxBuffAddr(BufIdx);

#if defined(USE_ETHER_CA_ENABLE) /* Use CA only */
    /* Invalidate buffer data */
    osal_ret = R_OSAL_MmngrInvalidate(Eth_GucTxBuff_handle, (uintptr_t)LulbufAddr - (uintptr_t)Eth_GucTxBuff, ETH_CTRL_TX_BUF_LEN_CACHE_ALIGN);
    if (OSAL_RETURN_OK != osal_ret)
    {
      R_PRINT_Log("Error in R_OSAL_MmngrInvalidate(Eth_GucTxBuff) ret=%d", osal_ret);
    }
#endif

    /* MISRA Violation: START Msg(4:0303)-1 */
    LpBufPtr = (volatile Eth_DataType *)LulbufAddr;
    /* MISRA Violation: END Msg(4:0303)-1 */
    for (LucCount = ETH_ZERO_8; ETH_MACADDR_SIZE > LucCount; LucCount++)
    {
      /* MISRA Violation: START Msg(4:0492)-6 */
      /* MISRA Violation: START Msg(2:2814)-7 */
      *LpBufPtr = PhysAddrPtr[LucCount];
      /* MISRA Violation: END Msg(2:2814)-7 */
      /* MISRA Violation: END Msg(4:0492)-6 */
      /* MISRA Violation: START Msg(4:0489)-4 */
      LpBufPtr++;
      /* MISRA Violation: END Msg(4:0489)-4 */
    }

    for (LucCount = ETH_ZERO; ETH_MACADDR_SIZE > LucCount; LucCount++)
    {
      *LpBufPtr = Eth_GaaDevice[CtrlIdx].MacAddr[LucCount];
      /* MISRA Violation: START Msg(4:0489)-4 */
      LpBufPtr++;
      /* MISRA Violation: END Msg(4:0489)-4 */
    }
    /* Cast from uint16 to Eth_DataType (uint8) to get higher bits */
    *LpBufPtr = (Eth_DataType)((FrameType & ETH_HIGH_BYTE_16) >> ETH_EIGHT);

    /* MISRA Violation: START Msg(4:0489)-4 */
    LpBufPtr++;
    /* MISRA Violation: END Msg(4:0489)-4 */

    /* Cast from uint16 to Eth_DataType (uint8) to get lower bits */
    *LpBufPtr = (Eth_DataType)(FrameType & ETH_LOW_BYTE_16);

    /* Flush buffer data */
    Eth_CacheFlush(LulbufAddr, ETH_CTRL_TX_BUF_LEN_CACHE_ALIGN);

    // osal_ret = R_OSAL_MmngrFlush(Eth_GucTxBuff_handle, (uintptr_t)LulbufAddr - (uintptr_t)Eth_GucTxBuff, ETH_CTRL_TX_BUF_LEN_CACHE_ALIGN);
    // if (OSAL_RETURN_OK != osal_ret)
    // {
    //   R_PRINT_Log("Error in R_OSAL_MmngrFlush(Eth_GucTxBuff) ret=%d", osal_ret);
    // }

    /* Cast from boolean type to uint8 for storage and calculation */
    LstFrame.Timestamp = (uint8)Eth_GstBufferLock[BufIdx].benableTS;
    LstFrame.MacStatus = ETH_DISABLE;

    /*
     * Cast to be compatible with HW support
     * Range of BufIdx is [0, 254], no risk for casting
     */
    LstFrame.Tag = (uint16)BufIdx;

    LstFrame.Address = LulbufAddr;
    /* Include the header before transmitting frame  */
    LstFrame.Length = LenByte + ETH_HEADER_SIZE;

    LenReturnError = Eth_SingleDescFrameSend(CtrlIdx, TxConfirmation, BufIdx,
      &LstFrame);

    if (ETH_ERR_OK == LenReturnError)
    {
       /* No action required */
    }
    else
    {
      /* There is error, the transmission failed */
      LddReturnValue = E_NOT_OK;
    }
  } /*
    * ((E_OK == LddReturnValue) &&
    * (ETH_BUF_LOCKED == Eth_GstBufferLock[BufIdx].bufLock))
    */
  else
  {
    /* There is DET error or the buffer is not locked */
    LddReturnValue = E_NOT_OK;
  } /*
    * ((E_OK == LddReturnValue) &&
    * (ETH_BUF_LOCKED == Eth_GstBufferLock[BufIdx].bufLock))
    */
  if (E_OK == LddReturnValue)
  {
    /* No action required */
  }
  /* Cast to larger type to unify type in comparison */
  else if (BufIdx < (Eth_BufIdxType)ETH_TX_BUF_TOTAL)
  {
    /* Release buffer if transmission failed to avoid buffer overflow */
    /* TRACE[ETH_DDD_ACT_009_GLB004] */
    /* START Msg(7:3689)-7 */
    Eth_GstBufferLock[BufIdx].bufLock = ETH_BUF_UNLOCKED;
    /* Clear Tx confirmation request */
    Eth_GstBufferLock[BufIdx].bufTxConfirm = ETH_FALSE;
    /* Clear transmitting flag */
    Eth_GstBufferLock[BufIdx].bufTxFlag = ETH_FALSE;
    /* TRACE[ETH_DDD_ACT_009_GLB005] */
    Eth_GstBufferLock[BufIdx].descAddr = ETH_ZERO_32;
    Eth_GstBufferLock[BufIdx].benableTS = ETH_FALSE;
    /* END Msg(7:3689)-7 */
  }
  else
  {
    /* No action required */
  }

  return LddReturnValue;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */

#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_OFF)
/*******************************************************************************
** Function Name         : Eth_Receive
**
** Service ID            : 0xB
**
** Description           : This API triggers frame reception
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : RxStatusPtr - Indicates whether a frame has been
**                                       received and if so, whether more frames
**                                       are available or frames got lost.
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GenControllerMode,
**                         Eth_GpConfigPtr, Eth_GaaDevice, Eth_GstDescTable,
**                         Eth_GaaTempTsDescr, Eth_GaaBaseAddr,
**                         Eth_GaaTempTimeStamp,Eth_GucRxMultiBuffFrame,
**                         Eth_GaaPayloadCrc
**
** Functions invoked     : Det_ReportError, Eth_UnreadDataCheck
**                         Eth_RxQueueProcess, Eth_RxFilterProcess
**                         Eth_RxCallEthIf, Eth_UFCounterGet
**                         Eth_UFCounterDecrement, Eth_RxDescChainUpdate
**                         Dem_ReportErrorStatus
**
** Registers Used        : CDARq(q = 0 to 21), UFCD, FSO_CTL,
**                         UFCVi i = 0,1,...,4
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_007] */
FUNC(void, ETH_CODE_SLOW) Eth_Receive
(
  uint8 CtrlIdx,
  CONSTP2VAR(Eth_RxStatusType, AUTOMATIC, ETH_APPL_DATA) RxStatusPtr
)
{
  VAR(uint8, AUTOMATIC) LucQIdx;
  VAR(uint8, AUTOMATIC) LucRxFrameCount;
  VAR(Eth_RxSingleFrameType, AUTOMATIC) LstRxFrame;
  VAR(Std_ReturnType, AUTOMATIC) LddFilterResult;
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;
  VAR(uint32, AUTOMATIC) LulDescAddr;
  VAR(uint32, AUTOMATIC) LulUFCVValue;
  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;
  #endif
  /* MISRA Violation: START Msg(4:2982)-9 */
  LenReturnError = ETH_ERR_OK;
  /* MISRA Violation: END Msg(4:2982)-9 */
  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  LddReturnValue = E_OK;
  /* Check if the module is initialized */
  if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    /* Report error to DET that module is not initialized */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_RECEIVE_SID, ETH_E_NOT_INITIALIZED);
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    /* Report error to DET if the CtrlIdx is Invalid */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_RECEIVE_SID, ETH_E_INV_CTRL_IDX);
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_MODE_DOWN == Eth_GenControllerMode)
  {
    /* Raise DET INV_Mode */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_RECEIVE_SID, ETH_E_INV_MODE);
    LddReturnValue = E_NOT_OK;
  }
  else if (NULL_PTR == RxStatusPtr)
  {
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_RECEIVE_SID, ETH_E_PARAM_POINTER);
    LddReturnValue = E_NOT_OK;
  }
  /* MISRA Violation: START Msg(2:2814)-7 */
  else if (ETH_ZERO_8 == Eth_GpConfigPtr->Eth_QueueConfig.ucNumberOfRxQueue)
  /* MISRA Violation: END Msg(2:2814)-7 */
  {
    /* Raise DET INV_Mode */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_RECEIVE_SID, ETH_E_INVALID_DATABASE);
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No action required */
  }

  if (E_OK != LddReturnValue)
  {
    /* No action required */
  }
  else
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
  {
    /* Initialize receive status */
    *RxStatusPtr = ETH_NOT_RECEIVED;
    for (LucQIdx = ETH_ZERO_8; LucQIdx < ETH_RX_QUEUE_COUNT; LucQIdx++)
    {
      /* Read the unread frame counter */
      LucRxFrameCount = (uint8)Eth_UFCounterGet(CtrlIdx, LucQIdx);

      if (LucRxFrameCount >= ETH_UFCV_MAX_VAL)
      {
        /* Maybe there are unread frames that do not include in UFCV */
        Eth_GaaDevice[CtrlIdx].UFCOverflow[LucQIdx] = ETH_TRUE;
      }
      else
      {
        /* No action required */
      }
      /* Check the Queue when UFCV equals zero and overflow flag is true */
      if (ETH_TRUE == Eth_GaaDevice[CtrlIdx].UFCOverflow[LucQIdx])
      {
        LucRxFrameCount = Eth_UnreadDataCheck(LucQIdx);
        /* Clear flag when no frame is available */
        if (ETH_ZERO == LucRxFrameCount)
        {
          Eth_GaaDevice[CtrlIdx].UFCOverflow[LucQIdx] = ETH_FALSE;
        }
        else
        {
          /* No action required */
        }
      }
      else
      {
        /* No action required */
      }
      /* Process the Queue until a frame is valid or no frame available */
      while ((ETH_NOT_RECEIVED == *RxStatusPtr) &&
        (ETH_ZERO_8 < LucRxFrameCount))
      {
        LulDescAddr = Eth_GaaDevice[CtrlIdx].NextRxDesc[LucQIdx];
        /* Stay on this Queue and give 1 frame for each call */
        /* Cast to larger type to be compatible with type of input parameter */
        LenReturnError = Eth_RxQueueProcess(CtrlIdx, LucQIdx,
          /* MISRA Violation: START Msg(4:0306)-2 */
          (uint32)(&LstRxFrame), LulDescAddr);
          /* MISRA Violation: END Msg(4:0306)-2 */
        /* A frame is processed, decrease the counter */
        LucRxFrameCount--;
        /* Decrement unread frame counter by 1 */
        Eth_UFCounterDecrement(CtrlIdx, LucQIdx);
        /* Read the unread frame counter */
        LulUFCVValue = Eth_UFCounterGet(CtrlIdx, LucQIdx);
        if (LulUFCVValue >= (uint32)(ETH_UFCV_MAX_VAL - ETH_ONE))
        {
          /* Maybe there are unread frames that do not include in UFCV */
          Eth_GaaDevice[CtrlIdx].UFCOverflow[LucQIdx] = ETH_TRUE;
        }
        else
        {
          /* No action required */
        }
        if (ETH_ERR_OK == LenReturnError)
        {
          /* Filter the received frame */
          LddFilterResult = Eth_RxFilterProcess(CtrlIdx, &LstRxFrame, LucQIdx);

          if (E_OK == LddFilterResult)
          {
            /* Frame valid, call EthIf Callback */
            Eth_RxCallEthIf(CtrlIdx, LucQIdx, &LstRxFrame);
            /* Update Status */
            *RxStatusPtr = ETH_RECEIVED;
          }
          else
          {
            /* Frame is discarded, No call of EthIf */
            /* Increase Discarded Packets */
            #if (ETH_GET_DROP_COUNT_API == STD_ON)
            Eth_GaaDevice[CtrlIdx].DropCount.DiscardedPacketNo++;
            #endif
          }
          /* Update descriptor chain */
          Eth_RxDescChainUpdate(LulDescAddr, LucQIdx);
        }
        else
        {
          /* Error happened, report DEM */
          Dem_ReportErrorStatus(ETH_E_DESC_DATA, DEM_EVENT_STATUS_FAILED);
          /* Frame is rejected */
          *RxStatusPtr = ETH_NOT_RECEIVED;
        }
      }
      if ((ETH_RECEIVED == *RxStatusPtr) && (ETH_ZERO_32 < LucRxFrameCount))
      {
        /* More frames are available */
        *RxStatusPtr = ETH_RECEIVED_MORE_DATA_AVAILABLE;
        break;
      }
      else
      {
        /* No frame was accepted, or continue checking available frame */
      }
    }
  } /* (E_OK != LddReturnValue) */
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
#endif /*End ETH_CTRL_ENABLE_RX_INTERRUPT == STD_OFF */

/*******************************************************************************
** Function Name         : Eth_TxConfirmation
**
** Service ID            : 0xC
**
** Description           : This API triggers frame transmission confirmation
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GenControllerMode,
**                         Eth_GstBufferLock
**
** Functions invoked     : Det_ReportError, Eth_TxBufferCheck
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_010] */
FUNC(void, ETH_CODE_SLOW) Eth_TxConfirmation (uint8 CtrlIdx)
{
  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  LddReturnValue = E_OK;

  /* Check if the module is initialized */
  if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    /* Report error to DET that module is not initialized */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_TXCONFIRMATION_SID, ETH_E_NOT_INITIALIZED);
    /* Cast type of macro to larger type to unify type in assignment */
    LddReturnValue = (uint8)E_NOT_OK;
  }
  else if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    /* Report error to DET if the CtrlIdx is Invalid*/
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_TXCONFIRMATION_SID, ETH_E_INV_CTRL_IDX);
    /* Cast type of macro to larger type to unify type in assignment */
    LddReturnValue = (uint8)E_NOT_OK;
  }
  else if (ETH_MODE_ACTIVE != Eth_GenControllerMode)
  {
    /* Report Error to DET, if the controller mode is not Active */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_TXCONFIRMATION_SID, ETH_E_INV_MODE);
    LddReturnValue = (uint8)E_NOT_OK;
  }
  else
  {
    /* No action required */
  } /* (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx) */

  /* Retrieve the Controller Mode */
  if (E_OK != LddReturnValue)
  {
    /* No action required */
  }
  else
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
  {
    /* Check the Tx buffers resource and release if data has been fetched */
    Eth_TxBufferCheck(CtrlIdx);
  }
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */


/*******************************************************************************
** Function Name         : Eth_SetPhysAddr
**
** Service ID            : 0x13
**
** Description           : This API sets the physical source address used
**                         of the specified controller
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant for the same Controller
**                         Reentrant for different Controller
**
** Input Parameters      : CtrlIdx     - Index of the ETH Controller
**                         PhysAddrPtr - Pointer to memory containing the
**                                       physical source address (MAC address)
**                                       in network byte order.
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GaaDevice,
**                         Eth_GenControllerMode, Eth_GaaBaseAddr
**
** Functions invoked     : Det_ReportError, Eth_StoreMacIntoReg
**
** Registers Used        : MAHR, MALR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_018] */
FUNC(void, ETH_CODE_SLOW) Eth_SetPhysAddr
(
  uint8 CtrlIdx,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) PhysAddrPtr
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;
  VAR(uint8, AUTOMATIC) LucCount;

  LddReturnValue = E_OK;

  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    /* Report error to DET that module is not initialized */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_SETPHYSADDR_SID,
      ETH_E_NOT_INITIALIZED);
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    /* DET Error */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_SETPHYSADDR_SID,
      ETH_E_INV_CTRL_IDX);
    LddReturnValue = E_NOT_OK;
  }
  else if (NULL_PTR == PhysAddrPtr)
  {
    /* DET Error */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_SETPHYSADDR_SID,
      ETH_E_PARAM_POINTER);
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_MODE_DOWN != Eth_GenControllerMode)
  {
    /* Report Error to DET, if the controller mode is not Down */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_SETPHYSADDR_SID, ETH_E_INV_MODE);
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No Action */
  }
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */

  if (E_OK == LddReturnValue)
  {
    Eth_StoreMacIntoReg(CtrlIdx, PhysAddrPtr);
    for (LucCount = ETH_ZERO; LucCount < ETH_MACADDR_SIZE; LucCount++)
    {
      /* MISRA Violation: START Msg(4:0488)-3 */
      Eth_GaaDevice[CtrlIdx].MacAddr[LucCount] = *(PhysAddrPtr + LucCount);
      /* MISRA Violation: END Msg(4:0488)-3 */
    }
  }
  else
  {
    /* No Action */
  }
}

/*******************************************************************************
** Function Name         : Eth_UpdatePhysAddrFilter
**
** Service ID            : 0x12
**
** Description           : This API Add/Remove a Address from the Filter in
**                         Reception of the specified controller
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant for the same Controller
**                         Reentrant for different Controller
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**                         PhysAddrPtr - Pointer to memory containing the
**                                       physical source address (MAC address)
**                                       in network byte order.
**                         Action - Add or remove the address from
**                                  the Ethernet controllers filter.
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LddReturnValue - E_OK: filter was successfully
**                                                changed
**                                        - E_NOT_OK: filter could not be
**                                                    changed
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GaaBroadcastAddr,
**                         Eth_GaaNullAddr, Eth_GenAddrFilterMode,
**                         Eth_GstControllerFilterAddr, Eth_GaaBaseAddr
**
** Functions invoked     : Eth_CompareMac, Eth_AddToFilter, Eth_EnablePromMode,
**                         Eth_RemoveFromFilter, Eth_DisablePromMode,
**                         Det_ReportError
**
** Registers Used        : ECMR
**
*******************************************************************************/
#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
/* TRACE[ETH_DDD_ACT_019] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_UpdatePhysAddrFilter
(
  uint8 CtrlIdx,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) PhysAddrPtr,
  Eth_FilterActionType Action
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;
  /* This is used only for get return value from Eth_CompareMac */
  VAR(Std_ReturnType, AUTOMATIC) LddCompareBroadcast;
  VAR(Std_ReturnType, AUTOMATIC) LddCompareNullAddr;

  LddReturnValue = E_OK;

  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    /* Report error to DET that module is not initialized */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_UPDATEPHYSADDRFILTER_SID, ETH_E_NOT_INITIALIZED);
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    /* DET Error */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_UPDATEPHYSADDRFILTER_SID, ETH_E_INV_CTRL_IDX);
    LddReturnValue = E_NOT_OK;
  }
  else if (NULL_PTR == PhysAddrPtr)
  {
    /* DET Error */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_UPDATEPHYSADDRFILTER_SID, ETH_E_PARAM_POINTER);
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No Action */
  }

  if (E_OK == LddReturnValue)
  #endif
  {
    LddCompareBroadcast = Eth_CompareMac(PhysAddrPtr, Eth_GaaBroadcastAddr);
    LddCompareNullAddr = Eth_CompareMac(PhysAddrPtr, Eth_GaaNullAddr);
    if (E_OK == LddCompareBroadcast)
    {
      /* Change filter mode to Promiscuous mode */
      Eth_GenAddrFilterMode[CtrlIdx] = ETH_PROM_MODE;
      Eth_EnablePromMode(CtrlIdx);
      Eth_GstControllerFilterAddr[CtrlIdx].RxFilterLength = ETH_ZERO_8;
    } /* (E_OK == LddCompareBroadcast) */
    else if (E_OK == LddCompareNullAddr)
    {
      /* Change filter mode to Normal Mode */
      Eth_GenAddrFilterMode[CtrlIdx] = ETH_NORMAL_MODE;
      Eth_DisablePromMode(CtrlIdx);
      Eth_GstControllerFilterAddr[CtrlIdx].RxFilterLength = ETH_ZERO_8;
    } /* (E_OK == LddCompareNullAddr) */
    else if (ETH_ADD_TO_FILTER == Action)
    {
      /* Only add address to filter in normal mode or filter mode */
      if ((ETH_NORMAL_MODE == Eth_GenAddrFilterMode[CtrlIdx]) ||
        (ETH_FILTER_MODE == Eth_GenAddrFilterMode[CtrlIdx]))
      {
        #if (ETH_CRITICAL_SECTION_PROTECTION == STD_ON)
        /* Enter Critical */
        ETH_ENTER_CRITICAL_SECTION(GLOBAL_PROTECTION);
        #endif
        /* Add Address to the Rx Filter Address Array */
        LddReturnValue = Eth_AddToFilter(CtrlIdx, PhysAddrPtr);
        #if (ETH_CRITICAL_SECTION_PROTECTION == STD_ON)
        /* Exit Critical */
        ETH_EXIT_CRITICAL_SECTION(GLOBAL_PROTECTION);
        #endif
      }
      else
      {
        /* No action in promiscuous mode */
      }
    } /* (ETH_ADD_TO_FILTER == Action) */
    else if (ETH_REMOVE_FROM_FILTER == Action)
    {
      #if (ETH_CRITICAL_SECTION_PROTECTION == STD_ON)
      /* Enter Critical */
      ETH_ENTER_CRITICAL_SECTION(GLOBAL_PROTECTION);
      #endif
      /* Remove Address from the Rx Filter Address Array */
      LddReturnValue = Eth_RemoveFromFilter(CtrlIdx, PhysAddrPtr);
      #if (ETH_CRITICAL_SECTION_PROTECTION == STD_ON)
      /* Exit Critical */
      ETH_EXIT_CRITICAL_SECTION(GLOBAL_PROTECTION);
      #endif
    }
    else
    {
      /* Invalid action */
      LddReturnValue = E_NOT_OK;
    }
  }
  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  else
  {
    /* No Action since there is an error */
  } /* (E_OK == LddReturnValue) */
  #endif

  return LddReturnValue;
}
#endif

/*******************************************************************************
** Function Name         : Eth_GetCurrentTime
**
** Service ID            : 0x16
**
** Description           : This API return TimeStamp from the HW register
**                         previously started
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : timeQualPtr  - Quality of the HW TimeStamp returned
**                         timeStampPtr - Current TimeStamp value
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GaaBaseAddr
**
** Functions invoked     : Det_ReportError, Eth_GetPTPTime
**
** Registers Used        : GCCR, CSR, GCTi(i = 0 to 2), CCC, FSO_CTL
**
*******************************************************************************/
#if (ETH_GLOBAL_TIME_SUPPORT == STD_ON )
/* TRACE[ETH_DDD_ACT_013] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_GetCurrentTime
(
  uint8 CtrlIdx,
  CONSTP2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
  CONSTP2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  LddReturnValue = E_OK;

  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    /* DET Error */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_GETCURRENTTIME_SID, ETH_E_INV_CTRL_IDX);
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    /* Report error to DET that module is not initialized */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_GETCURRENTTIME_SID, ETH_E_NOT_INITIALIZED);
    LddReturnValue = E_NOT_OK;
  }
  else if ((NULL_PTR == timeQualPtr) || (NULL_PTR == timeStampPtr))
  {
    /* Report error DET Error  ETH_E_PARAM_POINTER  */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_GETCURRENTTIME_SID, ETH_E_PARAM_POINTER);
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No Action */
  }
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */

  if (E_OK == LddReturnValue)
  {
    LddReturnValue = Eth_GetPTPTime(CtrlIdx, timeQualPtr, timeStampPtr);
  }
  else
  {
    /* No Action */
  }

  return LddReturnValue;
}

/*******************************************************************************
** Function Name         : Eth_EnableEgressTimeStamp
**
** Service ID            : 0x17
**
** Description           : This API enable TimeStamp capture for the message
**                         that will be transmitted
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**                         BufIdx  - Buffer index
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variable(s)    : Eth_GenDriverState, Eth_GstBufferLock,
**                         Eth_GaaBaseAddr
**
** Functions invoked     : Det_ReportError, Eth_EnableTSFifo
**
** Registers Used        : TCCR, FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_012] */
FUNC(void, ETH_CODE_SLOW) Eth_EnableEgressTimeStamp
(
  uint8 CtrlIdx,
  uint8 BufIdx
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  LddReturnValue = E_OK;

  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    /* DET Error */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_ENEGRESSTS_SID,
      ETH_E_INV_CTRL_IDX);
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_TX_BUF_TOTAL <= BufIdx)
  { /* Invalid Buffer */

    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_ENEGRESSTS_SID,
      ETH_E_INV_PARAM);
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    /* Report error to DET that module is not initialized   */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_ENEGRESSTS_SID,
      ETH_E_NOT_INITIALIZED);
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_BUF_LOCKED != Eth_GstBufferLock[BufIdx].bufLock)
  {
    /* Buffer has not been allocated for an active transmission */
    /* Report error DET Error  INV Parameter */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_ENEGRESSTS_SID,
      ETH_E_INV_PARAM);
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No Action */
  }
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */

  if (E_OK == LddReturnValue)
  {
    /* TRACE[ETH_DDD_ACT_012_GLB001] */
    Eth_GstBufferLock[BufIdx].benableTS = ETH_TRUE;
    /*
    * This will be written in the Ram Descriptor on the next Eth_Transit
    * call
    */
    Eth_EnableTSFifo(CtrlIdx);
  }
  else
  {
    /* No action */
  }
}

/*******************************************************************************
** Function Name         : Eth_GetEgressTimeStamp
**
** Service ID            : 0x18
**
** Description           : This API Return TimeStamp from the HW register
**                         previously started
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**                         BufIdx  - Buffer index
**
** InOut Parameters      : None
**
** Output Parameters     : timeQualPtr  - Quality of the HW TimeStamp returned
**                         timeStampPtr - Current TimeStamp value
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variable(s)    : Eth_GenDriverState, Eth_GaaBaseAddr
**
** Functions invoked     : Det_ReportError, Eth_ReadTSFifo
**
** Registers Used        : TFA2, TFA0, TCCR, TSR, FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_014] */
FUNC(void, ETH_CODE_SLOW) Eth_GetEgressTimeStamp
(
  uint8 CtrlIdx,
  uint8 BufIdx,
  CONSTP2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
  CONSTP2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  LddReturnValue = E_OK;

  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    /* DET Error */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_GETEGRESSTS_SID,
      ETH_E_INV_CTRL_IDX);
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_STATE_INIT != Eth_GenDriverState)
  {
    /* Report error to DET that module is not initialized */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_GETEGRESSTS_SID,
      ETH_E_NOT_INITIALIZED);
    LddReturnValue = E_NOT_OK;
  }
  else if ((NULL_PTR == timeQualPtr) || (NULL_PTR == timeStampPtr))
  {
    /* Report error DET Error  ETH_E_PARAM_POINTER  */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_GETEGRESSTS_SID,
      ETH_E_PARAM_POINTER);
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No Action */
  }
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */

  if (E_OK == LddReturnValue)
  {
    Eth_ReadTSFifo(CtrlIdx, BufIdx, timeQualPtr, timeStampPtr);
  }
  else
  {
    /* No action */
  }
}

/*******************************************************************************
** Function Name         : Eth_GetIngressTimeStamp
**
** Service ID            : 0x19
**
** Description           : This API return TimeStamp from the HW register
**                         previously started
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**                         DataPtr - Pointer to the received message
**
** InOut Parameters      : None
**
** Output Parameters     : timeQualPtr  - Quality of the HW TimeStamp returned
**                         timeStampPtr - Current TimeStamp value
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GaaTempTsDescr,
**                         Eth_GaaTempTimeStamp, Eth_GucTsQueueIdx
**
** Functions invoked     : Det_ReportError, Eth_ReadTSDescriptor
**
** Registers Used        : None
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_015] */
FUNC(void, ETH_CODE_SLOW) Eth_GetIngressTimeStamp
(
  uint8 CtrlIdx,
  CONSTP2CONST(Eth_DataType, AUTOMATIC, ETH_APPL_CONST) DataPtr,
  CONSTP2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
  CONSTP2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  #if (ETH_DEV_ERROR_DETECT != STD_ON)
  /* Suppress unused parameter warnings for the parameters below */
  /* MISRA Violation: START Msg(4:3112)-13 */
  (void) CtrlIdx;
  /* MISRA Violation: END Msg(4:3112)-13 */
  /* MISRA Violation: START Msg(4:3112)-13 */
  (void) DataPtr;
  /* MISRA Violation: END Msg(4:3112)-13 */
  #endif /* (ETH_DEV_ERROR_DETECT != STD_ON) */

  LddReturnValue = E_OK;

  #if (ETH_DEV_ERROR_DETECT == STD_ON)

  if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    /* Report error to DET that module is not initialized   */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_GETINGRESSTS_SID,
      ETH_E_NOT_INITIALIZED);
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    /* DET Error */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_GETINGRESSTS_SID,
      ETH_E_INV_CTRL_IDX);
    LddReturnValue = E_NOT_OK;
  }
  else if (NULL_PTR == DataPtr)
  {
    /* Report error DET Error  ETH_E_PARAM_POINTER  */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_GETINGRESSTS_SID,
      ETH_E_PARAM_POINTER);
    LddReturnValue = E_NOT_OK;
  }
  else if ((NULL_PTR == timeQualPtr) || (NULL_PTR == timeStampPtr))
  {
    /* Report error DET Error  ETH_E_PARAM_POINTER  */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_GETINGRESSTS_SID,
      ETH_E_PARAM_POINTER);
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No action */
  }
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */

  if (E_OK == LddReturnValue)
  {
    Eth_ReadTSDescriptor(timeQualPtr, timeStampPtr);
  }
  else
  {
    /* No Action */
  }
}

/*******************************************************************************
** Function Name         : Eth_SetCorrectionTime
**
** Service ID            : 0x1a
**
** Description           : This API Allows the Time Slave to adjust the local
**                         ETH Reference clock in HW.
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**                         timeOffsetPtr - offset between time stamp grandmaster
**                                         and time stamp by local clock:
**                         rateRatioPtr - time elements to calculate and to
**                                        modify the ratio of the frequency of
**                                        the grandmaster in relation to
**                                        the frequency of the Local Clock
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GaaBaseAddr
**
** Functions invoked     : Det_ReportError, Eth_AdjustPTPOffset,
**                         Eth_AdjustPTPTime, Eth_AdjustPTPIncrement
**
** Registers Used        : GTO0, GTO1, GTO2, GCCR, CCC, GCTi(i = 0,1,2),
**                         CSR, GTI, FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_016] */
FUNC(void, ETH_CODE_SLOW) Eth_SetCorrectionTime
(
  uint8 CtrlIdx,
  CONSTP2CONST(Eth_TimeIntDiffType, AUTOMATIC, ETH_APPL_CONST) timeOffsetPtr,
  CONSTP2CONST(Eth_RateRatioType, AUTOMATIC, ETH_APPL_CONST) rateRatioPtr
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  LddReturnValue = E_OK;

  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    /* DET Error */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_SETCORRECTIONTIME_SID, ETH_E_INV_CTRL_IDX);
    LddReturnValue = E_NOT_OK;
  }
  else if ((NULL_PTR == timeOffsetPtr) || (NULL_PTR == rateRatioPtr))
  {
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_SETCORRECTIONTIME_SID,  ETH_E_PARAM_POINTER);
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    /* Report error to DET that module is not initialized   */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_SETCORRECTIONTIME_SID, ETH_E_NOT_INITIALIZED);
    LddReturnValue = E_NOT_OK;
  }
  else if ((timeOffsetPtr->diff.nanoseconds > ETH_MAX_NANOSECOND) ||
    (rateRatioPtr->IngressTimeStampDelta.diff.nanoseconds > ETH_MAX_NANOSECOND)
    || (rateRatioPtr->OriginTimeStampDelta.diff.nanoseconds >
    ETH_MAX_NANOSECOND))
  {
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID,
      ETH_SETCORRECTIONTIME_SID, ETH_E_INV_PARAM);
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No Action */
  }
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */

  if (E_OK == LddReturnValue)
  {
    if (ETH_TRUE == timeOffsetPtr->sign)
    {
      /* Time Stamp Diff is positive */
      LddReturnValue = Eth_AdjustPTPOffset(CtrlIdx, timeOffsetPtr);
    }
    else
    {
       /* Time Stamp Diff is negative */
      LddReturnValue = Eth_AdjustPTPTime(CtrlIdx, timeOffsetPtr);
    }
    if (E_OK == LddReturnValue)
    {
      (void)Eth_AdjustPTPIncrement(CtrlIdx, rateRatioPtr);
    }
    else
    {
      /* No Action */
    }
  }
  else
  {
    /* No Action */
  }
}

/*******************************************************************************
** Function Name         : Eth_SetGlobalTime
**
** Service ID            : 0x1b
**
** Description           : This API allows the Time Master to adjust the global
**                         ETH Reference clock in HW.
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**                         timeStampPtr - new time stamp
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GaaBaseAddr
**
** Functions invoked     : Det_ReportError, Eth_SetPTPTime
**
** Registers Used        : GTO0, GTO1, GTO2, GCCR, CSR, FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_017] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_SetGlobalTime
(
  uint8 CtrlIdx,
  CONSTP2CONST(Eth_TimeStampType, AUTOMATIC, ETH_APPL_CONST) timeStampPtr
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  LddReturnValue = E_OK;

  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    /* DET Error */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_SETGLOBALTIME_SID,
      ETH_E_INV_CTRL_IDX);
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    /* Report error to DET that module is not initialized   */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_SETGLOBALTIME_SID,
      ETH_E_NOT_INITIALIZED);
    LddReturnValue = E_NOT_OK;
  }
  else if (NULL_PTR == timeStampPtr)
  {
    /* Report error DET Error  ETH_E_PARAM_POINTER  */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_SETGLOBALTIME_SID,
      ETH_E_PARAM_POINTER);
    LddReturnValue = E_NOT_OK;
  }
  else if (timeStampPtr->nanoseconds > ETH_MAX_NANOSECOND)
  {
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_SETGLOBALTIME_SID,
      ETH_E_INV_PARAM);
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /*No action */
  }

  if (E_OK == LddReturnValue)
  {
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
    LddReturnValue = Eth_SetPTPTime(CtrlIdx, timeStampPtr);
  #if (ETH_DEV_ERROR_DETECT == STD_ON)
  }
  else
  {
    /* An error occurred previously */
    LddReturnValue = E_NOT_OK;
  }
  #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
  return LddReturnValue;
}
#endif /* (ETH_GLOBAL_TIME_SUPPORT == STD_ON ) */

/*******************************************************************************
** Function Name         : Eth_GetDropCount
**
** Service ID            : 0x14
**
** Description           : This API read a list of dropped packet and return to
**                         the User
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**                         CountValues - Maximal number of values can be written
**                                       from DropCount
**
** InOut Parameters      : None
**
** Output Parameters     : DropCount - Pointer to list of dropped packet
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GaaMacCounterAddress,
**                         Eth_GaaDevice
**
** Functions invoked     : Det_ReportError, Eth_RetrieveMacCounter
**
** Registers Used        : FRECR, CEFCR, TSFRCR, TLFRCR, TROCR, RFCR
**
*******************************************************************************/
#if (ETH_GET_DROP_COUNT_API == STD_ON)
/* TRACE[ETH_DDD_ACT_134] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_GetDropCount
(
  uint8 CtrlIdx,
  uint8 CountValues,
  CONSTP2VAR(uint32, AUTOMATIC, ETH_APPL_DATA) DropCount
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  LddReturnValue = E_OK;

  if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    #if (ETH_DEV_ERROR_DETECT == STD_ON)
    /* Report error to DET that module is not initialized */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_GETDROPCOUNT_SID,
      ETH_E_NOT_INITIALIZED);
    #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    #if (ETH_DEV_ERROR_DETECT == STD_ON)
    /* DET Error */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_GETDROPCOUNT_SID,
      ETH_E_INV_CTRL_IDX);
    #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
    LddReturnValue = E_NOT_OK;
  }
  else if (NULL_PTR == DropCount)
  {
    #if (ETH_DEV_ERROR_DETECT == STD_ON)
    /* DET Error */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_GETDROPCOUNT_SID,
      ETH_E_PARAM_POINTER);
    #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No Action */
  }

  if (E_OK == LddReturnValue)
  {
    if (ETH_MAC_MAXCOUNTERS < CountValues)
    {
      CountValues = ETH_MAC_MAXCOUNTERS;
    }
    else
    {
      /* No action */
    }
    /* Get Statistics from Underlying Mac Layer */
    Eth_RetrieveMacCounter(CtrlIdx, CountValues, DropCount);
  }
  else
  {
    /* No Action since there is an error */
  }

  return LddReturnValue;
}
#endif

/*******************************************************************************
** Function Name         : Eth_GetEtherStats
**
** Service ID            : 0x15
**
** Description           : This API returns the list of values according to
**                         IETF RFC2819
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : etherStats - Pointer to list of values according to
**                                      IETF RFC 2819 (Remote Network Monitoring
**                                      Management Information Base)
**
** Return parameter      : Std_ReturnType E_OK / E_NOT_OK
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenDriverState, Eth_GaaDevice, Eth_GaaBaseAddr
**
** Functions invoked     : Det_ReportError, Eth_RetrieveStatistics
**
** Registers Used        : MAFCR, CEFCR, TSFRCR, TLFRCR
**
*******************************************************************************/
#if (ETH_GET_ETHER_STATS_API == STD_ON)
/* TRACE[ETH_DDD_ACT_135] */
FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_GetEtherStats
(
  uint8 CtrlIdx,
  CONSTP2VAR(uint32, AUTOMATIC, ETH_APPL_DATA) etherStats
)
{
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  LddReturnValue = E_OK;

  if (ETH_STATE_UNINIT == Eth_GenDriverState)
  {
    #if (ETH_DEV_ERROR_DETECT == STD_ON)
    /* Report error to DET that module is not initialized */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_GETETHERSTATS_SID,
      ETH_E_NOT_INITIALIZED);
    #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
    LddReturnValue = E_NOT_OK;
  }
  else if (ETH_MAX_CTRLS_SUPPORTED <= CtrlIdx)
  {
    #if (ETH_DEV_ERROR_DETECT == STD_ON)
    /* DET Error */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_GETETHERSTATS_SID,
      ETH_E_INV_CTRL_IDX);
    #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
    LddReturnValue = E_NOT_OK;
  }
  else if (NULL_PTR == etherStats)
  {
    #if (ETH_DEV_ERROR_DETECT == STD_ON)
    /* DET Error */
    (void)Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, ETH_GETETHERSTATS_SID,
      ETH_E_PARAM_POINTER);
    #endif /* (ETH_DEV_ERROR_DETECT == STD_ON) */
    LddReturnValue = E_NOT_OK;
  }
  else
  {
    /* No Action */
  }

  if (E_OK == LddReturnValue)
  {
    /* Call Low Level to get statistics */
    Eth_RetrieveStatistics(CtrlIdx, etherStats);
  }
  else
  {
    /* No Action since there is an error */
  }

  return LddReturnValue;
}
#endif

/*******************************************************************************
** Function Name         : Eth_MainFunction
**
** Service ID            : 0x0a
**
** Description           : This API checks for controller errors and lost frames
**                         Used for polling state changes
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
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GenMainFunctionState, Eth_GenControllerMode,
**                         Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GpConfigPtr,
**                         Eth_GucRxBuff, Eth_GenDriverState
**
** Functions invoked     : Eth_ModeTransitionProcess, Eth_ReportExtDem
**
** Registers Used        : CEFCR, TSFRCR, TLFRCR, RFCR, TROCR, FRECR, CSR,
**                         FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_136] */
FUNC(void, ETH_CODE_SLOW) Eth_MainFunction(void)
{
  VAR(uint8, AUTOMATIC) LucInc;

  /* Check whether ETH module has been initialized */
  if (ETH_STATE_INIT == Eth_GenDriverState)
  {
    /* MISRA Violation: START Msg(4:2877)-8 */
    for (LucInc = ETH_ZERO_8; ETH_MAX_CTRLS_SUPPORTED > LucInc; LucInc++)
    /* MISRA Violation: END Msg(4:2877)-8 */
    {
      /* Check for controller mode transition */
      Eth_ModeTransitionProcess(LucInc);
      /* Check for controller errors and lost frames */
      Eth_ReportExtDem(LucInc);
    }
  }
  else
  {
    /* No action required since ETH drive has not been initialized */
  }
}

/*******************************************************************************
** Function Name         : Eth_ModeTransitionProcess
**
** Service ID            : None
**
** Description           : This service performs controller mode transition
**                         checking
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variables      : Eth_GenMainFunctionState, Eth_GenControllerMode,
**                         Eth_GaaBaseAddr, Eth_GaaDevice, Eth_GpConfigPtr,
**                         Eth_GucRxBuff
**
** Functions invoked     : EthIf_CtrlModeIndication, Dem_ReportErrorStatus
**                         Eth_OpModeGet, Eth_DescConfig, Eth_DeviceStructInit
**
** Registers Used        : CSR, FSO_CTL
**
*******************************************************************************/
/* TRACE[ETH_DDD_ACT_195] */
/* TRACE[ETH_DDD_INF_161] */
static FUNC(void, ETH_CODE_SLOW) Eth_ModeTransitionProcess(uint8 CtrlIdx)
{
  VAR(Eth_OpStatusType, AUTOMATIC) LenMode;

  /* MISRA Violation: START Msg(4:3415)-11 */
  if ((AVB_CONFIG == Eth_GenMainFunctionState) &&
  /* MISRA Violation: END Msg(4:3415)-11 */
    (ETH_MODE_ACTIVE == Eth_GenControllerMode))
  {
    /* Check whether mode transition has been completed */
    LenMode = Eth_OpModeGet(CtrlIdx);
    if (ETH_CONFIG == LenMode)
    {
      /* Reset descriptor chain Tx and Rx */
      Eth_DescConfig(CtrlIdx);
      /* Reset the device specific global variables */
      Eth_DeviceStructInit(CtrlIdx);
      /* Notify EthIf with Mode DOWN */
      EthIf_CtrlModeIndication(CtrlIdx, ETH_MODE_DOWN);
      Eth_GenControllerMode = ETH_MODE_DOWN;
      /* Clear MainFunctionState */
      Eth_GenMainFunctionState = AVB_IDLE;
    }
    else if (LenMode != ETH_OPERATION)
    {
      /* Error handling */
      Dem_ReportErrorStatus(ETH_E_OPMODE, DEM_EVENT_STATUS_FAILED);
    }
    else
    {
      /* Mode transition has not been completed */
    }
  }
  else
  {
    /* No mode transition */
  }
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */

/*******************************************************************************
** Function Name         : Eth_DeviceStructInit
**
** Service ID            : NA
**
** Description           : This API reset the device specific global variables
**
** Sync/Async            : Synchronous.
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variables      : Eth_GaaDevice, Eth_GpConfigPtr, Eth_GenDriverState
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_100] */
/* TRACE[ETH_DDD_INF_029] */
static FUNC(void, ETH_CODE_SLOW) Eth_DeviceStructInit(uint8 CtrlIdx)
{
  VAR(uint8, AUTOMATIC) LucCount;

  if (ETH_STATE_INIT == Eth_GenDriverState)
  {
    /* No action required */
  }
  else
  {
    /* Initialize MAC address */
    for (LucCount = ETH_ZERO; LucCount < ETH_MACADDR_SIZE; LucCount++)
    {
      Eth_GaaDevice[CtrlIdx].MacAddr[LucCount] =
        /* MISRA Violation: START Msg(2:2814)-7 */
        Eth_GpConfigPtr->ucEthMACAddr[LucCount];
        /* MISRA Violation: END Msg(2:2814)-7 */
    }
  }
  /* Initialize UFCOverflow flags */
  for (LucCount = ETH_ZERO; LucCount < ETH_RXQ_MAXNUM; LucCount++)
  {
    Eth_GaaDevice[CtrlIdx].UFCOverflow[LucCount] = ETH_FALSE;
  }
  /*Initialize Statistics */
  #if (ETH_GET_ETHER_STATS_API == STD_ON)
  /* TRACE[ETH_DDD_ACT_100_GLB003] */
  Eth_GaaDevice[CtrlIdx].Stats.RxPacketNo = ETH_ZERO_32;
  /* TRACE[ETH_DDD_ACT_100_GLB004] */
  Eth_GaaDevice[CtrlIdx].Stats.BroadcastPktNo = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].Stats.LastMulticastPktNo = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].Stats.StatsOctetsNo = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].Stats.Pkts64OctetsNo = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].Stats.Pkts65to127OctetsNo = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].Stats.Pkts128to255OctetsNo = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].Stats.Pkts256to511OctetsNo = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].Stats.Pkts512to1023OctetsNo = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].Stats.Pkts1024to1518OctetsNo = ETH_ZERO_32;
  #endif
  /*Initialize drop counters */
  #if (ETH_GET_DROP_COUNT_API == STD_ON)
  Eth_GaaDevice[CtrlIdx].DropCount.DiscardedPacketNo = ETH_ZERO_32;
  #if (ETH_PAYLOAD_CRC_ERROR_DETECT == STD_ON)
  Eth_GaaDevice[CtrlIdx].DropCount.PayloadCRCErrPacketNo = ETH_ZERO_32;
  #endif
  Eth_GaaDevice[CtrlIdx].LastRxErrPacketMainApi = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].LastResBitPacketMainApi = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].LastRxErrPacketNo = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].LastResBitPacketNo = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].LastTxRetryOverPacketNo = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].LastTxRetryOverPacketMainApi = ETH_ZERO_32;
  #endif
  /*
   * Initialize last counters which are used for Eth_GetDropCount,
   *  Eth_GetEtherStats and Eth_MainFunction
   */
  #if ((ETH_GET_DROP_COUNT_API == STD_ON) || \
  (ETH_GET_ETHER_STATS_API == STD_ON))
  Eth_GaaDevice[CtrlIdx].LastCRCPacketNo = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].LastCRCPacketMainApi = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].LastOversizePacketNo = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].LastOversizePacketMainApi = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].LastUndersizePacketNo = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].LastUndersizePacketMainApi = ETH_ZERO_32;
  #endif
  /* Initialize Rx Frames Lost Counter */
  /* TRACE[ETH_DDD_ACT_100_GLB005] */
  Eth_GaaDevice[CtrlIdx].DropPacketNo = ETH_ZERO_32;
  Eth_GaaDevice[CtrlIdx].LastDropPacketNoMainApi = ETH_ZERO_32;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */

/*******************************************************************************
** Function Name         : Eth_FindTxQueue
**
** Service ID            : NA
**
** Description           : This API find the Tx Queue associated with the passed
**                         priority. Otherwise return the default Queue.
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : Priority - Required priority
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_TxChannelType - Type of found Channel
**
** Preconditions         : Controller initialized
**
** Global Variables      : Eth_GpConfigPtr
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
#if (ETH_QOS_SUPPORT == STD_ON)
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_118] */
/* TRACE[ETH_DDD_INF_032] */
static FUNC(Eth_TxChannelType, ETH_CODE_SLOW) Eth_FindTxQueue
(
  uint8 Priority
)
{
  VAR(uint8, AUTOMATIC) LucReturnQIdx;
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(uint8, AUTOMATIC) LucTotalPriorities;

  /* MISRA Violation: START Msg(2:2814)-7 */
  LucReturnQIdx = Eth_GpConfigPtr->stEthPriority.DflTxQueue;
  /* MISRA Violation: END Msg(2:2814)-7 */
  LucTotalPriorities = Eth_GpConfigPtr->stEthPriority.ucNumberOfPriorities;

  for (LucCount = ETH_ZERO_8; LucCount < LucTotalPriorities; LucCount++)
  {
    if ((NULL_PTR != Eth_GpConfigPtr->stEthPriority.pPriorityTable) &&
      (Eth_GpConfigPtr->
        /* MISRA Violation: START Msg(4:0491)-5 */
        stEthPriority.pPriorityTable[LucCount].PriorityValue == Priority))
        /* MISRA Violation: END Msg(4:0491)-5 */
    {
      /* Found Traffic associated to the passed Priority */
      /* Return Corresponding Tx Queue and exit */
      LucReturnQIdx =
        /* MISRA Violation: START Msg(4:0491)-5 */
        Eth_GpConfigPtr->stEthPriority.pPriorityTable[LucCount].TxQueueId;
        /* MISRA Violation: END Msg(4:0491)-5 */
      break;
    }
    else
    {
      /* No action */
    }
  }
  return (Eth_TxChannelType) LucReturnQIdx;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
#endif

/*******************************************************************************
** Function Name         : Eth_IntMaskSet
**
** Service ID            : NA
**
** Description           : Setting interrupt mask.
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - return type of error
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GaaBaseAddr, Eth_GpConfigPtr
**
** Function(s) invoked   : Eth_RxIntMaskSet, Eth_RxWarningIntMaskSet,
**                         Eth_RxQFullIntMaskSet, Eth_TxIntMaskSet,
**                         Eth_GPtpIntMaskSet, Eth_CommonLineIntMaskSet
**
** Registers Used        : RIC0, RIC1, RIC2, CIE, TIC, GIC, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_145] */
/* TRACE[ETH_DDD_INF_122] */
static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_IntMaskSet(uint8 CtrlIdx)
{
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;
  #if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
  /* set RIC0 interrupt */
  LenReturnError = Eth_RxIntMaskSet(CtrlIdx);
  if (ETH_ERR_OK == LenReturnError)
  {
  #endif

  /* clear RIC1 interrupt */
  LenReturnError = Eth_RxWarningIntMaskSet(CtrlIdx, ETH_ZERO_32);
  #if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
  }
  else
  {
    /* No action required */
  }
  #endif
  if (ETH_ERR_OK == LenReturnError)
  {
    /* set RIC2 interrupt always  */
    LenReturnError = Eth_RxQFullIntMaskSet(CtrlIdx);
  }
  else
  {
    /* No action required */
  }
  if (ETH_ERR_OK == LenReturnError)
  {
  /* set GIC interrupt */
    LenReturnError = Eth_GPtpIntMaskSet(CtrlIdx, ETH_ZERO_32);
  }
  else
  {
    /* No action required */
  }
  #if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)
  if ( ETH_ERR_OK == LenReturnError)
  {
    /* set TIC interrupt */
    LenReturnError = Eth_TxIntMaskSet(CtrlIdx);
  }
  else
  {
    /* No action required */
  }
  #endif
  #if ((ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON) || \
    (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON))
  if (ETH_ERR_OK == LenReturnError)
  {
    /* set Common line 0 interrupt mode */
    LenReturnError = Eth_CommonLineIntMaskSet(CtrlIdx);
  }
  else
  {
    /* No action required */
  }
  #endif
  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */

/*******************************************************************************
** Function Name         : Eth_RxConfig
**
** Service ID            : NA
**
** Description           : Setting Reception registers.
**
** Sync/Async            : NA
**
** Reentrancy            : NA
**
** Input Parameters      : CtrlIdx - Index of the ETH Controller
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : Eth_ErrorType - return type of error
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GpConfigPtr, Eth_GaaBaseAddr
**
** Function(s) invoked   : Eth_RxQueueSet, Eth_RxPaddingSet, Eth_WriteIntoSFPReg
**
** Registers Used        : RQCi i = 0,1,...,4, CSR, RPC, SFPi i = 0, 1..31,
**                         FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_146] */
/* TRACE[ETH_DDD_INF_123] */
static FUNC(Eth_ErrorType, ETH_CODE_SLOW) Eth_RxConfig(uint8 CtrlIdx)
{
  VAR(uint8, AUTOMATIC) LucCount;
  VAR(uint8, AUTOMATIC) LucQIdx;
  VAR(uint8, AUTOMATIC) LucNumberOfRxQueue;
  VAR(Eth_PaddingConfigType, AUTOMATIC) LstPadding;
  VAR(Eth_ErrorType, AUTOMATIC) LenReturnError;
  /* Set padding to zero - Padding Not used */
  LstPadding.Dcnt = ETH_ZERO_8;
  LstPadding.Pcnt = ETH_ZERO_8;
  LenReturnError = ETH_ERR_OK;

  /* MISRA Violation: START Msg(2:2814)-7 */
  LucNumberOfRxQueue = Eth_GpConfigPtr->Eth_QueueConfig.ucNumberOfRxQueue;
  /* MISRA Violation: END Msg(2:2814)-7 */
  for (LucCount = ETH_ZERO_8; LucCount < LucNumberOfRxQueue; LucCount++)
  {
    /*Set RQCi register */
    LenReturnError = Eth_RxQueueSet(CtrlIdx,
      /* Cast to larger type to be compatible with type of input parameter */
      /* MISRA Violation: START Msg(4:0491)-5 */
      (uint32)(Eth_GpConfigPtr->Eth_QueueConfig.pRxQueueConfig[LucCount].id));
      /* MISRA Violation: END Msg(4:0491)-5 */
    if (ETH_ERR_OK != LenReturnError)
    {
      break;
    }
    else
    {
      /* No action required */
    }
  }
  if (ETH_ERR_OK == LenReturnError)
  {
    /* Set the Receive Padding */
    LenReturnError = Eth_RxPaddingSet(CtrlIdx,
      (const Eth_PaddingConfigType *)(&LstPadding));
  }
  else
  {
    /* No action required */
  }
  if (ETH_ERR_OK == LenReturnError)
  {
    for (LucCount = ETH_ZERO_8; LucCount < LucNumberOfRxQueue; LucCount++)
    {
      /* MISRA Violation: START Msg(4:0491)-5 */
      LucQIdx = Eth_GpConfigPtr->Eth_QueueConfig.pRxQueueConfig[LucCount].id;
      /* MISRA Violation: END Msg(4:0491)-5 */
      /***************Set SFPi Register ************/
      /* If Rx Queue is Stream set corresponding SPFI register */
      if (ETH_NCCHANNEL < LucQIdx)
      {
        /* Write into SFP Register */
        Eth_WriteIntoSFPReg(CtrlIdx, Eth_GpConfigPtr->Eth_QueueConfig
          /* MISRA Violation: START Msg(4:0491)-5 */
          .pRxQueueConfig[LucQIdx].EthPatternAddres, LucQIdx);
          /* MISRA Violation: END Msg(4:0491)-5 */
      }
      else
      {
        /* No action as this is a RX BE or RX NC Queue */
      }
    }
  }
  else
  {
    /* No action required */
  }
  return LenReturnError;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/*******************************************************************************
** Function Name         : Eth_ModuleStopCheck
**
** Service ID            : 0x1c
**
** Description           : Unintended Module Stop Check for EAVB-IF (Type B).
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
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
** Global Variable(s)    : Eth_GaaBaseAddr
**
** Function(s) invoked   : RCar_RfsoFaultControl, Eth_ControllerAccessCheck,
**                         Dem_ReportErrorStatus
**
** Registers Used        : VRR, GECMR, FSO_CTL
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
#if (ETH_MODULE_STOP_CHECK == STD_ON)
/* TRACE[ETH_DDD_ACT_151] */
FUNC(void, ETH_CODE_SLOW) Eth_ModuleStopCheck(void)
{
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  LddReturnValue = Eth_ControllerAccessCheck(ETH_AVB0);
  if (E_OK == LddReturnValue)
  {
    /* Checking PASS, Nothing to do */
  }
  else
  {
    /* Checking FAIL, Report to DEM */
    Dem_ReportErrorStatus(ETH_E_MODULE_STOP_CHECK, DEM_EVENT_STATUS_FAILED);
    #if (ETH_MODULE_STOP_CHK_ERR_NOTIFICATION_TGT != RFSO_DISABLE)
    /* Report to the external systems through RFSO external pins  */
    RCar_RfsoFaultControl(ETH_MODULE_STOP_CHK_ERR_NOTIFICATION_TGT,
      RFSO_FAULT_PERMANENT);
    #endif
  }
}
#endif /* (ETH_MODULE_STOP_CHECK == STD_ON) */
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/*******************************************************************************
** Function Name         : Eth_AddToFilter
**
** Service ID            : NA
**
** Description           : Add an address to the Filter Address array
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                         pFilterAddr - Filter Address need to be added
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LddReturnValue
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GstControllerFilterAddr, Eth_GaaBaseAddr,
**                         Eth_GenAddrFilterMode
**
** Functions invoked     : Eth_CopyMacAddress
**
** Registers Used        : ECMR, FSO_CTL
**
*******************************************************************************/
#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_160] */
/* TRACE[ETH_DDD_INF_139] */
static FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_AddToFilter
(
  uint8 LucCtrlIdx,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) pFilterAddr
)
{
  VAR(uint8, AUTOMATIC) LucAddElemIndex;
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;

  /* MISRA Violation: START Msg(4:2982)-9 */
  LddReturnValue = E_OK;
  /* MISRA Violation: END Msg(4:2982)-9 */

  /* Check whether Filter Address array has space to add */
  if (ETH_MAX_PHYS_FILTER_ADDR > Eth_GstControllerFilterAddr[LucCtrlIdx].
    RxFilterLength)
  {
    /* Get the index of element in filter array to be added */
    LucAddElemIndex = Eth_GstControllerFilterAddr[LucCtrlIdx].RxFilterLength;
    /* Add the address to the array */
    LddReturnValue = Eth_CopyMacAddress(
      (volatile uint8 *)(&Eth_GstControllerFilterAddr[LucCtrlIdx].
      RxFilterAddress[LucAddElemIndex][ETH_ZERO_8]), pFilterAddr);
    if (E_OK == LddReturnValue)
    {
      /* Increase the number of filter addresses */
      /* TRACE[ETH_DDD_ACT_160_GLB001] */
      Eth_GstControllerFilterAddr[LucCtrlIdx].RxFilterLength =
        Eth_GstControllerFilterAddr[LucCtrlIdx].RxFilterLength + ETH_ONE_8;
      /* Filter address added, change to filter mode */
      Eth_GenAddrFilterMode[LucCtrlIdx] = ETH_FILTER_MODE;
    }
    else
    {
      /* No action required */
    }
  }
  else
  {
    /* Filter Address array is full */
    LddReturnValue = E_NOT_OK;
  }

  return LddReturnValue;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
#endif

/*******************************************************************************
** Function Name         : Eth_RemoveFromFilter
**
** Service ID            : NA
**
** Description           : Remove an address from the Filter Address array
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : LucCtrlIdx - Index of the ETH Controller
**                         pFilterAddr - Filter Address need to be removed
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LddReturnValue
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GstControllerFilterAddr, Eth_GaaBaseAddr,
**                         Eth_GenAddrFilterMode
**
** Functions invoked     : Eth_CompareMac, Eth_CopyMacAddress
**
** Registers Used        : ECMR, FSO_CTL
**
*******************************************************************************/
#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_161] */
/* TRACE[ETH_DDD_INF_140] */
static FUNC(Std_ReturnType, ETH_CODE_SLOW) Eth_RemoveFromFilter
(
  uint8 LucCtrlIdx,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) pFilterAddr
)
{
  VAR(uint8, AUTOMATIC) LucRemoveElemIndex;
  VAR(uint8, AUTOMATIC) LucLastElemIndex;
  VAR(Std_ReturnType, AUTOMATIC) LddReturnValue;
  VAR(Std_ReturnType, AUTOMATIC) LddComparePatternAddr;
  VAR(uint8, AUTOMATIC) LucCount;

  LddReturnValue = E_OK;
  /* MISRA Violation: START Msg(4:2982)-9 */
  LddComparePatternAddr = E_NOT_OK;
  /* MISRA Violation: END Msg(4:2982)-9 */

  /* Check whether Filter Address array has any address */
  if (ETH_ZERO_8 < Eth_GstControllerFilterAddr[LucCtrlIdx].RxFilterLength)
  {
    /* Get the last element in filter array */
    LucLastElemIndex =
      Eth_GstControllerFilterAddr[LucCtrlIdx].RxFilterLength - ETH_ONE_8;
    /* Get the index of element in filter array to be removed */
    for (LucCount = ETH_ZERO_8; LucLastElemIndex >= LucCount; LucCount++)
    {
      LddComparePatternAddr = Eth_CompareMac(
        (const volatile uint8 *)(&Eth_GstControllerFilterAddr[LucCtrlIdx].
        RxFilterAddress[LucCount][ETH_ZERO_8]), pFilterAddr);
      if (E_OK == LddComparePatternAddr)
      {
        LucRemoveElemIndex = LucCount;
        break;
      }
      else
      {
        /* Continue searching */
      }
    }
    if (E_OK == LddComparePatternAddr)
    {
      /* Decrease the number of filter addresses */
      /* TRACE[ETH_DDD_ACT_161_GLB001] */
      Eth_GstControllerFilterAddr[LucCtrlIdx].RxFilterLength =
        Eth_GstControllerFilterAddr[LucCtrlIdx].RxFilterLength - ETH_ONE_8;
      if (LucRemoveElemIndex < LucLastElemIndex)
      {
        /* Overwrite the element to be removed with the last element */
        /* This keep the list of addresses continuous */
        LddReturnValue = Eth_CopyMacAddress(
          (volatile uint8 *)(&Eth_GstControllerFilterAddr[LucCtrlIdx].
          RxFilterAddress[LucRemoveElemIndex][ETH_ZERO_8]),
          (const volatile uint8 *)(&Eth_GstControllerFilterAddr[LucCtrlIdx].
          RxFilterAddress[LucLastElemIndex][ETH_ZERO_8]));
      }
      else
      {
        /* The last element is the element to be removed */
        /* No need to overwrite it */
      }
      if (E_OK == LddReturnValue)
      {
        /* Write Null Address in the last element */
        LddReturnValue = Eth_CopyMacAddress(
          (volatile uint8 *)(&Eth_GstControllerFilterAddr[LucCtrlIdx].
          RxFilterAddress[LucLastElemIndex][ETH_ZERO_8]), Eth_GaaNullAddr);
      }
      else
      {
        /* No action required */
      }
    }
    else
    {
      /* Can not find the address to be removed */
      LddReturnValue = E_NOT_OK;
    }
    if (ETH_ZERO_8 == Eth_GstControllerFilterAddr[LucCtrlIdx].RxFilterLength)
    {
      /* No address in the filter address array, return to normal mode */
      Eth_GenAddrFilterMode[LucCtrlIdx] = ETH_NORMAL_MODE;
    }
    else
    {
      /* No action */
    }
  }
  else
  {
    /* Filter Address array is empty */
    LddReturnValue = E_NOT_OK;
  }

  return LddReturnValue;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
#endif

/*******************************************************************************
** Function Name         : Eth_DMACReset
**
** Service ID            : 0x21
**
** Description           : Set AVB DMAC mode to reset mode
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
** Global Variables      : Eth_GaaBaseAddr, Eth_GenMainFunctionState,
**                         Eth_GenDriverState, Eth_GenControllerMode,
**                         Eth_GpConfigPtr
**
** Functions invoked     : Eth_DMACResetHW
**
** Registers Used        : CCC, CSR, FSO_CTL
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/* TRACE[ETH_DDD_ACT_178] */
FUNC(void, ETH_CODE_FAST) Eth_DMACReset(void)
{
  /* Set AVB DMAC mode to reset mode */
  (void)Eth_DMACResetHW(ETH_AVB0);
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-12 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-12 */
/*******************************************************************************
**                                 End of File                                **
*******************************************************************************/
