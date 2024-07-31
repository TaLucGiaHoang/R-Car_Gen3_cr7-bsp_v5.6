/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* File name    = Eth_GeneralTypes.h                                          */
/* ===========================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2016 Renesas Electronics Corporation                     */
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
/*              Devices: R-Car Series, 3rd Generation                         */
/*============================================================================*/
/*
 * 1.0.0      30-Oct-2015      Initial Version
 * 1.0.1      05-Aug-2016      Remove Renesas specific number 59
 */

#ifndef ETH_GENERALTYPES_H
#define ETH_GENERALTYPES_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
/* Com Stack AUTOSAR types */
#include "ComStack_Types.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/*
 * AUTOSAR specification version information
 */

#define ETHGENERAL_TYPES_AR_RELEASE_MAJOR_VERSION    4
#define ETHGENERAL_TYPES_AR_RELEASE_MINOR_VERSION    1
#define ETHGENERAL_TYPES_AR_RELEASE_REVISION_VERSION 2

/*
 * File version information
 */
#define ETHGENERAL_TYPES_SW_MAJOR_VERSION 1
#define ETHGENERAL_TYPES_SW_MINOR_VERSION 0
#define ETHGENERAL_TYPES_SW_PATCH_VERSION 0

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      Global Data Types (ECU dependent)                     **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_070] */
typedef uint8 Eth_DataType;

/*******************************************************************************
**                      Global Data Types (ECU independent)                   **
*******************************************************************************/
/* TRACE[ETH_DDD_DAT_071] */
typedef uint32 Eth_BufIdxType;
/* TRACE[ETH_DDD_DAT_072] */
typedef uint16 Eth_FrameType;

/* Enumeration for Ethernet driverState */
/* TRACE[ETH_DDD_DAT_114] */
typedef enum
{
  ETH_STATE_UNINIT = 0,
  ETH_STATE_INIT
} Eth_StateType;

/* TRACE[ETH_DDD_DAT_073] */
typedef enum
{
  ETH_ADD_TO_FILTER,
  ETH_REMOVE_FROM_FILTER
}Eth_FilterActionType;

/* TRACE[ETH_DDD_DAT_074] */
typedef enum
{
  ETH_MODE_DOWN,
  ETH_MODE_ACTIVE
}Eth_ModeType;

/* TRACE[ETH_DDD_DAT_075] */
typedef enum
{
  ETH_RECEIVED,
  ETH_NOT_RECEIVED,
  ETH_RECEIVED_MORE_DATA_AVAILABLE
}Eth_RxStatusType;

/* TRACE[ETH_DDD_DAT_076] */
typedef enum
{
  ETH_VALID,
  ETH_INVALID,
  ETH_UNCERTAIN
}Eth_TimeStampQualType;

/* TRACE[ETH_DDD_DAT_077] */
typedef struct
{
  uint32 nanoseconds;
  uint32 seconds;
  uint16 secondsHi;
}Eth_TimeStampType;

/* TRACE[ETH_DDD_DAT_078] */
typedef struct
{
  Eth_TimeStampType diff ;
  /*F=Neg T=Pos */
  boolean sign ;
}Eth_TimeIntDiffType;

/* TRACE[ETH_DDD_DAT_079] */
typedef struct
{
  /* InTs2-InTs1 */
  Eth_TimeIntDiffType IngressTimeStampDelta;
  /*OrTs2-OrTs1*/
  Eth_TimeIntDiffType OriginTimeStampDelta;
}Eth_RateRatioType;


/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/


#endif /* ETHGENERAL_TYPES_H */
/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/

