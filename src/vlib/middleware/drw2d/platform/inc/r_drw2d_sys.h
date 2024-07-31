/*
****************************************************************************
PROJECT : Drw2D driver
FILE    : $Id: r_drw2d_sys.h 510 2019-09-30 02:32:37Z a5110279 $
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

#ifndef R_DRW2D_SYS_H_
#define R_DRW2D_SYS_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: DRW2D Support Functions for DaveHD (internal)

  DRW2D driver support functions

  The generic DRW2D driver uses these functions. They have to be implemented 
  within the driver library for a concrete device. (e.g. D1L, D1M)

  Support functions are functions that are not part of the driver itself 
  but they must be provided to integrate the driver on a particular board.

  It is not recommended to use this API directly.
  Use R_DRW2D_API_* instead.
*/


/***********************************************************
  Section: Global Defines
*/

/***********************************************************
  Constant: R_DRW2D_UNIT_GLOBAL

  Magic unit number used when handling a global error that
  occured in <R_DRW2D_Init> or <R_DRW2D_Exit>.

  See also:
   <drw2d_ErrorHandler>, <R_DRW2D_UNIT_UNKNOWN>
*/
#define R_DRW2D_UNIT_GLOBAL  (0x80008001u)



/***********************************************************
  Constant: R_DRW2D_UNIT_UNKNOWN

  Magic unit number used when handling an error that
  occured when no valid device context was provided
  by the application and the function called did not
  specify a Unit parameter.

  See also:
   <drw2d_ErrorHandler>, <R_DRW2D_UNIT_GLOBAL>
*/
#define R_DRW2D_UNIT_UNKNOWN  (0x80008002u)


/***********************************************************
  Group: API-side dirty flag defines/constants

  The API-side dirty flags are set and evaluated on the API-side.
*/



/***********************************************************
  Constant: R_DRW2D_API_DIRTY_EFFCLIPRECT

  Indicates that the effective clipping rectangle has to be recalculated.

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_API_DIRTY_EFFCLIPRECT           ((uint16_t)(((uint16_t)1u) << ((uint16_t)0)))



/***********************************************************
  Constant: R_DRW2D_API_DIRTY_ALL

  Bitmask that combines all API-side dirty flags.

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_API_DIRTY_ALL  (   \
   (R_DRW2D_API_DIRTY_EFFCLIPRECT) \
                                 )


#define R_DRW2D_API_DIRTY_ALL_INIT  (R_DRW2D_API_DIRTY_ALL)




/***********************************************************
  Group: Sys-side dirty flag defines/constants

  The Sys-side dirty flags are usually set on the API-side and evaluated/reset on the Sys-side.
*/



/***********************************************************
  Constant: R_DRW2D_SYS_DIRTY_CLIPRECT

  Indicates that the clipping rectangle has changed.

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_SYS_DIRTY_CLIPRECT              1u



/***********************************************************
  Constant: R_DRW2D_SYS_DIRTY_FGCOLOR

  Indicates that the foreground color has changed.

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_SYS_DIRTY_FGCOLOR               2u



/***********************************************************
  Constant: R_DRW2D_SYS_DIRTY_BGCOLOR

  Indicates that the background fill color has changed.

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_SYS_DIRTY_BGCOLOR               4u



/***********************************************************
  Constant: R_DRW2D_SYS_DIRTY_FILLMODE

  Indicates that the fill mode has changed.

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_SYS_DIRTY_FILLMODE              8u



/***********************************************************
  Constant: R_DRW2D_SYS_DIRTY_BLENDMODE

  Indicates that the blending functions have changed.

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_SYS_DIRTY_BLENDMODE             16u



/***********************************************************
  Constant: R_DRW2D_SYS_DIRTY_FRAMEBUFFER

  Indicates that the framebuffer (render target) has changed.

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_SYS_DIRTY_FRAMEBUFFER           32u



/***********************************************************
  Constant: R_DRW2D_SYS_DIRTY_TEXTURE_0

  Indicates that the source texture has changed.

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_SYS_DIRTY_TEXTURE_0              128u



/***********************************************************
  Constant: R_DRW2D_SYS_DIRTY_TEXTURE_1

  Indicates that the source texture has changed.

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_SYS_DIRTY_TEXTURE_1              256u



/***********************************************************
  Constant: R_DRW2D_SYS_DIRTY_TEXTURE_CACHEXFORM

  Indicates that the static texture mapping (via TextureMatrix) has changed and
  the texture transformation cache needs to be updated.

  (note) UV based texture coordinate transformation is done per primitive

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_SYS_DIRTY_TEXTURE_CACHEXFORM    512u



/***********************************************************
  Constant: R_DRW2D_SYS_DIRTY_TEXTURE_STATICMAP

  Indicates that the static texture mapping (via TextureMatrix) has changed.

  (note) UV based texture setup is done per primitive

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_SYS_DIRTY_TEXTURE_STATICMAP     1024u



/***********************************************************
  Constant: R_DRW2D_SYS_DIRTY_IMAGEQUALITY

  Indicates that the image quality has changed.

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_SYS_DIRTY_IMAGEQUALITY          2048u



/***********************************************************
  Constant: R_DRW2D_SYS_DIRTY_CONVKERNEL1D

  Indicates that the 1D convolution kernel (preset) has changed.

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_SYS_DIRTY_CONVKERNEL1D          4096u



/***********************************************************
  Constant: R_DRW2D_SYS_DIRTY_CONVKERNEL2D

  Indicates that the 2D convolution kernel (preset) has changed.

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_SYS_DIRTY_CONVKERNEL2D          8192u



/***********************************************************
  Constant: R_DRW2D_SYS_DIRTY_EFFECTS

  Indicates that the effect stages have changed.

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_SYS_DIRTY_EFFECTS               16384u



/***********************************************************
  Constant: R_DRW2D_SYS_DIRTY_CONVKERNEL

  Indicates that the convolution kernel has changed.

  See also:
    <r_drw2d_DeviceBase_t>
*/
#define R_DRW2D_SYS_DIRTY_CONVKERNEL            32768u



/* (note) do not add brackets () or GHS MISRA check will fail */
#define R_DRW2D_SYS_DIRTY_TEXTURE_STATIC (             \
    R_DRW2D_SYS_DIRTY_TEXTURE_0          |             \
    R_DRW2D_SYS_DIRTY_TEXTURE_CACHEXFORM |             \
    R_DRW2D_SYS_DIRTY_TEXTURE_STATICMAP)               \


/* (note) do not add brackets () or GHS MISRA check will fail */
#define R_DRW2D_SYS_DIRTY_ALL_INIT (                   \
    R_DRW2D_SYS_DIRTY_CLIPRECT           |             \
    R_DRW2D_SYS_DIRTY_FGCOLOR            |             \
    R_DRW2D_SYS_DIRTY_BGCOLOR            |             \
    R_DRW2D_SYS_DIRTY_FILLMODE           |             \
    R_DRW2D_SYS_DIRTY_BLENDMODE          |             \
    R_DRW2D_SYS_DIRTY_FRAMEBUFFER        |             \
    R_DRW2D_SYS_DIRTY_IMAGEQUALITY       |             \
    R_DRW2D_SYS_DIRTY_CONVKERNEL1D       |             \
    R_DRW2D_SYS_DIRTY_CONVKERNEL2D       |             \
    R_DRW2D_SYS_DIRTY_TEXTURE_STATIC)                  \


/* (note) do not add brackets () or GHS MISRA check will fail */
#define R_DRW2D_SYS_DIRTY_ALL_NOCONV (                 \
    R_DRW2D_SYS_DIRTY_CLIPRECT           |             \
    R_DRW2D_SYS_DIRTY_FGCOLOR            |             \
    R_DRW2D_SYS_DIRTY_BGCOLOR            |             \
    R_DRW2D_SYS_DIRTY_FILLMODE           |             \
    R_DRW2D_SYS_DIRTY_BLENDMODE          |             \
    R_DRW2D_SYS_DIRTY_FRAMEBUFFER        |             \
    R_DRW2D_SYS_DIRTY_IMAGEQUALITY       |             \
    R_DRW2D_SYS_DIRTY_TEXTURE_STATIC)                  \


/* (note) do not add brackets () or GHS MISRA check will fail */
#define R_DRW2D_SYS_DIRTY_ALL_NOTEX (                  \
    R_DRW2D_SYS_DIRTY_CLIPRECT           |             \
    R_DRW2D_SYS_DIRTY_FGCOLOR            |             \
    R_DRW2D_SYS_DIRTY_BGCOLOR            |             \
    R_DRW2D_SYS_DIRTY_FILLMODE           |             \
    R_DRW2D_SYS_DIRTY_BLENDMODE          |             \
    R_DRW2D_SYS_DIRTY_FRAMEBUFFER        |             \
    R_DRW2D_SYS_DIRTY_IMAGEQUALITY)



/* (note) do not add brackets () or GHS MISRA check will fail */
#define R_DRW2D_SYS_DIRTY_ALL_UV (                     \
    R_DRW2D_SYS_DIRTY_CLIPRECT           |             \
    R_DRW2D_SYS_DIRTY_FGCOLOR            |             \
    R_DRW2D_SYS_DIRTY_BGCOLOR            |             \
    R_DRW2D_SYS_DIRTY_FILLMODE           |             \
    R_DRW2D_SYS_DIRTY_BLENDMODE          |             \
    R_DRW2D_SYS_DIRTY_FRAMEBUFFER        |             \
    R_DRW2D_SYS_DIRTY_TEXTURE_0          |             \
    R_DRW2D_SYS_DIRTY_IMAGEQUALITY)



/* (note) do not add brackets () or GHS MISRA check will fail */
#define R_DRW2D_SYS_DIRTY_ALL_CONV (                   \
    R_DRW2D_SYS_DIRTY_CLIPRECT           |             \
    R_DRW2D_SYS_DIRTY_FGCOLOR            |             \
    R_DRW2D_SYS_DIRTY_BGCOLOR            |             \
    R_DRW2D_SYS_DIRTY_FILLMODE           |             \
    R_DRW2D_SYS_DIRTY_BLENDMODE          |             \
    R_DRW2D_SYS_DIRTY_FRAMEBUFFER        |             \
    R_DRW2D_SYS_DIRTY_TEXTURE_0)



/* (note) do not add brackets () or GHS MISRA check will fail */
#define R_DRW2D_SYS_DIRTY_ALL_CONV1D (                 \
    R_DRW2D_SYS_DIRTY_ALL_CONV        |                \
    R_DRW2D_SYS_DIRTY_CONVKERNEL1D)



/* (note) do not add brackets () or GHS MISRA check will fail */
#define R_DRW2D_SYS_DIRTY_ALL_CONV2D (                 \
    R_DRW2D_SYS_DIRTY_ALL_CONV        |                \
    R_DRW2D_SYS_DIRTY_CONVKERNEL2D)



/* (note) do not add brackets () or GHS MISRA check will fail */
#define R_DRW2D_SYS_DIRTY_ALL_CONV_USER (              \
    R_DRW2D_SYS_DIRTY_ALL_CONV        |                \
    R_DRW2D_SYS_DIRTY_CONVKERNEL)



#ifdef __cplusplus
}
#endif

#endif /* R_DRW2D_SYS_H_ */
