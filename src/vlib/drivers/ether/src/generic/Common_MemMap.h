/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Common_MemMap.h                                             */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2017-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* Provision for sections for Memory Mapping                                  */
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
/*              Devices:        R-Car Series, 3rd Generation                  */
/*============================================================================*/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * 1.0.0      17-Feb-2017     Initial Version
 * 1.0.1      28-Apr-2017     Fixed bug absence of section type with GHS
 *                            Ref: http://172.29.143.164/issues/81041
 * 1.0.2      20-Dec-2017     Update macro for alignment buffer
 * 1.0.3      18-Jan-2018     Update for compliance of data sections.
 * 1.0.4      26-Jan-2018     Update startup code to unify MemMap for integrated
 *                            environment
 * 1.0.5      21-Jun-2019     Add macro for user ram data sections.
 * 1.1.0      23-Mar-2022     Modify for R-CarH3 CR7 Safe-Rendering
 */
/******************************************************************************/

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**                      Default section mapping                               **
*******************************************************************************/
/* general start of #elif chain whith #if                                     */
#if defined (START_WITH_IF)

/* -------------------------------------------------------------------------- */
/* RAM variables initialized from ROM on reset                                */
/* -------------------------------------------------------------------------- */
#elif defined (DEFAULT_START_SEC_VAR_INIT_1)
   #undef      DEFAULT_START_SEC_VAR_INIT_1
   #define     PRIVATE_SECTION ".data.VAR_INIT_1"
   #define     SECTION_TYPE data
#elif defined (DEFAULT_STOP_SEC_VAR_INIT_1)
   #undef      DEFAULT_STOP_SEC_VAR_INIT_1
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE data

#elif defined (DEFAULT_START_SEC_VAR_INIT_8)
   #undef      DEFAULT_START_SEC_VAR_INIT_8
   #define     PRIVATE_SECTION ".data.VAR_INIT_8"
   #define     SECTION_TYPE data
#elif defined (DEFAULT_STOP_SEC_VAR_INIT_8)
   #undef      DEFAULT_STOP_SEC_VAR_INIT_8
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE data

#elif defined (DEFAULT_START_SEC_VAR_INIT_16)
   #undef      DEFAULT_START_SEC_VAR_INIT_16
   #define     PRIVATE_SECTION ".data.VAR_INIT_16"
   #define     SECTION_TYPE data
#elif defined (DEFAULT_STOP_SEC_VAR_INIT_16)
   #undef      DEFAULT_STOP_SEC_VAR_INIT_16
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE data

#elif defined (DEFAULT_START_SEC_VAR_INIT_32)
   #undef      DEFAULT_START_SEC_VAR_INIT_32
   #define     PRIVATE_SECTION ".data.VAR_INIT_32"
   #define     SECTION_TYPE data
#elif defined (DEFAULT_STOP_SEC_VAR_INIT_32)
   #undef      DEFAULT_STOP_SEC_VAR_INIT_32
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE data

#elif defined (DEFAULT_START_SEC_VAR_INIT_UNSPECIFIED)
   #undef      DEFAULT_START_SEC_VAR_INIT_UNSPECIFIED
   #define     PRIVATE_SECTION ".data.VAR_INIT_UNSPECIFIED"
   #define     SECTION_TYPE data
#elif defined (DEFAULT_STOP_SEC_VAR_INIT_UNSPECIFIED)
   #undef      DEFAULT_STOP_SEC_VAR_INIT_UNSPECIFIED
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE data

/* -------------------------------------------------------------------------- */
/* RAM variables not initialized                                              */
/* -------------------------------------------------------------------------- */
#elif defined (DEFAULT_START_SEC_VAR_NO_INIT_1)
   #undef      DEFAULT_START_SEC_VAR_NO_INIT_1
   #define     PRIVATE_SECTION ".data.VAR_NO_INIT_1"
   #define     SECTION_TYPE data
#elif defined (DEFAULT_STOP_SEC_VAR_NO_INIT_1)
   #undef      DEFAULT_STOP_SEC_VAR_NO_INIT_1
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE data

#elif defined (DEFAULT_START_SEC_VAR_NO_INIT_8)
   #undef      DEFAULT_START_SEC_VAR_NO_INIT_8
   #define     PRIVATE_SECTION ".data.VAR_NO_INIT_8"
   #define     SECTION_TYPE data
#elif defined (DEFAULT_STOP_SEC_VAR_NO_INIT_8)
   #undef      DEFAULT_STOP_SEC_VAR_NO_INIT_8
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE data

#elif defined (DEFAULT_START_SEC_VAR_NO_INIT_16)
   #undef      DEFAULT_START_SEC_VAR_NO_INIT_16
   #define     PRIVATE_SECTION ".data.VAR_NO_INIT_16"
   #define     SECTION_TYPE data
#elif defined (DEFAULT_STOP_SEC_VAR_NO_INIT_16)
   #undef      DEFAULT_STOP_SEC_VAR_NO_INIT_16
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE data

#elif defined (DEFAULT_START_SEC_VAR_NO_INIT_32)
   #undef      DEFAULT_START_SEC_VAR_NO_INIT_32
   #define     PRIVATE_SECTION ".data.VAR_NO_INIT_32"
   #define     SECTION_TYPE data
#elif defined (DEFAULT_STOP_SEC_VAR_NO_INIT_32)
   #undef      DEFAULT_STOP_SEC_VAR_NO_INIT_32
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE data

#elif defined (DEFAULT_START_SEC_VAR_NO_INIT_UNSPECIFIED)
   #undef      DEFAULT_START_SEC_VAR_NO_INIT_UNSPECIFIED
   #define     PRIVATE_SECTION ".data.VAR_NO_INIT_UNSPECIFIED"
   #define     SECTION_TYPE data
#elif defined (DEFAULT_STOP_SEC_VAR_NO_INIT_UNSPECIFIED)
   #undef      DEFAULT_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE data

/* -------------------------------------------------------------------------- */
/* RAM variables are cleared                                                  */
/* -------------------------------------------------------------------------- */
#elif defined (DEFAULT_START_SEC_VAR_CLEARED_1)
   #undef      DEFAULT_START_SEC_VAR_CLEARED_1
   #define     PRIVATE_SECTION ".bss.VAR_CLEARED_1"
   #define     SECTION_TYPE bss
#elif defined (DEFAULT_STOP_SEC_VAR_CLEARED_1)
   #undef      DEFAULT_STOP_SEC_VAR_CLEARED_1
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE bss

#elif defined (DEFAULT_START_SEC_VAR_CLEARED_8)
   #undef      DEFAULT_START_SEC_VAR_CLEARED_8
   #define     PRIVATE_SECTION ".bss.VAR_CLEARED_8"
   #define     SECTION_TYPE bss
#elif defined (DEFAULT_STOP_SEC_VAR_CLEARED_8)
   #undef      DEFAULT_STOP_SEC_VAR_CLEARED_8
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE bss

#elif defined (DEFAULT_START_SEC_VAR_CLEARED_16)
   #undef      DEFAULT_START_SEC_VAR_CLEARED_16
   #define     PRIVATE_SECTION ".bss.VAR_CLEARED_16"
   #define     SECTION_TYPE bss
#elif defined (DEFAULT_STOP_SEC_VAR_CLEARED_16)
   #undef      DEFAULT_STOP_SEC_VAR_CLEARED_16
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE bss

#elif defined (DEFAULT_START_SEC_VAR_CLEARED_32)
   #undef      DEFAULT_START_SEC_VAR_CLEARED_32
   #define     PRIVATE_SECTION ".bss.VAR_CLEARED_32"
   #define     SECTION_TYPE bss
#elif defined (DEFAULT_STOP_SEC_VAR_CLEARED_32)
   #undef      DEFAULT_STOP_SEC_VAR_CLEARED_32
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE bss

#elif defined (DEFAULT_START_SEC_VAR_CLEARED_UNSPECIFIED)
   #undef      DEFAULT_START_SEC_VAR_CLEARED_UNSPECIFIED
   #define     PRIVATE_SECTION ".bss.VAR_CLEARED_UNSPECIFIED"
   #define     SECTION_TYPE bss
#elif defined (DEFAULT_STOP_SEC_VAR_CLEARED_UNSPECIFIED)
   #undef      DEFAULT_STOP_SEC_VAR_CLEARED_UNSPECIFIED
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE bss

#elif defined (DEFAULT_START_SEC_VAR_CLEARED_USER_RAM_UNSPECIFIED)
   #undef      DEFAULT_START_SEC_VAR_CLEARED_USER_RAM_UNSPECIFIED
   #define     PRIVATE_SECTION ".bss.VAR_CLEARED_USER_RAM_UNSPECIFIED"
   #define     SECTION_TYPE bss
   #define     ALIGNMENT_SECTION
#elif defined (DEFAULT_STOP_SEC_VAR_CLEARED_USER_RAM_UNSPECIFIED)
   #undef      DEFAULT_STOP_SEC_VAR_CLEARED_USER_RAM_UNSPECIFIED
   #undef      PRIVATE_SECTION
   #undef      ALIGNMENT_SECTION
   #define     SECTION_TYPE bss

#elif defined (DEFAULT_START_SEC_VAR_CLEARED_DESCRIPTOR_32)
   #undef      DEFAULT_START_SEC_VAR_CLEARED_DESCRIPTOR_32
   #define     PRIVATE_SECTION ".eth_non_cache" /* Define non-cache area for descriptor */
   #define     SECTION_TYPE bss
#elif defined (DEFAULT_STOP_SEC_VAR_CLEARED_DESCRIPTOR_32)
   #undef      DEFAULT_STOP_SEC_VAR_CLEARED_DESCRIPTOR_32
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE bss

#elif defined (DEFAULT_START_SEC_VAR_CLEARED_USER_RAM_32)
   #undef      DEFAULT_START_SEC_VAR_CLEARED_USER_RAM_32
   #define     PRIVATE_SECTION ".bss.VAR_CLEARED_USER_RAM_32"
   #define     SECTION_TYPE bss
#elif defined (DEFAULT_STOP_SEC_VAR_CLEARED_USER_RAM_32)
   #undef      DEFAULT_STOP_SEC_VAR_CLEARED_USER_RAM_32
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE bss
   /* -------------------------------------------------------------------------- */
/* ROM constants                                                              */
/* -------------------------------------------------------------------------- */
#elif defined (DEFAULT_START_SEC_CONST_1)
   #undef      DEFAULT_START_SEC_CONST_1
   #define     PRIVATE_SECTION ".constdata.CONST_1"
   #define     SECTION_TYPE rodata
#elif defined (DEFAULT_STOP_SEC_CONST_1)
   #undef      DEFAULT_STOP_SEC_CONST_1
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE rodata

#elif defined (DEFAULT_START_SEC_CONST_8)
   #undef      DEFAULT_START_SEC_CONST_8
   #define     PRIVATE_SECTION ".constdata.CONST_8"
   #define     SECTION_TYPE rodata
#elif defined (DEFAULT_STOP_SEC_CONST_8)
   #undef      DEFAULT_STOP_SEC_CONST_8
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE rodata

#elif defined (DEFAULT_START_SEC_CONST_16)
   #undef      DEFAULT_START_SEC_CONST_16
   #define     PRIVATE_SECTION ".constdata.CONST_16"
   #define     SECTION_TYPE rodata
#elif defined (DEFAULT_STOP_SEC_CONST_16)
   #undef      DEFAULT_STOP_SEC_CONST_16
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE rodata

#elif defined (DEFAULT_START_SEC_CONST_32)
   #undef      DEFAULT_START_SEC_CONST_32
   #define     PRIVATE_SECTION ".constdata.CONST_32"
   #define     SECTION_TYPE rodata
#elif defined (DEFAULT_STOP_SEC_CONST_32)
   #undef      DEFAULT_STOP_SEC_CONST_32
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE rodata

#elif defined (DEFAULT_START_SEC_CONST_UNSPECIFIED)
   #undef      DEFAULT_START_SEC_CONST_UNSPECIFIED
   #define     PRIVATE_SECTION ".constdata.CONST_UNSPECIFIED"
   #define     SECTION_TYPE rodata
#elif defined (DEFAULT_STOP_SEC_CONST_UNSPECIFIED)
   #undef      DEFAULT_STOP_SEC_CONST_UNSPECIFIED
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE rodata

/* -------------------------------------------------------------------------- */
/* ROM configure constants                                                    */
/* -------------------------------------------------------------------------- */
#elif defined (DEFAULT_START_SEC_CONFIG_DATA_1)
   #undef      DEFAULT_START_SEC_CONFIG_DATA_1
   #define     PRIVATE_SECTION ".constdata.CONFIG_DATA_1"
   #define     SECTION_TYPE rodata
#elif defined (DEFAULT_STOP_SEC_CONFIG_DATA_1)
   #undef      DEFAULT_STOP_SEC_CONFIG_DATA_1
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE rodata

#elif defined (DEFAULT_START_SEC_CONFIG_DATA_8)
   #undef      DEFAULT_START_SEC_CONFIG_DATA_8
   #define     PRIVATE_SECTION ".constdata.CONFIG_DATA_8"
   #define     SECTION_TYPE rodata
#elif defined (DEFAULT_STOP_SEC_CONFIG_DATA_8)
   #undef      DEFAULT_STOP_SEC_CONFIG_DATA_8
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE rodata

#elif defined (DEFAULT_START_SEC_CONFIG_DATA_16)
   #undef      DEFAULT_START_SEC_CONFIG_DATA_16
   #define     PRIVATE_SECTION ".constdata.CONFIG_DATA_16"
   #define     SECTION_TYPE rodata
#elif defined (DEFAULT_STOP_SEC_CONFIG_DATA_16)
   #undef      DEFAULT_STOP_SEC_CONFIG_DATA_16
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE rodata

#elif defined (DEFAULT_START_SEC_CONFIG_DATA_32)
   #undef      DEFAULT_START_SEC_CONFIG_DATA_32
   #define     PRIVATE_SECTION ".constdata.CONFIG_DATA_32"
   #define     SECTION_TYPE rodata
#elif defined (DEFAULT_STOP_SEC_CONFIG_DATA_32)
   #undef      DEFAULT_STOP_SEC_CONFIG_DATA_32
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE rodata

#elif defined (DEFAULT_START_SEC_CONFIG_DATA_UNSPECIFIED)
   #undef      DEFAULT_START_SEC_CONFIG_DATA_UNSPECIFIED
   #define     PRIVATE_SECTION ".constdata.CONFIG_DATA_UNSPECIFIED"
   #define     SECTION_TYPE rodata
#elif defined (DEFAULT_STOP_SEC_CONFIG_DATA_UNSPECIFIED)
   #undef      DEFAULT_STOP_SEC_CONFIG_DATA_UNSPECIFIED
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE rodata

/* -------------------------------------------------------------------------- */
/* ROM code                                                                   */
/* -------------------------------------------------------------------------- */
#elif defined (DEFAULT_START_SEC_CODE)
   #undef      DEFAULT_START_SEC_CODE
   #define     PRIVATE_SECTION ".text.CODE"
   #define     SECTION_TYPE text
#elif defined (DEFAULT_STOP_SEC_CODE)
   #undef      DEFAULT_STOP_SEC_CODE
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE text

#elif defined (DEFAULT_START_SEC_CODE_SLOW)
   #undef      DEFAULT_START_SEC_CODE_SLOW
   #define     PRIVATE_SECTION ".text.CODE_SLOW"
   #define     SECTION_TYPE text
#elif defined (DEFAULT_STOP_SEC_CODE_SLOW)
   #undef      DEFAULT_STOP_SEC_CODE_SLOW
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE text

#elif defined (DEFAULT_START_SEC_CALLOUT_CODE)
   #undef      DEFAULT_START_SEC_CALLOUT_CODE
   #define     PRIVATE_SECTION ".text.CALLOUT_CODE"
   #define     SECTION_TYPE text
#elif defined (DEFAULT_STOP_SEC_CALLOUT_CODE)
   #undef      DEFAULT_STOP_SEC_CALLOUT_CODE
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE text

#elif defined (DEFAULT_START_SEC_CODE_FAST)
   #undef      DEFAULT_START_SEC_CODE_FAST
   #define     PRIVATE_SECTION ".text.CODE_FAST"
   #define     SECTION_TYPE text
#elif defined (DEFAULT_STOP_SEC_CODE_FAST)
   #undef      DEFAULT_STOP_SEC_CODE_FAST
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE text

#elif defined (DEFAULT_START_SEC_APPL_CODE)
   #undef      DEFAULT_START_SEC_APPL_CODE
   #define     PRIVATE_SECTION ".text.APPL_CODE"
   #define     SECTION_TYPE text
#elif defined (DEFAULT_STOP_SEC_APPL_CODE)
   #undef      DEFAULT_STOP_SEC_APPL_CODE
   #undef      PRIVATE_SECTION
   #define     SECTION_TYPE text
/* ---------------------------------------------------------------------------*/
/* End of default section mapping                                             */
/* ---------------------------------------------------------------------------*/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#endif  /* START_WITH_IF */

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "MemMap_dep.h"

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
