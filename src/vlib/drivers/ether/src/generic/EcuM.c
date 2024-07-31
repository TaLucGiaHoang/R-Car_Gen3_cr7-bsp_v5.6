/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = EcuM.c                                                      */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2020 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file is a source file for stub for EcuM Component                     */
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
/*                              R-Car V3H                                     */
/*============================================================================*/
/*
 * 1.0.0      30-Oct-2015      Initial Version
 * 1.0.1      04-Jan-2018      Update device name
 * 1.0.2      24-Jan-2018      Add device name M3 about comment.
 * 1.0.3      29-Jun-2018      Replace macro value in switch case of stub
 *                             functions EcuM_SetWakeupEvent, EcuM_CheckWakeup.
 * 1.0.4      17-Apr-2020      Add dummy code in EcuM_CheckWakeupEvent and
 *                             EcuM_ValidateWakeupEvent to avoid
 *                             unused parameter.
 */

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Std_Types.h"
#include "EcuM.h"
#include "EcuM_Cbk.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/

/*******************************************************************************
**                         Global Data                                        **
*******************************************************************************/
volatile uint8 GaaNotificationEcuMEntered[15];
volatile uint8 GaaEcuMWakeUpEvent[15];


/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/

/*******************************************************************************
**              Check Wakeup Indication                                       **
*******************************************************************************/

void EcuM_CheckWakeupEvent(EcuM_WakeupSourceType WakeupSource)
{
  (void)WakeupSource;
}


/*******************************************************************************
**              Wakeup Validation                                             **
*******************************************************************************/
void EcuM_ValidateWakeupEvent(EcuM_WakeupSourceType WakeupSource)
{
  (void)WakeupSource;
}

/*******************************************************************************
**              Setting the Wakeup                                            **
*******************************************************************************/

void EcuM_SetWakeupEvent(EcuM_WakeupSourceType WakeupSource)
{
  switch (WakeupSource)
  {
    case EcuM_EcuMWakeupSource_0:
      GaaEcuMWakeUpEvent[0]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_1:
      GaaEcuMWakeUpEvent[1]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_2:
      GaaEcuMWakeUpEvent[2]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_3:
      GaaEcuMWakeUpEvent[3]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_4:
      GaaEcuMWakeUpEvent[4]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_5:
      GaaEcuMWakeUpEvent[5]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_6:
      GaaEcuMWakeUpEvent[6]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_7:
      GaaEcuMWakeUpEvent[7]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_8:
      GaaEcuMWakeUpEvent[8]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_9:
      GaaEcuMWakeUpEvent[9]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_10:
      GaaEcuMWakeUpEvent[10]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_11:
      GaaEcuMWakeUpEvent[11]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_12:
      GaaEcuMWakeUpEvent[12]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_13:
      GaaEcuMWakeUpEvent[13]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_14:
      GaaEcuMWakeUpEvent[14]= 0x01;
      break;
    default:
      /* Not recognized Wakeup source */
      break;
  }
}
/*******************************************************************************
**              Checking the Wakeup                                           **
*******************************************************************************/

void EcuM_CheckWakeup (EcuM_WakeupSourceType WakeupSource)
{
  switch (WakeupSource)
  {
    case EcuM_EcuMWakeupSource_0:
      GaaNotificationEcuMEntered[0]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_1:
      GaaNotificationEcuMEntered[1]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_2:
      GaaNotificationEcuMEntered[2]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_3:
      GaaNotificationEcuMEntered[3]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_4:
      GaaNotificationEcuMEntered[4]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_5:
      GaaNotificationEcuMEntered[5]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_6:
      GaaNotificationEcuMEntered[6]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_7:
      GaaNotificationEcuMEntered[7]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_8:
      GaaNotificationEcuMEntered[8]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_9:
      GaaNotificationEcuMEntered[9]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_10:
      GaaNotificationEcuMEntered[10]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_11:
      GaaNotificationEcuMEntered[11]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_12:
      GaaNotificationEcuMEntered[12]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_13:
      GaaNotificationEcuMEntered[13]= 0x01;
      break;
    case EcuM_EcuMWakeupSource_14:
      GaaNotificationEcuMEntered[14]= 0x01;
      break;
    default:
      /* Not recognized Wakeup source */
      break;
  }

}
/*******************************************************************************
**                          End of File                                       **
*******************************************************************************/
