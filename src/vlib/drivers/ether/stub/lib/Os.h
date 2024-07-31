/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Os.h                                                        */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2018 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file is a stub for OS component                                       */
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
/*              Devices:        R-Car V3M                                     */
/*                              R-Car H3/M3/M3N                               */
/*============================================================================*/
/*
 * 1.0.0      30-Oct-2015      Initial Version
 * 1.1.0      14-Sep-2016      Update Compiler Abstraction and Memory Mapping
                               follow Specification of AUTOSAR Release 4.2.2
 * 1.2.0      07-Dec-2016      Add GetElapsedValue stub prototype.
 *                             Change memory class of definition to TYPEDEF
 *                             Add EnableAllInterrupts, DisableAllInterrupts
 * 1.3.0      07-Dec-2017      Update stub prototype of GetElapsedValue.
 *                             Add COUNTER_MAX_VALUE.
 *                             Add prototype for Os_Counter_Init.
 * 1.4.0      04-Jan-2018      Update device name
 * 1.4.1      24-Jan-2018      Add device name M3 about comment.
 * 1.4.2      16-Nov-2018      Add device name M3N about comment.
 */

#ifndef OS_H
#define OS_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Std_Types.h"
/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define OS_AR_RELEASE_MAJOR_VERSION    4
#define OS_AR_RELEASE_MINOR_VERSION    2
#define OS_AR_RELEASE_REVISION_VERSION 2

/* Module Software version information */
#define OS_SW_MAJOR_VERSION    4
#define OS_SW_MINOR_VERSION    0
#define OS_SW_PATCH_VERSION    0
/*******************************************************************************
**                      Macro                                                 **
*******************************************************************************/
#define ISR(X) void OS_ISR_##X(void)
/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
#define OsCounter0              (uint8)0u
#define COUNTER_MAX_VALUE       (uint32)4294967295
/* Maximum times which timeout is not updated by Os */
#define OS_COMMON_MIRROR_TIMEOUT_MAX     (uint8)10
/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/
/* OS ID Type */
typedef uint8 CounterType;
/* OS Tick reference type */
typedef uint32 TickType;
typedef P2VAR(TickType, TYPEDEF, OS_APPL_DATA) TickRefType;

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

extern StatusType GetCounterValue( CounterType CounterID, TickRefType Value );
extern StatusType GetElapsedValue(
  CounterType CounterID,
  TickRefType Value,
  TickRefType ElapsedValue );
extern void Os_Counter_Init(void);

extern void EnableAllInterrupts(void);
extern void DisableAllInterrupts(void);
#endif /* OS_H */
/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
