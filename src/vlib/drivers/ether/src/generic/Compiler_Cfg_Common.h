/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Compiler_Cfg_Common.h                                       */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2018-2019 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains compiler macros for common modules                      */
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
 * V1.0.1:  26-Mar-2018  : Add compiler abstraction macros for FuSa
 * V1.0.2:  04-Jul-2018  : Add compiler abstraction macros for CDD IPMMU
 * V1.0.3:  13-Aug-2018  : Add compiler abstraction macros for CDD RFSO
 * V1.0.4   21-Nov-2018  : Add compiler abstraction macros for CDD ICCOM
 * V1.0.5   10-May-2019  : Add compiler abstraction macros for CDD IIC
 */
#ifndef COMPILER_CFG_COMMON_H
#define COMPILER_CFG_COMMON_H

/******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Compiler_Cfg_dep.h"   /* Compiler specific attribute                */

/*******************************************************************************
**                      Version Information                                  **
*******************************************************************************/
#define COMPILER_CFG_AR_RELEASE_MAJOR_VERSION     4
#define COMPILER_CFG_AR_RELEASE_MINOR_VERSION     2
#define COMPILER_CFG_AR_RELEASE_REVISION_VERSION  2

/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/

/*******************************************************************************
 **                      Configuration data                                   **
*******************************************************************************/
/*
 * The following memory and pointer classes can be configured per module.
 * These #defines are passed to the compiler abstraction macros in Compiler.h
 *
 * Note:
 * module internal functions (statics) that get into one section
 * (together with API) shall fit into one page.
 */

/* ---------------------------------------------------------------------------*/
/*                   MCU                                                      */
/* ---------------------------------------------------------------------------*/

#define MCU_CODE_SLOW                  COMPILER_ATTRIBUTE_GENERAL
                                       /* API functions                       */
#define MCU_PUBLIC_CONST               COMPILER_ATTRIBUTE_GENERAL
                                       /* API constants                       */

#define MCU_PRIVATE_DATA               COMPILER_ATTRIBUTE_GENERAL
                                       /* Module internal data                */
#define MCU_PRIVATE_CONST              COMPILER_ATTRIBUTE_GENERAL
                                       /* Internal ROM Data                   */

#define MCU_APPL_CONST                 COMPILER_ATTRIBUTE_GENERAL
                                       /* Applications' ROM Data              */
#define MCU_APPL_DATA                  COMPILER_ATTRIBUTE_GENERAL
                                       /* Applications' RAM Data              */
#define MCU_FAST_DATA                  COMPILER_ATTRIBUTE_GENERAL
                                       /* 'Near' RAM Data                     */

#define MCU_CODE_FAST                  COMPILER_ATTRIBUTE_GENERAL
                                       /* API functions                       */
#define MCU_FAST_CONST                 COMPILER_ATTRIBUTE_GENERAL
                                       /* API constants                       */

#define MCU_CONFIG_CONST               COMPILER_ATTRIBUTE_GENERAL
                                       /* Desc. Tables -> Config-dependent    */
#define MCU_CONFIG_DATA                COMPILER_ATTRIBUTE_GENERAL
                                       /* Config. dependent (reg. size) data  */

#define MCU_INIT_DATA                  COMPILER_ATTRIBUTE_GENERAL
                                       /* Data which is initialized during
                                          Startup                             */
#define MCU_NO_INIT_DATA               COMPILER_ATTRIBUTE_GENERAL
                                       /* Data which is not initialized during
                                          Startup                             */
#define MCU_CONST                      COMPILER_ATTRIBUTE_GENERAL
                                       /* Data Constants                      */
#define MCU_VAR_INIT                   COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are initialized after every   */
                                       /* reset                               */
#define MCU_VAR_CLEARED                COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are cleared after every       */
                                       /* reset                               */
#define MCU_VAR_POWER_ON_CLEARED       COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are cleared only after power  */
                                       /* on reset                            */
#define MCU_VAR_POWER_ON_INIT          COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are initialized only after    */
                                       /* power on reset                      */
#define MCU_VAR_NO_INIT                COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are initialized by driver     */

/* ---------------------------------------------------------------------------*/
/*                   GPT                                                      */
/* ---------------------------------------------------------------------------*/

#define GPT_CODE_SLOW                  COMPILER_ATTRIBUTE_GENERAL
                                       /* API functions                       */
#define GPT_PUBLIC_CONST               COMPILER_ATTRIBUTE_GENERAL
                                       /* API constants                       */

#define GPT_PRIVATE_DATA               COMPILER_ATTRIBUTE_GENERAL
                                       /* Module internal data                */
#define GPT_PRIVATE_CONST              COMPILER_ATTRIBUTE_GENERAL
                                       /* Internal ROM Data                   */

#define GPT_NOTIFICATION_CODE          COMPILER_ATTRIBUTE_GENERAL
                                       /* callbacks name of the Application   */

#define GPT_APPL_CODE                  COMPILER_ATTRIBUTE_GENERAL
                                       /* callbacks of the Application        */
#define GPT_APPL_CONST                 COMPILER_ATTRIBUTE_GENERAL
                                       /* Applications' ROM Data              */
#define GPT_APPL_DATA                  COMPILER_ATTRIBUTE_GENERAL
                                       /* Applications' RAM Data              */
#define GPT_FAST_DATA                  COMPILER_ATTRIBUTE_GENERAL
                                       /* 'Near' RAM Data                     */

#define GPT_CODE_FAST                  COMPILER_ATTRIBUTE_GENERAL
                                       /* API functions                       */
#define GPT_FAST_CONST                 COMPILER_ATTRIBUTE_GENERAL
                                       /* API constants                       */

#define GPT_CONFIG_CONST               COMPILER_ATTRIBUTE_GENERAL
                                       /* Desc. Tables -> Config-dependent    */
#define GPT_CONFIG_DATA                COMPILER_ATTRIBUTE_GENERAL
                                       /* Config. dependent (reg. size) data  */

#define GPT_INIT_DATA                  COMPILER_ATTRIBUTE_GENERAL
                                       /* Data which is initialized during
                                          Startup                             */
#define GPT_NO_INIT_DATA               COMPILER_ATTRIBUTE_GENERAL
                                       /* Data which is not initialized during
                                          Startup                             */
#define GPT_CONST                      COMPILER_ATTRIBUTE_GENERAL
                                       /* Data Constants                      */
#define GPT_VAR_INIT                   COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are initialized after every   */
                                       /* reset                               */
#define GPT_VAR_CLEARED                COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are cleared after every       */
                                       /* reset                               */
#define GPT_VAR_POWER_ON_CLEARED       COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are cleared only after power  */
                                       /* on reset                            */
#define GPT_VAR_POWER_ON_INIT          COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are initialized only after    */
                                       /* power on reset                      */
#define GPT_VAR_NO_INIT                COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are initialized by driver     */

/* ---------------------------------------------------------------------------*/
/*                   WDG                                                      */
/* ---------------------------------------------------------------------------*/

#define WDG_59_RWDT_CODE_SLOW        COMPILER_ATTRIBUTE_GENERAL
                                       /* API functions                      */
#define WDG_59_RWDT_PUBLIC_CONST     COMPILER_ATTRIBUTE_GENERAL
                                       /* API constants                      */

#define WDG_59_RWDT_PRIVATE_DATA     COMPILER_ATTRIBUTE_GENERAL
                                       /* Module internal data               */
#define WDG_59_RWDT_PRIVATE_CONST    COMPILER_ATTRIBUTE_GENERAL
                                       /* Internal ROM Data                  */

#define WDG_59_RWDT_APPL_CONST       COMPILER_ATTRIBUTE_GENERAL
                                       /* Applications' ROM Data             */
#define WDG_59_RWDT_APPL_DATA        COMPILER_ATTRIBUTE_GENERAL
                                       /* Applications' RAM Data             */
#define WDG_59_RWDT_FAST_DATA        COMPILER_ATTRIBUTE_GENERAL
                                       /* 'Near' RAM Data                    */

#define WDG_59_RWDT_CODE_FAST        COMPILER_ATTRIBUTE_GENERAL
                                       /* API functions                      */
#define WDG_59_RWDT_FAST_CONST       COMPILER_ATTRIBUTE_GENERAL
                                       /* API constants                      */

#define WDG_59_RWDT_CONFIG_CONST     COMPILER_ATTRIBUTE_GENERAL
                                       /* Desc. Tables -> Config-dependent   */
#define WDG_59_RWDT_CONFIG_DATA      COMPILER_ATTRIBUTE_GENERAL
                                       /* Config. dependent (reg. size) data */

#define WDG_59_RWDT_INIT_DATA        COMPILER_ATTRIBUTE_GENERAL
                                       /* Data which is initialized during */
#define WDG_59_RWDT_NO_INIT_DATA     COMPILER_ATTRIBUTE_GENERAL
                                       /* Data which is not initialized during
                                      Startup */
                                        /* Data Constants                     */
#define WDG_59_RWDT_CONST            COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables  */
#define WDG_59_RWDT_VAR_INIT           COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are initialized after every   */
                                       /* reset                               */
#define WDG_59_RWDT_VAR_CLEARED        COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are cleared after every       */
                                       /* reset                               */
#define WDG_59_RWDT_VAR_POWER_ON_CLEARED COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables */
                                       /* which are cleared only after power  */
                                       /* on reset                            */
#define WDG_59_RWDT_VAR_POWER_ON_INIT  COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are initialized only after    */
                                       /* power on reset                      */
#define WDG_59_RWDT_VAR_NO_INIT      COMPILER_ATTRIBUTE_GENERAL
                                       /* which are initialized by driver    */


/* ---------------------------------------------------------------------------*/
/*                   PORT                                                     */
/* ---------------------------------------------------------------------------*/

#define PORT_CODE_SLOW                  COMPILER_ATTRIBUTE_GENERAL
                                       /* API functions                      */
#define PORT_PUBLIC_CONST               COMPILER_ATTRIBUTE_GENERAL
                                       /* API constants                      */

#define PORT_PRIVATE_DATA               COMPILER_ATTRIBUTE_GENERAL
                                       /* Module internal data               */
#define PORT_PRIVATE_CONST              COMPILER_ATTRIBUTE_GENERAL
                                       /* Internal ROM Data                  */

#define PORT_APPL_CONST                 COMPILER_ATTRIBUTE_GENERAL
                                       /* Applications' ROM Data             */
#define PORT_APPL_DATA                  COMPILER_ATTRIBUTE_GENERAL
                                       /* Applications' RAM Data             */
#define PORT_FAST_DATA                  COMPILER_ATTRIBUTE_GENERAL
                                       /* 'Near' RAM Data                    */

#define PORT_CODE_FAST                  COMPILER_ATTRIBUTE_GENERAL
                                       /* API functions                      */
#define PORT_FAST_CONST                 COMPILER_ATTRIBUTE_GENERAL
                                       /* API constants                      */

#define PORT_CONFIG_CONST               COMPILER_ATTRIBUTE_GENERAL
                                       /* Desc. Tables -> Config-dependent   */
#define PORT_CONFIG_DATA                COMPILER_ATTRIBUTE_GENERAL
                                       /* Config. dependent (reg. size) data */

#define PORT_INIT_DATA                  COMPILER_ATTRIBUTE_GENERAL
                                       /* Data which is initialized during
                                           Startup                            */
#define PORT_NO_INIT_DATA               COMPILER_ATTRIBUTE_GENERAL
                                       /* Data which is not initialized during
                                           Startup                            */
#define PORT_CONST                      COMPILER_ATTRIBUTE_GENERAL
                                       /* Data Constants                     */
#define PORT_VAR_INIT                  COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are initialized after every   */
                                       /* reset                               */
#define PORT_VAR_CLEARED               COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are cleared after every       */
                                       /* reset                               */
#define PORT_VAR_POWER_ON_CLEARED      COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are cleared only after power  */
                                       /* on reset                            */
#define PORT_VAR_POWER_ON_INIT         COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are initialized only after    */
                                       /* power on reset                      */
#define PORT_VAR_NO_INIT                COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables  */
                                        /* which are initialized by driver    */

/* ---------------------------------------------------------------------------*/
/*                   DIO                                                      */
/* ---------------------------------------------------------------------------*/

#define DIO_CODE_SLOW                  COMPILER_ATTRIBUTE_GENERAL
                                       /* API functions                       */
#define DIO_PUBLIC_CONST               COMPILER_ATTRIBUTE_GENERAL
                                       /* API constants                       */

#define DIO_PRIVATE_DATA               COMPILER_ATTRIBUTE_GENERAL
                                       /* Module internal data                */
#define DIO_PRIVATE_CONST              COMPILER_ATTRIBUTE_GENERAL
                                       /* Internal ROM Data                   */

#define DIO_APPL_CONST                 COMPILER_ATTRIBUTE_GENERAL
                                       /* Applications' ROM Data              */
#define DIO_APPL_DATA                  COMPILER_ATTRIBUTE_GENERAL
                                       /* Applications' RAM Data              */
#define DIO_FAST_DATA                  COMPILER_ATTRIBUTE_GENERAL
                                       /* 'Near' RAM Data                     */

#define DIO_CODE_FAST                  COMPILER_ATTRIBUTE_GENERAL
                                       /* API functions                       */
#define DIO_FAST_CONST                 COMPILER_ATTRIBUTE_GENERAL
                                       /* API constants                       */

#define DIO_CONFIG_CONST               COMPILER_ATTRIBUTE_GENERAL
                                       /* Desc. Tables -> Config-dependent    */
#define DIO_CONFIG_DATA                COMPILER_ATTRIBUTE_GENERAL
                                       /* Config. dependent (reg. size) data  */

#define DIO_INIT_DATA                  COMPILER_ATTRIBUTE_GENERAL
                                       /* Data which is initialized during
                                          Startup                             */
#define DIO_NO_INIT_DATA               COMPILER_ATTRIBUTE_GENERAL
                                       /* Data which is not initialized during
                                          Startup                             */
#define DIO_CONST                      COMPILER_ATTRIBUTE_GENERAL
                                       /* Data Constants                      */
#define DIO_VAR_INIT                   COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are initialized after every   */
                                       /* reset                               */
#define DIO_VAR_CLEARED                COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are cleared after every       */
                                       /* reset                               */
#define DIO_VAR_POWER_ON_CLEARED       COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are cleared only after power  */
                                       /* on reset                            */
#define DIO_VAR_POWER_ON_INIT          COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are initialized only after    */
                                       /* power on reset                      */
#define DIO_VAR_NO_INIT                COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are initialized by driver     */

/* ---------------------------------------------------------------------------*/
/*                   SPI                                                      */
/* ---------------------------------------------------------------------------*/

#define SPI_CODE_SLOW                  COMPILER_ATTRIBUTE_GENERAL
                                       /* API functions                       */
#define SPI_CODE_FAST                  COMPILER_ATTRIBUTE_GENERAL
                                       /* API functions                       */
#define SPI_APPL_CODE                  COMPILER_ATTRIBUTE_GENERAL
                                       /* callbacks of the Application        */
#define SPI_APPL_CONST                 COMPILER_ATTRIBUTE_GENERAL
                                       /* Applications' ROM Data              */
#define SPI_APPL_DATA                  COMPILER_ATTRIBUTE_GENERAL
                                       /* Applications' RAM Data              */
#define SPI_CONFIG_CONST               COMPILER_ATTRIBUTE_GENERAL
                                       /* Desc. Tables -> Config-dependent    */
#define SPI_CONFIG_DATA                COMPILER_ATTRIBUTE_GENERAL
                                       /* Config. dependent (reg. size) data  */
#define SPI_VAR_INIT                   COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are initialized after every   */
                                       /* reset                               */
#define SPI_VAR_NO_INIT                COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are initialized by driver     */
#define SPI_VAR_CLEARED                COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
                                       /* which are cleared after every       */
                                       /* reset                               */
#define SPI_VAR_CLEARED_ALIGNMENT      COMPILER_ATTRIBUTE_ALIGNMENT_32
                                       /* Memory class for global variables   */
                                       /* which are aligned                   */
#define SPI_CONST                      COMPILER_ATTRIBUTE_GENERAL
                                       /* Data Constants                      */
#define SPI_VAR                        COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */

/* ---------------------------------------------------------------------------*/
/*                   CAN                                                      */
/* ---------------------------------------------------------------------------*/

#define CAN_CODE_SLOW    COMPILER_ATTRIBUTE_GENERAL
                                       /* API functions                       */
#define CAN_APPL_CODE    COMPILER_ATTRIBUTE_GENERAL
                                       /* callbacks of the Application        */
#define CAN_APPL_CONST   COMPILER_ATTRIBUTE_GENERAL
                                       /* Applications' ROM Data              */
#define CAN_APPL_DATA    COMPILER_ATTRIBUTE_GENERAL
                                       /* Applications' RAM Data              */
#define CAN_CODE_FAST    COMPILER_ATTRIBUTE_GENERAL
                                       /* API functions                       */
#define CAN_CONFIG_DATA  COMPILER_ATTRIBUTE_GENERAL
                                       /* Config. dependent (reg. size) data  */
#define CAN_VAR_INIT     COMPILER_ATTRIBUTE_GENERAL
                                       /* Data which is initialized during
                                          Startup                             */
#define CAN_VAR_NO_INIT  COMPILER_ATTRIBUTE_GENERAL
                                       /* Data which is not initialized during
                                          Startup                             */
#define CAN_VAR_CLEARED  COMPILER_ATTRIBUTE_GENERAL
                                       /* Data which is cleared during
                                          Startup                             */
#define CAN_CONST        COMPILER_ATTRIBUTE_GENERAL
                                       /* Data Constants                      */
#define CAN_VAR          COMPILER_ATTRIBUTE_GENERAL
                                       /* Memory class for global variables   */
/* ---------------------------------------------------------------------------*/
/*                   ETH                                                      */
/* ---------------------------------------------------------------------------*/

/* API functions */
#define ETH_CODE_SLOW                  COMPILER_ATTRIBUTE_GENERAL
/* callbacks of the Application */
#define ETH_APPL_CODE                  COMPILER_ATTRIBUTE_GENERAL
/* Applications' ROM Data */
#define ETH_APPL_CONST                 COMPILER_ATTRIBUTE_GENERAL
/* Applications' RAM Data */
#define ETH_APPL_DATA                  COMPILER_ATTRIBUTE_GENERAL
/* API functions */
#define ETH_CODE_FAST                  COMPILER_ATTRIBUTE_GENERAL
/* Config. dependent (reg. size) data */
#define ETH_CONFIG_DATA                COMPILER_ATTRIBUTE_GENERAL
/* Data which is initialized during Startup */
#define ETH_VAR_INIT                   COMPILER_ATTRIBUTE_GENERAL
/* Data which is not initialized during Startup */
#define ETH_VAR_NO_INIT                COMPILER_ATTRIBUTE_GENERAL
/* Data which is cleared during Startup */
#define ETH_VAR_CLEARED                COMPILER_ATTRIBUTE_GENERAL
/* Special area of ETH which is cleared during Startup, non-cached */
#define ETH_VAR_CLEARED_DESCRIPTOR     COMPILER_ATTRIBUTE_GENERAL
/* Data Constants */
#define ETH_CONST                      COMPILER_ATTRIBUTE_GENERAL
/* Memory class for global variables */
#define ETH_VAR                        COMPILER_ATTRIBUTE_GENERAL

/* ---------------------------------------------------------------------------*/
/*                   CddIccom                                                 */
/* ---------------------------------------------------------------------------*/
/* API functions */
#define CDDICCOM_CODE                  COMPILER_ATTRIBUTE_GENERAL
/* API functions */
#define CDDICCOM_CODE_SLOW             COMPILER_ATTRIBUTE_GENERAL
/* API functions */
#define CDDICCOM_CODE_FAST             COMPILER_ATTRIBUTE_GENERAL
/* callbacks of the Application */
#define CDDICCOM_APPL_CODE             COMPILER_ATTRIBUTE_GENERAL
/* Applications' ROM Data */
#define CDDICCOM_APPL_CONST            COMPILER_ATTRIBUTE_GENERAL
/* Applications' RAM Data */
#define CDDICCOM_APPL_DATA             COMPILER_ATTRIBUTE_GENERAL
/* Config. dependent (reg. size) data */
#define CDDICCOM_CONFIG_DATA           COMPILER_ATTRIBUTE_GENERAL
/* Data which is initialized during Startup */
#define CDDICCOM_VAR_INIT              COMPILER_ATTRIBUTE_GENERAL
/* Data which is not initialized during Startup */
#define CDDICCOM_VAR_NO_INIT           COMPILER_ATTRIBUTE_GENERAL
/* Data which is cleared during Startup */
#define CDDICCOM_VAR_CLEARED           COMPILER_ATTRIBUTE_GENERAL
/* Data Constants */
#define CDDICCOM_CONST                 COMPILER_ATTRIBUTE_GENERAL
/* Memory class for global variables */
#define CDDICCOM_VAR                   COMPILER_ATTRIBUTE_GENERAL
/* Memory class for runnable entities */
#define CddIccom_CODE                  COMPILER_ATTRIBUTE_GENERAL
/* ---------------------------------------------------------------------------*/
/*                   CddIpmmu                                                 */
/* ---------------------------------------------------------------------------*/
/* TRACE [CDDIPMMU_DDD_COMPILER_001] */

/* API functions */
#define CDDIPMMU_CODE                    COMPILER_ATTRIBUTE_GENERAL
#define CDDIPMMU_CODE_SLOW               COMPILER_ATTRIBUTE_GENERAL
#define CDDIPMMU_CODE_FAST               COMPILER_ATTRIBUTE_GENERAL

/* Callback of the Application */
#define CDDIPMMU_APPL_CODE               COMPILER_ATTRIBUTE_GENERAL

/* Applications' ROM Data */
#define CDDIPMMU_APPL_CONST

/* Applications' RAM Data */
#define CDDIPMMU_APPL_DATA

/* Config. dependent (reg. size) data */
#define CDDIPMMU_CONFIG_DATA

/* Data which is initialized during Startup */
#define CDDIPMMU_VAR_INIT                COMPILER_ATTRIBUTE_GENERAL

/* Data which is not initialized during Startup */
#define CDDIPMMU_VAR_NO_INIT             COMPILER_ATTRIBUTE_GENERAL

/* Data which is cleared during Startup */
#define CDDIPMMU_VAR_CLEARED             COMPILER_ATTRIBUTE_GENERAL

/* Data Constants */
#define CDDIPMMU_CONST                   COMPILER_ATTRIBUTE_GENERAL

/* Memory class for global variables */
#define CDDIPMMU_VAR                     COMPILER_ATTRIBUTE_GENERAL

/* ---------------------------------------------------------------------------*/
/*                   CDD_RFSO                                                 */
/* ---------------------------------------------------------------------------*/
/* TRACE [CDDRFSO_DDD_COMPILER_001] */

/* API functions */
#define CDDRFSO_CODE                    COMPILER_ATTRIBUTE_GENERAL
#define CDDRFSO_CODE_SLOW               COMPILER_ATTRIBUTE_GENERAL
#define CDDRFSO_CODE_FAST               COMPILER_ATTRIBUTE_GENERAL

/* Callback of the Application */
#define CDDRFSO_APPL_CODE               COMPILER_ATTRIBUTE_GENERAL

/* Applications' ROM Data */
#define CDDRFSO_APPL_CONST

/* Applications' RAM Data */
#define CDDRFSO_APPL_DATA

/* Config. dependent (reg. size) data */
#define CDDRFSO_CONFIG_DATA

/* Data which is initialized during Startup */
#define CDDRFSO_VAR_INIT                COMPILER_ATTRIBUTE_GENERAL

/* Data which is not initialized during Startup */
#define CDDRFSO_VAR_NO_INIT             COMPILER_ATTRIBUTE_GENERAL

/* Data which is cleared during Startup */
#define CDDRFSO_VAR_CLEARED             COMPILER_ATTRIBUTE_GENERAL

/* Data Constants */
#define CDDRFSO_CONST                   COMPILER_ATTRIBUTE_GENERAL

/* Memory class for global variables */
#define CDDRFSO_VAR                     COMPILER_ATTRIBUTE_GENERAL

/* ---------------------------------------------------------------------------*/
/*                   CDDIIC                                                   */
/* ---------------------------------------------------------------------------*/
/* API functions */
#define CDDIIC_CODE                  COMPILER_ATTRIBUTE_GENERAL
/* API functions */
#define CDDIIC_CODE_SLOW             COMPILER_ATTRIBUTE_GENERAL
/* API functions */
#define CDDIIC_CODE_FAST             COMPILER_ATTRIBUTE_GENERAL
/* callbacks of the Application */
#define CDDIIC_APPL_CODE             COMPILER_ATTRIBUTE_GENERAL
/* Applications' ROM Data */
#define CDDIIC_APPL_CONST            COMPILER_ATTRIBUTE_GENERAL
/* Applications' RAM Data */
#define CDDIIC_APPL_DATA             COMPILER_ATTRIBUTE_GENERAL
/* Config. dependent (reg. size) data */
#define CDDIIC_CONFIG_DATA           COMPILER_ATTRIBUTE_GENERAL
/* Data which is initialized during Startup */
#define CDDIIC_VAR_INIT              COMPILER_ATTRIBUTE_GENERAL
/* Data which is not initialized during Startup */
#define CDDIIC_VAR_NO_INIT           COMPILER_ATTRIBUTE_GENERAL
/* Data which is cleared during Startup */
#define CDDIIC_VAR_CLEARED           COMPILER_ATTRIBUTE_GENERAL
/* Data Constants */
#define CDDIIC_CONST                 COMPILER_ATTRIBUTE_GENERAL
/* Memory class for global variables */
#define CDDIIC_VAR                   COMPILER_ATTRIBUTE_GENERAL
/* Memory class for runnable entities */
#define CddIic_CODE                  COMPILER_ATTRIBUTE_GENERAL

/* ---------------------------------------------------------------------------*/
/*                   Rte                                                      */
/* ---------------------------------------------------------------------------*/

#define RTE_CODE                  /* API functions                       */
#define RTE_CODE_SLOW             /* API functions                       */
#define RTE_CODE_FAST             /* API functions                       */
#define RTE_APPL_CODE             /* callbacks of the Application        */
#define RTE_APPL_CONST            /* Applications' ROM Data              */
#define RTE_APPL_DATA             /* Applications' RAM Data              */
#define RTE_CONFIG_DATA           /* Config. dependent (reg. size) data  */
#define RTE_VAR_INIT              /* Data which is initialized during
                                          Startup                             */
#define RTE_VAR_NO_INIT           /* Data which is not initialized during
                                          Startup                             */
#define RTE_VAR_CLEARED           /* Data which is cleared during
                                          Startup                             */
#define RTE_CONST                 /* Data Constants                      */
#define RTE_VAR                   /* Memory class for global variables   */

/* ---------------------------------------------------------------------------*/
/*                   FuSa                                                     */
/* ---------------------------------------------------------------------------*/

/* API functions */
#define FUSA_CODE_SLOW    COMPILER_ATTRIBUTE_GENERAL
/* API functions */
#define FUSA_CODE_FAST    COMPILER_ATTRIBUTE_GENERAL

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/


#endif /* COMPILER_CFG_COMMON_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
