/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Os.c                                                        */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2018 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This application file contains the OS Stub functions.                      */
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
/*                              R-Car H3/M3                                   */
/*============================================================================*/
/*
 * 1.0.0      30-Oct-2015     Initial Version
 * 1.1.0      07-Dec-2016     Add GetElapsedValue stub prototype.
 *                            Add EnableAllInterrupts, DisableAllInterrupts
 * 1.2.0      07-Dec-2017     - Update the implementation of
 *                            GetCounterValue, GetElapsedValue.
 *                            - Setup stub Counter by using SCMT IP.
 * 1.2.1      21-Dec-2017     Update Coverity issue.
 * 1.2.2      04-Jan-2018     Update device name
 * 1.2.3      24-Jan-2018     Add device name M3 about comment.
 */

/*******************************************************************************
**                     Include Section                                        **
*******************************************************************************/
#include "ComStack_Types.h"
#include "Os.h"

/*******************************************************************************
**                         Global Symbols                                     **
*******************************************************************************/

/*******************************************************************************
**                         Counter Setup                                      **
**               Use SCMT IP as timer for Os Counter                          **
*******************************************************************************/
/* SCMT */
#define CMSSTR      *((volatile uint16 *)(0xE6040000))
#define CMSCSR      *((volatile uint16 *)(0xE6040040))
#define CMSCNT      *((volatile uint32 *)(0xE6040044))
#define CMSCOR      *((volatile uint32 *)(0xE6040048))

/* Clock */
/* OSSCLK = 131.57 kHz */
#define CMSCSR_CKS_1    ((uint16)(0xFFFF))
#define CMSCSR_CKS_8    ((uint16)(0xFFFC))
#define CMSCSR_CKS_32   ((uint16)(0xFFFD))
#define CMSCSR_CKS_128  ((uint16)(0xFFFE))
/* Interrupt */
#define CMSCSR_CMR_EN   ((uint16)(0xFFEF))
#define CMSCSR_CMR_DIS  ((uint16)(0xFFCF))
/* Mode */
#define CMSCSR_CMM_ONE  ((uint16)(0xFEFF))
#define CMSCSR_CMM_FREE ((uint16)(0xFFFF))
/* Size */
#define CMSCSR_CMS_16   ((uint16)(0x0200))
#define CMSCSR_CMS_32   ((uint16)(0x0000))

/* Start counter */
void Os_Counter_Start()
{
  CMSSTR = 0x20;
}

/* Stop counter */
void Os_Counter_Stop()
{
  CMSSTR = 0;
}

/* Clear counter */
void Os_Counter_Clear_CNT()
{
  CMSCNT = 0;
}

/* Read counter */
uint32 Os_Counter_Read_CNT()
{
  return CMSCNT;
}

/* Configure Os_Counter */
void Os_Counter_Configure
(uint16 size, uint16 mode, uint16 divider, uint16 interrupt)
{
/* Set counter size */
  CMSCSR |= size;
  /* Set mode */
  CMSCSR &= mode;
  /* Set clock */
  CMSCSR &= divider;
  /* Set interrupt mode */
  CMSCSR &= interrupt;
  /* Clear OVF (bit 14) and CMF (bit 15) */
  CMSCSR &= 0x3FFF;
}

/* Set constant */
void Os_Counter_Set_Constant(uint32 value)
{
  CMSCOR = value;
}

/* Wait write permission */
void Os_Counter_Wait_Write_Permission()
{
  while(CMSCSR & 0x2000);
}

/* Initialize Os_Counter */
void Os_Counter_Init()
{
  Os_Counter_Stop();
  Os_Counter_Clear_CNT();
  /* 32bit mode, free running, clock/1, interrupt disable */
  /* 0.0000076s */
  Os_Counter_Configure(CMSCSR_CMS_32, CMSCSR_CMM_FREE, \
    CMSCSR_CKS_1, CMSCSR_CMR_DIS);
  Os_Counter_Set_Constant(COUNTER_MAX_VALUE);
  Os_Counter_Wait_Write_Permission();
  Os_Counter_Start();
}

/*******************************************************************************
**                        GetCounterValue                                     **
*******************************************************************************/
StatusType GetCounterValue(CounterType CounterID, TickRefType Value)
{
  if(CounterID != OsCounter0)
  {
    return (E_OS_ID);
  }
  else
  {
    *Value = Os_Counter_Read_CNT();
    return (E_OK);
  }
}

/*******************************************************************************
**                        GetElapsedValue                                     **
*******************************************************************************/
StatusType GetElapsedValue(
  CounterType CounterID,
  TickRefType Value,
  TickRefType ElapsedValue )
{
  uint32 LulCounterValue = 0;
  static TickType LulCounterValue_Mirroring = 0;
  uint8 LucTimeNoUpdateCount = 0;

  if(CounterID != OsCounter0)
  {
    return (E_OS_ID);
  }
  else if(*Value > COUNTER_MAX_VALUE)
  {
    return (E_OS_VALUE);
  }
  else
  {
    while(1)
    {
      LulCounterValue = Os_Counter_Read_CNT();
      /* Check elapsed time whether is updated or not */
      if(LulCounterValue_Mirroring == LulCounterValue)
      {
        /* Increase count */
        LucTimeNoUpdateCount++;
        /* Reached max times */
        if(OS_COMMON_MIRROR_TIMEOUT_MAX == \
                                              LucTimeNoUpdateCount)
        {
          return (E_OS_VALUE);
        }
      }
      else
      {
        if(LulCounterValue < *Value)
        {
          *ElapsedValue = LulCounterValue - *Value + COUNTER_MAX_VALUE + 1;
        }
        else
        {
          *ElapsedValue = LulCounterValue - *Value;
        }
        *Value = LulCounterValue;
        /* Save mirror value for elapsed time */
        LulCounterValue_Mirroring = LulCounterValue;
        
        return (E_OK);
      }
    }
  }
}

/*******************************************************************************
**                        Interrupt                                           **
*******************************************************************************/
#if 0
void EnableAllInterrupts(void)
{
  asm("cpsie i\n\t");
}
void DisableAllInterrupts(void)
{
  asm("cpsid i\n\t");
}
#endif
/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
