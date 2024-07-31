/*
****************************************************************************
PROJECT : Drw2D driver
FILE    : $Id: r_drw2d_sys_dhd.h 510 2019-09-30 02:32:37Z a5110279 $
============================================================================ 
DESCRIPTION
Driver for DRW2D
============================================================================
                            C O P Y R I G H T                            
============================================================================
                           Copyright (c) 2022
                                  by 
                    Renesas Electronics (Europe) GmbH. 
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
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
Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.     

****************************************************************************
*/

#ifndef R_DRW2D_CTX_DHD_H_
#define R_DRW2D_CTX_DHD_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 Title: The reserved module of Drw2D SYS on DHD.

 Implementation of the reserved functions of Drw2DSYS on the DHD to 
 use Drw2D with CPU only.
*/



/*******************************************************************************
  Function: R_DRW2D_Platform_Init_DHD

  A reserved function for resolving links to R_DRW2D_Platform_Init_DHD().
  Called by R_DRW2D_Init.


  Parameter: none.

  Return value:
    R_DRW2D_ERR_OK - No error occurred.
    See <r_drw2d_Error_t> for the list of error codes.
*/
extern r_drw2d_Error_t R_DRW2D_Platform_Init_DHD(void);



/*******************************************************************************
  Function: R_DRW2D_Platform_Exit_DHD

  A reserved function for resolving links to R_DRW2D_Platform_Exit_DHD().
  Called by R_DRW2D_Exit.


  Parameter: none.

  Return value:
    R_DRW2D_ERR_OK - No error occurred.
    See <r_drw2d_Error_t> for the list of error codes.
*/
extern r_drw2d_Error_t R_DRW2D_Platform_Exit_DHD(void);



/*******************************************************************************
  Function: R_DRW2D_Platform_Open_DHD

  A reserved function for resolving links to R_DRW2D_Platform_Open_DHD().
  Called when "R_DRW2D_UNIT_DHD" is specified for the "Unit" of R_DRW2D_Open.
  This function is executed only if the DHD version is not linked, so the process 
  just returns "R_DRW2D_ERR_UNIT_OUTOFBOUNDS".
  Called by R_DRW2D_Open.
  When the testing, specify 0 or NULL as the arguments.


  Parameter:
              Unit - This is a parameter that is not used.
        DriverUnit - This is a parameter that is not used.
    DeviceInternal - This is a parameter that is not used.
         RetDevice - This is a parameter that is not used.

  Return value:
    R_DRW2D_ERR_UNIT_OUTOFBOUNDS - Invalid unit number.
    See <r_drw2d_Error_t> for the list of error codes.
*/
extern r_drw2d_Error_t R_DRW2D_Platform_Open_DHD(r_drw2d_Unit_t    Unit,
                                                 int32_t           DriverUnit,
                                                 void             *DeviceInternal,
                                                 r_drw2d_Device_t *RetDevice
                                                 );



/*******************************************************************************
  Function: R_DRW2D_Platform_RenderContext_Init_DHD

  A reserved function for resolving links to R_DRW2D_RenderContext_Init_DHD().
  This function will not be executed as it will result in an error on the 
  previously executed R_DRW2D_Platform_Open_DHD.
  This function is provided to prevent link errors.
  When the testing, specify NULL as the arguments.


  Parameter:
    RenderContext - This is a parameter that is not used.
       DeviceBase - This is a parameter that is not used.

  Return value: none.
*/
extern void R_DRW2D_Platform_RenderContext_Init_DHD(r_drw2d_RenderContext_t *RenderContext,
                                                    r_drw2d_DeviceBase_t    *DeviceBase
                                                    );

#ifdef __cplusplus
}
#endif

#endif /* R_DRW2D_CTX_DHD_H_ */

