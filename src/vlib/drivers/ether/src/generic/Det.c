/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Det.c                                                       */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains the function definition of Det_ReportError API.         */
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
 * 1.0.3      11-Apr-2018      Add guard mechanism for buffer overflow
 * 1.0.4      19-Apr-2018      Change type of GstDemBuffIndex
 *                             Update guard mechanism
 * 1.1.0      23-Mar-2022      Modify for R-CarH3 CR7 Safe-Rendering
 */

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Std_Types.h"
#include "Det.h"
#include "r_print_api.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/


/*******************************************************************************
**                      Version Check                                         **
*******************************************************************************/


/*******************************************************************************
**                         Global Data                                        **
*******************************************************************************/
Det_Error GstDetErrBuffer[DET_MAX_BUFFER];
uint32 GstDetBuffIndex = 0;

/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/


/*****************************************************************************/
/*                   Det_ReportError                                         */
/*****************************************************************************/
Std_ReturnType Det_ReportError (uint16 ModuleId, uint8 InstanceId, uint8 ApiId,
  uint8 ErrorId)
{
  /* DET error is reported */
  Det_Error *Lp_DetBuffer = &GstDetErrBuffer[GstDetBuffIndex % DET_MAX_BUFFER];
  Lp_DetBuffer->ModuleId = ModuleId;
  Lp_DetBuffer->InstanceId = InstanceId;
  Lp_DetBuffer->ApiId = ApiId;
  Lp_DetBuffer->ErrorId = ErrorId;
  GstDetBuffIndex++;
  R_PRINT_Log("Det Error :Module Id = %d :Instance Id = %d :API ID = %d :Error Id = %d\r\n", ModuleId, InstanceId, ApiId, ErrorId);
  return(0);
}

Std_ReturnType Det_ReportRuntimeError (uint16 ModuleId, uint8 InstanceId,
  uint8 ApiId, uint8 ErrorId)
{
  /* DET error is reported */
  Det_Error *Lp_DetBuffer = &GstDetErrBuffer[GstDetBuffIndex % DET_MAX_BUFFER];
  Lp_DetBuffer->ModuleId = ModuleId;
  Lp_DetBuffer->InstanceId = InstanceId;
  Lp_DetBuffer->ApiId = ApiId;
  Lp_DetBuffer->ErrorId = ErrorId;
  GstDetBuffIndex++;
  return(0);
}
/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
