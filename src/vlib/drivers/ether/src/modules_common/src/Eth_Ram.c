/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Ram.c                                                   */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* Global RAM variable definitions for Eth Driver an RAM allocation functions */
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
* V1.0.0:  08-Dec-2015  : Initial Version
* V1.0.1:  05-Aug-2016  : Remove Renesas specific number 59
*                         Unify version number
* V1.1.0:  16-Sep-2016  : Update Compiler Abstraction and Memory Mapping
*                         follow Specification of AUTOSAR Release 4.2.2
* V1.1.1:  08-Dec-2016  : Change Compiler Abstraction and Memory Mapping
* V1.2.0:  07-Mar-2017  : Updated source code to support static memory
*                         allocation for descriptors and buffers
*                         via linker script (#68800)
* V1.2.1:  08-Mar-2017  : Updated MISRA-C checking
* V1.2.2:  22-Mar-2017  : Removed Eth_GblTxConfirm (#77583)
* V1.2.3:  04-Apr-2017  : Removed Eth_GucCurrentRxQueueIdx, Eth_GblRxFrameValid
*                         Added Eth_GstControllerFilterAddr and
*                         Eth_GenAddrFilterMode
* V1.2.4:  05-Apr-2017  : Added Eth_GucRxMultiBuffFrame, Eth_GucPayloadCrc,
*                         Eth_RamInit
* V1.2.5:  13-May-2017  : Added Eth_GetTxBuffAddr, Eth_GetRxBuffAddr functions
* V1.2.6:  17-May-2017  : Removed Eth_GulLastCdaDescriptor
* V1.2.7:  17-Jun-2017  : Added volatile to global variables which can be
*                         accessed in multiple context
* V1.2.8:  18-Jun-2017    Update memory mapping for functions which are related
*                         to interrupt
* V1.2.9:  12-Sep-2017    Remove the definitions of Eth_GstRxConfig,
*                         Eth_GstAvbConfig, Eth_GstMacConfig, Eth_GstQConfig
*                         and Eth_RamInit
* V1.2.10: 27-Nov-2017  : Move global arrays Eth_GaaBroadcastAddr,
*                         Eth_GaaNullAddr definition
*                         from Eth.c to Eth_Ram.c
* V1.2.11: 15-Dec-2017    Update Compiler Abstraction
*                         Update casting to fix MISRA C Violations
*                         Replace static by STATIC
* V1.2.12: 23-Dec-2017  : Move Eth_GucRxBuff, Eth_GucTxBuff and
*                         Eth_GucRxMultiBuffFrame to DRAM cached align 32 bytes
* V1.2.13: 15-Jan-2018  : Update Memory Mapping of Global data
* V1.2.14: 26-Jan-2018  : Add branch process for M3.
* V1.2.15: 13-Mar-2018  : Fix Eth_RamCopy about unaligned data
* V1.2.16: 10-May-2018  : Add pre-processor for M3N
* V1.2.17: 11-Jun-2018  : Invalidate "Payload CRC" for M3
* V1.2.18: 19-Aug-2018  : Update MISRA C rule violations
*                         Invalidate "Payload CRC" for M3N
* V1.2.19: 28-Nov-2019  : Add comment for MISRA C rule violation message
*                         (2:1514)
* V1.2.20: 23-Apr-2020  : Add MISRA C rule violation message Msg(4:5087) and
*                         remove Msg(2:1514)
* V1.3.0:  23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Included for Eth.h inclusion and macro definitions */
/* Header file inclusion */
#include "Eth_Ram.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define ETH_RAM_C_AR_RELEASE_MAJOR_VERSION \
  ETH_AR_RELEASE_MAJOR_VERSION_VALUE
#define ETH_RAM_C_AR_RELEASE_MINOR_VERSION \
  ETH_AR_RELEASE_MINOR_VERSION_VALUE
#define ETH_RAM_C_AR_RELEASE_REVISION_VERSION \
  ETH_AR_RELEASE_REVISION_VERSION_VALUE

/* File version information */
#define ETH_RAM_C_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION_VALUE
#define ETH_RAM_C_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION_VALUE

/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/

#if (ETH_RAM_AR_RELEASE_MAJOR_VERSION != \
  ETH_RAM_C_AR_RELEASE_MAJOR_VERSION)
#error "Eth_Ram.c : Mismatch in Release Major Version"
#endif
#if (ETH_RAM_AR_RELEASE_MINOR_VERSION != \
  ETH_RAM_C_AR_RELEASE_MINOR_VERSION)
#error "Eth_Ram.c : Mismatch in Release Minor Version"
#endif
#if (ETH_RAM_AR_RELEASE_REVISION_VERSION != \
  ETH_RAM_C_AR_RELEASE_REVISION_VERSION)
#error "Eth_Ram.c : Mismatch in Release Revision Version"
#endif

#if (ETH_RAM_SW_MAJOR_VERSION != ETH_RAM_C_SW_MAJOR_VERSION)
#error "Eth_Ram.c : Mismatch in Software Major Version"
#endif
#if (ETH_RAM_SW_MINOR_VERSION != ETH_RAM_C_SW_MINOR_VERSION)
#error "Eth_Ram.c : Mismatch in Software Minor Version"
#endif

/*******************************************************************************
**                  MISRA C Rule Violations                                   **
*******************************************************************************/
/* 1. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0306) [I] Cast between a pointer to object and an       */
/*                 integral type.                                             */
/* Rule          : MISRA-C:2004 Rule 3.1, 11.3                                */
/*                 REFERENCE - ISO:C90-6.3.4 Cast Operators - Semantics       */
/* Justification : The casting is necessary to access descriptor and buffer   */
/*                 of ETH driver                                              */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0306)-1 and                           */
/*                 END Msg(4:0306)-1 tags in the code.                        */
/******************************************************************************/
/* 2. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0310) Casting to different object pointer type.         */
/* Rule          : MISRA-C:2004 Rule 11.4                                     */
/*                 REFERENCE - ISO:C90-6.3.4 Cast Operators - Semantics       */
/* Justification : This is used to archive better throughput when copying     */
/*                 data                                                       */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0310)-2 and                           */
/*                 END Msg(4:0310)-2 tags in the code.                        */
/******************************************************************************/
/* 3. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0491) Array subscripting applied to an object of        */
/*                 pointer type.                                              */
/* Rule          : MISRA-C:2004 Rule 17.4                                     */
/*                 REFERENCE - ISO:C90-6.3.2.1 Array Subscripting             */
/*                             ISO:C90 6.2.2.1 Lvalues and Function           */
/*                                     Designators                            */
/*                             ISO:C90 6.7.1 Function Definitions - Semantics */
/* Justification : This is perfectly legitimate in C as Array                 */
/*                 and pointer are substantially the same thing               */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0491)-3 and                           */
/*                 END Msg(4:0491)-3 tags in the code.                        */
/******************************************************************************/
/* 4. MISRA C RULE VIOLATION:                                                 */
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
/* Reference     : Look for START Msg(4:0492)-4 and                           */
/*                 END Msg(4:0492)-4 tags in the code.                        */
/******************************************************************************/
/* 5. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:0612) [L] Size of object '%s' exceeds 32767 bytes -     */
/*                 program does not conform strictly to ISO:C90.              */
/* Rule          : MISRA-C:2004 Rule 1.1                                      */
/*                 REFERENCE - ISO:C90-5.2.4.1 Translation Limits             */
/* Justification : size of Rx/Tx descriptor and buffer to too large to cover  */
/*                 AUTOSAR requirement and HW feature, ETH driver support many*/
/*                 queues and buffers                                         */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:0612)-5 and                           */
/*                 END Msg(4:0612)-5 tags in the code.                        */
/******************************************************************************/
/* 6. MISRA C RULE VIOLATION:                                                 */
/* Message       : (2:2824) Possible: Arithmetic operation on NULL pointer.   */
/* Rule          : None                                                       */
/* Justification : The pointer cannot be NULL because:                        */
/*                 + NULL pointer has been checked before setting the address */
/*                 for pointer.                                               */
/*                 + The address which is used to set pointer is address of   */
/*                 register or global variable. All of them are not NULL      */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(2:2824)-6 and                           */
/*                 END Msg(2:2824)-6 tags in the code.                        */
/******************************************************************************/
/* 7. MISRA C RULE VIOLATION:                                                 */
/* Message       : (4:2981) This initialization is redundant. The value of    */
/*                 this object is never used before being modified.           */
/* Rule          : MISRA-C:2004 Rule 21.1                                     */
/* Justification : Assign the initialized value for local variables, it is    */
/*                 used to avoid the case that local variable is used before  */
/*                 setting                                                    */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:2981)-7 and                           */
/*                 END Msg(4:2981)-7 tags in the code.                        */
/******************************************************************************/
/* 8. MISRA C RULE VIOLATION:                                                 */
/* Message       : (3:3305) Pointer cast to stricter alignment.               */
/* Rule          : None                                                       */
/* Justification : This is used to archive better throughput when copying     */
/*                 data                                                       */
/* Verification  : All violations of this message were verified and reported  */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(3:3305)-8 and                           */
/*                 END Msg(3:3305)-8 tags in the code.                        */
/******************************************************************************/
/* 10. MISRA C RULE VIOLATION:                                                */
/* Message       : (4:5087) #include statements in a file should only be      */
/*                  preceded by other preprocessor directives or comments     */
/* Rule          : MISRA-C:2004 Rule 19.1                                     */
/* Justification : include "Fls_MemMap.h" searches for defined symbol to map  */
/*                 memory section in memory mapping. This "include" should be */
/*                 added before and after the definition of memory section    */
/*                 This implementation is correct                             */
/* Verification  : Violation of this message were verified and reported       */
/*                 There are no impacts                                       */
/* Reference     : Look for START Msg(4:5087)-10 and                          */
/*                 END Msg(4:5087)-10 tags in the code.                       */
/******************************************************************************/

/*******************************************************************************
**                      Global Data                                           **
*******************************************************************************/

#define ETH_START_SEC_VAR_CLEARED_UNSPECIFIED
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
/* Use to store a state change to notify upper layer */
volatile VAR(Eth_MainFunctionType, ETH_VAR_CLEARED)
  Eth_GenMainFunctionState;
#define ETH_STOP_SEC_VAR_CLEARED_UNSPECIFIED
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */

#define ETH_START_SEC_VAR_INIT_UNSPECIFIED
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
/* Global variable to store Controller State */
volatile VAR(Eth_StateType, ETH_VAR_INIT) Eth_GenDriverState = ETH_STATE_UNINIT;
#define ETH_STOP_SEC_VAR_INIT_UNSPECIFIED
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */

#define ETH_START_SEC_VAR_INIT_UNSPECIFIED
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
/*
* Global variable to store Initialization status of Ethernet Driver Component
*/
volatile VAR(Eth_ModeType, ETH_VAR_INIT) Eth_GenControllerMode =
  ETH_MODE_DOWN;
#define ETH_STOP_SEC_VAR_INIT_UNSPECIFIED
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */

#define ETH_START_SEC_VAR_CLEARED_32
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
/* Global variable to store pointer to Configuration */
P2CONST(Eth_ConfigType, ETH_VAR_CLEARED, ETH_APPL_CONST)
  Eth_GpConfigPtr;
#define ETH_STOP_SEC_VAR_CLEARED_32
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */

#define ETH_START_SEC_VAR_CLEARED_32
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
volatile VAR(Eth_GaaDeviceType, ETH_VAR_CLEARED)
  Eth_GaaDevice[ETH_MACRO_NUM];
#define ETH_STOP_SEC_VAR_CLEARED_32
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */

#define ETH_START_SEC_VAR_CLEARED_32
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
volatile VAR(Eth_BufHandlerType, ETH_VAR_CLEARED)
  Eth_GstBufferLock[ETH_TX_BUF_TOTAL];
#define ETH_STOP_SEC_VAR_CLEARED_32
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */

#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
#define ETH_START_SEC_VAR_CLEARED_8
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
volatile VAR(Eth_RxFilterAddrType, ETH_VAR_CLEARED)
  Eth_GstControllerFilterAddr[ETH_MAX_CTRLS_SUPPORTED];
#define ETH_STOP_SEC_VAR_CLEARED_8
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
#endif

#define ETH_START_SEC_VAR_CLEARED_UNSPECIFIED
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
volatile VAR(Eth_FilterModeType, ETH_VAR_CLEARED)
  Eth_GenAddrFilterMode[ETH_MAX_CTRLS_SUPPORTED];
#endif
#define ETH_STOP_SEC_VAR_CLEARED_UNSPECIFIED
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */

#define ETH_START_SEC_VAR_CLEARED_8
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
/* Flag to indicate the payload CRC status */
#if (ETH_DEVICE_NAME == RCAR_H3)
#if (ETH_PAYLOAD_CRC_ERROR_DETECT == STD_ON)
volatile VAR(uint8, ETH_VAR_CLEARED)
  Eth_GaaPayloadCrc[ETH_RX_QUEUE_COUNT];
#endif
#endif
#define ETH_STOP_SEC_VAR_CLEARED_8
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */

#define ETH_START_SEC_VAR_CLEARED_DESCRIPTOR_32
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
VAR(Eth_DataDescType, ETH_VAR_CLEARED_DESCRIPTOR)
  Eth_GstDescTable[ETH_TXQ_MAXNUM + ETH_RXQ_MAXNUM];
#if (ETH_RX_BEQUEUE_TIMESTAMP == STD_OFF)
VAR(Eth_DataDescType, ETH_VAR_CLEARED_DESCRIPTOR)
  Eth_GstRxBeQueue[ETH_RX_BUF_TOTAL + ETH_EXTRA_FOR_LINKDESC];
#else
VAR(Eth_ExtDataDescType, ETH_VAR_CLEARED_DESCRIPTOR)
  Eth_GstRxBeQueue[ETH_RX_BUF_TOTAL + ETH_EXTRA_FOR_LINKDESC];
#endif
#if (ETH_RX_NCQUEUE_NUM != ETH_QUEUE_SIZE_ZERO)
VAR(Eth_ExtDataDescType, ETH_VAR_CLEARED_DESCRIPTOR)
  Eth_GstRxNcQueue[ETH_RX_BUF_TOTAL + ETH_EXTRA_FOR_LINKDESC];
#endif
#if (ETH_RX_SQUEUE_NUM != ETH_QUEUE_SIZE_ZERO)
#if (ETH_RX_SQUEUE_TIMESTAMP == STD_OFF)
VAR(Eth_DataDescType, ETH_VAR_CLEARED_DESCRIPTOR)
  Eth_GstRxStreamQueue[ETH_RX_SQUEUE_NUM]
  [ETH_RX_BUF_TOTAL + ETH_EXTRA_FOR_LINKDESC];
#else
VAR(Eth_ExtDataDescType, ETH_VAR_CLEARED_DESCRIPTOR)
  /* MISRA Violation: START Msg(4:0612)-5 */
  Eth_GstRxStreamQueue[ETH_RX_SQUEUE_NUM]
  /* MISRA Violation: END Msg(4:0612)-5 */
  [ETH_RX_BUF_TOTAL + ETH_EXTRA_FOR_LINKDESC];
#endif
#endif
#if (ETH_TX_QUEUE_COUNT != ETH_QUEUE_SIZE_ZERO)
VAR(Eth_DataDescType, ETH_VAR_CLEARED_DESCRIPTOR)
  Eth_GstTxQueue[ETH_TX_QUEUE_COUNT]
  [ETH_TX_BUF_TOTAL + ETH_EXTRA_FOR_EOSDESC + ETH_EXTRA_FOR_LINKDESC];
#endif
#define ETH_STOP_SEC_VAR_CLEARED_DESCRIPTOR_32
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */

#define ETH_START_SEC_VAR_CLEARED_USER_RAM_UNSPECIFIED
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
/* MISRA Violation: START Msg(4:0612)-5 */
/* MISRA Violation: START Msg(2:1514)-9 */
/*VAR(uint8, ETH_VAR_CLEARED) Eth_GucRxBuff*/
/* MISRA Violation: END Msg(2:1514)-9 */
/* MISRA Violation: END Msg(4:0612)-5 */
/*  [ETH_RX_BUF_SIZE_CACHE_ALIGN];*/
VAR(uint8, ETH_VAR_CLEARED) *Eth_GucRxBuff;
/*VAR(uint8, ETH_VAR_CLEARED)*/
  /* MISRA Violation: START Msg(4:0612)-5 */
  /* MISRA Violation: START Msg(2:1514)-9 */
/*  Eth_GucTxBuff[ETH_TX_BUF_SIZE_CACHE_ALIGN];*/
VAR(uint8, ETH_VAR_CLEARED) *Eth_GucTxBuff;
  /* MISRA Violation: END Msg(2:1514)-9 */
  /* MISRA Violation: END Msg(4:0612)-5 */
VAR(uint8, ETH_VAR_CLEARED)
  Eth_GucRxMultiBuffFrame[ETH_RX_QUEUE_COUNT][ETH_VLFRAME_SIZE_CACHE_ALIGN];
#define ETH_STOP_SEC_VAR_CLEARED_USER_RAM_UNSPECIFIED
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */

#define ETH_START_SEC_VAR_CLEARED_32
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
#if (ETH_GLOBAL_TIME_SUPPORT == STD_ON )
/* This is the address of the Descriptor which has a TimeStamp just received */
P2VAR(volatile Eth_ExtDataDescType, ETH_VAR_CLEARED, ETH_APPL_DATA)
  Eth_GaaTempTsDescr[ETH_RX_QUEUE_COUNT];
/* This is the TimeStamp just read and stored */
volatile VAR(Eth_TimeStampStoreType, ETH_VAR_CLEARED)
  Eth_GaaTempTimeStamp[ETH_RX_QUEUE_COUNT];
#endif
#define ETH_STOP_SEC_VAR_CLEARED_32
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */

#define ETH_START_SEC_VAR_CLEARED_8
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
#if (ETH_GLOBAL_TIME_SUPPORT == STD_ON )
/* This stores the Queue index of TimeStamp */
volatile VAR(uint8, ETH_VAR_CLEARED) Eth_GucTsQueueIdx;
#endif
#define ETH_STOP_SEC_VAR_CLEARED_8
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */

#define ETH_START_SEC_CONST_8
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
CONST(uint8, ETH_CONST) Eth_GaaBroadcastAddr[ETH_MACADDR_SIZE] =
{
  ETH_OCTET_MAX_VAL,
  ETH_OCTET_MAX_VAL,
  ETH_OCTET_MAX_VAL,
  ETH_OCTET_MAX_VAL,
  ETH_OCTET_MAX_VAL,
  ETH_OCTET_MAX_VAL
};

#if (ETH_UPDATE_PHYS_ADDR_FILTER == STD_ON)
CONST(uint8, ETH_CONST) Eth_GaaNullAddr[ETH_MACADDR_SIZE] =
{
  ETH_ZERO_8, ETH_ZERO_8, ETH_ZERO_8, ETH_ZERO_8, ETH_ZERO_8, ETH_ZERO_8
};
#endif
#define ETH_STOP_SEC_CONST_8
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */

/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/

/*******************************************************************************
** Function Name         : Eth_RamCopy
**
** Service ID            : NA
**
** Description           : This perform memory copy used for ETH driver
**
** Sync/Async            : Synchronous
**
** Re-entrancy           : Non Reentrant
**
** Input Parameters      : pSrc - Address of the data to be copied
**                         ulSize - Size in byte of copied data
**
** InOutput Parameters   : pDst - Address where the data need to be copied to
**
** Output Parameters     : None
**
** Return parameter      : None
**
** Preconditions         : None
**
** Global Variable(s)    : None
**
** Functions invoked     : None
**
** Registers Used        : None
**
*******************************************************************************/
#define ETH_START_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
/* TRACE[ETH_DDD_ACT_171] */
/* TRACE[ETH_DDD_INF_144] */
FUNC(void, ETH_CODE_FAST) Eth_RamCopy
(
  CONSTP2VAR(uint8, AUTOMATIC, ETH_APPL_DATA) pDst,
  CONSTP2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) pSrc,
  VAR(uint32, AUTOMATIC) ulSize
)
{
  /* Local pointer for copy 4 byte */
  P2CONST(uint32, AUTOMATIC, ETH_APPL_DATA) LpSrc_32 = \
    /* MISRA Violation: START Msg(3:3305)-8 */
    /* MISRA Violation: START Msg(4:2981)-7 */
    /* MISRA Violation: START Msg(4:0310)-2 */
    (P2CONST(uint32, AUTOMATIC, ETH_APPL_DATA)) pSrc;
    /* MISRA Violation: END Msg(4:0310)-2 */
    /* MISRA Violation: END Msg(4:2981)-7 */
    /* MISRA Violation: END Msg(3:3305)-8 */

  /* Local pointer for copy 4 byte */
  P2VAR(uint32, AUTOMATIC, ETH_APPL_DATA) LpDst_32 = \
    /* MISRA Violation: START Msg(3:3305)-8 */
    /* MISRA Violation: START Msg(4:2981)-7 */
    /* MISRA Violation: START Msg(4:0310)-2 */
    (P2VAR(uint32, AUTOMATIC, ETH_APPL_DATA)) pDst;
    /* MISRA Violation: END Msg(4:0310)-2 */
    /* MISRA Violation: END Msg(4:2981)-7 */
    /* MISRA Violation: END Msg(3:3305)-8 */

  VAR(uint8, AUTOMATIC) LucUnalignedBytes;
  VAR(uint32, AUTOMATIC) LulCnt;
  VAR(uint32, AUTOMATIC) LulAlignedBytes;

  /* Check if both src and dst have same alignment */
  /* MISRA Violation: START Msg(4:0306)-1 */
  if (((uint32) pDst % ETH_MEMORY_ALIGN_SIZE) == \
  /* MISRA Violation: END Msg(4:0306)-1 */
    /* MISRA Violation: START Msg(4:0306)-1 */
    ((uint32) pSrc % ETH_MEMORY_ALIGN_SIZE))
    /* MISRA Violation: END Msg(4:0306)-1 */
  {
    /* Check if address is unaligned, if not calculate the unaligned byte with
     * will be copy first */
    /* MISRA Violation: START Msg(4:0306)-1 */
    if (ETH_ZERO_32 == (((uint32) pSrc) % ETH_MEMORY_ALIGN_SIZE))
    /* MISRA Violation: END Msg(4:0306)-1 */
    {
      LucUnalignedBytes = ETH_ZERO_8;
    }
    else
    {
      LucUnalignedBytes = (uint8) (ETH_MEMORY_ALIGN_SIZE -
        /* MISRA Violation: START Msg(4:0306)-1 */
        ((uint32) pSrc % ETH_MEMORY_ALIGN_SIZE));
        /* MISRA Violation: END Msg(4:0306)-1 */
    }

    /* Calculate number of aligned byte */
    LulAlignedBytes = ((ulSize - LucUnalignedBytes) /
      (ETH_MEMORY_ALIGN_SIZE * ETH_UNROLL_LOOP)) *
         (ETH_MEMORY_ALIGN_SIZE * ETH_UNROLL_LOOP);

    /* Copy unaligned byte of top adress */
    for (LulCnt = ETH_ZERO_32; LulCnt < LucUnalignedBytes; LulCnt++)
    {
      /* MISRA Violation: START Msg(4:0492)-4 */
      /* MISRA Violation: START Msg(4:0492)-4 */
      pDst[LulCnt] = pSrc[LulCnt];
      /* MISRA Violation: END Msg(4:0492)-4 */
      /* MISRA Violation: END Msg(4:0492)-4 */
    }

    /* Update address for 4 byte copy */
    /* MISRA Violation: START Msg(3:3305)-8 */
    /* MISRA Violation: START Msg(4:0310)-2 */
    LpDst_32 = (P2VAR(uint32, AUTOMATIC, ETH_APPL_DATA))
    /* MISRA Violation: END Msg(4:0310)-2 */
    /* MISRA Violation: END Msg(3:3305)-8 */
      /* MISRA Violation: START Msg(4:0492)-4 */
      &(pDst[LucUnalignedBytes]);
      /* MISRA Violation: END Msg(4:0492)-4 */
    /* MISRA Violation: START Msg(3:3305)-8 */
    /* MISRA Violation: START Msg(4:0310)-2 */
    LpSrc_32 = (P2CONST(uint32, AUTOMATIC, ETH_APPL_DATA))
    /* MISRA Violation: END Msg(4:0310)-2 */
    /* MISRA Violation: END Msg(3:3305)-8 */
      /* MISRA Violation: START Msg(4:0492)-4 */
      &(pSrc[LucUnalignedBytes]);
      /* MISRA Violation: END Msg(4:0492)-4 */

    /* Copy aligned data */
    for (LulCnt = ETH_ZERO_32;
      LulCnt < (LulAlignedBytes / ETH_MEMORY_ALIGN_SIZE);
        LulCnt += ETH_UNROLL_LOOP)
    {
      /* Unroll for loop */
      /* MISRA Violation: START Msg(2:2824)-6 */
      /* MISRA Violation: START Msg(4:0491)-3 */
      /* MISRA Violation: START Msg(2:2824)-6 */
      /* MISRA Violation: START Msg(4:0491)-3 */
      LpDst_32[LulCnt] = LpSrc_32[LulCnt];
      /* MISRA Violation: END Msg(4:0491)-3 */
      /* MISRA Violation: END Msg(2:2824)-6 */
      /* MISRA Violation: END Msg(4:0491)-3 */
      /* MISRA Violation: END Msg(2:2824)-6 */
      /* MISRA Violation: START Msg(4:0491)-3 */
      /* MISRA Violation: START Msg(4:0491)-3 */
      LpDst_32[LulCnt + ETH_ONE_32] = LpSrc_32[LulCnt + ETH_ONE_32];
      /* MISRA Violation: END Msg(4:0491)-3 */
      /* MISRA Violation: END Msg(4:0491)-3 */
      /* MISRA Violation: START Msg(4:0491)-3 */
      /* MISRA Violation: START Msg(4:0491)-3 */
      LpDst_32[LulCnt + ETH_TWO_32] = LpSrc_32[LulCnt + ETH_TWO_32];
      /* MISRA Violation: END Msg(4:0491)-3 */
      /* MISRA Violation: END Msg(4:0491)-3 */
      /* MISRA Violation: START Msg(4:0491)-3 */
      /* MISRA Violation: START Msg(4:0491)-3 */
      LpDst_32[LulCnt + ETH_THREE_32] = LpSrc_32[LulCnt + ETH_THREE_32];
      /* MISRA Violation: END Msg(4:0491)-3 */
      /* MISRA Violation: END Msg(4:0491)-3 */
    }

    /* Copy remained data */
    for (LulCnt = LucUnalignedBytes + LulAlignedBytes; LulCnt < ulSize;
      LulCnt++)
    {
      /* MISRA Violation: START Msg(4:0492)-4 */
      /* MISRA Violation: START Msg(4:0492)-4 */
      pDst[LulCnt] = pSrc[LulCnt];
      /* MISRA Violation: END Msg(4:0492)-4 */
      /* MISRA Violation: END Msg(4:0492)-4 */
    }
  }
  /* End of if (((uint32) pDst % ETH_MEMORY_ALIGN_SIZE) ==  *
   * ((uint32) pSrc % ETH_MEMORY_ALIGN_SIZE))               */
  else
  {
    /* Copy each 1 byte in case of src and dst do not have the same alignment */
    for (LulCnt = ETH_ZERO_32; LulCnt < ulSize; LulCnt++)
    {
      /* MISRA Violation: START Msg(4:0492)-4 */
      /* MISRA Violation: START Msg(4:0492)-4 */
      pDst[LulCnt] = pSrc[LulCnt];
      /* MISRA Violation: END Msg(4:0492)-4 */
      /* MISRA Violation: END Msg(4:0492)-4 */
    }
  }
}
#define ETH_STOP_SEC_CODE_FAST
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */

/*******************************************************************************
** Function Name         : Eth_GetTxBuffAddr
**
** Service ID            : NA
**
** Description           : This gets the address of corresponding Tx buffer
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : ulBuffIdx - Index of Tx buffer
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LulTxBuffAddr: Address of corresponding Tx buffer
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GucTxBuff
**
** Function(s) invoked   : None
**
** Registers Used        : None
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
/* TRACE[ETH_DDD_ACT_192] */
/* TRACE[ETH_DDD_INF_150] */
FUNC(uint32, ETH_CODE_SLOW) Eth_GetTxBuffAddr
(
  VAR(uint32, AUTOMATIC) ulBuffIdx
)
{
  VAR(uint32, AUTOMATIC) LulTxBuffAddr;

  LulTxBuffAddr =
    /* MISRA Violation: START Msg(4:0306)-1 */
    (uint32)(&Eth_GucTxBuff[(ulBuffIdx) *
    /* MISRA Violation: END Msg(4:0306)-1 */
    (uint32)ETH_CTRL_TX_BUF_LEN_CACHE_ALIGN]);

  return LulTxBuffAddr;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */

/*******************************************************************************
** Function Name         : Eth_GetRxBuffAddr
**
** Service ID            : NA
**
** Description           : This gets the address of corresponding Rx buffer
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Non Reentrant
**
** Input Parameters      : ucQueueIdx - Index of Rx Queue
**                         ucBuffIdx - Index of Rx buffer
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return parameter      : LulRxBuffAddr: Address of corresponding Rx buffer
**
** Preconditions         : None
**
** Global Variable(s)    : Eth_GucRxBuff
**
** Function(s) invoked   : None
**
** Registers Used        : None
*******************************************************************************/
#define ETH_START_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
/* TRACE[ETH_DDD_ACT_180] */
/* TRACE[ETH_DDD_INF_151] */
FUNC(uint32, ETH_CODE_SLOW) Eth_GetRxBuffAddr
(
  VAR(uint8, AUTOMATIC) ucQueueIdx,
  VAR(uint8, AUTOMATIC) ucBuffIdx
)
{
  VAR(uint32, AUTOMATIC) LulRxBuffAddr;

  /* MISRA Violation: START Msg(4:0306)-1 */
  LulRxBuffAddr = (uint32)(&Eth_GucRxBuff[(((ucQueueIdx) *
  /* MISRA Violation: END Msg(4:0306)-1 */
    (uint32)ETH_RX_BUF_TOTAL) +
      (uint32)(ucBuffIdx)) * (uint32)ETH_CTRL_RX_BUF_LEN_CACHE_ALIGN]);

  return LulRxBuffAddr;
}
#define ETH_STOP_SEC_CODE_SLOW
/* MISRA Violation: START Msg(4:5087)-10 */
#include "Eth_MemMap.h"
/* MISRA Violation: END Msg(4:5087)-10 */
/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/

