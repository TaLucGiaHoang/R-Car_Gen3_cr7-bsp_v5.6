/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Irq.h                                                   */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* Provision of prototypes for Interrupt service routine for ethernet driver. */
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
* V1.1.0:  08-Dec-2016 :  Added code to reported interrupt controller and
*                         Payload CRC check
*                         Implement EDC error detection
*                         Adding additional Interrupt handlers for testing
* V1.1.1:  08-Mar-2017  : Updated MISRA-C checking
* V1.1.2:  18-Jun-2017  : Add pre-compile option
*                         ETH_INTERRUPT_CTL_CHK_ERROR_DETECT to enable/disable
*                         unintended interrupt check
* V1.1.3:  08-Sep-2017  : Replace 'STATIC' keyword by 'static' keyword
* V1.1.4:  27-Nov-2017  : Move extern declaration of functions:
*                         Eth_RxIrqHdlr_01, Eth_TxIrqHdlr_01 from Eth.h to
*                         Eth_Irq.h
* V1.1.5:  15-Dec-2017    Move declaration of static function to source file
* V1.1.6:  15-Dec-2017  : Remove function Eth_GetRxQueueFullId
* V1.2.0:  23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
*******************************************************************************/

#ifndef ETH_IRQ_H
#define ETH_IRQ_H
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Included for version information macros */
#include "ether_api.h"
#include "Os.h"
/* Inclusion of RCar_Types.h file for FuSa related Macros */
#include "RCar_Types.h"
/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define ETH_IRQ_AR_RELEASE_MAJOR_VERSION    ETH_AR_RELEASE_MAJOR_VERSION
#define ETH_IRQ_AR_RELEASE_MINOR_VERSION    ETH_AR_RELEASE_MINOR_VERSION
#define ETH_IRQ_AR_RELEASE_REVISION_VERSION ETH_AR_RELEASE_REVISION_VERSION

/* Module Software version information */
#define ETH_IRQ_SW_MAJOR_VERSION    ETH_SW_MAJOR_VERSION
#define ETH_IRQ_SW_MINOR_VERSION    ETH_SW_MINOR_VERSION

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#define ETH_START_SEC_CODE_FAST
/* START Msg(4:5087)-1 */
#include "Eth_MemMap.h"
/* END Msg(4:5087)-1 */

#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
#if defined (Os_ETH_AVBCH00ISR_CAT2_ISR) || defined (ETH_AVBCH00ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh00Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH01ISR_CAT2_ISR) || defined (ETH_AVBCH01ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh01Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH02ISR_CAT2_ISR) || defined (ETH_AVBCH02ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh02Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH03ISR_CAT2_ISR) || defined (ETH_AVBCH03ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh03Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH04ISR_CAT2_ISR) || defined (ETH_AVBCH04ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh04Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH05ISR_CAT2_ISR) || defined (ETH_AVBCH05ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh05Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH06ISR_CAT2_ISR) || defined (ETH_AVBCH06ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh06Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH07ISR_CAT2_ISR) || defined (ETH_AVBCH07ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh07Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH08ISR_CAT2_ISR) || defined (ETH_AVBCH08ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh08Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH09ISR_CAT2_ISR) || defined (ETH_AVBCH09ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh09Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH10ISR_CAT2_ISR) || defined (ETH_AVBCH10ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh10Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH11ISR_CAT2_ISR) || defined (ETH_AVBCH11ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh11Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH12ISR_CAT2_ISR) || defined (ETH_AVBCH12ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh12Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH13ISR_CAT2_ISR) || defined (ETH_AVBCH13ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh13Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH14ISR_CAT2_ISR) || defined (ETH_AVBCH14ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh14Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH15ISR_CAT2_ISR) || defined (ETH_AVBCH15ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh15Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH16ISR_CAT2_ISR) || defined (ETH_AVBCH16ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh16Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH17ISR_CAT2_ISR) || defined (ETH_AVBCH17ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh17Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

#if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)
#if defined (Os_ETH_AVBCH18ISR_CAT2_ISR) || defined (ETH_AVBCH18ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh18Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH19ISR_CAT2_ISR) || defined (ETH_AVBCH19ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh19Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH20ISR_CAT2_ISR) || defined (ETH_AVBCH20ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh20Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH21ISR_CAT2_ISR) || defined (ETH_AVBCH21ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh21Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif
#endif /* (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON) */

#if defined (Os_ETH_AVBCH22ISR_CAT2_ISR) || defined (ETH_AVBCH22ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh22Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH23ISR_CAT2_ISR) || defined (ETH_AVBCH23ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh23Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if defined (Os_ETH_AVBCH24ISR_CAT2_ISR) || defined (ETH_AVBCH24ISR_CAT2_ISR)
/* Use ISR() macro from Os.h */
/* Defines the CAT1 interrupt mapping */
#else
extern _INTERRUPT_ FUNC(void, ETH_CODE_FAST) Eth_AvbCh24Isr(osal_device_handle_t device_handle,
                                                            uint64_t irq_channel, void* irq_arg);
#endif

#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
extern FUNC(void, ETH_CODE_FAST)Eth_ProcessRxInterrupt(uint8 LucRxQueueNo);
extern FUNC(void, ETH_CODE_FAST) Eth_RxIrqHdlr_01(uint8 Qidx);
#endif

#if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)
extern FUNC(void, ETH_CODE_FAST)Eth_ProcessTxInterrupt(uint8 LucTxQueueNo);
extern FUNC(void, ETH_CODE_FAST) Eth_TxIrqHdlr_01(void);
#endif

#define ETH_STOP_SEC_CODE_FAST
#include "Eth_MemMap.h"

#endif  /* ETH_IRQ_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
