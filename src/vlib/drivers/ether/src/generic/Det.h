/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Det.h                                                       */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2018 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file is a stub for DET component                  .                   */
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
 * 1.0.1      30-Mar-2017      Unify compiler support
 * 1.0.2      04-Jan-2018      Update device name
 * 1.0.3      24-Jan-2018      Add device name M3 about comment.
 * 1.0.4      19-Apr-2018      Change type of GstDemBuffIndex.
 */

#ifndef DET_H
#define DET_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Std_Types.h"

/*******************************************************************************
**                      Version Information                                  **
*******************************************************************************/
/* AUTOSAR release version information */
#define DET_AR_RELEASE_MAJOR_VERSION    4
#define DET_AR_RELEASE_MINOR_VERSION    2
#define DET_AR_RELEASE_REVISION_VERSION 2

/* Module Software version information */
#define DET_SW_MAJOR_VERSION    1
#define DET_SW_MINOR_VERSION    0

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
#define DET_VAR
#define DET_MAX_BUFFER      60


/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/
/* Defining the structure to store the parameters of Det Report Error
   function */
typedef struct STag_tDet_Error
{ /* It will store the ModuleId of the reporting module */
  uint16 ModuleId;

  /* It will store the index based InstanceId of the reporting module */
  uint8 InstanceId;

  /* It will store the ApiId of the reporting function */
  uint8 ApiId;

  /* It will store the ErrorId of the reporting error */
  uint8 ErrorId;
} Det_Error;

extern Det_Error GstDetErrBuffer[];
extern uint32 GstDetBuffIndex;

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
extern Std_ReturnType Det_ReportError(uint16 ModuleId, uint8 InstanceId, \
                                                    uint8 ApiId, uint8 ErrorId);
extern Std_ReturnType Det_ReportRuntimeError(uint16 ModuleId, uint8 InstanceId,\
                                                    uint8 ApiId, uint8 ErrorId);
#endif /* DET_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
