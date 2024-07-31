/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Dem.c                                                       */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains the function definition of Dem_ReportErrorStatus API.   */
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
#include "Dem.h"
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
Dem_Error GstDemErrBuffer[DEM_MAX_BUFFER];
uint32 GstDemBuffIndex = 0;

/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/


/*******************************************************************************
**              Dem_ReportErrorStatus                                         **
*******************************************************************************/
void Dem_ReportErrorStatus
                    (Dem_EventIdType EventId, Dem_EventStatusType EventStatus)
{
  /* DEM error is reported */
  Dem_Error *Lp_DemBuffer = &GstDemErrBuffer[GstDemBuffIndex % DEM_MAX_BUFFER];
  Lp_DemBuffer->Dem_EventId = EventId;
  Lp_DemBuffer->Dem_EventStatus = EventStatus;
  GstDemBuffIndex++;

  if ((DEM_EVENT_STATUS_PREPASSED == EventStatus) || (DEM_EVENT_STATUS_PASSED == EventStatus))
  {
    R_PRINT_Log("Dem_ReportErrorStatus(PASSED): Dem Event Id = %d :Dem Event Status = %d\r\n", EventId, EventStatus);
  }
  else
  {
    R_PRINT_Log("Dem_ReportErrorStatus(FAILED): Dem Event Id = %d :Dem Event Status = %d\r\n", EventId, EventStatus);
  }
}

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
