/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Eth_Cbk.h                                                   */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2017 Renesas Electronics Corporation                          */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains pre-compile time parameters.                            */
/* AUTOMATICALLY GENERATED FILE - DO NOT EDIT                                 */
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
/*              Devices:        R-Car H3                                      */
/*============================================================================*/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V1.0.0:  04-May-2017    1) Initial Version                                 */
/******************************************************************************/

/*******************************************************************************
**                   Generation Tool Version                                  **
*******************************************************************************/
/*
 * TOOL VERSION:  1.3.20
 */

/*******************************************************************************
**                          Input File                                        **
*******************************************************************************/
/*
 * INPUT FILE:     D:\RCar\common_platform\generic\stubs\4.2.2\Mcu\xml\
 *                  Mcu_Eth_H3.arxml
 *                 D:\RCar\H3\modules\eth\sample_application\4.2.2\config\
 *                  App_ETH_Sample.arxml
 *                 D:\RCar\common_platform\generic\stubs\4.2.2\Dem\xml\
 *                  Dem_Eth.arxml
 *                 D:\RCar\common_platform\generic\stubs\4.2.2\Os\xml\
 *                  Os_Eth.arxml
 *                 D:\RCar\H3\modules\eth\generator\
 *                  R422_ETH_H3_BSWMDT.arxml
 * GENERATED ON:    5 Nov 2021 - 19:16:34
 */


#ifndef ETH_CBK_H
#define ETH_CBK_H
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/* AUTOSAR release version information */
#define ETH_CBK_AR_RELEASE_MAJOR_VERSION  4U
#define ETH_CBK_AR_RELEASE_MINOR_VERSION  2U
#define ETH_CBK_AR_RELEASE_REVISION_VERSION  2U

/* File version information */
#define ETH_CBK_SW_MAJOR_VERSION   1
#define ETH_CBK_SW_MINOR_VERSION   5


/*******************************************************************************
**                       Common Published Information                         **
*******************************************************************************/

#define ETH_AR_RELEASE_MAJOR_VERSION_VALUE  4
#define ETH_AR_RELEASE_MINOR_VERSION_VALUE  2
#define ETH_AR_RELEASE_REVISION_VERSION_VALUE  2

#define ETH_SW_MAJOR_VERSION_VALUE  1
#define ETH_SW_MINOR_VERSION_VALUE  5
#define ETH_SW_PATCH_VERSION_VALUE  23

#define ETH_VENDOR_ID_VALUE  59
#define ETH_MODULE_ID_VALUE  88

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#define ETH_START_SEC_APPL_CODE
#include "Eth_MemMap.h"


#define ETH_STOP_SEC_APPL_CODE
#include "Eth_MemMap.h"

#endif /* ETH_CBK_H  */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
