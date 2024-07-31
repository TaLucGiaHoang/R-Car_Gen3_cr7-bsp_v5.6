/*
****************************************************************************
PROJECT : Drw2D driver
FILE    : $Id: r_drw2d_api.h 17204 2020-06-03 02:57:47Z shinya.tomari.wj@renesas.com $
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

DISCLAIMER

LICENSEE has read, understood and accepted the terms and conditions defined in
the license agreement, especially the usage rights. In any case, it is
LICENSEE's responsibility to make sure that any user of the software complies
with the terms and conditions of the signed license agreement.

****************************************************************************
*/

#ifndef R_DRW2D_API_H_
#define R_DRW2D_API_H_

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  Title: DRW2D API

  DRW2D (simple and slim 2D rendering) API.

  DRW2D controls DHD (DaveHD) driver and doesn't accesses H/W register directly. 
  DHD driver controls GPU2D H/W macro and accesses only register of GPU2D H/W macro.
*/




/***********************************************************
  Constant: API Version

  Major and minor version number and patchlevel

  - R_DRW2D_VERSION_MAJOR
  - R_DRW2D_VERSION_MINOR
  - R_DRW2D_VERSION_PATCH
*/

#define R_DRW2D_VERSION_MAJOR  1
#define R_DRW2D_VERSION_MINOR  9
#define R_DRW2D_VERSION_PATCH  0

/***************************************************************************
  Section: Global Types
*/

/***************************************************************************
  Group: Return codes
*/


/*******************************************************************************
  Enum: r_drw2d_Error_t

  Return codes used in almost all API functions.


  The lower 16 bits of an error code are used to encode detailed, possibly
  device-specific, error information.

  The <R_DRW2D_ERROR_CLASS> macro can be used to mask out the detailed error sub-code.


  Values:
    R_DRW2D_ERR_OK                        - No error occured.

    R_DRW2D_ERR_SYS                       - General system failure
    R_DRW2D_ERR_SYS_MUTEX_LOCK            - Failed to lock mutex
    R_DRW2D_ERR_SYS_MUTEX_UNLOCK          - Failed to unlock mutex
    R_DRW2D_ERR_SYS_MUTEX_CREATE          - Failed to create mutex
    R_DRW2D_ERR_SYS_MUTEX_DESTROY         - Failed to destroy mutex

    R_DRW2D_ERR_NOT_SUPPORTED             - Parameter/argument value or function not supported.

    R_DRW2D_ERR_INVALID_VALUE                                  - Parameter/argument value is out of range or undefined.
    R_DRW2D_ERR_INVALID_VALUE_NULLPTR                          - Parameter pointer argument is NULL
    R_DRW2D_ERR_INVALID_VALUE_FILLMODE                         - Invalid fill mode
    R_DRW2D_ERR_INVALID_VALUE_CULLMODE                         - Invalid cull mode
    R_DRW2D_ERR_INVALID_VALUE_LINEJOIN                         - Invalid LineJoin type
    R_DRW2D_ERR_INVALID_VALUE_LINECAP                          - Invalid LineCap type
    R_DRW2D_ERR_INVALID_VALUE_LINEWIDTH                        - Invalid line width (<= 0)
    R_DRW2D_ERR_INVALID_VALUE_MITERLIMIT                       - Invalid miter limit (<= 0)
    R_DRW2D_ERR_INVALID_VALUE_IMGQUALITY                       - Invalid image quality mode
    R_DRW2D_ERR_INVALID_VALUE_BLENDMODE                        - Invalid blend mode
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_SRCRGB               - Invalid SrcRGB blend factor
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_DSTRGB               - Invalid DstRGB blend factor
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_SRCALPHA             - Invalid SrcAlpha blend factor
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_DSTALPHA             - Invalid DstAlpha blend factor
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_SRCRGB_UNSUPPORTED   - Unsupported SrcRGB blend factor
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_DSTRGB_UNSUPPORTED   - Unsupported DstRGB blend factor
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_SRCALPHA_UNSUPPORTED - Unsupported SrcAlpha blend factor
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_DSTALPHA_UNSUPPORTED - Unsupported DstAlpha blend factor
    R_DRW2D_ERR_INVALID_VALUE_GPUFINISH                        - Invalid finish type 
                                                                 (<R_DRW2D_GpuFinish>)
    R_DRW2D_ERR_INVALID_VALUE_TRANSFORMMODE                    - Invalid vertex matrix transform 
                                                                 mode (<R_DRW2D_CtxTransformMode>)
    R_DRW2D_ERR_INVALID_VALUE_VIEWPORT_X                       - Invalid viewport Pos.X
    R_DRW2D_ERR_INVALID_VALUE_VIEWPORT_Y                       - Invalid viewport Pos.Y
    R_DRW2D_ERR_INVALID_VALUE_VIEWPORT_W                       - Invalid viewport Size.Width
    R_DRW2D_ERR_INVALID_VALUE_VIEWPORT_H                       - Invalid viewport Size.Height
    R_DRW2D_ERR_INVALID_VALUE_DRAWLINES_ODDPOINTCOUNT          - Odd number of points passed to 
                                                                 <R_DRW2D_DrawLines>
    R_DRW2D_ERR_INVALID_VALUE_PERF_TYPE                        - Invalid performance counter type
    R_DRW2D_ERR_INVALID_VALUE_POLYLINE_COUNT                   - Invalid polyline point count 
                                                                 (0 or 1)
    R_DRW2D_ERR_INVALID_VALUE_POLYBEZIER_COUNT                 - Invalid bezier curves point count 
                                                                 (0, 1 or 2)
    R_DRW2D_ERR_INVALID_VALUE_CONVKERNELPRESET1D               - Invalid 1D convolution preset
    R_DRW2D_ERR_INVALID_VALUE_CONVKERNELPRESET2D               - Invalid 2D convolution preset

    R_DRW2D_ERR_UNIT                      - General unit error
    R_DRW2D_ERR_UNIT_OUTOFBOUNDS          - Invalid unit number

    R_DRW2D_ERR_DEVICE                    - General device error
    R_DRW2D_ERR_DEVICE_INIT               - Failed to initialize device context
    R_DRW2D_ERR_DEVICE_HANDLE             - Invalid device handle
    R_DRW2D_ERR_DEVICE_INTERNAL           - Internal device driver error
    R_DRW2D_ERR_DEVICE_INTERNAL_SWIZZLEVT - Internal device driver error 
                                            (trying to mix swizzling + virtual tiling)
    R_DRW2D_ERR_DEVICE_INTERNAL_FINISH    - Internal device driver error (during finish)
    R_DRW2D_ERR_DEVICE_INTERNAL_SHUTDOWN  - Internal device driver error (during shutdown)
    R_DRW2D_ERR_DEVICE_INTERNAL_ALLOCCLUT - Internal device driver error (while handling CLUT memory)
    R_DRW2D_ERR_DEVICE_INTERNAL_FLUSH     - Internal device driver error (during flush)
    R_DRW2D_ERR_DEVICE_INTERNAL_CMDLIST   - Internal device driver error (during cmdlist create/..)
    R_DRW2D_ERR_DEVICE_HWINSTANCENR       - Invalid instance (hw unit) nr
    R_DRW2D_ERR_DEVICE_PIXELFMT           - Pixel format not supported by device
    R_DRW2D_ERR_DEVICE_OUTOFVIDMEM        - Failed to allocate video memory
    R_DRW2D_ERR_DEVICE_NATIVEDRVHANDLE    - Failed to query native driver handle
    R_DRW2D_ERR_DEVICE_SAVESTATEALLOC     - Failed to create save state
    R_DRW2D_ERR_DEVICE_SAVESTATE          - Failed to backup low level driver state
    R_DRW2D_ERR_DEVICE_RESTORESTATE       - Failed to restore low level driver state

    R_DRW2D_ERR_CONTEXT                   - General render context error
    R_DRW2D_ERR_CONTEXT_NOTINUSE          - Context not in use.
                                            (while calling <R_DRW2D_ContextSelect>,
                                             <R_DRW2D_ContextInit> has to be called first.)

    R_DRW2D_ERR_FRAMEBUFFER               - General framebuffer error
    R_DRW2D_ERR_FRAMEBUFFER_HANDLE        - Invalid framebuffer handle
    R_DRW2D_ERR_FRAMEBUFFER_ADDR          - Invalid framebuffer address

    R_DRW2D_ERR_BUFFER                    - General (pixel-) buffer error (framebuffer or texture)
    R_DRW2D_ERR_BUFFER_PIXELFMT           - Invalid/unsupported pixel format
    R_DRW2D_ERR_BUFFER_WIDTH              - Invalid/unsupported width
    R_DRW2D_ERR_BUFFER_HEIGHT             - Invalid/unsupported height
    R_DRW2D_ERR_BUFFER_ALLINUSE           - All buffers in use
    R_DRW2D_ERR_BUFFER_ALIGNMENT          - Buffer alignment not correct (for framebuffer)

    R_DRW2D_ERR_TEXTURE                   - General texture error
    R_DRW2D_ERR_TEXTURE_ADDR              - Invalid texture buffer address
    R_DRW2D_ERR_TEXTURE_PIXELFMT          - Unsupported texel format
    R_DRW2D_ERR_TEXTURE_RLE_BPP           - Bits per texel not suitable for RLE decoder 
                                            (DaveHD specific)
    R_DRW2D_ERR_TEXTURE_TRANSFORMMODE     - Invalid texture matrix transform mode 
                                            (<R_DRW2D_CtxTextureTransformMode>)
    R_DRW2D_ERR_TEXTURE_WIDTH             - Invalid texture width
    R_DRW2D_ERR_TEXTURE_HEIGHT            - Invalid texture height
    R_DRW2D_ERR_TEXTURE_UNIT              - Invalid texture unit number

    R_DRW2D_ERR_DRAWING                   - Drawing error
    R_DRW2D_ERR_DRAWING_DRAWTRI           - Failed to draw triangle
    R_DRW2D_ERR_DRAWING_DRAWTRIUV         - Failed to draw UV texture mapped triangle
    R_DRW2D_ERR_DRAWING_DRAWRECT          - Failed to draw rectangle
    R_DRW2D_ERR_DRAWING_DRAWRECTUV        - Failed to draw UV texture mapped rectangle
    R_DRW2D_ERR_DRAWING_DRAWQUAD          - Failed to draw quad

    R_DRW2D_ERR_COMMANDLIST               - Commandlist error
    R_DRW2D_ERR_COMMANDLIST_RETHANDLE     - Invalid commandlist handle return address
    R_DRW2D_ERR_COMMANDLIST_HANDLE        - Invalid commandlist handle (not allocated, faulty, 
                                            can not record, ..)
    R_DRW2D_ERR_COMMANDLIST_CBKNULLPTR    - Commandlist callback function null ptr

    R_DRW2D_ERR_PERF                      - Performance counter error
    R_DRW2D_ERR_PERF_ALLOC                - Failed to allocate performance counters 
                                            (used by other ctx?)
    R_DRW2D_ERR_PERF_FREE                 - Failed to free performance counters
    R_DRW2D_ERR_PERF_NOTAVAIL             - Performance counter is not available 
                                            (due to hw-limit or sw-config.)
    R_DRW2D_ERR_PERF_READ                 - Failed to query performance counter value.
    R_DRW2D_ERR_PERF_RESET                - Failed to reset performance counter.

    R_DRW2D_ERR_EFFECT                    - Effect API error
    R_DRW2D_ERR_EFFECT_INVALID_OPERAND    - Invalid Parameters provided for effects.
    R_DRW2D_ERR_EFFECT_OUT_OF_RESOURCES   - Combination of effects cannot be realized.
    R_DRW2D_ERR_EFFECT_INVALID_OPERATION  - Invalid Efefct Name.
    R_DRW2D_ERR_EFFECT_DIV_BY_ZERO        - Attempted Division by zero
                                            (Two identical points provided to Gradient Effect).
    R_DRW2D_ERR_EFFECT_INVALID_TEXTURE    - Invalid Texture Index.

    R_DRW2D_ERR_CONVOLUTION               - Convolution API error
    R_DRW2D_ERR_CONVOLUTION_ADDR          - Invalid coefficient array address.
    R_DRW2D_ERR_CONVOLUTION_DIMENSION     - Invalid kernel dimensions.
    R_DRW2D_ERR_CONVOLUTION_RES_CONFLICT  - Resource conflict. Color unit already in use by effect.
    R_DRW2D_ERR_CONVOLUTION_INVALID_PARAM - Invalid parameter.
*/
typedef enum
{
    R_DRW2D_ERR_OK                                             =  0,

    R_DRW2D_ERR_SYS                                            =  0x00010000,
    R_DRW2D_ERR_SYS_MUTEX_LOCK                                 =  0x00010010,
    R_DRW2D_ERR_SYS_MUTEX_UNLOCK                               =  0x00010011,
    R_DRW2D_ERR_SYS_MUTEX_CREATE                               =  0x00010012,
    R_DRW2D_ERR_SYS_MUTEX_DESTROY                              =  0x00010013,

    R_DRW2D_ERR_NOT_SUPPORTED                                  =  0x00020000,

    R_DRW2D_ERR_INVALID_VALUE                                  =  0x00030000,
    R_DRW2D_ERR_INVALID_VALUE_NULLPTR                          =  0x00030010,
    R_DRW2D_ERR_INVALID_VALUE_FILLMODE                         =  0x00030020,
    R_DRW2D_ERR_INVALID_VALUE_CULLMODE                         =  0x00030030,
    R_DRW2D_ERR_INVALID_VALUE_LINEJOIN                         =  0x00030040,
    R_DRW2D_ERR_INVALID_VALUE_LINECAP                          =  0x00030050,
    R_DRW2D_ERR_INVALID_VALUE_LINEWIDTH                        =  0x00030060,
    R_DRW2D_ERR_INVALID_VALUE_MITERLIMIT                       =  0x00030070,
    R_DRW2D_ERR_INVALID_VALUE_IMGQUALITY                       =  0x00030080,
    R_DRW2D_ERR_INVALID_VALUE_BLENDMODE                        =  0x00030090,
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_SRCRGB               =  0x000300A1,
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_DSTRGB               =  0x000300A2,
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_SRCALPHA             =  0x000300A3,
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_DSTALPHA             =  0x000300A4,
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_SRCRGB_UNSUPPORTED   =  0x000300A5,
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_DSTRGB_UNSUPPORTED   =  0x000300A6,
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_SRCALPHA_UNSUPPORTED =  0x000300A7,
    R_DRW2D_ERR_INVALID_VALUE_BLENDFACTOR_DSTALPHA_UNSUPPORTED =  0x000300A8,
    R_DRW2D_ERR_INVALID_VALUE_GPUFINISH                        =  0x000300B0,
    R_DRW2D_ERR_INVALID_VALUE_TRANSFORMMODE                    =  0x000300C0,
    R_DRW2D_ERR_INVALID_VALUE_VIEWPORT_X                       =  0x000300D1,
    R_DRW2D_ERR_INVALID_VALUE_VIEWPORT_Y                       =  0x000300D2,
    R_DRW2D_ERR_INVALID_VALUE_VIEWPORT_W                       =  0x000300D3,
    R_DRW2D_ERR_INVALID_VALUE_VIEWPORT_H                       =  0x000300D4,
    R_DRW2D_ERR_INVALID_VALUE_DRAWLINES_ODDPOINTCOUNT          =  0x000300E0,
    R_DRW2D_ERR_INVALID_VALUE_PERF_TYPE                        =  0x000300F0,
    R_DRW2D_ERR_INVALID_VALUE_POLYLINE_COUNT                   =  0x00030100,
    R_DRW2D_ERR_INVALID_VALUE_POLYBEZIER_COUNT                 =  0x00030110,
    R_DRW2D_ERR_INVALID_VALUE_CONVKERNELPRESET1D               =  0x00030120,
    R_DRW2D_ERR_INVALID_VALUE_CONVKERNELPRESET2D               =  0x00030130,

    R_DRW2D_ERR_UNIT                                           =  0x00050000,
    R_DRW2D_ERR_UNIT_OUTOFBOUNDS                               =  0x00050010,

    R_DRW2D_ERR_DEVICE                                         =  0x00060000,
    R_DRW2D_ERR_DEVICE_INIT                                    =  0x00060010,
    R_DRW2D_ERR_DEVICE_HANDLE                                  =  0x00060020,
    R_DRW2D_ERR_DEVICE_INTERNAL                                =  0x00060030,
    R_DRW2D_ERR_DEVICE_INTERNAL_SWIZZLEVT                      =  0x00060031,
    R_DRW2D_ERR_DEVICE_INTERNAL_FINISH                         =  0x00060032,
    R_DRW2D_ERR_DEVICE_INTERNAL_SHUTDOWN                       =  0x00060033,
    R_DRW2D_ERR_DEVICE_INTERNAL_ALLOCCLUT                      =  0x00060034,

    R_DRW2D_ERR_DEVICE_INTERNAL_FLUSH                          =  0x00060037,
    R_DRW2D_ERR_DEVICE_INTERNAL_CMDLIST                        =  0x00060038,

    R_DRW2D_ERR_DEVICE_HWINSTANCENR                            =  0x00060100,
    R_DRW2D_ERR_DEVICE_PIXELFMT                                =  0x00060110,
    R_DRW2D_ERR_DEVICE_OUTOFVIDMEM                             =  0x00060120,
    R_DRW2D_ERR_DEVICE_NATIVEDRVHANDLE                         =  0x00060130,
    R_DRW2D_ERR_DEVICE_SAVESTATEALLOC                          =  0x00060140,
    R_DRW2D_ERR_DEVICE_SAVESTATE                               =  0x00060150,
    R_DRW2D_ERR_DEVICE_RESTORESTATE                            =  0x00060160,

    R_DRW2D_ERR_CONTEXT                                        =  0x00070000,
    R_DRW2D_ERR_CONTEXT_NOTINUSE                               =  0x00070010,

    R_DRW2D_ERR_FRAMEBUFFER                                    =  0x00080000,
    R_DRW2D_ERR_FRAMEBUFFER_HANDLE                             =  0x00080010,
    R_DRW2D_ERR_FRAMEBUFFER_ADDR                               =  0x00080020,

    R_DRW2D_ERR_BUFFER                                         =  0x00090000,
    R_DRW2D_ERR_BUFFER_PIXELFMT                                =  0x00090010,
    R_DRW2D_ERR_BUFFER_WIDTH                                   =  0x00090020,
    R_DRW2D_ERR_BUFFER_HEIGHT                                  =  0x00090030,
    R_DRW2D_ERR_BUFFER_ALLINUSE                                =  0x00090040,
    R_DRW2D_ERR_BUFFER_ALIGNMENT                               =  0x00090050,

    R_DRW2D_ERR_TEXTURE                                        =  0x000A0000,
    R_DRW2D_ERR_TEXTURE_ADDR                                   =  0x000A0020,
    R_DRW2D_ERR_TEXTURE_PIXELFMT                               =  0x000A0030,
    R_DRW2D_ERR_TEXTURE_RLE_BPP                                =  0x000A0040,
    R_DRW2D_ERR_TEXTURE_TRANSFORMMODE                          =  0x000A0060,
    R_DRW2D_ERR_TEXTURE_WIDTH                                  =  0x000A0070,
    R_DRW2D_ERR_TEXTURE_HEIGHT                                 =  0x000A0080,
    R_DRW2D_ERR_TEXTURE_UNIT                                   =  0x000A0090,

    R_DRW2D_ERR_DRAWING                                        =  0x000B0000,
    R_DRW2D_ERR_DRAWING_DRAWTRI                                =  0x000B0010,
    R_DRW2D_ERR_DRAWING_DRAWTRIUV                              =  0x000B0020,
    R_DRW2D_ERR_DRAWING_DRAWRECT                               =  0x000B0030,
    R_DRW2D_ERR_DRAWING_DRAWRECTUV                             =  0x000B0040,
    R_DRW2D_ERR_DRAWING_DRAWQUAD                               =  0x000B0050,
    R_DRW2D_ERR_DRAWING_TEXTUREBLIT                            =  0x000B0060,

    R_DRW2D_ERR_COMMANDLIST                                    =  0x000C0000,
    R_DRW2D_ERR_COMMANDLIST_RETHANDLE                          =  0x000C0010,
    R_DRW2D_ERR_COMMANDLIST_HANDLE                             =  0x000C0020,
    R_DRW2D_ERR_COMMANDLIST_CBKNULLPTR                         =  0x000C0030,

    R_DRW2D_ERR_PERF                                           =  0x000D0000,
    R_DRW2D_ERR_PERF_ALLOC                                     =  0x000D0010,
    R_DRW2D_ERR_PERF_FREE                                      =  0x000D0020,
    R_DRW2D_ERR_PERF_NOTAVAIL                                  =  0x000D0030,
    R_DRW2D_ERR_PERF_READ                                      =  0x000D0040,
    R_DRW2D_ERR_PERF_RESET                                     =  0x000D0050,

    R_DRW2D_ERR_EFFECT                                         =  0x000E0000,
    R_DRW2D_ERR_EFFECT_INVALID_OPERAND                         =  0x000E0010,
    R_DRW2D_ERR_EFFECT_OUT_OF_RESOURCES                        =  0x000E0020,
    R_DRW2D_ERR_EFFECT_INVALID_OPERATION                       =  0x000E0030,
    R_DRW2D_ERR_EFFECT_DIV_BY_ZERO                             =  0x000E0040,
    R_DRW2D_ERR_EFFECT_INVALID_TEXTURE                         =  0x000E0050,

    R_DRW2D_ERR_CONVOLUTION                                    =  0x000F0000,
    R_DRW2D_ERR_CONVOLUTION_ADDR                               =  0x000F0010,
    R_DRW2D_ERR_CONVOLUTION_DIMENSION                          =  0x000F0020,
    R_DRW2D_ERR_CONVOLUTION_RES_CONFLICT                       =  0x000F0030,
    R_DRW2D_ERR_CONVOLUTION_INVALID_PARAM                      =  0x000F0040,

    R_DRW2D_NUM_ERR_CODES
} r_drw2d_Error_t;


/***************************************************************************
  Group: Macros and Types
*/

/*******************************************************************************
  Macro: R_DRW2D_ERROR_CLASS

  Mask out error sub-code (lower 16bits of error code)

  Parameter:
    a - Error code

  Return value:
    Error class (lower 16bits of error code set to 0)

  See also:
   <r_drw2d_Error_t>
*/
#define R_DRW2D_ERROR_CLASS(a) ((a) & 0x7FFF0000)



/*******************************************************************************
  Type: r_drw2d_Device_t

  Device handle

  See also:
   <R_DRW2D_Init>
*/
typedef void *r_drw2d_Device_t;



/*******************************************************************************
  Type: r_drw2d_Context_t

  Opaque render context handle
*/
typedef void *r_drw2d_Context_t;



/*******************************************************************************
  Type: r_drw2d_FixedP_t

  A 1:15:16 fixed point number (signed 2s complement format. 
  MSB=sign bit, 15 integer bits, 16 fractional bits).
*/
typedef int32_t r_drw2d_FixedP_t;



/*******************************************************************************
  Macro: R_DRW2D_2X

  Convert integer or float to fixed point.

  Parameter:
    v - Integer or float value

  Return value:
    Fixed point value.

  See also:
   <r_drw2d_FixedP_t>, <R_DRW2D_2I>, <R_DRW2D_2F>, <R_DRW2D_2U>
*/
#define R_DRW2D_2X(v) ((r_drw2d_FixedP_t)((v) * 65536))



/*******************************************************************************
  Macro: R_DRW2D_2I

  Convert fixed point to int.

  Parameter:
    x - Fixed point value

  Return value:
    Integer value

  See also:
   <r_drw2d_FixedP_t>, <R_DRW2D_2X>, <R_DRW2D_2F>
*/
#define R_DRW2D_2I(x)  (((int32_t)(x)) / 65536)



/*******************************************************************************
  Macro: R_DRW2D_2F

  Convert fixed point to float.

  Parameter:
    x - Fixed point value

  Return value:
    Float value

  See also:
   <r_drw2d_FixedP_t>, <R_DRW2D_2I>, <R_DRW2D_2X>
*/
#define R_DRW2D_2F(x)  (((float64_t)(x)) * (1.0f / 65536.0f))



/*******************************************************************************
  Macro: R_DRW2D_2U

  Convert integer or float to fixed point normalized texture U coordinate.

  (note) Texture coordinates are prescaled by 256.

  Parameter:
    v - Integer or float value

  Return value:
    Fixed point value.

  See also:
   <r_drw2d_FixedP_t>, <R_DRW2D_2I>, <R_DRW2D_2F>
*/
#define R_DRW2D_2U(v) ((r_drw2d_FixedP_t)((v) * (256 * 65536)))



/*******************************************************************************
  Macro: R_DRW2D_2V

  Convert integer or float to fixed point normalized texture V coordinate.

  (note) Texture coordinates are prescaled by 256.

  Parameter:
    v - Integer or float value

  Return value:
    Fixed point value.

  See also:
   <r_drw2d_FixedP_t>, <R_DRW2D_2I>, <R_DRW2D_2F>
*/
#define R_DRW2D_2V(v) ((r_drw2d_FixedP_t)((v) * (256 * 65536)))



/*******************************************************************************
  Type: r_drw2d_Boolean_t

  A boolean (R_TRUE or R_FALSE, 1 or 0).
*/
typedef int32_t r_drw2d_Boolean_t;



/*******************************************************************************
  Type: r_drw2d_ErrorCallback_t

  Device-context specific error callback function type.

  This callback function is used when a valid Device handle is available.

  Therefore, a different callback function can be set for each thread that
  uses the DRW2D API.

  Return R_TRUE if the error was handled.

  See also:
    <r_drw2d_Error_t>, <R_DRW2D_ErrCallbackSet>
*/
typedef r_drw2d_Boolean_t (*r_drw2d_ErrorCallback_t) (r_drw2d_Device_t Device,
                                                      r_drw2d_Error_t  Error,
                                                      void *           UserData
                                                      );



/*******************************************************************************
  Type: r_drw2d_GlobalErrorCallback_t

  Global error callback function type.

  This callback function is used when no Device handle is available (e.g.
  when the application passed a NULL pointer handle).

  The callback is shared by all threads that use the DRW2D API.

  Return R_TRUE if the error was handled.   

  See also:
    <r_drw2d_Error_t>, <R_DRW2D_GlobalErrCallbackSet>
*/
typedef r_drw2d_Boolean_t (*r_drw2d_GlobalErrorCallback_t) (r_drw2d_Error_t  Error,
                                                            void *           UserData
                                                            );



/*******************************************************************************
  Type: r_drw2d_Color_t

  Packed 32bit ARGB color.

  See also:
   <R_DRW2D_CtxFgColor>, <R_DRW2D_CtxBgColor>
*/
typedef uint32_t r_drw2d_Color_t;



/*******************************************************************************
  Type: r_drw2d_GpuCmdList_t

  GPU command list handle/address.

  See also:
   <R_DRW2D_GpuCmdListCreate>, <R_DRW2D_GpuCmdListGenerate>, <R_DRW2D_GpuCmdListExec>, 
   <R_DRW2D_GpuCmdListCopy>
*/
typedef void *r_drw2d_GpuCmdList_t;



/*******************************************************************************
  Type: r_drw2d_GpuCmdListCallback_t

  GPU command list callback used by <R_DRW2D_GpuCmdListGenerate>.

  See also:
   <R_DRW2D_GpuCmdListCreate>, <R_DRW2D_GpuCmdListGenerate>, <R_DRW2D_GpuCmdListExec>, 
   <R_DRW2D_GpuCmdListCopy>
*/
typedef r_drw2d_Error_t (*r_drw2d_GpuCmdListCallback_t) (void *UserData);



/*******************************************************************************
  Type: r_drw2d_EdgeFlag_t

  Specifies which edge(s) of a triangle or quad will be antialiased.

  Values:
    R_DRW2D_EDGE_AB - Antialias edge between first and second vertex
    R_DRW2D_EDGE_BC - Antialias edge between second and third vertex
    R_DRW2D_EDGE_CA - Antialias edge between third and first triangle vertex (triangle only)
    R_DRW2D_EDGE_CD - Antialias edge between third and fourth vertex (quad only)
    R_DRW2D_EDGE_DA - Antialias edge between fourth and first vertex (quad only)
*/
typedef uint32_t r_drw2d_EdgeFlag_t;
#define R_DRW2D_EDGE_AB  ((r_drw2d_EdgeFlag_t)((uint8_t)(((uint8_t)1u) << ((uint8_t)0))))
#define R_DRW2D_EDGE_BC  ((r_drw2d_EdgeFlag_t)((uint8_t)(((uint8_t)1u) << ((uint8_t)1))))
#define R_DRW2D_EDGE_CA  ((r_drw2d_EdgeFlag_t)((uint8_t)(((uint8_t)1u) << ((uint8_t)2))))
#define R_DRW2D_EDGE_CD  ((r_drw2d_EdgeFlag_t)((uint8_t)(((uint8_t)1u) << ((uint8_t)3))))
#define R_DRW2D_EDGE_DA  ((r_drw2d_EdgeFlag_t)((uint8_t)(((uint8_t)1u) << ((uint8_t)4))))





/***************************************************************************
  Group: Enumerations
*/

/*******************************************************************************
  Enum: r_drw2d_PixelFormat_t

  Describes the color model and pixel storage format of an <r_drw2d_Buffer_t>.


  Values:
    R_DRW2D_PIXELFORMAT_NONE     - Unspecified format (e.g. unallocated buffers)
    R_DRW2D_PIXELFORMAT_ALPHA8   - 8 alpha bits per pixel (only available as framebuffer format:
                                   input is read to all 4 channels, alpha channel is written out)
    R_DRW2D_PIXELFORMAT_LUM8     - 8 luminance bits per pixel 
                                   (only available as framebuffer format: alpha channel set to 1.0 
                                   on read, blue channel is written out)
    R_DRW2D_PIXELFORMAT_AL17     - 1 alpha bit, 7 luminance bits per pixel
    R_DRW2D_PIXELFORMAT_AL44     - 4 alpha bits, 4 luminance bits per pixel
    R_DRW2D_PIXELFORMAT_AL88     - 8 alpha bits, 8 luminance bits per pixel
    R_DRW2D_PIXELFORMAT_AL1      - 1 alpha/luminance bit per pixel 
                                   (only available as texture format)
    R_DRW2D_PIXELFORMAT_AL2      - 2 alpha/luminance bits per pixel 
                                   (only available as texture format)
    R_DRW2D_PIXELFORMAT_AL4      - 4 alpha/luminance bits per pixel 
                                   (only available as texture format)
    R_DRW2D_PIXELFORMAT_AL8      - 8 alpha/luminance bits per pixel 
                                   (only available as texture format)
    R_DRW2D_PIXELFORMAT_RGB565   - 5 green, 6 red, 5 blue bits per pixel, packed into a 16bit word
    R_DRW2D_PIXELFORMAT_ARGB1555 - 1 alpha, 5 green, 5 red, 5 blue bits per pixel, 
                                   packed into a 16bit word (alpha=MSB)
    R_DRW2D_PIXELFORMAT_RGBA5551 - 5 green, 5 red, 5 blue, 1 alpha bits per pixel, 
                                   packed into a 16bit word (alpha=LSB)
    R_DRW2D_PIXELFORMAT_ARGB4444 - 4 alpha, 4 green, 4 red, 4 blue bits per pixel, 
                                   packed into a 16bit word (alpha=MSB)
    R_DRW2D_PIXELFORMAT_RGBA4444 - 4 green, 4 red, 4 blue, 4 alpha bits per pixel, 
                                   packed into a 16bit word (alpha=LSB)
    R_DRW2D_PIXELFORMAT_ARGB8888 - 8 alpha, 8 red, 8 green, 8 blue bits per pixel, 
                                   packed into a 32bit word (alpha=MSB)
    R_DRW2D_PIXELFORMAT_RGBA8888 - 8 red, 8 green, 8 blue, 8 alpha bits per pixel, 
                                   packed into a 32bit word (alpha=LSB)
    R_DRW2D_PIXELFORMAT_CLUT_8   - 8 bpp color lookup format (only available as texture format)
    R_DRW2D_PIXELFORMAT_CLUT_4   - 4 bpp color lookup format (only available as texture format)
    R_DRW2D_PIXELFORMAT_CLUT_2   - 2 bpp color lookup format (only available as texture format)
    R_DRW2D_PIXELFORMAT_CLUT_1   - 1 bpp color lookup format (only available as texture format)

  See also:
    <r_drw2d_Buffer_t>, <r_drw2d_Texture_t>, <R_DRW2D_FramebufferSet>, <R_DRW2D_CtxTextureSet>

*/
typedef enum
{
    R_DRW2D_PIXELFORMAT_NONE     =  0,
    R_DRW2D_PIXELFORMAT_ALPHA8   =  1,
    R_DRW2D_PIXELFORMAT_LUM8     =  2,
    R_DRW2D_PIXELFORMAT_AL17     =  3,
    R_DRW2D_PIXELFORMAT_AL44     =  4,
    R_DRW2D_PIXELFORMAT_AL88     =  5,
    R_DRW2D_PIXELFORMAT_AL1      =  6,
    R_DRW2D_PIXELFORMAT_AL2      =  7,
    R_DRW2D_PIXELFORMAT_AL4      =  8,
    R_DRW2D_PIXELFORMAT_AL8      =  9,
    R_DRW2D_PIXELFORMAT_RGB565   = 10,
    R_DRW2D_PIXELFORMAT_ARGB1555 = 11,
    R_DRW2D_PIXELFORMAT_RGBA5551 = 12,
    R_DRW2D_PIXELFORMAT_ARGB4444 = 13,
    R_DRW2D_PIXELFORMAT_RGBA4444 = 14,
    R_DRW2D_PIXELFORMAT_ARGB8888 = 15,
    R_DRW2D_PIXELFORMAT_RGBA8888 = 16,
    R_DRW2D_PIXELFORMAT_CLUT_8   = 17,
    R_DRW2D_PIXELFORMAT_CLUT_4   = 18,
    R_DRW2D_PIXELFORMAT_CLUT_2   = 19,
    R_DRW2D_PIXELFORMAT_CLUT_1   = 20,

    R_DRW2D_NUM_PIXELFORMATS
} r_drw2d_PixelFormat_t;



/*******************************************************************************
  Enum: r_drw2d_FramebufferFlags_t

  Flags used for <R_DRW2D_FramebufferSet> call.

  *reserved for future extensions*

  See also:
    <R_DRW2D_FramebufferSet>
*/
typedef enum
{
    __R_DRW2D_FRAMEBUFFERFLAGS_PLACEHOLDER__        /* PRQA S 602 *//* $Misra: This identifier is unique in Drw2D. $*/
} r_drw2d_FramebufferFlags_t;



/*******************************************************************************
  Enum: r_drw2d_TextureFlags_t

  Flags used for <R_DRW2D_CtxTextureSet> call.

  The upper 8 bits are reserved for internal purposes (texture lock flags).



  Values:
           R_DRW2D_TEX_NONE - No-op texture flag
          R_DRW2D_TEX_WRAPU - Wrap bitmap on U axis (x-direction)
          R_DRW2D_TEX_WRAPV - Wrap bitmap on V axis (y-direction)
       R_DRW2D_TEX_BILINEAR - Enable bilinear filtering
    R_DRW2D_TEX_PERSPECTIVE - Enable perspective texture mapping
            R_DRW2D_TEX_RLE - Enable run length encoding (RLE) texture compression.
        R_DRW2D_TEX_SWIZZLE - Enable texture swizzling. 
                              This flag cannot be combined with <R_DRW2D_TEX_VT>.
                              The textures' pitch & height have to be aligned to the swizzle mode 
                              value (by default it is 4x4 which means the pitch & height need to 
                              be aligned to 4).
             R_DRW2D_TEX_VT - Enable virtual tiling. 
                              This flag cannot be combined with <R_DRW2D_TEX_SWIZZLE>.
                              Virtual tiling settings is defined in r_config_drw2d.h 
                              (R_DRW2D_DHD_VT_BPP_*).
                              This feature is requires the texture pitch to be a multiple of the 
                              virtual tile width.

  See also:
    <R_DRW2D_CtxTextureSet>
*/
typedef uint32_t r_drw2d_TextureFlags_t;
#define R_DRW2D_TEX_NONE         ((r_drw2d_TextureFlags_t)(0u))
#define R_DRW2D_TEX_WRAPU        ((r_drw2d_TextureFlags_t)((uint8_t)(((uint8_t)1u) << ((uint8_t)0))))
#define R_DRW2D_TEX_WRAPV        ((r_drw2d_TextureFlags_t)((uint8_t)(((uint8_t)1u) << ((uint8_t)1))))
#define R_DRW2D_TEX_BILINEAR     ((r_drw2d_TextureFlags_t)((uint8_t)(((uint8_t)1u) << ((uint8_t)2))))
#define R_DRW2D_TEX_PERSPECTIVE  ((r_drw2d_TextureFlags_t)((uint8_t)(((uint8_t)1u) << ((uint8_t)3))))
#define R_DRW2D_TEX_RLE          ((r_drw2d_TextureFlags_t)((uint8_t)(((uint8_t)1u) << ((uint8_t)4))))
#define R_DRW2D_TEX_SWIZZLE      ((r_drw2d_TextureFlags_t)((uint8_t)(((uint8_t)1u) << ((uint8_t)5))))
#define R_DRW2D_TEX_VT           ((r_drw2d_TextureFlags_t)((uint8_t)(((uint8_t)1u) << ((uint8_t)6))))



/*******************************************************************************
  Enum: r_drw2d_BlendMode_t

  Enumeration of preset blending equations.


  Values:
     R_DRW2D_BLENDMODE_CUSTOM - User defined blendmode specified by <R_DRW2D_CtxBlendFactors>.
       > dst_color = src_color*src_factor_color + dst_color*dst_factor_color
       > dst_alpha = src_alpha*src_factor_alpha + dst_alpha*dst_factor_alpha

     R_DRW2D_BLENDMODE_SRC - Porter-Duff Src
       > dst_color = src_color
       > dst_alpha = src_alpha

     R_DRW2D_BLENDMODE_SRC_OVER - Porter-Duff Src-over-Dst
       > dst_color = src_color + dst_color*(1-src_alpha)
       > dst_alpha = src_alpha + dst_alpha*(1-src_alpha)

     R_DRW2D_BLENDMODE_DST_OVER - Porter-Duff Dst-over-Src
       > dst_color = src_color*(1-dst_alpha) + dst_color
       > dst_alpha = src_alpha*(1-dst_alpha) + dst_alpha

     R_DRW2D_BLENDMODE_SRC_IN - Porter-Duff Src-in-Dst
       > dst_color = src_color*dst_alpha
       > dst_alpha = src_alpha*dst_alpha

     R_DRW2D_BLENDMODE_DST_IN - Porter-Duff Dst-in-Src
       > dst_color = dst_color*src_alpha
       > dst_alpha = dst_alpha*src_alpha

     R_DRW2D_BLENDMODE_MULTIPLY - Multiply the source and destination colors together, 
                                  producing the effect of placing a transparent filter 
                                  over a background.
       > dst_color = (src_alpha*src_color)*(1-dst_alpha) + (dst_alpha*dst_color)*(1-src_alpha)
       >              + (src_alpha*src_color*dst_alpha*dst_color)
       > dst_alpha = src_alpha + dst_alpha*(1-src_alpha)

     R_DRW2D_BLENDMODE_SCREEN - The opposite of multiplication, producing the effect of 
                                projecting a slide over a background.
       > dst_color = (src_alpha*src_color) + (dst_alpha*dst_color)
       >              - (src_alpha*src_color*dst_alpha*dst_color)
       > dst_alpha = src_alpha + dst_alpha*(1-src_alpha)

     R_DRW2D_BLENDMODE_DARKEN - Compute (Src over Dst) and (Dst over Src) and take the smaller 
                                (darker) value for each channel
       > dst_color = min(src_alpha*src_color + dst_alpha*dst_color *(1-src_alpha),
       >                 dst_alpha*dst_color + src_alpha*src_color *(1-dst_alpha))
       > dst_alpha = src_alpha + dst_alpha*(1-src_alpha)

     R_DRW2D_BLENDMODE_LIGHTEN - Compute (Src over Dst) and (Dst over Src) and take the larger 
                                 (lighter) value for each channel
       > dst_color = max(src_alpha*src_color + dst_alpha*dst_color *(1-src_alpha),
       >                 dst_alpha*dst_color + src_alpha*src_color *(1-dst_alpha))
       > dst_alpha = src_alpha + dst_alpha*(1-src_alpha)

     R_DRW2D_BLENDMODE_ADDITIVE - Add alpha and color channels
       > dst_color = (src_alpha*src_color + dst_alpha*dst_color) / min(src_alpha + dst_alpha, 1)
       > dst_alpha = min(src_alpha+dst_alpha, 1)

  See also:
    <R_DRW2D_CtxBlendMode>, <R_DRW2D_CtxBlendFactors>, <r_drw2d_BlendFactor_t>
*/
typedef enum
{
    R_DRW2D_BLENDMODE_CUSTOM    =  0,
    R_DRW2D_BLENDMODE_SRC       =  1,
    R_DRW2D_BLENDMODE_SRC_OVER  =  2,
    R_DRW2D_BLENDMODE_DST_OVER  =  3,
    R_DRW2D_BLENDMODE_SRC_IN    =  4,
    R_DRW2D_BLENDMODE_DST_IN    =  5,
    R_DRW2D_BLENDMODE_MULTIPLY  =  6,
    R_DRW2D_BLENDMODE_SCREEN    =  7,
    R_DRW2D_BLENDMODE_DARKEN    =  8,
    R_DRW2D_BLENDMODE_LIGHTEN   =  9,
    R_DRW2D_BLENDMODE_ADDITIVE  = 10,

    R_DRW2D_NUM_BLENDMODES
} r_drw2d_BlendMode_t;



/*******************************************************************************
  Enum: r_drw2d_BlendFactor_t

  Source/destination blend factors used in <R_DRW2D_CtxBlendFactors>.


  When <R_DRW2D_BLENDMODE_CUSTOM> is selected, the effective color/alpha blend equation is
    > dst_color = src_color*src_factor_color + dst_color*dst_factor_color
    > dst_alpha = src_alpha*src_factor_alpha + dst_alpha*dst_factor_alpha


  Values:
    R_DRW2D_BLEND_ZERO                    - Multiply by 0 (discard)
    R_DRW2D_BLEND_ONE                     - Multiply by 1
    R_DRW2D_BLEND_SRC_COLOR               - Multiply by source color
    R_DRW2D_BLEND_ONE_MINUS_SRC_COLOR     - Multiply by inverse source color
    R_DRW2D_BLEND_DST_COLOR               - Multiply by destination color
    R_DRW2D_BLEND_ONE_MINUS_DST_COLOR     - Multiply by inverse destination color
    R_DRW2D_BLEND_SRC_ALPHA               - Multiply by source alpha
    R_DRW2D_BLEND_ONE_MINUS_SRC_ALPHA     - Multiply by inverse source alpha
    R_DRW2D_BLEND_DST_ALPHA               - Multiply by destination alpha
    R_DRW2D_BLEND_ONE_MINUS_DST_ALPHA     - Multiply by inverse destination alpha
    R_DRW2D_BLEND_CONSTANT_FGCOLOR        - (not supported) 
    R_DRW2D_BLEND_ONE_MINUS_CONST_FGCOLOR - (not supported) 
    R_DRW2D_BLEND_CONSTANT_FGALPHA        - (not supported) 
    R_DRW2D_BLEND_ONE_MINUS_CONST_FGALPHA - (not supported) 
    R_DRW2D_BLEND_CONSTANT_BGCOLOR        - (not supported) 
    R_DRW2D_BLEND_ONE_MINUS_CONST_BGCOLOR - (not supported) 
    R_DRW2D_BLEND_CONSTANT_BGALPHA        - (not supported) 
    R_DRW2D_BLEND_ONE_MINUS_CONST_FGALPHA - (not supported) 
    R_DRW2D_BLEND_SRC_ALPHA_SATURATE      - (not supported) 
    R_DRW2D_BLEND_SRC1_COLOR              - (not supported)
    R_DRW2D_BLEND_ONE_MINUS_SRC1_COLOR    - (not supported)
    R_DRW2D_BLEND_SRC1_ALPHA              - (not supported)
    R_DRW2D_BLEND_ONE_MINUS_SRC1_ALPHA    - (not supported)
    R_DRW2D_BLEND_SRC1_DST1               - (not supported)

  See also:
    <R_DRW2D_CtxBlendFactors>, <R_DRW2D_CtxBlendMode>, <r_drw2d_BlendMode_t>

*/
typedef enum
{
    R_DRW2D_BLEND_ZERO                    =  0,
    R_DRW2D_BLEND_ONE                     =  1,
    R_DRW2D_BLEND_SRC_COLOR               =  2,
    R_DRW2D_BLEND_ONE_MINUS_SRC_COLOR     =  3,
    R_DRW2D_BLEND_DST_COLOR               =  4,
    R_DRW2D_BLEND_ONE_MINUS_DST_COLOR     =  5,
    R_DRW2D_BLEND_SRC_ALPHA               =  6,
    R_DRW2D_BLEND_ONE_MINUS_SRC_ALPHA     =  7,
    R_DRW2D_BLEND_DST_ALPHA               =  8,
    R_DRW2D_BLEND_ONE_MINUS_DST_ALPHA     =  9,
    R_DRW2D_BLEND_CONSTANT_FGCOLOR        = 10,
    R_DRW2D_BLEND_ONE_MINUS_CONST_FGCOLOR = 11,
    R_DRW2D_BLEND_CONSTANT_FGALPHA        = 12,
    R_DRW2D_BLEND_ONE_MINUS_CONST_FGALPHA = 13,
    R_DRW2D_BLEND_CONSTANT_BGCOLOR        = 14,
    R_DRW2D_BLEND_ONE_MINUS_CONST_BGCOLOR = 15,
    R_DRW2D_BLEND_CONSTANT_BGALPHA        = 16,
    R_DRW2D_BLEND_ONE_MINUS_CONST_BGALPHA = 17,
    R_DRW2D_BLEND_SRC_ALPHA_SATURATE      = 18,
    R_DRW2D_BLEND_SRC1_COLOR              = 19,
    R_DRW2D_BLEND_ONE_MINUS_SRC1_COLOR    = 20,
    R_DRW2D_BLEND_SRC1_ALPHA              = 21,
    R_DRW2D_BLEND_ONE_MINUS_SRC1_ALPHA    = 22,
    R_DRW2D_BLEND_SRC1_DST1               = 23
} r_drw2d_BlendFactor_t;



/*******************************************************************************
  Enum: r_drw2d_FillMode_t

  Enumeration of primitive fill modes.

  The fill mode influences the drawing mode of all render functions.

  The initial fill mode is <R_DRW2D_FILLMODE_SOLID>.


  Values:
    R_DRW2D_FILLMODE_SOLID   - Fill primitive with foreground color
    R_DRW2D_FILLMODE_TEXTURE - Fill primitive with current texture

  See also:
    <R_DRW2D_CtxFillMode>
*/
typedef enum
{
    R_DRW2D_FILLMODE_SOLID    = 1,
    R_DRW2D_FILLMODE_TEXTURE  = 2,

    R_DRW2D_NUM_FILLMODES
} r_drw2d_FillMode_t;



/*******************************************************************************
  Enum: r_drw2d_CullMode_t

  Enumeration of winding order culling modes.

  The initial cull mode is <R_DRW2D_CULLMODE_NONE>.


  Value:
    R_DRW2D_CULLMODE_NONE - Never cull primitives [default]
    R_DRW2D_CULLMODE_CCW  - Cull primitives that have a counter clock wise winding order
    R_DRW2D_CULLMODE_CW   - Cull primitives that have a clock wise winding order
*/
typedef enum
{
    R_DRW2D_CULLMODE_NONE = 0,
    R_DRW2D_CULLMODE_CCW  = 1,
    R_DRW2D_CULLMODE_CW   = 2,

    R_DRW2D_NUM_CULLMODES
} r_drw2d_CullMode_t;



/*******************************************************************************
  Enum: r_drw2d_LineCap_t

  Enumeration of line start/end drawing styles.


  Values:
    R_DRW2D_LINECAP_ROUND  - Draw round line endings.
    R_DRW2D_LINECAP_SQUARE - Draw flat line ending, offset by half the line width.
    R_DRW2D_LINECAP_BUTT   - Draw flat line ending.

  See also:
    <R_DRW2D_CtxLineStyle>, <r_drw2d_LineJoin_t>, <r_drw2d_LineStyle_t>, <R_DRW2D_DrawPolyline>, 
    <R_DRW2D_DrawLines>
*/

typedef enum
{
    R_DRW2D_LINECAP_ROUND   = 0,
    R_DRW2D_LINECAP_SQUARE  = 1,
    R_DRW2D_LINECAP_BUTT    = 2
} r_drw2d_LineCap_t;



/*******************************************************************************
  Enum: r_drw2d_LineJoin_t

  Enumeration of line connection drawing styles for multi-segment lines (see <R_DRW2D_DrawPolyline>).


  Values:
    R_DRW2D_LINEJOIN_NONE  - Do not connect lines (gaps may appear at sharp angles)
    R_DRW2D_LINEJOIN_MITER - Connect lines using sharp edges
    R_DRW2D_LINEJOIN_ROUND - Connect lines using round edges
    R_DRW2D_LINEJOIN_BEVEL - Connect lines using flat edges

  See also:
    <R_DRW2D_CtxLineStyle>,  <r_drw2d_LineCap_t>, <r_drw2d_LineStyle_t>, <R_DRW2D_DrawPolyline>
*/
typedef enum
{
    R_DRW2D_LINEJOIN_NONE  = 0,
    R_DRW2D_LINEJOIN_MITER = 1,
    R_DRW2D_LINEJOIN_ROUND = 2,
    R_DRW2D_LINEJOIN_BEVEL = 3,

    R_DRW2D_NUM_LINEJOIN_TYPES
} r_drw2d_LineJoin_t;



/*******************************************************************************
  Enum: r_drw2d_ImgQuality_t

  Enumeration of anti-aliasing / quality modes.


  Values:
    R_DRW2D_IMGQUALITY_LOW    - No antialiasing 
    R_DRW2D_IMGQUALITY_MEDIUM - Medium quality antialiasing
    R_DRW2D_IMGQUALITY_HIGH   - High quality antialiasing (default)

  See also:
    <R_DRW2D_CtxImgQuality>
*/
typedef enum
{
    R_DRW2D_IMGQUALITY_LOW     = 0,
    R_DRW2D_IMGQUALITY_MEDIUM  = 1,
    R_DRW2D_IMGQUALITY_HIGH    = 2
} r_drw2d_ImgQuality_t;



/*******************************************************************************
  Enum: r_drw2d_TransformMode_t

  Vertex matrix transform mode.

  Values:
    R_DRW2D_TRANSFORM_NONE - No vertex transformation
    R_DRW2D_TRANSFORM_2D   - 2D vertex transformation (default)
    R_DRW2D_TRANSFORM_3D   - 3D vertex transformation and viewport projection

  See also:
    <R_DRW2D_CtxTransformMode>
*/
typedef enum
{
    R_DRW2D_TRANSFORM_NONE = 0,
    R_DRW2D_TRANSFORM_2D   = 1,
    R_DRW2D_TRANSFORM_3D   = 2
} r_drw2d_TransformMode_t;



/*******************************************************************************
  Enum: r_drw2d_TextureTransformMode_t

  Texture matrix transform mode.

  Values:
      R_DRW2D_TEX_TRANSFORM_NONE - No texture coordinate transformation
        R_DRW2D_TEX_TRANSFORM_2D - 2D texture transformation (default)

  See also:
    <R_DRW2D_CtxTextureTransformMode>
*/
typedef enum
{
    R_DRW2D_TEX_TRANSFORM_NONE    = 0,
    R_DRW2D_TEX_TRANSFORM_2D      = 2,
} r_drw2d_TextureTransformMode_t;



/*******************************************************************************
  Enum: r_drw2d_Performance_t

  Used in <R_DRW2D_PerfValueGet> and <R_DRW2D_PerfValueReset> to query/reset performance values.


  Values:
    R_DRW2D_PERF_GPU_TIME  - Total GPU time spent
    R_DRW2D_PERF_HW_READS  - Total GPU time spent for texture/framebuffer reads
    R_DRW2D_PERF_HW_WRITES - Total GPU time spent for texture/framebuffer writes
*/
typedef enum
{
    R_DRW2D_PERF_GPU_TIME   = 0,
    R_DRW2D_PERF_HW_READS   = 1,
    R_DRW2D_PERF_HW_WRITES  = 2
} r_drw2d_Performance_t;



/*******************************************************************************
  Enum: r_drw2d_Finish_t

  Whether <R_DRW2D_GpuFinish> will block or not can be specified with this option.


  Values:
    R_DRW2D_FINISH_NOWAIT      - Send current command list to GPU and do not wait for completion.
    R_DRW2D_FINISH_WAIT        - Send current command list to GPU and wait for completion.
    R_DRW2D_FINISH_NOWAIT_MARK - Send current command list to GPU non-waiting and mark this list
                                 to be queried with <R_DRW2D_GpuFinished>

*/
typedef enum
{
    R_DRW2D_FINISH_NOWAIT      = 0,
    R_DRW2D_FINISH_WAIT        = 1,
    R_DRW2D_FINISH_NOWAIT_MARK = 2
} r_drw2d_Finish_t;



/***********************************************************
  Enum: r_drw2d_ConvolveMode_t

  Convolution filter mode (1d horizontal, 1d vertical, 2d)

  Values:
    R_DRW2D_SYS_CONVOLVEMODE_NONE - No convolution filter mode
    R_DRW2D_SYS_CONVOLVEMODE_1DX  - 1d horizontal convolution filter
    R_DRW2D_SYS_CONVOLVEMODE_1DY  - 1d vertical convolution filter
    R_DRW2D_SYS_CONVOLVEMODE_2D   - 2d convolution filter
    R_DRW2D_SYS_CONVOLVEMODE_USER - user defined convolution filter
*/
typedef enum
{
    R_DRW2D_SYS_CONVOLVEMODE_NONE = 0,
    R_DRW2D_SYS_CONVOLVEMODE_1DX  = 1,
    R_DRW2D_SYS_CONVOLVEMODE_1DY  = 2,
    R_DRW2D_SYS_CONVOLVEMODE_2D   = 3,
    R_DRW2D_SYS_CONVOLVEMODE_USER = 4
} r_drw2d_ConvolveMode_t;



/*******************************************************************************
  Enum: r_drw2d_ConvolutionKernelPreset1d_t

  1D convolution filter kernel presets. 

  Values:
    R_DRW2D_CONV1D_GAUSSIAN_BLUR_3 - 3 coefficient gaussian blur
    R_DRW2D_CONV1D_GAUSSIAN_BLUR_5 - 5 coefficient gaussian blur
    R_DRW2D_CONV1D_GAUSSIAN_BLUR_7 - 7 coefficient gaussian blur
    R_DRW2D_CONV1D_SOBEL_DIFF      - 3x1 Sobel differentiation (edge detection)
    R_DRW2D_CONV1D_SOBEL_AVG       - 3x1 Sobel averaging (edge detection)

  See also:
    <R_DRW2D_CtxConvolutionKernelPreset1d>, <R_DRW2D_DrawRectConvolve1dx>,
    <R_DRW2D_DrawRectConvolve1dy>
*/
typedef enum
{
    R_DRW2D_CONV1D_GAUSSIAN_BLUR_3 = 1,
    R_DRW2D_CONV1D_GAUSSIAN_BLUR_5 = 2,
    R_DRW2D_CONV1D_GAUSSIAN_BLUR_7 = 3,
    R_DRW2D_CONV1D_SOBEL_DIFF      = 4,
    R_DRW2D_CONV1D_SOBEL_AVG       = 5,

    R_DRW2D_CONV1D_NUM_PRESETS     = 6

} r_drw2d_ConvolutionKernelPreset1d_t;



/*******************************************************************************
  Enum: r_drw2d_ConvolutionKernelPreset2d_t

  2D convolution filter kernel presets. 

  Values:
    R_DRW2D_CONV2D_GAUSSIAN_BLUR_3x3 - 3x3 gaussian blur
    R_DRW2D_CONV2D_GAUSSIAN_BLUR_5x5 - 5x5 gaussian blur
    R_DRW2D_CONV2D_GAUSSIAN_BLUR_7x7 - 7x7 gaussian blur
    R_DRW2D_CONV2D_SOBEL_H_3x3       - 3x3 Sobel horizontal (edge detection)
    R_DRW2D_CONV2D_SOBEL_V_3x3       - 3x3 Sobel vertical (edge detection)
    R_DRW2D_CONV2D_SHARPEN_3x3       - 3x3 sharpen
    R_DRW2D_CONV2D_EMBOSS_3x3        - 3x3 emboss

  See also:
    <R_DRW2D_CtxConvolutionKernelPreset2d>, <R_DRW2D_DrawRectConvolve2d>.
*/
typedef enum
{
    R_DRW2D_CONV2D_GAUSSIAN_BLUR_3x3 = 1,
    R_DRW2D_CONV2D_GAUSSIAN_BLUR_5x5 = 2,
    R_DRW2D_CONV2D_GAUSSIAN_BLUR_7x7 = 3,
    R_DRW2D_CONV2D_SOBEL_H_3x3       = 4,
    R_DRW2D_CONV2D_SOBEL_V_3x3       = 5,
    R_DRW2D_CONV2D_SHARPEN_3x3       = 6,
    R_DRW2D_CONV2D_EMBOSS_3x3        = 7,

    R_DRW2D_CONV2D_NUM_PRESETS       = 8

} r_drw2d_ConvolutionKernelPreset2d_t;



/*******************************************************************************
  Enum: r_drw2d_NativeDrvFlags_t

  Flags that can be passed to <R_DRW2D_NativeDriverBegin>.

  Values:
    R_DRW2D_NATIVEDRV_APPCONTEXT - Application uses a different device context to access the low 
                                   level driver. No DRW2D state flush or backup will be done. 
                                   The device context has to be created/destroyed by the 
                                   application by calling the respective low level driver 
                                   functions.
    R_DRW2D_NATIVEDRV_SAVESTATE  - Application uses the DRW2D device context to access the low 
                                   level driver. All pending DRW2D state updates will be sent to 
                                   the low level driver and the current driver state is backed up. 
                                   It will automatically be restored when 
                                   <R_DRW2D_NativeDriverEnd> is called.
    R_DRW2D_NATIVEDRV_KEEPSTATE  - Application uses the DRW2D device context to access the low 
                                   level driver. All pending DRW2D state updates will be sent to 
                                   the low level driver. The current driver state will not be 
                                   backed up so that state updates done using low level driver 
                                   access will potentially have an effect on subsequent DRW2D 
                                   render calls.
*/
typedef enum
{
    R_DRW2D_NATIVEDRV_APPCONTEXT  = 0,
    R_DRW2D_NATIVEDRV_SAVESTATE   = 1,
    R_DRW2D_NATIVEDRV_KEEPSTATE   = 2
} r_drw2d_NativeDrvFlags_t;


/*******************************************************************************
  Enum: r_drw2d_EffectName_t

  Available effects for the effect stage. Also see <R_DRW2D_CtxEffectsSet>.

  Values:
    R_DRW2D_EFFECT_REPLACE          - Replace by color value. (single 'Color' argument required)
    R_DRW2D_EFFECT_MODULATE         - Product of two color values (two 'Color' arguments required)
    R_DRW2D_EFFECT_ADD              - Sum of two color values (two 'Color' arguments required)
    R_DRW2D_EFFECT_ADD_SIGNED       - Sum of two color values minus 0.5 (two 'Color' arguments 
                                      required)
    R_DRW2D_EFFECT_SUBTRACT         - Difference of two color values (two 'Color' arguments 
                                      required)
    R_DRW2D_EFFECT_INTERPOLATE      - Interpolation of two color values by a third color value 
                                      (three 'Color' arguments required)
    R_DRW2D_EFFECT_DOT3             - Dot product of two color values. Result is a scalar present 
                                      in all color channels (two 'Color' arguments required)
    R_DRW2D_EFFECT_CONSTANT_ALPHA   - Blending by constant alpha (single 'Constant' argument 
                                      required) requires <R_DRW2D_IMGQUALITY_MEDIUM> or 
                                      <R_DRW2D_IMGQUALITY_HIGH>.
    R_DRW2D_EFFECT_GRADIENT         - Blending by alpha gradient (two 'Point' and two 'Constant' 
                                      arguments required) requires <R_DRW2D_IMGQUALITY_MEDIUM> or 
                                      <R_DRW2D_IMGQUALITY_HIGH>
*/
typedef enum
{
    R_DRW2D_EFFECT_REPLACE,
    R_DRW2D_EFFECT_MODULATE,
    R_DRW2D_EFFECT_ADD,
    R_DRW2D_EFFECT_ADD_SIGNED,
    R_DRW2D_EFFECT_SUBTRACT,
    R_DRW2D_EFFECT_INTERPOLATE,
    R_DRW2D_EFFECT_DOT3,
    R_DRW2D_EFFECT_CONSTANT_ALPHA,
    R_DRW2D_EFFECT_GRADIENT
} r_drw2d_EffectName_t;



/*******************************************************************************
  Enum: r_drw2d_EffectParamSource_t

  Specifies where the type of the parameters are coming from.

  Values:
    R_DRW2D_EFFECT_SOURCE_TEXTURE_UNIT      - Texture unit. Set texture unit index via 
                                              Param.Color.Source.TextureUnit
    R_DRW2D_EFFECT_SOURCE_CONSTANT_COLOR    - 32 bit constant color value. Set color value via
                                              Param.Color.Source.ConstantColor
    R_DRW2D_EFFECT_SOURCE_CONSTANT          - 16.16 signed fixpoint value (e.g. for constant alpha 
                                              effect). Set value via Param.Constant
    R_DRW2D_EFFECT_SOURCE_POINT             - 2D position (e.g. for gradient effect). Set value 
                                              via Param.Point
    R_DRW2D_EFFECT_SOURCE_PREV_STAGE        - The result of previous effects stage. Nothing in 
                                              'Param' is to be set.
*/
typedef enum
{
    R_DRW2D_EFFECT_SOURCE_TEXTURE_UNIT,
    R_DRW2D_EFFECT_SOURCE_CONSTANT_COLOR,
    R_DRW2D_EFFECT_SOURCE_CONSTANT,
    R_DRW2D_EFFECT_SOURCE_POINT,
    R_DRW2D_EFFECT_SOURCE_PREV_STAGE
} r_drw2d_EffectParamSource_t;



/*******************************************************************************
  Enum: r_drw2d_EffectColorParamOperand_t

  Specifies how the color source parameters shall be accessed.

  Values:
    R_DRW2D_EFFECT_COLOR_OPERAND_RGBA               - uses color channels directly
    R_DRW2D_EFFECT_COLOR_OPERAND_ONE_MINUS_RGBA     - uses inverted color channels
    R_DRW2D_EFFECT_COLOR_OPERAND_ALPHA              - uses alpha value for all color channels
    R_DRW2D_EFFECT_COLOR_OPERAND_ONE_MINUS_ALPHA    - uses inverted alpha value for all color 
                                                      channels
    R_DRW2D_EFFECT_COLOR_OPERAND_111A               - uses Alpha value. Color channels is set to 1
*/
typedef enum
{
    R_DRW2D_EFFECT_COLOR_OPERAND_RGBA,           
    R_DRW2D_EFFECT_COLOR_OPERAND_ONE_MINUS_RGBA, 
    R_DRW2D_EFFECT_COLOR_OPERAND_ALPHA,          
    R_DRW2D_EFFECT_COLOR_OPERAND_ONE_MINUS_ALPHA,
    R_DRW2D_EFFECT_COLOR_OPERAND_111A            
} r_drw2d_EffectColorParamOperand_t;



/*******************************************************************************
  Enum: r_drw2d_ConvKernelColorChannel_t

  Specifies the channels which are being processed by a convolution kernel.

  Values:
    R_DRW2D_CONVKERNEL_COLOR_CHANNEL_RGBA    - all color and alpha channels are being processed 
                                               by the kernel
    R_DRW2D_CONVKERNEL_COLOR_CHANNEL_RGB     - only RGB channels are being processed by the kernel
*/
typedef enum
{
    R_DRW2D_CONVKERNEL_COLOR_CHANNEL_RGBA, 
    R_DRW2D_CONVKERNEL_COLOR_CHANNEL_RGB   
} r_drw2d_ConvKernelColorChannel_t;



/*******************************************************************************
  Enum: r_drw2d_ConvMode_t

  Specifies how the texture is convoluted.

  Values:
    R_DRW2D_CONVMODE_TRIMMED    - Convolution is trimmed to the texture's size.
    R_DRW2D_CONVMODE_BLEEDING   - Convolution is also applied to pixels outside of the texture,
                                  which leads to a "bleeding" effect when using a blurr kernel.
                                  The number of pixels affected outside depends on the used
                                  kernel's size.
*/
typedef enum
{
    R_DRW2D_CONVMODE_TRIMMED,  
    R_DRW2D_CONVMODE_BLEEDING  
} r_drw2d_ConvMode_t;



/*******************************************************************************
  Enum: r_drw2d_Unit_t

  Enumeration of Drw2D Unit.

  Values:
    R_DRW2D_UNIT_DEFAULT    - This is the specification method provided 
                              for compatibility with previous applications.
    R_DRW2D_UNIT_CPU        - Select Drw2D unit for Drw2D on CPU.
    R_DRW2D_UNIT_DHD        - Select Drw2D unit for Drw2D on DHD.
*/
typedef enum
{
    R_DRW2D_UNIT_DEFAULT    = 0,
    R_DRW2D_UNIT_CPU        = 1,
    R_DRW2D_UNIT_DHD        = 2
} r_drw2d_Unit_t;





/*******************************************************************************
  Section: Structures
*/

/*******************************************************************************
  Type: r_drw2d_LineStyle_t

  Describes the drawing style for (poly-)lines.


  Fields:
     LineJoin - Line join style. One of <R_DRW2D_LINEJOIN_NONE>, <R_DRW2D_LINEJOIN_MITER>, 
                <R_DRW2D_LINEJOIN_BEVEL>, <R_DRW2D_LINEJOIN_ROUND>.
      LineCap - Line cap style. One of <R_DRW2D_LINECAP_ROUND>, <R_DRW2D_LINECAP_SQUARE>, 
                <R_DRW2D_LINECAP_BUTT>.
        Width - Line width
   MiterLimit - Maximum distance between miter line join tip and line point. 
                Value must be greater than zero. If the miter limit is exceeded, a bevel joint 
                will be drawn at the miter limit position.
     IsClosed - If true, draw closed polyline (last vertex is connected to first one)

  See also:
    <R_DRW2D_CtxLineStyle>, <r_drw2d_LineCap_t>, <r_drw2d_LineJoin_t>, <R_DRW2D_DrawPolyline>, 
    <R_DRW2D_DrawLines>
*/
typedef struct
{
    r_drw2d_LineJoin_t  LineJoin;
    r_drw2d_LineCap_t   LineCap;
    r_drw2d_FixedP_t    Width;
    r_drw2d_FixedP_t    MiterLimit;
    r_drw2d_Boolean_t   IsClosed;
} r_drw2d_LineStyle_t;



/*******************************************************************************
  Type: r_drw2d_Point_t

  A 2D point / vertex (fixed point).

  The vertex matrix (see <R_DRW2D_CtxTransform>) can be used to setup custom coordinate systems.


  Fields:
    X - Horizontal position (see <r_drw2d_FixedP_t>)
    Y - Vertical position (see <r_drw2d_FixedP_t>)

  See also:
    <R_DRW2D_DrawRect>
*/
typedef struct
{
    r_drw2d_FixedP_t X;
    r_drw2d_FixedP_t Y;
} r_drw2d_Point_t;



/***********************************************************
  Type: r_drw2d_Vec4_t

  A 3D point / vertex (4 component fixed point vector).

  This vertex type is used for the assessment of vertex transformations 
  (see <R_DRW2D_VtxTransform>).


  Fields:
    X - Horizontal position (see <r_drw2d_FixedP_t>)
    Y - Vertical position (see <r_drw2d_FixedP_t>)
    Z - 'Stacked' position (see <r_drw2d_FixedP_t>)
    W - Perspective information (see <r_drw2d_FixedP_t>)

  See also:
    <R_DRW2D_VtxTransform>
*/
typedef struct
{
    r_drw2d_FixedP_t X;
    r_drw2d_FixedP_t Y;
    r_drw2d_FixedP_t Z;
    r_drw2d_FixedP_t W;
} r_drw2d_Vec4_t;



/*******************************************************************************
  Type: r_drw2d_Size_t

  Specifies the size of a rectangle (fixed point).


  Fields:
     Width - Horizontal size (see <r_drw2d_FixedP_t>)
    Height - Vertical size (see <r_drw2d_FixedP_t>)

  See also:
    <R_DRW2D_DrawRect>
*/
typedef struct
{
    r_drw2d_FixedP_t Width;
    r_drw2d_FixedP_t Height;
} r_drw2d_Size_t;



/*******************************************************************************
  Type: r_drw2d_Rect_t

  A 2D rectangle, described by position and dimension (fixed point coordinates).


  Fields:
      Pos - Position (see <r_drw2d_Point_t>)
     Size - Size (see <r_drw2d_Size_t>)

  See also:
    <R_DRW2D_DrawRect>
*/
typedef struct
{
    r_drw2d_Point_t Pos;
    r_drw2d_Size_t  Size;
} r_drw2d_Rect_t;



/*******************************************************************************
  Type: r_drw2d_IntPoint_t

  A 2D point (integer).


  Fields:
    X - Horizontal position
    Y - Vertical position

  See also:
    <r_drw2d_IntRect_t>
*/
typedef struct
{
    int32_t X;
    int32_t Y;
} r_drw2d_IntPoint_t;



/*******************************************************************************
  Type: r_drw2d_IntSize_t

  Specifies the size of a rectangle (integer coordinates).


  Fields:
     Width - Horizontal size
    Height - Vertical size

  See also:
    <r_drw2d_IntRect_t>, <r_drw2d_Buffer_t>
*/
typedef struct
{
    int32_t Width;
    int32_t Height;
} r_drw2d_IntSize_t;



/*******************************************************************************
  Type: r_drw2d_IntRect_t

  A 2D rectangle, described by position and dimension (integer coordinates).


  Fields:
       Pos - Position (see <r_drw2d_IntPoint_t>)
      Size - Size (see <r_drw2d_IntSize_t>)

  See also:
    <R_DRW2D_CtxClipRect>
*/
typedef struct
{
    r_drw2d_IntPoint_t Pos;
    r_drw2d_IntSize_t  Size;
} r_drw2d_IntRect_t;



/*******************************************************************************
  Type: r_drw2d_UVCoord_t

  A 2D, normalized U/V coordinate (fixed point).

  (note) U/V are prescaled by 256, i.e. (256.0; 256.0) always maps to the bottom right texture 
  corner.
  
  R_DRW2D_2U and R_DRW2D_2V are supported macros to make this value.
  (U,V) = (R_DRW2D_2U(0.0), R_DRW2D_2V(0.0)) is top-left.
  (U,V) = (R_DRW2D_2U(1.0), R_DRW2D_2V(1.0)) is bottom-right.

  The texture matrix (see <R_DRW2D_CtxTextureTransform>) can be used to setup custom texture 
  coordinate systems.


  Fields:
    U - Normalized horizontal texel position, scaled by 256 
    V - Normalized vertical texel position, scaled by 256 

  See also:
    <R_DRW2D_DrawRectUV>, <R_DRW2D_DrawTrianglesUV>
*/
typedef struct
{
    r_drw2d_FixedP_t U;
    r_drw2d_FixedP_t V;
} r_drw2d_UVCoord_t;



/*******************************************************************************
  Type: r_drw2d_Buffer_t

  Buffers are used with <R_DRW2D_FramebufferSet> and textures (<R_DRW2D_CtxTextureSet>).

  The buffer starts at the top/left corner.

  (note) Please consult target-specific documentation regarding alignment rules
         (4bits-per-pixel, scanlines, start address)


  Fields:
          Data - Reference to pixel data (Specify the physical address that GPU can access)
         Pitch - Total number of pixels per line (including alignment/padding)
          Size - Buffer width and height
   PixelFormat - Pixel storage format. See <r_drw2d_PixelFormat_t>.
*/
typedef struct
{
    void                 *Data;
    int32_t               Pitch;
    r_drw2d_IntSize_t     Size;
    r_drw2d_PixelFormat_t PixelFormat;
} r_drw2d_Buffer_t;



/*******************************************************************************
  Type: r_drw2d_Framebuffer_t

  Framebuffer handle and attributes.


  Fields:
    Handle - Internal framebuffer handle (must not be modified by application)
    Buffer - Stores framebuffer geometry and pixel format
     Flags - reserved for future extensions (see <r_drw2d_FramebufferFlags_t>)

  See also:
    <R_DRW2D_FramebufferSet>
*/
/* Disable MISRA checks here  */
/* Rule 5.3 is violated, because
   Integrity defines a Buffer type, and we cannot change either GHS nor this code */
#if defined (__ghs__)
#pragma ghs startnomisra
#endif

typedef struct
{
    void                      *Handle;
    r_drw2d_Buffer_t           Buffer;
    r_drw2d_FramebufferFlags_t Flags;
} r_drw2d_Framebuffer_t;



/*******************************************************************************
  Type: r_drw2d_Texture_t

  Texture handle and attributes.


  Fields:
    Handle - Internal texture handle (must not be modified by application)
    Buffer - Stores texture geometry and pixel format.
     Flags - Texture flags, see <r_drw2d_TextureFlags_t>

  See also:
    <r_drw2d_Buffer_t>, <R_DRW2D_CtxTextureSet>
*/

typedef struct
{
    void                  *Handle;
    r_drw2d_Buffer_t       Buffer;
    r_drw2d_TextureFlags_t Flags;
} r_drw2d_Texture_t;

#if defined (__ghs__)
#pragma ghs endnomisra
#endif



/*******************************************************************************
  Type: r_drw2d_EffectParam_t

  Effect parameter contains parameter information for one parameter of one effect.


  Fields:
    Source                           - Source of the parameter (type 
                                       <r_drw2d_EffectParamSource_t>), declares what 'Param' is.
    Param                            - The parameter itself as a union. Can be .Color, .Constant 
                                       or .Point, as indicated by 'Source'. See below for details.
    Param.Color                      - A struct with the fields .Operand and .Source. 
                                       See below for details.
    Param.Color.Operand              - Defines how the color value is to be used,
                                       e.g. inverted or not 
                                       (type <r_drw2d_EffectColorParamOperand_t>)
    Param.Color.Source               - A union, either .TextureUnit or .ConstantColor, as 
                                       indicated by 'Source' above.
    Param.Color.Source.TextureUnit   - Index of a texture unit. 0 or 1. 
    Param.Color.Source.ConstantColor - 32bit ARGB8888 color value (type <r_drw2d_Color_t>)
    Param.Constant                   - 16.16 signed fixed point constant value 
                                       (type <r_drw2d_FixedP_t>)
    Param.Point                      - 16.16 signed 2D fixed point coordinate 
                                       (type <r_drw2d_Point_t>).

  See also:
    <r_drw2d_EffectName_t>, <r_drw2d_EffectParam_t>, <R_DRW2D_CtxEffectsSet>, 
    <R_DRW2D_CtxEffectsDelete>, <R_DRW2D_CtxEffectsUpdate>
*/
typedef struct
{
    r_drw2d_EffectParamSource_t Source;
    union
    {
        struct
        {
            union
            {
                uint32_t        TextureUnit; 
                r_drw2d_Color_t ConstantColor;
            } Source;
            r_drw2d_EffectColorParamOperand_t Operand;
        } Color;
        r_drw2d_FixedP_t Constant;
        r_drw2d_Point_t  Point;
    } Param;
} r_drw2d_EffectParam_t;



/*******************************************************************************
  Type: r_drw2d_EffectStage_t

  Effect stage contains information about one effect.


  Fields:
    Name - Name of the effect, type <r_drw2d_EffectName_t>
    Args - Parameters of the effect, array of type <r_drw2d_EffectParam_t> 
           (used length of this array depends on 'Name' of the effect, maximum is 4)

  See also:
    <r_drw2d_EffectName_t>, <r_drw2d_EffectParam_t>, <R_DRW2D_CtxEffectsSet>, 
    <R_DRW2D_CtxEffectsDelete>, <R_DRW2D_CtxEffectsUpdate>
*/
typedef struct
{
    r_drw2d_EffectName_t  Name;
    r_drw2d_EffectParam_t Args[4];
} r_drw2d_EffectStage_t;



/*******************************************************************************
  Type: r_drw2d_ConvKernel_t

  Convolution kernel containing dimensions and coefficients of a kernel.


  Fields:
    Coeff   - The coefficients of the kernel (row by row).
    Channel - The color channels being processed by the kernel.
    Width   - Width of the kernel.
    Height  - Height of the kernel.
    Bias    - Bias value that is added to the resulting color channel values (range: -1.0 to 1.0).

  See also:
    <R_DRW2D_CtxConvolutionKernel>, <R_DRW2D_DrawRectConvolve2d>
*/
typedef struct
{
    const r_drw2d_FixedP_t           *Coeff;
    r_drw2d_ConvKernelColorChannel_t Channel;
    int32_t                          Width;
    int32_t                          Height;
    r_drw2d_FixedP_t                 Bias;
} r_drw2d_ConvKernel_t;



/***********************************************************
  Type: r_drw2d_DeviceBase_t

  Common base class type for all DRW2D-internal device contexts.
*/
struct r_drw2d_DeviceBase_s;
typedef struct r_drw2d_DeviceBase_s r_drw2d_DeviceBase_t;



/***********************************************************
  Type: r_drw2d_RenderContext_s

  Render context attributes.


  Fields:
       DeviceBase       - Parent device context or NULL if render context is unused
         ClipRect       - Clipping rectangle (not clipped to current framebuffer)
         Viewport       - View port.
          FgColor       - Foreground color (ARGB32)
          BgColor       - Background color (ARGB32)
         FillMode       - Fill mode
        LineStyle       - Line join/cap style, line width, miterlimit and "closed" flag
       ImgQuality       - Current image quality (antialiasing mode)
    TransformMode       - Vertex transform mode (see <r_drw2d_TransformMode_t>)
  TextureTransformMode  - Texture transform mode (See <r_drw2d_TextureTransformMode_t>)
        BlendMode       - Blendmode
        CullMode        - Cull mode (See <r_drw2d_CullMode_t>)
    EnableStriping      - Enable stripping.
    ConvKernelPreset1d  - 1D convolution filter kernel presets
    ConvKernelPreset2d  - 2D convolution filter kernel presets
    EffectStages        - Effect stages
    NumberOfStages      - Number of effect stages.
     BlendFactors       - Used when BlendMode is set to <R_DRW2D_BLENDMODE_CUSTOM>
    TextureMatrix       - Current texture matrix
     VertexMatrix       - Current vertex matrix
          Texture       - Current texture

  See also:
    <r_drw2d_RenderContext_t>
*/
struct r_drw2d_RenderContext_s
{
    r_drw2d_DeviceBase_t *DeviceBase;

    r_drw2d_IntRect_t                   ClipRect;
    r_drw2d_IntRect_t                   Viewport; /* (0,0),(0,0) == use current framebuffer size */
    r_drw2d_Color_t                     FgColor;
    r_drw2d_Color_t                     BgColor;
    r_drw2d_FillMode_t                  FillMode;
    r_drw2d_LineStyle_t                 LineStyle;
    r_drw2d_ImgQuality_t                ImgQuality;
    r_drw2d_TransformMode_t             TransformMode;
    r_drw2d_TextureTransformMode_t      TextureTransformMode;
    r_drw2d_BlendMode_t                 BlendMode;
    r_drw2d_CullMode_t                  CullMode;
    r_drw2d_Boolean_t                   EnableStriping;
    r_drw2d_ConvolutionKernelPreset1d_t ConvKernelPreset1d;
    r_drw2d_ConvolutionKernelPreset2d_t ConvKernelPreset2d;
    const r_drw2d_ConvKernel_t*         ConvKernel;
    r_drw2d_ConvMode_t                  ConvMode;

    r_drw2d_EffectStage_t              *EffectStages;
    uint32_t                            NumberOfStages;

    uint32_t                            ClutBase;
    struct
    {
        r_drw2d_BlendFactor_t           SrcRGB;
        r_drw2d_BlendFactor_t           DstRGB;
        r_drw2d_BlendFactor_t           SrcAlpha;
        r_drw2d_BlendFactor_t           DstAlpha;
    } BlendFactors;

    r_drw2d_FixedP_t                    TextureMatrix[3*2];
    r_drw2d_FixedP_t                    VertexMatrix[4*4];
    r_drw2d_Texture_t                   Texture[2];
};



/***********************************************************
  Type: r_drw2d_RenderContext_t

  Structure type for the render context.

  See also:
    <r_drw2d_RenderContext_s>
*/
typedef struct r_drw2d_RenderContext_s r_drw2d_RenderContext_t;



/***********************************************************
  Type: r_drw2d_DeviceBase_s

  Description:
    Common base class for all DRW2D-internal device contexts.

    This structure must be the first field of all driver-specific device context structures.


  Fields:
                 NumBytes - Total size (in bytes) of driver specific device context
                            (including DeviceBase structure)
                     Unit - Parent unit# of device context (DRW2D unit number)
                   Mutex - Used to synchronize access to this device context
                    Error - Error callback function pointer and user data. 
                            (see <R_DRW2D_ErrCallbackSet>, <r_drw2d_ErrorCallback_t>).
     DefaultRenderContext - Default render context.
     CurrentRenderContext - Reference to the current render context. 
                            Points whether to the DefaultRenderContext or a user-set context.
       CurrentFramebuffer - Reference to the current framebuffer.
                            Points whether to the default FB or a user-set FB.
            APIDirtyFlags - Bitmask that indicates what to update on API-side when a Draw*() 
                            function is called. See R_DRW2D_API_DIRTY_*.
            SysDirtyFlags - Bitmask that indicates what to update on Sys-side when a Draw*() 
                            function is called. See R_DRW2D_SYS_DIRTY_*.
        EffectiveClipRect - Current clipping rectangle
           TexCoordsTrans - Transformed texture coordinate cache for static texture mapping
             ConvolveMode - Convolution filter mode (1d horizontal, 1d vertical, 2d)
            FunctionTable - Where to store Function Table defined to have a common interface 
                            to processing that depends on the drawing engine used.

  See also:
    <r_drw2d_Device_t>, <r_drw2d_DeviceBase_t>

*/
/* Rules 5.4 and 5.3 are violated, and we cannot change either GHS nor this code */
#if defined (__ghs__)
#pragma ghs startnomisra
#endif

struct r_drw2d_DeviceBase_s
{
    uint32_t                     NumBytes;
    r_drw2d_Unit_t               Unit;
    r_drw2d_OS_Mutex_t           Mutex;

    struct
    {
        void                    *UserData;
        r_drw2d_ErrorCallback_t  Callback;
    } Error;

    r_drw2d_FixedP_t             DepthRangeNear;
    r_drw2d_FixedP_t             DepthRangeFar;

    r_drw2d_RenderContext_t      DefaultRenderContext;

    struct
    {
        r_drw2d_RenderContext_t *CurrentRenderContext;
        r_drw2d_Framebuffer_t    CurrentFramebuffer;
        uint32_t                 APIDirtyFlags;
        uint32_t                 SysDirtyFlags;
        r_drw2d_IntRect_t        EffectiveClipRect;
        r_drw2d_Point_t          TexCoordsTrans[3];
        r_drw2d_ConvolveMode_t   ConvolveMode;
    } State;
    void                        *FunctionTable;
};

#if defined (__ghs__)
#pragma ghs endnomisra
#endif




/*******************************************************************************
  Section: Global functions
*/


/*******************************************************************************
  Group: Basic functions
*/

/*******************************************************************************
  Function: R_DRW2D_Init

  Initialize DRW2D API and initialize global DRW2D resources.

  Must be called during system startup.

  Not threadsafe, The application must ensure that only one thread calls this function.

  The underlying drawing engine should be initialized prior to calling this function. 
  In case of a D/AVE HD, this means calling the D/AVE HD kernel mode API initialization functions 
  and enable the GPU2D interrupts.


  Parameter:
    void

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
*/
r_drw2d_Error_t R_DRW2D_Init(void);



/*******************************************************************************
  Function: R_DRW2D_Open

  Initialize DRW2D unit and driver-dependent graphics engine and return device handle.

  This function initializes the driver-dependent graphics engine to its default configuration.
  It initializes the internal device structure and returns an opaque handle to that structure.

  A default render context is created implicitly.

  <R_DRW2D_Open> must be called before any drawing function can take place.

  The application must ensure that the RetDevice is not used in more than one thread at a time.
  Ensure that you provide a real structure pointer as DeviceInternal, not a void pointer!
  Also ensure that the DeviceInternal structure members are initialised with 0.


  Parameter:
              Unit - Unit number (see <r_drw2d_Unit_t>)
                     This parameter can only be set to 0.
        DriverUnit - Driver unit number
                     This parameter can only be set to 0.
    DeviceInternal - Pointer to the gfx driver handle,
                     Set <r_drw2d_DeviceDHD_t> defined in "r_drw2d_ctx_dhd.h" 
                     in case of Drw2D with D/AVE HD.
                     Set <r_drw2d_DeviceCPU_t> defined in "r_drw2d_ctx_cpu.h" 
                     in case of Drw2D with CPU drawing.
                     Normally, this variable is never used by user, 
                     but it must be kept until <R_DRW2D_Close> is called.
         RetDevice - Returns the (opaque) Drw2d device handle

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
*/
r_drw2d_Error_t R_DRW2D_Open(r_drw2d_Unit_t    Unit,
                             int32_t           DriverUnit,
                             void             *DeviceInternal,
                             r_drw2d_Device_t *RetDevice
                             );



/*******************************************************************************
  Function: R_DRW2D_Exit

  Shutdown DRW2D API and de-initialize global DRW2D resources.

  Must be called during system shutdown.

  Must not be called when <R_DRW2D_Init> has failed.

  Not threadsafe, The application must ensure that all DRW2D units have been closed and
  only one thread calls this function.
  
  The underlying drawing engine should be de-initialized after calling this function. 
  In case of a D/AVE HD, this means calling the D/AVE HD kernel mode API shutdown functions 
  and disables the GPU2D interrupts.


  Parameter:
    void

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
*/
r_drw2d_Error_t R_DRW2D_Exit(void);



/*******************************************************************************
  Function: R_DRW2D_Close

  Shutdown DRW2D unit, de-initialize driver-dependent graphics engine and cleans up 
  its internally used resources.


  Parameter:
    Device - Device handle (see <r_drw2d_Device_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
*/
r_drw2d_Error_t R_DRW2D_Close(r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_VersionString

  Returns the version string of the DRW2D API.


  Parameter:
    void

  Return value:
   Version string
*/
const char_t *R_DRW2D_VersionString(void);





/*******************************************************************************
  Group: Native driver interface

   The following functions can be used to bypass the DRW2D API and access the low level, 
   hardware-specific driver directly.
*/

/*******************************************************************************
  Function: R_DRW2D_NativeDriverHandleGet

  Returns a handle to the low level driver instance.

  DRW2D must have been initialized using <R_DRW2D_Init> prior to calling this function.


  Parameter:
                Device - Device handle (see <r_drw2d_Device_t>)
    RetNativeDrvHandle - Returns Native driver handle

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
   <R_DRW2D_NativeDriverBegin>, <R_DRW2D_NativeDriverEnd>, <r_drw2d_NativeDrvFlags_t>
*/
r_drw2d_Error_t R_DRW2D_NativeDriverHandleGet(r_drw2d_Device_t   Device,
                                              void             **RetNativeDrvHandle
                                              );



/*******************************************************************************
  Function: R_DRW2D_NativeDriverBegin

  Notify DRW2D that the application wants to access the low level, hardware-specific driver 
  directly.


  Parameter:
    Device - Device handle (see <r_drw2d_Device_t>)
    Flags  - One of <R_DRW2D_NATIVEDRV_SAVESTATE>, <R_DRW2D_NATIVEDRV_KEEPSTATE>, 
             <R_DRW2D_NATIVEDRV_APPCONTEXT>. 
             The upper 16bits of this parameter are reserved for driver-specific extensions.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
   <R_DRW2D_NativeDriverHandleGet>, <R_DRW2D_NativeDriverEnd>, <r_drw2d_NativeDrvFlags_t>
*/
r_drw2d_Error_t R_DRW2D_NativeDriverBegin(r_drw2d_Device_t Device,
                                          uint32_t         Flags
                                          );



/*******************************************************************************
  Function: R_DRW2D_NativeDriverEnd

  Notify DRW2D that the application has finished accessing the low level, hardware-specific driver 
  directly.


  Parameter:
    Device - Device handle (see <r_drw2d_Device_t>)
    Flags  - Reserved for future extensions. Pass 0 for now.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
   <R_DRW2D_NativeDriverHandleGet>, <R_DRW2D_NativeDriverBegin>, <r_drw2d_NativeDrvFlags_t>
*/
r_drw2d_Error_t R_DRW2D_NativeDriverEnd(r_drw2d_Device_t Device,
                                        uint32_t         Flags
                                        );





/*******************************************************************************
  Group: Context management functions

  The application can initialize and select render contexts with this API.

  A context stores the following settings:
    - Clipping rectangle (see <R_DRW2D_CtxClipRect>)
    - View port (see <R_DRW2D_CtxViewport>)
    - Foreground color and alpha (see <R_DRW2D_CtxFgColor>)
    - Background color and alpha (see <R_DRW2D_CtxBgColor>)
    - Cull mode (see <R_DRW2D_CtxCullMode>)
    - Striping (see <R_DRW2D_CtxStripingEnable>, <R_DRW2D_CtxStripingDisable>)
    - 1D convolution filter kernel presets (see <R_DRW2D_CtxConvolutionKernelPreset1d>)
    - 2D convolution filter kernel presets (see <R_DRW2D_CtxConvolutionKernelPreset2d>)
    - Convolution filter kernel (see <R_DRW2D_CtxConvolutionKernel>)
    - Effect stages (see <R_DRW2D_CtxEffectsSet>, <R_DRW2D_CtxEffectsDelete>, 
      <R_DRW2D_CtxEffectsUpdate>)
    - Number of effect stages (see <R_DRW2D_CtxEffectsSet>, <R_DRW2D_CtxEffectsDelete>,
      <R_DRW2D_CtxEffectsUpdate>)
    - Fill mode (see <R_DRW2D_CtxFillMode>, <r_drw2d_FillMode_t>)
    - Blend mode (see <R_DRW2D_CtxBlendMode>, <r_drw2d_BlendMode_t>)
    - Blend factors (see <R_DRW2D_CtxBlendFactors>, <r_drw2d_BlendFactor_t>)
    - Transformation matrix (see <R_DRW2D_CtxTransform>, <R_DRW2D_CtxTranslate>, 
      <R_DRW2D_CtxRotate>, <R_DRW2D_CtxRotate3d>, <R_DRW2D_CtxScale>)
    - Texture matrix (see <R_DRW2D_CtxTextureTransform>, <R_DRW2D_CtxTextureTranslate>, 
      <R_DRW2D_CtxTextureRotate>, <R_DRW2D_CtxTextureScale>)
    - Line style (see <R_DRW2D_CtxLineStyle>, <r_drw2d_LineCap_t>, <r_drw2d_LineJoin_t>,
      <r_drw2d_LineStyle_t>)
    - Image quality / antialiasing mode (see <R_DRW2D_CtxImgQuality>, <r_drw2d_ImgQuality_t>)
    - Transform mode (see <R_DRW2D_CtxTransformMode>, <r_drw2d_TransformMode_t>)
    - Source texture (see <R_DRW2D_CtxTextureSet>, <r_drw2d_Texture_t>)

  The context state can be changed by calling one of the R_DRW2D_Ctx*() functions.

  The DRW2D API provides one default context by default, which is initialized in <R_DRW2D_Open>.

  Custom render contexts can be setup using <R_DRW2D_ContextInit> and selected with 
  <R_DRW2D_ContextSelect>.

*/

/*******************************************************************************
  Function: R_DRW2D_ContextInit

  Initialize a render context with default settings.


  Parameter:
           Device - Device handle (see <r_drw2d_Device_t>)
    RenderContext - Reference to uninitialized render context structure. 
                    (see <r_drw2d_RenderContext_s>).
       RetContext - The context pointer that is initialized

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_ContextSelect>
*/
r_drw2d_Error_t R_DRW2D_ContextInit(r_drw2d_Device_t                Device,
                                    struct r_drw2d_RenderContext_s *RenderContext,
                                    r_drw2d_Context_t              *RetContext
                                    );



/*******************************************************************************
  Function: R_DRW2D_ContextSelect

  Sets the given context as the current one. 
  Passing Context handle NULL will select the default context.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
    Context - Render context handle (see <r_drw2d_Context_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_ContextInit>

*/
r_drw2d_Error_t R_DRW2D_ContextSelect(r_drw2d_Device_t  Device,
                                      r_drw2d_Context_t Context
                                      );





/*******************************************************************************
  Group: Context control functions

  All of the functions in this section operate globally on the currently selected context
  (statemachine-like in OpenGL/OpenVG). That means they influence primitive drawing.

  Resetting values or setting back to default is the responsibility of the application/framework 
  above Drw2D.
*/

/*******************************************************************************
  Function: R_DRW2D_CtxFgColor

  Set the foreground color to be used for drawing primitives with solid mode.

  Between calls to <R_DRW2D_CtxEffectsSet> and <R_DRW2D_CtxEffectsDelete>, 
  this function is invalid and an error will be returned.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
      Color - The color value (32bit packed ARGB)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxBgColor>, <R_DRW2D_CtxBlendFactors>, <r_drw2d_BlendFactor_t>
*/
r_drw2d_Error_t R_DRW2D_CtxFgColor(r_drw2d_Device_t Device,
                                   r_drw2d_Color_t  Color
                                   );



/*******************************************************************************
  Function: R_DRW2D_CtxBgColor

  Set the background color to be used for drawing primitives.
  
  The <R_DRW2D_FramebufferClear> function always uses the current background color.

  Between calls to <R_DRW2D_CtxEffectsSet> and <R_DRW2D_CtxEffectsDelete>, 
  this function is invalid and an error will be returned.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
      Color - The color value (32bit packed ARGB)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxFgColor>, <R_DRW2D_CtxBlendFactors>, <r_drw2d_BlendFactor_t>
*/
r_drw2d_Error_t R_DRW2D_CtxBgColor(r_drw2d_Device_t Device,
                                   r_drw2d_Color_t  Color
                                   );



/*******************************************************************************
  Function: R_DRW2D_CtxClipRect

  Sets a global clipping rectangle for subsequent drawing operations.
  A clip-rect can be set by calling this function before each R_DRW2D_Draw* API 
  and R_DRW2D_FramebufferClear.

  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
       Rect - Rectangular clipping area (window coordinates) (see <r_drw2d_IntRect_t>).
              All elements must be positive numbers (or 0).

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
*/
r_drw2d_Error_t R_DRW2D_CtxClipRect(r_drw2d_Device_t   Device,
                              const r_drw2d_IntRect_t *Rect
                                    );



/*******************************************************************************
  Function: R_DRW2D_CtxFillMode

  Set the filling mode for drawing with primitives.

  This is used for placing e.g. a texture on top of any primitive.

  Between calls to <R_DRW2D_CtxEffectsSet> and <R_DRW2D_CtxEffectsDelete>, 
  this function is invalid and an error will be returned.

  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
       Mode - The fill mode to be used. See <r_drw2d_FillMode_t>.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
*/
r_drw2d_Error_t R_DRW2D_CtxFillMode(r_drw2d_Device_t   Device,
                                    r_drw2d_FillMode_t Mode
                                    );



/*******************************************************************************
  Function: R_DRW2D_CtxCullMode

  Set the culling mode for drawing with primitives.

  This is used to discard triangles or rectangles depending on their winding order.

  The default cull mode is <R_DRW2D_CULLMODE_NONE>.


  Parameter:
      Device - Device handle (see <r_drw2d_Device_t>)
    CullMode - The cull mode to be used. See <r_drw2d_CullMode_t>.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
*/
r_drw2d_Error_t R_DRW2D_CtxCullMode(r_drw2d_Device_t   Device,
                                    r_drw2d_CullMode_t CullMode
                                    );



/*******************************************************************************
  Function: R_DRW2D_CtxLineStyle

  Set the line drawing style (e.g. line caps, line width, ..).


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
      Style - Pointer to line style struct (see <r_drw2d_LineStyle_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <r_drw2d_LineCap_t>, <r_drw2d_LineJoin_t>, <R_DRW2D_DrawLines>, <R_DRW2D_DrawPolyline>
*/
r_drw2d_Error_t R_DRW2D_CtxLineStyle(r_drw2d_Device_t     Device,
                               const r_drw2d_LineStyle_t *Style
                                     );



/*******************************************************************************
  Function: R_DRW2D_CtxBlendMode

  Set preset color/alpha source/destination blending equations.

  When <R_DRW2D_BLENDMODE_CUSTOM> is selected, the color/alpha blending equations
  are determined by the blend factors set by <R_DRW2D_CtxBlendFactors>.


  Parameter:
       Device - Device handle (see <r_drw2d_Device_t>)
    BlendMode - The blendmode mode to be used. See <r_drw2d_BlendMode_t>.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <r_drw2d_BlendMode_t>, <R_DRW2D_CtxBlendFactors>, <r_drw2d_BlendFactor_t>
*/
r_drw2d_Error_t R_DRW2D_CtxBlendMode(r_drw2d_Device_t    Device,
                                     r_drw2d_BlendMode_t BlendMode
                                     );



/*******************************************************************************
  Function: R_DRW2D_CtxBlendFactors

  Set color/alpha source/destination blending factors.

  In order for these to have an effect, the <R_DRW2D_BLENDMODE_CUSTOM>
  blendmode must be selected (see <R_DRW2D_CtxBlendMode>).

  The effective color/alpha blend equation is determined by
    > dst_color = src_color*src_factor_color + dst_color*dst_factor_color
    > dst_alpha = src_alpha*src_factor_alpha + dst_alpha*dst_factor_alpha


  Parameter:
       Device - Device handle (see <r_drw2d_Device_t>)
       SrcRGB - The blendfactor to be used for source RGB values (see <r_drw2d_BlendFactor_t>)
       DstRGB - The blendfactor to be used for destination RGB values (see <r_drw2d_BlendFactor_t>)
     SrcAlpha - The blendfactor to be used for source alpha values (see <r_drw2d_BlendFactor_t>)
     DstAlpha - The blendfactor to be used for destination alpha values (see <r_drw2d_BlendFactor_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <r_drw2d_BlendFactor_t>, <R_DRW2D_CtxBlendMode>, <r_drw2d_BlendMode_t>

*/
r_drw2d_Error_t R_DRW2D_CtxBlendFactors(r_drw2d_Device_t      Device,
                                        r_drw2d_BlendFactor_t SrcRGB,
                                        r_drw2d_BlendFactor_t DstRGB,
                                        r_drw2d_BlendFactor_t SrcAlpha,
                                        r_drw2d_BlendFactor_t DstAlpha
                                        );



/*******************************************************************************
  Function: R_DRW2D_CtxImgQuality

  Sets for the current context a global quality value used for graphics primitives.
  
  If Quality is R_DRW2D_IMGQUALITY_LOW, antialiasing and some effect features are not 
  enabled. Refer to <R_DRW2D_CtxEffectsSet> for details of effect features. 
  This mode can be used as a performance optimization.

  Note that the DHD platform is limited to an edge width/height of max. 2048 when
  using non antialiased (R_DRW2D_IMGQUALITY_LOW) edges.
  
  No restrictions on R_DRW2D_IMGQUALITY_MIDDLE and R_DRW2D_IMGQUALITY_HIGH.

  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
    Quality - Image quality/antialiasing mode. See <r_drw2d_ImgQuality_t>.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <r_drw2d_ImgQuality_t>
*/
r_drw2d_Error_t R_DRW2D_CtxImgQuality(r_drw2d_Device_t     Device,
                                      r_drw2d_ImgQuality_t Quality
                                      );



/*******************************************************************************
  Function: R_DRW2D_CtxTransformMode

  Set the vertex transform/projection mode.

  The default is <R_DRW2D_TRANSFORM_2D> (2D vertex matrix transformation).

  When set to <R_DRW2D_TRANSFORM_NONE>, vertices will be used as-is, i.e. they
  will not be transformed by the vertex matrix. This mode can be used as a
  performance optimization.
  When no explicit UV coordinates are given, the texture is mapped to the framebuffer 1:1.

  When set to <R_DRW2D_TRANSFORM_3D>, vertices will be transformed by the
  4x4 vertex matrix and projected onto the current viewport.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
       Mode - The transform mode to be used. See <r_drw2d_TransformMode_t>.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
*/
r_drw2d_Error_t R_DRW2D_CtxTransformMode(r_drw2d_Device_t        Device,
                                         r_drw2d_TransformMode_t Mode
                                         );



/*******************************************************************************
  Function: R_DRW2D_CtxTextureTransformMode

  Set the texture coordinate transformation mode.

  The default is <R_DRW2D_TEX_TRANSFORM_2D> (2D texture matrix transformation).

  When set to <R_DRW2D_TEX_TRANSFORM_NONE>, texture coordinates will be used as-is,
  i.e. they will not be transformed by the texture matrix. This mode can be used as a
  performance optimization. When no explicit UV coordinates are given, the texture
  is mapped to the screen 1:1.

  When set to <R_DRW2D_TEX_TRANSFORM_2D>, texture coordinates will be transformed 
  by the 3x2 texture matrix.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
       Mode - The transform mode to be used. See <r_drw2d_TextureTransformMode_t>.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
*/
r_drw2d_Error_t R_DRW2D_CtxTextureTransformMode(r_drw2d_Device_t               Device,
                                                r_drw2d_TextureTransformMode_t Mode
                                                );



/*******************************************************************************
  Function: R_DRW2D_CtxViewport

  Set viewport for 3D vertex transformation/projection.

  The default is to use the current framebuffer width/height, offset by (0; 0).

  After setting a custom viewport, set the Rect.Size to (0; 0) to revert to the default behaviour.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
       Rect - The viewport rectangle. See <r_drw2d_IntRect_t>.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
*/
r_drw2d_Error_t R_DRW2D_CtxViewport(r_drw2d_Device_t   Device,
                              const r_drw2d_IntRect_t *Rect
                                    );



/*******************************************************************************
  Function: R_DRW2D_CtxStripingEnable

  Enable striped pixel enumeration (for performance reasons).

  This should only be enabled if texture mapping is used and larger triangles/quads 
  are being rendered.

  The actual stripe settings are determined per-primitive.

  
  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxStripingDisable>
*/
r_drw2d_Error_t R_DRW2D_CtxStripingEnable(r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_CtxStripingDisable

  Disable striped pixel enumeration.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxStripingEnable>
*/
r_drw2d_Error_t R_DRW2D_CtxStripingDisable(r_drw2d_Device_t Device);





/*******************************************************************************
  Group: Effect functions

  For more information, help and examples concerning the Drw2D Effects API, 
  see User's manual.

*/

/*******************************************************************************
  Function: R_DRW2D_CtxEffectsSet

  Sets an array of effects to be used for colorization and blending.
  The data in the array must be readable/writeable until a call to <R_DRW2D_CtxEffectsDelete>.

  The content of the array will be modified by <R_DRW2D_CtxEffectsUpdate>.

  Between calls to <R_DRW2D_CtxEffectsSet> and <R_DRW2D_CtxEffectsDelete> any calls to functions
  <R_DRW2D_CtxFgColor>, <R_DRW2D_CtxBgColor>, and <R_DRW2D_CtxFillMode> are invalid and an error
  will be returned.

  The effects <R_DRW2D_EFFECT_REPLACE>, <R_DRW2D_EFFECT_MODULATE>, <R_DRW2D_EFFECT_ADD>, 
  <R_DRW2D_EFFECT_SUBTRACT>, <R_DRW2D_EFFECT_ADD_SIGNED>, <R_DRW2D_EFFECT_INTERPOLATE>, 
  and <R_DRW2D_EFFECT_DOT3> can be combined to calculate a final color.  
  If the input source is specified as <R_DRW2D_EFFECT_SOURCE_PREV_STAGE>, the result of the 
  previous stage will be used.

  If the combination of effects can not be realized, an error will be returned by the drawing call.

  The effects <R_DRW2D_EFFECT_CONSTANT_ALPHA> and <R_DRW2D_EFFECT_GRADIENT> provide a final alpha 
  blending.
  
  It is necessary to set the image quality to <R_DRW2D_IMGQUALITY_MEDIUM> or 
  <R_DRW2D_IMGQUALITY_HIGH> (see <R_DRW2D_CtxImgQuality>) for these effects to work:
  
  In the case of <R_DRW2D_EFFECT_CONSTANT_ALPHA> the result will be blended with a constant alpha.
  The <R_DRW2D_EFFECT_GRADIENT> can be used to specify two points and two alpha values to 
  calculate a linear gradient, which will be used to blend the final color. 
  
  The effects <R_DRW2D_EFFECT_GRADIENT> and <R_DRW2D_EFFECT_CONSTANT_ALPHA> can be combined.

  See User's manual for more information on how to use effects.


  Parameter:
     Device  - Device handle (see <r_drw2d_Device_t>)
     Effects - Array with effects (see <r_drw2d_EffectStage_t>)
     Count   - Number of effects in the array

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxEffectsUpdate>, <R_DRW2D_CtxEffectsDelete>
*/
r_drw2d_Error_t R_DRW2D_CtxEffectsSet(r_drw2d_Device_t       Device,
                                      r_drw2d_EffectStage_t *Effects,
                                      uint32_t               Count
                                      );



/*******************************************************************************
  Function: R_DRW2D_CtxEffectsUpdate

  updates effect at stage Stage.


  Parameter:
     Device    - Device handle (see <r_drw2d_Device_t>)
     Name      - Effect (see <r_drw2d_EffectName_t>)
     Stage     - Stage of effect to be updated
     NumParams - Number of parameters to be passed
     Params    - Array of parameters (see <r_drw2d_EffectParam_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxEffectsSet>, <R_DRW2D_CtxEffectsDelete>
*/
r_drw2d_Error_t R_DRW2D_CtxEffectsUpdate(r_drw2d_Device_t Device,
                                         r_drw2d_EffectName_t Name,
                                         uint32_t Stage,
                                         uint32_t NumParams,
                                   const r_drw2d_EffectParam_t *Params
                                         );



/*******************************************************************************
  Function: R_DRW2D_CtxEffectsDelete

  Deletes all effects.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxEffectsSet>, <R_DRW2D_CtxEffectsUpdate>
*/
r_drw2d_Error_t R_DRW2D_CtxEffectsDelete(r_drw2d_Device_t Device);





/*******************************************************************************
  Group: Texture functions

*/

/*******************************************************************************
  Function: R_DRW2D_CtxTextureSet

  Set source texture.

  The Drw2D with D/AVE HD supports the direct / zero-copy use of an application provided texture 
  address.
  
  The texture is used when the fill mode is set to <R_DRW2D_FILLMODE_TEXTURE>.


  Parameter:
    Device      - Device handle (see <r_drw2d_Device_t>)
    TextureUnit - Texture Unit Number.
    Texture     - Reference to texture structure (see <r_drw2d_Texture_t>). 
                  NULL to deselect current texture.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <r_drw2d_Texture_t>, <r_drw2d_TextureFlags_t>, <r_drw2d_Buffer_t>, <R_DRW2D_CtxFillMode>, 
    <R_DRW2D_TextureBlit>
*/
r_drw2d_Error_t R_DRW2D_CtxTextureSet(r_drw2d_Device_t   Device,
                                      uint32_t           TextureUnit,
                                const r_drw2d_Texture_t *Texture
                                      );



/*******************************************************************************
  Function: R_DRW2D_TextureBlit

  Blit texture from Src to Dest.

  Src can be NULL, in that case the blit origin is 0,0 inside of the texture and dimensions are 
  taken from the DstRect.


  Parameter:
    Device - Device handle (see <r_drw2d_Device_t>)
    SrcRect - Source rectangle.
    DstRect - Destination rectangle.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <r_drw2d_Texture_t>, <r_drw2d_TextureFlags_t>, <r_drw2d_Buffer_t>, <R_DRW2D_CtxFillMode>, 
    <R_DRW2D_CtxTextureSet>
*/
r_drw2d_Error_t R_DRW2D_TextureBlit(r_drw2d_Device_t  Device,
                              const r_drw2d_Rect_t   *SrcRect,
                              const r_drw2d_Rect_t   *DstRect
                                    );



/*******************************************************************************
  Function: R_DRW2D_CtxTextureColorKeyEnable

  Enables Color Keying for the provided RGB color.
  The color after replacement will be fully transparent.


  Parameter:
    Device   - Device handle (see <r_drw2d_Device_t>)
    ColorKey - The color before replacement in RGB (alpha component is ignored)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
*/
r_drw2d_Error_t R_DRW2D_CtxTextureColorKeyEnable(r_drw2d_Device_t Device,
                                                 r_drw2d_Color_t  ColorKey
                                                 );



/*******************************************************************************
  Function: R_DRW2D_CtxTextureColorKeyDisable

  Disabled a previously set color keying.


  Parameter:
    Device   - Device handle (see <r_drw2d_Device_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
*/
r_drw2d_Error_t R_DRW2D_CtxTextureColorKeyDisable(r_drw2d_Device_t Device);





/*******************************************************************************
  Group: Matrix transformation functions

*/

/*******************************************************************************
  Function: R_DRW2D_CtxIdentity

  Reset vertex transformation matrix.

  The identity matrix maps the vertex at (0; 0) to the top/left framebuffer position.

  The vertex at (framebuffer_width-1, framebuffer_height-1) is mapped to the bottom/right
  framebuffer position.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxTransform>, <R_DRW2D_CtxRotate>, <R_DRW2D_CtxRotate3d>, <R_DRW2D_CtxScale>, 
    <R_DRW2D_CtxTranslate>, <R_DRW2D_CtxTextureIdentity>, <R_DRW2D_CtxTextureTransform>, 
    <R_DRW2D_CtxTextureRotate>, <R_DRW2D_CtxTextureScale>, <R_DRW2D_CtxTextureTranslate>
*/
r_drw2d_Error_t R_DRW2D_CtxIdentity(r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_CtxTextureIdentity

  Reset texture matrix.

  If both texture and vertex matrices are set to identity, vertices and texture coordinates 
  will use the same coordinate system.

  I.e. drawing a textured rectangle at (10;10) with size (40;30) will result in a 1:1 mapping of 
  the respective texture area.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxTextureTransform>, <R_DRW2D_CtxTextureRotate>, <R_DRW2D_CtxTextureScale>, 
    <R_DRW2D_CtxTextureTranslate>, <R_DRW2D_CtxIdentity>, <R_DRW2D_CtxTransform>, 
    <R_DRW2D_CtxRotate>, <R_DRW2D_CtxRotate3d>, <R_DRW2D_CtxScale>, <R_DRW2D_CtxTranslate>
*/
r_drw2d_Error_t R_DRW2D_CtxTextureIdentity(r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_CtxTransform

  Set 4x4 vertex transformation matrix.

  Can be used for 3D perspective mapping or affine transformations (scale, translate, rotate).

  The matrix is expected to be in column-major format and use the following element order:
  > [ 0 4  8 12 ]
  > [ 1 5  9 13 ]
  > [ 2 6 10 14 ]
  > [ 3 7 11 15 ]
  (the translation vector is stored in elements 12, 13, and 14)

  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
     Matrix - Reference to 4x4 transformation matrix. 
  
  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxIdentity>, <R_DRW2D_CtxRotate>, <R_DRW2D_CtxRotate3d>, <R_DRW2D_CtxScale>, 
    <R_DRW2D_CtxTranslate>, <R_DRW2D_CtxTextureIdentity>, <R_DRW2D_CtxTextureTransform>, 
    <R_DRW2D_CtxTextureRotate>, <R_DRW2D_CtxTextureScale>, <R_DRW2D_CtxTextureTranslate>
*/
r_drw2d_Error_t R_DRW2D_CtxTransform(r_drw2d_Device_t  Device,
                               const r_drw2d_FixedP_t *Matrix
                                     );



/*******************************************************************************
  Function: R_DRW2D_CtxTextureTransform

  Set 3x2 texture transformation matrix.

  Can be used for e.g. affine transformations (scale, translate, rotate).

  The matrix is expected to be in column-major format and use the following element order:
  > [ 0 2 4 ]
  > [ 1 3 5 ]
  (the translation vector is stored in elements 4 and 5)


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
     Matrix - Reference to 3x2 transformation matrix. 

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxTextureIdentity>, <R_DRW2D_CtxTextureRotate>, <R_DRW2D_CtxTextureScale>, 
    <R_DRW2D_CtxTextureTranslate>, <R_DRW2D_CtxIdentity>, <R_DRW2D_CtxTransform>, 
    <R_DRW2D_CtxRotate>, <R_DRW2D_CtxRotate3d>, <R_DRW2D_CtxScale>, <R_DRW2D_CtxTranslate>

*/
r_drw2d_Error_t R_DRW2D_CtxTextureTransform(r_drw2d_Device_t  Device,
                                      const r_drw2d_FixedP_t *Matrix
                                            );



/*******************************************************************************
  Function: R_DRW2D_CtxRotate

  Multiply current vertex matrix by rotation matrix.

  > [ cos(Angle), -sin(Angle), 0, 0 ]
  > [ sin(Angle),  cos(Angle), 0, 0 ]
  > [          0,           0, 1, 0 ]
  > [          0,           0, 0, 1 ]


  Parameter:
    Device - Device handle (see <r_drw2d_Device_t>)

     Angle - Angle of rotation about z axis.
             Angle is in degrees measure.
             When converting to fixed point value, we recommend using <R_DRW2D_2X> macros.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxIdentity>, <R_DRW2D_CtxTransform>, <R_DRW2D_CtxScale>, <R_DRW2D_CtxTranslate>,
    <R_DRW2D_CtxTextureIdentity>, <R_DRW2D_CtxTextureTransform>, <R_DRW2D_CtxTextureRotate>, 
    <R_DRW2D_CtxTextureScale>, <R_DRW2D_CtxTextureTranslate>
*/
r_drw2d_Error_t R_DRW2D_CtxRotate(r_drw2d_Device_t Device,
                                  r_drw2d_FixedP_t Angle
                                  );



/*******************************************************************************
  Function: R_DRW2D_CtxRotate3d

  Multiply current vertex matrix by 3d rotation matrix.

   > [ xx(1-c)+c    xy(1-c)-zs   xz(1-c)+ys   0 ]
   > [ yx(1-c)+zs   yy(1-c)+c    yz(1-c)-xs   0 ]
   > [ xz(1-c)-ys   yz(1-c)+xs   zz(1-c)+c    0 ]
   > [ 0            0            0            1 ]

  c=cos(Angle), s= sin(Angle)

  The argument (X, Y, Z) should be unit vector. 

  Parameter:
    Device - Device handle (see <r_drw2d_Device_t>)
         X - Coordinate of vector starting in origin (0,0,0)
         Y - Coordinate of vector starting in origin (0,0,0)
         Z - Coordinate of vector starting in origin (0,0,0)
     Angle - Angle of rotation around axis of vector defined by (X,Y,Z) in (0..360 degrees)
             Angle is in degrees measure.
             When converting to fixed point value, we recommend using <R_DRW2D_2X> macros.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxIdentity>, <R_DRW2D_CtxTransform>, <R_DRW2D_CtxScale>, <R_DRW2D_CtxTranslate>,
    <R_DRW2D_CtxTextureIdentity>, <R_DRW2D_CtxTextureTransform>, <R_DRW2D_CtxTextureRotate>, 
    <R_DRW2D_CtxTextureScale>, <R_DRW2D_CtxTextureTranslate>
*/
r_drw2d_Error_t R_DRW2D_CtxRotate3d(r_drw2d_Device_t Device,
                                    r_drw2d_FixedP_t X,
                                    r_drw2d_FixedP_t Y,
                                    r_drw2d_FixedP_t Z,
                                    r_drw2d_FixedP_t Angle
                                    );



/*******************************************************************************
  Function: R_DRW2D_CtxTextureRotate

  Multiply current texture matrix by rotation matrix.

  > [ cos(Angle), -sin(Angle), 0 ]
  > [ sin(Angle),  cos(Angle), 0 ]


  Parameter:
    Device - Device handle (see <r_drw2d_Device_t>)
     Angle - Angle of rotation about z axis (0..360 degrees)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxTextureIdentity>, <R_DRW2D_CtxTextureTransform>, <R_DRW2D_CtxTextureScale>,
    <R_DRW2D_CtxTextureTranslate>, <R_DRW2D_CtxIdentity>, <R_DRW2D_CtxTransform>, 
    <R_DRW2D_CtxRotate>, <R_DRW2D_CtxRotate3d>, <R_DRW2D_CtxScale>, <R_DRW2D_CtxTranslate>
*/
r_drw2d_Error_t R_DRW2D_CtxTextureRotate(r_drw2d_Device_t Device,
                                         r_drw2d_FixedP_t Angle
                                         );



/*******************************************************************************
  Function: R_DRW2D_CtxScale

  Multiply current vertex matrix by scaling matrix.

  > [ ScaleX, 0     , 0,      0 ]
  > [      0, ScaleY, 0,      0 ]
  > [      0, 0     , ScaleZ, 0 ]
  > [      0, 0     , 0,      1 ]


  Parameter:
    Device - Device handle (see <r_drw2d_Device_t>)
    ScaleX - Scale factor for the X axis (1.0 means no scaling)
    ScaleY - Scale factor for the Y axis (1.0 means no scaling)
    ScaleZ - Scale factor for the Z axis (1.0 means no scaling)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxIdentity>, <R_DRW2D_CtxTransform>, <R_DRW2D_CtxRotate>, <R_DRW2D_CtxRotate3d>, 
    <R_DRW2D_CtxTranslate>, <R_DRW2D_CtxTextureIdentity>, <R_DRW2D_CtxTextureTransform>, 
    <R_DRW2D_CtxTextureRotate>, <R_DRW2D_CtxTextureScale>, <R_DRW2D_CtxTextureTranslate>
*/
r_drw2d_Error_t R_DRW2D_CtxScale(r_drw2d_Device_t Device,
                                 r_drw2d_FixedP_t ScaleX,
                                 r_drw2d_FixedP_t ScaleY,
                                 r_drw2d_FixedP_t ScaleZ
                                 );



/*******************************************************************************
  Function: R_DRW2D_CtxTextureScale

  Multiply current texture matrix by scaling matrix.

  > [ ScaleX, 0     , 0 ]
  > [      0, ScaleY, 0 ]

  When performing the same transformation as <R_DRW2D_CtxScale>,
  texture matrix should be inverse matrix of vertex matrix.


  Parameter:
    Device - Device handle (see <r_drw2d_Device_t>)
    ScaleX - Scale factor for the X axis (1.0 means no scaling)
    ScaleY - Scale factor for the Y axis (1.0 means no scaling)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxTextureIdentity>, <R_DRW2D_CtxTextureTransform>, <R_DRW2D_CtxTextureRotate>,
    <R_DRW2D_CtxTextureTranslate>, <R_DRW2D_CtxIdentity>, <R_DRW2D_CtxTransform>, 
    <R_DRW2D_CtxRotate>, <R_DRW2D_CtxRotate3d>, <R_DRW2D_CtxScale>, <R_DRW2D_CtxTranslate>
*/
r_drw2d_Error_t R_DRW2D_CtxTextureScale(r_drw2d_Device_t Device,
                                        r_drw2d_FixedP_t ScaleX,
                                        r_drw2d_FixedP_t ScaleY
                                        );



/*******************************************************************************
  Function: R_DRW2D_CtxTranslate

  Multiply current vertex matrix by translation matrix.

  > [ 1, 0, 0, TransX ]
  > [ 0, 1, 0, TransY ]
  > [ 0, 0, 1, TransZ ]
  > [ 0, 0, 0, 1      ]


  Parameter:
    Device - Device handle (see <r_drw2d_Device_t>)
    TransX - X axis translation
    TransY - Y axis translation
    TransZ - Z axis translation

  See also:
    <R_DRW2D_CtxIdentity>, <R_DRW2D_CtxTransform>, <R_DRW2D_CtxRotate>, <R_DRW2D_CtxRotate3d>, 
    <R_DRW2D_CtxScale>, <R_DRW2D_CtxTextureIdentity>, <R_DRW2D_CtxTextureTransform>, 
    <R_DRW2D_CtxTextureRotate>, <R_DRW2D_CtxTextureScale>, <R_DRW2D_CtxTextureTranslate>
*/
r_drw2d_Error_t R_DRW2D_CtxTranslate(r_drw2d_Device_t Device,
                                     r_drw2d_FixedP_t TransX,
                                     r_drw2d_FixedP_t TransY,
                                     r_drw2d_FixedP_t TransZ
                                     );



/*******************************************************************************
  Function: R_DRW2D_CtxTextureTranslate

  Multiply current texture matrix by translation matrix.

  > [ 1, 0  TransX ]
  > [ 0, 1, TransY ]


  Parameter:
    Device - Device handle (see <r_drw2d_Device_t>)
    TransX - X axis translation
    TransY - Y axis translation

  See also:
    <R_DRW2D_CtxTextureIdentity>, <R_DRW2D_CtxTextureTransform>, <R_DRW2D_CtxTextureRotate>, 
    <R_DRW2D_CtxTextureScale>, <R_DRW2D_CtxIdentity>, <R_DRW2D_CtxTransform>, <R_DRW2D_CtxRotate>, 
    <R_DRW2D_CtxRotate3d>, <R_DRW2D_CtxScale>, <R_DRW2D_CtxTranslate>
*/
r_drw2d_Error_t R_DRW2D_CtxTextureTranslate(r_drw2d_Device_t Device,
                                            r_drw2d_FixedP_t TransX,
                                            r_drw2d_FixedP_t TransY
                                            );



/***********************************************************
  Function: R_DRW2D_CtxFrustum

  Multiply current vertex matrix by perspective matrix.

> [ (2*near)/(right-left), 0                    , (right+left)/(right-left), 0]
> [ 0                    , (2*near)/(top-bottom), (top+bottom)/(top-bottom), 0]
> [ 0                    , 0                    , -((far+near)/(far-near)) , -((2*far*near) / (far-near))]
> [ 0                    , 0                    , -1                       , 0]


  Parameter:
    Device - Device handle (see <r_drw2d_Device_t>)
      Left - Left vertical clipping plane
     Right - Right vertical clipping plane
    Bottom - Bottom horizontal clipping plane
       Top - Top horizontal clipping plane
     ZNear - Distance to near clipping plane
      ZFar - Distance to far clipping plane
*/
r_drw2d_Error_t R_DRW2D_CtxFrustum(r_drw2d_Device_t Device,
                                   r_drw2d_FixedP_t Left,
                                   r_drw2d_FixedP_t Right,
                                   r_drw2d_FixedP_t Bottom,
                                   r_drw2d_FixedP_t Top,
                                   r_drw2d_FixedP_t ZNear,
                                   r_drw2d_FixedP_t ZFar
                                   );



/***********************************************************
  Function: R_DRW2D_VtxTransform

  Transform a list of vertices by the current vertex transformation matrix.

  This function applies the current transformation matrix to an arbitrary number of
  points supplied via the parameters Vertices and NumVertices.
  
  The calculation result is written back to instance of Vertices.

  This function can be used to get the scope of the next drawing operation before executing it.
  This information can be used to allocate buffers with as small as possible size,
  as the target scope of the drawing operation is already known.


  Parameter:
         Device - Device handle (see <r_drw2d_Device_t>)
       Vertices - Pointer to vertices
    NumVertices - Number of vertices provided by "Vertices" parameter
*/
r_drw2d_Error_t R_DRW2D_VtxTransform(r_drw2d_Device_t  Device,
                                     r_drw2d_Vec4_t   *Vertices,
                                     uint32_t          NumVertices
                                     );



/*******************************************************************************
  Function: R_DRW2D_CtxMatrix

  Get the 4x4 vertex transformation matrix and the 3x2 texture transformation matrix.

  Can be used to save and restore (<R_DRW2D_CtxTransform>, <R_DRW2D_CtxTextureTransform>)
  the current transformation matrices.
  This may reduce the CPU overhead of repeatedly calling almost identical transformation
  operations for several similar drawing operations.
  If just one of the matrices is required, set the other pointer to zero.

  VertexMatrix is 4x4 transformation matrix. 
  The matrix is expected to be in column-major format and use the following element order:  
   > [ 0 4  8 12 ]
   > [ 1 5  9 13 ]
   > [ 2 6 10 14 ]
   > [ 3 7 11 15 ]
  (the translation vector is stored in elements 12, 13, and 14)

  TextureMatrix is 3x2 transformation matrix. 
  The matrix is expected to be in column-major format and use the following element order:
   > [ 0 2 4 ]
   > [ 1 3 5 ]
  (the translation vector is stored in elements 4 and 5)
  
  Parameter:
            Device - Device handle (see <r_drw2d_Device_t>)
      VertexMatrix - Reference to 4x4 transformation matrix. 
     TextureMatrix - Reference to 3x2 transformation matrix. 

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxIdentity>, <R_DRW2D_CtxRotate>, <R_DRW2D_CtxRotate3d>, <R_DRW2D_CtxScale>, 
    <R_DRW2D_CtxTranslate>, <R_DRW2D_CtxTextureIdentity>, <R_DRW2D_CtxTextureTransform>, 
    <R_DRW2D_CtxTextureRotate>, <R_DRW2D_CtxTextureScale>, <R_DRW2D_CtxTextureTranslate>
*/
r_drw2d_Error_t R_DRW2D_CtxMatrix(r_drw2d_Device_t        Device,
                                  r_drw2d_FixedP_t *const VertexMatrix,
                                  r_drw2d_FixedP_t *const TextureMatrix
                                  );



/***********************************************************
  Function: R_DRW2D_ClutAlloc

  Allocates space for a CLUT used by <R_DRW2D_ClutSet> and <R_DRW2D_CtxClutSet>
  Call this function first when using CLUT.
  CLUT memory can store 512 elements in total.
  Control it so that it does not become fragmented.

  Parameter:
    Device   - Device handle (see <r_drw2d_Device_t>)
    Size     - Size of CLUT entries to allocate.
               Specify the Size according to the pixel format to use.
               When using R_DRW2D_PIXELFORMAT_CLUT_1, specify "2" for Size.
               When using R_DRW2D_PIXELFORMAT_CLUT_2, specify "4" for Size.
               When using R_DRW2D_PIXELFORMAT_CLUT_4, specify "16" for Size.
               When using R_DRW2D_PIXELFORMAT_CLUT_8, specify "256" for Size.
    ClutBase - This function will write the offset address of CLUT memory to store specified size.
               Clutbase is used as a handle value for subsequent CLUT APIs.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
*/
r_drw2d_Error_t R_DRW2D_ClutAlloc(r_drw2d_Device_t  Device,
                                  uint32_t          Size,
                                  uint32_t         *ClutBase
                                  );



/***********************************************************
  Function: R_DRW2D_ClutFree

  Frees CLUT memory previously allocated with <R_DRW2D_ClutAlloc>


  Parameter:
    Device   - Device handle (see <r_drw2d_Device_t>)
    Size     - Number of CLUT entries to free
    ClutBase - Offset address of CLUT memory.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
*/
r_drw2d_Error_t R_DRW2D_ClutFree(r_drw2d_Device_t Device,
                                 uint32_t         Size,
                                 uint32_t         ClutBase
                                 );



/***********************************************************
  Function: R_DRW2D_CtxClutSet

  Assign a previously created CLUT with the Offset ClutBase (as returned by <R_DRW2D_ClutAlloc>)
  to the texture.
  
  Before calling this function, call <R_DRW2D_ClutAlloc> and acquire ClutBase.


  Parameter:
       Device - Device handle (see <r_drw2d_Device_t>)
     ClutBase - Offset address of CLUT memory.
*/
r_drw2d_Error_t R_DRW2D_CtxClutSet(r_drw2d_Device_t Device,
                                   uint32_t         ClutBase
                                   );



/***********************************************************
  Function: R_DRW2D_ClutSet

  Create and set a CLUT that was previously allocated with <R_DRW2D_ClutAlloc>
  
  Before calling this function, call <R_DRW2D_ClutAlloc> and acquire ClutBase.
  
  The relationship between pixel format and "Data" and "Size" are as follows.
  * R_DRW2D_PIXELFORMAT_CLUT_1 - "Data" will be 2 colors of data. Specify 2 for "Size".
  * R_DRW2D_PIXELFORMAT_CLUT_2 - "Data" will be 4 colors of data. Specify 4 for "Size".
  * R_DRW2D_PIXELFORMAT_CLUT_4 - "Data" will be 16 colors of data. Specify 16 for "Size".
  * R_DRW2D_PIXELFORMAT_CLUT_8 - "Data" will be 256 colors of data. Specify 256 for "Size".


  Parameter:
    Device   - Device handle (see <r_drw2d_Device_t>)
    Data     - Pointer to the array of CLUT data. The data consists of 32 bit ARGB color data.
    Start    - Offset address of CLUT memory.
               Specify the ClutBase acquired with <R_DRW2D_ClutAlloc>.
    Size     - Size of CLUT elements.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
*/
r_drw2d_Error_t R_DRW2D_ClutSet(r_drw2d_Device_t  Device,
                                uint32_t         *Data,
                                uint32_t          Start,
                                uint32_t          Size
                                );





/*******************************************************************************
  Group: Framebuffer functions
*/

/*******************************************************************************
  Function: R_DRW2D_FramebufferSet

  Set current destination framebuffer.
  Allocation of the framebuffer is the responsibility of the application.

  Parameter:
         Device - Device handle (see <r_drw2d_Device_t>)
    Framebuffer - Reference to framebuffer structure (see <r_drw2d_Framebuffer_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <r_drw2d_Framebuffer_t>, <r_drw2d_Buffer_t>

*/
r_drw2d_Error_t R_DRW2D_FramebufferSet(r_drw2d_Device_t       Device,
                                       r_drw2d_Framebuffer_t *Framebuffer
                                       );



/*******************************************************************************
  Function: R_DRW2D_FramebufferClear

  Clears the current clip rectangle with the current background color 
  (can be set with <R_DRW2D_CtxBgColor>)

  <R_DRW2D_FramebufferClear> will always use the SOLID fill mode and ignore current 
  blend mode/factor settings.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <r_drw2d_Framebuffer_t>, <r_drw2d_Buffer_t>, <R_DRW2D_FramebufferSet>
*/
r_drw2d_Error_t R_DRW2D_FramebufferClear(r_drw2d_Device_t Device);





/*******************************************************************************
  Group: Render functions

  All following render functions take the current context settings (fill mode, fg/bg colors, 
  texture, ..) into account.
*/

/*******************************************************************************
  Function: R_DRW2D_DrawTriangles

  Render an array of triangles.

  Each triangle vertex will be transformed by the current vertex transformation matrix.

  If texture mapping is enabled, the texture will be applied according to the current texture 
  transformation matrix.

  Note that the Dave HD platform is limited to an edge width/height of max. 2048 when
  using non antialiased edges (Edge flag set to 0).


  Parameter:
       Device - Device handle (see <r_drw2d_Device_t>)
       Points - Pointer to array of vertices (see <r_drw2d_Point_t>).
                The array should consist of Count elements.
        Count - Number of vertices. (Count/3) triangles will be drawn.
    EdgeFlags - Pointer to array of edge flag. The array should consist of (Count/3) elements.
                One byte per triangle. Specifies which triangles edge will be antialiased. 
                can be combined with OR setting. See <r_drw2d_EdgeFlag_t>.
                If NULL, do not use antialiasing. 

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawPolyline>, <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>, 
    <R_DRW2D_DrawRect>, <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_DrawTriangles(r_drw2d_Device_t  Device,
                                const r_drw2d_Point_t  *Points,
                                      uint32_t          Count,
                                const uint8_t          *EdgeFlags
                                      );



/*******************************************************************************
  Function: R_DRW2D_DrawTrianglesUV

  Render an array of UV texture mapped triangles.

  Each triangle vertex will be transformed by the current vertex transformation matrix.

  Each UV coordinate will be transformed by the current texture transformation matrix 
  if the texture transform mode (<R_DRW2D_CtxTextureTransformMode>) is set to 
  <R_DRW2D_TEX_TRANSFORM_2D>.

  If the fill mode is set to <R_DRW2D_FILLMODE_SOLID>, this function behaves like 
  <R_DRW2D_DrawTriangles> and the UV coordinate array is ignored.

  Note that the Dave HD platform is limited to an edge width/height of max. 2048 when
  using non antialiased edges (Edge flag set to 0).


  Parameter:
       Device - Device handle (see <r_drw2d_Device_t>)
       Points - Pointer to array of vertices (see <r_drw2d_Point_t>).
                The array should consist of Count elements.
        Count - Number of vertices. (Count/3) triangles will be drawn.
    EdgeFlags - Pointer to array of edge flag. The array should consist of (Count/3) elements.
                One byte per triangle. Specifies which triangles edge will be antialiased. 
                Can be combined with OR setting. See <r_drw2d_EdgeFlag_t>.
                If NULL, do not use antialiasing. 
     UVCoords - Pointer to array of UV coordinates (see <r_drw2d_UVCoord_t>).
                The array should consist of Count elements.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawPolyline>, <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>, 
    <R_DRW2D_DrawRect>, <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_DrawTrianglesUV(r_drw2d_Device_t   Device,
                                  const r_drw2d_Point_t   *Points,
                                        uint32_t           Count,
                                  const uint8_t           *EdgeFlags,
                                  const r_drw2d_UVCoord_t *UVCoords
                                        );



/*******************************************************************************
  Function: R_DRW2D_DrawRect

  Render a rectangle.

  The *Pos* and *Size* fields of the Rect argument are used to construct a quad which will
  then be transformed by the current vertex matrix.

  If texture mapping is enabled, the texture will be applied according to the current texture 
  transformation matrix.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
       Rect - Rectangle position and size.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_DrawTriangles>, <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawPolyline>, 
    <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>, <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_DrawRect(r_drw2d_Device_t  Device,
                           const r_drw2d_Rect_t   *Rect
                                 );



/*******************************************************************************
  Function: R_DRW2D_DrawRectUV

  Render a UV texture mapped rectangle.

  The *Pos* and *Size* fields of the Rect argument are used to construct a quad which will
  then be transformed by the current vertex matrix.

  Each UV coordinate will be transformed by the current texture transformation matrix if the 
  texture transform mode (<R_DRW2D_CtxTextureTransformMode>) is set to <R_DRW2D_TEX_TRANSFORM_2D>.

  If the fill mode is set to <R_DRW2D_FILLMODE_SOLID>, this function behaves like 
  <R_DRW2D_DrawRect> and the UV coordinate array is ignored.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
       Rect - Rectangle position and size.
   UVCoords - Reference to an array of 4 UV coordinates (left/top, right/top, right/bottom, 
              left/bottom order) (see <r_drw2d_UVCoord_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_DrawTriangles>, <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawPolyline>, 
    <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>, <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_DrawRectUV(r_drw2d_Device_t   Device,
                             const r_drw2d_Rect_t    *Rect,
                             const r_drw2d_UVCoord_t *UVCoords
                                   );



/*******************************************************************************
  Function: R_DRW2D_DrawQuads

  Renders an array of quadrilaterals.

  Each vertex will be transformed by the current vertex matrix.

  If texture mapping is enabled, the texture will be applied according to the current texture 
  transformation matrix.

  Note that the Dave HD platform is limited to an edge width/height of max. 2048 when
  using non antialiased edges (Edge flag set to 0).


  Parameter:
       Device - Device handle (see <r_drw2d_Device_t>)
       Points - Pointer to array of vertices (see <r_drw2d_Point_t>).
                The array should consist of Count elements.
        Count - Number of vertices. (Count/4) quadrilaterals will be drawn.
    EdgeFlags - Pointer to array of edge flag. The array should consist of (Count/4) elements.
                One byte per quadrilaterals. Specifies which quadrilaterals edge will be 
                antialiased. Can be combined with OR setting. See <r_drw2d_EdgeFlag_t>.
                If NULL, do not use antialiasing. 

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_DrawQuadsUV>, <R_DRW2D_DrawTriangles>, <R_DRW2D_DrawTrianglesUV>, 
    <R_DRW2D_DrawPolyline>, <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>, <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_DrawQuads(r_drw2d_Device_t  Device,
                            const r_drw2d_Point_t  *Points,
                                  uint32_t          Count,
                            const uint8_t          *EdgeFlags
                                  );



/*******************************************************************************
  Function: R_DRW2D_DrawQuadsUV

  Renders an array of UV texture mapped quadrilaterals.

  Each vertex will be transformed by the current vertex matrix.

  Each UV coordinate will be transformed by the current texture transformation matrix if the texture  transform mode (<R_DRW2D_CtxTextureTransformMode>) is set to <R_DRW2D_TEX_TRANSFORM_2D>.

  If the fill mode is set to <R_DRW2D_FILLMODE_SOLID>, this function behaves like 
  <R_DRW2D_DrawQuads> and the UV coordinate array is ignored.

  Note that the Dave HD platform is limited to an edge width/height of max. 2048 when
  using non antialiased edges (Edge flag set to 0).


  Parameter:
       Device - Device handle (see <r_drw2d_Device_t>)
       Points - Pointer to array of vertices (see <r_drw2d_Point_t>)
                The array should consist of Count elements.
        Count - Number of vertices. (Count/4) quadrilaterals will be drawn.
    EdgeFlags - Pointer to array of edge flag. The array should consist of (Count/4) elements.
                One byte per quadrilaterals. Specifies which quadrilaterals edge will be 
                antialiased. Can be combined with OR setting. See <r_drw2d_EdgeFlag_t>.
                If NULL, do not use antialiasing. 
     UVCoords - Pointer to array of UV coordinates (see <r_drw2d_UVCoord_t>).
                The array should consist of Count elements.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_DrawQuads>, <R_DRW2D_DrawTriangles>, <R_DRW2D_DrawTrianglesUV>, 
    <R_DRW2D_DrawPolyline>, <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>, <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_DrawQuadsUV(r_drw2d_Device_t   Device,
                              const r_drw2d_Point_t   *Points,
                                    uint32_t           Count,
                                    uint8_t           *EdgeFlags,
                              const r_drw2d_UVCoord_t *UVCoords
                                    );



/*******************************************************************************
  Function: R_DRW2D_DrawQuads3dUV

  Renders an array of UV texture mapped 3D-quadrilaterals.

  Each vertex will be transformed by the current vertex matrix.

  Each UV coordinate will be transformed by the current texture transformation matrix if the 
  texture transform mode (<R_DRW2D_CtxTextureTransformMode>) is set to <R_DRW2D_TEX_TRANSFORM_2D>.

  If the fill mode is set to <R_DRW2D_FILLMODE_SOLID>, this function behaves like 
  <R_DRW2D_DrawQuads> and the UV coordinate array is ignored.

  Note that the Dave HD platform is limited to an edge width/height of max. 2048 when
  using non antialiased edges (Edge flag set to 0).


  Parameter:
       Device - Device handle (see <r_drw2d_Device_t>)
       Points - Pointer to array of 3D vertices (see <r_drw2d_Vec4_t>).
                The array should consist of Count elements.
        Count - Number of vertices. (Count/4) quadrilaterals will be drawn.
    EdgeFlags - Pointer to array of edge flag. The array should consist of (Count/4) elements.
                One byte per quadrilaterals. Specifies which quadrilaterals edge will be 
                antialiased. Can be combined with OR setting. See <r_drw2d_EdgeFlag_t>.
                If NULL, do not use antialiasing. 
     UVCoords - Pointer to array of UV coordinates (see <r_drw2d_UVCoord_t>).
                The array should consist of Count elements.
                

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_DrawQuads>, <R_DRW2D_DrawQuadsUV>, <R_DRW2D_DrawTriangles>, <R_DRW2D_DrawTrianglesUV>,
    <R_DRW2D_DrawPolyline>, <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>, <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_DrawQuads3dUV(r_drw2d_Device_t   Device,
                                const r_drw2d_Vec4_t    *Points,
                                      uint32_t           Count,
                                      uint8_t           *EdgeFlags,
                                const r_drw2d_UVCoord_t *UVCoords
                                      );



/*******************************************************************************
  Function: R_DRW2D_DrawEllipse

  Render an ellipse at Point with the specified x and y radius.

  The center point will be transformed by the current vertex matrix.

  In order to draw a circle, use the same values for RadiusX and RadiusY.

  If texture mapping is enabled, the texture will be applied according to the current texture 
  transformation matrix.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
      Point - Center point (see <r_drw2d_Point_t>)
    RadiusX - Horizontal radius
    RadiusY - Vertical radius

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_DrawTriangles>, <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawPolyline>, 
    <R_DRW2D_DrawLines>, <R_DRW2D_DrawRect>, <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_DrawEllipse(r_drw2d_Device_t Device,
                                    r_drw2d_Point_t  Point,
                                    r_drw2d_FixedP_t RadiusX,
                                    r_drw2d_FixedP_t RadiusY
                                    );



/*******************************************************************************
  Function: R_DRW2D_DrawLines

  Render an array of lines. The rendered lines include the start and end points.

  Each line point will be transformed by the current transformation matrix.

  The *Width* field of <r_drw2d_LineStyle_t> specifies the line width.

  The *LineCap* field of <r_drw2d_LineStyle_t> specifies how to render the line starts/ends 
  (see <r_drw2d_LineCap_t>).

  If texture mapping is enabled, the texture will be applied according to the current texture 
  transformation matrix.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
     Points - Pointer to array of Line start/end vertices (see r_drw2d_Point_t).
              The array should consist of Count elements.
              The order is (start of the 1st Line), (end of the 1st Line), 
              (start of the 2nd Line), (end of the 2nd Line), ...
      Count - Number of vertices. (Count/2) lines will be drawn.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_DrawTriangles>, <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawPolyline>, 
    <R_DRW2D_DrawEllipse>, <R_DRW2D_DrawRect>, <R_DRW2D_FramebufferClear>

*/
r_drw2d_Error_t R_DRW2D_DrawLines(r_drw2d_Device_t Device,
                            const r_drw2d_Point_t *Points,
                                  uint32_t         Count
                                  );



/*******************************************************************************
  Function: R_DRW2D_DrawPolyline

  Render a polyline consisting of one or many line segments.

  Each line point will be transformed by the current transformation matrix.

  The *Width* field of <r_drw2d_LineStyle_t> specifies the line width.

  If the *IsClosed* field of <r_drw2d_LineStyle_t> is set to 1 (true), the last segment will be 
  connected to the first segment.

  The line segments will be connected as specified by the *LineJoin* field of <r_drw2d_LineStyle_t>
  (see <R_DRW2D_CtxLineStyle>, <r_drw2d_LineStyle_t>, <r_drw2d_LineJoin_t>).

  If the <R_DRW2D_LINEJOIN_MITER> join type is selected, the *MiterLimit* field of 
  <r_drw2d_LineStyle_t> specifies the maximum distance between the line join tip and line point.
  If the miter limit is exceeded, a bevel joint will be drawn at the miter limit position.

  If the polyline is not closed, the *LineCap* field of <r_drw2d_LineStyle_t> specifies how to 
  render the polyline start/end (see <r_drw2d_LineCap_t>).

  If texture mapping is enabled, the texture will be applied according to the current texture 
  transformation matrix.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
     Points - Pointer to array of Line start/end vertices (see r_drw2d_Point_t).
              The array should consist of Count elements.
      Count - Number of vertices. (Count-1) or (Count) lines will be drawn depending on 
              IsClosed setting.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_DrawTriangles>, <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>,
    <R_DRW2D_DrawRect>, <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_DrawPolyline(r_drw2d_Device_t  Device,
                               const r_drw2d_Point_t  *Points,
                                     uint32_t          Count
                                     );



/*******************************************************************************
  Function: R_DRW2D_DrawBezierCurves

  Render a bezier curve consisting of one or more quadratic bezier segments.

  Each curve point will be transformed by the current transformation matrix.

  The *Width* field of <r_drw2d_LineStyle_t> specifies the line width. 
  The value must be less than 16.

  If the *IsClosed* field of <r_drw2d_LineStyle_t> is set to 1 (true), the last segment will be 
  connected to the first segment by a straight line.

  The bezier segments will be connected by round joints. The ends of the curve will be flat.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
     Points - Pointer to array of curve point vertices (see r_drw2d_Point_t).
              The array should consist of Count elements.
      Count - Number of vertices. ((Count - 1) / 2) curves will be drawn

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_DrawTriangles>, <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>,
    <R_DRW2D_DrawRect>, <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_DrawBezierCurves(r_drw2d_Device_t  Device,
                                   const r_drw2d_Point_t  *Points,
                                         uint32_t          Count
                                         );





/*******************************************************************************
  Group: Convolution filter functions

  The convolution filter render functions take the following context settings into account:
    - source texture (see <R_DRW2D_CtxTextureSet>)
    - destination framebuffer (see <R_DRW2D_FramebufferSet>)
    - convolution filter kernel (see <R_DRW2D_CtxConvolutionKernel>, 
      <R_DRW2D_CtxConvolutionKernelPreset1d>, <R_DRW2D_CtxConvolutionKernelPreset2d>)
    - clipping rectangle (see <R_DRW2D_CtxClipRect>)
*/

/*******************************************************************************
  Function: R_DRW2D_DrawRectConvolve1dx

  Apply one dimensional convolution filter to texture and store result in framebuffer.

  The currently selected 1D convolution kernel is applied in the horizontal direction, 
  i.e. the kernel coefficients determine the weights of the pixel(s) to the left and right of the 
  current texel while the source texture is being traversed.

  The *Pos* field of the Rect argument is used to select the framebuffer destination position.

  The *Size* field of the Rect argument is used to select the width and height of the convoluted 
  area.

  The *TextureOffX* and *TextureOffY* arguments are used to select the left/top texel of the 
  convolution source area.

  Unless the R_DRW2D_TEX_WRAPU flag is used, the texture width must be equal or greater than the 
  kernel size.

  This function does not regard the vertex and texture transformation matrices.

  The fill mode must be set to <R_DRW2D_FILLMODE_TEXTURE>.

  Note that using textures with an improper pitch or height on D/AVE HD will lead to 
  *severe performance loss*.
  The texture's pitch should be a multiple of 256 / BPP, where BPP is the current texture format's 
  number of bits per pixel. The texture's height should be a multiple of 4. Furthermore, the 
  texture's start address should be 8 byte aligned.


  Parameter:
         Device - Device handle (see <r_drw2d_Device_t>)
           Rect - Rectangle position and size (see <r_drw2d_IntRect_t>)
    TextureOffX - Horizontal texture offset (number of texels)
    TextureOffY - Vertical texture offset (number of texels)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_DrawRectConvolve1dy>, <R_DRW2D_DrawRectConvolve2d>, 
    <R_DRW2D_CtxConvolutionKernelPreset1d>, <R_DRW2D_CtxConvolutionKernelPreset2d>, 
    <R_DRW2D_DrawTriangles>, <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawPolyline>, 
    <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>, <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_DrawRectConvolve1dx(r_drw2d_Device_t   Device,
                                      const r_drw2d_IntRect_t *Rect,
                                            uint16_t           TextureOffX,
                                            uint16_t           TextureOffY
                                            );



/*******************************************************************************
  Function: R_DRW2D_DrawRectConvolve1dy

  Apply one dimensional convolution filter to texture and store result in framebuffer.

  The currently selected 1D convolution kernel is applied in the vertical direction, 
  i.e. the kernel coefficients determine the weights of the pixel(s) above and below the current 
  texel while the source texture is being traversed.

  The *Pos* field of the Rect argument is used to select the framebuffer destination position.

  The *Size* field of the Rect argument is used to select the width and height of the convoluted 
  area.

  The *TextureOffX* and *TextureOffY* arguments are used to select the left/top texel of the 
  convolution source area.

  Unless the <R_DRW2D_TEX_WRAPV> flag is used, the texture height must be equal or greater than 
  the kernel size.

  This function does not regard the vertex and texture transformation matrices.

  The fill mode must be set to <R_DRW2D_FILLMODE_TEXTURE>.

  Note that using textures with an improper pitch or height on D/AVE HD will lead to 
  *severe performance loss*. The texture's pitch should be a multiple of 256 / BPP, where BPP is 
  the current texture format's number of bits per pixel. The texture's height should be a 
  multiple of 4. Furthermore, the texture's start address should be 8 byte aligned.


  Parameter:
         Device - Device handle (see <r_drw2d_Device_t>)
           Rect - Rectangle position and size (see <r_drw2d_IntRect_t>)
    TextureOffX - Horizontal texture offset (number of texels)
    TextureOffY - Vertical texture offset (number of texels)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_DrawRectConvolve1dx>, <R_DRW2D_CtxConvolutionKernelPreset1d>, 
    <R_DRW2D_DrawRectConvolve2d>, <R_DRW2D_CtxConvolutionKernelPreset2d>, 
    <R_DRW2D_DrawTriangles>, <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawPolyline>, 
    <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>, <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_DrawRectConvolve1dy(r_drw2d_Device_t   Device,
                                      const r_drw2d_IntRect_t *Rect,
                                            uint16_t           TextureOffX,
                                            uint16_t           TextureOffY
                                            );



/*******************************************************************************
  Function: R_DRW2D_DrawRectConvolve2d

  Apply two dimensional convolution filter to texture and store result in framebuffer.

  The currently selected 2D convolution kernel is applied in both x and y directions, i.e. the 
  kernel coefficients determine the weights of the pixel(s) above and below resp. to the left and 
  right of the current texel while the source texture is being traversed.

  The *Pos* field of the Rect argument is used to select the framebuffer destination position.

  The *Size* field of the Rect argument is used to select the width and height of the convoluted 
  area.

  The *TextureOffX* and *TextureOffY* arguments are used to select the left/top texel of the 
  convolution source area.

  Unless the <R_DRW2D_TEX_WRAPU> flag is used, the texture width must be equal or greater than 
  the kernel width.

  Unless the <R_DRW2D_TEX_WRAPV> flag is used, the texture height must be equal or greater than 
  the kernel height.

  This function does not regard the vertex and texture transformation matrices.

  If the selected convolution kernel is separable, i.e. if the 2d kernel matrix can be expressed 
  as a product of a row and column vector (e.g. Gaussian blur), applications should convolute the 
  texture in two passes using <R_DRW2D_DrawRectConvolve1dx> and <R_DRW2D_DrawRectConvolve1dy>. 
  Doing so will decrease computational complexity from O(n^2) to O(n). The drawback of this 
  approach is that a temporary buffer is required to store the result of the first pass.

  Due to HW restrictions, not all separable kernels can be implemented using aforementioned 
  two-pass rendering technique.
  In particular, the D/AVE HD can not process separable kernels that contain negative 
  weights (e.g. Sobel edge detect), since the result pixels are clamped to the 0..255 range after 
  the first pass.

  The fill mode must be set to <R_DRW2D_FILLMODE_TEXTURE>.

  Note that using textures with an improper pitch or height on D/AVE HD will lead to 
  *severe performance loss*.
  The texture's pitch should be a multiple of 256 / BPP, where BPP is the current texture format's 
  number of bits per pixel. The texture's height should be a multiple of 4. Furthermore, the 
  texture's start address should be 8 byte aligned.


  Parameter:
         Device - Device handle (see <r_drw2d_Device_t>)
           Rect - Rectangle position and size (see <r_drw2d_IntRect_t>)
    TextureOffX - Horizontal texture offset (number of texels)
    TextureOffY - Vertical texture offset (number of texels)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxConvolutionKernelPreset2d>, <R_DRW2D_DrawRectConvolve1dx>, 
    <R_DRW2D_DrawRectConvolve1dy>, <R_DRW2D_CtxConvolutionKernelPreset1d>, 
    <R_DRW2D_DrawTriangles>, <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawPolyline>, 
    <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>, <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_DrawRectConvolve2d(r_drw2d_Device_t   Device,
                                     const r_drw2d_IntRect_t *Rect,
                                           uint16_t           TextureOffX,
                                           uint16_t           TextureOffY
                                           );



/*******************************************************************************
  Function: R_DRW2D_DrawRectConvolve

  Apply two dimensional convolution filter to texture and store result in framebuffer.

  The currently selected 2D convolution kernel is applied in both x and y directions, i.e. the 
  kernel coefficients determine the weights of the pixel(s) above and below resp. to the left and 
  right of the current texel while the source texture is being traversed.

  The *Pos* field of the Rect argument is used to select the framebuffer destination position.

  The *Size* field of the Rect argument is used to select the width and height of the convoluted 
  area.

  The *TextureOffX* and *TextureOffY* arguments are used to select the left/top texel of the 
  convolution source area.

  Unless the <R_DRW2D_TEX_WRAPU> flag is used, the texture width must be equal or greater than 
  the kernel width.

  Unless the <R_DRW2D_TEX_WRAPV> flag is used, the texture height must be equal or greater than 
  the kernel height.

  This function does not regard the vertex and texture transformation matrices.

  If the selected convolution kernel is separable, i.e. if the 2d kernel matrix can be expressed 
  as a product of a row and column vector (e.g. Gaussian blur), applications should convolute the 
  texture in two passes using <R_DRW2D_DrawRectConvolve1dx> and <R_DRW2D_DrawRectConvolve1dy>. 
  Doing so will decrease computational complexity from O(n^2) to O(n). The drawback of this 
  approach is that a temporary buffer is required to store the result of the first pass.

  Due to HW restrictions, not all separable kernels can be implemented using aforementioned 
  two-pass rendering technique.
  In particular, the D/AVE HD can not process separable kernels that contain negative 
  weights (e.g. Sobel edge detect), since the result pixels are clamped to the 0..255 range after 
  the first pass.

  The fill mode must be set to <R_DRW2D_FILLMODE_TEXTURE>.

  Note that using textures with an improper pitch or height on D/AVE HD will lead to 
  *severe performance loss*.
  The texture's pitch should be a multiple of 256 / BPP, where BPP is the current texture format's 
  number of bits per pixel. The texture's height should be a multiple of 4. Furthermore, the 
  texture's start address should be 8 byte aligned.


  Parameter:
         Device - Device handle (see <r_drw2d_Device_t>)
           Rect - Rectangle position and size (see <r_drw2d_IntRect_t>)
    TextureOffX - Horizontal texture offset (number of texels)
    TextureOffY - Vertical texture offset (number of texels)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxConvolutionKernelPreset2d>, <R_DRW2D_DrawRectConvolve1dx>, 
    <R_DRW2D_DrawRectConvolve1dy>, <R_DRW2D_CtxConvolutionKernelPreset1d>, 
    <R_DRW2D_DrawTriangles>, <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawPolyline>, 
    <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>, <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_DrawRectConvolve(r_drw2d_Device_t   Device,
                                   const r_drw2d_IntRect_t *Rect,
                                         uint16_t           TextureOffX,
                                         uint16_t           TextureOffY
                                         );



/*******************************************************************************
  Function: R_DRW2D_CtxConvolutionKernelPreset1d

  Select 1d convolution kernel size and weights.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
     Preset - Kernel preset (see <r_drw2d_ConvolutionKernelPreset1d_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxConvolutionKernelPreset2d>, <R_DRW2D_DrawRectConvolve1dx>, 
    <R_DRW2D_DrawRectConvolve1dy>, <R_DRW2D_DrawRectConvolve2d>, <R_DRW2D_DrawTriangles>, 
    <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawPolyline>, <R_DRW2D_DrawLines>, 
    <R_DRW2D_DrawEllipse>, <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_CtxConvolutionKernelPreset1d(r_drw2d_Device_t                    Device,
                                                     r_drw2d_ConvolutionKernelPreset1d_t Preset
                                                     );



/*******************************************************************************
  Function: R_DRW2D_CtxConvolutionKernelPreset2d

  Select 2d convolution kernel size and weights.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
     Preset - Kernel preset (see <r_drw2d_ConvolutionKernelPreset2d_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxConvolutionKernelPreset1d>, <R_DRW2D_DrawRectConvolve1dx>, 
    <R_DRW2D_DrawRectConvolve1dy>, <R_DRW2D_DrawRectConvolve2d>, <R_DRW2D_DrawTriangles>, 
    <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawPolyline>, <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>, 
    <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_CtxConvolutionKernelPreset2d(r_drw2d_Device_t                    Device,
                                                     r_drw2d_ConvolutionKernelPreset2d_t Preset
                                                     );



/*******************************************************************************
  Function: R_DRW2D_GetGaussKernel

  Computes a gauss gernel with the given size and sigma.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
     Kernel - Pointer to an array that has a size of (Width*Height)
     Width  - Width of the kernel (needs to be an odd value!).
     Height - Height of the kernel (needs to be an odd value!).
     Sigma  - The sigma used to compute the gauss coefficients.

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxConvolutionKernel>, <R_DRW2D_DrawRectConvolve1dx>, <R_DRW2D_DrawRectConvolve1dy>,
    <R_DRW2D_DrawRectConvolve2d>, <R_DRW2D_DrawRectConvolve>
*/
r_drw2d_Error_t R_DRW2D_GetGaussKernel(r_drw2d_Device_t  Device,
                                       r_drw2d_FixedP_t *Kernel,
                                       int32_t           Width,
                                       int32_t           Height,
                                       r_drw2d_FixedP_t  Sigma
                                       );



/*******************************************************************************
  Function: R_DRW2D_CtxConvolutionKernel

  Select 2d convolution kernel size and weights.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
     Kernel - Convolution kernel (see <r_drw2d_ConvKernel_t>).
              Kernel width and height needs to be an odd value

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxConvolutionKernelPreset1d>, <R_DRW2D_DrawRectConvolve1dx>, 
    <R_DRW2D_DrawRectConvolve1dy>, <R_DRW2D_DrawRectConvolve2d>, <R_DRW2D_DrawTriangles>, 
    <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawPolyline>, <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>, 
    <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_CtxConvolutionKernel(r_drw2d_Device_t      Device,
                                       const r_drw2d_ConvKernel_t *Kernel
                                             );



/*******************************************************************************
  Function: R_DRW2D_CtxConvolutionMode

  Sets the convolution mode.

  Restriction: 
  If <R_DRW2D_CONVMODE_BLEEDING> is selected, the convolution filter will output incorrect pixels 
  infrequently.

  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
     Mode   - Convolution mode (see <r_drw2d_ConvMode_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_CtxConvolutionKernelPreset1d>, <R_DRW2D_DrawRectConvolve1dx>, 
    <R_DRW2D_DrawRectConvolve1dy>, <R_DRW2D_DrawRectConvolve2d>, <R_DRW2D_DrawTriangles>, 
    <R_DRW2D_DrawTrianglesUV>, <R_DRW2D_DrawPolyline>, <R_DRW2D_DrawLines>, <R_DRW2D_DrawEllipse>, 
    <R_DRW2D_FramebufferClear>
*/
r_drw2d_Error_t R_DRW2D_CtxConvolutionMode(r_drw2d_Device_t   Device,
                                           r_drw2d_ConvMode_t Mode
                                           );





/*******************************************************************************
  Group: Display list control functions
*/

/*******************************************************************************
  Function: R_DRW2D_GpuFinish

  Tell the driver to explicitly trigger the finishing of the current drawing scene operation 
  (display list execution).

  This function can block. i.e. wait for all commands in the GPU to be processed if the Block 
  parameter is <R_DRW2D_FINISH_WAIT> or return immediately if the parameter is 
  <R_DRW2D_FINISH_NOWAIT>. 
  
  Alternatively, a non-blocking approach can be used: Call once <R_DRW2D_FINISH_NOWAIT_MARK> 
  and later query with <R_DRW2D_GpuFinished>.
  
  Use of <R_DRW2D_FINISH_NOWAIT_MARK> and confirmation of RetFinshed = R_TRUE 
  by <R_DRW2D_GpuFinished> are one to one relationship. If it doesn't confirm completion for 
  the previous <R_DRW2D_FINISH_NOWAIT_MARK>, calling this function with 
  <R_DRW2D_FINISH_NOWAIT_MARK> has no effect.

 Parameter:
    Device - Device handle (see <r_drw2d_Device_t>)
     Block - Specify to wait for execution or not (see <r_drw2d_Finish_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_GpuFinished>, <R_DRW2D_GpuCmdListCreate>, <R_DRW2D_GpuCmdListDelete>, 
    <R_DRW2D_GpuCmdListGenerate>, <R_DRW2D_GpuCmdListExec>, <R_DRW2D_GpuCmdListCopy>, 
    <R_DRW2D_GpuCmdListDelete>
*/
r_drw2d_Error_t R_DRW2D_GpuFinish(r_drw2d_Device_t Device,
                                  r_drw2d_Finish_t Block
                                  );



/*******************************************************************************
  Function: R_DRW2D_GpuFinished

  Queries the driver for a yes/no whether there are still pending jobs in its pipeline.

  Can be used for single-threaded non-blocking use cases.
  <R_DRW2D_GpuFinish> with <R_DRW2D_FINISH_NOWAIT_MARK> must be be called once before this 
  function.
  RetFinished = R_TRUE can only be checked once per <R_DRW2D_FINISH_NOWAIT_MARK>.

  Parameter:
    Device      - Device handle (see <r_drw2d_Device_t>)
    RetFinished - Result whether the GPU is finished (yes/no)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_GpuFinish>
*/
r_drw2d_Error_t R_DRW2D_GpuFinished(r_drw2d_Device_t Device, r_drw2d_Boolean_t *RetFinished);


/*******************************************************************************
  Function: R_DRW2D_GpuCmdListCreate

  Allocate empty command list.

  The application must call <R_DRW2D_GpuCmdListDelete> to delete the command list when it is 
  no longer used.


  Parameter:
         Device - Device handle (see <r_drw2d_Device_t>)
     RetCmdList - Receives the allocated command list address (see <r_drw2d_GpuCmdList_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.
    

  See also:
    <R_DRW2D_GpuFinish>, <R_DRW2D_GpuCmdListDelete>, <R_DRW2D_GpuCmdListGenerate>, 
    <R_DRW2D_GpuCmdListExec>, <R_DRW2D_GpuCmdListCopy>, <R_DRW2D_GpuCmdListDelete>
*/
r_drw2d_Error_t R_DRW2D_GpuCmdListCreate(r_drw2d_Device_t      Device,
                                         r_drw2d_GpuCmdList_t *RetCmdList
                                         );



/*******************************************************************************
  Function: R_DRW2D_GpuCmdListGenerate

  Record command list by calling an application provided function that invokes render commands.

  Any previously recorded command list data will be discarded.

  The command list must have been created using <R_DRW2D_GpuCmdListCreate>.

  Please notice that not all API functions may be called in the callback functions. 
  Drawing API functions are supported.


  Parameter:
      Device - Device handle (see <r_drw2d_Device_t>)
     CmdList - Command list handle (see <r_drw2d_GpuCmdList_t>)
         Cbk - Callback function (see <r_drw2d_GpuCmdListCallback_t>)
    UserData - Arbitrary user data which will be passed to the callback function

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_GpuFinish>, <R_DRW2D_GpuCmdListCreate>, <R_DRW2D_GpuCmdListExec>, 
    <R_DRW2D_GpuCmdListCopy>, <R_DRW2D_GpuCmdListDelete>
*/
r_drw2d_Error_t R_DRW2D_GpuCmdListGenerate(r_drw2d_Device_t              Device,
                                           r_drw2d_GpuCmdList_t          CmdList,
                                           r_drw2d_GpuCmdListCallback_t  Cbk,
                                           void                         *UserData
                                           );



/*******************************************************************************
  Function: R_DRW2D_GpuCmdListExec

  Execute previously recorded command list.


  Parameter:
      Device - Device handle (see <r_drw2d_Device_t>)
     CmdList - Command list handle (see <r_drw2d_GpuCmdList_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_GpuFinish>, <R_DRW2D_GpuCmdListCreate>, <R_DRW2D_GpuCmdListDelete>, 
    <R_DRW2D_GpuCmdListGenerate>, <R_DRW2D_GpuCmdListCopy>, <R_DRW2D_GpuCmdListDelete>
*/
r_drw2d_Error_t R_DRW2D_GpuCmdListExec(r_drw2d_Device_t     Device,
                                       r_drw2d_GpuCmdList_t CmdList
                                       );



/*******************************************************************************
  Function: R_DRW2D_GpuCmdListCopy

  Copy command list data to memory area.

  If DestAddr is NULL, Size returns the required size (in bytes) and no command list data is 
  copied.

  If RelocBaseAddr is != NULL, relocate jump commands so that the command list can later be
  executed from the given address (e.g. in flash memory).

  The copied command list may not be re-recorded or deleted using <R_DRW2D_GpuCmdListDelete>.


  Parameter:
    Device          - Device handle (see <r_drw2d_Device_t>)
    CmdList         - Command list handle (see <r_drw2d_GpuCmdList_t>)
    DestAddr        - Where to copy the commandlist. NULL to query required size.
    Size            - If DestAddr is null, returns the required size. 
                      Otherwise this parameters determines the maximum number of bytes that 
                      DestAddr can hold.
    RelocBaseAddr   - If not null, specifies the start address from where the command list can be 
                      executed later on (e.g. a flash memory address).

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_GpuFinish>, <R_DRW2D_GpuCmdListCreate>, <R_DRW2D_GpuCmdListDelete>, 
    <R_DRW2D_GpuCmdListGenerate>, <R_DRW2D_GpuCmdListExec>, <R_DRW2D_GpuCmdListDelete>
*/
r_drw2d_Error_t R_DRW2D_GpuCmdListCopy(r_drw2d_Device_t      Device,
                                       r_drw2d_GpuCmdList_t  CmdList,
                                       void                 *DestAddr,
                                       uint32_t             *Size,
                                       void                 *RelocBaseAddr
                                       );



/*******************************************************************************
  Function: R_DRW2D_GpuCmdListDelete

  Delete command list.

  The command list must have been created using <R_DRW2D_GpuCmdListCreate>.


  Parameter:
      Device - Device handle (see <r_drw2d_Device_t>)
     CmdList - Command list handle (see <r_drw2d_GpuCmdList_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_GpuFinish>, <R_DRW2D_GpuCmdListCreate>, <R_DRW2D_GpuCmdListGenerate>, 
    <R_DRW2D_GpuCmdListExec>, <R_DRW2D_GpuCmdListCopy>
*/
r_drw2d_Error_t R_DRW2D_GpuCmdListDelete(r_drw2d_Device_t     Device,
                                         r_drw2d_GpuCmdList_t CmdList
                                         );





/*******************************************************************************
  Group: Performance counter functions
*/

/*******************************************************************************
  Function: R_DRW2D_PerfCountersAlloc

  Allocate hardware performance counter resources for this device context.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <r_drw2d_Performance_t>, <R_DRW2D_PerfCountersFree>, <R_DRW2D_PerfValueGet>, 
    <R_DRW2D_PerfValueReset>
*/
r_drw2d_Error_t R_DRW2D_PerfCountersAlloc(r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_PerfCountersFree

  Free hardware performance counter resources for this device context.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <r_drw2d_Performance_t>, <R_DRW2D_PerfCountersAlloc>, <R_DRW2D_PerfValueGet>, 
    <R_DRW2D_PerfValueReset>
*/
r_drw2d_Error_t R_DRW2D_PerfCountersFree(r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_PerfValueGet

  Query the driver for HW cycles of type Type and return the value in RetValue.

  The cycle counts are reset when <R_DRW2D_PerfValueReset> is called.

  <R_DRW2D_PerfCountersAlloc> must have been called to allocate the hardware performance 
  counters for the DRW2D device context.


  Parameter:
       Device - Device handle (see <r_drw2d_Device_t>)
         Type - Performance type to query (see <r_drw2d_Performance_t>)
     RetValue - Cycle count is stored here (must not be NULL).

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <r_drw2d_Performance_t>, <R_DRW2D_PerfValueReset>
*/
r_drw2d_Error_t R_DRW2D_PerfValueGet(r_drw2d_Device_t      Device,
                                     r_drw2d_Performance_t Type,
                                     uint32_t *            RetValue
                                     );



/*******************************************************************************
  Function: R_DRW2D_PerfValueReset

  Reset the HW cycles of the given performance type to 0.


  Parameter:
     Device - Device handle (see <r_drw2d_Device_t>)
       Type - Performance type to query (see <r_drw2d_Performance_t>)

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <R_DRW2D_PerfValueGet>
*/
r_drw2d_Error_t R_DRW2D_PerfValueReset(r_drw2d_Device_t      Device,
                                       r_drw2d_Performance_t Type
                                       );





/*******************************************************************************
  Group: Error handling functions

  If the driver detects any fatal error, it will call the driver internal error handler function
  set by <R_DRW2D_ErrCallbackSet>.

  The error handler itself is not part of the API and must not be called by an application.
*/

/*******************************************************************************
  Function: R_DRW2D_ErrCallbackSet

  Install a device context / thread specific application error handler for the driver.

  If ErrorCb is zero, no callback function will be used.

  If the application has set an error handler callback function, the central error handler will 
  call it and then return to its caller.

  To uninstall a device context / thread specific error handler, ErrorCb should be set to NULL.


  Parameter:
      Device - Device handle (see <r_drw2d_Device_t>)
     ErrorCb - Reference to error handler callback function (see <r_drw2d_ErrorCallback_t>)
    UserData - Arbitrary user data that is passed on to the error callback function

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <r_drw2d_Error_t>, <r_drw2d_ErrorCallback_t>
*/
r_drw2d_Error_t R_DRW2D_ErrCallbackSet(r_drw2d_Device_t        Device,
                                       r_drw2d_ErrorCallback_t ErrorCb,
                                       void *                  UserData
                                       );



/*******************************************************************************
  Function: R_DRW2D_GlobalErrCallbackSet

  Install a global error handler for the driver.

  If GlobalErrorCb is zero, no callback function will be used.

  If a valid device context (<r_drw2d_Device_t>) is available when an error occurs,
  and the application has set an error callback for that device context, the device context
  error handler will have precedence over the global error handler.

  If no valid device context is available, only the global error handler will be called.

  If the application has set an error handler callback function, the central error handler will 
  call it and then return to its caller.

  To uninstall the global error handler, GlobalErrorCb should be set to NULL.


  Parameter:
            Device - Device handle (see <r_drw2d_Device_t>)
     GlobalErrorCb - Reference to error handler callback function.
                     (see <r_drw2d_GlobalErrorCallback_t>).
          UserData - Arbitrary user data that is passed on to the error callback function

  Return value:
    Errorcode (<R_DRW2D_ERR_OK> if successful). See <r_drw2d_Error_t> for the list of error codes.

  See also:
    <r_drw2d_Error_t>, <r_drw2d_GlobalErrorCallback_t>, <R_DRW2D_ErrCallbackSet>, 
    <r_drw2d_ErrorCallback_t>
*/
r_drw2d_Error_t R_DRW2D_GlobalErrCallbackSet(r_drw2d_GlobalErrorCallback_t GlobalErrorCb,
                                             void *                        UserData
                                             );





/*******************************************************************************
  Group: Math Utility Functions

*/

/*******************************************************************************
  Function: R_DRW2D_FixMul

  Multiply two fixed point values.
  
  Parameter:
    A - Fixed point number of value "A".
    B - Fixed point number of value "B".

  Return value:
    Multiplication result of fixed point by value "A" and value "B".

*/
R_INLINE r_drw2d_FixedP_t R_DRW2D_FixMul(r_drw2d_FixedP_t A, r_drw2d_FixedP_t B);

R_INLINE r_drw2d_FixedP_t R_DRW2D_FixMul(r_drw2d_FixedP_t A, r_drw2d_FixedP_t B)
{
    union { uint32_t lh[2]; int64_t ll;} c;
    c.ll = A;
    c.ll *= B;
    return (c.lh[0] >> 16) | (c.lh[1] << 16);
}



/*******************************************************************************
  Function: R_DRW2D_FixDiv

  Divide fixed point value "a" by value "b".

  Parameter:
    A - Fixed point number of value "A".
    B - Fixed point number of value "B".

  Return value:
    Division result of fixed point value "A" by value "B".
*/
R_INLINE r_drw2d_FixedP_t R_DRW2D_FixDiv(r_drw2d_FixedP_t A, r_drw2d_FixedP_t B);

R_INLINE r_drw2d_FixedP_t R_DRW2D_FixDiv(r_drw2d_FixedP_t A, r_drw2d_FixedP_t B)
{
#if defined(__ghs__)
#pragma ghs startnomisra
#endif
    int64_t a64 = A;
    /* PRQA S 4533 1 *//* $Misra: There is no problem because it is processed in the area expanded to 64 bits. $*/
    return (r_drw2d_FixedP_t) ((a64 << 16) / B);
#if defined(__ghs__)
#pragma ghs endnomisra
#endif
}



/*******************************************************************************
  Function: R_DRW2D_FixAbs

  Returns the absolute value of parameter A. 
  Not defined for A = 0x80000000 = R_DRW2D_2X(-32768).

  Parameter:
    A - Fixed point number of value "A".

  Return value:
    Absolute value of value "A".
*/
R_INLINE r_drw2d_FixedP_t R_DRW2D_FixAbs(r_drw2d_FixedP_t A);

R_INLINE r_drw2d_FixedP_t R_DRW2D_FixAbs(r_drw2d_FixedP_t A)
{
    uint32_t result;

    if(A < 0)
    {
        result = ~((uint32_t) A);
        result++;
    }
    else
    {
        result = (uint32_t) A;
    }

    return (int32_t) result;
}



/*******************************************************************************
  Function: R_DRW2D_FixSin

  Calculate sine function for Angle
  
  Example
    * Angle = R_DRW2D_2X(0.0) - 0-degree
    * Angle = R_DRW2D_2X(0.2) - 18-degree
    * Angle = R_DRW2D_2X(0.5) - 45-degree
    * Angle = R_DRW2D_2X(1.0) - 90-degree
    * Angle = R_DRW2D_2X(2.0) - 180-degree
    * Angle = R_DRW2D_2X(4.0) - 360-degree

  Parameter:
    Angle - Fixed point number of Angle. It is a value when 90-degree is 1.0.

  Return value:
    Result of sin(Angle)
*/
r_drw2d_FixedP_t R_DRW2D_FixSin(r_drw2d_FixedP_t Angle);



/*******************************************************************************
  Function: R_DRW2D_FixCos

  Calculate cosine function for Angle

  Parameter:
    Angle - Fixed point number of Angle. It is a value when 90-degree is 1.0.

  Return value:
    Result of cos(Angle)
*/
r_drw2d_FixedP_t R_DRW2D_FixCos(r_drw2d_FixedP_t Angle);



/*******************************************************************************
  Function: R_DRW2D_FixTan

  Calculate tangent function for Angle

  Parameter:
    Angle - Fixed point number of Angle. It is a value when 90-degree is 1.0.

  Return value:
    tan(Angle)
*/
r_drw2d_FixedP_t R_DRW2D_FixTan(r_drw2d_FixedP_t Angle);



/*******************************************************************************
  Function: R_DRW2D_FixSqrt

  Calculate square root of fixed point value

  Parameter:
    FixValue - Fixed point value

  Return value:
    Result of sqrt(Value)
*/
r_drw2d_FixedP_t R_DRW2D_FixSqrt(r_drw2d_FixedP_t FixValue);


#ifdef __cplusplus
}
#endif

#endif /* R_DRW2D_API_H_ */


