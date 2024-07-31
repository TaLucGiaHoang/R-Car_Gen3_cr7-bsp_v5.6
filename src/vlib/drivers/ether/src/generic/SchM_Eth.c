/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = SchM_Eth.c                                                  */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2016 Renesas Electronics Corporation                          */
/*============================================================================*/
/* Purpose:                                                                   */
/* This application file contains the Schm Ethernet Stub functions.           */
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

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V1.0.0:  30-Mar-2016  : Initial Version
 * V1.0.1:  13-Apr-2018  : Add ETH SchM Exclusive Area API
 */
/******************************************************************************/

/*******************************************************************************
**                     Include Section                                        **
*******************************************************************************/
#include "Os.h"
#include "SchM_Eth.h"


#define ENABLE_IRQ() asm("cpsie i\n\t")
#define DISABLE_IRQ() asm("cpsid i\n\t")
/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/

/*******************************************************************************
**        SchM_Enter_Eth_REGISTER_PROTECTION()                                **
*******************************************************************************/
void SchM_Enter_Eth_REGISTER_PROTECTION(void)
{
  DISABLE_IRQ();
}

/*******************************************************************************
**        SchM_Exit_Eth_REGISTER_PROTECTION()                                 **
*******************************************************************************/
void SchM_Exit_Eth_REGISTER_PROTECTION(void)
{
  ENABLE_IRQ();
}

/*******************************************************************************
**        SchM_Enter_Eth_STATISTIC_DATA_PROTECTION()                          **
*******************************************************************************/
void SchM_Enter_Eth_STATISTIC_DATA_PROTECTION(void)
{
  DISABLE_IRQ();
}

/*******************************************************************************
**        SchM_Exit_Eth_STATISTIC_DATA_PROTECTION()                           **
*******************************************************************************/
void SchM_Exit_Eth_STATISTIC_DATA_PROTECTION(void)
{
  ENABLE_IRQ();
}

/*******************************************************************************
**        SchM_Enter_Eth_GLOBAL_PROTECTION()                                  **
*******************************************************************************/
void SchM_Enter_Eth_GLOBAL_PROTECTION(void)
{
  DISABLE_IRQ();
}

/*******************************************************************************
**        SchM_Exit_Eth_GLOBAL_PROTECTION()                                   **
*******************************************************************************/
void SchM_Exit_Eth_GLOBAL_PROTECTION(void)
{
  ENABLE_IRQ();
}

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/

