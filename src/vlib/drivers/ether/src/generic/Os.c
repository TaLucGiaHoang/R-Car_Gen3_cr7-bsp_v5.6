/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Os.c                                                        */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2022 Renesas Electronics Corporation                     */
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
 * 1.2.4      17-Apr-2020     Change asm to __arm inside function
 *                            EnableAllInterrupts and DisableAllInterrupts
 *                            to satisfy compiler option -std=c99
 * 1.3.0      23-Mar-2022     Modify for R-CarH3 CR7 Safe-Rendering
 */

/*******************************************************************************
**                     Include Section                                        **
*******************************************************************************/
#include "ComStack_Types.h"
#include "Os.h"
#include "rcar-xos/osal/r_osal.h"
#include "r_print_api.h"

#define ETH_MUTEX_TIMEOUT_MS 3000 /* 3000 milisecond */

/*******************************************************************************
**                         Global Symbols                                     **
*******************************************************************************/
extern osal_mutex_handle_t Ether_mutex_handle;
/*******************************************************************************
**                         Counter Setup                                      **
**               Use OSAL for Os Counter                                      **
*******************************************************************************/
/* Read counter */
uint32 Os_Counter_Read_CNT()
{
  st_osal_time_t osal_time;
  uint32 count;
  e_osal_return_t osal_ret;
  osal_ret = R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &osal_time);
  if(osal_ret != OSAL_RETURN_OK)
  {
    R_PRINT_Log("Error in OSAL_Counter_Read_CNT ret=%d", osal_ret);
  }
  count = (uint32)((osal_time.tv_sec * 1000) + (osal_time.tv_nsec / 1000000)); /* Convert 1ms to 1 clock */

  return count;
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
  uint32 LucTimeNoUpdateCount = 0;

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
          /* return (E_OS_VALUE); // ToDo: Need to confirm expected OS_COMMON_MIRROR_TIMEOUT_MAX value */
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

#ifdef USE_FREERTOS
/*******************************************************************************
**                        Interrupt                                           **
*******************************************************************************/
void EnableAllInterrupts(void)
{
  R_OSAL_CriticalExit();
}
void DisableAllInterrupts(void)
{
  R_OSAL_CriticalEnter();
}
#endif
/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
