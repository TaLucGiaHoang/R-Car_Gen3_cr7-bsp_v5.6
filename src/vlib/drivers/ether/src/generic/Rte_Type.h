/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Rte_Type.h                                                  */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2019 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* Provision of external declaration of APIs and Service IDs.                 */
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
/*                              R-Car M3                                      */
/*============================================================================*/
/*
 * 1.0.0      31-May-2016     Initial Version
 * 1.1.0      30-Sep-2016     [ICCOM] Revise sample Application and
 *                            memory mapping.
 * 1.2.0      07-Dec-2016     Add definition for IccomDataPtr, IccomNoticeType,
 *                            IccomNoticeInfo.
 * 1.2.1      31-Dec-2016     Add definition for EmmDataPtr, EmmTargetType
 *                            EmmPseudoErrorModeType
 * 1.2.2      11-Jan-2017     Add definition for CrcInDataPtr, CrcOutDataPtr
 * 1.2.3      15-Jun-2017     Add definition for Rte_CDS_CddIpmmu,
 *                            Rte_CDS_IpmmuApp
 * 1.2.4      04-Jan-2018     Update device name
 * 1.2.5      24-Jan-2018   - Updated CRC Polynomial name.
 *                          - Changed DCRA0CTL2 -> DCRAnCTL2
 * 1.2.6      24-Jan-2018     Add device name M3 about comment.
 * 1.2.7      20-Mar-2018     Change CrcInDataPtr to P2CONST
 * 1.2.8      28-Jul-2018     Move CddCrc_PolyType, CddCrc_DataConfigType to
 *                              CRC header file.
 * 1.2.9      12-May-2019     Added Rte type definition for IIC driver
 */

#ifndef RTE_TYPE_H
#define RTE_TYPE_H

/*******************************************************************************
**      To support different AUTOSAR releases in a single package             **
*******************************************************************************/

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Rte.h"

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/*******************************************************************************
**                      MISRA C Rule Violations                              **
*******************************************************************************/

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
/**************************** datatypes ******************************/
typedef P2VAR(uint8, TYPEDEF, RTE_APPL_DATA) IccomDataPtr;
typedef P2CONST(uint8, TYPEDEF, RTE_APPL_DATA) CrcInDataPtr;
typedef P2VAR(uint32, TYPEDEF, RTE_APPL_DATA) CrcOutDataPtr;

/* Type descibes the Channel ID */
typedef uint8 CddCrc_ChannelType;

typedef VAR(unsigned char, TYPEDEF) IccomNoticeType;
typedef VAR(uint8, TYPEDEF) IicMessageType;
typedef VAR(uint8, TYPEDEF) IicOperationType;

typedef struct ETag_IccomNoticeInfo
{
  VAR(IccomNoticeType, TYPEDEF) Type;
  VAR(uint32, TYPEDEF) Message;
} IccomNoticeInfo;

typedef struct STag_IicNotification
{
  VAR(uint32, AUTOMATIC) ByteTransferred;
  VAR(IicOperationType, AUTOMATIC)  Operation;
  VAR(IicMessageType, AUTOMATIC)  Message;
} IicNotification;

/*******************************************************************************
 * Data types for CddEmm
 ******************************************************************************/
typedef P2VAR(uint32, TYPEDEF, RTE_APPL_DATA) EmmDataPtr;

typedef VAR(uint8, TYPEDEF) EmmTargetType;
typedef VAR(uint8, TYPEDEF) EmmPseudoErrorModeType;

/*********************************************************************
 * component data structure for SWC: CddIccom
 *********************************************************************/
typedef struct STag_Rte_CDS_CddIccom
{
  /* Data Handles section*/
  /* Per-Instance Memory Handles section*/
  /* Inter-runnable Variable Handles section*/
  /* Calibration Parameter Handles section*/
  /* Exclusive-area Handles section*/
  /* Port API Handles section*/
  /* Inter Runnable Variable API Handles section*/
  uint8 _dummy;
} Rte_CDS_CddIccom;

/*********************************************************************
 * component data structure for SWC: IccomApp
 *********************************************************************/
typedef struct STag_Rte_CDS_IccomApp
{
  /* Data Handles section*/
  /* Per-Instance Memory Handles section*/
  /* Inter-runnable Variable Handles section*/
  /* Calibration Parameter Handles section*/
  /* Exclusive-area Handles section*/
  /* Port API Handles section*/
  /* Inter Runnable Variable API Handles section*/
  uint8 _dummy;
} Rte_CDS_IccomApp;

/*********************************************************************
 * component data structure for SWC: IicApp
 *********************************************************************/
typedef struct STag_Rte_CDS_CddIic
{
  /* Data Handles section*/
  /* Per-Instance Memory Handles section*/
  /* Inter-runnable Variable Handles section*/
  /* Calibration Parameter Handles section*/
  /* Exclusive-area Handles section*/
  /* Port API Handles section*/
  /* Inter Runnable Variable API Handles section*/
  uint8 _dummy;
} Rte_CDS_CddIic;

/*********************************************************************
 * component data structure for SWC: IccomApp
 *********************************************************************/
typedef struct STag_Rte_CDS_IicApp
{
  /* Data Handles section*/
  /* Per-Instance Memory Handles section*/
  /* Inter-runnable Variable Handles section*/
  /* Calibration Parameter Handles section*/
  /* Exclusive-area Handles section*/
  /* Port API Handles section*/
  /* Inter Runnable Variable API Handles section*/
  uint8 _dummy;
} Rte_CDS_IicApp;

/*********************************************************************
 * component data structure for SWC: CddCrc
 *********************************************************************/
typedef struct STag_Rte_CDS_CddCrc
{
  /* Data Handles section*/
  /* Per-Instance Memory Handles section*/
  /* Inter-runnable Variable Handles section*/
  /* Calibration Parameter Handles section*/
  /* Exclusive-area Handles section*/
  /* Port API Handles section*/
  /* Inter Runnable Variable API Handles section*/
  uint8 _dummy;
} Rte_CDS_CddCrc;

/*********************************************************************
 * component data structure for SWC: CrcmApp
 *********************************************************************/
typedef struct STag_Rte_CDS_CrcApp
{
  /* Data Handles section*/
  /* Per-Instance Memory Handles section*/
  /* Inter-runnable Variable Handles section*/
  /* Calibration Parameter Handles section*/
  /* Exclusive-area Handles section*/
  /* Port API Handles section*/
  /* Inter Runnable Variable API Handles section*/
  uint8 _dummy;
} Rte_CDS_CrcApp;

/*********************************************************************
 * component data structure for SWC: CddIpmmu
 *********************************************************************/
typedef struct STag_Rte_CDS_CddIpmmu
{
  /* Data Handles section*/
  /* Per-Instance Memory Handles section*/
  /* Inter-runnable Variable Handles section*/
  /* Calibration Parameter Handles section*/
  /* Exclusive-area Handles section*/
  /* Port API Handles section*/
  /* Inter Runnable Variable API Handles section*/
  uint8 _dummy;
} Rte_CDS_CddIpmmu;

/*********************************************************************
 * component data structure for SWC: IpmmuApp
 *********************************************************************/
typedef struct STag_Rte_CDS_IpmmuApp
{
  /* Data Handles section*/
  /* Per-Instance Memory Handles section*/
  /* Inter-runnable Variable Handles section*/
  /* Calibration Parameter Handles section*/
  /* Exclusive-area Handles section*/
  /* Port API Handles section*/
  /* Inter Runnable Variable API Handles section*/
  uint8 _dummy;
} Rte_CDS_IpmmuApp;

/*******************************************************************************
*component data structure for SWC: CddRfso
*******************************************************************************/
typedef struct STag_Rte_CDS_CddRfso
{
  /* Data Handles section*/
  /* Per-Instance Memory Handles section*/
  /* Inter-runnable Variable Handles section*/
  /* Calibration Parameter Handles section*/
  /* Exclusive-area Handles section*/
  /* Port API Handles section*/
  /* Inter Runnable Variable API Handles section*/
  uint8 _dummy;
} Rte_CDS_CddRfso;

/*********************************************************************
 * component data structure for SWC: RfsoApp
 *********************************************************************/
typedef struct STag_Rte_CDS_RfsoApp
{
  /* Data Handles section*/
  /* Per-Instance Memory Handles section*/
  /* Inter-runnable Variable Handles section*/
  /* Calibration Parameter Handles section*/
  /* Exclusive-area Handles section*/
  /* Port API Handles section*/
  /* Inter Runnable Variable API Handles section*/
  uint8 _dummy;
} Rte_CDS_RfsoApp;



/*******************************************************************************
**                      Service IDs                                         **
*******************************************************************************/

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**                      Extern declarations for Global Data                   **
*******************************************************************************/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#endif /* RTE_TYPE_H */
/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/

