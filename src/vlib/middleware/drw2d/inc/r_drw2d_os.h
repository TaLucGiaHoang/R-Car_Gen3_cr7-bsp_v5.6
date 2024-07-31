/*
****************************************************************************
PROJECT : Drw2D driver
FILE    : $Id: r_drw2d_os.h 510 2019-09-30 02:32:37Z a5110279 $
============================================================================
DESCRIPTION
Driver for DRW2D
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2016 - 2022
                                  by
                       Renesas Electronics (Europe) GmbH.
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
Purpose: only for testing, not for mass production

DISCLAIMER

LICENSEE has read, understood and accepted the terms and conditions defined in
the license agreement, especially the usage rights. In any case, it is
LICENSEE's responsibility to make sure that any user of the software complies
with the terms and conditions of the signed license agreement.

SAMPLE CODE is not part of the licensed software, as such it must not be used in
mass-production applications. It can only be used for evaluation and
demonstration purposes at customer's premises listed in the signed license
agreement.

****************************************************************************
*/

#ifndef R_DRW2D_OS_H_
#define R_DRW2D_OS_H_

#ifdef __cplusplus
extern "C" {
#endif


/* (note) Since the API must not use dynamic memory allocation we need to include the OS header 
          file(s) to reserve memory for OS objects (e.g. CRITICAL_SECTION).
 */
#if defined(R_DRW2D_OS_WIN32)
#include <windows.h>
#elif defined(R_DRW2D_OS_RENESAS)
#include <r_os_api.h>
#elif defined(R_DRW2D_OS_INTEGRITY)
/* Disable MISRA checks for GHS Integrity - it's already perfect by default ;) */
#if defined (__ghs__)
#pragma ghs startnomisra
#endif /* __ghs__ */
#include <INTEGRITY.h>
#include "r_typedefs.h"
#if defined (__ghs__)
#pragma ghs endnomisra
#endif
#elif defined(R_DRW2D_OSAL)
#include "r_osal.h"
#elif defined(R_DRW2D_OS_NO_OS)
#else
#define R_DRW2D_OS_NO_OS
#endif



/***********************************************************
  Title: DRW2D OS abstraction

  DRW2D OS functions.

  Declaration of DRW2D OS functions.

  The generic DRW2D driver uses these functions. They have to be implemented
  within the driver library for a concrete device. (e.g. D1L, D1M)

  Support functions are functions that are not part of the driver itself 
  but they must be provided to integrate the driver on a particular board.
*/



/*******************************************************************************
  Section: Global Defines
*/



/***********************************************************
  Enum: r_drw2d_OS_Error_t

  DRW2D OS abstraction layer error code.

  Values:
    R_DRW2D_OS_ERR_OK             - No error occurred.
    R_DRW2D_OS_ERR_NOT_SUPPORTED  - (Currently) unsupported feature
    R_DRW2D_OS_ERR_PARAM_NOT_SUPPORTED  - unsupported parameter
    R_DRW2D_OS_ERR_THREAD_CREATE  - Failed to create OS-specific thread
    R_DRW2D_OS_ERR_THREAD_INFO    - Invalid thread info
    R_DRW2D_OS_ERR_THREAD_HANDLE  - Invalid thread handle
    R_DRW2D_OS_ERR_MUTEX_HANDLE   - Invalid mutex handle
    R_DRW2D_OS_ERR_MUTEX_CREATE   - Failed to create mutex
    R_DRW2D_OS_ERR_MUTEX_DESTROY  - Failed to destroy mutex
    R_DRW2D_OS_ERR_MUTEX_LOCK     - Failed to lock mutex
    R_DRW2D_OS_ERR_MUTEX_UNLOCK   - Failed to unlock mutex
*/
typedef enum
{
    R_DRW2D_OS_ERR_OK,
    R_DRW2D_OS_ERR_NOT_SUPPORTED,
    R_DRW2D_OS_ERR_PARAM_NOT_SUPPORTED,
    R_DRW2D_OS_ERR_THREAD_CREATE,
    R_DRW2D_OS_ERR_THREAD_INFO,
    R_DRW2D_OS_ERR_THREAD_HANDLE,
    R_DRW2D_OS_ERR_MUTEX_HANDLE,
    R_DRW2D_OS_ERR_MUTEX_CREATE,
    R_DRW2D_OS_ERR_MUTEX_DESTROY,
    R_DRW2D_OS_ERR_MUTEX_LOCK,
    R_DRW2D_OS_ERR_MUTEX_UNLOCK,

    NUM_R_DRW2D_OS_ERR_CODES
} r_drw2d_OS_Error_t;



/***********************************************************
  Enum: r_drw2d_OS_Mem_t

  DRW2D framework OS abstraction layer memory allocation type.

  Values:
    R_DRW2D_OS_MEM_CPU   - Memory to be used by the CPU only
    R_DRW2D_OS_MEM_VIDEO - Memory to be used as video memory
*/
typedef enum
{
    R_DRW2D_OS_MEM_CPU   = 0,
    R_DRW2D_OS_MEM_VIDEO = 1
} r_drw2d_OS_Mem_t;



/***********************************************************
  Type: r_drw2d_OS_Mutex_t

  Abstract type representing an OS mutex .
*/
#if defined(R_DRW2D_OS_WIN32)
typedef CRITICAL_SECTION r_drw2d_OS_Mutex_t;
#elif defined(R_DRW2D_OS_RENESAS)
typedef R_OS_ThreadMutex_t r_drw2d_OS_Mutex_t;
#elif defined(R_DRW2D_OS_INTEGRITY)
typedef LocalMutex r_drw2d_OS_Mutex_t;
#elif defined(R_DRW2D_OSAL)
typedef osal_mutex_handle_t r_drw2d_OS_Mutex_t;
#elif defined(R_DRW2D_OS_NO_OS)
typedef void* r_drw2d_OS_Mutex_t;
#else
#error Unable to determine r_drw2d_OS_Mutex_t
#endif


/***********************************************************
  Section: Global Functions
*/


/***********************************************************
  Group: Thread functions
*/


/***********************************************************
  Function: R_DRW2D_OS_Mutex_Create

  Creates and initializes an OS mutex.

  This function is empty as default.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  If you want to customize depending on the OS environment,
  please implement a function that creates the mutex.

  Parameters:
  Mutex - Reference to OS-specific mutex handle or structure, see <r_drw2d_OS_Mutex_t>.

  Returns:
  See <r_drw2d_OS_Error_t>.

*/
r_drw2d_OS_Error_t R_DRW2D_OS_Mutex_Create (r_drw2d_OS_Mutex_t *Mutex);

/***********************************************************
  Function: R_DRW2D_OS_Mutex_Destroy

  Destroys a mutex.

  This function is empty as default.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  If you want to customize depending on the OS environment,
  please implement a function that destroys the mutex.

  Parameters:
  Mutex - The mutex to be destroyed.

  Returns:
  See <r_drw2d_OS_Error_t>.

*/
r_drw2d_OS_Error_t R_DRW2D_OS_Mutex_Destroy (r_drw2d_OS_Mutex_t *Mutex);

/***********************************************************
  Function: R_DRW2D_OS_Mutex_Lock

  Locks a mutex.

  This function is empty as default.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  If you want to customize depending on the OS environment,
  please implement a function that locks the mutex.

  Parameters:
  Mutex - The mutex to be locked.

  Returns:
  See <r_drw2d_OS_Error_t>.

*/
r_drw2d_OS_Error_t R_DRW2D_OS_Mutex_Lock (r_drw2d_OS_Mutex_t *Mutex);

/***********************************************************
  Function: R_DRW2D_OS_Mutex_Unlock

  Unlocks a mutex.

  This function is empty as default.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  If you want to customize depending on the OS environment,
  please implement a function that unlocks the mutex.

  Parameters:
  Mutex - The mutex to be unlocked.

  Returns:
  See <r_drw2d_OS_Error_t>.

*/
r_drw2d_OS_Error_t R_DRW2D_OS_Mutex_Unlock (r_drw2d_OS_Mutex_t *Mutex);


/***********************************************************
  Group: Memory utility functions
*/


/***********************************************************
  Function: R_DRW2D_OS_Mem_Alloc

  Allocate memory.

  This function is return 0 as default.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  If you want to customize depending on the OS environment,
  please implement a function that allocates the memory.

  Parameters:
   NumBytes - Number of bytes to allocate
  Alignment - Address alignment (power of two) (0/1=any, 2=align to 16bit word boundary,
              4=align to 32bit dword boundary, ..)
       Type - R_DRW2D_OS_MEM_CPU (CPU usage only) or R_DRW2D_OS_MEM_VIDEO (CPU usage)

  Returns:
  none.

*/
void *R_DRW2D_OS_Mem_Alloc (uint32_t NumBytes, uint32_t Alignment, r_drw2d_OS_Mem_t Type);

/***********************************************************
  Function: R_DRW2D_OS_Mem_Free

  Free memory previously allocated with <R_DRW2D_OS_Mem_Alloc>.

  This function is empty as default.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  If you want to customize depending on the OS environment,
  please implement a function that frees the memory.

  Parameters:
  Addr - Start address of allocated memory

  Returns:
  None

*/
void R_DRW2D_OS_Mem_Free (void *Addr);

/***********************************************************
  Function: R_DRW2D_OS_Mem_Clear

  Clear memory area (fill with 0)

  This function is empty as default.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  If you want to customize depending on the OS environment,
  please implement a function that clears the memory.

  Parameters:
      Addr - Start address of allocated memory
  NumBytes - Number of bytes to clear

  Returns:
  None

*/
void R_DRW2D_OS_Mem_Clear (void *Addr, uint32_t NumBytes);

/***********************************************************
  Function: R_DRW2D_OS_Mem_Copy

  Copy memory area

  This function is empty as default.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  If you want to customize depending on the OS environment,
  please implement a function that copies the memory.

  Parameters:
  DestAddr - Buffer to copy to
   SrcAddr - Buffer to copy from
  NumBytes - Number of bytes to copy

  Returns:
  None

*/
void R_DRW2D_OS_Mem_Copy (void *DestAddr, const void *SrcAddr, uint32_t NumBytes);

/***********************************************************
  Function: R_DRW2D_OS_Sqrt

  Calculate the square-root of a floating point number.

  Parameters:
  val - Floating point number to calculate the square root.

  Returns:
  result of the square-root
*/
float64_t R_DRW2D_OS_Sqrt (float64_t val);


#ifdef __cplusplus
}
#endif 

#endif /* R_DRW2D_OS_H_ */
