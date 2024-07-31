/****************************************************************************
PROJECT : VLIB (device and OS adaptation)
FILE    : $Id: r_dhd_sys_mem.h 287 2018-03-13 12:23:51Z a5089413 $
============================================================================ 
DESCRIPTION
Malloc wrapper 
============================================================================
                            C O P Y R I G H T                                    
============================================================================
                           Copyright (c) 2016
                                  by 
                       Renesas Electronics (Europe) GmbH. 
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
Purpose: only for testing

DISCLAIMER                                                                   
This software is supplied by Renesas Electronics Corporation and is only     
intended for use with Renesas products. No other uses are authorized. This   
software is owned by Renesas Electronics Corporation and is protected under  
all applicable laws, including copyright laws.                               
THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING  
THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT      
LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE   
AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.          
TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS       
ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE  
FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR   
ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE  
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.                             
Renesas reserves the right, without notice, to make changes to this software 
and to discontinue the availability of this software. By using this software,
you agree to the additional terms and conditions found by accessing the      
following link:                                                              
http://www.renesas.com/disclaimer *                                          
Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.     

****************************************************************************
*/

/****************************************************************************
  Title: r_dhd_sys_mem.h

 Wrapper for malloc, free an realloc 

*/

#ifndef R_DAVEHD_SYS_MEM_H
#define R_DAVEHD_SYS_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Function: R_DHD_SYS_Malloc

  Description:
  Allocate memory from heap 
  
  Parameter:
    Sz      - Amount of required memoryin bytes
  
  Return value:
  pointer to the allocated buffer
*/
void * R_DHD_SYS_Malloc(size_t Sz);


/***********************************************************
  Function: R_DHD_SYS_Free

  Description:
  Free the memory given by a pointer 

  Parameter:
    AMemory      - Pointe to the memory are that shall be freed
  
  Return value:
  none
*/
void R_DHD_SYS_Free(void *AMemory);


/***********************************************************
  Function: dhd_sys_clear_mem

  Clear memory area (fill with 0).

  Parameters:
  Addr           - Start address of allocated memory.
  NumBytes       - Number of bytes to clear.

  Returns:
  none
*/
void dhd_sys_clear_mem(void     *Addr,
                       uint32_t  NumBytes
                       );


/***********************************************************
  Function: dhd_sys_copy_mem

  Copy memory area.

  Parameters:
  DestAddr       - Start address of allocated memory.
  SrcAddr        - Start address of copy source memory.
  NumBytes       - Number of bytes to copy.

  Returns:
  none
*/
void dhd_sys_copy_mem(void       *DestAddr,
                      const void *SrcAddr,
                      uint32_t    NumBytes
                      );


#ifdef __cplusplus
}
#endif
#endif /* R_DAVEHD_SYS_MEM_H*/

