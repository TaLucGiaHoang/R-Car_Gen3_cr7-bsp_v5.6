/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Dem.h                                                       */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2018 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file is a stub for DEM component                  .                   */
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
 * 1.1.0      30-Sep-2016      Added macros: DEM_EVENT_STATUS_PREPASSED,
 *                             DEM_EVENT_STATUS_PREFAILED
 * 1.1.1      30-Mar-2017      Unify compiler support
 * 1.1.2      04-Jan-2018      Update device name
 * 1.1.3      24-Jan-2018      Add device name M3 about comment.
 * 1.1.4      19-Apr-2018      Change type of GstDemBuffIndex.
 */

#ifndef DEM_H
#define DEM_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Std_Types.h"
#include "Dem_Cfg.h"

/*******************************************************************************
**                      Version Information                                  **
*******************************************************************************/
/* AUTOSAR release version information */
#define DEM_AR_RELEASE_MAJOR_VERSION    4
#define DEM_AR_RELEASE_MINOR_VERSION    2
#define DEM_AR_RELEASE_REVISION_VERSION 2

/* Module Software version information */
#define DEM_SW_MAJOR_VERSION    4
#define DEM_SW_MINOR_VERSION    0
#define DEM_SW_PATCH_VERSION    0
/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
#define DEM_VAR
#define DEM_MAX_BUFFER      60

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

typedef uint8 Dem_EventStatusType;
typedef uint16 Dem_EventIdType;

#define DEM_EVENT_STATUS_PASSED (Dem_EventStatusType)0x00
#define DEM_EVENT_STATUS_FAILED (Dem_EventStatusType)0x01
#define DEM_EVENT_STATUS_PREPASSED (Dem_EventStatusType)0x02
#define DEM_EVENT_STATUS_PREFAILED (Dem_EventStatusType)0x03

/*******************************************************************************
**                         Global Data  Declarations                          **
*******************************************************************************/
/* Defining the structure to store the parameters of Dem Report Error
   function */
typedef struct STag_Dem_Error
{ /* It will store the DEM event ID */
  uint16 Dem_EventId;

  /* It will store the DEM event status */
  uint8 Dem_EventStatus;
} Dem_Error;
extern Dem_Error GstDemErrBuffer[];
extern uint32 GstDemBuffIndex;

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/


extern void Dem_ReportErrorStatus
                    (Dem_EventIdType EventId, Dem_EventStatusType EventStatus);

#endif /* DEM_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
