/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* File name    = ComStack_Types.h                                            */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2017 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* Provision for Communication Stack dependent types                          */
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
 * V1.0.1:      30-Mar-2017       Variable definitions IcomSwitch_ErrorType and
 *                                IcomConfigIdType are added.
 */

#ifndef COMSTACK_TYPES_H
#define COMSTACK_TYPES_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

#include "Std_Types.h"                  /* standard AUTOSAR types */

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR specification version information */
#define COMSTACK_TYPES_AR_RELEASE_MAJOR_VERSION    4
#define COMSTACK_TYPES_AR_RELEASE_MINOR_VERSION    0
#define COMSTACK_TYPES_AR_RELEASE_REVISION_VERSION 3

/* File version information */
#define COMSTACK_TYPES_SW_MAJOR_VERSION 1
#define COMSTACK_TYPES_SW_MINOR_VERSION 0
#define COMSTACK_TYPES_SW_PATCH_VERSION 0

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      Global Data Types (ECU dependent)                     **
*******************************************************************************/

/* Chapter 8.1.1 */
typedef uint8        PduIdType;     /* Type of PDU ID.
                                       Allowed ranges: uint8 .. uint16 */

/* Chapter 8.1.2 */
typedef uint16       PduLengthType; /* Type of PDU Length.
                                       Allowed ranges: uint8 .. uint32 */

/* Chapter 8.1.6 */
typedef uint8        NotifResultType;

/* Chapter 8.1.7 */
typedef uint8        BusTrcvErrorType;

/* Chapter 8.1.10 */
typedef uint8        NetworkHandleType;

/* Chapter 8.1.1 */
typedef uint8        IcomConfigIdType;

/*******************************************************************************
**                      Global Data Types (ECU independent)                   **
*******************************************************************************/

/* Chapter 8.1.3 */
typedef struct STag_PduInfoType
{
  P2VAR(uint8, AUTOMATIC, AUTOSAR_COMSTACKDATA) SduDataPtr;
  PduLengthType  SduLength;
} PduInfoType;                   /* Basic information about a PDU of any type */

/* Chapter 8.1.8 */
typedef enum ETag_TpDataStateType
{
  TP_DATACONF,
  TP_DATARETRY,
  TP_CONFPENDING
} TpDataStateType; /* Variables of this type shall be used to store the */
                   /* state of TP buffer */
/* Chapter 8.1.9 */
typedef struct STag_RetryInfoType
{
  TpDataStateType TpDataState;
  PduLengthType TxTpDataCnt;
} RetryInfoType;  /* Variables of this type shall be used to store the */
                  /* information about Tp buffer handling. */

/* Chapter 8.1.4 */
typedef enum ETag_TPParameterType
{
  TP_STMIN,
  TP_BS,
  TP_BC
} TPParameterType;     /* Specify parameter to which value has to be changed */

/* Chapter 8.1.5 */
typedef enum ETag_BufReq_ReturnType
{
  BUFREQ_OK,
  BUFREQ_E_NOT_OK,
  BUFREQ_E_BUSY,
  BUFREQ_E_OVFL
} BufReq_ReturnType;                 /* result of a buffer request */

/* Chapter 8.1.5 */
typedef enum STag_IcomSwitch_ErrorType
{
  ICOM_SWITCH_E_OK,
  ICOM_SWITCH_E_FAILED
} IcomSwitch_ErrorType;              /* result of Icom Configuration */

#endif /* COMSTACK_TYPES_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
