/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* File name    = Std_Types.h                                                 */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* Provision for Standard types                                               */
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
/*                              R-Car H3                                      */
/*============================================================================*/
/*
 * V1.0.0:      30-Oct-2015:      Initial Version
 * V1.0.1:      07-Dec-2017:      Add E_OS_ID and E_OS_VALUE.
 * V1.1.0:      23-Mar-2022:      Modify for R-CarH3 CR7 Safe-Rendering
 */

#ifndef STD_TYPES_H
#define STD_TYPES_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Compiler_Common.h"           /* mapping compiler specific keywords */
#include "Platform_Types.h"            /* platform specific type definitions */

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR specification version information */
#define STD_TYPES_AR_RELEASE_MAJOR_VERSION     4
#define STD_TYPES_AR_RELEASE_MINOR_VERSION     2
#define STD_TYPES_AR_RELEASE_REVISION_VERSION  2

/* File version information */
#define STD_TYPES_SW_MAJOR_VERSION  1
#define STD_TYPES_SW_MINOR_VERSION  0
#define STD_TYPES_SW_PATCH_VERSION  0

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      Global Data Types                                    **
*******************************************************************************/

/* for OSEK compliance this typedef has been added */
#ifndef STATUSTYPEDEFINED
  #define STATUSTYPEDEFINED
  #define E_OK         0U
  #define E_OS_ID      3U
  #define E_OS_VALUE   8U

  typedef unsigned char StatusType;
#endif

/* STD011
  The Std_ReturnType (STD005) may be used with the following values (STD006):
  E_OK:     0
  E_NOT_OK: 1
*/
typedef uint8 Std_ReturnType;
#define E_NOT_OK    1U

typedef struct STag_Std_VersionInfoType
{
  uint16  vendorID;
  uint16  moduleID;
  uint8  sw_major_version;
  uint8  sw_minor_version;
  uint8  sw_patch_version;
} Std_VersionInfoType; /* STD015                                */


#define STD_HIGH    1U  /* Physical state 5V or 3.3V             */
#define STD_LOW     0U  /* Physical state 0V                     */

#define STD_ACTIVE  1U  /* Logical state active                  */
#define STD_IDLE    0U  /* Logical state idle                    */

#define STD_ON      1U
#define STD_OFF     0U

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#endif  /* STD_TYPES_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
