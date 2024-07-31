/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = FuSa_MemMap.h                                               */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2018 Renesas Electronics Corporation                          */
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
 * 1.0.0      26-Mar-2018     Initial Version
 */
/******************************************************************************/

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/*
 * AUTOSAR specification version information
 */
#ifndef FUSA_MEMMAP_AR_RELEASE_MAJOR_VERSION
#define FUSA_MEMMAP_AR_RELEASE_MAJOR_VERSION      4
#endif
#ifndef FUSA_MEMMAP_AR_RELEASE_MINOR_VERSION
#define FUSA_MEMMAP_AR_RELEASE_MINOR_VERSION      2
#endif
#ifndef FUSA_MEMMAP_AR_RELEASE_REVISION_VERSION
#define FUSA_MEMMAP_AR_RELEASE_REVISION_VERSION   2
#endif
/*
 * File version information
 */
#ifndef FUSA_MEMMAP_SW_MAJOR_VERSION
#define FUSA_MEMMAP_SW_MAJOR_VERSION   1
#endif
#ifndef FUSA_MEMMAP_SW_MINOR_VERSION
#define FUSA_MEMMAP_SW_MINOR_VERSION   0
#endif
#ifndef FUSA_MEMMAP_SW_PATCH_VERSION
#define FUSA_MEMMAP_SW_PATCH_VERSION   0
#endif
/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**                      Module section mapping                                **
*******************************************************************************/
/*
 * The symbol 'START_WITH_IF' is undefined.
 *
 * Thus, the preprocessor continues searching for defined symbols
 * This first #ifdef makes integration of delivered parts of MemMap.h
 * easier because every supplier starts with #elif
 */
#if defined (START_WITH_IF)

/* -------------------------------------------------------------------------- */
/*             FuSa Function                                                  */
/* -------------------------------------------------------------------------- */

#elif defined (FUSA_START_SEC_VAR_INIT_BOOLEAN)
   #ifdef VAR_INIT_BOOLEAN_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define VAR_INIT_BOOLEAN_SEC_STARTED
     #undef  FUSA_START_SEC_VAR_INIT_BOOLEAN
     #define DEFAULT_START_SEC_VAR_INIT_1
   #endif
#elif defined (FUSA_STOP_SEC_VAR_INIT_BOOLEAN)
   #ifndef VAR_INIT_BOOLEAN_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  VAR_INIT_BOOLEAN_SEC_STARTED
     #undef  FUSA_STOP_SEC_VAR_INIT_BOOLEAN
     #define DEFAULT_STOP_SEC_VAR_INIT_1
   #endif

#elif defined (FUSA_START_SEC_VAR_CLEARED_BOOLEAN)
   #ifdef VAR_CLEARED_BOOLEAN_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define VAR_CLEARED_BOOLEAN_SEC_STARTED
     #undef  FUSA_START_SEC_VAR_CLEARED_BOOLEAN
     #define DEFAULT_START_SEC_VAR_CLEARED_1
   #endif
#elif defined (FUSA_STOP_SEC_VAR_CLEARED_BOOLEAN)
   #ifndef VAR_CLEARED_BOOLEAN_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  VAR_CLEARED_BOOLEAN_SEC_STARTED
     #undef  FUSA_STOP_SEC_VAR_CLEARED_BOOLEAN
     #define DEFAULT_STOP_SEC_VAR_CLEARED_1
   #endif

#elif defined (FUSA_START_SEC_VAR_INIT_8)
   #ifdef VAR_INIT_8_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define VAR_INIT_8_SEC_STARTED
     #undef  FUSA_START_SEC_VAR_INIT_8
     #define DEFAULT_START_SEC_VAR_INIT_8
   #endif
#elif defined (FUSA_STOP_SEC_VAR_INIT_8)
   #ifndef VAR_INIT_8_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  VAR_INIT_8_SEC_STARTED
     #undef  FUSA_STOP_SEC_VAR_INIT_8
     #define DEFAULT_STOP_SEC_VAR_INIT_8
   #endif

#elif defined (FUSA_START_SEC_VAR_CLEARED_8)
   #ifdef VAR_CLEARED_8_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define VAR_CLEARED_8_SEC_STARTED
     #undef  FUSA_START_SEC_VAR_CLEARED_8
     #define DEFAULT_START_SEC_VAR_CLEARED_8
   #endif
#elif defined (FUSA_STOP_SEC_VAR_CLEARED_8)
   #ifndef VAR_CLEARED_8_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  VAR_CLEARED_8_SEC_STARTED
     #undef  FUSA_STOP_SEC_VAR_CLEARED_8
     #define DEFAULT_STOP_SEC_VAR_CLEARED_8
   #endif

#elif defined (FUSA_START_SEC_VAR_INIT_16)
   #ifdef VAR_INIT_16_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define VAR_INIT_16_SEC_STARTED
     #undef  FUSA_START_SEC_VAR_INIT_16
     #define DEFAULT_START_SEC_VAR_INIT_16
   #endif
#elif defined (FUSA_STOP_SEC_VAR_INIT_16)
   #ifndef VAR_INIT_16_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  VAR_INIT_16_SEC_STARTED
     #undef  FUSA_STOP_SEC_VAR_INIT_16
     #define DEFAULT_STOP_SEC_VAR_INIT_16
   #endif

#elif defined (FUSA_START_SEC_VAR_CLEARED_16)
   #ifdef VAR_CLEARED_16_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define VAR_CLEARED_16_SEC_STARTED
     #undef  FUSA_START_SEC_VAR_CLEARED_16
     #define DEFAULT_START_SEC_VAR_CLEARED_16
   #endif
#elif defined (FUSA_STOP_SEC_VAR_CLEARED_16)
   #ifndef VAR_CLEARED_16_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  VAR_CLEARED_16_SEC_STARTED
     #undef  FUSA_STOP_SEC_VAR_CLEARED_16
     #define DEFAULT_STOP_SEC_VAR_CLEARED_16
   #endif

#elif defined (FUSA_START_SEC_VAR_INIT_32)
   #ifdef VAR_INIT_32_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define VAR_INIT_32_SEC_STARTED
     #undef  FUSA_START_SEC_VAR_INIT_32
     #define DEFAULT_START_SEC_VAR_INIT_32
   #endif
#elif defined (FUSA_STOP_SEC_VAR_INIT_32)
   #ifndef VAR_INIT_32_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  VAR_INIT_32_SEC_STARTED
     #undef  FUSA_STOP_SEC_VAR_INIT_32
     #define DEFAULT_STOP_SEC_VAR_INIT_32
   #endif

#elif defined (FUSA_START_SEC_VAR_CLEARED_32)
   #ifdef VAR_CLEARED_32_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define VAR_CLEARED_32_SEC_STARTED
     #undef  FUSA_START_SEC_VAR_CLEARED_32
     #define DEFAULT_START_SEC_VAR_CLEARED_32
   #endif
#elif defined (FUSA_STOP_SEC_VAR_CLEARED_32)
   #ifndef VAR_CLEARED_32_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  VAR_CLEARED_32_SEC_STARTED
     #undef  FUSA_STOP_SEC_VAR_CLEARED_32
     #define DEFAULT_STOP_SEC_VAR_CLEARED_32
   #endif

#elif defined (FUSA_START_SEC_VAR_INIT_UNSPECIFIED)
   #ifdef VAR_INIT_UNSPECIFIED_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define VAR_INIT_UNSPECIFIED_SEC_STARTED
     #undef  FUSA_START_SEC_VAR_INIT_UNSPECIFIED
     #define DEFAULT_START_SEC_VAR_INIT_UNSPECIFIED
   #endif
#elif defined (FUSA_STOP_SEC_VAR_INIT_UNSPECIFIED)
   #ifndef VAR_INIT_UNSPECIFIED_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  VAR_INIT_UNSPECIFIED_SEC_STARTED
     #undef  FUSA_STOP_SEC_VAR_INIT_UNSPECIFIED
     #define DEFAULT_STOP_SEC_VAR_INIT_UNSPECIFIED
   #endif

#elif defined (FUSA_START_SEC_VAR_CLEARED_UNSPECIFIED)
   #ifdef VAR_CLEARED_UNSPECIFIED_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define VAR_CLEARED_UNSPECIFIED_SEC_STARTED
     #undef  FUSA_START_SEC_VAR_CLEARED_UNSPECIFIED
     #define DEFAULT_START_SEC_VAR_CLEARED_UNSPECIFIED
   #endif
#elif defined (FUSA_STOP_SEC_VAR_CLEARED_UNSPECIFIED)
   #ifndef VAR_CLEARED_UNSPECIFIED_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  VAR_CLEARED_UNSPECIFIED_SEC_STARTED
     #undef  FUSA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
     #define DEFAULT_STOP_SEC_VAR_CLEARED_UNSPECIFIED
   #endif

#elif defined (FUSA_START_SEC_CONST_BOOLEAN)
   #ifdef CONST_BOOLEAN_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define CONST_BOOLEAN_SEC_STARTED
     #undef  FUSA_START_SEC_CONST_BOOLEAN
     #define DEFAULT_START_SEC_CONST_1
   #endif
#elif defined (FUSA_STOP_SEC_CONST_BOOLEAN)
   #ifndef CONST_BOOLEAN_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  CONST_BOOLEAN_SEC_STARTED
     #undef  FUSA_STOP_SEC_CONST_BOOLEAN
     #define DEFAULT_STOP_SEC_CONST_1
   #endif

#elif defined (FUSA_START_SEC_CONST_8)
   #ifdef CONST_8_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define CONST_8_SEC_STARTED
     #undef  FUSA_START_SEC_CONST_8
     #define DEFAULT_START_SEC_CONST_8
   #endif
#elif defined (FUSA_STOP_SEC_CONST_8)
   #ifndef CONST_8_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  CONST_8_SEC_STARTED
     #undef  FUSA_STOP_SEC_CONST_8
     #define DEFAULT_STOP_SEC_CONST_8
   #endif

#elif defined (FUSA_START_SEC_CONST_16)
   #ifdef CONST_16_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define CONST_16_SEC_STARTED
     #undef  FUSA_START_SEC_CONST_16
     #define DEFAULT_START_SEC_CONST_16
   #endif
#elif defined (FUSA_STOP_SEC_CONST_16)
   #ifndef CONST_16_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  CONST_16_SEC_STARTED
     #undef  FUSA_STOP_SEC_CONST_16
     #define DEFAULT_STOP_SEC_CONST_16
   #endif

#elif defined (FUSA_START_SEC_CONST_32)
   #ifdef CONST_32_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define CONST_32_SEC_STARTED
     #undef  FUSA_START_SEC_CONST_32
     #define DEFAULT_START_SEC_CONST_32
   #endif
#elif defined (FUSA_STOP_SEC_CONST_32)
   #ifndef CONST_32_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  CONST_32_SEC_STARTED
     #undef  FUSA_STOP_SEC_CONST_32
     #define DEFAULT_STOP_SEC_CONST_32
   #endif

#elif defined (FUSA_START_SEC_CONST_UNSPECIFIED)
   #ifdef CONST_UNSPECIFIED_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define CONST_UNSPECIFIED_SEC_STARTED
     #undef  FUSA_START_SEC_CONST_UNSPECIFIED
     #define DEFAULT_START_SEC_CONST_UNSPECIFIED
   #endif
#elif defined (FUSA_STOP_SEC_CONST_UNSPECIFIED)
   #ifndef CONST_UNSPECIFIED_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  CONST_UNSPECIFIED_SEC_STARTED
     #undef  FUSA_STOP_SEC_CONST_UNSPECIFIED
     #define DEFAULT_STOP_SEC_CONST_UNSPECIFIED
   #endif

#elif defined (FUSA_START_SEC_CONFIG_DATA_BOOLEAN)
   #ifdef CONFIG_DATA_BOOLEAN_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define CONFIG_DATA_BOOLEAN_SEC_STARTED
     #undef  FUSA_START_SEC_CONFIG_DATA_BOOLEAN
     #define DEFAULT_START_SEC_CONFIG_DATA_1
   #endif
#elif defined (FUSA_STOP_SEC_CONFIG_DATA_BOOLEAN)
   #ifndef CONFIG_DATA_BOOLEAN_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  CONFIG_DATA_BOOLEAN_SEC_STARTED
     #undef  FUSA_STOP_SEC_CONFIG_DATA_BOOLEAN
     #define DEFAULT_STOP_SEC_CONFIG_DATA_1
   #endif

#elif defined (FUSA_START_SEC_CONFIG_DATA_8)
   #ifdef CONFIG_DATA_8_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define CONFIG_DATA_8_SEC_STARTED
     #undef  FUSA_START_SEC_CONFIG_DATA_8
     #define DEFAULT_START_SEC_CONFIG_DATA_8
   #endif
#elif defined (FUSA_STOP_SEC_CONFIG_DATA_8)
   #ifndef CONFIG_DATA_8_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  CONFIG_DATA_8_SEC_STARTED
     #undef  FUSA_STOP_SEC_CONFIG_DATA_8
     #define DEFAULT_STOP_SEC_CONFIG_DATA_8
   #endif

#elif defined (FUSA_START_SEC_CONFIG_DATA_16)
   #ifdef CONFIG_DATA_16_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define CONFIG_DATA_16_SEC_STARTED
     #undef  FUSA_START_SEC_CONFIG_DATA_16
     #define DEFAULT_START_SEC_CONFIG_DATA_16
   #endif
#elif defined (FUSA_STOP_SEC_CONFIG_DATA_16)
   #ifndef CONFIG_DATA_16_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  CONFIG_DATA_16_SEC_STARTED
     #undef  FUSA_STOP_SEC_CONFIG_DATA_16
     #define DEFAULT_STOP_SEC_CONFIG_DATA_16
   #endif

#elif defined (FUSA_START_SEC_CONFIG_DATA_32)
   #ifdef CONFIG_DATA_32_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define CONFIG_DATA_32_SEC_STARTED
     #undef  FUSA_START_SEC_CONFIG_DATA_32
     #define DEFAULT_START_SEC_CONFIG_DATA_32
   #endif
#elif defined (FUSA_STOP_SEC_CONFIG_DATA_32)
   #ifndef CONFIG_DATA_32_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  CONFIG_DATA_32_SEC_STARTED
     #undef  FUSA_STOP_SEC_CONFIG_DATA_32
     #define DEFAULT_STOP_SEC_CONFIG_DATA_32
   #endif

#elif defined (FUSA_START_SEC_CONFIG_DATA_UNSPECIFIED)
   #ifdef CONFIG_DATA_UNSPECIFIED_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define CONFIG_DATA_UNSPECIFIED_SEC_STARTED
     #undef  FUSA_START_SEC_CONFIG_DATA_UNSPECIFIED
     #define DEFAULT_START_SEC_CONFIG_DATA_UNSPECIFIED
   #endif
#elif defined (FUSA_STOP_SEC_CONFIG_DATA_UNSPECIFIED)
   #ifndef CONFIG_DATA_UNSPECIFIED_SEC_STARTED
     #error "Memory section is not started"
   #else
     #undef  CONFIG_DATA_UNSPECIFIED_SEC_STARTED
     #undef  FUSA_STOP_SEC_CONFIG_DATA_UNSPECIFIED
     #define DEFAULT_STOP_SEC_CONFIG_DATA_UNSPECIFIED
   #endif

#elif defined (FUSA_START_SEC_CODE_SLOW)
   #ifdef CODE_SLOW_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define CODE_SLOW_SEC_STARTED
     #undef  FUSA_START_SEC_CODE_SLOW
     #define DEFAULT_START_SEC_CODE_SLOW
   #endif
#elif defined (FUSA_STOP_SEC_CODE_SLOW)
   #ifndef CODE_SLOW_SEC_STARTED
      #error "Memory section is not started"
   #else
     #undef  CODE_SLOW_SEC_STARTED
     #undef  FUSA_STOP_SEC_CODE_SLOW
     #define DEFAULT_STOP_SEC_CODE_SLOW
   #endif

#elif defined (FUSA_START_SEC_CALLOUT_CODE)
   #ifdef CALLOUT_CODE_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define CALLOUT_CODE_SEC_STARTED
     #undef  FUSA_START_SEC_CALLOUT_CODE
     #define DEFAULT_START_SEC_CALLOUT_CODE
   #endif
#elif defined (FUSA_STOP_SEC_CALLOUT_CODE)
   #ifndef CALLOUT_CODE_SEC_STARTED
      #error "Memory section is not started"
   #else
     #undef  CALLOUT_CODE_SEC_STARTED
     #undef  FUSA_STOP_SEC_CALLOUT_CODE
     #define DEFAULT_STOP_SEC_CALLOUT_CODE
   #endif

#elif defined (FUSA_START_SEC_APPL_CODE)
   #ifdef APPL_CODE_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define APPL_CODE_SEC_STARTED
     #undef  FUSA_START_SEC_APPL_CODE
     #define DEFAULT_START_SEC_APPL_CODE
   #endif
#elif defined (FUSA_STOP_SEC_APPL_CODE)
   #ifndef APPL_CODE_SEC_STARTED
      #error "Memory section is not started"
   #else
     #undef  APPL_CODE_SEC_STARTED
     #undef  FUSA_STOP_SEC_APPL_CODE
     #define DEFAULT_STOP_SEC_APPL_CODE
   #endif

#elif defined (FUSA_START_SEC_CODE)
   #ifdef CODE_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define CODE_SEC_STARTED
     #undef  FUSA_START_SEC_CODE
     #define DEFAULT_START_SEC_CODE
   #endif

#elif defined (FUSA_STOP_SEC_CODE)
   #ifndef CODE_SEC_STARTED
      #error "Memory section is not started"
   #else
     #undef  CODE_SEC_STARTED
     #undef  FUSA_STOP_SEC_CODE
     #define DEFAULT_STOP_SEC_CODE
   #endif

#elif defined (FUSA_START_SEC_CODE_FAST)
   #ifdef CODE_FAST_SEC_STARTED
     #error "Memory section is not stopped"
   #else
     #define CODE_FAST_SEC_STARTED
     #undef  FUSA_START_SEC_CODE_FAST
     #define DEFAULT_START_SEC_CODE_FAST
   #endif
#elif defined (FUSA_STOP_SEC_CODE_FAST)
   #ifndef CODE_FAST_SEC_STARTED
      #error "Memory section is not started"
   #else
     #undef  CODE_FAST_SEC_STARTED
     #undef  FUSA_STOP_SEC_CODE_FAST
     #define DEFAULT_STOP_SEC_CODE_FAST
   #endif

/* -------------------------------------------------------------------------- */
/* End of module section mapping                                              */
/* -------------------------------------------------------------------------- */
#else
  #error "FuSa_MemMap.h: No valid section define found"
#endif  /* START_WITH_IF */


/*******************************************************************************
**                      Default section mapping                               **
*******************************************************************************/
#include "Common_MemMap.h"

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
