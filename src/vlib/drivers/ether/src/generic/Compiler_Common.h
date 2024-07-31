/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* File name    = Compiler_Common.h                                           */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2018-2022 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* Provision for sections for compiler macros                                 */
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
 * V1.0.0:  26-Jan-2018  : Initial Version
 * V1.1.0:  23-Mar-2022  : Modify for R-CarH3 CR7 Safe-Rendering
 */
/******************************************************************************/

#ifndef COMPILER_COMMON_H
#define COMPILER_COMMON_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

#include "Compiler_Cfg_Common.h"    /* Module specific memory and pointer classes */

/*******************************************************************************
**                      Version Information                                  **
*******************************************************************************/
#define COMPILER_AR_RELEASE_MAJOR_VERSION  4
#define COMPILER_AR_RELEASE_MINOR_VERSION  2
#define COMPILER_AR_RELEASE_REVISION_VERSION  2

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*
 * Compiler abstraction symbols
 */

#define INLINE inline
#define _INTERRUPT_

#ifndef NULL_PTR
  #define NULL_PTR  ((void *)0)
#endif

#define REGSPACE

/* AUTOMATIC used for the declaration of local pointers */
#define AUTOMATIC

/* TYPEDEF used for defining pointer types within type definitions */
#define TYPEDEF

/* Type definition of pointers to functions
   rettype     return type of the function
   ptrclass    defines the classification of the pointer's distance
   fctname     function name respectively name of the defined type
 */
#define P2FUNC(rettype, ptrclass, fctname) rettype (*fctname)

/* The compiler abstraction shall define the FUNC macro for the declaration and
   definition of functions, that ensures correct syntax of function
   declarations as required by a specific compiler. - used for API functions
   rettype     return type of the function
   memclass    classification of the function itself
 */
#define FUNC(type, memclass) memclass type

/* The compiler abstraction shall define the FUNC_P2CONST macro for the
   declaration and definition of functions returning a pointer to a constant.
   This shall ensure the correct syntax of function declarations as required by
   a specific compiler.
   rettype return type of the function
   ptrclass defines the classification of the pointer's distance
   memclass classification of the function itself
 */
#define FUNC_P2CONST(rettype, ptrclass, memclass) const rettype *

/* The compiler abstraction shall define the FUNC_P2VAR macro for the
   declaration and definition of functions returning a pointer to a variable.
   This shall ensure the correct syntax of function declarations as required by
   a specific compiler.
   rettype return type of the function
   ptrclass defines the classification of the pointer's distance
   memclass classification of the function itself
 */
#define FUNC_P2VAR(rettype, ptrclass, memclass) rettype *

/* Pointer to constant data
   ptrtype     type of the referenced data
   memclass    classification of the pointer's variable itself
   ptrclass    defines the classification of the pointer's distance
 */
#define P2CONST(ptrtype, memclass, ptrclass)  const ptrtype *

/* Pointer to variable data
   ptrtype     type of the referenced data
   memclass    classification of the pointer's variable itself
   ptrclass    defines the classification of the pointer's distance
 */
#define P2VAR(ptrtype, memclass, ptrclass) ptrtype *

/* Const pointer to variable data
   ptrtype     type of the referenced data
   memclass    classification of the pointer's variable itself
   ptrclass    defines the classification of the pointer's distance
 */
#define CONSTP2VAR(ptrtype, memclass, ptrclass) ptrtype * const

/* Const pointer to constant data
   ptrtype     type of the referenced data
   memclass    classification of the pointer's variable itself
   ptrclass    defines the classification of the pointer's distance
 */
#define CONSTP2CONST(ptrtype, memclass, ptrclass) const ptrtype * const

/* The compiler abstraction shall define the CONSTP2FUNC macro for the type
   definition of constant pointers to functions.
   rettype return type of the function
   ptrclass defines the classification of the pointer's distance
   fctname function name respectively name of the defined type
 */
#define CONSTP2FUNC(rettype, ptrclass, fctname) rettype (* const fctname)

/* ROM constant
   type        type of the constant
   memclass    classification of the constant
 */
#define CONST(type, memclass) const type memclass

/* RAM variables
   type        type of the variable
   memclass    classification of the variable
 */
#define VAR(type, memclass) type memclass

/* Inline assembler support for NOP instruction */
#define ASM_NOP() __asm("nop")

/* Inline assembler support for WFI instruction */
#define ASM_WFI() __asm("wfi")

/* Inline assembler support for DSB instruction */
#define ASM_DSB() __asm("dsb")

/* Macro is used to fix QAC error, not implementation */
#define ENABLE_INTERRUPT()             __asm("nop")
#define DISABLE_INTERRUPT()             __asm("nop")

#define NO_PROLOGUE

#define PRAGMA(x)                 _Pragma(x)

#define __HAS_FPU__

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/

#endif /* COMPILER_COMMON_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
