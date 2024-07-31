/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = EcuM_Cbk.h                                                  */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2018 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file is a stub for EcuM Component                                     */
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
/*              Devices:        R-Car V3M                                     */
/*                              R-Car H3/M3                                   */
/*============================================================================*/
/*
 * 1.0.0      30-Oct-2015      Initial Version
 * 1.0.1      04-Jan-2018      Update device name
 * 1.0.2      24-Jan-2018      Add device name M3 about comment.
 * 1.0.3      29-Jun-2018      Add macro for EcuMWakeupSource
 */

/*******************************************************************************
**                         Input File                                         **
*******************************************************************************/
#ifndef ECUM_CBK_H
#define ECUM_CBK_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Std_Types.h"     /* To include AUTOSAR standard types */
#include "EcuM_Types.h"
#include "EcuM_Cfg.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/*
 * AUTOSAR specification version information
 */
#define ECUM_CBK_AR_RELEASE_MAJOR_VERSION  4
#define ECUM_CBK_AR_RELEASE_MINOR_VERSION  2
#define ECUM_CBK_AR_RELEASE_REVISION_VERSION  2
/*
 * File version information
 */
#define ECUM_CBK_SW_MAJOR_VERSION    4
#define ECUM_CBK_SW_MINOR_VERSION    0
#define ECUM_CBK_SW_PATCH_VERSION    0

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

#define  CAN_WKP_SRC_1               (EcuM_WakeupSourceType)2
#define  CAN_WKP_SRC_2               (EcuM_WakeupSourceType)4
#define  CAN_WKP_SRC_3               (EcuM_WakeupSourceType)6
#define  CAN_WKP_SRC_4               (EcuM_WakeupSourceType)8

#define  EcumWkSourcePower           (EcuM_WakeupSourceType)0
#define  EcumWkSourceReset           (EcuM_WakeupSourceType)1
#define  EcuM_EcuMWakeupSource_0     (EcuM_WakeupSourceType)0x00000001ul
#define  EcuM_EcuMWakeupSource_1     (EcuM_WakeupSourceType)0x00000002ul
#define  EcuM_EcuMWakeupSource_2     (EcuM_WakeupSourceType)0x00000004ul
#define  EcuM_EcuMWakeupSource_3     (EcuM_WakeupSourceType)0x00000008ul
#define  EcuM_EcuMWakeupSource_4     (EcuM_WakeupSourceType)0x00000010ul
#define  EcuM_EcuMWakeupSource_5     (EcuM_WakeupSourceType)0x00000020ul
#define  EcuM_EcuMWakeupSource_6     (EcuM_WakeupSourceType)0x00000040ul
#define  EcuM_EcuMWakeupSource_7     (EcuM_WakeupSourceType)0x00000080ul
#define  EcuM_EcuMWakeupSource_8     (EcuM_WakeupSourceType)0x00000100ul
#define  EcuM_EcuMWakeupSource_9     (EcuM_WakeupSourceType)0x00000200ul
#define  EcuM_EcuMWakeupSource_10    (EcuM_WakeupSourceType)0x00000400ul
#define  EcuM_EcuMWakeupSource_11    (EcuM_WakeupSourceType)0x00000800ul
#define  EcuM_EcuMWakeupSource_12    (EcuM_WakeupSourceType)0x00001000ul
#define  EcuM_EcuMWakeupSource_13    (EcuM_WakeupSourceType)0x00002000ul
#define  EcuM_EcuMWakeupSource_14    (EcuM_WakeupSourceType)0x00004000ul

#define  GPT_WKP_SRC_1               (EcuM_WakeupSourceType)8
#define  GPT_WKP_SRC_2               (EcuM_WakeupSourceType)4
#define  GPT_WKP_SRC_3               (EcuM_WakeupSourceType)6
#define  GPT_WKP_SRC_4               (EcuM_WakeupSourceType)2

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
extern void EcuM_SetWakeupEvent(EcuM_WakeupSourceType WakeupSource);
extern void EcuM_ValidateWakeupEvent(EcuM_WakeupSourceType WakeupSource);
extern void EcuM_CheckWakeupEvent(EcuM_WakeupSourceType WakeupSource);
extern void EcuM_CheckWakeup(EcuM_WakeupSourceType WakeupSource);

#endif /* ECUM_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
